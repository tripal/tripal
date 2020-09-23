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

    // This is where we will save/update chado.
    // But it's too late to change the record_id and value in the Drupal db.

    // First, make sure we have a string!
    // @serialize happening here?
    if (is_array($value)) {
      $this->value = serialize($value);
    }
    else {
      $this->value = $value;
    }
  }
}
