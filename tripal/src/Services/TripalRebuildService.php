<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Config\FileStorage;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Extension\ModuleExtensionList;

/**
 * Service for handling tripal's rebuild logic.
 */
class TripalRebuildService {

  /**
   * The Drupal entity type manager service.
   *
   * @var Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entity_type_manager;

  /**
   * The Drupal extension list service.
   *
   * @var Drupal\Core\Extension\ModuleExtensionList
   */
  protected $module_extension_list;

  /**
   * Constructs a new TripalRebuildService object.
   *
   * @param Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The drupal entity type manager service.
   * @param Drupal\Core\Extension\ModuleExtensionList $module_extension_list
   *   The drupal module extension list service.
   */
  public function __construct(
    EntityTypeManagerInterface $entity_type_manager,
    ModuleExtensionList $module_extension_list,
  ) {
    $this->entity_type_manager = $entity_type_manager;
    $this->module_extension_list = $module_extension_list;
  }

  /**
   * Executes the rebuild process for tripal.
   */
  public function executeRebuild() {

    // These call a Tripal API function.
    tripal_load_configuration('tripal', 'tripalentitytype_collection');
    tripal_load_configuration('tripal', 'tripal_content_terms');
    tripal_load_configuration('tripal', 'tripalfield_collection');

    // Make sure the CustomTables view is present. If not then add it.
    // First get the config file.
    $dir = $this->module_extension_list->getPath('tripal');
    $fileStorage = new FileStorage($dir);
    $config = $fileStorage->read('config/install/views.view.tripal_jobs');

    // Next load the storage system for views entities and check if the view
    // already exists. If it does then don't reload it.
    $storage = $this->entity_type_manager->getStorage('view');
    $view = $storage->load('tripal_jobs');
    if (!$view) {
      $view = $storage->create($config);
      $view->save();
    }

    /* Installs the Manage Pub Search Queries View */
    $config = $fileStorage->read('config/install/views.view.manage_pub_search_queries');

    // Next load the storage system for views entities and check if the view
    // already exists. If it does then don't reload it.
    $storage = $this->entity_type_manager->getStorage('view');
    $view = $storage->load('manage_pub_search_queries');
    if (!$view) {
      $view = $storage->create($config);
      $view->save();
    }
  }

}
