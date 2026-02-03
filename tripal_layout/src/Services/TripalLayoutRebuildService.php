<?php

namespace Drupal\tripal_layout\Services;

use Drupal\Core\Config\FileStorage;
use Drupal\Core\Config\InstallStorage;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Extension\ExtensionDiscovery;
use Drupal\Core\Extension\ModuleHandler;

/**
 * Service for handling tripal_layout's rebuild logic.
 */
class TripalLayoutRebuildService {

  /**
   * The Drupal root path.
   *
   * @var string
   */
  protected string $drupal_root;

  /**
   * The Drupal entity type manager service.
   *
   * @var Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected EntityTypeManagerInterface $entity_type_manager;

  /**
   * The Drupal module handler service.
   *
   * @var Drupal\Core\Extension\ModuleHandler
   */
  protected ModuleHandler $module_handler;

  /**
   * Constructs a new TripalLayoutRebuildService object.
   *
   * @param string $root
   *   The drupal root path.
   * @param Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The drupal entity type manager service.
   * @param Drupal\Core\Extension\ModuleHandler $module_handler
   *   The drupal module handler service.
   */
  public function __construct(
    string $root,
    EntityTypeManagerInterface $entity_type_manager,
    ModuleHandler $module_handler,
  ) {
    $this->drupal_root = $root;
    $this->entity_type_manager = $entity_type_manager;
    $this->module_handler = $module_handler;
  }

  /**
   * Executes the rebuild process for tripal_layout.
   */
  public function executeRebuild() {
    $this->tripalLayoutSyncEntities('tripal_layout_default_view');
    $this->tripalLayoutSyncEntities('tripal_layout_default_form');
  }

  /**
   * Synchronizes configuration entities.
   *
   * Specifically,
   *  - scans all module config install directories for YAML.
   *  - creates config entity if the YAML id does not match an existing entity.
   *  - updates the config entity if the YAML id matches an existing entity.
   *
   * This function is analogous to tripal_load_configuration() but focuses on
   * the config entity rather then just the config files.
   *
   * @param string $config_entity_type
   *   The name of the configuration entity, e.g. "tripal_layout_default_view".
   *
   * @return void
   *   No return value.
   */
  public function tripalLayoutSyncEntities(string $config_entity_type): void {

    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_storage **/
    $config_storage = $this->entity_type_manager->getStorage($config_entity_type);
    /** @var \Drupal\Core\Entity\EntityTypeInterface $definition **/
    $definition = $this->entity_type_manager->getDefinition($config_entity_type);

    // Load all the existing entities of this type.
    $existing_entities = $this->entity_type_manager
      ->getStorage($config_entity_type)
      ->loadByProperties([]);

    // Get the list of active modules.
    $active_modules = array_keys($this->module_handler->getModuleList());

    // Iterate through the config/install directory of all installed modules
    // looking for YAML files encoding this config entity type.
    $listing = new ExtensionDiscovery($this->drupal_root);
    $modules = $listing->scan('module');
    foreach ($modules as $module) {
      // Only install configuration if the module is enabled.
      if (in_array($module->getName(), $active_modules)) {
        $extension_path = $module->getPath();
        $config_path = $extension_path . '/' . InstallStorage::CONFIG_INSTALL_DIRECTORY;
        if (is_dir($config_path)) {
          $file_storage = new FileStorage($config_path);
          $configs = $file_storage->listAll($definition->getConfigPrefix());
          foreach ($configs as $config_file) {

            // Now for each YAML file found for our config entity type:
            // -- Read Config from file.
            $current_config = $file_storage->read($config_file);
            // -- Extract the ID of the config entity.
            $parts = explode('.', $config_file);
            $config_entity_id = $parts[2];
            // -- If it matches an existing entity then update it.
            if (array_key_exists($config_entity_id, $existing_entities)) {
              $config_entity = $existing_entities[$config_entity_id];
              $config_entity = $config_storage->updateFromStorageRecord($config_entity, $current_config);
              $config_entity->save();
            }
            // -- If it is new, then create a new entity.
            else {
              $config_entity = $config_storage->createFromStorageRecord($current_config);
              $config_entity->save();
            }
          }
        }
      }
    }
  }

}
