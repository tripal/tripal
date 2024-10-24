ARG phpversion='8.3'
FROM php:${phpversion}-apache-bullseye

ARG drupalversion='~10.4.0'
ARG postgresqlversion='16'
ARG modules='devel devel_php field_group field_group_table'
ARG chadoschema='chado'
ARG installchado=TRUE
ARG phpuploadsize=8M

# Label docker image
LABEL drupal.version=${drupalversion}
LABEL drupal.stability="production"
LABEL tripal.version="4.x-dev"
LABEL tripal.stability="development"
LABEL os.version="bullseye"
LABEL postgresql.version="${postgresqlversion}"

COPY . /app
COPY tripaldocker/init_scripts/motd /etc/motd

## Install some basic support programs and update apt-get.
RUN chmod -R +x /app && apt-get update 1> ~/aptget.update.log \
  && apt-get install git unzip zip wget gnupg2 supervisor vim --yes -qq 1> ~/aptget.extras.log

########## POSTGRESQL #########################################################

## See https://stackoverflow.com/questions/51033689/how-to-fix-error-on-postgres-install-ubuntu
RUN mkdir -p /usr/share/man/man1 && mkdir -p /usr/share/man/man7

## Add the PostgreSQL Package Source for versions > 13
RUN if [ "$postgresqlversion" > "13" ] ; then \
  apt-get install -y curl apt-transport-https gpg --yes -qq 1>> ~/aptget.extras.log \
  && curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor -o /usr/share/keyrings/postgresql-keyring.gpg \
  && echo "deb [signed-by=/usr/share/keyrings/postgresql-keyring.gpg] http://apt.postgresql.org/pub/repos/apt/ bullseye-pgdg main" > /etc/apt/sources.list.d/postgresql.list \
  && apt-get update 1>> ~/aptget.update.log ; \
  fi

## Install PostgreSQL version ${postgresqlversion}
RUN DEBIAN_FRONTEND=noninteractive apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y postgresql-${postgresqlversion} postgresql-client-${postgresqlversion} postgresql-contrib-${postgresqlversion}

## Run the rest of the commands as the ``postgres`` user
## created by the ``postgres-${postgresqlversion}`` package when it was installed.
USER postgres

## Create a PostgreSQL role named ``docker`` with ``docker`` as the password and
## then create a database `docker` owned by the ``docker`` role.
RUN    /etc/init.d/postgresql start &&\
  psql --command "CREATE USER docker WITH SUPERUSER PASSWORD 'docker';"  \
  && createdb -O docker docker \
  && psql --command="CREATE USER drupaladmin WITH PASSWORD 'drupaldevelopmentonlylocal'" \
  && psql --command="ALTER USER drupaladmin WITH LOGIN" \
  && psql --command="ALTER USER drupaladmin WITH CREATEDB" \
  && psql --command="CREATE DATABASE sitedb WITH OWNER drupaladmin" \
  && psql sitedb --command="CREATE EXTENSION pg_trgm" \
  && service postgresql stop

## Now back to the root user.
USER root

## Adjust PostgreSQL configuration so that remote connections to the
## database are possible.
RUN mv /app/tripaldocker/default_files/postgresql/pg_hba.conf /etc/postgresql/${postgresqlversion}/main/pg_hba.conf

## And add ``listen_addresses`` to ``/etc/postgresql/${postgresqlversion}/main/postgresql.conf``
RUN echo "listen_addresses='*'" >> /etc/postgresql/${postgresqlversion}/main/postgresql.conf \
  && echo "max_locks_per_transaction = 1024" >> /etc/postgresql/${postgresqlversion}/main/postgresql.conf

########## PHP EXTENSIONS #####################################################
RUN mv "$PHP_INI_DIR/php.ini-development" "$PHP_INI_DIR/php.ini"

## Xdebug
RUN pecl install xdebug-3.3.2 \
  && docker-php-ext-enable xdebug \
  && cat /app/tripaldocker/default_files/xdebug/xdebug-coverage.ini >> /usr/local/etc/php/php.ini \
  && echo "error_reporting=E_ALL" >> /usr/local/etc/php/conf.d/error_reporting.ini \
  && cp /app/tripaldocker/default_files/xdebug/xdebug.ini /usr/local/etc/php/conf.d/docker-php-ext-xdebug.dis \
  && rm /usr/local/etc/php/conf.d/docker-php-ext-xdebug.ini

## install the PHP extensions we need
RUN set -eux; \
  \
  if command -v a2enmod; then \
  a2enmod rewrite; \
  fi; \
  \
  savedAptMark="$(apt-mark showmanual)"; \
  \
  apt-get update; \
  apt-get install -y --no-install-recommends \
  libfreetype6-dev \
  libjpeg-dev \
  libpng-dev \
  libwebp-dev \
  libpq-dev \
  libzip-dev \
  ; \
  \
  docker-php-ext-configure gd --enable-gd --with-freetype --with-jpeg --with-webp; \
  \
  docker-php-ext-install -j "$(nproc)" \
  gd \
  opcache \
  pdo_mysql \
  pdo_pgsql \
  pgsql \
  zip \
  ; \
  \
  # reset apt-mark's "manual" list so that "purge --auto-remove" will remove all build dependencies
  apt-mark auto '.*' > /dev/null; \
  apt-mark manual $savedAptMark; \
  ldd "$(php -r 'echo ini_get("extension_dir");')"/*.so \
  | awk '/=>/ { print $3 }' \
  | sort -u \
  | xargs -r dpkg-query -S \
  | cut -d: -f1 \
  | sort -u \
  | xargs -rt apt-mark manual; \
  \
  apt-get purge -y --auto-remove -o APT::AutoRemove::RecommendsImportant=false; \
  rm -rf /var/lib/apt/lists/*

## set recommended PHP.ini settings
## see https://secure.php.net/manual/en/opcache.installation.php
RUN { \
  echo 'opcache.memory_consumption=128'; \
  echo 'opcache.interned_strings_buffer=8'; \
  echo 'opcache.max_accelerated_files=4000'; \
  echo 'opcache.revalidate_freq=60'; \
  echo 'opcache.fast_shutdown=1'; \
  echo 'opcache.memory_limit=1028M';\
  } > /usr/local/etc/php/conf.d/opcache-recommended.ini

RUN echo 'memory_limit = 1028M' >> /usr/local/etc/php/conf.d/docker-php-memlimit.ini \
  && sed -i 's/upload_max_filesize = 2M/upload_max_filesize = '"$phpuploadsize"'/' /usr/local/etc/php/php.ini

WORKDIR /var/www/html

############# APACHE ##########################################################

# Fix Could not determine server's fully qualified domain name.
RUN echo "ServerName localhost" >> /etc/apache2/apache2.conf

############# DRUPAL ##########################################################

## Environment variables used for phpunit testing.
ENV SIMPLETEST_BASE_URL=http://localhost
ENV SIMPLETEST_DB=pgsql://drupaladmin:drupaldevelopmentonlylocal@localhost/sitedb
ENV BROWSER_OUTPUT_DIRECTORY=/var/www/drupal/web/sites/default/files/simpletest
ENV COMPOSER_MEMORY_LIMIT=-1
ENV COMPOSER_NO_INTERACTION=1
ENV COMPOSER_ALLOW_SUPERUSER=1

## Install composer and Drush.
WORKDIR /var/www
RUN chmod a+x /app/tripaldocker/init_scripts/composer-init.sh \
  && /app/tripaldocker/init_scripts/composer-init.sh

## Use composer to install Drupal.
WORKDIR /var/www
ARG requiredcomposerpackages="drupal/core:${drupalversion} drupal/core-dev:${drupalversion} drush/drush phpspec/prophecy-phpunit"
ARG composerpackages="drupal/devel drupal/devel_php drupal/gin_toolbar drupal/gin"
RUN composer create-project drupal/recommended-project:${drupalversion} --stability dev --no-install drupal \
  && cd drupal \
  && composer config --no-plugins allow-plugins.composer/installers true \
  && composer config --no-plugins allow-plugins.drupal/core-composer-scaffold true \
  && composer config --no-plugins allow-plugins.drupal/core-project-message true \
  && composer config --no-plugins allow-plugins.dealerdirect/phpcodesniffer-composer-installer true \
  && rm composer.lock \
  && packages="${requiredcomposerpackages} ${composerpackages}" \
  && if $(dpkg --compare-versions "${drupalversion}" "lt" "10.6"); then packages="$packages drupal/field_group drupal/field_group_table"; fi \
  && composer require --dev $packages \
  && composer install

## Set files directory permissions
RUN mkdir /var/www/drupal/web/sites/default/files \
  && mkdir /var/www/drupal/web/sites/default/files/simpletest \
  && chown -R www-data:www-data /var/www/drupal \
  && chmod 02775 -R /var/www/drupal/web/sites/default/files \
  && usermod -g www-data root

## Install Drupal.
RUN cd /var/www/drupal \
  && service apache2 start \
  && service postgresql start \
  && sleep 30 \
  && /var/www/drupal/vendor/drush/drush/drush site-install standard \
  --db-url=pgsql://drupaladmin:drupaldevelopmentonlylocal@localhost/sitedb \
  --account-mail="drupaladmin@localhost" \
  --account-name=drupaladmin \
  --account-pass=some_admin_password \
  --site-mail="drupaladmin@localhost" \
  --site-name="Tripal 4.x-dev on Drupal ${drupalversion}" \
  && service apache2 stop \
  && service postgresql stop

## Handle Admin theme
RUN cd /var/www/drupal \
  && service apache2 start \
  && service postgresql start \
  && /var/www/drupal/vendor/drush/drush/drush pm:install gin_toolbar --yes \
  && /var/www/drupal/vendor/drush/drush/drush theme:enable gin --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set system.theme admin gin --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings enable_darkmode auto --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings preset_accent_color neutral --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings preset_focus_color dark --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings classic_toolbar new --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings secondary_toolbar_frontend 1 --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings layout_density small --yes \
  && /var/www/drupal/vendor/drush/drush/drush config-set gin.settings show_user_theme_settings 1 --yes \
  && service apache2 stop \
  && service postgresql stop

############# Scripts #########################################################

## Configuration files & Activation script
RUN mv /app/tripaldocker/init_scripts/supervisord.conf /etc/supervisord.conf \
  && mv /app/tripaldocker/default_files/000-default.conf /etc/apache2/sites-available/000-default.conf \
  && echo "\$settings['trusted_host_patterns'] = [ '^localhost$', '^127\.0\.0\.1$', \$_SERVER['SERVER_NAME'] ];" >> /var/www/drupal/web/sites/default/settings.php \
  && mv /app/tripaldocker/init_scripts/init.sh /usr/bin/init.sh \
  && chmod +x /usr/bin/init.sh \
  && mv /app/tripaldocker/default_files/xdebug/xdebug_toggle.sh /usr/bin/xdebug_toggle.sh \
  && echo "\$config['system.logging']['error_level'] = 'verbose';" >> /var/www/drupal/web/sites/default/settings.php

## Make global commands. Symlink for drupal9 is for backward compatibility.
RUN ln -s /var/www/drupal/vendor/phpunit/phpunit/phpunit /usr/local/bin/ \
  && ln -s /var/www/drupal/vendor/drush/drush/drush /usr/local/bin/ \
  && ln -s /var/www/drupal /var/www/drupal9

## Set the working directory to DRUPAL_ROOT
WORKDIR /var/www/drupal/web

## Expose http, xdebug and psql port
EXPOSE 80 5432 9003

ENTRYPOINT ["init.sh"]
