<?php

namespace Drupal\tripal_image\Plugin\Field\FieldType;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Chado image field type.
 */
#[FieldType(
  id: 'chado_eimage_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Image'),
  description: new TranslatableMarkup('Biological or biomedical data has been rendered into an image, typically for display on screen.'),
  default_widget: 'chado_eimage_widget_default',
  default_formatter: 'chado_eimage_formatter_default',
)]
class ChadoEimageTypeDefault extends ChadoFieldItemBase {

  /**
   * The machine name of this field.
   *
   * @var string
   */
  public static $id = 'chado_eimage_type_default';

  /**
   * The name of the table linked to from the base table.
   *
   * @var string
   */
  protected static $object_table = 'eimage';

  /**
   * The name of the primary key column in the object table.
   *
   * @var string
   */
  protected static $object_id = 'eimage_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // The property that indicates if this field is empty.
    return self::$object_id;
  }

#                                          Table "chado.eimage"
#   Column    |          Type          | Collation | Nullable |                  Default
#-------------+------------------------+-----------+----------+-------------------------------------------
# eimage_id   | bigint                 |           | not null | nextval('eimage_eimage_id_seq'::regclass)
# eimage_data | text                   |           |          |
# eimage_type | character varying(255) |           | not null |
# image_uri   | character varying(255) |           |          |
#COMMENT ON COLUMN eimage.eimage_data IS 'We expect images in eimage_data (e.g. JPEGs) to be uuencoded.';
#COMMENT ON COLUMN eimage.eimage_type IS 'Describes the type of data in eimage_data.';
#    -   name: 'eimage'
#        columns:
#            -   name: 'eimage_id'
#                term_id: 'data:2968'
#                term_name: 'Image'
#            -   name: 'eimage_data'
#                term_id: 'IAO:0000101'
#                term_name: 'image'
#            -   name: 'eimage_type'
#                term_id: 'IAO:0000098'
#                term_name: 'data format specification'
#            -   name: 'image_uri'
#                term_id: 'data:1047'
#                term_name: 'URI'

  /**
   * {@inheritdoc}
   */
  public static function mainDisplayPropertyName() {
    // The property to use in the entity title/url.
    return 'eimage_type';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'EDAM:Image'.
    $field_settings['termIdSpace'] = 'data';
    $field_settings['termAccession'] = '2968';
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];
    $value['record_id'] = 0;
    $value['entity_id'] = 0;
    $value['linker_id'] = 0;
    $value['link'] = 0;
    $value[self::$object_id] = 0;

    // Do we want to conditionally include type_id and rank?
    $value['linker_type_id'] = mt_rand(1, 500);
    $value['linker_rank'] = 0;

    // Object table properties.
    $value['eimage_data'] = '';
    $value['eimage_type'] = '';
    $value['image_uri'] = '';

    return [$value];
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Create a variable for easy access to settings.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the various tables and columns needed for this field. We will
    // get the property terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Base table.
    $base_pkey_col = self::getPrimaryKey($base_table, $schema);

    // Object table.
    $object_table = self::$object_table;
    $object_pkey_col = self::getPrimaryKey($object_table, $schema);

    // Cvterm table, to retrieve the name for the linker type.
    $cvterm_schema_def = self::getChadoTableDef('cvterm', $schema);
    $cvterm_name_term = self::getColumnTermId('cvterm', 'name', 'schema:additionalType');
    $cvterm_name_len = $cvterm_schema_def['fields']['name']['size'];

    // Term for multiple json-encoded properties from eimageprop table.
    $property_term = 'schema:description';

    // Columns specific to the object table.
    $object_schema_def = self::getChadoTableDef($object_table, $schema);
    $data_term = self::getColumnTermId($object_table, 'eimage_data', 'IAO:0000101');
    $type_term = self::getColumnTermId($object_table, 'eimage_type', 'IAO:0000098');
    $type_len = $object_schema_def['fields']['eimage_type']['size'];
    $uri_term = self::getColumnTermId($object_table, 'image_uri', 'data:1047');
    $uri_len = $object_schema_def['fields']['image_uri']['size'];
    // Linker table, when used, requires specifying the linker table and column.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);

    if ($linker_table != $base_table) {
      $linker_pkey_col = self::getPrimaryKey($linker_table, $schema);
      // The following should be the same as $base_pkey_col.
      $linker_left_col = self::getChadoForeignKeyColumn($linker_table, $base_table, $schema);
      $linker_left_term = self::getColumnTermId($linker_table, $linker_left_col, self::$record_id_term);
      $linker_fkey_term = self::getColumnTermId($linker_table, $linker_fkey_column, self::$record_id_term);
      $linker_type_id_term = self::getColumnTermId($linker_table, 'type_id', 'schema:additionalType');
      $linker_rank_term = self::getColumnTermId($linker_table, 'rank', 'OBCS:0000117');
    }
    else {
      $linker_fkey_term = self::getColumnTermId($base_table, $linker_fkey_column, self::$record_id_term);
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the Drupal entity ID of the linked chado
    // record, if one exists.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => self::$chadostorage_namespace,
      'function' => self::$drupal_entity_callback,
      'ftable' => self::$object_table,
      'fkey' => $linker_fkey_column,
    ]);

    // Base table links directly.
    if ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $linker_fkey_column, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $linker_fkey_column,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);
    }
    // An intermediate linker table is used.
    else {
      // Define the linker table that links the base table to the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', self::$record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'path' => $linker_table . '.' . $linker_pkey_col,
      ]);

      // Define the link between the base table and the linker table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link', $linker_left_term, [
        'action' => 'store_link',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col,
      ]);

      // Define the link between the linker table and the object table.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $linker_table . '.' . $linker_fkey_column,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);

      // Linker type.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_type_id', $linker_type_id_term, [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.type_id',
        'as' => 'linker_type_id',
      ]);

      // The name of the linker type.
      $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'linker_type_name', $cvterm_name_term, $cvterm_name_len, [
        'action' => 'read_value',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.type_id>cvterm.cvterm_id;name',
        'as' => 'linker_type_name',
      ]);

      // Linker rank.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_rank', $linker_rank_term, [
        'action' => 'store',
        'drupal_store' => FALSE,
        'path' => $linker_table . '.rank',
        'as' => 'linker_rank',
      ]);
    }

    // The object table, the destination table of the linker table.
    // The image data if the image is stored directly in the eimage table.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'eimage_data', $data_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';eimage_data',
      'as' => 'eimage_data',
    ]);

    // The data format of the eimage_data column. Not null.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'eimage_type', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';eimage_type',
      'as' => 'eimage_type',
    ]);

    // The URI where the image can be found.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'image_uri', $uri_term, $uri_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $linker_table . '.' . $linker_fkey_column . '>' . $object_table . '.' . $object_pkey_col . ';image_uri',
      'as' => 'image_uri',
    ]);

    // Retrieve all properties from the eimageprop table.
    // __CLASS__ resolves to this class.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'eimage_properties', $property_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => __CLASS__,
      'function' => 'getAllProperties',
    ]);

    return $properties;
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
    $linker_tables = $this->getLinkerTables(self::$object_table, $base_table);
    if (count($linker_tables) < 1) {
      $compatible = FALSE;
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
      'table' => self::$object_table,
      'label' => 'Image',
      'termIdSpace' => 'data',
      'termAccession' => '2968',
      'description' => 'Biological or biomedical data has been rendered into an image, typically for display on screen.',
      'cardinality' => -1,
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);

    return $field_list;
  }

  /**
   * Retrieves all properties for a given image and stores as json.
   *
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   * @return string
   *   A tree representation in newick format.
   */
  public static function getAllProperties(array $context): string {

    // This will hold each of the tripalTypes values.
    $field_name = $context['field_name'];
    $delta = $context['delta'];
    $values = $context['values'][$field_name][$delta];

    // This retrieves the eimage_id value.
    $eimage_id = $values['eimage_id']['value']->getValue();

    // This will retrieve all properties for this image.
    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select('1:eimageprop', 'P');
    $query->join('1:cvterm', 'T', '"P".type_id = "T".cvterm_id');
    $query->addField('T', 'name', 'name');
    $query->addField('P', 'value', 'value');
    $query->addField('P', 'rank', 'rank');
    $query->condition('P.eimage_id', $eimage_id, '=');
    $query->orderBy('P.rank');
    $results = $query->execute();

    // Convert to sorted json, or else return empty string.
    $results_array = [];
    foreach ($results as $result) {
      $results_array[$result->name][$result->rank] = $result->value;
    }
    if ($results_array) {
      uksort($results_array, 'strcasecmp');
      // We want the image legend to always sort to the beginning.
      if (array_key_exists('legend', $results_array)) {
        $results_array = ['legend' => $results_array['legend']] + $results_array;
      }
      $json = json_encode($results_array);
    }
    else {
      $json = '';
    }

    return $json;
  }

}
