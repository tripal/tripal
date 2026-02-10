<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Component\Utility\Random;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Plugin implementation of string field type for Chado.
 */
#[TripalFieldType(
  id: 'chado_string_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado String Field Type'),
  description: new TranslatableMarkup('A text field with a maximum length.'),
  default_widget: 'chado_string_type_widget',
  default_formatter: 'chado_string_type_formatter',
  cardinality: 1,
)]
class ChadoStringTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = "chado_string_type_default";

  /**
   * Indicate if we should provide a column selector in the add field form.
   *
   * @var bool
   *   If TRUE then provide the select element for the column; if FALSE don't.
   * @see ChadoFieldItemBase
   */
  protected static $select_base_column = TRUE;

  /**
   * Indicate the types of base table columns this field can manage.
   *
   * @var array
   *   Simple list of valid base column types for this field to manage.
   * @see ChadoFieldItemBase
   */
  protected static $valid_base_column_types = ['character', 'character varying'];

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [];
    return $settings + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['max_length'] = 255;
    $settings['storage_plugin_settings']['base_column'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $random = new Random();
    $value['record_id'] = 0;
    $value['value'] = $random->sentences(3, TRUE);

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();
    // @todo this next line looks like a bug
    if ($max_length = $this->getSetting('max_length')) {
      $constraint_manager = \Drupal::typedDataManager()->getValidationConstraintManager();
      $constraints[] = $constraint_manager->create('ComplexData', [
        'value' => [
          'Length' => [
            'max' => $max_length,
            'maxMessage' => $this->t('%name: may not be longer than @max characters.', [
              '%name' => $this
                ->getFieldDefinition()
                ->getLabel(),
              '@max' => $max_length,
            ]),
          ],
        ],
      ]);
    }
    return $constraints;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    if (!$base_table) {
      return;
    }

    // Get the base table columns needed for this field.
    $max_length = $field_definition->getSetting('max_length');
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_pkey_col = self::getPrimaryKey($base_table, $schema);
    $base_column = $settings['base_column'];

    // Get the property terms by using the Chado table columns they map to.
    $value_term = self::getColumnTermId($base_table, $base_column, 'NCIT:C25712');

    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col,
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, $max_length, [
        'action' => 'store',
        'path' => $base_table . '.' . $base_column,
      ]),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);

    $elements['max_length'] = [
      '#type' => 'number',
      '#title' => $this->t('Maximum length'),
      '#default_value' => $this->getSetting('max_length'),
      '#required' => TRUE,
      '#description' => $this->t('The maximum length of the field in characters.'),
      '#min' => 1,
      '#disabled' => $has_data,
    ];

    return $elements;
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = TRUE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $table_columns = $this->getTableColumns($base_table, self::$valid_base_column_types);
    if (count($table_columns) < 1) {
      $compatible = FALSE;
    }
    return $compatible;
  }

}
