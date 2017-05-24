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
  }
}