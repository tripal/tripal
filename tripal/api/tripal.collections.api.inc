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
 * @param $values
 *   An array of key/value pairs to uniquely identify a collection.  The
 *   following keys can be used:
 *   - collection_id:  The numeric value for the collection.
 *   - uid: The ID of the user that owns the collection. This key must
 *     always be used with the 'name' key.
 *   - name:  The name of the collection.  This key must always be used
 *     with the 'uid' key.
 *
 * @return
 *  An instance of a TripalEntityCollection class or FALSE on failure.
 *
 */
function tripal_get_collection($values) {
  $collection_id = array_key_exists('collection_id', $values) ? $values['collection_id'] : NULL;
  $uid = array_key_exists('uid', $values) ? $values['uid'] : NULL;
  $name = array_key_exists('name', $values) ? $values['name'] : NULL;

  if ($uid and !$name) {
    throw new Exception('tripal_get_collection: Missing the collection name when specifying the User ID.');
  }
  if (!$uid and $name) {
    throw new Exception('tripal_get_collection: Missing the User ID when specifying the collection name.');
  }
  if (!$collection_id and !$uid and !$name) {
    throw new Exception('tripal_get_collection: Missing a valid key.');
  }

  if ($name and $uid) {
    $collection_id = db_select('tripal_collection', 'tc')
      ->fields('tc', array('collection_id'))
      ->condition('uid', $uid)
      ->condition('collection_name', $name)
      ->execute()
      ->fetchField();
    if (!$collection_id) {
      throw new Exception('tripal_get_collection: The collection could not be found with the given uid and name.');
    }
  }

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