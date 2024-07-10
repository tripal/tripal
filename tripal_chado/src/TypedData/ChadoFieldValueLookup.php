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

    // Grab the record_id from the Field Type.
    $property_name = $this->definition->getSetting('record_id');
    $item = $this->getParent();
    $record_id = (string) $item->get($property_name)->getValue();

    // Use the record_id to look up the new organism.
    if ($record_id) {
      $value = $item->selectChadoValue($record_id, $item);
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
