<?php

/**
 * The Controller for Tripal data type entities
 */
class TripalBundleController extends EntityAPIControllerExportable {

  public function __construct($entityType) {
    parent::__construct($entityType);

  }

  /**
   * Create a type we first set up the values that are specific
   * to our type schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the tripal data entity.
   *
   * @return
   *   A type object with all default fields initialized.
   */
  public function create(array $values = []) {
    // Add values that are specific to our entity
    $values += [
      'id' => '',
      'is_new' => TRUE,
      'data' => '',
    ];
    $bundle = parent::create($values);

    // Allow modules to make additions to the entity when it's created.
    $modules = module_implements('bundle_create');
    foreach ($modules as $module) {
      $function = $module . '_bundle_create';
      $function($bundle, []);
    }

    return $bundle;
  }

  /**
   * Overrides the parent delete function.
   *
   * @param $ids
   * @param DatabaseTransaction $transaction
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $bundles = $ids ? $this->load($ids) : FALSE;

    if (!$transaction) {
      $transaction = db_transaction();
    }

    try {

      if ($bundles) {

        foreach ($bundles as $id => $bundle) {

          // Allow modules to perform actions when the bundle is deleted.
          $modules = module_implements('bundle_delete');
          foreach ($modules as $module) {
            $function = $module . '_bundle_delete';
            $function($bundle);
          }

          // Find any TripalEntity fields that are attached to this bundle and
          // remove them.
          $instances = field_info_instances('TripalEntity', $bundle->name);
          foreach ($instances as $instance) {
            // Mark the instance as deleted and purge it.
            $field = field_info_field($instance['field_name']);
            field_delete_instance($instance);
            field_purge_instance($instance);

            // If the field has no more instances then purge it too.
            if (count($field['bundles']) == 1 and
              count($field['bundles']['TripalEntity']) == 1 and
              in_array($bundle->name, $field['bundles']['TripalEntity'])
            ) {
              field_purge_field($field);
            }
          }

          // Remove any entities from the tripal_entity table.
          db_delete('tripal_entity')
            ->condition('bundle', $bundle->name)
            ->execute();

          // Remove the terms for the bundles that are to be deleted.
          db_delete('tripal_term')
            ->condition('id', $bundle->term_id)
            ->execute();
        }

        // Use the parent function to delete the bundles.
        parent::delete($ids, $transaction);

        // Not sure what this does, but copied from the
        // EntityAPIControllerExportable->delete() function which this one
        // overrides.
        foreach ($bundles as $id => $bundle) {
          if (entity_has_status($this->entityType, $bundle, ENTITY_IN_CODE)) {
            entity_defaults_rebuild([$this->entityType]);
            break;
          }
        }
      }
    } catch (Exception $e) {
      if ($transaction) {
        $transaction->rollback();
      }
      watchdog_exception('tripal', $e);
      throw $e;
      return FALSE;
    }
  }

  /**
   * Finds any orphaned entities associated with this bundle.
   *
   * An orphaned entity can occur if the module that created the entity
   * unknowingly lost its underlying record in its data store.  Such a case
   * could happen if someone directly removed the record from the data store
   * outside of the module's control. This function allows each module
   * to report if any orphans are missing for a given bundle type.
   *
   * @param integer $id
   *   The ID of the bundle.
   * @param bool $count
   *   Set to TRUE to return only a count of orphans.
   * @param integer $offset
   *   For paging of entities set this to the offset within the total count.
   * @param integer $limit
   *   For paging of entities set this to the total number to return.
   *
   * @return array|integer
   *  If $count == FALSE then an array of all entity IDs that are orphaned. If
   *  $count == TRUE then a single integer count value is returned.
   */
  public function findOrphans($id, $count = FALSE, $offset = 0, $limit = 0) {

    // Call the hook for modules to find their orphans
    $bundle = tripal_load_bundle_entity(['id' => $id]);


    // If a count is desired, we need to sum up the values returned by all.
    if ($count) {
      $response = module_invoke_all('bundle_find_orphans', $bundle, TRUE);
      $sum = 0;
      foreach ($response as $key => $value) {
        $sum += $value;
      }
      return $sum;
    }
    // Otherwise just return the entity ID.s
    else {
      $response = module_invoke_all('bundle_find_orphans', $bundle, FALSE, $offset, $limit);
      return $response;
    }
  }

  /**
   * Finds any orphaned entities associated with all bundles.
   *
   * An orphaned entity can occur if the module that created the entity
   * unknowingly lost its underlying record in its data store.  Such a case
   * could happen if someone directly removed the record from the data store
   * outside of the module's control. This function allows each module
   * to report if any orphans are missing for a given bundle type.
   *
   * @param bool $count
   *   Set to TRUE to return only a count of orphans.
   * @param integer $offset
   *   For paging of entities set this to the offset within the total count.
   * @param integer $limit
   *   For paging of entities set this to the total number to return.
   *
   * @return array|integer
   *  If $count == FALSE then an array of all entity IDs that are orphaned. If
   *  $count == TRUE then a single integer count value is returned.
   */
  public function findAllOrphans($count = FALSE, $offset = 0, $limit = 0) {
    $results = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->orderBy('label')
      ->execute();
    if ($count) {
      $response = 0;
    }
    else {
      $response = [];
    }
    while (($bundle_record = $results->fetchObject())) {
      $bid = $bundle_record->id;
      $bundle_response = $this->findOrphans($bid, $count, $offset, $limit);
      if (is_array($bundle_response)) {
        foreach ($bundle_response as $key => $value) {
          $response[] = $value;
        }
      }
      else {
        $response[] = $bundle_response;
      }
    }
    return $response;
  }


  /**
   * Deletes orphaned entities.
   *
   * An orphaned entity can occur if the module that created the entity
   * unknowingly lost its underlying record in its data store.  Such a case
   * could happen if someone directly removed the record from the data store
   * outside of the module's control. This function allows each module
   * to report if any orphans are missing for a given bundle type.
   *
   *
   * @param integer $id
   *   The ID of the bundle.
   * @param TripalJob $job
   *   An optional Tripal Job object. This is provided when this function is
   *   called using the Tripal Jobs system.
   *
   * @return integer
   *   The number of entitites that were cleaned up.
   */
  public function deleteOrphans(int $id, TripalJob $job = NULL) {

    $num_deleted = 0;
    $transaction = db_transaction();
    try {
      if ($id === 0) {
        return $this->deleteAllOrphans($job);
      }

      // Get the list of entities that need cleanup.
      $eids = $this->findOrphans($id, FALSE, 0, 0);
      $num_entities = count($eids);

      // Initialize the job count.
      if ($job) {
        $job->logMessage('Found !num orphaned entities.', ['!num' => $num_entities]);
        $job->setInterval(1);
        $job->setTotalItems($num_entities);
      }

      // If there are no entities then just return.
      if ($num_entities == 0) {
        return 0;
      }

      // Allow the modules to cleanup their records.
      $bundle = tripal_load_bundle_entity(['id' => $id]);
      $response = module_invoke_all('bundle_delete_orphans', $bundle, $eids, $job);

      // Now remove the entities.
      $num_deleted = db_delete('tripal_entity')
        ->condition('id', $eids, 'IN')
        ->execute();
    } catch (Exception $e) {
      $transaction->rollback();
      $err_msg = "Failed to remove orphans: " . $e->getMessage();
      if ($job) {
        $job->logMessage($err_msg, [], 'error');
      }
      else {
        drupal_set_message($erro_msg, 'error');
      }
      return 0;
    }

    return $num_deleted;

  }

  /**
   * Deletes orphaned entities from all bundles.
   *
   * An orphaned entity can occur if the module that created the entity
   * unknowingly lost its underlying record in its data store.  Such a case
   * could happen if someone directly removed the record from the data store
   * outside of the module's control. This function allows each module
   * to report if any orphans are missing for a given bundle type.
   *
   * @param TripalJob $job
   *   An optional Tripal Job object. This is provided when this function is
   *   called using the Tripal Jobs system.
   *
   * @return integer
   *   The number of entitites that were cleaned up.
   */
  public function deleteAllOrphans(TripalJob $job = NULL) {
    $num_deleted = 0;
    $transaction = db_transaction();

    try {
      $results = db_select('tripal_bundle', 'tb')
        ->fields('tb')
        ->orderBy('label')
        ->execute();
      $eids = $this->findAllOrphans(FALSE, 0, 0);
      $num_entities = count($eids);
      if ($job) {
        $job->logMessage('Found !num orphaned entities.', ['!num' => $num_entities]);
        $job->setInterval(1);
        $job->setTotalItems($num_entities);
      }

      if ($num_entities == 0) {
        return 0;
      }

      while (($bundle_record = $results->fetchObject())) {
        $num_bundle_del = $this->deleteOrphans($bundle_record->id);
        if ($job) {
          $job->addItemsHandled($num_bundle_del);
          $job->logMessage("Removed !num orphaned !label entities.", ['!num' => $num_bundle_del, '!label' => $bundle_record->label]);
        }
        $num_deleted += $num_bundle_del;
      }
    }
    catch (Exception $e) {
      $transaction->rollback();
      $err_msg = "Failed to remove orphans: " . $e->getMessage();
      if ($job) {
        $job->logMessage($err_msg, [], 'error');
      }
      else {
        drupal_set_message($err_msg, 'error');
      }
      return 0;
    }
    return $num_deleted;
  }
}
