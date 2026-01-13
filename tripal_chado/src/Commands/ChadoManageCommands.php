<?php

namespace Drupal\tripal_chado\Commands;

use Drupal\Core\Config\ConfigFactory;
use Drupal\Core\Datetime\DateFormatter;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal\TripalImporter\PluginManagers\TripalImporterManager;
use Drupal\tripal\TripalPubLibrary\PluginManagers\TripalPubLibraryManager;
use Drupal\tripal_chado\Task\ChadoApplyMigrations;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Task\ChadoInstaller;
use Drupal\tripal_chado\Task\ChadoIntegrator;
use Drupal\tripal_chado\Services\ChadoMviewsManager;
use Drupal\tripal_chado\Task\ChadoPreparer;
use Drupal\tripal_chado\Task\ChadoRemover;
use Drush\Attributes as CLI;
use Drush\Commands\DrushCommands;
use Symfony\Component\Console\Style\SymfonyStyle;

/**
 * Drush commands.
 */
class ChadoManageCommands extends DrushCommands {

  use StringTranslationTrait;

  /**
   * We use SymfonyStyle instead of $this->io() to allow phpunit testing.
   *
   * @var Symfony\Component\Console\Style\SymfonyStyle
   */
  protected SymfonyStyle $ssio;

  /**
   * TripalCommands Drush command class constructor.
   *
   * This is used to inject the services used by the various commands.
   */
  public function __construct(
    protected ConfigFactory $config_factory,
    protected DateFormatter $date_formatter,
    protected EntityTypeManagerInterface $entityTypeManager,
    protected TripalBackendPublishManager $publish_manager,
    protected TripalDbx $tripaldbx,
    protected TripalImporterManager $importer_manager,
    protected TripalPubLibraryManager $pub_library_manager,
    protected ChadoApplyMigrations $migrator,
    protected ChadoConnection $chado_connection,
    protected ChadoInstaller $installer,
    protected ChadoIntegrator $integrator,
    protected ChadoMviewsManager $mview_manager,
    protected ChadoPreparer $preparer,
    protected ChadoRemover $remover,
  ) {
    // Parent currently doesn't do anything here.
    parent::__construct();
  }

  /**
   * Looks up a user ID from a user name.
   *
   * @param string $username
   *   The name of a Drupal user.
   *
   * @return int
   *   The corresponding user ID, or zero if user name is not a valid account.
   */
  protected function lookupUserId(string $username): int {
    $uid = 0;
    $users = $this->entityTypeManager
      ->getStorage('user')
      ->loadByProperties(['name' => $username]);
    $user = $users ? reset($users) : FALSE;
    if ($user) {
      $uid = $user->id();
    }
    return $uid;
  }

  /**
   * Install the Chado schema.
   *
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:install-chado', aliases: ['trp-install-chado'])]
  #[CLI\Option(name: 'schema-name', description: 'The name of the schema to install chado in.')]
  #[CLI\Option(name: 'chado-version', description: 'The version of chado to install. Currently only 1.3 is supported.')]
  #[CLI\Usage(
    name: "drush trp-install-chado --schema-name='teapot' --version=1.3",
    description: 'Installs chado 1.3 in a schema named "teapot".',
  )]
  public function installChado(
    array $options = [
      'schema-name' => 'chado',
      'chado-version' => '1.3',
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');
    $chado_version = $options['chado-version'] ?? 1.3;

    $this->logger->notice($this->t('Installing chado version @ver in a schema named "@schema_name"',
      ['@ver' => $chado_version, '@schema_name' => $schema_name]));

    $this->installer->setParameters([
      'output_schemas' => [$schema_name],
      'version' => $chado_version,
    ]);
    if ($this->installer->performTask()) {
      $this->logger->notice($this->t('Chado was successfully installed.'));
    }
    else {
      $this->logger->error($this->t('Unable to install chado version @ver in a schema named "@schema_name"',
        ['@ver' => $chado_version, '@schema_name' => $schema_name]));
    }
  }

  /**
   * Apply migrations to an existing Chado schema.
   *
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:migrate-chado', aliases: ['trp-migrate-chado'])]
  #[CLI\Option(name: 'schema-name', description: 'The name of the schema to apply chado migrations to.')]
  #[CLI\Option(name: 'list', description: 'List migrations, but do not apply them.')]
  #[CLI\Usage(
    name: "drush trp-migrate-chado --schema-name='teapot'",
    description: 'Applies all pending migrations to a schema named "teapot".',
  )]
  public function migrateChado(
    array $options = [
      'schema-name' => 'chado',
      'list' => 0,
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');
    $option_list = $options['list'] ?? 0;
    $option_yes = $options['yes'] ?? 0;

    // We can't iniitialze this in __construct because the input and
    // output are not yet initialized there.
    $this->ssio = new SymfonyStyle($this->input(), $this->output());

    // Confirm the schema exists.
    $schema_exists = $this->tripaldbx->schemaExists($schema_name);
    if (!$schema_exists) {
      $this->logger->error($this->t('The schema "@schema_name" does not exist and therefore cannot be migrated.',
        ['@schema_name' => $schema_name]));
      return;
    }

    // First setup our task.
    $this->migrator->setParameters([
      'input_schemas' => [$schema_name],
    ]);

    // Look up the install ID.
    $this->migrator->lookupInstallID();

    // Determine what work is to be done and format into a table.
    $all_migrations = $this->migrator->checkMigrationStatus();
    $header = ['Chado Version', 'Description', 'Applied On', 'Status'];
    $rows = [];
    $pending_migrations = 0;
    foreach ($all_migrations as $migration) {
      $formatted_date = '';
      if ($migration->applied_on) {
        $formatted_date = $this->date_formatter->format($migration->applied_on, 'html_date');
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

    $this->output()->writeln("\nThe following table summarizes the migrations for the '" . $schema_name . "' schema.");
    $this->ssio->table($header, $rows);
    $this->output()->writeln('');

    if ($option_list) {
      return;
    }
    if ($pending_migrations) {
      $response = ($option_yes || $this->ssio->confirm(
        "Would you like to apply $pending_migrations pending migrations?",
        TRUE
      ));
      if ($response) {
        if ($this->migrator->performTask()) {
          $this->logger->notice($this->t('Chado in schema "@schema_name" was successfully migrated to the most recent version.',
            ['@schema_name' => $schema_name]));
        }
        else {
          $this->logger->error($this->t('Unable to migrate chado in schema "@schema_name"',
            ['@schema_name' => $schema_name]));
        }
      }
    }
    else {
      $this->logger->notice($this->t('Chado in schema "@schema_name" is already up to date. There are no migrations pending.',
        ['@schema_name' => $schema_name]));
    }
    $this->output()->writeln('');
  }

  /**
   * Drops the Chado schema.
   *
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:drop-chado', aliases: ['trp-drop-chado'])]
  #[CLI\Option(name: 'schema-name', description: 'The name of the schema to drop.')]
  #[CLI\Usage(
    name: "drush trp-drop-chado --schema-name='teapot'",
    description: 'Removes the chado schema named "teapot".',
  )]
  public function dropChado(
    array $options = [
      'schema-name' => 'chado',
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');

    $this->remover->setParameters([
      'output_schemas' => [$schema_name],
    ]);
    if ($this->remover->performTask()) {
      $this->logger->notice($this->t('Chado schema "@schema_name" was successfully dropped.',
        ['@schema_name' => $schema_name]));
    }
    else {
      $this->logger->error($this->t('Unable to drop chado schema "@schema_name".',
        ['@schema_name' => $schema_name]));
    }
  }

  /**
   * Prepare the Tripal Chado schema.
   *
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:prepare', aliases: ['trp-prep-chado'])]
  #[CLI\Option(name: 'schema-name', description: 'Optional name of the chado schema to prepare. If not specified, the default schema is prepared. Only a single chado schema should be prepared with Tripal and this will become the default chado schema.')]
  #[CLI\Usage(
    name: 'drush trp-prep-chado --schema-name="chado"',
    description: 'Prepare the Tripal Chado system and set the schema named "chado" as the default Chado instance to use with Tripal.',
  )]
  public function prepareChado(
    array $options = [
      'schema-name' => NULL,
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');

    $this->logger->notice($this->t('Preparing Drupal ("public") + Chado ("@schema_name").',
      ['@schema_name' => $schema_name]));

    $this->preparer->setParameters([
      'output_schemas' => [$schema_name],
    ]);

    if ($this->preparer->performTask()) {
      $this->logger->notice($this->t('Preparation complete.'));
    }
    else {
      $this->logger->error($this->t('Unable to prepare Drupal + Chado schema "@schema_name".',
        ['@schema_name' => $schema_name]));
    }
  }

  /**
   * Add a Chado schema to Tripal.
   *
   * This command does not set this schema as the default, as
   * there can be more than one Chado schema added to Tripal.
   * See the command tripal-chado:set_default for this functionality.
   *
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:add_to_tripal', aliases: ['trp-add-chado'])]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema to add to Tripal.')]
  #[CLI\Usage(
    name: 'drush trp-add-chado --schema-name="chado"',
    description: 'Adds the specified Chado schema to Tripal.',
  )]
  public function addToTripal(
    array $options = [
      'schema-name' => 'chado',
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');

    $this->logger->notice($this->t('Adding the Chado schema "@schema_name" to Tripal.',
      ['@schema_name' => $schema_name]));

    $this->integrator->setParameters(
      [
        'input_schemas' => [$schema_name],
      ]
    );

    if ($this->integrator->performTask()) {
      $this->logger->notice($this->t('Successfully added the Chado schema "@schema_name" to Tripal.',
        ['@schema_name' => $schema_name]));
    }
    else {
      $this->logger->notice($this->t('Failed to add the Chado schema "@schema_name" to Tripal.',
        ['@schema_name' => $schema_name]));
    }
  }

  /**
   * Sets a specified Chado schema to be the default in Tripal.
   *
   * Only one schema may be set to default at a time.
   *
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:set_default_schema', aliases: ['trp-set-default'])]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema to be set to default in Tripal.')]
  #[CLI\Usage(
    name: 'drush trp-set-default --schema-name="chado"',
    description: 'Sets the Chado schema "chado" to be default in Tripal.',
  )]
  public function setDefault(
    array $options = [
      'schema-name' => NULL,
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? NULL;
    if (!$schema_name) {
      $this->logger->error($this->t('The "schema-name" parameter is required.'));
      return;
    }

    $this->logger->notice($this->t('Setting the schema "@schema_name" to be default in Tripal.',
      ['@schema_name' => $schema_name]));

    // Ensure that the provided schema exists.
    $success = FALSE;
    if ($this->tripaldbx->schemaExists($schema_name)) {
      $config = $this->config_factory->getEditable('tripal_chado.settings');
      $success = $config->set('default_schema', $schema_name)->save();
    }
    if ($success) {
      $this->logger->notice($this->t('Successfully set the schema "@schema_name" to be default.',
        ['@schema_name' => $schema_name]));
    }
    else {
      $this->logger->error($this->t('Unable to set the default schema to "@schema_name" - that schema does not exist.',
        ['@schema_name' => $schema_name]));
    }
  }

  /**
   * Set-up the Tripal Chado test environment.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:setup-tests', aliases: ['trp-prep-tests'])]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema to prepare. Only a single chado schema should be prepared with Tripal and this will become the default chado schema.')]
  #[CLI\Usage(
    name: 'drush trp-prep-tests',
    description: 'A legacy command to set up the standard Tripal Chado test environment. This command is no longer needed and does not do anything.',
  )]
  public function setupTests(): void {
    $this->logger->notice($this->t('There is no longer any need to prepare the chado test environment.'));
  }

  /**
   * Publish Chado Records as Tripal Content.
   *
   * @param string $bundle
   *   The id of the TripalContentType you would like to unpublish content for.
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:publish', aliases: ['trp-chado-publish'])]
  #[CLI\Argument(
    name: 'bundle',
    description: 'The id of the TripalContentType you would like to publish content for.',
  )]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema to use.')]
  #[CLI\Option(name: 'datastore', description: 'Storage used, defaults to chado_storage.')]
  #[CLI\Option(name: 'migration-file', description: 'Name of a file used to migrate entity ID numbers from a Tripal 3 site.')]
  #[CLI\Option(name: 'lenient-migration', description: 'Use if some content was not published on the Tripal 3 site.')]
  #[CLI\Option(name: 'batch-size', description: 'Publish in batches of this size to reduce memory usage.')]
  #[CLI\Usage(
    name: 'drush trp-chado-publish organism',
    description: 'Publishes the organism content type using records in the default chado schema organism table.',
  )]
  #[CLI\Usage(
    name: 'drush trp-chado-publish organism --schema-name=teacup',
    description: 'Publishes the organism content type using records in the teacup.organism table.',
  )]
  #[CLI\Usage(
    name: 'drush trp-chado-publish organism --migration-file=tripal3_entity_mapping.tsv --lenient-migration --batch-size=500',
    description: 'Publishes organism content from a migrated tripal 3 site where not every record had been published. Memory is limited so reduce the batch sise.',
  )]
  public function publish(
    string $bundle,
    array $options = [
      'schema-name' => NULL,
      'datastore' => 'chado_storage',
      'batch-size' => '1000',
      'migration-file' => '',
      'lenient-migration' => FALSE,
      'republish' => FALSE,
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');
    $datastore = $options['datastore'] ?? 'chado_storage';
    $batch_size = $options['batch-size'] ?? '1000';
    $migration_file = $options['migration-file'] ?? '';
    $lenient_migration = $options['lenient-migration'] ?? FALSE;
    $republish = $options['republish'] ?? FALSE;

    if ($migration_file and !file_exists($migration_file)) {
      $this->logger->error($this->t('The specified migration file "@migration_file" does not exist.',
        ['@migration_file' => $migration_file]));
      return;
    }
    if ($migration_file and $republish) {
      $this->logger->error($this->t('The options --republish and --migration-file cannot be combined.'));
      return;
    }

    $publish_options = [
      'schema_name' => $schema_name,
      'batch_size' => $batch_size,
      'republish' => $republish,
      'migration_file' => $migration_file,
      'lenient_migration' => $lenient_migration,
      'bundle' => $bundle,
      'datastore' => $datastore,
      'job' => NULL,
    ];
    $publish_instance = $this->publish_manager->createInstance($datastore);
    $publish_instance->publish($publish_options);
  }

  /**
   * Unpublish previously published Tripal Content.
   *
   * Chado records are not modified in any way by unpublish.
   *
   * @param string $bundle
   *   The id of the TripalContentType you would like to unpublish content for.
   * @param array $options
   *   Options passed on the drush command line.
   *
   * @return void
   *   No return value.
   */
  #[CLI\Command(name: 'tripal-chado:unpublish', aliases: ['trp-chado-unpublish'])]
  #[CLI\Argument(
    name: 'bundle',
    description: 'The id of the TripalContentType you would like to unpublish content for.',
  )]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema to use.')]
  #[CLI\Option(name: 'all', description: 'Unpublish all records of the specified content type. Without this option, only orphaned records are unpublished.')]
  #[CLI\Usage(
    name: 'drush trp-chado-unpublish contact',
    description: 'Submits a standard chado publish job to unpublish only orphaned records in the contact content type.',
  )]
  #[CLI\Usage(
    name: 'drush trp-chado-unpublish organism --all --schema-name=teacup',
    description: 'Submits a chado publish job for the organism content type which unpublishes ALL records based on the teacup.organism table.',
  )]
  public function unpublish(
    string $bundle,
    array $options = [
      'schema-name' => NULL,
      'datastore' => 'chado_storage',
      'all' => FALSE,
    ],
  ): void {
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');
    $datastore = $options['datastore'] ?? 'chado_storage';
    $option_all = $options['all'] ?? FALSE;

    $publish_options = [
      'schema_name' => $schema_name,
      'orphaned' => !$option_all,
      'unpublish' => TRUE,
      'bundle' => $bundle,
      'datastore' => $datastore,
      'job' => NULL,
    ];
    $publish_instance = $this->publish_manager->createInstance($datastore);
    $publish_instance->publish($publish_options);
  }

  /**
   * Populate one or more Chado materialized views.
   *
   * @param string|null $view
   *   The name of a materialized view, required unless --all or --list is used.
   * @param array $options
   *   Options passed on the drush command line.
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

    // Get options or set default if not specified.
    $schema_name = $options['schema-name'] ?? $this->config_factory->get('tripal_chado.settings')->get('default_schema');
    $option_all = $options['all'] ?? FALSE;
    $option_list = $options['list'] ?? FALSE;
    $option_time = $options['time'] ?? FALSE;

    $schema_exists = $this->tripaldbx->schemaExists($schema_name);
    if (!$schema_exists) {
      $this->logger->error($this->t('The schema "@schema_name" does not exist.',
        ['@schema_name' => $schema_name]));
      return;
    }

    if (!$option_all && !$option_list && !$view) {
      $this->logger->error($this->t('Provide a materialized view name or use --all or --list, or use --help for options.'));
      return;
    }

    // List of all materialized views, key is numeric ID, value is name.
    $all_mviews = $this->mview_manager->getTables($schema_name);
    if ($option_list) {
      if ($all_mviews) {
        $this->logger->notice($this->t('The following materialized views exist in the "@schema_name" schema: @list.',
          ['@schema_name' => $schema_name, '@list' => implode(', ', $all_mviews)]));
      }
      else {
        $this->logger->notice($this->t('No materialized views exist in the "@schema_name" schema.',
          ['@schema_name' => $schema_name]));
      }
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
          $mview = $this->mview_manager->loadByName($view_name, $schema_name);
          $mview->populate();
          if ($option_time) {
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

  /**
   * Imports publications.
   */
  #[CLI\Command(name: 'tripal-chado:import-pub', aliases: ['trp-import-pub'])]
  #[CLI\Option(name: 'username', description: 'Required, the name of the user for whom the import is associated.')]
  #[CLI\Option(name: 'name', description: 'The name of an existing publication search query.')]
  #[CLI\Option(name: 'id', description: 'The ID number(s) of one or more existing publication search query, comma-delimited.')]
  #[CLI\Option(name: 'pmid', description: 'The PubMed ID(s) of one or more publications to import, comma-delimited.')]
  #[CLI\Option(name: 'schema-name', description: 'The name of the chado schema to use (defaults to "chado")')]
  #[CLI\Option(name: 'api-key', description: 'pmid only: Optional NCBI API key for faster requests.')]
  #[CLI\Option(name: 'create-contact', description: 'pmid only: Set to 1 to create contact records for authors (default: 0).')]
  #[CLI\Usage(
    name: 'drush trp-import-pub --name="Query Name" --username=[USERNAME]',
    description: 'Performs the search and imports publications from the search query with the name "Query Name".',
  )]
  #[CLI\Usage(
    name: 'drush trp-import-pub --id=1 --username=[USERNAME]',
    description: 'Performs the search and imports publications from the search query with the record ID of 1 in the tripal_pub_library_query table.',
  )]
  #[CLI\Usage(
    name: 'drush trp-import-pub --pmid=12345678 --username=[USERNAME]',
    description: 'Imports a single publication with PMID 12345678',
  )]
  #[CLI\Usage(
    name: 'drush trp-import-pub --pmid=12345678 --create-contact=1 --api-key=[API_KEY] --username=[USERNAME]',
    description: 'Imports publication with contact creation and API key for faster processing.',
  )]
  public function tripalImportPublication(
    $options = [
      'username' => NULL,
      'name' => NULL,
      'id' => NULL,
      'pmid' => NULL,
      'schema-name' => 'chado',
      'api-key' => NULL,
      'create-contact' => 0,
    ],
  ) {

    $username = $options['username'] ?? NULL;
    $name = $options['name'] ?? NULL;
    $id = $options['id'] ?? NULL;
    $pmid = $options['pmid'] ?? NULL;
    $chado_schema_name = $options['schema-name'] ?? 'chado';
    $api_key = $options['api-key'] ?? '';
    $create_contact = $options['create-contact'] ?? 0;

    if (!$name && !$id && !$pmid) {
      $this->logger->error($this->t('Either the --name, --id, or --pmid argument is required.'));
      return;
    }

    if (!$username) {
      $this->logger->error($this->t('The --username argument is required.'));
      return;
    }
    $uid = $this->lookupUserId($username);
    if (!$uid) {
      $this->logger->error($this->t('The specified username "@username" does not exist.',
        ['@username' => $username]));
      return;
    }

    if ($name) {
      $this->tripalImportPublicationByName($name, $chado_schema_name, $uid);
    }
    if ($id) {
      $this->tripalImportPublicationById($id, $chado_schema_name, $uid);
    }
    if ($pmid) {
      $this->tripalImportPublicationByPmid($pmid, $chado_schema_name, $uid, $api_key, $create_contact);
    }
  }

  /**
   * Import publications into chado by pub search query name.
   *
   * @param string $name
   *   The name of an existing publication search query.
   *   Note that there is no unique constraint on name, so if more than one
   *   pub search query has the exact same name, we will import them all.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param int $uid
   *   The ID of the user for whom the import is associated.
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationByName(string $name, string $chado_schema_name, int $uid): void {
    $all_queries = $this->pub_library_manager->getSearchQueries();
    $query_ids = [];
    foreach ($all_queries as $query) {
      if ($query->name == $name) {
        $query_ids[] = $query->pub_library_query_id;
      }
    }
    if (count($query_ids) == 0) {
      $this->logger->error($this->t('No pub search query matches the supplied name "@name"',
        ['@name' => $name]));
      return;
    }
    $this->tripalImportPublicationById(implode(',', $query_ids), $chado_schema_name, $uid);
  }

  /**
   * Import publications into chado by pub search query ID.
   *
   * @param string $id
   *   Primary key (int) of the query in the tripal_pub_library_query table.
   *   Multiple id values can be specified if comma-delimited.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param int $uid
   *   The ID of the user for whom the import is associated.
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationById(string $id, string $chado_schema_name, int $uid): void {
    $importer = $this->importer_manager->createInstance('pub_search_query_loader');
    $valid_queries = [];
    $ids = explode(',', $id);
    foreach ($ids as $id) {
      $id = trim($id);
      if ($id) {
        $query = $this->pub_library_manager->getSearchQuery($id);
        if ($query) {
          $disabled = $query->disabled;
          if ($disabled != 0) {
            $this->logger->error($this->t('Pub search query "@id" is marked as disabled',
              ['@id' => $id]));
            return;
          }
          $valid_queries[$id] = $query;
        }
        else {
          $this->logger->error($this->t('No pub search query matches the supplied ID "@id"',
            ['@id' => $id]));
          return;
        }
      }
    }
    // This could happen if only a comma was entered.
    if (!$valid_queries) {
      $this->logger->error($this->t('No valid search queries were supplied.'));
      return;
    }

    foreach ($valid_queries as $id => $query) {
      // Set up the importer arguments.
      $criteria = unserialize($query->criteria, ['allowed_classes' => FALSE]);
      $importer_args = [
        'run_args' => [
          'criteria' => $criteria,
          'schema_name' => $chado_schema_name,
          'uid' => $uid,
          'return_citations' => TRUE,
        ],
      ];
      $importer->setArguments($importer_args);
      $citations = $importer->run();

      // Show the citation for each imported publication.
      $this->listCitations($citations);
    }
  }

  /**
   * Import publication(s) into chado by pubmed ID.
   *
   * @param string $pmid
   *   The PubMed ID of one or more publications to import.
   *   Multiple id values can be specified if comma-delimited.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param int $uid
   *   The ID of the user for whom the import is associated.
   * @param string $api_key
   *   Optional NCBI API key for faster requests.
   * @param int $create_contact
   *   Set to 1 to create contact records for authors (default: 0).
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationByPmid(string $pmid, string $chado_schema_name, int $uid, string $api_key, int $create_contact): void {
    // Use the importer manager to create the PubSearchQueryImporter instance.
    $importer = $this->importer_manager->createInstance('pub_search_query_loader');

    // Set up criteria using pmid value(s).
    $pmids = explode(',', $pmid);
    $criteria = [];
    foreach ($pmids as $index => $id) {
      $id = trim($id);
      if ($id) {
        // Validate that each PMID is numeric.
        if (!is_numeric($id)) {
          $this->logger->error($this->t('A PMID must be a numeric value. Invalid value supplied was "@id"',
            ['@id' => $id]));
          return;
        }
        // Criteria array is expected to start at 1.
        $criteria[$index + 1] = [
          'search_terms' => $id,
          'scope' => 'id',
          'is_phrase' => 0,
          'operation' => $criteria ? 'OR' : '',
        ];
      }
    }

    // Set up the importer arguments.
    $importer_args = [
      'run_args' => [
        'criteria' => [
          'plugin_id' => 'tripal_pub_library_PMID',
          'remote_db' => 'PMID',
          'num_criteria' => count($criteria),
          'count' => count($criteria),
          'page' => 0,
          'do_contact' => $create_contact ? 1 : 0,
          'disabled' => 0,
          'criteria' => $criteria,
        ],
        'schema_name' => $chado_schema_name,
        'uid' => $uid,
        'return_citations' => TRUE,
      ],
    ];

    try {
      $importer->setArguments($importer_args);
      $citations = $importer->run();

      // Show the citation for each imported publication.
      $this->listCitations($citations);
    }
    catch (\Exception $e) {
      throw new \Exception('Failed to import publication: ' . $e->getMessage());
    }
  }

  /**
   * Display citations of imported publications.
   *
   * @param array $citations
   *   Citations of imported publications, could be an empty array.
   *
   * @return void
   *   No return value, output is to screen.
   */
  protected function listCitations(array $citations): void {
    if ($citations) {
      if (count($citations) == 1) {
        $this->logger->success($this->t('Imported "@cit"', ['@cit' => $citations[0]]));
      }
      else {
        $this->logger->success($this->t('Imported @n publications:', ['@n' => count($citations)]));
        foreach ($citations as $index => $citation) {
          $this->output()->writeln(($index + 1) . ': "' . $citation . '"');
        }
      }
    }
  }

}
