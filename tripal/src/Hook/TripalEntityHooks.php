<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface;

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
      $bundle = $entity->bundle();

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

        // Update the entity values with the values returned by loadValues().
        $field_items = $entity->getFields();
        foreach ($field_items as $field_name => $items) {
          foreach ($items as $k => $item) {

            // If it is not a TripalField then skip it.
            if (!$item instanceof TripalFieldItemInterface) {
              continue;
            }
            $delta = $item->getName();
            $tsid = $item->tripalStorageId();

            // If the Tripal Storage Backend is not set on a Tripal-based field,
            // we log an error and not support the field. If developers want
            // to use Drupal storage for a Tripal-based field then they need to
            // indicate that by using our Drupal SQL Storage option OR by not
            // creating a Tripal-based field at all depending on their needs.
            if (empty($tsid)) {
              \Drupal::logger('tripal')->error('The Tripal-based field :field on
                this content type must indicate a TripalStorage backend and currently does not.',
                [':field' => $field_name]
              );
              continue;
            }

            // Create a new properties array for this field item.
            $prop_values = [];
            $prop_types = [];
            foreach ($values[$tsid][$field_name][$delta] as $key => $info) {
              $prop_values[] = $info['value'];
              $prop_types[] = $tripal_storages[$tsid]->getPropertyType($bundle, $field_name, $key);
            }

            // Now set the entity values for this field.
            $item->tripalLoad($item, $field_name, $prop_types, $prop_values, $entity);

            // Set the item back to the list.
            $items->set($k, $item);
          }
        }
      }
    }
  }

}
