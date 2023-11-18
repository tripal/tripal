<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Plugin implementation of string field type for Chado.
 *
 * @FieldType(
 *   id = "chado_string_type",
 *   label = @Translation("Chado String Field Type"),
 *   description = @Translation("A string field."),
 *   default_widget = "chado_string_type_widget",
 *   default_formatter = "chado_string_type_formatter"
 * )
 */
class ChadoStringTypeItem extends ChadoFieldItemBase {

  public static $id = "chado_string_type";

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
    $values = [];
    //$random = new Random();
    //$values['value'] = $random->word(mt_rand(1, $field_definition->getSetting('max_length')));
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();
    $settings = $this->getSetting('storage_plugin_settings');
    if ($max_length == $settings['max_length']) {
      $constraint_manager = \Drupal::typedDataManager()->getValidationConstraintManager();
      $constraints[] = $constraint_manager->create('ComplexData', [
        'value' => [
          'Length' => [
            'max' => $max_length,
            'maxMessage' => t('%name: may not be longer than @max characters.', [
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

    // Get the base table columns needed for this field.
    $base_table = $settings['base_table'];
    if (!$base_table) {
      return;
    }
    $base_column = $settings['base_column'];
    $max_length = $settings['max_length'];
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $value_term = $mapping->getColumnTermId($base_table, $base_column);

    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id,'record_id', $record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, "value", $value_term, $max_length, [
        'action' => 'store',
        'chado_table' => $base_table,
        'chado_column' => $base_column,
      ]),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);
    $storage_settings = $this->getSetting('storage_plugin_settings');
    $base_table = $form_state->getValue(['settings', 'storage_plugin_settings', 'base_table']);

    // Add an ajax callback so that when the base table is selected, the
    // base column select can be populated.
    $elements['storage_plugin_settings']['base_table']['#ajax'] = [
      'callback' =>  [$this, 'storageSettingsFormAjaxCallback'],
      'event' => 'change',
      'progress' => [
        'type' => 'throbber',
        'message' => $this->t('Retrieving table columns...'),
      ],
      'wrapper' => 'edit-base_column',
    ];

    $base_columns = $this->getTableColumns($base_table, ['text', 'character varying']);
    $elements['storage_plugin_settings']['base_column'] = [
      '#type' => 'select',
      '#title' => t('Table Column'),
      '#description' => t('Select the column in the base table that contains the field data'),
      '#options' => $base_columns,
      '#default_value' => $this->getSetting('base_column') ?? '',
      '#required' => TRUE,
      '#disabled' => $has_data or !$base_table,
      '#prefix' => '<div id="edit-base_column">',
      '#suffix' => '</div>',
    ];

    $elements['storage_plugin_settings']['max_length'] = [
      '#type' => 'number',
      '#title' => t('Maximum length'),
      '#default_value' => $this->getSetting('max_length'),
      '#required' => TRUE,
      '#description' => t('The maximum length of the field in characters.'),
      '#min' => 1,
      '#disabled' => $has_data,
    ];

    return $elements;
  }

  /**
   * Ajax callback to update the base column select. The select
   * can't be populated until we know the base table.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function storageSettingsFormAjaxCallback($form, &$form_state) {
    $response = new AjaxResponse();
    $response->addCommand(new ReplaceCommand('#edit-base_column', $form['settings']['storage_plugin_settings']['base_column']));
    return $response;
  }

}
