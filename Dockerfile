ARG phpversion='8.3'
ARG drupalversion='10.4.x-dev'
ARG postgresqlversion='16'
FROM tripalproject/tripaldocker-drupal:drupal${drupalversion}-php${phpversion}-pgsql${postgresqlversion}

## Redefine the core args so that they are within the build scope.
ARG phpversion='8.3'
ARG drupalversion='10.3.x-dev'
ARG postgresqlversion='16'

## Now define the args only needed within the build scope.
ARG modules='devel devel_php field_group'
ARG tripalmodules='tripal tripal_biodb tripal_chado'
## Redefine the core args so that they are within the build scope.
ARG phpversion='8.3'
ARG drupalversion='10.3.x-dev'
ARG postgresqlversion='16'

## Now define the args only needed within the build scope.
ARG modules='devel devel_php field_group'
ARG tripalmodules='tripal tripal_biodb tripal_chado'
ARG chadoschema='chado'
ARG installchado=TRUE
# see issue #2000 for the reason for updating the PATH:
ENV PATH="/var/www/drupal/vendor/drush/drush:$PATH"

# Label docker image
LABEL tripal.version="4.x-dev"
LABEL tripal.stability="development"

COPY . /app

############# Tripal ##########################################################

WORKDIR /var/www/drupal

RUN service apache2 start \
  && service postgresql start \
  && mkdir -p /var/www/drupal/web/modules/contrib \
  && cp -R /app /var/www/drupal/web/modules/contrib/tripal \
  && allmodules="${tripalmodules} ${modules}" \
  && if $(dpkg --compare-versions "${drupalversion}" "lt" "10.6"); then \
  allmodules="$allmodules field_group_table"; \
  fi \
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
