<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\Attribute\TripalFieldType;

/**
 *
 */
#[TripalFieldType(
  id: 'chado_relationship_by_role_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Relationship: Specific Role'),
  description: new TranslatableMarkup('Supports linking relationships fullfilling a specific role to the current content type.'),
  default_widget: 'chado_relationship_widget_default',
  default_formatter: 'chado_relationship_formatter_default',
)]
class ChadoRelationshipByRoleTypeDefault extends ChadoFieldItemBase {
  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = 'chado_contact_by_role_type_default';

  /**
   * Indicate if we should provide a column selector in the add field form.
   *
   * @var bool
   *   If TRUE then provide the select element for the column; if FALSE don't.
   * @see ChadoFieldItemBase
   */
  protected static $select_base_column = TRUE;

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // The property that indicates if this field is empty.
    return 'linker_id';
  }

  /**
   * {@inheritdoc}
   */
  public static function mainDisplayPropertyName() {
    // The property to use in the entity title/url.
    return 'value';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    // If this field needs to set a fixed value, set this to TRUE.
    // It indicates to the publishing step to include this field.
    // If not set, then the publishing step may not be able to find matches
    // for this field based on the fixed value.
    $settings['fixed_value'] = FALSE;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['base_column'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['subject_column'] = '';
    $storage_settings['storage_plugin_settings']['object_column'] = '';
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $value['record_id'] = 0;
    $value['linker_id'] = 0;

    $value['subject_id'] = 0;
    $value['subject_entity_id'] = 0;
    $value['subject_name'] = '';

    $value['object_id'] = 0;
    $value['object_entity_id'] = 0;
    $value['object_name'] = '';

    $value['type_id'] = mt_rand(1, 500);
    $value['type_name'] = '';

    $value['relationship_value'] = '';
    $value['relationship_rank'] = 0;

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];

    if (empty($base_table)) {
      return;
    }

    $terms = [
      'record_id' => self::$record_id_term,
    ];
    $max_lengths = [];

    $schemaObj = \Drupal::service('tripal_chado.database')->schema();
    $mappingObj = \Drupal::entityTypeManager()->getStorage('chado_term_mapping')->load('core_mapping');
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {

    // Get the base table for the content type.
    $has_linker = FALSE;
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    // Relationship tables have a standard naming method.
    $relationship_table = $base_table . '_relationship';
    $table_exists = $schema->tableExists($relationship_table);
    if ($table_exists) {
      $has_linker = TRUE;
    }

    // Ensure that the relationship table has a type_id.
    $has_type_id = FALSE;

    // Check there is a type_id field.
    $has_type_id = $schemaObj->fieldExists($relationship_table, 'type_id');
    if (!$has_type_id) {
      \Drupal::messenger()->addError('The Relationship By Role field requires a type_id in the linking table. This is not present in Chado 1.31 but will likely be added in subsequent versions.');
    }

    // Only compatible if there is a linker and it has a type_id.
    if ($has_linker and $has_type_id) {
      return TRUE;
    }
    return FALSE;
  }

}
