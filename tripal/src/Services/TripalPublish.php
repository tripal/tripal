<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\TripalStorage\StoragePropertyValue;
use \Drupal\tripal\Services\TripalJob;
use \Drupal\tripal\Services\TripalEntityTitle;

class TripalPublish extends TripalEntityTitle {

  /**
   * The number of items that this importer needs to process. A progress
   * can be calculated by dividing the number of items process by this
   * number.
   *
   * @var integer $total_items
   */
  private $total_items;

  /**
   * The number of items that have been handled so far.  This must never
   * be below 0 and never exceed $total_items;
   *
   * @var integer $num_handled
   */
  private $num_handled;

  /**
   * The interval when the job progress should be updated. Updating the job
   * progress incurrs a database write which takes time and if it occurs too
   * frequently can slow down the loader.  This should be a value between
   * 0 and 100 to indicate a percent interval (e.g. 1 means update the
   * progress every time the num_handled increases by 1%).
   *
   * @var integer $interval
   */
  private $interval;

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
   * @var \Drupal\tripal\TripalStorage\TripalStorageBase $storage
   **/
  protected $storage = NULL;

  /**
   * A list of property types that are required to uniquely identify an entity.
   *
   * @var array $required_types
   */
  protected $required_types = [];

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
  protected $unsupported_fields;

  /**
   * Stores the last percentage that progress was reported.
   *
   * @var integer
   */
  protected $reported;

  /**
   * Initializes the publisher service.
   *
   * @param string $bundle
   *   The id of the bundle or entity type.
   * @param string $datastore
   *   The id of the TripalStorage plugin.
   */
  public function init($bundle, $datastore, $datastore_options = [], TripalJob $job = NULL) {

    $this->bundle = $bundle;
    $this->datastore = $datastore;
    $this->job = $job;
    $this->total_items = 0;
    $this->interval = 1;
    $this->num_handled = 0;
    $this->reported = 0;


    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');
    if ($job) {
      $this->logger->setJob($job);
    }

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
   * Makes sure that we will not be adding any duplicate entities.
   *
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $titles
   *   The array of entity titles in the same order as the matches.
   *
   * @return array
   *   An associative array of matched entities keyed by the
   *   entity title with a value of the entity id.
   */
  protected function findEntities($matches, $titles) {
    $database = \Drupal::database();

    $batch_size = 1000;
    $num_matches = count($matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $entities = [];

    $sql = "
      SELECT id,type,title FROM tripal_entity\n
      WHERE type = :type AND title in (:titles[])\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $args = [];
    $batch_titles = [];
    foreach ($titles as $title) {
      $batch_titles[] = $title;
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
        $this->setItemsHandled($batch_num);
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
  protected function insertEntities($matches, $titles) {
    $database = \Drupal::database();

    $batch_size = 1000;
    $num_matches = count($matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {tripal_entity}
        (type, title, status, created, changed)
      VALUES\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($titles as $title) {
      $total++;
      $i++;

      // Add to the list of entities to insert only those
      // that don't already exist.  We shouldn't have any that
      // exist because the querying to find matches should have
      // excluded existing records that are already published, but
      // just in case.
      $sql .= "(:type_$i, :title_$i, :status_$i, :created_$i, :changed_$i),\n";
      $args[":type_$i"] = $this->bundle;
      $args[":title_$i"] = $title;
      $args[":status_$i"] = 1;
      $args[":created_$i"] = time();
      $args[":changed_$i"] = time();

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_matches) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $database->query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   * Makes sure that we will not be adding any dupliate entities.
   *
   * @param string $field_name
   *   The name of the field
   * @param array $entities
   *   An associative array of entities
   *
   * @return array
   *   An associative array of matched entities keyed by the
   *   entity_id with a value of the entity id. This is an
   *   associative array to take advantage of quick lookups.
   */
  protected function findFieldItems($field_name, $entities) {
    $database = \Drupal::database();
    $field_table = 'tripal_entity__' . $field_name;

    $batch_size = 1000;
    $num_matches = count($entities);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $items = [];

    $sql = "
      SELECT entity_id FROM $field_table\n
      WHERE bundle = :bundle\n
        AND entity_id IN (:entity_ids[])\n";

    $i = 0;
    $total = 0;
    $batch_num = 1;
    $args = [];
    $batch_ids = [];
    foreach ($entities as $title => $entity_id) {
      $batch_ids[] = $entity_id;
      $total++;
      $i++;

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_matches) {
        $args = [
          ':bundle' => $this->bundle,
          ':entity_ids[]' => $batch_ids
        ];
        $results = $database->query($sql, $args);
        while ($result = $results->fetchAssoc()) {
          $items[$result['entity_id']] = $result['entity_id'];
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the variables for the next batch.
        $i = 0;
        $args = [];
        $batch_ids = [];
      }
    }
    return $items;
  }

  /**
   * Counts the total items to insert for a field.
   *
   * The matches array returned by the TripalStorage is orgnized by entity
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
   * @param array $titles
   *   The array of entity titles in the same order as the matches.
   * @param array $entities
   *   An associative array that maps entity titles to their keys.
   * @param array $existing
   *   An associative array of entities that already have an existing item for this field.
   */
  protected function insertFieldItems($field_name, $matches, $titles, $entities, $existing) {

    $database = \Drupal::database();
    $field_table = 'tripal_entity__' . $field_name;

    $batch_size = 1000;
    $num_matches = $this->countFieldMatches($field_name, $matches);
    $num_batches = (int) ($num_matches / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Generate the insert SQL and add to it the field-specific columns.
    $init_sql = "
      INSERT INTO {$field_table}
        (bundle, deleted, entity_id, revision_id, langcode, delta, ";
    foreach (array_keys($this->required_types[$field_name]) as $key) {
      $init_sql .= $field_name . '_'. $key . ', ';
    }
    $init_sql = rtrim($init_sql, ", ");
    $init_sql .= ") VALUES\n";

    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];

    // Iterate through the matches.
    foreach ($matches as $match) {
      $title = $titles[$total];
      $entity_id = $entities[$title];

      $num_delta = count(array_keys($match[$field_name]));
      for ($delta = 0; $delta < $num_delta; $delta++) {
        $j++;
        $total++;

        // No need to add items to those that are already published.
        if (array_key_exists($entity_id, $existing)) {
          continue;
        }

        // Add items to those that are not already published.
        $sql .= "(:bundle_$j, :deleted_$j, :entity_id_$j, :revision_id_$j, :langcode_$j, :delta_$j, ";
        $args[":bundle_$j"] = $this->bundle;
        $args[":deleted_$j"] = 0;
        $args[":entity_id_$j"] = $entity_id;
        $args[":revision_id_$j"] = 1;
        $args[":langcode_$j"] = 'und';
        $args[":delta_$j"] = $delta;
        foreach (array_keys($this->required_types[$field_name]) as $key) {
          $placeholder = ':' . $field_name . '_'. $key . '_' . $j;
          $sql .=  $placeholder . ', ';
          $args[$placeholder] = $match[$field_name][$delta][$key]['value']->getValue();
        }
        $sql = rtrim($sql, ", ");
        $sql .= "),\n";

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
  }

  /**
   * Removes existing records from the set of matched records.
   *
   * @param array $matches
   *   The array of matches for each entity.
   * @param array $titles
   *   The array of entity titles in the same order as the matches.
   * @param array $existing
   *   The array of existing records.
   *
   * @return array
   *   A new array of two elements: the matches and titles arrays
   *   but with existing records excluded.
   */
  protected function excludeExisting($matches, $titles, $existing) {
    $new_matches = [];
    $new_titles = [];

    $i = 0;
    foreach ($matches as $match) {
      $title = $titles[$i];
      if (!array_key_exists($title, $existing)) {
        $new_matches[] = $match;
        $new_titles[] = $title;
      }
      $i++;
    }

    return [$new_matches, $new_titles];
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

    $this->logger->notice("Step  1 of 6: Find matching records... ");
    $matches = $this->findMatches($this->datastore, $this->bundle);

    $this->logger->notice("Step  2 of 6: Generate page titles...");
    $titles = $this->getTitlesFromMatches($matches);

    $this->logger->notice("Step  3 of 6: Find existing published entities...");
    $existing = $this->findEntities($matches, $titles);

    // Exclude any matches that are already published. We
    // need to publish only new matches.
    list($new_matches, $new_titles) = $this->excludeExisting($matches, $titles, $existing);

    // Note: entities are not tied to any storage backend. An entity
    // references an "object".  The information about that object
    // is in the form of fields and can come from any number of data storage
    // backends. But, if the entity with a given title for this content type
    // doesn't exist, then let's create one.
    $this->logger->notice("Step  4 of 6: Publishing " . number_format(count($new_titles))  . " new entities...");
    $this->insertEntities($new_matches, $new_titles);

    $this->logger->notice("Step  5 of 6: Find IDs of entities...");
    $entities = $this->findEntities($matches, $titles);

    // Now we have to publish the field items. These represent storage back-end information
    // about the entity. If the entity was previously published we still may be adding new
    // information about it (say if we are publishing genes from a noSQL back-end but the
    // original entity was created when it was first published when using the Chado backend).
    $this->logger->notice("Step  6 of 6: Add field items to published entities...");

    if (!empty($this->unsupported_fields)) {
      $this->logger->warning("  The following fields are not supported by publish at this time: " . implode(', ', $this->unsupported_fields));
    }

    $total_items = 0;
    foreach ($this->field_info as $field_name => $field_info) {

      $this->logger->notice("  Checking for published items for the field: $field_name...");
      $existing_field_items = $this->findFieldItems($field_name, $entities);

      $num_field_items =  $this->countFieldMatches($field_name, $matches);
      $this->logger->notice("  Publishing " . number_format($num_field_items) . " items for field: $field_name...");
      $this->insertFieldItems($field_name, $matches, $titles, $entities, $existing_field_items);
      $total_items += $num_field_items;
    }
    $this->logger->notice("Published " .  number_format(count($new_matches)) . " new entities, and " . number_format($total_items) . " field values.");
    $this->logger->notice('Done');
    return $entities;
  }
}
