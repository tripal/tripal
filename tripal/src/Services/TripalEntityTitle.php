<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\DependencyInjection\DependencySerializationTrait; //@@@ ???


class TripalEntityTitle {
use DependencySerializationTrait; //@@@ ???

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * A list of the fields and their information.
   *
   * This is to store the field information for fields that are attached
   * to the bundle (entity type) that is being published.
   *
   * @var \Drupal\Core\Field\BaseFieldDefinition $field_definition
   */
  protected $field_info = [];

  /**
   * Stores the bundle (entity type) object.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType $entity_type
   **/
  protected $entity_type = NULL;

  /**
   * The TripalStorage object.
   *
   * @var \Drupal\tripal\TripalStorage\TripalStorageBase $storage
   **/
  protected $storage = NULL;

  /**
   *  A list of property types that are required to uniquely identify an entity.
   *
   * @var array $required_types
   */
  protected $required_types = [];

  /**
   * Supported actions during publishing.
   * Any field containing properties that are not in this list, will not be published!
   *
   * DO NOT YET SUPPORT store_pkey and store_link.
   *
   * @var array $supported_actions
   */
  protected $supported_actions = ['store_id', 'store', 'read_value', 'replace', 'function'];

  /**
   * Keep track of fields which are not supported in order to let the user know.
   *
   * @var array $unsupported_fields
   */
  protected $unsupported_fields;



  /**
   * Retrieves a list of titles for the records in the specified bundle,
   * or a single title if record_id is specified.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage".
   * @param string $bundle
   *   The id of the bundle or entity type, e.g. "contact".
   * @param integer $record_id
   *   Used to limit to one specific record. If null, all matches for
   *   the bundle are returned, which is used for publishing.
   *
   * @return array
   *   A list of titles for the bundle.
   */
  public function getEntityTitles($datastore, $bundle, $record_id = NULL) {
    $matches = $this->findMatches($datastore, $bundle, $record_id);
    $titles = $this->getTitlesFromMatches($matches);
    return $titles;
  }

  /**
   * Retrieves a list of matching records for the specified bundle.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage".
   * @param string $bundle
   *   The id of the bundle or entity type, e.g. "contact".
   * @param integer $record_id
   *   Used to limit to one specific record. If null, all matches for
   *   the bundle are returned, which is used for publishing.
   *
   * @return array
   *   A list of matched records for the specified bundle.
   */
  protected function findMatches($datastore, $bundle, $record_id=NULL) {

    // Get the storage plugin.
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');
    $this->storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
    if (!$this->storage) {
      $error_msg = 'Could not find an instance of the TripalStorage backend: "%datastore".';
      throw new \Exception(t($error_msg, ['%datastore' => $datastore]));
    }

    // Get the bundle object so we can get the title format setting.
    // It will also be used later by replaceTokens().
    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    $entity_type_manager = \Drupal::entityTypeManager();
    /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
    $this->entity_type = $entity_type_manager->getStorage('tripal_entity_type')->load($bundle);
    if (!$this->entity_type) {
      $error_msg = 'Could not find the entity type with an id of: "%bundle".';
      throw new \Exception(t($error_msg, ['%bundle' => $bundle]));
    }

    // Populates the $field_info variable with field information
    $this->setFieldInfo($datastore, $bundle);

    // Get the required field properties that will uniquely identify an entity.
    $this->required_types = $this->storage->getStoredTypes();

    // Build the search values array. This takes time, so use cached version if possible.
    // https://api.drupal.org/api/drupal/core%21core.api.php/group/cache/10
    $cache_id = 'tripalentitytitle:' . $bundle;
//dpm($cache_id, "CPC01 cache_id="); //@@@
    if ($cache = \Drupal::cache()->get($cache_id)) {
      $search_values = $cache->data;
//dpm($search_values, "CPC02 retrieved cached search_values="); //@@@
    }
    else {
      $search_values = [];
      $this->addRequiredValues($search_values);
      $this->addTokenValues($search_values);
      $this->addFixedTypeValues($search_values);
//dpm($search_values, "CPC03 build new search_values="); //@@@

//dpm('', "CPC04 cache the search_values"); //@@@
//        \Drupal::cache()->set($cache_id, $search_values);

      // If limiting to a single record, add this record_id to the search values
      if ($record_id) {
        foreach ($search_values as $field_name => $deltas) {
          foreach ($deltas as $delta => $keys) {
            foreach ($keys as $key => $info) {
              if ($key == 'record_id') {
                /** @var Drupal\tripal\TripalStorage\StoragePropertyValue $property_value **/
                $property_value = $search_values[$field_name][$delta][$key]['value'];
                $property_value->setValue($record_id);
              }
            }
          }
        }
      }
    }

    // Performs the actual query to retrieve information.
    $matches = $this->storage->findValues($search_values);

    return $matches;
  }

  /**
   * Retrieves a list of titles for the entities that should be published.
   *
   * @param array $matches
   *   The array of matches for each entity.
   *
   * @return array
   *   A list of titles in order of the entities provided by the $matches array.
   */
  public function getTitlesFromMatches($matches) {

    // Construct a title for each returned record.
    $title_format = $this->entity_type->getTitleFormat();
    $titles = $this->replaceTokens($title_format, $matches);

    return $titles;
  }

  /**
   * Populates the $field_info variable with field information
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin.
   * @param string $bundle
   *   The id of the bundle or entity type.
   */
  protected function setFieldInfo($datastore, $bundle) {

    // Get the field manager, field definitions for the bundle type, and
    // the field type manager.
    /** @var \Drupal\Core\Entity\EntityFieldManager $field_manager **/
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $bundle);
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    // Iterate over the field definitions for the bundle and collect the
    // information so we can use it later.
    /** @var \Drupal\Core\Field\BaseFieldDefinition $field_definition **/
    $field_definition = NULL;
    foreach ($field_defs as $field_name => $field_definition) {

      if (!empty($field_definition->getTargetBundle())) {
        $storage_definition = $field_definition->getFieldStorageDefinition();
        if ($storage_definition->getSetting('storage_plugin_id') == $datastore) {
          $configuration = [
            'field_definition' => $field_definition,
            'name' => $field_name,
            'parent' => NULL,
          ];
          $instance = $field_type_manager->createInstance($field_definition->getType(), $configuration);
          $prop_types = $instance->tripalTypes($field_definition);
          $field_class = get_class($instance);
          $this->storage->addTypes($field_name, $prop_types);
          $this->storage->addFieldDefinition($field_name, $field_definition);
          $field_info = [
            'definition' => $field_definition,
            'class' => $field_class,
            'prop_types' => [],
            'instance' => $instance,
          ];
          // Order the property types by key for easy lookup.
          foreach ($prop_types as $prop_type) {
            $field_info['prop_types'][$prop_type->getKey()] = $prop_type;
          }
          $this->field_info[$field_name] = $field_info;
        }
      }
    }
  }

  /**
   * Replace tokens in title format strings with actual values.
   *
   * @param string $title_format
   *   A string with one or more tokens enclosed in [square brackets]
   *   specifying how to construct the entity title.
   * @param array $matches
   *   The results from findValues() of the selected storage.
   *
   * @return array
   *   A title for each entity that was present in $matches.
   */
  protected function replaceTokens($title_format, $matches) {
    $titles = [];
    // Iterate through the findValues() results and build the entity titles.
    foreach ($matches as $match) {
      $entity_title = $title_format;
      foreach ($match as $field_name => $deltas) {
        if (preg_match("/\[$field_name\]/", $title_format)) {

          // There should only be one delta for the fields that
          // are used for title formats so default this to 0.
          $delta = 0;
          $field = $this->field_info[$field_name]['instance'];
          $main_prop = $field->mainPropertyName();
          $value = $match[$field_name][$delta][$main_prop]['value']->getValue();
          $entity_title = trim(preg_replace("/\[$field_name\]/", $value, $entity_title));
        }
      }
      $titles[] = $entity_title;
    }
    return $titles;
  }

  /**
   * Adds to the search values array the required property values.
   *
   * @param array $seach_values
   */
  protected function addRequiredValues(&$search_values) {

    // Iterate through the property types that can uniquely identify an entity.
    foreach ($this->required_types as $field_name => $keys) {
      foreach ($keys as $key => $prop_type) {
        $not_supported = FALSE;

        // This property may be part of a field which has already been marked as unsupported...
        // If so then it won't be in the field_info and we should skip it.
        if (!array_key_exists($field_name, $this->field_info)) {
          // Add it to the list of unsupported fields just in case
          // it wasn't added before...
          $this->unsupported_fields[$field_name] = $field_name;
          continue;
        }

        // Add this property value to the search values array.
        $field_definition = $this->field_info[$field_name]['definition'];
        $field_class = $this->field_info[$field_name]['class'];

        // We only want to add fields where we support the action for all property types in it.
        foreach ($this->field_info[$field_name]['prop_types'] as $checking_prop_key => $checking_prop_type) {
          $settings = $checking_prop_type->getStorageSettings();
          if (!in_array($settings['action'], $this->supported_actions)) {
            $not_supported = TRUE;
          }
        }

        // If it is not supported then we need to remove it from the required types list.
        if ($not_supported == TRUE) {
          // Note: We are adding the field to the unsupported list
          // and will let the admin know later on in this job.
          $this->unsupported_fields[$field_name] = $field_name;
          unset($this->required_types[$field_name]);
          unset($this->field_info[$field_name]);
        }
        else {
          $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
              $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
          $search_values[$field_name][0][$prop_type->getKey()] = ['value' => $prop_value];
        }
      }
    }
  }

  /**
   * Adds to the search values array properties needed for tokens in titles.
   *
   * Tokens are used in the title format and URL alias of entities.
   *
   * @param array $seach_values
   */
  protected function addTokenValues(&$search_values) {
    // We also need to add in the properties required to build a
    // title and URL alias.
    $title_format = $this->entity_type->getTitleFormat();
    $url_format = $this->entity_type->getURLFormat();
    foreach ($this->field_info as $field_name => $field_info) {
      if (preg_match("/\[$field_name\]/", $title_format) or
          preg_match("/\[$field_name\]/", $url_format)) {

        $field_definition = $field_info['definition'];
        $field_class = $field_info['class'];
        $field = $field_info['instance'];

        // Every field has a "main" property that provides the value for the
        // token. We need to make sure we add this property as well as the
        // record_id.

        // Add the record_id
        $prop = $field_info['prop_types']['record_id'];
        $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
            $field_class::$id, 'record_id', $prop->getTerm()->getTermId(), NULL);
        $search_values[$field_name][0]['record_id'] = ['value' => $prop_value];

        // Add the main property.
        /** @var \Drupal\tripal\TripalField\TripalFieldItemBase $field */
        /** @var \Drupal\tripal\TripalStorage\StoragePropertyBase $prop **/
        $field = $this->field_info[$field_name]['instance'];
        $main_prop = $field->mainPropertyName();
        $prop = $field_info['prop_types'][$main_prop];
        $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
            $field_class::$id, $main_prop, $prop->getTerm()->getTermId(), NULL);
        $search_values[$field_name][0][$main_prop] = ['value' => $prop_value];
      }
    }
  }

  /**
   * Adds search criteria for fixed values.
   *
   * Sometimes type values are fixed and the user cannot change
   * them. An example of this is are cases where the ChadoAdditionalTypeDefault
   * field has a type_id that will never changed. Content types such as "mRNA"
   * or "gene" use these. We need to add these to our search filter.
   *
   * @param array $seach_values
   */
  protected function addFixedTypeValues(&$search_values) {

    // Iterate through fields.
    foreach ($this->field_info as $field_name => $field_info) {

      /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
      /** @var \Drupal\Field\Entity\FieldConfig $field_definition **/
      $field_type_manager = \Drupal::service("plugin.manager.field.field_type");
      $field_definition = $field_info['definition'];

      // Skip fields without a fixed value.
      $settings = $field_definition->getSettings();
      if (!array_key_exists('fixed_value', $settings)) {
        continue;
      }

      // Get the default field values using the fixed value.
      $configuration = [
        'field_definition' => $field_definition,
        'name' => $field_name,
        'parent' => NULL,
      ];
      $field = $field_type_manager->createInstance($field_definition->getType(), $configuration);
      $prop_values = $field->tripalValuesTemplate($field_definition, $settings['fixed_value']);

      // Iterate through the properties and for those with a value add it to
      // the search values.
      /** @var \Drupal\tripal\TripalStorage\StoragePropertyValue $prop_value **/
      foreach ($prop_values as $prop_value) {
        if (($prop_value->getValue())) {
          $prop_key = $prop_value->getKey();
          $search_values[$field_name][0][$prop_key] = [
            'value' => $prop_value,
            'operation' => '=',
          ];
        }
      }
    }
  }

}
