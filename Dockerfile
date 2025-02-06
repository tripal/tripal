ARG phpversion='8.3'
ARG drupalversion='11.0.x-dev'
ARG postgresqlversion='17'
FROM tripalproject/tripaldocker-drupal:drupal${drupalversion}-php${phpversion}-pgsql${postgresqlversion}

## Redefine the core args so that they are within the build scope.
ARG phpversion='8.3'
ARG drupalversion='11.0.x-dev'
ARG postgresqlversion='17'

## Now define the args only needed within the build scope.
ARG modules='devel devel_php field_group field_group_table'
ARG tripalmodules='tripal tripal_biodb tripal_chado tripal_layout'
ARG chadoschema='chado'
ARG installchado=TRUE
# see issue #2000 for the reason for updating the PATH:
ENV PATH="/var/www/drupal/vendor/drush/drush:$PATH"

# Label docker image
LABEL tripal.version="4.x-dev"
LABEL tripal.stability="development"

COPY . /tripal_app

############# Tripal ##########################################################

WORKDIR /var/www/drupal

RUN service apache2 start \
  && service postgresql start \
  && mkdir -p /var/www/drupal/web/modules/contrib \
  && cp -R /tripal_app /var/www/drupal/web/modules/contrib/tripal \
  && rm -rf /app /tripal_app \
  && allmodules="${tripalmodules} ${modules}" \
  && vendor/bin/drush en ${allmodules} -y \
  && service apache2 stop \
  && service postgresql stop

RUN service apache2 start \
  && service postgresql start \
  && if [ "$installchado" = "TRUE" ]; then \
  vendor/bin/drush trp-install-chado --schema-name=${chadoschema} \
  && vendor/bin/drush trp-prep-chado --schema-name=${chadoschema}; \
  fi \
  && service apache2 stop \
  && service postgresql stop

RUN service apache2 start \
  && service postgresql start \
  && if [ "$installchado" = "TRUE" ]; then \
  vendor/bin/drush trp-import-types --collection_id=general_chado --username=drupaladmin; \
  fi \
  && service apache2 stop \
  && service postgresql stop
