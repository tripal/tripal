<?php
namespace Drupal\tripal\Controller;
use Drupal\Core\Controller\ControllerBase;

/**
 * Controller routines for the Tripal Module
 */
class TripalImporterPermissionsController extends ControllerBase{


  public function permissions() {
    $permissions = [];

    $importer_manager = \Drupal::service('tripal.importer');
    $importer_defs = $importer_manager->getDefinitions();

    foreach ($importer_defs as $plugin_id => $def) {
      $plugin_label = $def['label']->getUntranslatedString();
      $permissions['use ' . $plugin_id . ' importer'] = [
        'title' => t('Tripal Importer: Use the %label', ['%label' => $plugin_label]),
        'description' => t('Allow the user to import data using the %label.  Note: you may also need to give the "Upload Files" permission for importers to work.', ['%label' => $plugin_label]),
        'restrict access' => TRUE,
      ];
    }

    return $permissions;
  }

}
