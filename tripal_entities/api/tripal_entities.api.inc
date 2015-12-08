<?php


/**
 * Creates a new Tripal Entity type (i.e. bundle).
 *
 * @param $cvterm
 *  A cvterm object created using the tripal_get_cvterm() function.
 * @param $error
 *  A string, passed by reference, that is filled with the error message
 *  if the function fails.
 *
 * @return
 *  TRUE if the entity type (bundle) was succesfully created.  FALSE otherwise.
 */
function tripal_create_entity_type($cvterm, &$error = '') {

  $bundle_id = 'bio-data_' . $cvterm->cvterm_id;

  // Check to see if this bundle exists. If not then create it
  $bundle = db_select('tripal_bundle', 't')
    ->fields('t')
    ->condition('type', 'TripalEntity')
    ->condition('bundle', $bundle_id)
    ->execute()
    ->fetchObject();

  if (!$bundle) {
    // Inser the bundle.
    db_insert('tripal_bundle')
      ->fields(array(
        'label' => $cvterm->name,
        'type' => 'TripalEntity',
        'bundle' => $bundle_id,
        'data' => serialize($bundle_data),
        'module' => 'tripal_entities'
      ))
      ->execute();
  }

  // Clear the entity cache so that Drupal will read our
  // hook_entity_info() implementation.
  global $language;
  $langcode = $language->language;
  cache_clear_all("entity_info:$langcode", 'cache');
  variable_set('menu_rebuild_needed', TRUE);


  // Allow modules to now add fields to the bundle
  module_invoke_all('add_bundle_fields', 'TripalEntity', $bundle_id, $cvterm);

  return TRUE;
}


/**
 * Allows a module to add key/value pairs about a bundle.
 *
 * If a module needs to associate variables with a particular TripalEntity
 * type (bundle), it can do so by setting the $bundle_data array passed into
 * this function.  This hook is called prior to creation of a new entity type.
 *
 * @param $bundle_data
 *   An array for key/value pairs to be associated with a bundle.
 * @param $bundle_id
 *   The ID for the bundle.
 * @param $cvterm
 *   The CV term object that the bundle represents.
 */
function hook_tripal_bundle_data_alter(&$bundle_data, $bundle_id, $cvterm) {
  // Get the cvterm for this entity type.
  $bundle_id = $entity->bundle;
  $cvterm_id = preg_replace('/bio-data_/', $bundle_id);
  $cvterm = tripal_get_cv(array('cvterm_id' => $cvterm_id));

  // Add any key/value pairs to the $bundle_data array as desired.
}