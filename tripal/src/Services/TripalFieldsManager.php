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
    if (array_key_exists('label', $field_def) and !empty($field_def['label'])) {
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
    $field_class = $field_type_def['class'];
    $default_storage_settings = $field_class::defaultStorageSettings();
    $new_defs['storage_settings'] = [];
    $new_defs['storage_settings']['storage_plugin_id'] = '';
    $new_defs['storage_settings']['storage_plugin_settings'] = [
      // The properties should be specific to the storage back-end so no
      // defaults are set.
      'property_settings' => [],
      // Copy the cardinality and required values for Drupal into the storage
      // settings for the Tripal field.
      'cardinality' => $new_defs['cardinality'],
      'required' => $new_defs['required']
    ];
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
    // Available settings include: "name", "label", "termIdSpace",
    // "termAccession", "help_text", "category", "title_format",
    // "url_format", "hide_empty_field", "ajax_field".
    // Get the defaults for the storage setting for this field type.
    $new_defs['settings']['termIdSpace'] = '';
    $new_defs['settings']['termAccession'] = '';
    $default_storage_settings = $field_class::defaultFieldSettings();
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

    // View Display Settings
    // Available view settings: label, weight @todo add more
    // Available form settings: 'region' @todo add more
    $new_defs['display']['view']['default'] = [
      'label' => 'above',
      'weight' => 10
    ];
    $new_defs['display']['view']['teaser'] = [
      'label' => 'hidden',
    ];
    $new_defs['display']['form']['default'] = [
      'region' => 'content',
    ];

    if (array_key_exists('display', $field_def)) {
      foreach ($field_def['display'] as $display_type => $view_modes) {
        foreach ($view_modes as  $view_mode => $mode_config) {
          foreach ($mode_config as $setting_name => $value) {
            // Maake sure the label is an allowed value.
            if ($setting_name == 'label') {
              if (in_array($value, ['above', 'inline', 'hidden', 'visually_hidden'])) {
                $new_defs['display'][$display_type][$view_mode][$setting_name] = $value;
              }
            }
            // Copy all other values as is.
            else {
              $new_defs['display'][$display_type][$view_mode][$setting_name] = $value;
            }
          }
        }
      }
    }

    return $new_defs;
  }

  /**
   * Validates a field definition array.
   *
   * This function can be used to check a field definition prior to adding
   * the field to a bundle.  It is also called automatically prior to adding
   * a new field to a content type.
   *
   * @param array $field_def
       A field definition array to which any default values should
   *   be added.
   * @return bool
   *   True if the array passes validation checks. False otherwise.
   */
  public function validateFieldDef(array $field_def) : bool {
    $logger = \Drupal::service('tripal.logger');

    if (!array_key_exists('name', $field_def)) {
      $logger->error('The field is missing the "name" property.');
      return False;
    }
    if (!array_key_exists('type', $field_def)) {
      $logger->error('The field is missing the "type" property.');
      return False;
    }
    if (!array_key_exists('storage_settings', $field_def)) {
      $logger->error('The field is missing the "storage_settings" property');
      return False;
    }
    if (!array_key_exists('storage_plugin_id', $field_def['storage_settings'])) {
      $logger->error('The field is missing the "storage_plugin_id" of the "settings" property');
      return False;
    }
    if (empty($field_def['storage_settings']['storage_plugin_id'])) {
      // @todo verify the name of the plugin is a real plugin.
      $logger->error('You must set the "storage_plugin_id" property.');
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
   *   - required:  (bool) True if the field is required. False otherwise. If
   *     not set then defaults to False.
   *   - cardinality: (int) Set to -1 for unlimited or any number.
   *   - storage_settings: (array) An array of settings specific to storage
   *     by the storage back-end. It must contain the following keys:
   *     - storage_plugin_id: the name of the storage plugin
   *       (e.g. 'chado_storage').
   *     - storage_plugin_setings: an array of any settings that the storage
   *       plugin expects for the field..
   *   - settings: (array) Any other settings needed for the field. Every
   *     field can have different settings.
   *   - display: Provides details for display of the field. By default it
   *     should provide the following keys:
   *     - view: an array of settings for the "view" display.  The keys of this
   *       array should be the names of the available view modes. By deafult it
   *       should always provide a 'default' key.  Each display mode can
   *       then have the following key/value pairs:
   *       - weight: indicates the weight (or position) of the field in the
   *         display.
   *       - region: the name of the region where the field should be placed. By
   *         default there are two regions: 'content' or 'hidden'.  If the field
   *         should not be visible by default use 'hidden'.
   *       - label:  indicates where on the page the field label should be
   *         placed in relationship to the value. Valid values include 'above',
   *         'inline' or 'hidden'.
   *     - form: an array of settings for the "form" display.  The keys of this
   *       array should be the names of the available form modes. By deafult it
   *       should always provide a 'default' key.  Each display mode can
   *       then have the following key/value pairs:
   *       - weight: indicates the weight (or position) of the field in the
   *         display.
   *       - region: the name of the region where the field should be placed. By
   *         default there are two regions: 'content' or 'hidden'.  If the field
   *         should not be visible by default use 'hidden'.
   *
   * An example field defintion:
   *
   * @code
   * $species = [
   *   'name' => 'taxrank__species',
   *   'label' => 'Species',
   *   'type' => 'tripal_string_type',
   *   'description' => 'The organism species name',
   *   'cardinality' => 1,
   *   'required' => True,
   *   'storage_settings' => [
   *     'max_length' => 255,
   *     'storage_plugin_id' => 'chado_storage',
   *     'storage_plugin_settings' => [
   *       // This setting is specific to the Chado Storage Backend.
   *       'value' => ['store' => 'organism.species']
   *     ],
   *   ],
   *   'settings' => [
   *     'termIdSpace' => 'TAXRANK',
   *     'termAccession' => '0000006',
   *   ],
   *   'display' => [
   *     'view' => [
   *       'default' => [
   *         'region' => 'content',
   *         'label' => 'above',
   *         'weight' => 11,
   *       ],
   *     ],
   *     'form' => [
   *       'default' => [
   *         'region' => 'content',
   *         'weight' => 11,
   *       ],
   *     ],
   *   ],
   * ];
   * $tripal_fields->addBundleField('bio_data_1', $species);
   * @endcode
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
          'settings' => $field_def['storage_settings'],
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

        // Add field to the default display modes.
        $entity_display = \Drupal::service('entity_display.repository');
        $view_modes = $entity_display->getViewModeOptionsByBundle('tripal_entity', $bundle);
        foreach (array_keys($view_modes) as $view_mode) {
          \Drupal::service('entity_display.repository')
            ->getViewDisplay('tripal_entity', $bundle, $view_mode)
            ->setComponent($field_def['name'], $field_def['display']['view'][$view_mode])
            ->save();
        }
        $from_modes = $entity_display->getFormModeOptionsByBundle('tripal_entity', $bundle);
        foreach (array_keys($from_modes) as $form_mode) {
          \Drupal::service('entity_display.repository')
            ->getFormDisplay('tripal_entity', $bundle, $form_mode)
            ->setComponent($field_def['name'], $field_def['display']['form'][$form_mode])
            ->save();
        }
      }
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
