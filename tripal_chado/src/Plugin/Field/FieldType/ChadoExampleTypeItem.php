<?php declare(strict_types = 1);

namespace Drupal\tripal_chado\Plugin\Field\FieldType;


use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
// Make sure to include the Property type class you are going to create
// in your addTypes() method below.
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;

/**
 * Plugin implementation of the 'chado_example' field type.
 *
 * @FieldType(
 *   id = "chado_example",
 *   label = @Translation("Chado Example Field Type"),
 *   description = @Translation(""),
 *   default_widget = "chado_example_widget",
 *   default_formatter = "chado_example_formatter"
 * )
 */
class ChadoExampleTypeItem extends ChadoFieldItemBase {

  public static $id = "chado_example";

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $record_id_term = 'SIO:000729';

    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');

    return [
      // Add your chado property types here.
    ];
  }

}

