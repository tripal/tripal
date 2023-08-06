<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use \Drupal\tripal\Services\TripalJob;

class TripalPublish {

  /**
   * The TripalJob object.
   *
   * @var \Drupal\tripal\Services\TripalJob $job
   */
  protected $job = NULL;

  /**
   * The TripalLogger object.
   *
   * @var \Drupal\tripal\Services\TripalLogger $logger
   */
  protected $logger = NULL;

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
   * @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager
   **/
  protected $storage = NULL;

  /**
   *  A list of property types that are required to uniquely identify an entity.
   *
   * @var array $required_types
   */
  protected $required_types = [];


  /**
   * Initializes the publisher service.
   *
   * @param string $bundle
   *   The id of the bundle or entity type.
   * @param string $datastore
   *   The id of the TripalStorage plugin.
   */
  public function init($bundle, $datastore, TripalJob $job=NULL) {

    $this->bundle = $bundle;
    $this->datastore = $datastore;
    $this->job = $job;

    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');
    $this->logger->setJob($job);

    // Get the bundle object so we can get settings such as the title format.
    /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
    $entity_types = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties(['name' => $bundle]);
    if (!array_key_exists($bundle, $entity_types)) {
      $error_msg = 'Could not find the entity type with an id of: "%bundle".';
      throw new \Exception(t($error_msg, ['%bundle' => $bundle]));
    }
    $this->entity_type = $entity_types[$bundle];

    // Get the storage plugin used to publish.
    $storage_manager = \Drupal::service('tripal.storage');
    $this->storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
    if (!$this->storage) {
      $error_msg = 'Could not find an instance of the TripalStorage backend: "%datastore".';
      throw new \Exception(t($error_msg, ['%datastore' => $datastore]));
    }

    $this->setFieldInfo();

    // Get the rquired field properties that will uniquely identify an entity.
    // We only need to search on those properties.
    $this->required_types = $this->storage->getStoredTypes();
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
    $field_manager = \Drupal::service('entity_field.manager');
    $field_defs = $field_manager->getFieldDefinitions('tripal_entity', $this->bundle);
    /** @var \Drupal\Core\Field\FieldTypePluginManager $field_type_manager **/
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
          $this->storage->addTypes($this->bundle, $field_name, $prop_types);
          $field_info = [
            'definition' => $field_definition,
            'class' => $field_class,
            'prop_types' => [],
            'instance' => $instance,
          ];
          // Order the property types by key for eacy lookup.
          foreach ($prop_types as $prop_type) {
            $field_info['prop_types'][$prop_type->getKey()] = $prop_type;
          }
          $this->field_info[$field_name] = $field_info;
        }
      }
    }
  }

  /**
   * Adds to the search values array the required proprty values.
   *
   * @param array $seach_values
   */
  protected function addRequiredValues(&$search_values) {

    // Iterate through the property types that can uniquely identify an entity.
    foreach ($this->required_types as $bundle => $field_names) {
      foreach ($field_names as $field_name => $keys) {
        foreach ($keys as $key => $prop_type) {

          // Add this property value to the search values array.
          $field_definition = $this->field_info[$field_name]['definition'];
          $field_class = $this->field_info[$field_name]['class'];
          $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
              $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
          $search_values[$field_name][0][$prop_type->getKey()] = [
            'type' => $prop_type,
            'value' => $prop_value,
            'operation' => '<>',
            'definition' => $field_definition
          ];
        }
      }
    }
  }

  /**
   * Adds to the search values array property values for tokens.
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

        /** @var \Drupal\tripal\TripalStorage\StoragePropertyBase $prop_type **/
        foreach ($field_info['prop_types'] as $prop_key => $prop_type) {

          // Add this property value to the search values array.
          $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
              $field_class::$id, $prop_key, $prop_type->getTerm()->getTermId(), NULL);
          $search_values[$field_name][0][$prop_type->getKey()] = [
            'type' => $prop_type,
            'value' => $prop_value,
            'operation' => '=',
            'definition' => $field_definition
          ];
        }
      }
    }
  }

  /**
   * Adds search criteria for fixed values.
   *
   * Sometimes type values are fixed and the user cannot change
   * them.  An example of this is are cases where the ChadoAdditionalTypeDefault
   * field has a type_id that will never changed.  Content types such as "mRNA"
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
      $field_class = $field_info['class'];
      $settings = $field_definition->getSettings();

      // Skip fields without a fixed value.
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
        if ($prop_value->getValue()) {
          $prop_key = $prop_value->getKey();
          $search_values[$field_name][0][$prop_key] = [
            'type' => $this->field_info[$field_name]['prop_types'][$prop_key],
            'value' => $prop_value,
            'operation' => '=',
            'definition' => $field_definition
          ];
        }
      }
    }
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
    $title_format = $this->entity_type->getTitleFormat();

    // Iterate through the results and build the bulk SQL statements that
    // will publish the records.
    foreach ($matches as $record) {
      $entity_title = $title_format;
      foreach ($record as $field_name => $deltas) {
        if (preg_match("/\[$field_name\]/", $title_format)) {
          // There should only be one value for the fields that
          // are used for title formats so default this to 0.
          $delta = 0;
          $field = $this->field_info[$field_name]['instance'];
          $main_prop = $field->mainPropertyName();
          $value = $record[$field_name][$delta][$main_prop]['value']->getValue();
          $entity_title = trim(preg_replace("/\[$field_name\]/", $value,  $entity_title));
        }
      }
      $titles[] = $entity_title;
    }
    return $titles;
  }

  /**
   * Makes sure that we will not be adding any dupliate entities.
   *
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $titles
   *   The array of entity titles in the same order as the matches.
   *
   * @return array
   *   An associative array of  of matched entities keyed by the
   *   entity title with a value of the entity id.
   */
  protected function findEntities($matches, $titles) {
    $database = \Drupal::database();

    $batch_size = 1000;
    $num_matches = count($matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;
    $entities = [];

    $sql = "
      SELECT id,type,title FROM tripal_entity\n
      WHERE type = :type AND title in (:titles[])\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $args = [];
    $batch_titles = [];
    foreach ($matches as $match) {
      $batch_titles[] = $titles[$i];
      $total++;
      $i++;

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_matches) {
        $args = [
          ':type' => $this->bundle,
          ':titles[]' => $batch_titles
        ];
        $results = $database->query($sql, $args);
        while ($result = $results->fetchAssoc()) {
          $entities[$result['title']] = $result['id'];
        }
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $i = 0;
        $args = [];
        $batch_titles = [];
      }
    }
    return $entities;
  }

  /**
   * Performs bulk insert of new entities into the tripal_entity table
   *
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $titles
   *   The array of entity titles in the same order as the matches.
   */
  protected function insertEntities($matches, $titles, $existing) {
    $database = \Drupal::database();

    $batch_size = 1000;
    $num_matches = count($matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    $init_sql = "
      INSERT INTO {tripal_entity}
        (type, title, status, created, changed)
      VALUES\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($matches as $match) {
      $title = $titles[$i];
      $total++;
      $i++;

      // Add to the list of entities to insert only those
      // that don't already exist.  We shouldn't have any that
      // exist because the querying to find matches should have
      // excluded existing records that are already bublished, but
      // just in case.
      if (!in_array($title, array_keys($existing))) {
        $sql .= "(:type_$i, :title_$i, :status_$i, :created_$i, :changed_$i),\n";
        $args[":type_$i"] = $this->bundle;
        $args[":title_$i"] = $title;
        $args[":status_$i"] = 1;
        $args[":created_$i"] = time();
        $args[":changed_$i"] = time();
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_matches) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $database->query($sql, $args);
        }
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   * Inserts records into the field tables for entities.
   *
   * @param string $field_name
   *   The name of the field
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $titles
   *   The array of entity titles in the same order as the matches.
   * @param array $entities
   *   An associative array of entities to which fields should be associated.
   * @param array $existing
   *   An associative array of entities that already existed that
   *   should be skipped
   */
  protected function insertField($field_name, $matches, $titles, $entities, $existing) {

    $database = \Drupal::database();
    $field_table = 'tripal_entity__' . $field_name;

    $batch_size = 1000;
    $num_matches = count($matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    // Generate the insert SQL and add to it the field-specific columns.
    $init_sql = "
      INSERT INTO {$field_table}
        (bundle, deleted, entity_id, revision_id, langcode, delta, ";
    foreach ($this->required_types[$this->bundle][$field_name] as $key => $prop_type) {
      $init_sql .= $field_name . '_'. $key . ', ';
    }
    $init_sql = rtrim($init_sql, ", ");
    $init_sql .= ") VALUES\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($matches as $match) {
      $title = $titles[$i];
      $entity_id = $entities[$title];
      // @todo: deal with fields that have multiple values.
      // right now we just assume only one record per field.
      $delta = 0;
      $total++;
      $i++;

      // Add to the list of entities to insert only those
      // that don't already exist.  We shouldn't have any that
      // exist because the querying to find matches should have
      // excluded existing records that are already bublished, but
      // just in case.
      if (!in_array($title, array_keys($existing))) {
        $sql .= "(:bundle_$i, :deleted_$i, :entity_id_$i, :revision_id_$i, :langcode_$i, :delta_$i, ";
        foreach ($this->required_types[$this->bundle][$field_name] as $key => $prop_type) {
          $placeholder = ':' . $field_name . '_'. $key . '_' . $i;
          $sql .=  $placeholder . ', ';
          $args[$placeholder] = $match[$field_name][$delta][$key]['value']->getValue();
        }
        $sql = rtrim($sql, ", ");
        $sql .= "),\n";
        $args[":bundle_$i"] = $this->bundle;
        $args[":deleted_$i"] = 0;
        $args[":entity_id_$i"] = $entity_id;
        $args[":revision_id_$i"] = 1;
        $args[":langcode_$i"] = 'und';
        $args[":delta_$i"] = $delta;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_matches) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $database->query($sql, $args);
        }
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
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
   * @return int
   *   The number of items published, FALSE on failure (for now).
   *
   */
  public function publish($filters = []) {

    // Build the search values array
    $search_values = [];
    $this->addRequiredValues($search_values);
    $this->addTokenValues($search_values);
    $this->addFixedTypeValues($search_values);

    // Perform the query to find matching records.
    $matches = $this->storage->findValues($search_values);

    // Get the titles for these entities
    $titles = $this->getEntityTitles($matches);

    // Find any entities that are already in the database.
    $existing = $this->findEntities($matches, $titles);

    // Insert new entities
    $this->insertEntities($matches, $titles, $existing);

    // Now get the list of the entity IDs. We need these
    // for adding the fields.
    $entities = $this->findEntities($matches, $titles);

    foreach ($this->field_info as $field_name => $info) {
      $this->insertField($field_name, $matches, $titles, $entities, $existing);
    }

  }
}
