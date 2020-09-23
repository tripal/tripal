<?php

namespace Drupal\tripal_chado\TypedData;

use Drupal\Core\TypedData\TypedData;
use Drupal\Core\TypedData\TypedDataInterface;

/**
 * @{inheritdoc}
 */
class ChadoFieldValueLookup extends TypedData implements TypedDataInterface {

  /**
   * @{inheritdoc}
   */
  public function setValue($value, $notify = TRUE) {

    // Grab the record_id from the OBIOrganismItem.
    // This may grab the old value not the new one :-(.
    $property_name = $this->definition->getSetting('record_id');
    $item = $this->getParent();
    $record_id = (string) $item->get($property_name)->getValue();

    // Use the record_id to look up the new organism.
    if ($record_id) {
      $orgs = chado_query('SELECT * FROM {organism} WHERE organism_id=:id',
        [':id' => $record_id]);
      // @todo make sure we use the chado_schema

      // Now overwrite the old values (i.e. cache the new organism).
      foreach ($orgs as $organism) {
        $value = [
          'genus' => $organism->genus,
          'species' => $organism->species,
          'common_name' => $organism->common_name,
          'abbreviation' => $organism->abbreviation,
        ];
      }
    }

    // This is where we cache the data from chado in the Drupal Database.
    // We need to ensure we are storing a single string. Thus we serialize
    // the data at this point so that it's saved that way in the database.
    if (is_array($value)) {
      $this->value = serialize($value);
    }
    else {
      $this->value = $value;
    }
  }
}
