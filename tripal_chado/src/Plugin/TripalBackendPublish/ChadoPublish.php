<?php

namespace Drupal\tripal_chado\Plugin\TripalBackendPublish;

use Drupal\Component\Utility\Xss;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalBackendPublish\TripalBackendPublishBase;
use Drupal\tripal\TripalBackendPublish\Exceptions\TripalPublishException;

/**
 * Chado-specific TripalEntity publish.
 *
 *  @TripalBackendPublish(
 *    id = "chado_storage",
 *    label = @Translation("Chado Publish"),
 *    description = @Translation("Creates Tripal content based on records in a chado database."),
 *  )
 */
class ChadoPublish extends TripalBackendPublishBase {

  /**
   * The base table of the bundle
   *
   * @var string $base_table
   */
  protected $base_table = '';

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
   * A list of the main properties for each field.
   * The key is the field name, and the value is the name of the main property.
   *
   * @var array $main_property_names
   */
  protected $main_property_names = [];

  /**
   * Stores the bundle (entity type) object.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType $entity_type
   **/
  protected $entity_type = NULL;

  /**
   * Stores the user that is publishing content.
   *
   * @var int $uid
   **/
  protected $uid = NULL;

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
   * All published entities for the current bundle. The key will be
   * the chado record ID, the values will be the entity IDs.
   *
   * @var array $existing_published_entities
   */
  protected $existing_published_entities = [];

  /**
   * The first 100 published entities, key is entity_id,
   * value is title. These are only used for unit tests,
   * so don't need to use memory to store them all.
   *
   * @var array $published_or_updated_entities
   */
  protected $published_or_updated_entities = [];

  /**
   * Array of values to search in chado storage.
   *
   * @var array $search_values
   */
  protected $search_values = [];

  /**
   * Information used to migrate Tripal 3 entity values
   * when first publishing a migrated chado instance.
   *
   * @var array $migration_data
   */
  protected array $migration_data = [];

  /**
   * Stores the maximum entity ID value present in $this->migration_data
   *
   * @var int $max_migrated_entity_id
   */
  protected int $max_migrated_entity_id = 0;

  /**
   * Flag to permit a more lenient Tripal 3 migration.
   * If a record is missing in the migration data, then skip it.
   *
   * @var bool $lenient_migration
   */
  protected bool $lenient_migration = FALSE;

  /**
   * Entity and field values used for token replacement, keyed by record_id.
   *
   * @var array $token_values
   */
  protected array $token_values = [];

  /**
   * Flag to indicate if we should republish in order to cache chado values.
   *
   * @var bool
   */
  protected $republish = TRUE;

  /**
   * Populates the $this->field_info variable with field information
   *
   * @param string $filename
   *   If not an empty string, load this data file.
   *   If an empty string, do nothing.
   *
   * @return string
   *   Empty string if successful, including no file specified.
   *   Error message if something went wrong.
   */
  protected function loadMigrationData(string $filename): string {
    $errormsg = '';
    $n_records = 0;
    $this->max_migrated_entity_id = 0;
    if ($filename) {
      if (!file_exists($filename)) {
        $errormsg = 'The specified file "' . $filename . '" does not exist';
      }
      else {
        try {
          $fh = fopen($filename, 'r');
          $nlines = 0;
          while (!feof($fh)) {
            $nlines++;
            $line = fgets($fh, 2048);
            if ($line) {
              $cols = str_getcsv($line, "\t");
              if (count($cols) != 4) {
                $errormsg = 'Incorrect number of columns line ' . $nlines
                            . ', expected 4, found ' . count($cols);
                return $errormsg;
              }
            }
            // The columns are bundle_name(not used), chado_table, pkey_id, entity_id
            $this->migration_data[$cols[1]][$cols[2]] = $cols[3];
            $n_records++;
            if ($cols[3] > $this->max_migrated_entity_id) {
              $this->max_migrated_entity_id = $cols[3];
            }
          }
        }
        catch (\Exception $e) {
          $errormsg = $this->t('Unable to load migration data from file ":filename" '. $e->getMessage(),
            [':filename' => $filename]);
        }
      }
      // Makes sure value of next entity ID is above any in the migration data
      $this->set_tripal_entity_id_seq();

      if (!$errormsg) {
        $this->logger->notice(t('Loaded @n_records records of migration data, maximum entity ID is @max_eid',
                                ['@n_records' => $n_records, '@max_eid' => $this->max_migrated_entity_id]));
      }
    }
    return $errormsg;
  }

  /**
   * When migrating Tripal 3 entity ID values, makes sure the
   * sequence "tripal_entity_id_seq" next value is higher than
   * the maximum from the migration data.
   */
  protected function set_tripal_entity_id_seq() {
    if ($this->max_migrated_entity_id) {
      $sql = "SELECT NEXTVAL('tripal_entity_id_seq')";
      $nextval = $this->connection->query($sql, [])->fetchField();
      if ($nextval <= $this->max_migrated_entity_id) {
        $start = $this->max_migrated_entity_id + 1;
        $this->logger->notice('Updating tripal entity sequence to start at @start',
            ['@start' => $start]);
        $sql = "ALTER SEQUENCE tripal_entity_id_seq RESTART " . $start;
        $this->connection->query($sql, []);
      }
    }
  }

  /**
   * Populates the $this->field_info variable with field information
   *
   * @param string $bundle
   *   The id of the bundle or entity type.
   */
  protected function setFieldInfo() {

    // Get the field definitions for the bundle type
    $field_defs = $this->entity_field_manager->getFieldDefinitions('tripal_entity', $this->bundle);

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
          $instance = $this->field_type_manager->createInstance($field_definition->getType(), $configuration);
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

          // Store the main properties for later
          $this->main_property_names[$field_name] = $instance->mainPropertyName();
        }
      }
    }
  }

  /**
   * Adds to the search values array the required property values.
   */
  protected function addRequiredValues() {
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
        $this->search_values[$field_name][0][$prop_type->getKey()] = ['value' => $prop_value];
      }
    }
  }

  /**
   * Adds to the search values array properties needed for tokens in titles.
   *
   * Tokens are used in the title format and URL alias of entities.
   */
  protected function addTokenValues() {
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
        $this->search_values[$field_name][0]['record_id'] = ['value' => $prop_value];

        // Add the main property.
        /** @var \Drupal\tripal\TripalField\TripalFieldItemBase $field */
        /** @var \Drupal\tripal\TripalStorage\StoragePropertyBase $prop **/
        $field = $this->field_info[$field_name]['instance'];
        $main_prop = $field->mainPropertyName();
        $prop = $field_info['prop_types'][$main_prop];
        $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
            $field_class::$id, $main_prop, $prop->getTerm()->getTermId(), NULL);
        $this->search_values[$field_name][0][$main_prop] = ['value' => $prop_value];
      }
    }
  }

  /**
   * Adds search criteria for fixed values.
   *
   * Sometimes type values are fixed and the user cannot change
   * them.  An example of this is are cases where the ChadoAdditionalTypeDefault
   * field has a type_id that will never be changed. Content types such as "mRNA"
   * or "gene" use these. We need to add these to our search filter.
   */
  protected function addFixedTypeValues() {

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
          $this->search_values[$field_name][0][$prop_key] = [
            'value' => $prop_value,
            'operation' => '=',
          ];
        }
      }
    }
  }

  /**
   * Adds to the search values array any remaining property values.
   */
  protected function addNonRequiredValues() {
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
        if (!($this->search_values[$field_name][0][$prop_type->getKey()]['value'] ?? FALSE)) {
          $prop_value = new StoragePropertyValue($field_definition->getTargetEntityTypeId(),
              $field_class::$id, $prop_type->getKey(), $prop_type->getTerm()->getTermId(), NULL);
          $this->search_values[$field_name][0][$prop_type->getKey()] = ['value' => $prop_value];
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
   * @param string $title_format
   *   The format for titles for this bundle, contains one or more tokens.
   *
   * @return array
   *   A list of titles in order of the entities provided by the $matches array.
   */
  protected function getEntityTitles(array $matches, string $title_format) {
    $titles = [];

    // Iterate through each match we are checking for an existing entity for.
    foreach ($matches as $match) {
      // Retrieve the chado record pkey ID for this match
      $record_id = $this->getChadoRecordID($match);
      $entity_id = $this->existing_published_entities[$record_id] ?? NULL;

      // Build an array of token keys and values to use for token replacement.
      // n.b. We do not currently support the entity_id field in the title for
      // a newly published entity, as the entity gets created later.
      $token_values = $this->getBundleTokenValues($title_format, $entity_id);

      foreach ($match as $field_name => $field_items) {
        if ($field_items) {
          foreach($field_items as $delta => $properties) {
            foreach ($properties as $property_name => $prop_deets) {
              $prop_value = $prop_deets['value']->getValue();
              if ($property_name == ($this->main_property_names[$field_name]??'')) {
                $token_values[$field_name] = $prop_value;
              }
            }
          }
        }
      }
      // Now that we've gotten the values out of the property value objects,
      // we can use the token parser to get the title!
      $entity_title = $this->token_parser->replaceTokens($title_format, $token_values);
      $sanitized_title = Xss::filter($entity_title, $this->allowed_title_tags);
      $titles[$record_id] = $sanitized_title;
      // Save the token values, we will need them again when we generate the URL Alias
      $this->token_values[$record_id] = $token_values;
    }
    return $titles;
  }

  /**
   * Implements bundle token lookup similar to that done in
   * Drupal\tripal\Entity\getBundleEntityTokenValues getBundleEntityTokenValues()
   *
   * @param string $tokenized_string
   *   The title format template
   * @param int|null $entity_id
   *   The drupal entity numeric ID. Not known for a newly published entity.
   * @return array
   *   Associative array of all tokens and their values,
   *   ready to use for token replacement.
   */
  protected function getBundleTokenValues(string $tokenized_string, ?int $entity_id): array {
    $values = [];
    if (preg_match_all('/\[([^\[\]]+)\]/', $tokenized_string, $matches)) {
      $tokens = $matches[1];
      foreach ($tokens as $token) {
        $value = NULL;

        // Look for values for bundle or entity related tokens.
        if (($token === 'TripalEntityType__entity_id') OR ($token === 'TripalBundle__bundle_id')) {
          $value = $this->entity_type->getID();
        }
        elseif ($token == 'TripalEntityType__label') {
          $value = $this->entity_type->getLabel();
        }
        elseif ($token === 'TripalEntity__entity_id') {
          $value = $entity_id;
        }
        elseif ($token == 'TripalEntityType__term_namespace') {
          $value = $this->entity_type->get('termIdSpace');
        }
        elseif ($token == 'TripalEntityType__term_accession') {
          $value = $this->entity_type->get('termAccession');
        }
        elseif ($token == 'TripalEntityType__term_label') {
          $value = $this->entity_type->getTerm()->getName();
        }
        // We skip over any tokens other than those defined here
        if (!is_null($value)) {
          $values[$token] = $value;
        }
      }
    }
    return $values;
  }

  /**
   * Makes sure the title format is not the generic default. For publishing
   * a user-created content type, we enforce that the title format has been
   * set by the site admin.
   *
   * @param string $title_format
   *   A string containing one or more tokens
   *
   * @return bool
   *   TRUE if title_format is valid, FALSE if not valid.
   */
  private function validateTitleFormat(string $title_format) {
    $message = '';
    if (!preg_match('/\[.*\]/', $title_format)) {
      $message = 'The Page Title Format for this content type does not contain any tokens.';
    }
    elseif ($title_format == 'Entity [TripalEntity__entity_id]') {
      $message = 'The Page Title Format for this content type is the default generic format.';
    }
    if ($message) {
      $message .= ' You must update the format before publishing content.';
      $this->logger->error($message);
      return FALSE;
    }
    else {
      return TRUE;
    }
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
   *
   * @return int
   *   Number of titles that were updated.
   */
  protected function updateExistingTitles(array $titles, array $existing_titles) {

    $num_updated = 0;
    foreach ($titles as $record_id => $new_title) {
      $existing_title = $existing_titles[$record_id] ?? NULL;
      if (!is_null($existing_title) and ($new_title != $existing_title)) {
        $entity_id = $this->existing_published_entities[$record_id];
        $query = $this->connection->update('tripal_entity')
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
   *
   * @return array
   *   An associative array of published entities keyed by the
   *   chado record ID with a value of the existing entity title.
   */
  protected function findEntities(array $record_ids) {

    // Convert published chado record IDs to entity IDs
    $entity_ids = [];
    foreach ($record_ids as $record_id) {
      $entity_id = $this->existing_published_entities[$record_id] ?? NULL;
      if ($entity_id) {
        $entity_ids[] = $entity_id;
      }
    }

    // If any published entities exist, retrieve their current titles.
    $titles = [];
    if ($entity_ids) {
      $query = $this->connection->select('tripal_entity', 'E');
      $query->Fields('E', ['id', 'title']);
      $query->condition('id', $entity_ids, 'IN');
      $results = $query->execute();
      while ($record = $results->fetchObject()) {
        $entity_id = $record->id;
        $chado_record_id = array_search($entity_id, $this->existing_published_entities);
        $titles[$chado_record_id] = $record->title;
      }
    }

    // If we are going to republish, then clear the cache for all of the
    // existing entities because we may change titles or field values.
    if ($this->republish) {
      $tags = [];
      foreach ($this->existing_published_entities as $entity_id) {
        $tags[] = 'values:tripal_entity:' . $entity_id;
      }
      if ($tags) {
        \Drupal::service('cache.entity')->invalidateMultiple($tags);
        \Drupal::service('cache_tags.invalidator')->invalidateTags(['rendered']);
      }
    }

    return $titles;
  }

  /**
   * When using Tripal 3 migration data, validate that all
   * values are present and available.
   *
   * @param array &$matches
   *   The array of matches for each entity.
   * @param bool $lenient
   *   If TRUE, allow records to be missing from migration data.
   *   This can happen if you had unpublished records on your
   *   Tripal 3 site.
   *
   * @return bool
   *   Returns TRUE if validation passes, FALSE if not.
   */
  protected function validateMigrationData(array &$matches, bool $lenient): bool {
    $success = TRUE;
    if ($this->migration_data) {
      $entity_ids = [];
      $n_skipped = 0;
      // Validate that we have migration data for all records to be published.
      foreach ($matches as $index => $match) {
        $record_id = $this->getChadoRecordID($match);
        $old_entity_id = $this->migration_data[$this->base_table][$record_id] ?? NULL;
        if (!$old_entity_id) {
          if (!$lenient) {
            $this->logger->error('Encountered missing migration data for bundle'
                . ' @bundle record @record_id, cannot continue. Consider using'
                . ' the "--lenient-migration" option if you had unpublished'
                . ' records on your Tripal 3 site.',
                ['@bundle' => $this->bundle, '@record_id' => $record_id]);
            $success = FALSE;
            break;
          }
          else {
            // In lenient mode, remove this record from the $matches
            // array, it will not be published.
            unset($matches[$index]);
            $n_skipped++;
          }
        }
        else {
          $entity_ids[] = $old_entity_id;
        }
      }
      if ($n_skipped) {
        $this->logger->notice('@n records without migration data were skipped',
            ['@n' => number_format($n_skipped)]);
      }
      if ($success and $entity_ids) {
        // Validate that all migrated entity ID values are currently available.
        $query = $this->connection->select('tripal_entity', 'E');
        $query->Fields('E', ['id']);
        $query->condition('id', $entity_ids, 'IN');
        $count = $query->countQuery()->execute()->fetchField();
        if ($count > 0) {
          $first_record = $query->execute()->fetchField();
          $this->logger->error('@count migrated entity ID values are'
              . ' already present on this site, cannot continue.'
              . ' First instance is @first_record',
              ['@count' => $count, '@first_record' => $first_record]);
          $success = FALSE;
        }
      }
    }
    return $success;
  }

  /**
   * Performs bulk insert of new entities into the tripal_entity table
   *
   * @param array $matches
   *   The array of new matches for each entity.
   * @param array $titles
   *   The array of entity titles keyed by the record ID.
   */
  protected function insertEntities($matches, $titles) {

    $timestamp = time();

    $fields = ['type', 'uid', 'title', 'status', 'created', 'changed'];
    if ($this->migration_data) {
      $fields[] = 'id';
    }
    $query = $this->connection->insert('tripal_entity', [])
      -> fields($fields);
    $added_record_ids = [];
    $entity_ids = [];
    foreach ($matches as $match) {
      $record_id = $this->getChadoRecordID($match);
      $title = $titles[$record_id];
      $added_record_ids[] = $record_id;
      $values = [$this->bundle, $this->uid, $title, 1, $timestamp, $timestamp];
      if ($this->migration_data) {
        $eid = $this->migration_data[$this->base_table][$record_id];
        $values[] = $eid;
        $entity_ids[] = $eid;
      }
      $query->values($values);
    }
    $first_added_entity_id = $query->execute();

    // Store the new entity IDs of the newly inserted
    // entities along with any existing ones.
    foreach ($added_record_ids as $index => $record_id) {
      if (!$this->migration_data) {
        $entity_ids[] = $index + $first_added_entity_id;
      }
      $this->existing_published_entities[$record_id] = $entity_ids[$index];
      // Return only the first 100 for the publish job
      if (count($this->published_or_updated_entities) < 100) {
        $this->published_or_updated_entities[$index + $first_added_entity_id] = $titles[$record_id];
      }
    }

    // Insert the default URL alias for each new entity
    $storage = \Drupal::entityTypeManager()->getStorage('tripal_entity');
    $entities = $storage->loadMultiple($entity_ids);
    $index = 0;
    $tags = [];
    foreach ($entities as $entity_id => $entity) {
      $record_id = $added_record_ids[$index];
      $entity->setTokenValues($this->token_values[$record_id]);
      $entity->setAlias();
      $tags[] = 'values:tripal_entity:' . $entity_id;
      $index++;
    }
    // Clear cache so that fields will appear on new entities
    if ($index) {
      \Drupal::service('cache.entity')->invalidateMultiple($tags);
      \Drupal::service('cache_tags.invalidator')->invalidateTags(['rendered']);
    }
  }

  /**
   * Finds existing fields so that we will not be adding any duplicate fields.
   *
   * @param string $field_name
   *   The name of the field
   * @param array $record_ids
   *   Chado record IDs to process
   *
   * @return array
   *   An associative array of matched entities keyed first by the
   *   entity_id and then by the delta. Value is always TRUE.
   */
  protected function findFieldItems($field_name, $record_ids) {
    $field_table = 'tripal_entity__' . $field_name;

    $items = [];
    $sql = 'SELECT entity_id, delta FROM {' . $field_table . '}'
         . ' WHERE bundle = :bundle AND entity_id IN (:entity_ids[]) ';

    $batch_ids = [];
    foreach ($record_ids as $record_id) {
      $entity_id = $this->existing_published_entities[$record_id] ?? NULL;
      if ($entity_id) {
        $batch_ids[] = $entity_id;
      }
    }
    $args = [
      ':bundle' => $this->bundle,
      ':entity_ids[]' => $batch_ids
    ];
    $results = $this->connection->query($sql, $args);
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
   * @param array $existing
   *   An associative array of entities that already have an existing item for this field.
   * @param array $titles
   *   The array of entity titles keyed by the record ID.
   *
   * @return int
   *   The number of items inserted for the field.
   */
  protected function insertFieldItems($field_name, $matches, $existing, $titles) {
    $field_table = 'tripal_entity__' . $field_name;

    $insert_batch_size = 1000;
    $num_total_matches = $this->countFieldMatches($field_name, $matches);

    // Construct a list of the columns in the field table
    $fields = ['bundle', 'deleted', 'entity_id', 'revision_id', 'langcode', 'delta'];
    $all_types = array_merge(
      $this->required_types[$field_name] ?? [],
      $this->non_required_types[$field_name] ?? []
    );
    foreach (array_keys($all_types) as $key) {
      $fields[] = $field_name . '_'. $key;
    }

    // Initialize queries
    $insert_query = $this->connection->insert($field_table)->fields($fields);
    $insert_count = 0;
    $num_processed_matches = 0;
    $num_inserted = 0;
    $num_updated = 0;
    $batch_num_values = 0;

    // Iterate through the matches. Each match corresponds to a single
    // entity.
    foreach ($matches as $match) {
      $record_id = $this->getChadoRecordID($match);
      $entity_id = $this->existing_published_entities[$record_id];

      // Iterate through the "items" of each field and insert a record value
      // for each non-empty item.
      $num_items = count(array_keys($match[$field_name]));
      for ($delta = 0; $delta < $num_items; $delta++) {
        $num_processed_matches++;

        // No need to add items to those that are already published.
        $add_record = $this->checkOneFieldItem($field_name, $match, $existing, $titles, $entity_id, $delta, $record_id);
        if ($add_record) {
          $this->insertOneFieldItem($insert_query, $match, $entity_id, $delta, $field_name);
          $num_inserted++;
          $batch_num_values++;
        }
        elseif ($this->republish) {
          $this->updateOneFieldItem($match, $entity_id, $delta, $field_name, $field_table);
          $num_updated++;
        }

        // If we've reached the size of the batch then let's do the insert.
        if ($batch_num_values) {
          if (($batch_num_values == $insert_batch_size) or ($num_processed_matches == $num_total_matches)) {
            $insert_query->execute();
            $batch_num_values = 0;
            $insert_query = $this->connection->insert($field_table)->fields($fields);
          }
        }
      }
    }
    return [$num_inserted, $num_updated];
  }

  /**
   * Determine if a particular field item is new and needs to be published.
   *
   * @param string $field_name
   *   Name of the field being published
   * @param array $match
   *   Contains all data to be published
   * @param array $existing
   *   An associative array of entities that already have an existing item for this field.
   * @param array $titles
   *   The array of entity titles keyed by the record ID.
   * @param int $entity_id
   *   Id of the entity for this field
   * @param int $delta
   *   Field delta
   * @param int $record_id
   *   The chado record ID
   * @return bool
   *   TRUE if item should be published.
   */
  private function checkOneFieldItem($field_name, $match, $existing, $titles, $entity_id, $delta, $record_id): bool {
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
    // We later only return the first 100 published entities,
    // since they are only used for unit tests.
    if ($add_record and (count($this->published_or_updated_entities) < 100)) {
      $this->published_or_updated_entities[$entity_id] = $titles[$record_id];
    }
    return $add_record;
  }

  /**
   * Add a single field item to the insert query.
   * This is a helper function for insertFieldItems().
   *
   * @param &$insert_query
   *   The query builder object under construction
   * @param array $match
   *   Contains all data to be published
   * @param int $entity_id
   *   Id of the entity for this field
   * @param int $delta
   *   Field delta
   * @param string $field_name
   *   Name of the field being published
   * @return void
   */
  private function insertOneFieldItem(&$insert_query, $match, $entity_id, $delta, $field_name): void {
    $values = [
      'bundle' => $this->bundle,
      'deleted' => 0,
      'entity_id' => $entity_id,
      'revision_id' => $entity_id,  // For an unversioned entity this is the same as the entity id
      'langcode' => 'und',
      'delta' => $delta,
    ];
    foreach ($this->required_types[$field_name] as $key => $properties) {
      $column = $field_name . '_'. $key;
      $value = $match[$field_name][$delta][$key]['value']->getValue();
      // If there is no value, use a placeholder of the correct type, string '', int 0, etc.
      if (is_null($value)) {
        $value = $properties->getDefaultValue();
      }
      $values[$column] = $value;
    }
    // Non-required types never have a value stored, just a placeholder.
    // There might not be non_required_types for this field.
    if (isset($this->non_required_types[$field_name])) {
      foreach ($this->non_required_types[$field_name] as $key => $properties) {
        $column = $field_name . '_'. $key;
        $values[$column] = $properties->getDefaultValue();
      }
    }
    $insert_query->values($values);
  }

  /**
   * Update an existing published field item.
   * This is a helper function for insertFieldItems().
   * Unlike insertOneFieldItem(), each update is a separate
   * query because we need to include conditions.
   *
   * @param array $match
   *   Contains all data to be published
   * @param int $entity_id
   *   Id of the entity for this field
   * @param int $delta
   *   Field delta
   * @param string $field_name
   *   Name of the field being published
   * @param string $field_table
   *   Name of the drupal field table
   * @return void
   */
  private function updateOneFieldItem($match, $entity_id, $delta, $field_name, $field_table): void {
    $values = [
      'bundle' => $this->bundle,
      'deleted' => 0,
      'entity_id' => $entity_id,
      'revision_id' => $entity_id,  // For an unversioned entity this is the same as the entity id
      'langcode' => 'und',
      'delta' => $delta,
    ];
    foreach ($this->required_types[$field_name] as $key => $properties) {
      $column = $field_name . '_'. $key;
      $value = $match[$field_name][$delta][$key]['value']->getValue();
      // If there is no value, use a placeholder of the correct type, string '', int 0, etc.
      if (is_null($value)) {
        $value = $properties->getDefaultValue();
      }
      $values[$column] = $value;
    }
    // Non-required types never have a value stored, just a placeholder.
    // There might not be non_required_types for this field.
    if (isset($this->non_required_types[$field_name])) {
      foreach ($this->non_required_types[$field_name] as $key => $properties) {
        $column = $field_name . '_'. $key;
        $values[$column] = $properties->getDefaultValue();
      }
    }
    $update_query = $this->connection->update($field_table, [])->fields($values);
    $update_query->condition('bundle', $this->bundle, '=');
    $update_query->condition('entity_id', $entity_id, '=');
    $update_query->condition('delta', $delta, '=');
    $update_query->execute();
  }

  /**
   * Removes existing records from the set of matched records.
   *
   * @param array $matches
   *   The array of matches for each entity.
   *
   * @return array
   *   The passed $matches with already published entities excluded.
   */
  protected function excludeExisting($matches) {
    $new_matches = [];
    foreach ($matches as $match) {
      $record_id = $this->getChadoRecordID($match);
      if (!array_key_exists($record_id, $this->existing_published_entities)) {
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
   * Retrieves an array of chado record pkeys eligible for publishing.
   *
   * @return array
   *   An array of chado record IDs in no particular order
   */
  protected function getRecordIds() {

    // Populates the $this->field_info variable with field information
    $this->setFieldInfo();

    // Get the required field properties that will uniquely identify an entity.
    // We only need to search on those properties.
    $this->required_types = $this->storage->getStoredTypes();
    $this->non_required_types = $this->storage->getNonStoredTypes();

    // Build the $this->search_values array
    $this->addRequiredValues();
    $this->addTokenValues();
    $this->addFixedTypeValues();
    $this->addNonRequiredValues();

    // We retrieve a list of all primary keys for the base table of the
    // content type. This allows us to later divide publishing into small
    // batches to reduce the amount of memory required if there are
    // thousands of records to publish.
    $this->logger->notice('Finding all candidate records in the "'.$this->base_table.'" chado table');
    $record_ids = $this->storage->findAllRecordIds($this->bundle);

    // Get a list of already-published entities.
    // The key will be the chado table record ID, the values will be the entity IDs.
    $this->existing_published_entities = $this->entity_lookup_manager->getPublishedEntityIds($this->bundle, 'tripal_entity');

    // If not republishing everything, remove any already published records.
    if (!$this->republish) {
      $record_ids = array_diff($record_ids, array_keys($this->existing_published_entities));
    }

    return $record_ids;
  }

  /**
   * Initialization for publishing.
   *
   * @param array $options
   *   An associative array defining what and how to publish. Required keys are:
   *     'bundle' - The id of the bundle or entity type
   *     'datastore' - The id of the TripalStorage plugin
   *   Optional keys are:
   *     'republish' - If true, then republish existing entitites.
   *     'job' - A Tripal job object
   *     'batch_size' - Maximum number of records to publish per batch, defaults to 1000
   *     'migration_file' - Used to migrate Tripal 3 bio_data entity IDs.
   *     'lenient_migration' - Do not stop if there are missing records in the migration
   *         data, rather just skip over them.
   *
   * @return bool
   *   TRUE if successful, FALSE if an error occurred
   */
  public function publish_init(array $options): bool {
    $this->logger->notice('Initializing publish');

    // Required options
    $this->bundle = $options['bundle'] ?? '';
    $this->datastore = $options['datastore'] ?? '';
    if (!$this->bundle) {
      throw new TripalPublishException(t('A bundle must be specified to publish'));
    }
    if (!$this->datastore) {
      throw new TripalPublishException(t('A datastore must be specified to publish'));
    }
    // Optional values
    $this->schema_name = $options['schema_name'] ?? 'chado';
    $this->republish = boolval($options['republish'] ?? 1);
    $this->job = $options['job'] ?? NULL;
    if ($options['batch_size'] ?? 0) {
      $this->batch_size = $options['batch_size'];
    }

    // If $options['migration'] is set, load the specified data file
    $this->lenient_migration = $options['lenient_migration'] ?? FALSE;
    $errormsg = $this->loadMigrationData($options['migration_file'] ?? '');
    if ($errormsg) {
      $this->logger->error($errormsg);
      return FALSE;
    }

    // The current user will be the author of any newly published entitites
    $this->uid = \Drupal::currentUser()->id();

    // List of allowed HTML tags in entity titles
    $tag_string = \Drupal::config('tripal.settings')->get('tripal_entity_type.allowed_title_tags');
    $this->allowed_title_tags = explode(' ', $tag_string ?? '');

    // Initialize class variables that may persist between consecutive jobs
    $this->field_info = [];
    $this->entity_type = NULL;
    $this->required_types = [];
    $this->non_required_types = [];
    $this->unsupported_fields = [];
    $this->existing_published_entities = [];
    $this->published_or_updated_entities = [];
    $this->search_values = [];

    if ($this->job) {
      $this->logger->setJob($this->job);
    }

    // Get the bundle object so we can get settings such as the title format.
    /** @var \Drupal\tripal\Entity\TripalEntityType $entity_type **/
    $this->entity_type = $this->entity_type_manager->getStorage('tripal_entity_type')->load($this->bundle);
    if (!$this->entity_type) {
      throw new TripalPublishException(t('Could not find the entity type with an id of: "@bundle".',
          ['@bundle' => $this->bundle]));
    }
    $this->base_table = $this->entity_type->getThirdPartySetting('tripal', 'chado_base_table');

    // Get the storage plugin used to publish.
    $this->storage = $this->storage_manager->getInstance(['plugin_id' => $this->datastore]);
    if (!$this->storage) {
      throw new \TripalPublishException(t('Could not find an instance of the TripalStorage backend: "@datastore".',
          ['@datastore' => $this->datastore]));
    }
    // @todo somehow set the chado schema using the value in $this->schema_name
    return TRUE;
  }

  /**
   * Provides a final summary message for publish
   *
   * @param bool $success
   *   TRUE if no errors encountered
   * @param array $stats
   *   Various statistics to display
   * @return void
   */
  protected function publish_summarize(bool $success, array $stats): void {
    $message = "Publish " . ($success?'completed.':'encountered errors.');
    $message .= " Published " . number_format($stats['total_new_entities']) . " new entities";
    // This summary value is displayed only when republish is specified
    if ($this->republish) {
      $message .= ", checked " . number_format($stats['total_existing_entities']) . " existing entities";
    }
    // Titles will be updated only if the entity title format was changed
    if ($stats['total_updated_titles']) {
      $message .= ", updated titles for " . number_format($stats['total_updated_titles']) . " entities";
    }
    $message .= ", added " . number_format($stats['total_new_field_items']) . " new field values";
    if ($this->republish) {
      $message .= ", and republished ". number_format($stats['total_republished_field_items']) . " existing field values";
    }
    $message .= '.';
    $this->logger->notice($message);
  }

  /**
   * Publishes Chado content to Tripal entities.
   *
   * @param array $options
   *   Associative array defining what and how to publish.
   *   Required keys are:
   *     'bundle' - The id of the bundle or entity type
   *     'datastore' - The id of the TripalStorage plugin
   *   Optional keys are:
   *     'republish' - If true, then republish existing entitites.
   *     'job' - A Tripal job object
   *     'batch_size' - Maximum number of records to publish per batch, defaults to 1000
   *     'migration_file' - During migration of a Tripal 3 site, we would like to preserve
   *         the numeric entity IDs. This option specifies the name of a file generated
   *         by the tripal_chado/migration/export_tripal3_entity_mapping.php utility
   *         that was run on the Tripal 3 site.
   *     'lenient_migration' - Do not stop if there are missing records in the migration
   *         data, rather just skip over them.
   * .
   * @return array
   *   An associative array of the first 100 entities that were published,
   *   keyed by their titles, and the value being the entity_id.
   *
   */
  public function publish(array $options): array {
    // Initialization for publish
    if (!$this->publish_init($options)) {
      return [];
    }

    // Retrieve all chado record IDs for this bundle
    $record_ids = $this->getRecordIds();

    // If there are no record IDs for this bundle, return early
    if (!count($record_ids)) {
      $this->logger->notice('There are no records to publish for this content type');
      return [];
    }

    // Retrieve and validate the page title format
    $title_format = $this->entity_type->getTitleFormat();
    if (!$this->validateTitleFormat($title_format)) {
      return [];
    }

    // Sort and divide $record_id array into batches of $this->batch_size records
    $record_id_batches = $this->divideIntoBatches($record_ids);
    $number_of_batches = count($record_id_batches);

    // Let user know what and how much will be published
    $message = 'Preparing to publish ' . number_format(count($record_ids)) . ' "' . $this->bundle . '" records';
    if ($number_of_batches > 1) {
      $message .= ' in ' . number_format($number_of_batches) . ' batches';
    }
    $this->logger->notice($message);

    $total_new_field_items = 0;
    $total_republished_field_items = 0;
    $total_existing_entities = 0;
    $total_new_entities = 0;
    $total_updated_titles = 0;
    $success = TRUE;

    foreach ($record_id_batches as $batch_num => $record_id_batch) {

      // Only display a batch prefix when there is more than one batch.
      $batch_prefix = '';
      if ($number_of_batches > 1) {
        $batch_prefix = 'Batch ' . number_format($batch_num + 1) . ' of ' . number_format($number_of_batches) . ', ';
      }

      $this->logger->notice($batch_prefix . 'Step 1 of 6: Find matching records');
      $matches = $this->storage->findValues($this->search_values, $this->main_property_names, $record_id_batch);

      $success = $this->validateMigrationData($matches, $this->lenient_migration);
      if (!$success) {
        break;
      }

      if (!count($matches)) {
        $this->logger->notice($batch_prefix . 'No matching records found, skipping remaining steps');
        continue;
      }

      $this->logger->notice($batch_prefix . 'Step 2 of 6: Generate page titles');
      $titles = $this->getEntityTitles($matches, $title_format);

      $this->logger->notice($batch_prefix . 'Step 3 of 6: Find existing published entity titles');
      $existing_titles = $this->findEntities($record_id_batch);
      $total_existing_entities += count($existing_titles);

      // At this point we start to make database updates, so wrap in a transaction
      try {
        $transaction_drupal = $this->connection->startTransaction();

        $this->logger->notice($batch_prefix . 'Step 4 of 6: Updating existing page titles');
        $total_updated_titles += $this->updateExistingTitles($titles, $existing_titles);

        // Exclude any matches that are already published. We
        // need to publish only new matches.
        $new_matches = $this->excludeExisting($matches);
        $total_new_entities += count($new_matches);

        // Note: entities are not tied to any storage backend. An entity
        // references an "object".  The information about that object
        // is in the form of fields and can come from any number of data storage
        // backends. But, if the entity with a given title for this content type
        // doesn't exist, then let's create one.
        $this->logger->notice($batch_prefix . 'Step 5 of 6: Publishing ' . number_format(count($new_matches))  . ' new entities');
        $this->insertEntities($new_matches, $titles);

        // Now we have to publish the field items. These represent storage back-end information
        // about the entity. If the entity was previously published we still may be adding new
        // information about it (say if we are publishing genes from a noSQL back-end but the
        // original entity was created when it was first published when using the Chado backend).
        $this->logger->notice($batch_prefix . 'Step 6 of 6: Adding field items to published entities');

        if (!empty($this->unsupported_fields)) {
          $this->logger->warning("  The following fields are not supported by publish at this time: " . implode(', ', $this->unsupported_fields));
        }

        foreach (array_keys($this->field_info) as $field_name) {

          $existing_field_items = $this->findFieldItems($field_name, $record_id_batch);
          [$num_inserted, $num_updated] = $this->insertFieldItems($field_name, $matches, $existing_field_items, $titles);

          // To reduce clutter, a log message is not shown for fields with no items added
          if ($num_inserted) {
            $this->logger->notice('  Published ' . number_format($num_inserted) . " items for field \"$field_name\"");
            $total_new_field_items += $num_inserted;
          }
          if ($num_updated) {
            $this->logger->notice('  Republished ' . number_format($num_updated) . " items for field \"$field_name\"");
            $total_republished_field_items += $num_updated;
          }
        }
      }
      catch (Exception $e) {
        $transaction_drupal->rollback();
        $this->logger->error($e->getMessage() . " - Since an error occurred, database changes for $batch_prefix have been rolled back");
        // In case of error, do not attempt to publish any more batches
        $success = FALSE;
        break;
      }
    } // end of the batch loop

    // Present a final summary message, cumulative for all batches
    $this->publish_summarize($success, [
      'total_new_entities' => $total_new_entities,
      'total_existing_entities' => $total_existing_entities,
      'total_updated_titles' => $total_updated_titles,
      'total_new_field_items' => $total_new_field_items,
      'total_republished_field_items' => $total_republished_field_items,
    ]);

    // This return value is currently only used for unit tests, so is limited to 100 records.
    return $this->published_or_updated_entities;
  }

}
