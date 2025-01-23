<?php
namespace Drupal\tripal\Services;

use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager;
use Drupal\field\Entity\FieldConfig;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Provides an tripalStorage plugin manager.
 */
class TripalFieldCollection implements ContainerInjectionInterface  {

  /**
   * The IdSpace service
   *
   * @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idSpaceManager
   */
  protected $idSpaceManager;

  /**
   * The vocabulary service
   *
   * @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalVocabularyManager $vocabularyManager
   */
  protected $vocabularyManager;

  /**
   * A logger object.
   *
   * @var TripalLogger $logger
   */
  protected $logger;

  /**
   * Constructor
   */
  public function __construct(TripalIdSpaceManager $idSpaceManager,
      TripalVocabularyManager $vocabularyManager, TripalLogger $logger) {

    $this->idSpaceManager = $idSpaceManager;
    $this->vocabularyManager = $vocabularyManager;
    $this->logger = $logger;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static (
      $container->get('tripal.collection_plugin_manager.idspace'),
      $container->get('tripal.collection_plugin_manager.vocabulary'),
      $container->get('tripal.logger')
    );
  }

  /**
   * Adds default values for keys in the field definition array.
   *
   * This function will only add defaults if the value is not already present
   * in the $field_def array. You can retrieve a fully populated definition
   * array, with defaults, by not passing an argument.  This function will
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
    $new_defs['content_type'] = '';
    if (array_key_exists('content_type', $field_def) and !empty($field_def['content_type'])) {
      $new_defs['content_type'] = $field_def['content_type'];
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

    // Determine the field class.
    $field_types = \Drupal::service('plugin.manager.field.field_type');
    $field_type_def = $field_types->getDefinition($new_defs['type']);
    $field_class = $field_type_def['class'];

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

    // Storage Settings
    // Get the defaults for the storage setting for this field type.
    $default_storage_settings = $field_class::defaultStorageSettings();

    $new_defs['storage_settings']['storage_plugin_id'] = '';
    $new_defs['storage_settings']['storage_plugin_settings'] = [];
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

    // Copy the termIdSpace and termAccession to the storage settings.
    $new_defs['storage_settings']['termIdSpace'] = $new_defs['settings']['termIdSpace'];
    $new_defs['storage_settings']['termAccession'] = $new_defs['settings']['termAccession'];

    // View Display Settings
    // Available view settings: label, weight @todo add more
    // Available form settings: 'region' @todo add more
    $new_defs['display']['view']['default'] = [
      'region' => 'content',
      'label' => 'above',
      'weight' => 10
    ];
    $new_defs['display']['view']['teaser'] = [
      'label' => 'hidden',
    ];
    $new_defs['display']['form']['default'] = [
      'region' => 'content',
      'weight' => 10
    ];

    if (array_key_exists('display', $field_def)) {
      foreach ($field_def['display'] as $display_type => $view_modes) {
        foreach ($view_modes as  $view_mode => $mode_config) {
          foreach ($mode_config as $setting_name => $value) {
            // Make sure the label is an allowed value.
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
   * Discovers new fields for a given entity type.
   *
   * Fields can be specified in one of 2 ways.  Using the installation method
   * where they are specified by a YML file created by the module developer or
   * by fields that have a discover() function implemented. This function
   * supports adding new fields to the collection using the discover approach.
   *
   * @param \Drupal\tripal\Entity\TripalEntityType $tripal_entity_type
   *   The object representing the bundle.
   */
  public function discover(TripalEntityType $tripal_entity_type) {
    $bundle_name = $tripal_entity_type->id();

    // Holds the status of each field (e.g., skipped, added, etc.)
    $field_status = [
      'invalid' => [],
      'new' => [],
      'existing' => [],
    ];

    // Get all of the fields and call the `discover()` method for each one.
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');
    /** @var \Drupal\Core\Entity\EntityFieldManager $entity_field_manager **/
    $entity_field_manager = \Drupal::service('entity_field.manager');

    $all_field_defs = $field_type_manager->getDefinitions();
    $entity_field_defs = $entity_field_manager->getFieldDefinitions('tripal_entity', $bundle_name);
    $field_names = [];
    foreach ($all_field_defs as $field_id => $field_def) {
      $field_class = $field_def['class'];
      if (is_subclass_of($field_class, 'Drupal\tripal\TripalField\TripalFieldItemBase')) {
        $discovered = $field_class::discover($tripal_entity_type, $field_id, $all_field_defs, $entity_field_defs);
        foreach ($discovered as $discovered_field) {
          $discovered_field_name = $discovered_field['name'];
          $field_names[$discovered_field_name] = ($field_names[$discovered_field_name] ?? 0) + 1;

          // If the CV term for the discovered field is currently used by an
          // existing field, then mark it as existing.
          $discoveredIdSpace = $discovered_field['settings']['termIdSpace'];
          $discoveredAccession = $discovered_field['settings']['termAccession'];
          $existing_field_name = $this->checkDiscoveredTerm($discoveredIdSpace, $discoveredAccession, $entity_field_defs);
          if ($existing_field_name) {
            // Note that the existing name may differ from the name proposed by discover()
            $discovered_field['name'] = $existing_field_name;
            $field_status['existing'][$existing_field_name] = $discovered_field;
            // We may have already added a new field with this existing field's name
            // earlier in the loop. If so, go back and update that new field.
            $this->recheckTerms($field_status, $existing_field_name);
            continue;
          }

          // If the field is not valid then skip it.
          $reason = '';
          $is_valid = $this->validate($discovered_field, $reason);
          if (!$is_valid) {
            $field_status['invalid'][$discovered_field_name] = $discovered_field;
            $field_status['invalid'][$discovered_field_name]['invalid_reason'] = $reason;
            continue;
          }

          // If the field name already exists, then adjust it to be unique
          if ($field_names[$discovered_field_name] > 1) {
            $discovered_field['name'] = $this->updateFieldName($discovered_field);
          }
          $field_status['new'][$discovered_field_name] = $discovered_field;
        }
      }
    }
    return $field_status;
  }

  /**
   * Updates a field name if it is already used for a different field.
   *
   * @param string $discovered_field
   *   Current field with duplicated name
   *
   * @return string
   *   The updated field name
   */
  protected function updateFieldName(array $discovered_field): string {
    $max_length = FieldStorageConfig::NAME_MAX_LENGTH;
    // CV term id is currently only available for property fields
    $suffix = strtolower($discovered_field['cvterm_id'] ?? $discovered_field['termIdSpace'] ?? uniqid());
    $field_name = $discovered_field['name'] . '_' . $suffix;
    // If name is now longer than Drupal allows, shorten the original
    // name before adding the suffix.
    if (mb_strlen($field_name) > $max_length) {
      $truncate_to = $max_length - strlen($suffix) - 1;
      $field_name = mb_substr($discovered_field['name'], 0, $truncate_to) . '_' . $suffix;
    }
    $field_name = preg_replace('/[^\w]/u', '_', $field_name);
    return $field_name;
  }

  /**
   * If a new field's name conflicts with an existing one, update
   * the new field's name to eliminate the duplication.
   *
   * @param array &$field_status
   *   Holds the status of each field. Will be updated if necessary.
   * @param string $field_name
   *   Name of one existing field to check against
   * @return void
   */
  protected function recheckTerms(array &$field_status, string $field_name) {
    if (array_key_exists($field_name, $field_status['new'])) {
      $def = $field_status['new'][$field_name];
      $new_name = $this->updateFieldName($def);
      $def['name'] = $new_name;
      $field_status['new'][$new_name] = $def;
      unset($field_status['new'][$field_name]);
    }
  }

  /**
   * Provides a compatible array for a field based on it's Field Config.
   *
   * This is to be used in the discover process to add existing fields to the
   * field list in a standardized way.
   *
   * @param FieldConfig $instance
   *   The existing field to generate an array for so it can be added to the
   *   field list as an existing field.
   * @return array
   *   An array which matches the form expected to be a single item in the
   *   discover method.
   */
  public static function getFieldArrayFromFieldInstance(FieldConfig $instance) {
    $field_storage = $instance->getFieldStorageDefinition();
    $settings = $instance->getSettings();
    return [
      'name' => $instance->getName(),
      'content_type' => $instance->getTargetBundle(),
      'label' => $instance->getLabel(),
      'type' => $instance->getType(),
      'description' => $instance->getDescription(),
      'cardinality' => $field_storage->getCardinality(),
      'required' => $instance->isRequired(),
      'storage_settings' => [
        'storage_plugin_id' => $settings['storage_plugin_id'],
        'storage_plugin_settings' => $settings['storage_plugin_settings'],
      ],
      'settings' => $settings,
    ];
  }

  /**
   * Helper function to determine if any existing fields use a particular CV term.
   *
   * @param string $idSpace
   *   ID space of term we want to check
   * @param string $accession
   *   Accession of term we want to check
   * @param array $entity_field_defs
   *   Array of field definitions returned from getFieldDefinitions()
   *
   * @return string
   *   The name of the field if term already used by a field, empty string otherwise
   */
  private function checkDiscoveredTerm($idSpace, $accession, $entity_field_defs) {
    $existing = '';
    foreach ($entity_field_defs as $name => $def) {
      $settings = $def->getSettings();
      if ( (($settings['termIdSpace'] ?? '') == $idSpace)
          and (($settings['termAccession'] ?? '') == $accession) ) {
        $existing = $name;
        break;
      }
    }
    return $existing;
  }

  /**
   * Validates a field definition array.
   *
   * This function can be used to check a field definition prior to adding
   * the field to a Tripal content type.
   *
   * @param array $field_def
   *   A definition array for the field.
   * @param string $reason
   *   The reason for why a field is invalid. Set only if the function returns
   *   FALSE.
   * @return bool
   *   True if the array passes validation checks. False otherwise.
   */
  public function validate($field_def, string &$reason = '') : bool {

    if (!array_key_exists('content_type', $field_def)) {
      $reason = 'The field is missing the "content_type" property.';
      $this->logger->error($reason);
      return FALSE;
    }
    // Check if the type already exists.
    $entityTypes = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['id' => $field_def['content_type']]);
    if (empty($entityTypes)) {
      $reason = 'The specified entity type, "' . $field_def['content_type'] . '", for field "' . $field_def['name'] . '", does not exist.';
      $this->logger->error($reason);
      return FALSE;
    }
    if (!array_key_exists('name', $field_def) or empty($field_def['name'])) {
      $reason = 'The field is missing the "name" property.';
      $this->logger->error($reason);
      return FALSE;
    }
    if (!array_key_exists('content_type', $field_def) or empty($field_def['content_type'])) {
      $reason = 'The field is missing the "content_type" property.';
      $this->logger->error($reason);
      return FALSE;
    }
    if (!array_key_exists('type', $field_def) or empty($field_def['type'])) {
      $reason = 'The field is missing the "type" property.';
      $this->logger->error($reason);
      return FALSE;
    }
    else {
      $field_types = \Drupal::service('plugin.manager.field.field_type')->getDefinitions();
      if (!in_array($field_def['type'], array_keys($field_types))) {
        $reason = 'The field type, "' . $field_def['type'] . '", is not a valid field type.';
        $this->logger->error($reason);
        return FALSE;
      }
    }
    if (!array_key_exists('storage_settings', $field_def) or empty($field_def['storage_settings'])) {
      $reason = 'The field is missing the "storage_settings" property';
      $this->logger->error($reason);
      return FALSE;
    }
    if (!array_key_exists('storage_plugin_id', $field_def['storage_settings'])) {
      $reason = 'The field is missing the "storage_plugin_id" of the "storage_settings" property';
      $this->logger->error($reason);
      return FALSE;
    }
    if (!array_key_exists('termIdSpace', $field_def['settings'])) {
      $reason = 'The field is missing the "termIdSpace" of the "settings" property';
      $this->logger->error($reason);
      return FALSE;
    }
    if (!array_key_exists('termAccession', $field_def['settings'])) {
      $reason = 'The field is missing the "termAccession" of the "settings" property';
      $this->logger->error($reason);
      return FALSE;
    }
    if (empty($field_def['storage_settings']['storage_plugin_id'])) {
      // @todo verify the name of the plugin is a real plugin.
      $reason = 'You must set the "storage_plugin_id" property.';
      $this->logger->error($reason);
      return FALSE;
    }

    // Make sure the ID space exists. If it is not yet a collection, and a
    // plugin is specified, then we can create it.
    $collection_plugin_id = $field_def['settings']['id_space_plugin_id'] ?? '';
    /** @var Drupal\tripal_chado\Plugin\TripalIdSpace\ChadoIdSpace */
    $idSpace = $this->idSpaceManager->loadCollection($field_def['settings']['termIdSpace'], $collection_plugin_id);
    if (!$idSpace) {
      $reason = t('The term Id Space "@idspace" is not known. Check the "termIdSpace" element.',
          ['@idspace' => $field_def['settings']['termIdSpace']]);
      $this->logger->error($reason);
      return FALSE;
    }
    $term = $idSpace->getTerm($field_def['settings']['termAccession']);
    if (!$term) {
      $reason = t('The term accession. "@id:@accession", is not known in the Term Id Space for field, "@field". Check the "termIdSpace" and "termAccession" elements.',
        ['@id' => $field_def['settings']['termIdSpace'],
          '@accession' => $field_def['settings']['termAccession'],
          '@field' => $field_def['name']
        ]);
      $this->logger->error($reason);
      return FALSE;
    }

    // Verify that the term for this field is not already used by another field.
    // To do this we check if any fields on this content type have a field with
    // the same term and that any existing field is not the same field we are
    // validating.
    $existing_terms = $this->getExistingFieldTerms($field_def['content_type']);
    $new_term = $field_def['settings']['termIdSpace'] . ':' . $field_def['settings']['termAccession'];
    if (array_key_exists($new_term, $existing_terms) && ($existing_terms[$new_term] !== $field_def['name'])) {
      $reason = t('The term "@new_term" for field "@name" in bundle "@bundle" is already'
          . ' being used by another field, so this field cannot be added.',
          ['@new_term' => $new_term, '@name' => $field_def['name'], '@bundle' => $field_def['content_type']]);
      $this->logger->error($reason);
      return FALSE;
    }

    return True;
  }

  /**
   * Returns a list of field terms for all fields on the specified
   * bundle, including non-chado fields.
   *
   * @param string $bundle
   *   The bundle id, e.g. 'project', 'analysis', etc.
   *
   * @return array
   *   An associative array of existing terms. The key is the term
   *   stored as 'IdSpace:Accession', the value is the name of the field.
   */
  protected static function getExistingFieldTerms(string $bundle) {
    $existing_terms = [];
    /** @var \Drupal\Core\Entity\EntityFieldManager $field_manager **/
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $bundle);
    foreach ($field_defs as $field_name => $field_definition) {
      $settings = $field_definition->getSettings();
      if (array_key_exists('termIdSpace', $settings)) {
        $term = $settings['termIdSpace'] . ':' . $settings['termAccession'];
        $existing_terms[$term] = $field_name;
      }
    }
    return $existing_terms;
  }

  /**
   * Attaches fields to Tripal content types.
   *
   * @param array $collection_ids
   *   An array of the collection 'id' you would like to install.
   */
  public function install(array $collection_ids) {
    $yaml_prefix = 'tripal.tripalfield_collection.';

    $config_factory = \Drupal::service('config.factory');

    foreach ($collection_ids as $config_id) {

      $config_item = $yaml_prefix . $config_id;
      $config = $config_factory->get($config_item);

      if (is_object($config)) {
        $label = $config->get('label');
        $this->logger->notice("Attaching fields to Tripal content types from: " . $label);
        $fields = $config->get('fields');

        // Iterate through each field in the config file.
        foreach ($fields as $field) {
          $this->addBundleField($field);
        }
      }
      else {
        throw new \Exception("Unable to retrieve the configuration with an id of $config_id using the assumption that it's in the file $config_item.");
      }
    }
  }

  /**
   * Adds a field to a Tripal entity type.
   *
   * @param string $bundle
   *   The bundle name (e.g. organism).
   * @param array $field_def
   *   An associative array providing the necessary information about a field
   *   instance for this entity type. The following key/values are supported
   *   - name: (string) The machine-readable name for this field.
   *   - content_type: (string) The machine-readable name of the content type.
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
   *       plugin expects for the field.
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
   * $fields_service = \Drupal::service('tripal.fields');
   * $field_def = [
   *   'name' => 'organism_genus',
   *   'content_type' => 'organism',
   *   'label' => 'Genus',
   *   'type' => 'tripal_string_type',
   *   'description' => "The genus name of the organism.",
   *   'cardinality' => 1,
   *   'required' => TRUE,
   *   'storage_settings' => [
   *     'storage_plugin_id' => 'drupal_sql_storage',
   *     'storage_plugin_settings'=> [
   *     ],
   *     'max_length' => 255,
   *   ],
   *   'settings' => [
   *     'termIdSpace' => 'TAXRANK',
   *     'termAccession' => "0000005",
   *   ],
   *   'display' => [
   *     'view' => [
   *       'default' => [
   *         'region' => 'content',
   *         'label' => 'above',
   *         'weight' => 15
   *       ],
   *     ],
   *     'form' => [
   *       'default'=> [
   *         'region'=> 'content',
   *         'weight' => 15
   *       ],
   *     ],
   *   ],
   * ];
   * $fields_service->addBundleField($field_def);
   * @endcode
   *
   * @return bool
   *   True if the field was added successfully. False otherwise.
   */
  public function addBundleField($field_def) : bool {

    // Make sure the field definition is valid.
    if (!$this->validate($field_def)) {
      return FALSE;
    }

    // Get the entitytype
    $entity_types = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['id' => $field_def['content_type']]);
    $entity_type = array_pop($entity_types);

    // Set defaults for the field if they are not already set.
    $field_def = $this->setFieldDefDefaults($field_def);

    // Get the bundle and field id.
    $field_id = 'tripal_entity' . '.' . $entity_type->id() . '.' . $field_def['name'];

    try {

      // Check if field storage exists for this field. If not, add it.
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
          'bundle' => $field_def['content_type'],
          'label' => $field_def['label'],
        ]);
        $field->setLabel($field_def['label']);
        $field->setDescription($field_def['description']);
        $field->setRequired($field_def['required']);
        $field->setTranslatable($field_def['translatable']);
        $field->setSettings($field_def['settings']);
        $field->save();

        // If the additional type field specifies a fixed_value, this
        // is used to define the bundle type. Add this setting directly
        // to the entity for efficient access when publishing.
        if ($field_def['type'] == 'chado_additional_type_type_default') {
          if (!$entity_type->getThirdPartySetting('tripal', 'bundle_type_column')) {
            $fixed_value = $field_def['settings']['fixed_value'] ?? NULL;
            if ($fixed_value) {
              $storage_plugin_settings = $field_def['storage_settings']['storage_plugin_settings'] ?? [];
              if (array_key_exists('type_table', $storage_plugin_settings) and array_key_exists('type_column', $storage_plugin_settings)) {
                $entity_type->setThirdPartySetting('tripal', 'bundle_type_table', $storage_plugin_settings['type_table']);
                $entity_type->setThirdPartySetting('tripal', 'bundle_type_column', $storage_plugin_settings['type_column']);
                $entity_type->save();
              }
            }
          }
        }

        // Add field to the default display modes.
        $entity_display = \Drupal::service('entity_display.repository');
        $bundle_id = $entity_type->id();
        $view_modes = $entity_display->getViewModeOptionsByBundle('tripal_entity', $bundle_id);
        foreach (array_keys($view_modes) as $view_mode) {
          $view_mode_options = $field_def['display']['view'][$view_mode] ?? [];
          \Drupal::service('entity_display.repository')
            ->getViewDisplay('tripal_entity', $bundle_id, $view_mode)
            ->setComponent($field_def['name'], $view_mode_options)
            ->save();
        }
        $form_modes = $entity_display->getFormModeOptionsByBundle('tripal_entity', $bundle_id);
        foreach (array_keys($form_modes) as $form_mode) {
          $form_mode_options = $field_def['display']['form'][$form_mode] ?? [];
          \Drupal::service('entity_display.repository')
            ->getFormDisplay('tripal_entity', $bundle_id, $form_mode)
            ->setComponent($field_def['name'], $form_mode_options)
            ->save();
        }

        $this->logger->notice(t('Added field, "@field", to content type: "@type".',
            ['@field' => $field_def['name'], '@type' => $entity_type->id()]));
      }
      else {
        $this->logger->notice(t('Skipping addition of field, "@field", to content type: "@type" as it is already added.',
            ['@field' => $field_def['name'], '@type' => $entity_type->id()]));
      }
    }
    catch (\Exception $e) {
      print_r([$e->getMessage()]);
      $this->logger->error(t('Error adding field, "@field_name", to "@bundle": @error', [
         '@field_name' => $field_def['name'],
         '@bundle' => $entity_type->id(),
         '@error' => $e->getMessage(),
      ]));
      return FALSE;
    }
    return TRUE;
  }
}
