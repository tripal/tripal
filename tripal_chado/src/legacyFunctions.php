<?php

use Drupal\tripal\Services\TripalJob;

/**
 * Submit Load Chado Schema Form
 *
 * @ingroup tripal_chado
 */
function tripal_chado_load_drush_submit($action, $chado_schema = 'chado') {

  if ($action == 'Install Chado v1.3') {
    $installer = \Drupal::service('tripal_chado.chadoInstaller');
    $installer->setSchema($chado_schema);
    $success = $installer->install(1.3);
  }
  else {
    \Drupal::logger('tripal_chado')->error("NOT SUPPORTED: " . $action);
  }
}

/**
 * Install Chado Schema
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_chado($action, $chado_schema = 'chado', $job = NULL) {

  if ($action == 'Install Chado v1.3') {
    $installer = \Drupal::service('tripal_chado.chadoInstaller');
    $installer->setSchema($chado_schema);
    if ($job) {
      $installer->setJob($job);
    }
    $success = $installer->install(1.3);
  }
  else {
    \Drupal::logger('tripal_chado')->error("NOT SUPPORTED: " . $action);
  }
}

/**
 * Drop Chado Schema
 *
 * @ingroup tripal_chado
 */
function tripal_chado_drop_schema($schema, $job = NULL) {
  if ($schema) {
    \Drupal::service('tripal.bulkPgSchemaInstaller')->dropSchema($schema);
  }
  else {
    \Drupal::logger('tripal_chado')->error("No schema was provided. Cannot drop.");
  }
}
