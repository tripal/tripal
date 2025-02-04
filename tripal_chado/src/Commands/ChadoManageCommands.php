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

  /**
   * Add a Chado schema to Tripal. Does not set this schema as the default, as
   * there can be more than one Chado schema added to Tripal.
   * See the command tripal-chado:set_default for this functionality.
   *
   * @command tripal-chado:add_to_tripal
   * @alias trp-add-chado
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
   * @alias trp-set-default
   * @options schema-name
   *   The name of the chado schema to be set to default in Tripal.
   * @usage drush trp-set-default --schema-name="chado"
   *   Sets the specified Chado to be default in Tripal.
   *
   */
  public function setDefault($options = ['schema-name' => 'chado']) {

    $this->output()->writeln('Setting the schema "' . $options['schema-name'] . '" to be default in Tripal...');

    $config = \Drupal::service('config.factory')
      ->getEditable('tripal_chado.settings')
    ;
    $success = $config->set('default_schema', $options['schema-name'])->save();

    if ($success) {
      $this->output()->writeln('Successfull set the schema "' . $options['schema-name'] . '" to be default.');
    }
  }
}
