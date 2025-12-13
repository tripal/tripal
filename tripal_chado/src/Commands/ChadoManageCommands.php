<?php
namespace Drupal\tripal_chado\Commands;

use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drush\Attributes as CLI;
use Drush\Commands\DrushCommands;

/**
 * Drush commands
 */
class ChadoManageCommands extends DrushCommands {

use StringTranslationTrait;

  /**
   * Install the Chado schema.
   *
   * @command tripal-chado:install-chado
   * @aliases trp-install-chado
   * @options schema-name
   *   The name of the schema to install chado in.
   * @options chado-version
   *   The version of chado to install. Currently only 1.3 is supported.
   * @usage drush trp-install-chado --schema-name='teapot' --version=1.3
   *   Installs chado 1.3 in a schema named "teapot".
   */
  public function installChado($options = ['schema-name' => 'chado', 'chado-version' => 1.3]) {

    $this->output()->writeln('Installing chado version ' . $options['chado-version'] . ' in a schema named "' . $options['schema-name']. '"');

    $installer = \Drupal::service('tripal_chado.installer');
    $installer->setParameters([
      'output_schemas' => [  $options['schema-name']  ],
      'version' => $options['chado-version'],
    ]);
    if ($installer->performTask()) {
      $this->output()->writeln(dt('<info>[Success]</info> Chado was successfully installed.'));
    }
    else {
      throw new \Exception('Unable to install chado ' . $options['chado-version'] . ' in ' . $options['schema-name']);
    }
  }

  /**
   * Apply migrations to an existing Chado schema.
   *
   * @command tripal-chado:migrate-chado
   * @aliases trp-migrate-chado
   * @options schema-name
   *   The name of the schema to apply chado migrations to.
   * @usage drush trp-migrate-chado --schema-name='teapot'
   *   Applies all pending migrations to a schema named "teapot".
   */
  public function migrateChado($options = ['schema-name' => 'chado']) {

    // Confirm the schema exists.
    $tripaldbx = \Drupal::service('tripal.dbx');
    $schema_exists = $tripaldbx->schemaExists($options['schema-name']);
    if (!$schema_exists) {
      throw new \Exception("The schema '" . $options['schema-name'] . "' does not exist and therefore cannot be migrated.");
    }

    // First setup our task.
    $migrator = \Drupal::service('tripal_chado.apply_migrations');
    $migrator->setParameters([
      'input_schemas' => [$options['schema-name']],
    ]);

    // Look up the install ID
    $migrator->lookupInstallID();

    // Determine what work is to be done and format into a table.
    $all_migrations = $migrator->checkMigrationStatus();
    $header = ['Chado Version', 'Description', 'Applied On', 'Status'];
    $rows = [];
    $pending_migrations = 0;
    foreach ($all_migrations as $migration) {
      $formatted_date = '';
      if ($migration->applied_on) {
        $formatted_date = \Drupal::service('date.formatter')->format($migration->applied_on, 'html_date');
      }
      $rows[] = [
        $migration->version,
        $migration->description,
        $formatted_date,
        $migration->status,
      ];

      if ($migration->status !== 'Successful') {
        $pending_migrations++;
      }
    }

    $this->output()->writeln("\nThe following table summarizes the migrations for the '" . $options['schema-name'] . "' schema.");
    $this->io()->table($header, $rows);
    $this->output()->writeln('');

    if ($pending_migrations) {
      $response = $this->io()->confirm(
        "Would you like to apply $pending_migrations pending migrations?",
        TRUE
      );
      if ($response) {
        $success = $migrator->performTask();
        if ($success) {
          $this->output()->writeln(dt('<info>[Success]</info> Chado was successfully migrated to the most recent version.'));
        } else {
          throw new \Exception("Unable to migrate chado in schema '" . $options['schema-name'] . "'");
        }
      }
    }
    else {
      $this->output()->writeln(dt('<info>[Success]</info> Chado is already up to date. There are no migrations pending.'));
    }
    $this->output()->writeln('');
  }

  /**
   * Drops the Chado schema.
   *
   * @command tripal-chado:drop-chado
   * @aliases trp-drop-chado
   * @options schema-name
   *   The name of the schema to drop.
   * @usage drush trp-drop-chado --schema-name='teapot'
   *   Removes the chado schema named "teapot".
   */
  public function dropChado($options = ['schema-name' => 'chado']) {

    $remover = \Drupal::service('tripal_chado.remover');
    $remover->setParameters([
      'output_schemas' => [$options['schema-name']],
    ]);
    if ($remover->performTask()) {
      $this->output()->writeln('<info>[Success]</info> Chado was successfully dropped.');
    }
    else {
      throw new \Exception('Unable to drop chado in ' . $options['schema-name']);
    }

  }

  /**
   * Prepare the Tripal Chado system.
   *
   * @command tripal-chado:prepare
   * @aliases trp-prep-chado
   * @options schema-name
   *   The name of the chado schema to prepare. Only a single chado schema
   *   should be prepared with Tripal and this will become the default chado schema.
   * @usage drush trp-prep-chado --schema-name="chado"
   *   Prepare the Tripal Chado system and set the schema named "chado" as the
   *   default Chado instance to use with Tripal.
   */
  public function prepareChado($options = ['schema-name' => 'chado']) {

    $this->output()->writeln('Preparing Drupal ("public") + Chado ("' . $options['schema-name'] . '")...');

    $preparer = \Drupal::service('tripal_chado.preparer');
    $preparer->setParameters([
      'output_schemas' => [$options['schema-name']],
    ]);
    if ($preparer->performTask()) {
      $this->output()->writeln('<info>[Success]</info> Preparation complete.');
    }
    else {
      throw new \Exception('Unable to prepare Drupal + Chado in ' . $options['schema-name']);
    }
  }

  /**
   * Set-up the Tripal Chado test environment.
   *
   * @command tripal-chado:setup-tests
   * @aliases trp-prep-tests
   * @usage drush trp-prep-tests
   *   Sets up the standard Tripal Chado test environment.
   */
  public function setupTests() {

    $this->output()->writeln('There is no longer any need to prepare the chado test environment.');
  }

  /**
   * Publish Chado Records as Tripal Content.
   *
   * @command tripal-chado:publish
   * @aliases trp-chado-publish
   * @options schema-name
   *   The name of the chado schema to use.
   * @param string $bundle
   *   The id of the TripalContentType you would like to publish content for.
   * @param array $options
   *   Publish options. Defaults are
   *   'schema-name' => 'chado'
   *   'datastore' => 'chado_storage'
   *   'migration-file' => ''
   *   'lenient-migration' => FALSE
   *   'batch-size' => '1000'
   * @usage drush trp-chado-publish organism
   *   Submits a standard chado publish job for the organism content type which
   *   publishes records in the default chado schema organism table.
   * @usage drush trp-chado-publish organism --schema-name=prod
   *   Submits a chado publish job for the organism content type which
   *   publishes records in the prod.organism table.
   */
  public function publish(string $bundle, array $options = [
    'schema-name' => '',
    'datastore' => 'chado_storage',
    'batch-size' => '1000',
    'migration-file' => '',
    'lenient-migration' => FALSE,
    'republish' => FALSE]) {

    if ($options['migration-file'] and !file_exists($options['migration-file'])) {
      $this->output()->writeln('The specified file "' . $options['migration-file'] . '" does not exist');
      return;
    }
    if ($options['migration-file'] and $options['republish']) {
      $this->output()->writeln('The options --republish and --migration-file cannot be combined');
      return;
    }
    // If schema not supplied then grab default chado schema.
    if (!$options['schema-name']) {
      $chado = \Drupal::service('tripal_chado.database');
      $default_chado_schema = $chado->getSchemaName();
      $options['schema-name'] = $default_chado_schema;
    }
    $values = [
      'schema_name' => $options['schema-name'],
      'batch_size' => $options['batch-size'],
      'republish' => $options['republish'],
      'migration_file' => $options['migration-file'],
      'lenient_migration' => $options['lenient-migration'],
    ];

    $datastore = $options['datastore'];
    \Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager::runTripalJob(
       $bundle, $datastore, $values);
  }

  /**
   * Unpublish previously published Tripal Content.
   *
   * Chado records are not modified in any way by unpublish.
   *
   * @param string $bundle
   *   The id of the TripalContentType you would like to unpublish content for.
   * @param array $options
   *   Publish options, defaults are provided in the function declaration.
   *
   * @command tripal-chado:unpublish
   * @aliases trp-chado-unpublish
   * @options schema-name
   *   The name of the chado schema to use.
   * @options all
   *   Unpublish all records of the specified content type. Without this
   *   option, only orphaned records are unpublished.
   *
   * @usage drush trp-chado-unpublish contact
   *   Submits a standard chado publish job to unpublish only orphaned records
   *   in the contact content type.
   * @usage drush trp-chado-unpublish organism --all --schema-name=prod
   *   Submits a chado publish job for the organism content type which
   *   unpublishes ALL records based on the prod.organism table.
   */
  public function unpublish(
    string $bundle,
    array $options = [
      'schema-name' => '',
      'datastore' => 'chado_storage',
      'all' => FALSE,
    ]) {

    // If schema not supplied then grab default chado schema.
    if (!$options['schema-name']) {
      $chado = \Drupal::service('tripal_chado.database');
      $default_chado_schema = $chado->getSchemaName();
      $options['schema-name'] = $default_chado_schema;
    }
    $values = [
      'orphaned' => !$options['all'],
      'unpublish' => TRUE,
    ];

    $datastore = $options['datastore'];
    \Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager::runTripalJob(
      $bundle, $datastore, $values);
  }

  /**
   * Add a Chado schema to Tripal. Does not set this schema as the default, as
   * there can be more than one Chado schema added to Tripal.
   * See the command tripal-chado:set_default for this functionality.
   *
   * @command tripal-chado:add_to_tripal
   * @aliases trp-add-chado
   * @options schema-name
   *   The name of the chado schema to add to Tripal.
   * @usage drush trp-add-chado --schema-name="chado"
   *   Adds the specified Chado to Tripal.
   *
   */
  public function addToTripal($options = ['schema-name' => 'chado']) {

    $this->output()->writeln('Adding the schema "' . $options['schema-name'] . '" to Tripal...');

    $integrator = \Drupal::service('tripal_chado.integrator');
    $integrator->setParameters(
      [
        'input_schemas' => [$options['schema-name']]
      ]
    );

    if ($integrator->performTask()) {
      $this->output()->writeln('Successfully added the schema "' . $options['schema-name'] . '" to Tripal.');
    }
  }

  /**
   * Sets a specified Chado schema to be the default in Tripal. Only one
   * schema may be set to default at a time.
   *
   * @command tripal-chado:set_default_schema
   * @aliases trp-set-default
   * @options schema-name
   *   The name of the chado schema to be set to default in Tripal.
   * @usage drush trp-set-default --schema-name="chado"
   *   Sets the specified Chado to be default in Tripal.
   *
   */
  public function setDefault($options = ['schema-name' => 'chado']) {

    $this->output()->writeln('Setting the schema "' . $options['schema-name'] . '" to be default in Tripal...');

    // Ensure that the provided schema exists.
    $tripaldbx = \Drupal::service('tripal.dbx');

   if ($tripaldbx->schemaExists($options['schema-name'])) {
      $config = \Drupal::service('config.factory')
        ->getEditable('tripal_chado.settings')
      ;
      $success = $config->set('default_schema', $options['schema-name'])->save();

      if ($success) {
        $this->output()->writeln('Successfully set the schema "' . $options['schema-name'] . '" to be default.');
      }
    }
    else {
      throw new \Exception("Unable to set the default schema to '"
          . $options['schema-name'] . "' - that schema does not exist.");
    }
  }

  /**
   * Populate one or more Chado materialized views.
   *
   * @param string|null $view
   *   The name of a materialized view, required unless --all or --list is used.
   * @param array $options
   *   Optional settings.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:populate-mview', aliases: ['trp-pop-mview'])]
  #[CLI\Argument(
    name: 'view',
    description: 'A comma-delimited list of one or more materialized views to populate. Required unless --all or --list is specified.',
  )]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema.')]
  #[CLI\Option(name: 'all', description: 'Populate all materialized views.')]
  #[CLI\Option(name: 'list', description: 'List all materialized views, but do not populate them.')]
  #[CLI\Option(name: 'time', description: 'Show elapsed time to populate a materialized view.')]
  #[CLI\Usage(
    name: 'drush tripal-chado:populate-mview db2cv_mview,cv_root_mview --schema-name="teacup"',
    description: 'Populates the db2cv_mview and cv_root_mview materialized views in the chado schema named "teacup".',
  )]
  #[CLI\Usage(
    name: 'drush trp-pop-mview --all --time',
    description: 'Populates all materialized views in the default chado schema and shows elapsed time for each.',
  )]
  #[CLI\Usage(
    name: 'drush trp-pop-mview --list',
    description: 'Lists all existing materialized views in the default chado schema.',
  )]
  public function populateMview(
    ?string $view = NULL,
    array $options = [
      'schema-name' => NULL,
      'all' => FALSE,
      'list' => FALSE,
      'time' => FALSE,
    ],
  ): void {

    /** @var Drupal\tripal\TripalDBX\TripalDbx $tripal_dbx */
    $tripal_dbx = \Drupal::service('tripal.dbx');
    /** @var Drupal\Core\Config\ConfigFactory $config_factory */
    $config_factory = \Drupal::service('config.factory');
    /** @var Drupal\tripal_chado\Services\ChadoMviewsManager $mview_manager */
    $mview_manager = \Drupal::service('tripal_chado.materialized_views');

    $schema_name = $options['schema-name'] ?? NULL;
    if (!$schema_name) {
      $schema_name = $config_factory->get('tripal_chado.settings')->get('default_schema');
    }
    $schema_exists = $tripal_dbx->schemaExists($schema_name);
    if (!$schema_exists) {
      $this->logger->error($this->t('The schema "@schema_name" does not exist.',
        ['@schema_name' => $schema_name]));
      return;
    }

    if (!$options['all'] && !$options['list'] && !$view) {
      $this->logger->error($this->t('Provide a materialized view name or use --all or --list, or use --help for options.'));
      return;
    }

    // List of all materialized views, key is numeric ID, value is name.
    $all_mviews = $mview_manager->getTables($schema_name);
    if ($options['list']) {
      $this->logger->notice($this->t('The following materialized views exist in the "@schema_name" schema: @list.',
        ['@schema_name' => $schema_name, '@list' => implode(', ', $all_mviews)]));
      return;
    }
    if (!$all_mviews) {
      $this->logger->error($this->t('No materialized views exist in the "@schema_name" schema.',
        ['@schema_name' => $schema_name]));
      return;
    }

    // List of views to populate as specified by the drush command.
    $populate_list = [];
    if ($view) {
      $populate_list = explode(',', $view);
    }
    else {
      $populate_list = $all_mviews;
    }

    // Populate the materialized views.
    foreach ($populate_list as $view_name) {
      $view_name = trim($view_name);
      if ($view_name) {
        if (in_array($view_name, $all_mviews)) {
          $this->logger->notice($this->t('Populating "@view_name"',
            ['@view_name' => $view_name]));
          $start_time = microtime(TRUE);
          $mview = $mview_manager->loadByName($view_name, $schema_name);
          $mview->populate();
          if ($options['time']) {
            $etime = sprintf('%0.6f', microtime(TRUE) - $start_time);
            $this->logger->notice($this->t('Elapsed time @etime seconds.',
              ['@etime' => $etime]));
          }
        }
        else {
          $this->logger->error($this->t('Materialized view "@view_name" does not exist. Use the --list option to list available views.',
            ['@view_name' => $view_name]));
        }
      }
    }
  }

}
