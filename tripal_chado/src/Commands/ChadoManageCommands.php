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
   * @usage drush trp-install-chado --schema-name='testchado' --version=1.3
   *   Installs chado 1.3 in a schema named "testchado".
   */
  public function installChado($options = ['schema-name' => 'chado', 'chado-version' => 1.3]) {

    $this->output()->writeln('Installing chado version ' . $options['chado-version'] . ' in a schema named "' . $options['schema-name']. '"');

    $installer = \Drupal::service('tripal_chado.chadoInstaller');
    $installer->setSchema($options['schema-name']);
    $success = $installer->install($options['chado-version']);
    if ($success) {
      $this->output()->writeln('<info>[Success]</info> Chado was successfully installed.');
    }
  }

  /**
   * Drops the Chado schema.
   *
   * @command tripal-chado:drop-chado
   * @aliases trp-drop-chado
   * @options schema-name
   *   The name of the schema to drop.
   * @usage drush trp-drop-chado --schema-name='testchado'
   *   Removes the chado schema named "testchado".
   */
  public function dropChado($options = ['schema-name' => 'chado']) {

    $installer = \Drupal::service('tripal.bulkPgSchemaInstaller');
    $installer->dropSchema($options['schema-name']);
    $present = $installer->checkSchema($options['schema-name']);
    if (!$present) {
      $this->output()->writeln('<info>[Success]</info> Chado was successfully dropped.');
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

    $chado_version = 1.3;
    $schema_name = 'testchado';

    $this->output()->writeln('Installing chado version ' . $chado_version . ' in a schema named "' . $schema_name. '"');

    $installer = \Drupal::service('tripal_chado.chadoInstaller');
    $installer->setSchema($schema_name);
    $success = $installer->install($chado_version);
    if ($success) {
      $this->output()->writeln('<info>[Success]</info> Chado was successfully installed for testing.');
    }
  }
}
