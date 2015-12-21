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

}