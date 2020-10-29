<?php

/**
 * @file
 *
 * Provides the API for working with Data collections. This includes creation,
 * expiration, and retrieval.
 */

/**
 * @defgroup tripal_data_collections_api Data Collections
 * @ingroup tripal_api
 * @{
 * New with Tripal v3 are data collections. A data collection can be compared
 * to a "shopping cart" system where site users can collect data of interest
 * to them, and use that data later for other purposes.  By default, Tripal
 * offers data collections with the default search interface provided by views.
 * This interface allows end-users to create data collections after searching
 * for items using the default search.  Once in a collection users can then
 * generate files for downloading or use by other tools that support data
 * collections.
 * @}
 */

/**
 * Creates a collection of entities for a given user.
 *
 * Use this function if you want to create a new collection with an bundle.
 * Otherwise, a new collection can also be created by creating an new instance
 * of a TripalEntityCollection object.
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
 *
 * @return TripalEntityCollection
 *   An instance of a TripalEntityCollection class or FALSE on failure.
 *
 * @ingroup tripal_data_collections_api
 */
function tripal_create_collection($details) {

  global $user;

  try {
    $collection = new TripalEntityCollection();
    $collection->create($details);
    $collection_id = $collection->getCollectionID();
    $collection->addBundle($details);


    drupal_set_message(t("Collection '%name' created with %num_recs record(s). Files for this collection are not yet created.  To generate files, check the !view.",
        [
          '%name' => $details['collection_name'],
          '%num_recs' => count($details['ids']),
          '!view' => l('files page', 'user/' . $user->uid . '/files'),
        ])
    );
    return $collection;
  } catch (Exception $e) {
    drupal_set_message(t("Failed to create the collection '%name': " . $e->getMessage(), ['%name' => $details['collection_name']]), 'error');
    return FALSE;
  }

}

/**
 * Retrieves an array of collections for a user.
 *
 * @param $uid
 *   The User ID
 *
 * @return
 *   An array of TripalEntityCollection objects.
 *
 * @ingroup tripal_data_collections_api
 */
function tripal_get_user_collections($uid) {
  if (!$uid) {
    throw new Exception('tripal_get_user_collections: Missing the $uid argument.');
  }
  $user = user_load($uid);
  if (!$user) {
    throw new Exception('tripal_get_user_collections: Invalid $uid provided.');
  }

  $collections = [];
  $results = db_select('tripal_collection', 'tc')
    ->fields('tc', ['collection_id'])
    ->condition('uid', $uid)
    ->execute();
  while ($collection_id = $results->fetchField()) {
    $collection = new TripalEntityCollection();
    $collection->load($collection_id);
    $collections[] = $collection;
  }
  return $collections;
}

/**
 * Deletes all collections that have surpassed their lifespan
 *
 * @ingroup tripal_data_collections_api
 */
function tripal_expire_collections() {
  $max_days = variable_get('tripal_data_collections_lifespan', 30);
  $ctime = time();

  $query = db_select('tripal_collection', 'tc');
  $query->fields('tc', ['collection_id']);
  $query->where("((($ctime - create_date) / 60) / 60) / 24 >= $max_days");
  $results = $query->execute();
  while ($collection_id = $results->fetchField()) {
    $collection = new TripalEntityCollection();
    $collection->load($collection_id);
    $collections[] = $collection;
    $collection->delete();
  }
  return $collections;
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
 * @ingroup tripal_data_collections_api
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
      ->fields('tc', ['collection_id'])
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
  } catch (Exception $e) {
    drupal_set_message(t("Failed to load the collection with id '%id': " . $e->getMessage(), ['%id' => $collection_id]), 'error');
    return FALSE;
  }
}

/**
 * Generates the downloadable files for a Collection
 *
 * @param TripalEntityCollection $collection
 *
 * @ingroup tripal_data_collections_api
 */
function tripal_create_collection_files($collection_id, TripalJob $job = NULL) {
  $collection = new TripalEntityCollection();
  $collection->load($collection_id);
  $collection->write(NULL, $job);
}
