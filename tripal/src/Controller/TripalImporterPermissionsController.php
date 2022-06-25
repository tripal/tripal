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
      $permissions['allow tripal importer ' . $plugin_id] = [
        'title' => t('Use Tripal ' . $def['label']->getUntranslatedString()),
        'description' => [
          '#prefix' => '<em>',
          '#markup' => t('Warning: This permission may have security implications since it can alter database tables '
           . t(' and data.')
          ),
          '#suffix' => '</em>',
        ],
      ];
    }

    return $permissions;
  }

}

?>