<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use \Drupal\tripal\Services\TripalTokenParser;
use \Drupal\tripal\Services\TripalJob;

class TripalPublish {

  /**
   * The number of items that this importer needs to process. A progress
   * can be calculated by dividing the number of items process by this
   * number.
   *
   * @var integer $total_items
   */
  private $total_items = 0;

  /**
   * The number of items that have been handled so far.  This must never
   * be below 0 and never exceed $total_items;
   *
   * @var integer $num_handled
   */
  private $num_handled = 0;

  /**
   * The interval when the job progress should be updated. Updating the job
   * progress incurrs a database write which takes time and if it occurs too
   * frequently can slow down the loader.  This should be a value between
   * 0 and 100 to indicate a percent interval (e.g. 1 means update the
   * progress every time the num_handled increases by 1%).
   *
   * @var integer $interval
   */
  private $interval = 1;

  /**
   * Specifies the maximum number of records to publish at one time.
   * This limits memory consumption if there are many thousands of
   * records, for example gene records in the feature table.
   * @todo We might want to add this as an option on the publish form.
   *
   * @var integer $batch_size
   */
  private $batch_size = 1000;

  /**
   * The TripalJob object.
   *
   * @var \Drupal\tripal\Services\TripalJob $job
   */
  protected $job = NULL;

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The base table of the bundle
   *
   * @var string $base_table
   */
  protected $base_table = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * The republish flag specifies whether to publish all or just new records.
   *
   * @var string $republish
   */
  protected $republish = NULL;

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
   *  A list of property types that are not one of the required types.
   *
   * @var array $non_required_types
   */
  protected $non_required_types = [];

  /**
   * Supported actions during publishing.
   * Any field containing properties that are not in this list, will not be published!
   *
   * @var array $supported_actions
   */
  protected $supported_actions = ['store_id', 'store', 'store_link', 'store_pkey', 'read_value', 'replace', 'function'];

  /**
   * Keep track of fields which are not supported in order to let the user know.
   *
   * @var array $unsupported_fields
   */
  protected $unsupported_fields = [];

  /**
   * Stores the last percentage that progress was reported.
   *
   * @var integer
   */
  protected $reported = 0;

  /**
   * The TripalLogger object.
   *
   * @var \Drupal\tripal\Services\TripalLogger $logger
   */
  protected $logger = NULL;

  /**
   * The Entity Lookup service.
   *
   * @var \Drupal\tripal\Services\TripalEntityLookup $entity_lookup_manager
   */
  protected $entity_lookup_manager = NULL;

  /**
   * Publish content of a specified type. Uses a Tripal service.
   *
   * @param string $bundle
   *   The entity type id (bundle) to be published.
   *
   * @param string $datastore
   *   The plugin id for the TripalStorage backend to publish from.
   *
   * @param \Drupal\tripal\Services\TripalJob $job
   *  An optional TripalJob object.
   */
  public static function runTripalJob($bundle, $datastore, $options = [], TripalJob $job = NULL) {
    // Initialize the logger.
    /** @var \Drupal\tripal\Services\TripalLogger $logger **/
    $logger = \Drupal::service('tripal.logger');

    // Load the Publish service.
    /** @var \Drupal\tripal\Services\TripalPublish $publish */
    $publish = \Drupal::service('tripal.publish');

    try {
      $publish->init($bundle, $datastore, $options, $job);
      $publish->publish();
    }
    catch (Exception $e) {
      if ($job) {
        $logger->error($e->getMessage());
      }
    }
  }

  /**
   * Initializes the publisher service.
   *
   * @param string $bundle
   *   The id of the bundle or entity type.
   * @param string $datastore
   *   The id of the TripalStorage plugin.
   */
  public function init($bundle, $datastore, $datastore_options = [], TripalJob $job = NULL) {

    // Initialize class variables that may persist between consecutive jobs
    $this->total_items = 0;
    $this->num_handled = 0;
    $this->interval = 1;
    $this->job = $job;
    $this->bundle = $bundle;
    $this->republish = $datastore_options['republish'] ?? 1;
    $this->datastore = $datastore;
    $this->field_info = [];
    $this->entity_type = NULL;
    $this->storage = NULL;
    $this->required_types = [];
    $this->non_required_types = [];
    $this->unsupported_fields = [];
    $this->reported = 0;

    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');
    if ($job) {
      $this->logger->setJob($job);
    }

    // Get the bundle object so we can get settings such as the title format.
    /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
    /** @var \Drupal\Core\Entity\EntityTypeManager $entity_type_manager **/
    $entity_type_manager = \Drupal::entityTypeManager();
    $entity_type = $entity_type_manager->getStorage('tripal_entity_type')->load($bundle);
    if (!$entity_type) {
      $error_msg = 'Could not find the entity type with an id of: "%bundle".';
      throw new \Exception(t($error_msg, ['%bundle' => $bundle]));
    }
    $this->entity_type = $entity_type;
    $this->base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');

    // Get the storage plugin used to publish.
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');
    $this->storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
    if (!$this->storage) {
      $error_msg = 'Could not find an instance of the TripalStorage backend: "%datastore".';
      throw new \Exception(t($error_msg, ['%datastore' => $datastore]));
    }

    // Get the tripal entity lookup service.
    $this->entity_lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    $this->setFieldInfo();

    // Get the required field properties that will uniquely identify an entity.
    // We only need to search on those properties.
    $this->required_types = $this->storage->getStoredTypes();
    $this->non_required_types = $this->storage->getNonStoredTypes();
  }

  /**
   * Updates the percent interval when the job progress is updated.
   *
   * Updating the job progress incurrs a database write which takes time
   * and if it occurs to frequently can slow down the loader.  This should
   * be a value between 0 and 100 to indicate a percent interval (e.g. 1
   * means update the progress every time the num_handled increases by 1%).
   *
   * @param int $interval
   *   A number between 0 and 100.
   */
  protected function setInterval($interval) {
    $this->interval = $interval;
  }

  /**
   * Adds to the count of the total number of items that have been handled.
   *
   * @param int $num_handled
   */
  protected function addItemsHandled($num_handled) {
    $items_handled = $this->num_handled = $this->num_handled + $num_handled;
    $this->setItemsHandled($items_handled);
  }

  /**
   * Sets the total number if items to be processed.
   *
   * This should typically be called near the beginning of the loading process
   * to indicate the number of items that must be processed.
   *
   * @param int $total_items
   *   The total number of items to process.
   */
  protected function setTotalItems($total_items) {
    $this->total_items = $total_items;
  }

  /**
   * Sets the number of items that have been processed.
   *
   * This code was shamelessly copied from the TripalImporterBase class.
   *
   * @param int $total_handled
   *   The total number of items that have been processed.
   */
  protected function setItemsHandled($total_handled) {
    // First set the number of items handled.
    $this->num_handled = $total_handled;

    if ($total_handled == 0) {
      $memory = number_format(memory_get_usage());
      //$this->logger->info("    Percent complete: 0%. Memory: " . $memory . " bytes.");
      return;
    }

    // Now see if we need to report to the user the percent done.  A message
    // will be printed on the command-line if the job is run there.
    if ($this->total_items) {
      $percent = ($this->num_handled / $this->total_items) * 100;
      $ipercent = (int) $percent;
    }
    else {
      $percent = 0;
      $ipercent = 0;
    }

    // If we've reached our interval then print update info.
    if ($ipercent > 0 and $ipercent != $this->reported and $ipercent % $this->interval == 0) {
      $memory = number_format(memory_get_usage());
      $spercent = sprintf("%.2f", $percent);
      //$this->logger->info("    Percent complete: " . $spercent . " %. Memory: " . $memory . " bytes.");

      // If we have a job the update the job progress too.
      if ($this->job) {
        $this->job->setProgress($percent);
      }
      $this->reported = $ipercent;
    }
  }


  /**
   * Populates the $field_info variable with field information
   *
   * @param string $bundle
   *   The id of the bundle or entity type.
   */
  protected function setFieldInfo() {

    // Get the field manager, field definitions for the bundle type, and
    // the field type manager.
    /** @var \Drupal\Core\Entity\EntityFieldManager $field_manager **/
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $this->bundle);
    $field_type_manager = \Drupal::service('plugin.manager.field.field_type');

    // Iterate over the field definitions for the bundle and collect the
    // information so we can use it later.
    /** @var \Drupal\Core\Field\BaseFieldDefinition $field_definition **/
    $field_definition = NULL;
    foreach ($field_defs as $field_name => $field_definition) {

      if (!empty($field_definition->getTargetBundle())) {
        $storage_definition = $field_definition->getFieldStorageDefinition();
        if ($storage_definition->getSetting('storage_plugin_id') == $this->datastore) {
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
   * Adds to the search values array the required property values.
   *
   * @param array $seach_values
   */
  protected function addRequiredValues(&$search_values) {
    // Iterate through the property types that can uniquely identify an entity.
    foreach ($this->required_types as $field_name => $keys) {

      // Skip any fields not supported by publish.
      if (!$this->checkFieldIsSupported($field_name)) {
        unset($this->required_types[$field_name]);
        continue;
      }

      // Add this property value to the search values array.
      $field_definition = $this->field_info[$field_name]['definition'];
      $field_class = $this->field_info[$field_name]['class'];

      foreach ($keys as $key => $prop_type) {
        $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
            $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
        $search_values[$field_name][0][$prop_type->getKey()] = ['value' => $prop_value];
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
   * them.  An example of this is are cases where the ChadoAdditionalTypeDefault
   * field has a type_id that will never be changed.  Content types such as "mRNA"
   * or "gene" use these.  We need to add these to our search filter.
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

  /**
   * Adds to the search values array any remaining property values.
   *
   * @param array $seach_values
   */
  protected function addNonRequiredValues(&$search_values) {
    // Iterate through the property types that can uniquely identify an entity.
    foreach ($this->non_required_types as $field_name => $keys) {

      // Skip any fields not supported by publish.
      if (!$this->checkFieldIsSupported($field_name)) {
        unset($this->non_required_types[$field_name]);
        continue;
      }

      // Add this property value to the search values array.
      $field_definition = $this->field_info[$field_name]['definition'];
      $field_class = $this->field_info[$field_name]['class'];

      foreach ($keys as $key => $prop_type) {
        // Only add here if not already added in one of the previous steps
        if (!($search_values[$field_name][0][$prop_type->getKey()]['value'] ?? FALSE)) {
          $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
              $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
          $search_values[$field_name][0][$prop_type->getKey()] = ['value' => $prop_value];
        }
      }
    }
  }

  /**
   * Determines whether a field is supported for publishing.
   *
   * @param string $field_name
   *   The name of the field to check.
   *
   * @return bool
   *   TRUE if supported, FALSE if not.
   */
  protected function checkFieldIsSupported(string $field_name): bool {

    // This property may be part of a field which has already been marked
    // as unsupported. If so then it won't be in the field_info and we
    // should skip it.
    if (!array_key_exists($field_name, $this->field_info)) {
      // Add it to the list of unsupported fields just in case
      // it wasn't added before.
      $this->unsupported_fields[$field_name] = $field_name;
      return FALSE;
    }

    // We only want to add fields where we support the action for all property types in it.
    foreach ($this->field_info[$field_name]['prop_types'] as $checking_prop_key => $checking_prop_type) {
      $settings = $checking_prop_type->getStorageSettings();
      if (!in_array($settings['action'], $this->supported_actions)) {
        // Add it to the list of unsupported fields just in case
        // it wasn't added before.
        $this->unsupported_fields[$field_name] = $field_name;
        unset($this->field_info[$field_name]);
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Retrieve the chado record ID for a single match record.
   *
   * @param array $match
   *
   * @return int
   *   The chado record ID
   */
  protected function getChadoRecordID(array $match) {
    $record_id = NULL;
    foreach ($match as $field_info) {
      if ($field_info[0]['record_id']['value'] ?? NULL) {
        $record_id = $field_info[0]['record_id']['value']->getValue();
        break;
      }
    }
    return $record_id;
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
  protected function getEntityTitles($matches) {
    $titles = [];

    // Iterate through each match we are checking for an existing entity for.
    foreach ($matches as $match) {

      // Retrieve the chado record pkey ID for this match
      $record_id = $this->getChadoRecordID($match);

      // Collapse match array to follow the format expected by getEntityTitle.
      $entity_values = [];
      foreach ($match as $field_name => $field_items) {
        if ($field_items) {
          foreach($field_items as $delta => $properties) {
            foreach ($properties as $property_name => $prop_deets) {
              $entity_values[$field_name][$delta][$property_name] = $prop_deets['value']->getValue();
            }
          }
        }
        else {
          // Any fields without values are also included as NULL, although only
          // as delta zero. This is because these might be part of the entity
          // title but are missing, e.g. organism_infraspecific_name.
          foreach ($this->field_info[$field_name]['prop_types'] as $property_name => $prop_deets) {
            $entity_values[$field_name][0][$property_name] = NULL;
          }
        }
      }

      // Now that we've gotten the values out of the property value objects,
      // we can use the token parser to get the title!
      $entity_title = TripalTokenParser::getEntityTitle($this->entity_type, $entity_values);

      $titles[$record_id] = $entity_title;
    }
    return $titles;
  }

  /**
   * Check if the new title does not match the existing published
   * title, and if so, update it.
   * This can happen if the title format has been changed.
   * Array keys for both input arrays are the chado record ID,
   * array values are the titles.
   *
   * @param array $titles
   *   A list of new titles to check, key is chado record ID.
   * @param array $existing_titles
   *   A list of already published titles, key is chado record ID.
   * @param array $published_entities
   *   A list of published entities. The key will be the chado table
   *   record ID, the values will be the entity IDs.
   *
   * @return int
   *   Number of titles that were updated.
   */
  protected function updateExistingTitles(array $titles, array $existing_titles, array $published_entities) {

    $conn = \Drupal::service('database');
    $num_updated = 0;
    foreach ($titles as $record_id => $new_title) {
      $existing_title = $existing_titles[$record_id] ?? NULL;
      if ($existing_title and ($new_title != $existing_title)) {
        $entity_id = $published_entities[$record_id];
        $query = $conn->update('tripal_entity')
          ->fields(['title' => $new_title])
          ->condition('id', $entity_id, '=')
          ->execute();
        $num_updated++;
      }
    }
    return $num_updated;
  }

  /**
   * Retrieves a list of chado record IDs that have already been published.
   *
   * @param array $record_ids
   *   A list of chado record IDs to process.
   * @param array $published_entities
   *   All published entities for the current bundle. The key will be
   *   the chado record ID, the values will be the entity IDs.
   *
   * @return array
   *   An associative array of published entities keyed by the
   *   chado record ID with a value of the existing entity title.
   */
  protected function findEntities(array $record_ids, array $published_entities) {

    // Convert published chado record IDs to entity IDs
    $entity_ids = [];
    foreach ($record_ids as $record_id) {
      $entity_id = $published_entities[$record_id] ?? NULL;
      if ($entity_id) {
        $entity_ids[] = $entity_id;
      }
    }

    // If any published entities exist, retrieve their current titles.
    $titles = [];
    if ($entity_ids) {
      $connection = \Drupal::service('database');
      $query = $connection->select('tripal_entity', 'E');
      $query->Fields('E', ['id', 'title']);
      $query->condition('id', $entity_ids, 'IN');
      $results = $query->execute();
      while ($record = $results->fetchObject()) {
        $entity_id = $record->id;
        $chado_record_id = array_search($entity_id, $published_entities);
        $titles[$chado_record_id] = $record->title;
      }
    }

    return $titles;
  }

  /**
   * Performs bulk insert of new entities into the tripal_entity table
   *
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $titles
   *   The array of entity titles keyed by the chado record ID.
   * @param array $published_entities
   *   All published entities for the current bundle. The key will be
   *   the chado record ID, the values will be the entity IDs.
   */
  protected function insertEntities($matches, $titles, &$published_entities) {

    $connection = \Drupal::service('database');
    $timestamp = time();
    $query = $connection->insert('tripal_entity', [])
      -> fields(['type', 'title', 'status', 'created', 'changed']);
    $added_record_ids = [];
    foreach ($matches as $match) {
      //$i++;
      $record_id = $this->getChadoRecordID($match);
      $title = $titles[$record_id];
      $added_record_ids[] = $record_id;
      $query->values([$this->bundle, $title, 1, $timestamp, $timestamp]);
    }
    $first_added_entity_id = $query->execute();

    // Store the new entity IDs of the newly inserted
    // entities along with any existing ones.
    foreach ($added_record_ids as $index => $record_id) {
      $published_entities[$record_id] = $index + $first_added_entity_id;
    }
  }

  /**
   * Finds existing fields so that we will not be adding any duplicate fields.
   *
   * @param string $field_name
   *   The name of the field
   * @param array $record_ids
   *   Chado record IDs to process
   * @param array $published_entities
   *   All published entities for the current bundle. The key will be
   *   the chado record ID, the values will be the entity IDs.
   *
   * @return array
   *   An associative array of matched entities keyed first by the
   *   entity_id and then by the delta. Value is always TRUE.
   */
  protected function findFieldItems($field_name, $record_ids, $published_entities) {
    $database = \Drupal::database();
    $field_table = 'tripal_entity__' . $field_name;

    $items = [];
    $sql = 'SELECT entity_id, delta FROM {' . $field_table . '}'
         . ' WHERE bundle = :bundle AND entity_id IN (:entity_ids[]) ';

    $batch_ids = [];
    foreach ($record_ids as $record_id) {
      $entity_id = $published_entities[$record_id] ?? NULL;
      if ($entity_id) {
        $batch_ids[] = $entity_id;
      }
    }
    $args = [
      ':bundle' => $this->bundle,
      ':entity_ids[]' => $batch_ids
    ];
    $results = $database->query($sql, $args);
    while ($result = $results->fetchAssoc()) {
      $entity_id = $result['entity_id'];
      if (!array_key_exists($entity_id, $items)) {
        $items[$entity_id] = [];
      }
      $items[$entity_id][$result['delta']] = TRUE;
    }
    return $items;
  }

  /**
   * Counts the total items to insert for a field.
   *
   * The matches array returned by the TripalStorage is organized by entity
   * but fields can have a cardinality > 1.  This function counts the number
   * of items for the given field.
   *
   * @param string $field_name
   *   The name of the field
   * @param array $matches
   *   The array of matches for each entity.
   *
   * @return int
   *   The number of items for the field
   */
  protected function countFieldMatches(string $field_name, array $matches) : int {
    $total = 0;
    foreach ($matches as $match) {
      $total += count(array_keys($match[$field_name]));
    }
    return $total;
  }

  /**
   * Inserts records into the field tables for entities.
   *
   * @param string $field_name
   *   The name of the field
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $published_entities
   *   All published entities for the current bundle. The key will be
   *   the chado record ID, the values will be the entity IDs.
   * @param array $existing
   *   An associative array of entities that already have an existing item for this field.
   * @param array $published
   *   ???
   *
   * @return int
   *   The number of items inserted for the field.
   */
  protected function insertFieldItems($field_name, $matches, $published_entities, $existing, &$published) {

    $database = \Drupal::database();
    $field_table = 'tripal_entity__' . $field_name;

    $batch_size = 1000;
    $num_matches = $this->countFieldMatches($field_name, $matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Generate the insert SQL and add to it the field-specific columns.
    $init_sql = "
      INSERT INTO {" . $field_table . "}
        (bundle, deleted, entity_id, revision_id, langcode, delta, ";
    foreach (array_keys(array_merge($this->required_types[$field_name],
                                    $this->non_required_types[$field_name])) as $key) {
      $init_sql .= $field_name . '_'. $key . ', ';
    }
    $init_sql = rtrim($init_sql, ", ");
    $init_sql .= ") VALUES\n";

    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    $num_inserted = 0;


    // Iterate through the matches. Each match corresponds to a single
    // entity.
    foreach ($matches as $match) {
      $record_id = $this->getChadoRecordID($match);
      $entity_id = $published_entities[$record_id] ?? NULL;
      // entity_id will be NULL in the case of an excluded match resulting
      // from a duplicated title. In this case, skip all fields.
      if (!$entity_id) {
        continue;
      }

      // Iterate through the "items" of each field and insert a record value
      // for each non-empty item.
      $num_items = count(array_keys($match[$field_name]));
      for ($delta = 0; $delta < $num_items; $delta++) {
        // Leave these increments outside the add_record check
        // to keep our count predictable, just note that some
        // values of $j may not be used, however, $num_inserted
        // will be accurate.
        $j++;
        $total++;

        // No need to add items to those that are already published.
        $add_record = TRUE;
        if (array_key_exists($entity_id, $existing) and
            array_key_exists($delta, $existing[$entity_id])) {
          $add_record = FALSE;
        }
        // Determine if we want to add this item.
        else {
          foreach (array_keys($this->required_types[$field_name]) as $key) {
            $storage_settings = $this->field_info[$field_name]['prop_types'][$key]->getStorageSettings();
            $drupal_store = $storage_settings['drupal_store'] ?? FALSE;
            if ($drupal_store) {
              $value = '';
              if (array_key_exists($key, $match[$field_name][$delta])) {
                $value = $match[$field_name][$delta][$key]['value']->getValue();
              }
              if (is_null($value)) {
                $add_record = FALSE;
                break;
              }
            }
          }
        }
        if ($add_record) {
          $this->insertOneFieldItem($sql, $args, $j, $match, $entity_id, $delta, $field_name);
          $num_inserted++;
        }

        // If we've reached the size of the batch then let's do the insert.
        if ($j == $batch_size or $total == $num_matches) {
          if (count($args) > 0) {
            $sql = rtrim($sql, ",\n");
            $sql = $init_sql . $sql;
            $database->query($sql, $args);
          }
          $this->setItemsHandled($batch_num);
          $batch_num++;

          // Now reset all of the variables for the next batch.
          $sql = '';
          $j = 0;
          $args = [];
        }
      }
    }
    return $num_inserted;
  }

  /**
   * Add a single field item to the sql and args.
   * This is a helper function for insertFieldItems().
   *
   * @param string &$sql
   *   The sql command under construction
   * @param array &$args
   *   Values for the placeholders
   * @param int $j
   *   Index for the placeholders
   * @param array $match
   *   Contains all data to be published
   * @param int $entity_id
   *   Id of the entity for this field
   * @param int $delta
   *   Field delta
   * @param string $field_name
   *   Name of the field being published
   */
  private function insertOneFieldItem(&$sql, &$args, $j, $match, $entity_id, $delta, $field_name) {
    $sql .= "(:bundle_$j, :deleted_$j, :entity_id_$j, :revision_id_$j, :langcode_$j, :delta_$j, ";
    $args[":bundle_$j"] = $this->bundle;
    $args[":deleted_$j"] = 0;
    $args[":entity_id_$j"] = $entity_id;
    $args[":revision_id_$j"] = $entity_id;  // For an unversioned entity this is the same as the entity id
    $args[":langcode_$j"] = 'und';
    $args[":delta_$j"] = $delta;
    foreach ($this->required_types[$field_name] as $key => $properties) {
      $placeholder = ':' . $field_name . '_'. $key . '_' . $j;
      $sql .=  $placeholder . ', ';
      $value = $match[$field_name][$delta][$key]['value']->getValue();
      // If there is no value, use a placeholder of the correct type, string '', int 0, etc.
      if (is_null($value)) {
        $value = $properties->getDefaultValue();
      }
      $args[$placeholder] = $match[$field_name][$delta][$key]['value']->getValue();
    }
    // Non-required types never have a value stored, just a placeholder.
    foreach ($this->non_required_types[$field_name] as $key => $properties) {
      $placeholder = ':' . $field_name . '_'. $key . '_' . $j;
      $sql .=  $placeholder . ', ';
      $args[$placeholder] = $properties->getDefaultValue();
    }
    $sql = rtrim($sql, ", ");
    $sql .= "),\n";
  }

  /**
   * Removes existing records from the set of matched records.
   *
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $existing_published_entities
   *   The array of existing records.
   *
   * @return array
   *   The passed $matches with already published entities excluded.
   */
  protected function excludeExisting($matches, $existing_published_entities) {
    $new_matches = [];
    foreach ($matches as $match) {
      $record_id = $this->getChadoRecordID($match);
      if (!array_key_exists($record_id, $existing_published_entities)) {
        $new_matches[] = $match;
      }
    }

    return $new_matches;
  }

  /**
   * Divides up a long list of record IDs into smaller batches
   * for publishing, to reduce memory requirements.
   *
   * @param array $record_ids
   *   A list of primary key values.
   *
   * @return array
   *   Original array values divided into a 2-D array of several batches.
   *   First level array key is a delta value starting at zero.
   */
  protected function divideIntoBatches($record_ids) {
    $batches = [];
    $num_batches = (int) ((count($record_ids) + $this->batch_size - 1) / $this->batch_size);
    for ($delta = 0; $delta < $num_batches; $delta++) {
      $batches[$delta] = array_slice($record_ids, $delta * $this->batch_size, $this->batch_size);
    }
    return $batches;
  }

  /**
   * Publishes Tripal entities.
   *
   * Publishes content to Tripal from Chado or another
   * specified datastore that matches the provided
   * filters.
   *
   * @param array $filters
   *   Filters that determine which content will be published.
   *
   * @return array
   *   An associative array of the entities that were published, keyed
   *   by their titles, and the value being the entity_id.
   *
   */
  public function publish($filters = []) {

//@todo $transaction_chado = $this->connection->startTransaction();
//$transaction_chado->rollback();

    $total_items = 0;
    $published_entities = [];
    $total_existing_entities = 0;
    $total_new_entities = 0;
    $total_updated_titles = 0;

    // Build the search values array
    $search_values = [];
    $this->addRequiredValues($search_values);
    $this->addTokenValues($search_values);
    $this->addFixedTypeValues($search_values);
    $this->addNonRequiredValues($search_values);

    // We retrieve a list of all primary keys for the base table of the
    // content type. This allows us to divide publishing into small batches
    // to reduce the amount of memory required if there are thousands of
    // records to publish.
    $this->logger->notice("Finding all candidate record IDs...");
    $record_ids = $this->storage->findAllRecordIds($this->bundle);

    // Get a list of already-published entities.
    // The key will be the chado table record ID, the values will be the entity IDs.
    $existing_published_entities = $this->entity_lookup_manager->getPublishedEntityIds($this->bundle, 'tripal_entity');

    // If not republishing everything, remove any already published records.
    if (!$this->republish) {
      $record_ids = array_diff($record_ids, array_keys($existing_published_entities));
    }

    // If there is nothing to publish, quit early
    if (!count($record_ids)) {
      $this->logger->notice('There are no records to publish for this content type');
      return [];
    }

    // Divide into batches
    $record_id_batches = $this->divideIntoBatches($record_ids);
    $number_of_batches = count($record_id_batches);

    // Let user know how much will be published
    $message = 'Will publish ' . number_format(count($record_ids)) . ' records';
    if ($number_of_batches > 1) {
      $message .= ' in ' . number_format($number_of_batches) . ' batches';
    }
    $this->logger->notice($message);

    foreach ($record_id_batches as $batch_num => $record_id_batch) {

      // Only display a batch prefix when there is more than one batch.
      $batch_prefix = '';
      if ($number_of_batches > 1) {
        $batch_prefix = 'Batch ' . number_format($batch_num + 1) . ' of ' . number_format($number_of_batches) . ', ';
      }

      $this->logger->notice($batch_prefix . "Step 1 of 6: Find matching records...");
      $matches = $this->storage->findValues($search_values, $record_id_batch);

      $this->logger->notice($batch_prefix . "Step 2 of 6: Generate page titles...");
      $titles = $this->getEntityTitles($matches);

      $this->logger->notice($batch_prefix . "Step 3 of 6: Find existing published entity titles...");
      $existing_titles = $this->findEntities($record_id_batch, $existing_published_entities);
      $total_existing_entities += count($existing_titles);

      $this->logger->notice($batch_prefix . "Step 4 of 6: Updating existing page titles...");
      $total_updated_titles += $this->updateExistingTitles($titles, $existing_titles, $existing_published_entities);

      // Exclude any matches that are already published. We
      // need to publish only new matches.
      $new_matches = $this->excludeExisting($matches, $existing_published_entities);
      $total_new_entities += count($new_matches);

      // Note: entities are not tied to any storage backend. An entity
      // references an "object".  The information about that object
      // is in the form of fields and can come from any number of data storage
      // backends. But, if the entity with a given title for this content type
      // doesn't exist, then let's create one.
      $this->logger->notice($batch_prefix . "Step 5 of 6: Publishing " . number_format(count($new_matches))  . " new entities...");
      $this->insertEntities($new_matches, $titles, $existing_published_entities);

      // Now we have to publish the field items. These represent storage back-end information
      // about the entity. If the entity was previously published we still may be adding new
      // information about it (say if we are publishing genes from a noSQL back-end but the
      // original entity was created when it was first published when using the Chado backend).
      $this->logger->notice($batch_prefix . "Step 6 of 6: Add field items to published entities...");

      if (!empty($this->unsupported_fields)) {
        $this->logger->warning("  The following fields are not supported by publish at this time: " . implode(', ', $this->unsupported_fields));
      }

      foreach ($this->field_info as $field_name => $field_info) {

        $existing_field_items = $this->findFieldItems($field_name, $record_id_batch, $existing_published_entities);
        $num_inserted = $this->insertFieldItems($field_name, $matches, $existing_published_entities,
          $existing_field_items, $published_entities);

        if ($num_inserted) {
          $this->logger->notice("  Published " . number_format($num_inserted) . " items for field: $field_name...");
        }
        $total_items += $num_inserted;
      }
    }

    $this->logger->notice("Publish completed. Published " . number_format($total_new_entities)
        . " new entities, checked " . number_format($total_existing_entities)
        . " existing entities, and added " . number_format($total_items) . " field values.");
    return $published_entities;
  }
}
