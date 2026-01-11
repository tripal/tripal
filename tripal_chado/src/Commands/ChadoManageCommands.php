<?php
namespace Drupal\tripal_chado\Commands;

use Drush\Commands\DrushCommands;

/**
 * Drush commands
 */
class ChadoManageCommands extends DrushCommands {

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
   * Imports publications.
   */
  #[CLI\Command(name: 'tripal:trp-import-pub', aliases: ['trp-import-pub'])]
  #[CLI\Option(name: 'username', description: 'Required, the name of the user for whom the import is associated.')]
  #[CLI\Option(name: 'name', description: 'The name of an existing publication search query.')]
  #[CLI\Option(name: 'id', description: 'The ID number of an existing publication search query.')]
  #[CLI\Option(name: 'pmid', description: 'The PubMed ID of one or more publications to import, comma-delimited.')]
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
    $api_key = $options['api-key'] ?? NULL;
    $create_contact = $options['create-contact'] ?? 0;

    if (!$username) {
      $this->logger->error($this->t('The --username argument is required.'));
      return;
    }
    if (!$name && !$id && !$pmid) {
      $this->logger->error($this->t('Either the --name, --id, or --pmid argument is required.'));
      return;
    }

    if ($name) {
      $this->tripalImportPublicationByName($name, $chado_schema_name, $username);
    }
    if ($id) {
      $this->tripalImportPublicationById($id, $chado_schema_name, $username);
    }
    if ($pmid) {
      $this->tripalImportPublicationByPmid($pmid, $chado_schema_name, $username, $api_key, $create_contact);
    }
  }

  /**
   * Helper function to import by pub search query name.
   *
   * @param string $name
   *   The name of an existing publication search query.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param string $username
   *   The name of the user for whom the import is associated.
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationByName (string $name, string $chado_schema_name, string $username): void {
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $all_queries = $pub_library_manager->getSearchQueries();
    $queries = [];
    foreach ($all_queries as $query) {
      if ($query->name == $name) {
        $queries[] = $query;
      }
    }
    if (count($queries) == 0) {
      $this->logger->error($this->t('No pub search query matches the supplied name "@name"',
        ['@name' => $name]));
      return;
    }
    // Note that there is no unique constraint on name, so if more than one
    // matches, then do them all.
    foreach ($queries as $query) {
      $id = $query->pub_library_query_id;
      $this->tripalImportPublicationById($id, $chado_schema_name, $username);
    }
  }

  /**
   * Helper function to import by pub search query ID.
   *
   * @param int $id
   *   Primary key of the query in the tripal_pub_library_query table.
   *   Multiple id values can be specified if comma-delimited.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param string $username
   *   The name of the user for whom the import is associated.
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationById (int $id, string $chado_schema_name, string $username): void {
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $importer_manager = \Drupal::service('tripal.importer');
    $importer = $importer_manager->createInstance('pub_search_query_loader');

    $ids = explode(',', $id);
    foreach ($ids as $id) {
      $id = trim($id);
      if ($id) {
        $query = $pub_library_manager->getSearchQuery($id);
        if (!$query) {
          $this->logger->error($this->t('No pub search query matches the supplied ID "@id"',
            ['@id' => $id]));
        }
        else {
          $disabled = $query->disabled;
          if ($disabled != 0) {
            $this->logger->error($this->t('Pub search query "@id" is marked as disabled',
              ['@id' => $id]));
            }
          else {
            $criteria = unserialize($query->criteria);
            $do_contact = $query->do_contact;

            // Set up the importer arguments.
            $importer_args = [
              'run_args' => [
                'criteria' => $criteria,
                'schema_name' => $chado_schema_name,
              ],
            ];
            $importer->setArguments($importer_args);

            $this->output()->writeln('CP73 Importing publications');
            $importer->run();
          }
        }
      }
    }
  }

  /**
   * Helper function to import into chado by pubmed ID.
   *
   * @param string $pmid
   *   The PubMed ID of one or more publications to import.
   *   Multiple id values can be specified if comma-delimited.
   * @param string $chado_schema_name
   *   The name of the chado schema to use (defaults to "chado").
   * @param string $username
   *   The name of the user for whom the import is associated.
   * @param string $api_key
   *   Optional NCBI API key for faster requests.
   * @param int $create_contact
   *   Set to 1 to create contact records for authors (default: 0).
   *
   * @return void
   *   No return value.
   */
  protected function tripalImportPublicationByPmid (string $pmid, string $chado_schema_name, string $username, string $api_key, int $create_contact): void {
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

    $this->switchUser($username);

    try {
/**
      // Get services.
      $pub_library_manager = \Drupal::service('tripal.pub_library');

      $this->output()->writeln("Importing publication with PMID: " . $options['pmid'] . " from PubMed...");

      // Create criteria array to search for specific PMID.
      $criteria = [
        'plugin_id' => 'tripal_pub_library_PMID',
        'remote_db' => 'PMID',
        'num_criteria' => 1,
        'count' => 1,
        'page' => 0,
        'do_contact' => $create_contact ? 1 : 0,
        'disabled' => 0,
        'criteria' => $criteria;
          1 => [
            'search_terms' => $id,
            'scope' => 'id',
            'is_phrase' => 0,
            'operation' => '',
          ],
        ],
      ];

      // Add API key if provided.
      if ($options['api-key']) {
        $criteria['ncbi_api_key'] = $options['api-key'];
        $criteria['form_state_user_input']['ncbi_api_key'] = $options['api-key'];
      }

      // Create PubMed plugin instance.
      $plugin = $pub_library_manager->createInstance('tripal_pub_library_PMID', []);

      // Retrieve publication data.
      $this->output()->writeln("Fetching publication data from PubMed...");
      $page_results = $plugin->run($criteria);

      if (!is_array($page_results) || empty($page_results['pubs'])) {
        throw new \Exception("Failed to retrieve publication data for PMID: " . $options['pmid']);
      }

      if ($page_results['total_records'] == 0) {
        $this->output()->writeln("No publication found with PMID: " . $options['pmid']);
        return;
      }

      $publications = $page_results['pubs'];
      $this->output()->writeln("Found publication: " . ($publications[0]['Title'] ?? 'Unknown Title'));
*/

      // Use the importer manager to create the PubSearchQueryImporter instance.
      $importer_manager = \Drupal::service('tripal.importer');
      $importer = $importer_manager->createInstance('pub_search_query_loader');

      // Set up the importer arguments.
      $importer_args = [
        'run_args' => [
          'criteria' => [
            'plugin_id' => 'tripal_pub_library_PMID',
            'remote_db' => 'PMID',
            'num_criteria' => 1,
            'count' => 1,
            'page' => 0,
            'do_contact' => $create_contact ? 1 : 0,
            'disabled' => 0,
            'criteria' => $criteria,
          ],
          'schema_name' => $chado_schema_name,
        ],
      ];
      $importer->setArguments($importer_args);

      $this->output()->writeln("CP31 Importing publication into Chado database...");
      $x = $importer->run();
print "CP5 ";var_dump($x);//@@@

      $this->logger->success($this->t('CP37 Successfully imported publication(s) with PMID: ' . $pmid));

      // Show citation.
#      if (!empty($publications[0]['Citation'])) {
#        $this->output()->writeln("Citation: " . $publications[0]['Citation']);
#      }
    }
    catch (\Exception $e) {
      throw new \Exception("Failed to import publication: " . $e->getMessage());
    }
  }

}
