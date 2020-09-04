<?php

namespace Drupal\tripal_chado\Field;

use Drupal\tripal\Field\TripalFieldItemBase;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\TypedData\DataDefinitionInterface;
use Drupal\Core\TypedData\Plugin\DataType\Map;
use Drupal\Core\TypedData\TypedDataInterface;


/**
 * A Tripal-based entity field item.
 *
 * Entity field items making use of this base class have to implement
 * the static method propertyDefinitions().
 *
 */

abstract class ChadoFieldItemBase extends TripalFieldItemBase {
  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [
      // The table in Chado that the field maps to.
      'chado_table' => '',
      // The column of the table in Chado where the value comes from.
      'chado_column' => '',
      // The base table
      'base_table' => '',
    ] + parent::defaultFieldSettings();
    return $settings;
  }
}