FROM php:7.2-apache

USER root

# Update the system and add any missing repos
# install the PHP extensions we need
RUN set -ex; \
	\
	if command -v a2enmod; then \
		a2enmod rewrite; \
	fi; \
	\
	savedAptMark="$(apt-mark showmanual)"; \
	\
	apt-get update; \
	apt-get install -y --no-install-recommends \
		libjpeg-dev \
		libpng-dev \
		libpq-dev \
	; \
	\
	docker-php-ext-configure gd --with-png-dir=/usr --with-jpeg-dir=/usr; \
	docker-php-ext-install -j "$(nproc)" \
		gd \
		opcache \
		pdo_mysql \
		pdo_pgsql \
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

# set recommended PHP.ini settings
# see https://secure.php.net/manual/en/opcache.installation.php
RUN { \
		echo 'opcache.memory_consumption=128'; \
		echo 'opcache.interned_strings_buffer=8'; \
		echo 'opcache.max_accelerated_files=4000'; \
		echo 'opcache.revalidate_freq=60'; \
		echo 'opcache.fast_shutdown=1'; \
		echo 'opcache.enable_cli=1'; \
	} > /usr/local/etc/php/conf.d/opcache-recommended.ini

# Install services
# (Apache2, php7.1, posgtes, curl, git, zip, unzip, supervisor ... etc)
RUN set -x \
 	&& yum install -y wget curl git zip unzip httpd crontabs \

# Database and other services
RUN set -x \
	&& yum install -y https://download.postgresql.org/pub/repos/yum/9.6/redhat/rhel-7-x86_64/pgdg-centos96-9.6-3.noarch.rpm \
	&& yum install -y postgresql96-server postgresql96-contrib \
    && yum install -y yum-utils \
    && yum-config-manager --enable remi-php71 \
    && yum update -y \
    && yum install -y mod_php php-mcrypt php-mbstring php-json php-posix php-pgsql \
      				  php-pdo php-xml php-curl php-gd supervisor sudo \
      				  patch which sendmail

WORKDIR /var/www/html

# https://www.drupal.org/node/3060/release
ENV DRUPAL_VERSION 8.6.15
ENV DRUPAL_MD5 85ae6b9f7309cc8564331fd77369dffd

# Install composer and drush
COPY install-composer.sh /install-composer.sh
RUN chmod +x /install-composer.sh \
	&& /install-composer.sh \
	&& rm /install-composer.sh \
	&& mv composer.phar /usr/local/bin/composer \
	&& composer global require drush/drush:8.*

ENV PATH "/root/.composer/vendor/bin:/usr/pgsql-9.6/bin:$PATH"
ENV PGDATA "/var/lib/pgsql/9.6/data"

# Prepare the Database
RUN set -x \
	&& sudo -u postgres /usr/pgsql-9.6/bin/initdb -D /var/lib/pgsql/9.6/data \
	&& touch /var/log/pgsql.log \
	&& chown postgres:postgres /var/log/pgsql.log \
	&& sudo -u postgres /usr/pgsql-9.6/bin/pg_ctl -D /var/lib/pgsql/9.6/data -l /var/log/pgsql.log start \
	&& sleep 30 \
	# Centos postgres needs to accept UTF8 encoding
	&& sudo -u postgres /usr/pgsql-9.6/bin/psql -c "update pg_database set datistemplate=false where datname='template1';" \
	&& sudo -u postgres /usr/pgsql-9.6/bin/psql -c "drop database Template1;" \
    &&  sudo -u postgres /usr/pgsql-9.6/bin/psql -c "create database template1 with owner=postgres encoding='UTF-8' \
									 lc_collate='en_US.utf8' lc_ctype='en_US.utf8' template template0;" \
 	&& sudo -u postgres /usr/pgsql-9.6/bin/psql -c "update pg_database set datistemplate=true where datname='template1';" \
	&& sudo -u postgres /usr/pgsql-9.6/bin/createuser drupal \
	&& sudo -u postgres /usr/pgsql-9.6/bin/createdb -O drupal -E 'utf8' drupal \
	&& sudo -u postgres /usr/pgsql-9.6/bin/psql -c "ALTER USER drupal WITH password 'secret'" \
	# Download drupal and other modules
	&& curl -fSL "https://ftp.drupal.org/files/projects/drupal-${DRUPAL_VERSION}.tar.gz" -o drupal.tar.gz \
	&& echo "${DRUPAL_MD5} *drupal.tar.gz" | md5sum -c - \
	&& tar -xz --strip-components=1 -f drupal.tar.gz \
	&& rm drupal.tar.gz \
	&& chown -R www-data:www-data sites modules themes
	# Install Drupal and enable tripal dependencies
	&& cd /var/www/html \
	&& drush si --db-url=pgsql://drupal:secret@127.0.0.1/drupal --account-pass=secret -y \

  # Finally update drupal to the latest version
  && drush pm-update drupal -y

# Add contrib and custom modules dicrectory
RUN ln -s /var/www/html/modules/contrib /modules
RUN ln -s /var/www/html/modules/custom /modules

# Set files directory permissions
RUN chown -R apache:apache /var/www/html/sites/default/files

# Expose http and psql port
EXPOSE 80 5432

# Configuration files
COPY supervisord.conf /etc/supervisord.d/app.ini

# Activation scripts
COPY init.sh /usr/bin/init.sh
RUN chmod +x /usr/bin/init.sh

ENTRYPOINT ["init.sh"]