<?php

/**
 * Creates a collection of entities for a given user.
 *
 * @param  $details
 *   An association array containing the details for a collection. The
 *   details must include the following key/value pairs:
 *   - uid:  The ID of the user that owns the collection
 *   - collection_name:  The name of the collection
 *   - bundle_name:  The name of the TripalEntity content type.
 *   - ids:  An array of the entity IDs that form the collection.
 *   - fields: An array of the field IDs that the collection is limited to.
 *   - description:  A user supplied description for the collection.
 * @return TripalEntityCollection
 *   An instance of a TripalEntityCollection class or FALSE on failure.
 */
function tripal_create_collection($details) {
  try {
    $collection = new TripalEntityCollection();
    $collection_id = $collection->create($details);
    drupal_set_message(t("Collection '%name' created.", array('%name' => $details['collection_name'])));
  }
  catch (Exception $e) {
    drupal_set_message(t("Failed to create the collection '%name': " . $e->getMessage(), array('%name' =>  $details['collection_name'])), 'error');
    return FALSE;
  }
  return $collection;
}

/**
 * Retrieve a collection using the collection ID
 *
 * @param $collection_id
 *   The unique identifier for the collection.
 *
 * @return
 *  An instance of a TripalEntityCollection class or FALSE on failure.
 *
 */
function tripal_get_collection($collection_id) {
  try {
    $collection = new TripalEntityCollection();
    $collection->load($collection_id);
    return $collection;
  }
  catch (Exception $e) {
    drupal_set_message(t("Failed to load the collection with id '%id': " . $e->getMessage(), array('%id' =>  $collection_id)), 'error');
    return FALSE;
  }
}