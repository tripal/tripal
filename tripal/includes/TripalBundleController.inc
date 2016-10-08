<?php
/**
 * The Controller for Tripal data type entities
 */
class TripalBundleController extends EntityAPIControllerExportable {
  public function __construct($entityType) {
    parent::__construct($entityType);

  }

  /**
   * Create a type - we first set up the values that are specific
   * to our type schema but then also go through the EntityAPIController
   * function.
   *
   * @param $type
   *   The machine-readable type of the tripal data entity.
   *
   * @return
   *   A type object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our entity
    $values += array(
      'id' => '',
      'is_new' => TRUE,
      'data' => '',
    );
    return parent::create($values);
  }

  /**
   * Overrides the parent delete function.
   *
   * @param $ids
   * @param DatabaseTransaction $transaction
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $entities = $ids ? $this->load($ids) : FALSE;

    if (!$transaction) {
      $transaction = db_transaction();
    }

    if ($entities) {

      foreach ($entities as $id => $entity) {

        // Find any TripalEntity fields that are attached to this bundle and
        // remove them.
        $instances = field_info_instances('TripalEntity', $entity->name);
        foreach ($instances as $instance) {
          // Mark the instance as deleted and purge it.
          $field = field_info_field($instance['field_name']);
          field_delete_instance($instance);
          field_purge_instance($instance);

          // If the field has no more instances then purge it too.
          if (count($field['bundles']) == 1 and
              count($field['bundles']['TripalEntity']) == 1 and
              in_array($entity->name, $field['bundles']['TripalEntity'])) {
            field_purge_field($field);
          }
        }

        // Remove the entries in the chado_entity and tripal_entity t
        $query = db_select('chado_entity', 'ce');
        $query->join('tripal_entity', 'te', 'te.id = ce.entity_id');
        $records = $query->fields('ce', array('chado_entity_id', 'data_table', 'record_id'))
          ->condition('te.bundle', $entity->name)
          ->execute();
        $num_removed = 0;
        while ($record = $records->fetchObject()) {
          db_delete('chado_entity')
            ->condition('chado_entity_id', $record->chado_entity_id)
            ->execute();
          db_delete('tripal_entity')
            ->condition('id', $record->chado_entity_id)
            ->execute();
          $num_removed++;
        }
        if ($num_removed > 0) {
          drupal_set_message(t('Removed %num records', array('%num' => $num_removed)));
        }

        // Remove the terms for the bundles that are to be deleted.
        db_delete('tripal_term')
          ->condition('id', $entity->term_id)
          ->execute();
      }

      // Use the parent function to delete the bundles.
      parent::delete($ids, $transaction);

      // Not sure what this does, but copied from the
      // EntityAPIControllerExportable->delete() function which this one
      // overrides.
      foreach ($entities as $id => $entity) {
        if (entity_has_status($this->entityType, $entity, ENTITY_IN_CODE)) {
          entity_defaults_rebuild(array($this->entityType));
          break;
        }
      }
    }
  }
}