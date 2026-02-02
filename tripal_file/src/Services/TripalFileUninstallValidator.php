<?php

namespace Drupal\tripal_file\Services;

use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Service to assist with uninstalling the tripal_file module.
 */
class TripalFileUninstallValidator {

  use StringTranslationTrait;

  /**
   * The field storage config storage.
   *
   * @var \Drupal\Core\Config\Entity\ConfigEntityStorageInterface
   */
  protected $fieldStorageConfigStorage;

  /**
   * Constructs a new TripalFileUninstallValidator object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager) {
    $this->fieldStorageConfigStorage = $entity_type_manager->getStorage('field_storage_config');
  }

  /**
   * {@inheritdoc}
   */
  public function validate($module) {
    $reasons = [];
    if ($module == 'tripal_file') {
      // Uninstall will be blocked if there are any field storages
      // present for the tripal_file module.
      $uninstall_blocked = FALSE;
      $field_storages = $this->fieldStorageConfigStorage->loadByProperties([
        'module' => $module,
        'include_deleted' => TRUE,
      ]);
      if ($field_storages) {
        foreach ($field_storages as $field_storage) {
          if (!$field_storage->isDeleted()) {
            $uninstall_blocked = TRUE;
          }
        }
      }

      // The offending fields will have been listed by the
      // FieldUninstallValidator class. Here we just want to add a helpful
      // message to tell the user how to fix it. The failing validation
      // will prevent the module uninstall hook from being called, so we
      // can't do this automatically for the user.
      if ($uninstall_blocked) {
        $reasons[] = $this->t('HINT: Before you can uninstall the "tripal_file" module, you must delete the content types that the module has created. Go to Tripal → Page Structure, and delete the two "Tripal File" content types.');
      }
    }
    return $reasons;
  }

}
