<?php
/**
 * The Controller for Tripal data type entities
 */
class TripalBundleController extends EntityAPIControllerExportable {
  public function __construct($entityType) {
    parent::__construct($entityType);
    // The 'bundle of' property is usually set in the hook_entity_info()
    // function for the "entity type" entity.  This allows the Entity API
    // to provide the user interface for managing fields attached to the
    // bundle.  But, we are using the same controller classes for
    // all entity types and we do not want new links for every
    // entity type (vocabulary) on the Administration >> Structure menu.
    // We just want one menu item.  So to support one menu item that
    // can handle all of the Tripal entity types, we have to set the
    // 'bundle of' property here rather than in the hook_entity_info() function.
    $bundle_of = $entityType;
    $bundle_of = preg_replace('/_bundle/', '', $bundle_of);
    $info = entity_get_info($bundle_of);
    $this->bundleKey = $info['bundle keys']['bundle'];
    $this->entityInfo['bundle of'] = $bundle_of;
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