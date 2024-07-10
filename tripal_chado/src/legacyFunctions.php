<?php

use Drupal\tripal\Services\TripalJob;

/**
 * Submit Load Chado Schema Form.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_load_drush_submit($action, $chado_schema = 'chado') {

  if ($action == 'Install Chado v1.3') {
    tripal_chado_install_chado($chado_schema, '1.3');
  }
  else {
    \Drupal::logger('tripal_chado')->error("NOT SUPPORTED: " . $action);
  }
}

/**
 * Install Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_chado($chado_schema = 'chado', $version = '1.3', $job = NULL) {
  $installer = \Drupal::service('tripal_chado.installer');
  $installer->setParameters([
    'output_schemas' => [$chado_schema],
    'version' => $version,
  ]);
  if (!$installer->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to install Chado in schema "
      . $chado_schema
      . ". See previous log messages for details."
    );
  }
}

/**
 * Rename Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_rename_schema($old_schema, $new_schema, $job = NULL) {
  $renamer = \Drupal::service('tripal_chado.renamer');
  $renamer->setParameters([
    'output_schemas' => [
      $old_schema,
      $new_schema,
    ],
  ]);
  if (!$renamer->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to rename schema '"
      . $old_schema
      . "' into '"
      . $new_schema
      . "'. See previous log messages for details."
    );
  }
}

/**
 * Drop Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_drop_schema($schema, $job = NULL) {
  $remover = \Drupal::service('tripal_chado.remover');
  $remover->setParameters([
    'output_schemas' => [$schema],
  ]);
  if (!$remover->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to remove schema "
      . $schema
      . ". See previous log messages for details."
    );
  }
}

/**
 * Clone Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_clone_schema($source_schema, $new_schema, $job = NULL) {
  $cloner = \Drupal::service('tripal_chado.cloner');
  $cloner->setParameters([
    'input_schemas' => [$source_schema, ],
    'output_schemas' => [$new_schema, ],
  ]);
  if (!$cloner->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to clone schema '"
      . $source_schema
      . "'. See previous log messages for details."
    );
  }
}

/**
 * Upgrades Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_upgrade_schema($chado_schema, $version = '1.3', $sql_file = FALSE, $cleanup = TRUE, $job = NULL) {
  $upgrader = \Drupal::service('tripal_chado.upgrader');
  $upgrader->setParameters([
    'output_schemas' => [$chado_schema, ],
    'version' => $version,
    'cleanup' => $cleanup,
    'filename' => $sql_file,
  ]);
  if (!$upgrader->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to upgrade schema '"
      . $chado_schema
      . "'. See previous log messages for details."
    );
  }
}

/**
 * Integrates Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_integrate_schema($chado_schema, $job = NULL) {
  $integrator = \Drupal::service('tripal_chado.integrator');
  $integrator->setParameters([
    'input_schemas' => [$chado_schema, ],
  ]);
  if (!$integrator->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to integrate schema '"
      . $chado_schema
      . "'. See previous log messages for details."
    );
  }
}

/**
 * Prepares Chado Schema.
 *
 * @ingroup tripal_chado
 */
function tripal_chado_prepare_chado($chado_schema = 'chado', $job = NULL) {
  $preparer = \Drupal::service('tripal_chado.preparer');
  $preparer->setParameters([
    'output_schemas' => [$chado_schema],
  ]);
  if (!$preparer->performTask()) {
    \Drupal::logger('tripal_chado')->error(
      "Failed to prepare schema "
      . $chado_schema
      . ". See previous log messages for details."
    );
  }
}
