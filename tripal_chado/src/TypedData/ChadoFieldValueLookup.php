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
