<?php

namespace Drupal\tripal_chado\Plugin\Field;

use Drupal\Component\Utility\Html;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Plugin\Field\TripalFormatterBase;

/**
 * A Tripal-based entity field formatter.
 */
abstract class ChadoFormatterBase extends TripalFormatterBase {

  /**
   * Retrieve a specific value from the items list.
   *
   * @param array $item
   *   The values for this field on the current entity.
   * @param string $property_name
   *   The name of the value or property you would like to pull out. Supported
   *   values include record_id, chado_schema, etc.
   */
  public function getChadoValue($item, $property_name) {

    if ($property_name == 'record_id') {
      return $item->get('record_id')->getValue();
    }
    elseif ($property_name == 'chado_schema') {
      return $item->get('chado_schema')->getValue();
    }
    else {
      $values = unserialize($item->getValue());
      if (isset($values[$property_name])) {
        return $values[$property_name];
      }
    }
    return NULL;
  }
}
