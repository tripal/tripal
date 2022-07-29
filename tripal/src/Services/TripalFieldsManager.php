<?php
namespace Drupal\tripal\Services;

use Drupal\tripal\Entity\TripalEntity;
use Drupal\field\Entity\FieldConfig;
use Drupal\field\Entity\FieldStorageConfig;

/**
 * Provides an tripalStorage plugin manager.
 */
class TripalFieldsManager {

  /**
   * Constructor
   */
  public function __construct() {

  }

  /**
   * Adds default values for keys in the field definition array.
   *
   * This function will only add defaults if the value is not already present
   * in the $field_def array. You can retrieve a fully populated definition
   * array, with derfaults, by not passing an argument.  This function will
   * remove any keys in the definition array that are not supported.
   *
   * @return array $field_def
   *   Optional. A field definition array to which any default values should
   *   be added.
   */
  public function setFieldDefDefaults(array $field_def = []) : array {
    $new_defs = [];

    $new_defs['name'] = 'tripal_entity.change.me';
    if (array_key_exists('name', $field_def) and !empty($field_def['name'])) {
      $new_defs['name'] = $field_def['name'];
    }
    $new_defs['label'] = $new_defs['name'];
    if (!array_key_exists('label', $field_def) and !empty($field_def['label'])) {
      $new_defs['label'] = $field_def['label'];
    }
    $new_defs['type'] = 'tripal_string_type';
    if (array_key_exists('type', $field_def) and !empty($field_def['type'])) {
      $new_defs['type'] = $field_def['type'];
    }
    $new_defs['description'] = '';
    if (array_key_exists('description', $field_def) and !empty($field_def['description'])) {
      $new_defs['description'] = $field_def['description'];
    }
    $new_defs['required'] = False;
    if (array_key_exists('required', $field_def) and $field_def['required'] === True) {
      $new_defs['required'] = True;
    }
    $new_defs['revisionable'] = False;
    if (array_key_exists('revisionable', $field_def) and $field_def['revisionable'] === True) {
      $new_defs['revisionable'] = True;
    }
    $new_defs['translatable'] = False;
    if (array_key_exists('translatable', $field_def) and $field_def['translatable'] === True) {
      $new_defs['translatable'] = True;
    }
    $new_defs['cardinality'] = -1;
    if (array_key_exists('cardinality', $field_def) and is_int($field_def['cardinality'])) {
      $new_defs['cardinality'] = $field_def['cardinality'];
    }

    // Storage Settings
    // Get the defaults for the storage setting for this field type.
    $field_types = \Drupal::service('plugin.manager.field.field_type');
    $field_type_def = $field_types->getDefinition($new_defs['type']);
    $default_storage_settings = $field_type_def['class']::defaultStorageSettings();
    $new_defs['storage_settings'] = [];
    foreach ($default_storage_settings as $setting_name => $value) {
      $new_defs['storage_settings'][$setting_name] = $value;
    }

    // Now copy over any user-provided settings if they are supported.
    if (array_key_exists('storage_settings', $field_def)) {
      foreach ($field_def['storage_settings'] as $setting_name => $value) {
        if (array_key_exists($setting_name, $new_defs['storage_settings'])) {
          $new_defs['storage_settings'][$setting_name] = $value;
        }
      }
    }

    // Field Settings
    // Get the defaults for the storage setting for this field type.
    $new_defs['settings']['tripal_term'] = '';
    $default_storage_settings = $field_type_def['class']::defaultFieldSettings();
    $new_defs['settings'] = [];
    foreach ($default_storage_settings as $setting_name => $value) {
      $new_defs['settings'][$setting_name] = $value;
    }

    // Now copy over any user-provided settings if they are supported.
    if (array_key_exists('settings', $field_def)) {
      foreach ($field_def['settings'] as $setting_name => $value) {
        if (array_key_exists($setting_name, $new_defs['settings'])) {
          $new_defs['settings'][$setting_name] = $value;
        }
      }
    }

    return $new_defs;
  }

  /**
   *
   * @param array $field_def
   * @param bool $set_defaults
   * @return bool
   */
  public function validateFieldDef(array $field_def) : bool {
    $logger = \Drupal::service('tripal.logger');

    if (!array_key_exists('name', $field_def)) {
      $logger->error('Cannot add to an entity type without providing a field name.');
      return False;
    }
    if (!array_key_exists('type', $field_def)) {
      $logger->error('Cannot add to an entity type without providing a field type.');
      return False;
    }
    return True;
  }

  /**
   * Adds a field to a Tripal entity type.
   *
   * @param string $bundle
   *   The bundle name (e.g. bio_data_1).
   * @param array $field_def
   *   An associative array providing the necessary information about a field
   *   instance for this entity type. The following key/values are supported
   *   - name: (string) The machine-readable name for this field.
   *   - type: (string) The field type
   *   - label: (string) The default label for the field.
   *   - idSpace: (string) The name of the ID space for this field.
   *   - term: (string) The controlled vocabulary term accession for this field.
   *   - required:  (bool) True if the field is required. False otherwise. If not
   *     set then defaults to False.
   *   - cardinality: (int) Set to -1 for unlimited or any number.
   *
   * @return bool
   *   True if the field was added successfully. False otherwise.
   */
  public function addBundleField(string $bundle, array $field_def) : bool {
    $logger = \Drupal::service('tripal.logger');

    if (!$this->validateFieldDef($field_def)) {
      return False;
    }
    $field_def = $this->setFieldDefDefaults($field_def);
    print_r($field_def);
    $field_id = 'tripal_entity' . '.' . $bundle . '.' . $field_def['name'];

    try {

      // Check if field storage exists for this field. If not, add it..
      $field_storage = FieldStorageConfig::loadByName('tripal_entity', $field_def['name']);
      if (!$field_storage) {
        $field_storage = FieldStorageConfig::create([
          'field_name' => $field_def['name'],
          'entity_type' => 'tripal_entity',
          'type' => $field_def['type'],
          'cardinality' => $field_def['cardinality'],
          'revisionable' => $field_def['revisionable'],
          'provider' => 'tripal',
          'settings' => $field_def['storage_settings']
        ]);
        $field_storage->save();
      }

      // If the field doesn't already exist for this bundle then add it.
      $field = FieldConfig::load($field_id);
      if (!$field instanceof FieldConfig) {
        $field = FieldConfig::create([
          'field_storage' => $field_storage,
          'bundle' => $bundle,
          'label' => $field_def['label'],
        ]);
        $field->setLabel($field_def['label']);
        $field->setDescription($field_def['description']);
        $field->setRequired($field_def['required']);
        $field->setTranslatable($field_def['translatable']);
        $field->setSettings($field_def['settings']);
        $field->save();
      }


      // Finally set display options
      // @todo add these.

    }
    catch (\Exception $e) {
      $logger->error(t('Error adding field @field_name to @bundle:<br>@error', [
        '@field_name' => $field_def['name'],
        '@bundle' => $bundle,
        '@error' => $e->getMessage(),
      ]));
      return False;
    }
    return True;
  }
}