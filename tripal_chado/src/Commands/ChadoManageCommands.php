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
      throw new \Exception(dt(
        'Unable to install chado {version} in {schema}',
        [
          'schema' => $options['schema-name'],
          'version' => $options['chado-version'],
        ]
      ));
    }
  }

  /**
   * Apply migrations to an existing Chado schema.
   *
   * @command tripal-chado:migrate-chado
   * @aliases trp-migrate-chado
   * @options schema-name
   *   The name of the schema to apply chado migrations to.
   * @usage drush trp-install-chado --schema-name='teapot'
   *   Applies all pending migrations to a schema named "teapot".
   */
  public function migrateChado($options = ['schema-name' => 'chado']) {

    // First setup our task.
    $migrator = \Drupal::service('tripal_chado.apply_migrations');
    $migrator->setParameters([
      'input_schemas' => [$options['schema-name']],
    ]);

    // Look up the install ID
    // @todo lookup the install ID.

    // Determine what work is to be done and format into a table.
    $all_migrations = $migrator->checkMigrationStatus();
    $header = ['Chado Version', 'Description', 'Applied On', 'Status'];
    $rows = [];
    $pending_migrations = 0;
    foreach ($all_migrations as $migration) {
      $formatted_date = '';
      if ($migration->applied_on) {
        $formatted_date = \Drupal::service('date.formatter')->format($migration->applied_on, 'medium');
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

    $response = $this->io()->confirm(
      "Would you like to apply $pending_migrations pending migrations?",
      TRUE
    );
    if ($response) {
      $success = $migrator->performTask();
      if ($success) {
        $this->output()->writeln(dt('<info>[Success]</info> Chado was successfully migrated to the most recent version.'));
      } else {
        throw new \Exception(dt(
          'Unable to migrate chado in {schema}',
          ['schema' => $options['schema-name']]
        ));
      }
    }

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
      throw new \Exception(dt(
        'Unable to drop chado in {schema}',
        [
          'schema' => $options['schema-name'],
        ]
      ));
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
      throw new \Exception(dt(
        'Unable to prepare Drupal + Chado in @schema',
        [
          '@schema' => $options['schema-name'],
        ]
      ));
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
    // @todo validate the bundle
    $bundle = $bundle;
    $datastore = $options['datastore'];

    \Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager::runTripalJob(
       $bundle, $datastore, $values);
  }

}
