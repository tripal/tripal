<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal_chado\Services\ChadoPhylotree;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;

/**
 * Plugin implementation of the 'phylotreevisualization' field type for Chado.
 */
#[TripalFieldType(
  id: 'chado_phylotreevis_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Phylogenetic Visualization Field Type'),
  description: new TranslatableMarkup('Visualization of a phylogenetic tree.'),
  default_widget: 'chado_phylotreevis_widget_default',
  default_formatter: 'chado_phylotreevis_formatter_default',
  cardinality: 1,
)]
class ChadoPhylotreeVisTypeDefault extends ChadoFieldItemBase {

  /**
   * The machine name of this field.
   *
   * @var string
   */
  public static $id = "chado_phylotreevis_type_default";

  /**
   * The CV term used to store the formatting settings.
   *
   * @var string
   */
  public static $formatter_settings_term = 'NCIT:C85439';

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['base_table'] = 'phylotree';
    $storage_settings['storage_plugin_settings']['base_column'] = 'phylotree_id';
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'EDAM:Phylogenetic tree rendering'.
    $field_settings['termIdSpace'] = 'operation';
    $field_settings['termAccession'] = '0567';
    $field_settings['formatterSettingsTerm'] = self::$formatter_settings_term;
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'] ?? 'phylotree';
    $base_pkey_col = $storage_settings['base_column'] ?? 'phylotree_id';
    $prop_table = $base_table . 'prop';
    $prop_pkey_col = self::getPrimaryKey($prop_table, $schema);
    $prop_fk_col = self::getChadoForeignKeyColumn($prop_table, $base_table, $schema);
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $tree_vis_term = 'operation:0567';
    // This matches how ChadoPropertyTypeDefault generates aliases.
    $prop_alias = $prop_table . '_' . preg_replace('/[^a-z0-9]+/', '', strtolower(self::$formatter_settings_term));

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the newick tree representation.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'tree_data', $tree_vis_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      // __CLASS__ resolves to 'Drupal\tripal_chado\Plugin\Field\FieldType\ChadoPhylotreeVisTypeDefault'.
      'namespace' => __CLASS__,
      'function' => 'getTreeNewick',
    ]);

    // The remaining field properties link to the chado property holding
    // visualization settings.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'formatter_settings_id', self::$record_id_term, [
      'action' => 'store_pkey',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col . '>' . $prop_alias . '.' . $prop_pkey_col,
      'table_alias_mapping' => [$prop_alias => $prop_table],
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'formatter_settings_prop_fkey', self::$record_id_term, [
      'action' => 'store_link',
      'path' => $base_table . '.' . $base_pkey_col . '>' . $prop_alias . '.' . $prop_fk_col,
      'table_alias_mapping' => [$prop_alias => $prop_table],
    ]);
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'formatter_settings_type_id', 'schema:additionalType', [
      'action' => 'store',
      'path' => $base_table . '.' . $base_pkey_col . '>' . $prop_alias . '.' . $prop_fk_col . ';type_id',
      'table_alias_mapping' => [$prop_alias => $prop_table],
    ]);
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'formatter_settings_value', 'NCIT:C25712', [
      'action' => 'store',
      'path' => $base_table . '.' . $base_pkey_col . '>' . $prop_alias . '.' . $prop_fk_col . ';value',
      'table_alias_mapping' => [$prop_alias => $prop_table],
      'delete_if_empty' => TRUE,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'formatter_settings_rank', 'OBCS:0000117', [
      'action' => 'store',
      'path' => $base_table . '.' . $base_pkey_col . '>' . $prop_alias . '.' . $prop_fk_col . ';rank',
      'table_alias_mapping' => [$prop_alias => $prop_table],
    ]);

    return $properties;
  }

  /**
   * We need to set the type_id property value to match the cvterm_id.
   *
   * This ensures only the correct property with the correct type_id is
   * used to store the tree visualization settings.
   * To do this we'll override the tripalValuesTemplate() and give the
   * `type_id` property a specific fixed value.
   *
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalField\TripalFieldItemBase::tripalValuesTemplate()
   */
  public function tripalValuesTemplate($field_definition, $default_value = NULL) {
    $prop_values = parent::tripalValuesTemplate($field_definition, $default_value);
    $term_parts = explode(':', self::$formatter_settings_term, 2);
    $cvterm_instance = \Drupal::service('tripal_chado.chado_buddy')->createInstance('chado_cvterm_buddy', []);
    $cvterm_records = $cvterm_instance->getCvterm(['db.name' => $term_parts[0], 'dbxref.accession' => $term_parts[1]]);
    $settings_cvterm_id = $cvterm_records[0]->getValue('cvterm.cvterm_id');
    foreach ($prop_values as $index => $prop_value) {
      if ($prop_value->getKey() == 'formatter_settings_type_id') {
        $prop_values[$index]->setValue($settings_cvterm_id);
      }
    }
    return $prop_values;
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type): bool {
    $compatible = FALSE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    if ($base_table == 'phylotree') {
      $compatible = TRUE;
    }

    return $compatible;
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface::discover()
   */
  public static function discover(
    TripalEntityType $bundle,
    string $field_id,
    array $field_types,
    array $field_instances,
    array $options = [],
  ): array {

    // Specific settings for this field.
    $options += [
      'id' => self::$id,
      'name' => self::generateFieldName($bundle, 'phylotreevis'),
      'base_table' => 'phylotree',
      'base_column' => 'phylotree_id',
      'label' => 'Phylogenetic Tree Visualization',
      'termIdSpace' => 'operation',
      'termAccession' => '0567',
      'description' => 'Render or visualise a phylogenetic tree.',
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);
    return $field_list;
  }

  /**
   * Retrieves all phylonodes for one phylotree and converts to newick format.
   *
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   * @return string
   *   A tree representation in newick format.
   */
  public static function getTreeNewick(array $context): string {

    // This will hold each of the tripalTypes values.
    $field_name = $context['field_name'];
    $delta = $context['delta'];
    $values = $context['values'][$field_name][$delta];

    // This retrieves the phylotree_id value.
    $record_id = $values['record_id']['value']->getValue();

    // This will retrieve the phylonodes and convert to json format.
    $chado = \Drupal::service('tripal_chado.database');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
    $phylotree = new ChadoPhylotree($chado, $lookup_manager);
    $json = $phylotree->getTreeNewick($record_id);
    return $json;
  }

}
