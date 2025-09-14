<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\tripal\Entity\TripalEntity;

/**
 * Implements hooks from the Drupal Entity API and focused on Tripal Content.
 */
class TripalEntityHooks {

  /**
   * Finish loading TripalEntities via the TripalStorage plugin API.
   *
   * This is called during SqlContentStorage::doLoadMultiple() and only for
   * entities queried from the database. After this point the entities will
   * be added to the persistent cache until they are invalidated.
   *
   * @param array $entities
   *   An array of TripalEntity objects to finish loading.
   */
  #[Hook('tripal_entity_storage_load')]
  public function tripalEntityStorageLoad(array &$entities) {

    // Iterate through the entities provided.
    foreach ($entities as &$entity) {

      // @todo it would be great to skip the entity entirely if it is
      // fully cached in Drupal.
      // Create a values array appropriate for `loadValues()`
      [$values, $tripal_storages] = TripalEntity::getValuesArray($entity, TRUE);

      // Only do the following if there are any values to load.
      if (!empty($values)) {
        // Call the loadValues() function for each storage type.
        $load_success = FALSE;
        foreach ($values as $tsid => $tsid_values) {
          try {
            // If this storage backend is cache-aware then only the values for
            // fields which have un-cached properties will be loaded here.
            $load_success = $tripal_storages[$tsid]->loadValues($tsid_values);
            if ($load_success) {
              $values[$tsid] = $tsid_values;
            }
          }
          catch (\Exception $e) {
            \Drupal::logger('tripal')->notice($e->getMessage());
            \Drupal::messenger()->addError('Cannot load the entity. See the recent ' .
                'logs for more details or contact the site administrator if you cannot ' .
                'view the logs.');
          }
        }

        TripalEntity::saveValuesArray($entity, $values, $tripal_storages);
      }
    }
  }

}
