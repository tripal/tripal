FROM php:7.3-apache-buster

## Base of this image is from Official DockerHub PHP image.
## Heavily influenced by https://github.com/statonlab/docker-containers

MAINTAINER Lacey-Anne Sanderson <laceyannesanderson@gmail.com>

COPY . /app

## Install some basic support programs and update apt-get.
RUN chmod -R +x /app && apt-get update 1> ~/aptget.update.log \
  && apt-get install git unzip zip wget gnupg2 supervisor vim --yes -qq 1> ~/aptget.extras.log

########## POSTGRESQL #########################################################

## See https://stackoverflow.com/questions/51033689/how-to-fix-error-on-postgres-install-ubuntu
RUN mkdir -p /usr/share/man/man1 && mkdir -p /usr/share/man/man7

## Install PostgreSQL 11
RUN DEBIAN_FRONTEND=noninteractive apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y postgresql-11 postgresql-client-11 postgresql-contrib-11

## Run the rest of the commands as the ``postgres`` user
## created by the ``postgres-11`` package when it was installed.
USER postgres

## Create a PostgreSQL role named ``docker`` with ``docker`` as the password and
## then create a database `docker` owned by the ``docker`` role.
RUN    /etc/init.d/postgresql start &&\
    psql --command "CREATE USER docker WITH SUPERUSER PASSWORD 'docker';" &&\
    createdb -O docker docker \
    && psql --command="CREATE USER drupaladmin WITH PASSWORD 'drupal8developmentonlylocal'" \
    && psql --command="ALTER USER drupaladmin WITH LOGIN" \
    && psql --command="ALTER USER drupaladmin WITH CREATEDB" \
    && psql --command="CREATE DATABASE sitedb WITH OWNER drupaladmin"

## Now back to the root user.
USER root

## Adjust PostgreSQL configuration so that remote connections to the
## database are possible.
RUN mv /app/default_files/postgresql/pg_hba.conf /etc/postgresql/11/main/pg_hba.conf

## And add ``listen_addresses`` to ``/etc/postgresql/11/main/postgresql.conf``
RUN echo "listen_addresses='*'" >> /etc/postgresql/11/main/postgresql.conf \
  && echo "max_locks_per_transaction = 1024" >> /etc/postgresql/11/main/postgresql.conf

########## PHP EXTENSIONS #####################################################
RUN mv "$PHP_INI_DIR/php.ini-development" "$PHP_INI_DIR/php.ini"
## Xdebug
RUN pecl install xdebug-3.0.1 \
    && docker-php-ext-enable xdebug \
    && echo "xdebug.mode = coverage" >> /usr/local/etc/php/php.ini
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
		libpq-dev \
		libzip-dev \
	; \
	\
	docker-php-ext-configure gd \
		--with-freetype-dir=/usr \
		--with-jpeg-dir=/usr \
		--with-png-dir=/usr \
	; \
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

RUN echo 'memory_limit = 1028M' >> /usr/local/etc/php/conf.d/docker-php-memlimit.ini

WORKDIR /var/www/html

############# DRUPAL ##########################################################

## Environment variables used for phpunit testing.
ENV SIMPLETEST_BASE_URL=http://localhost
ENV SIMPLETEST_DB=pgsql://drupaladmin:drupal8developmentonlylocal@localhost/sitedb
ENV BROWSER_OUTPUT_DIRECTORY=/var/www/drupal8/web/sites/default/files/simpletest

## Install composer and Drush.
WORKDIR /var/www
RUN chmod a+x /app/init_scripts/composer-init.sh \
  && /app/init_scripts/composer-init.sh \
  && vendor/bin/drush --version

## Use composer to install Drupal.
WORKDIR /var/www
RUN export COMPOSER_MEMORY_LIMIT=-1 \
  && composer create-project drupal-composer/drupal-project:8.x-dev drupal8 --stability dev --no-interaction

## Set files directory permissions
RUN mkdir /var/www/drupal8/web/sites/default/files/simpletest \
  && chown -R www-data:www-data /var/www/drupal8 \
  && chmod 02775 -R /var/www/drupal8/web/sites/default/files \
  && usermod -g www-data root

## Install Drupal.
RUN cd /var/www/drupal8 \
  && service apache2 restart \
  && service postgresql restart \
  && sleep 30 \
  && /var/www/drupal8/vendor/drush/drush/drush site-install standard \
  --db-url=pgsql://drupaladmin:drupal8developmentonlylocal@localhost/sitedb \
  --account-mail="drupaladmin@localhost" \
  --account-name=drupaladmin \
  --account-pass=some_admin_password \
  --site-mail="drupaladmin@localhost" \
  --site-name="Drupal 8 Development"

############# Tripal ##########################################################

WORKDIR /var/www/drupal8
RUN service apache2 restart \
  && service postgresql restart \
  && sleep 30 \
  && composer require tripal/tripal \
  && composer require drupal/devel \
  && vendor/bin/drush en devel tripal tripal_chado -y \
  && vendor/bin/drush trp-install-chado

############# Scripts #########################################################

## Configuration files & Activation script
RUN mv /app/init_scripts/supervisord.conf /etc/supervisord.conf \
  && mv /app/default_files/000-default.conf /etc/apache2/sites-available/000-default.conf \
  && echo "\$settings["trusted_host_patterns"] = [ '^localhost$', '^127\.0\.0\.1$' ];" >> /var/www/drupal8/web/sites/default/settings.php \
  && mv /app/init_scripts/init.sh /usr/bin/init.sh \
  && chmod +x /usr/bin/init.sh

## Make global commands.
RUN ln -s /var/www/drupal8/vendor/drupal/console/bin/drupal /usr/local/bin/ \
  && ln -s /var/www/drupal8/vendor/phpunit/phpunit/phpunit /usr/local/bin/ \
  && ln -s /var/www/drupal8/vendor/drush/drush/drush /usr/local/bin/

## Set the working directory to DRUPAL_ROOT
WORKDIR /var/www/drupal8/web

## Expose http and psql port
EXPOSE 80 5432

ENTRYPOINT ["init.sh"]
