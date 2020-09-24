<?php

namespace Drupal\tripal_chado\Plugin\Field;

use Drupal\Core\Field\WidgetBase;
use Drupal\tripal\Plugin\Field\TripalWidgetBase;

/**
 * A Chado-based entity field widget.
 */
abstract class ChadoWidgetBase extends TripalWidgetBase {

  /**
   * Extract the default record_id for the field.
   *
   * @return integer
   *   The record ID represented by the default value set in the field settings.
   */
  public function getDefaultRecordID() {
    return NULL;
  }

  /**
   * Retrieve a specific value from the items list.
   *
   * @todo move this into a ChadoWidgetBase class.
   *
   * @param array $items
   *   An array of default value items for the OBIOrganismItem field.
   * @param int $delta
   *   The index of the current item.
   * @param string $property_name
   *   The name of the value or property you would like to pull out. Supported
   *   values include record_id, chado_schema, etc.
   */
  public function getChadoValue($items, $delta, $property_name) {

    if ($property_name == 'record_id') {
      $record_id = $items[$delta]->get('record_id')->getValue();

      // Get a default value if it's not set?
      if (!$record_id) {
        $record_id = $this->getDefaultRecordID();
      }

      return $record_id;
    }
    elseif ($property_name == 'chado_schema') {
      return $items[$delta]->get('chado_schema')->getValue();
    }
    else {
      $values = unserialize($items[$delta]->getValue());
      if (isset($values[$property_name])) {
        return $values[$property_name];
      }
    }
    return NULL;
  }
}
