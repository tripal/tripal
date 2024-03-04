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
}
