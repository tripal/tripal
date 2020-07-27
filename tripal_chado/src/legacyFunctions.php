<?php

/**
 * Submit Load Chado Schema Form
 *
 * @ingroup tripal_chado
 */
function tripal_chado_load_drush_submit($action, $chado_schema = 'chado') {

  if ($action == 'Install Chado v1.3') {
    \Drupal::service('tripal_chado.chadoInstaller', $chado_schema)
      ->install(1.3);
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
function tripal_chado_install_chado($action, $chado_schema = 'chado', TripalJob $job = NULL) {

  if ($action == 'Install Chado v1.3') {
    \Drupal::service('tripal_chado.chadoInstaller', $chado_schema)
      ->install(1.3);
  }
  else {
    \Drupal::logger('tripal_chado')->error("NOT SUPPORTED: " . $action);
  }
}
