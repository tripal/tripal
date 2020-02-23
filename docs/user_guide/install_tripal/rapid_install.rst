Installation Method #1: Rapid Installation
==========================================

.. note::

  Remember you must set the ``$DRUPAL_HOME`` environment variable if you want to cut-and-paste the commands below. See :doc:`./drupal_home`


Before installing via the rapid installation process please ensure drush is installed, and the server is setup.   Rapid Installation works with Tripal v3.0-rc2 (release candidate 2) and later.  If you are using a previous version of Tripal, please proceed to the step-by-step instructions.

Database Setup
---------------

Before we can install Tripal we must have a database ready for it.  In the server setup instructions were provided to set up a PostgreSQL database server. Now, we need to create the Drupal database. To do so we must first become the PostgreSQL user.

.. code-block:: bash

  sudo su - postgres

Next, create the new 'drupal' user account. This account will not be a "superuser" nor allowed to create new roles, but should be allowed to create a database.

.. code-block:: bash

  createuser -P drupal

When requested, enter an appropriate password. Finally, create the new database:

.. code-block:: bash

  createdb drupal -O drupal

We no longer need to be the postgres user so exit

.. code-block:: bash

  exit

Tripal Installation
-------------------

Navigate to your Drupal install directory.

.. code-block:: bash

  cd $DRUPAL_HOME

.. note::

  Make sure you have write permissions within this directory.

Clone the tripal_install project using the ``git`` command and move the contents up one level into the web document directory:

.. code-block:: bash

  git clone https://github.com/tripal/tripal_install.git
  mv tripal_install/* ./

Begin the installation for a generic installation with the following command:

.. code-block:: bash

  drush --include=. tripal-generic-install

From this point onward, you will be asked a series of questions in the terminal window.  First you will be asked for the name of the site (this will appear at the top of your site after creation), the site administrator's email address, a username for the administrator to log on, and the password for the administrator:

::

  Name of the site : Tripal
  Admin email for the site : admin@gmail.com
  Name for your admin user on the site : admin
  Password for the admin user, needs to be complex including numbers and characters, example P@55w0rd: P@55w0rd

  These are the site settings provided, please review and confirm they are correct
     Site Name: Tripal
     Site email address: admin@gmail.com
     Administrator username: admin
     Administrator password: P@55w0rd
  Is this information correct? (y/n): y

Next, you will be asked for the database information: database name, database  username, database  user password, host, and port.  The database name and user should match what you created in the previous section (i.e. database name = 'drupal' and database user = 'drupal').  The 'host' is the name of the server or its IP address, and the port is a numerical value that PostgreSQL uses for communication.  By default PostgreSQL uses the port 5432.  If a mistake is made you can make corrections as shown below:

::

  Now we need to setup Drupal to connect to the database you want to use. These settings are added to Drupal‘s settings.php file.

  database name: database
  postgres username: drupal
  postgres password: drupal
  host, like localhost or 127.0.0.1: 127.0.01
  port, usually S432: 5432
  This is the database information provided, please review and confirm it is correct:
  Database name: database
  Database username: drupal
  Database user password: drupal
  Database host: 127.0.01
  Database port: 5432
  Is this information correct? (Y/n): n

  Now we need to setup Drupal to connect to the database you want to use. These settings are added to Drupal‘s settings.php file.

  database name: database
  postgres username: drupal
  postgres password: drupal
  host, like localhost or 127.0.0.1: 127.0.0.1
  port, usually S432: 5432
  This is the database information provided, please review and confirm it is correct:
  Database name: database
  Database username: drupal
  Database user password: drupal
  Database host: 127.0.0.1
  Database port: 5432
  Is this information correct? (Y/n): y



After site information and database credentials are provided, Drupal will be installed.  You will see this in the terminal:

::

  Now installing Drupal.

  --2017-09-20 12:29:16-- https://www.drupal.org/files/projects/drupal-7.56.tar.gz

  Resolving www.drupal.org (www.drupal.org)... 151.101.5.175
  Connecting to www.drupal.org (www.drupal.org)|151.101.5.175|:443... connected.
  HTTP request sent, awaiting response... 200 OK

  Length: 3277833 (3.1M) [application/x-gzip]
  Saving to: ‘drupal-7.56.tar.gz'

  drupal-7.56.tar.gz 100%[::::::::::::::::::::::::::::::::::::::::::::::::>] 3.13M 1.82MB/s in 1.75

  2017-09-20 12:29:20 (1.82 MB/S) - ‘drupal-7.56.tar.gz' saved [3277833/3277833]

  You are about to create a /var/www/html/sites/default/settings.php file and DROP all tables in your ‘database‘ database. Do you want to continue? (y/n): y

  Starting Drupal installation. This takes a while. Consider using the --notify global option.
  Installation complete. User name: admin User password: P@55word


Next, the required modules will be downloaded:

::

  Downloading modules.

  Project field_group (7.x-1.5) downloaded to /var/www/html/sites/all/modules/field_group.
  Project field_group_table (7.x-1.6) downloaded to /var/www/html/sites/all/modules/field_group_table.
  Project field_formatter_class (7.x-1.1) downloaded to /var/www/html/sites/all/modules/field_formatter_class.
  Project field_formatter_settings (7.x-1.1) downloaded to /var/www/html/sites/all/modules/field_formatter_settings.
  Project ctools (7.x-1.12) downloaded to /var/www/html/sites/all/modules/ctools. [success]
  Project ctools contains 10 modules: ctools_custom_content, stylizer, ctools_plugin_example, views_content, ctools_ajax_sample, term_depth, ctools_access_ruleset, page_manager, bulk_export, ctools.
  Project date (7.x-2.10) downloaded to /var/www/html/sites/all/modules/date.
  Project date contains 11 modules: date_context, date_migrate_example, date_migrate, date_popup, date_tool
  repeat, date_views, date_all_day, date_api, date_repeat_field, date.
  Project devel (7.x-1.5) downloaded to /var/www/html/sites/all/modules/devel.
  Project devel contains 3 modules: devel_generate, devel, devel_node_access.
  Project ds (7.x-2.14) downloaded to /var/www/html/sites/all/modules/ds.
  Project ds contains 7 modules: ds_forms, ds_ui, ds_devel, ds_format, ds_extras, ds_search, ds.
  Project link (7.x-1.4) downloaded to /var/www/html/sites/all/modules/link.
  Project entity (7.x-1.8) downloaded to /var/www/html/sites/all/modules/entity.
  Project entity contains 2 modules: entity_token, entity.
  Project libraries (7.x-2.3) downloaded to /var/www/html/sites/all/modules/libraries.
  redirect (7.x-1.0-rc3) downloaded to /var/www/html/sites/all/modules/redirect.
  Project token (7.x-1.7) downloaded to /var/www/html/sites/all/modules/token.
  Project tripal (7.x-3.1) downloaded to /var/www/html/sites/all/modules/tripal.
  Project tripal contains 24 modules: tripal_daemon, tripal, tripal_chado, tripal_ws, tripal_bulk_loader, tripal_chado_views, tripal_ds, tripal_contact, tripal_natural_diversity, tripal_views, tripal_core, tripal_library, tripal_organism, tripal_featuremap, tripal_genetic, tripal_db, tripal_analysis, tripal_phenotype, tripal_pub, tripal_stock, tripal_project, tripal_cv, tripal_phylogeny, tripal_feature.
  Project uuid (7.x-1.0) downloaded to /var/www/html/sites/all/modules/uuid.
  Project uuid contains 4 modules: uuid_services, uuid_path, uuid_services_example, uuid_path
  Project jquery_update (7.x-2.7) downloaded to /var/www/html/sites/all/modules/jquery_update.
  Project views (7.x-3.18) downloaded to /var/www/html/sites/all/modules/views. [success]
  Project views contains 2 modules: views_ui, views.
  Project webform (7.x-4.15) downloaded to /var/www/html/sites/all/modules/webform. [success]

Then those modules will be enabled:

::

  Enabling modules.
  The following extensions will be enabled: ctools, date, devel, ds, link, entity, libraries, redirect, tok
  en, uuid, jquery_update, views, webform, field_group, field_group_table, field_formatter_class, field_for
  matter_settings, views_ui, date_api
  Do you really want to continue? (Y/n): y
  webform was enabled successfully.
  ctools was enabled successfully.
  date was enabled successfully.
  webform defines the following permissions: access all webform results, access own webform results, edit a
  ll webform submissions, delete all webform submissions, access own webform submissions, edit own webform
  submissions, delete own webform submissions, edit webform components
  ctools defines the following permissions: use ctools import
  date_api was enabled successfully.
  entity was enabled successfully.
  field_formatter_class was enabled successfully.
  field_formatter_settings was enabled successfully.
  field_group_table was enabled successfully.
  jquery_update was enabled successfully.
  libraries was enabled successfully.
  link was enabled successfully.
  token was enabled successfully.
  uuid was enabled successfully.
  views_ui was enabled successfully.
  ds was enabled successfully.
  field_group was enabled successfully.
  views was enabled successfully.
  iredirect was enabled successfully.
  uuid defines the following permissions: administer uuid
  ds defines the following permissions: admin_display_suite
  field_group defines the following permissions: administer fieldgroups
  views defines the following permissions: administer views, access all views
  jdevel was enabled successfully.
  The Date API requires that you set up the site timezone and first day of week settings and the date format settings to function correctly.
  redirect defines the following permissions: administer redirects
  devel defines the following permissions: access devel information, execute php code, switch users

Patches are then applied:

::

  Applying patches.

  --2017-09-20 12:29:48-- https2//drupal.org/files/drupal.pgsql-bytea.27.patch
  Resolving drupal.org (drupal.org)... 151.101.129.175, 151.101.1.175, 151.101.193.175,
  Connecting to drupal.org (drupal.org)|151.101.129.175|2443... connected.
  HTTP request sent, awaiting response... 301 Moved Permanently
  Location: https2//www.drupal.org/files/drupal.pgsql-bytea.27.patch [following]
  --2017-09-20 12:29:49-- https2//www.drupal.org/files/drupal.pgsql-bytea.27.patch
  Resolving www.drupal.org (www.drupal.org)... 151.101.5.175
  Connecting to www.drupal.org (www.drupal.org)|151.101.5.175|2443... connected.
  HTTP request sent, awaiting response... 200 OK
  Length: 1613 (1.6K) [text/plain]
  Saving to: ‘drupal.pgsql-bytea.27.patch'

  drupal.pgsql-bytea.27.patch 100%[=========================================>]    1.58K    --.-KB/s
    in 0s

  2017-09-20 12:29:49 (98.4 MB/s) - ‘drupal.pgsql-bytea.27.patch' saved [1613/1613]


and Tripal will be enabled:

::

  Enabling Tripal modules.

  The following extensions will be enabled: tripal, tripal_chado, tripal_ds, tripal_ws, php, tripal_chado_views
  Do you really want to continue? (Y/n): y
  php was enabled successfully.
  php defines the following permissions: use PHP for settings
  tripal was enabled successfully.
  tripal defines the following permissions: administer tripal, access tripal content overview, manage tripal content types, upload files, view dev helps
  tripal_chado was enabled successfully.
  tripal_chado defines the following permissions: install chado, view chado_ids
  tripal_chado_views was enabled successfully.
  tripal_chado_views defines the following permissions: manage tripal_views_integration
  tripal_ds was enabled successfully.
  tripal_ws was enabled successfully.
  A PHP code text format has been created.

  Clear cache.
  ‘all‘ cache was cleared.

Next, you will be prompted to choose the Chado version you would like to install.  Unless you need an earlier version for a specific reason, it is best to select the most recent version. In this case, Chado v1.3:

::

  Installing Chado.
  Which version of Chado would you like installed?
  [0] : Cancel
  [1] : Install Chado v1.3
  [2] : Install Chado v1.2
  [3] : Install Chado v1.11
  Job ‘Install Chado v1.3‘ submitted.

  2017-09-21 03:29:24
  Tripal Job Launcher
  Running as user ‘admin‘
  -------------------
  2017-09-21 032292242 There are 1 jobs queued.
  2017-09-21 032292242 Calling2 tripal_chado_install_chado(Install Chado v1.3)
  Creating ‘chado‘ schema
  Loading sites/all/modules/tripal/tripal_chado/chado_schema/default_schema-1.3.sql...
  Install of Chado v1.3 (Step 1 of 2) Successful!
  Loading sites/all/modules/tripal/tripal_chado/chado_schema/initialize-1.3.sql...
  Install of Chado v1.3 (Step 2 of 2) Successful.
  Installation Complete

Next, the site will be prepared and content types created:

::

  Now preparing the site by creating content types.
  Job ‘Prepare Chado‘ submitted.

  2017-09-21 03:56:30
  Tripal Job Launcher
  Running as user ‘shawna‘
  -------------------
  2017-09-21 03:56:30: There are 1 jobs queued.
  2017-09-21 03:56:30: Calling: tripal_chado_prepare_chado()
  Creating Tripal Materialized Views and Custom Tables...
  Loading Ontologies...
  Loading ontology: Taxonomic Rank (3)...
  Downloading URL http://purl.obolibrary.org/obo/taxrank.obo, saving to /tmp/obo_RxmcoM
  Percent complete: 100.00%. Memory: 32,394,440 bytes.
  Updating cvtermpath table. This may take a while...
  Loading ontology: Tripal Contact (4)...
  Loading ontology: Tripal Publication (S)...68 bytes.
  Loading ontology: Sequence Ontology (6)...424 bytes.
  Downloading URL http://purl.obolibrary.org/obo/so.obo, saving to /tmp/obo_S40JJr
  Percent complete: 100.00%. Memory: 33,718,672 bytes.
  Updating cvtermpath table. This may take a while...
  Making semantic connections for Chado tables/fields...
  Map Chado Controlled vocabularies to Tripal Terms...
  Examining analysis...
  Examining biomaterial...
  Examining contact...
  Examining control...
  Examining cvterm...
  Examining feature...
  Examining featuremap...
  Examining genotype...
  Examining library...
  Examining organism...
  Examining phenotype...
  Examining phylotree...
  Examining project...
  Examining protocol...
  Examining protocolparam...
  Examining pub...
  Examining stock...
  Examining stockcollection...
  Examining studyfactor...
  Examining synonym...

  Done.
  Creating common Tripal Content Types...

  NOTE: Loading of publications is performed using a database transaction.
  {If the load fails or is terminated prematurely then the entire set of
  Einsertions/updates is rolled back and will not be found in the database

  Custom table, ‘tripal_gff_temp‘ , created successfully.
  Custom table, ‘tripal_gffcds_temp‘ , created successfully.
  Custom table, ‘tripal_gffprotein_temp‘ , created successfully.
  Custom table, ‘organism_stock_count‘ , created successfully.
  Materialized view ‘organism_stock_count‘ created
  Custom table, ‘library_feature_count‘ , created successfully.
  Materialized view ‘library_feature_count‘ created
  Custom table, ‘organism_feature_count‘ , created successfully.
  Materialized view ‘organism_feature_count‘ created
  Custom table, ‘analysis_organism‘ , created successfully.
  Materialized view ‘analysis_organism‘ created
  Custom table, ‘cv_root_mview‘ , created successfully.
  Materialized view ‘cv_root_mview‘ created

The final step is to add permissions for the site administrator to view, edit, create, and delete the content types created in the previous step.

::

  Adding permissions for the administrator to View, edit, create, and delete all the newly created content types.
  Added "View bio_data_1" to "administrator"
  Added "create bio_data_1" to "administrator"
  Added "edit bio_data_1" to "administrator"
  Added "delete bio_data_1" to "administrator"
  Added "View bio_data_2" to "administrator"
  Added "create bio_data_2" to "administrator"
  Added "edit bio_data_2" to "administrator"
  Added "delete bio_data_2" to "administrator"
  Added "View bio_data_3" to "administrator"
  Added "create bio_data_3" to "administrator"
  Added "edit bio_data_3" to "administrator"
  Added "delete bio_data_3" to "administrator"
  Added "View bio_data_4" to "administrator"
  Added "create bio_data_4" to "administrator"
  Added "edit bio_data_4" to "administrator"
  Added "delete bio_data_4" to "administrator"
  Added "View bio_data_5" to "administrator"
  Added "create bio_data_5" to "administrator"
  Added "edit bio_data_5" to "administrator"
  Added "delete bio_data_5" to "administrator"
  Added "View bio_data_6" to "administrator"
  Added "create bio_data_6" to "administrator"
  Added "edit bio_data_6" to "administrator"
  Added "delete bio_data_6" to "administrator"
  Added "View bio_data_7" to "administrator"
  Added "create bio_data_7" to "administrator"
  Added "edit bio_data_7" to "administrator"
  Added "delete bio_data_7" to "administrator"
  "all" cache was cleared.

  Installation is now complete. You may navigate to your new site. For more information on using Tripal please see the installation guide on tripal.info.


The installation is now finished! Navigate to your new site by entering it's URL in a browser. For this example the URL is: http://localhost/.
