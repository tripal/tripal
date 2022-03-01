<?php
namespace Drupal\tripal\Controller;
use Drupal\Core\Controller\ControllerBase;

/**
 * Controller routines for the Tripal Module
 */
class TripalImporterPermissionsController extends ControllerBase{


  /**
   * Constructs the TripalController.
   *
   */
  public function __construct() {

  }

  public function permissions() {
    $permissions = [];
    $importers = tripal_get_importers();
    foreach ($importers as $class_name) {
      tripal_load_include_importer_class($class_name);
      if (class_exists($class_name)) {
        $machine_name = $class_name::$machine_name;  
        $name = $class_name::$name;  
        $permissions['allow tripal importer ' . $machine_name] = [
          'title' => t('Use Tripal ' . $name),
          'description' => [
            '#prefix' => '<em>',
            '#markup' => t('Warning: This permission may have security implications since it can alter database tables '
             . t(' and data.')
            ),
            '#suffix' => '</em>',
          ],
        ];
      }
    }

    return $permissions;
  }

}

?>