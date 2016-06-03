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
    if ($entities) {


      foreach ($entities as $id => $entity) {
        $bundle = tripal_load_bundle_entity(array('id' => $id));

        // Remove the terms for the bundles that are to be deleted.
        db_delete('tripal_term')
          ->condition('id', $bundle->term_id)
          ->execute();

        // Remove the chado_entity records for this type.

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