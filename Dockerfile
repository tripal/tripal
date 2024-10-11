ARG phpversion='8.3'
ARG drupalversion='10.3.x-dev'
ARG postgresqlversion='16'
FROM tripalproject/tripaldocker-drupal:drupal${drupalversion}-php${phpversion}-pgsql${postgresqlversion}

ARG modules='devel devel_php field_group field_group_table'
ARG tripalmodules='tripal tripal_biodb tripal_chado tripal_layout'
ARG chadoschema='chado'
ARG installchado=TRUE
# see issue #2000 for the reason for this argument:
ARG COMPOSER_RUNTIME_BIN_DIR=/var/www/drupal/vendor/bin

# Label docker image
LABEL tripal.version="4.x-dev"
LABEL tripal.stability="development"

COPY . /app

############# Tripal ##########################################################

RUN service apache2 start \
  && service postgresql start \
  && mkdir -p /var/www/drupal/web/modules/contrib \
  && cp -R /app /var/www/drupal/web/modules/contrib/tripal \
  && drush en ${tripalmodules} ${modules} -y \
  && service apache2 stop \
  && service postgresql stop

RUN service apache2 start \
  && service postgresql start \
  && if [ "$installchado" = "TRUE" ]; then \
  drush trp-install-chado --schema-name=${chadoschema} \
  && drush trp-prep-chado --schema-name=${chadoschema}; \
  fi \
  && service apache2 stop \
  && service postgresql stop

RUN service apache2 start \
  && service postgresql start \
  && if [ "$installchado" = "TRUE" ]; then \
  drush trp-import-types --collection_id=general_chado --username=drupaladmin; \
  fi \
  && service apache2 stop \
  && service postgresql stop
