<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Config\FileStorage;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Extension\ModuleExtensionList;
use Drupal\Core\Extension\ModuleHandlerInterface;

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
   * The Drupal module handler service.
   *
   * @var Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $module_handler;

  /**
   * Constructs a new TripalRebuildService object.
   *
   * @param Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The drupal entity type manager service.
   * @param Drupal\Core\Extension\ModuleExtensionList $module_extension_list
   *   The drupal module extension list service.
   * @param Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The drupal module handler service.
   */
  public function __construct(
    EntityTypeManagerInterface $entity_type_manager,
    ModuleExtensionList $module_extension_list,
    ModuleHandlerInterface $module_handler,
  ) {
    $this->entity_type_manager = $entity_type_manager;
    $this->module_extension_list = $module_extension_list;
    $this->module_handler = $module_handler;
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

    // Load default views for any tripal content types.
    $this->rebuildViews();
  }

  /**
   * Used to load default drupal views for tripal content types.
   *
   * This allows us to have default views for content types that may not
   * exist upon module install, but are created later. For this reason the
   * yaml files for these views are stored in the config/optional directory.
   */
  public function rebuildViews() {
    $view_storage = $this->entity_type_manager->getStorage('view');
    $file_storage = new FileStorage(DRUPAL_ROOT);
    $module_list = $this->module_handler->getModuleList();
    $content_types = $this->entity_type_manager
      ->getStorage('tripal_entity_type')
      ->loadMultiple();
    foreach ($content_types as $content_type) {
      $view_id = 'tripal_entity_' . $content_type->id();
      $view = $storage->load($view_id);
      if (!$view) {
        foreach ($module_list as $extension) {
          $module_path = $extension->getPath();
          $yaml_path = $module_path . '/config/optional/views.view.' . $view_id;
          $config = $file_storage->read($yaml_path);
          if ($config) {
            $view = $view_storage->create($config);
            $view->save();
          }
        }
      }
    }
  }

}
