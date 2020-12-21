<?php

namespace Drupal\tripal_chado\Commands;

use Drush\Commands\DrushCommands;

/**
 * Drush commands
 */
class ChadoManageCommands extends DrushCommands {

  /**
   * Executes one or more jobs in the Tripal Jobs Queue.
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
}
