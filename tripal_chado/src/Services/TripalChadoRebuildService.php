<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Config\FileStorage;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Extension\ExtensionPathResolver;
use Drupal\Core\Extension\ModuleExtensionList;

/**
 * Service for handling tripal_chado's rebuild logic.
 */
class TripalChadoRebuildService {

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
   * The Drupal extension path resolver service.
   *
   * @var Drupal\Core\Extension\ExtensionPathResolver
   */
  protected $extension_path_resolver;

  /**
   * Constructs a new TripalChadoRebuildService object.
   *
   * @param Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The drupal entity type manager service.
   * @param Drupal\Core\Extension\ModuleExtensionList $module_extension_list
   *   The drupal module extension list service.
   * @param Drupal\Core\Extension\ExtensionPathResolver $extension_path_resolver
   *   The drupal extension path resolver service.
   */
  public function __construct(
    EntityTypeManagerInterface $entity_type_manager,
    ModuleExtensionList $module_extension_list,
    ExtensionPathResolver $extension_path_resolver,
  ) {
    $this->entity_type_manager = $entity_type_manager;
    $this->module_extension_list = $module_extension_list;
    $this->extension_path_resolver = $extension_path_resolver;
  }

  /**
   * Executes the rebuild process for tripal.
   */
  public function executeRebuild() {

    $this->rebuildViews();
    $this->rebuildChadoTermMappings();

    // Rebuild the Custom Tables and Materialized views views.
    $dir = $this->module_extension_list->getPath('tripal_chado');
    $fileStorage = new FileStorage($dir);

    $custom_tables_config = $fileStorage->read('config/install/views.view.chado_custom_tables');
    $mviews_config = $fileStorage->read('config/install/views.view.chado_mviews');

    $storage = $this->entity_type_manager->getStorage('view');

    // Reload the custom tables view.
    $view = $storage->load('chado_custom_tables');
    $view->delete();
    $view = $storage->create($custom_tables_config);
    $view->save();

    // Reload the materialized view view.
    $view = $storage->load('chado_mviews');
    $view->delete();
    $view = $storage->create($mviews_config);
    $view->save();
  }

  /**
   * Used to recreate views from default.
   *
   * If the user deletes one of the views that are created on install of the
   * Tripal Chado module, then this will restore them when the cache is cleared.
   */
  protected function rebuildViews() {

    // Make sure the Views are present.
    $storage = $this->entity_type_manager->getStorage('view');
    $dir = $this->extension_path_resolver->getPath('module', 'tripal_chado');
    $fileStorage = new FileStorage($dir);

    // The chado_custom_tables view.
    $view = $storage->load('chado_custom_tables');
    if (!$view) {
      $config = $fileStorage->read('config/install/views.view.chado_custom_tables');
      $view = $storage->create($config);
      $view->save();
    }

    // The chado_materialized_views view.
    $view = $storage->load('chado_mviews');
    if (!$view) {
      $config = $fileStorage->read('config/install/views.view.chado_mviews');
      $view = $storage->create($config);
      $view->save();
    }
  }

  /**
   * Used to recreate chado term mappings from default.
   */
  protected function rebuildChadoTermMappings() {

    $storage = $this->entity_type_manager->getStorage('chado_term_mapping');
    $dir = $this->module_extension_list->getPath('tripal_chado');
    $fileStorage = new FileStorage($dir);

    $mapping = $storage->load('core_mapping');
    if (!$mapping) {
      $config = $fileStorage->read('config/install/tripal_chado.chado_term_mapping.core_mapping');
      $mapping = $storage->create($config);
      $mapping->save();
    }

    $storage = $this->entity_type_manager->getStorage('tripal_content_terms');
    $mapping = $storage->load('chado_content_terms');
    if (!$mapping) {
      $config = $fileStorage->read('config/install/tripal.tripal_content_terms.chado_content_terms');
      $mapping = $storage->create($config);
      $mapping->save();
    }
  }

}
