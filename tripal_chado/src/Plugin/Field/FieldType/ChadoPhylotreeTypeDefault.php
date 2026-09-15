<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of default Tripal phylotree field type.
 */
#[TripalFieldType(
  id: 'chado_phylotree_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Phylogenetic Tree'),
  description: new TranslatableMarkup('Add a phylogenetic tree to the content type.'),
  default_widget: 'chado_phylotree_widget_default',
  default_formatter: 'chado_phylotree_formatter_default',
)]
class ChadoPhylotreeTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = 'chado_phylotree_type_default';

  /**
   * The chado table which is the object of the relationship.
   *
   * Note: this should be in all fields linking a base table to another
   * main chado table (i.e. object table).
   *
   * @var string
   */
  protected static $object_table = 'phylotree';

  /**
   * The foreign key that links the linking table to the object table.
   *
   * Note: this should be in all fields linking a base table to another
   * main chado table (i.e. object table).
   *
   * @var string
   */
  protected static $object_id = 'phylotree_id';

  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    // The property that indicates if this field is empty.
    return self::$object_id;
  }

  /**
   * {@inheritdoc}
   */
  public static function mainDisplayPropertyName() {
    // The property to use in the entity title/url.
    return 'phylotree_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $storage_settings = parent::defaultStorageSettings();
    $storage_settings['storage_plugin_settings']['linking_method'] = '';
    $storage_settings['storage_plugin_settings']['linker_table'] = '';
    $storage_settings['storage_plugin_settings']['linker_fkey_column'] = '';
    $storage_settings['storage_plugin_settings']['object_table'] = self::$object_table;
    return $storage_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $field_settings = parent::defaultFieldSettings();
    // CV Term is 'Phylogenetic tree' in 'EDAM' CV.
    $field_settings['termIdSpace'] = 'data';
    $field_settings['termAccession'] = '0872';
    return $field_settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    // Get the Chado table and column this field maps to.
    $settings = $field_definition->getSettings();
    $storage_settings = $settings['storage_plugin_settings'];
    $base_table = $storage_settings['base_table'];
    $linker_table = array_key_exists('linker_table', $storage_settings) ? $storage_settings['linker_table'] : $base_table;

    $value['record_id'] = 0;
    $value['entity_id'] = 0;
    $value[self::$object_id] = 0;
    if ($base_table !== $linker_table) {
      $value['linker_id'] = 0;
      $value['link'] = 0;
      $value[self::$object_id] = 0;

      // Do we want to conditionally include type_id and rank?
      $value['linker_type_id'] = mt_rand(1, 500);
      $value['linker_rank'] = 0;
    }

    // Object table properties.
    $value['phylotree_type_id'] = 1;
    $value['phylotree_name'] = '';
    $value['phylotree_comment'] = '';
    $value['phylotree_analysis_id'] = 0;

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

    // Get the various tables and columns needed for this field.
    // We will get the terms by using the Chado table columns they map to.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Base table.
    $base_pkey_col = self::getPrimaryKey($base_table, $schema);

    // Object table.
    $object_table = self::$object_table;
    $object_schema_def = self::getChadoTableDef($object_table, $schema);
    $object_pkey_col = $object_schema_def['primary key'];

    // Columns specific to the object table.
    $comment_term = self::getColumnTermId($object_table, 'comment', 'schema:comment');
    $name_term = self::getColumnTermId($object_table, 'name', 'schema:name');
    $name_len = $object_schema_def['fields']['name']['size'];

    // Cvterm table, to retrieve the name for the phylotree type.
    $cvterm_schema_def = self::getChadoTableDef('cvterm', $schema);
    $type_term = self::getColumnTermId('cvterm', 'name', 'schema:additionalType');
    $type_len = $cvterm_schema_def['fields']['name']['size'];

    // Analysis table, to retrieve the name of the analysis.
    $analysis_schema_def = self::getChadoTableDef('analysis', $schema);
    $analysis_term = self::getColumnTermId('analysis', 'name', 'schema:name');
    $analysis_len = $analysis_schema_def['fields']['name']['size'];

    // Columns from linked tables.
    $dbxref_schema_def = self::getChadoTableDef('dbxref', $schema);
    $dbxref_term = self::getColumnTermId('dbxref', 'accession', 'data:2091');
    $dbxref_len = $dbxref_schema_def['fields']['accession']['size'];
    $db_schema_def = self::getChadoTableDef('db', $schema);
    $db_term = self::getColumnTermId('db', 'name', 'ERO:0001716');
    $db_len = $db_schema_def['fields']['name']['size'];

    // Linker table, when used, requires specifying the linker table and column.
    // Because this field handles the analysis base table specially, the value
    // of $linker_fkey_column will be ignored for that case.
    [$linker_table, $linker_fkey_column] = self::get_linker_table_and_column($storage_settings, $base_table, $object_pkey_col);

    $extra_linker_columns = [];
    $linker_fkey_term = self::getColumnTermId($base_table, $linker_fkey_column, self::$record_id_term);
    $linker_fkey_path = $base_table . '.' . $linker_fkey_column;
    if ($linker_table != $base_table) {
      $linker_schema_def = self::getChadoTableDef($linker_table, $schema);
      $linker_pkey_col = $linker_schema_def['primary key'];
      // The following should be the same as $base_pkey_col.
      $linker_left_col = self::getChadoForeignKeyColumn($linker_table, $base_table, $schema);
      $linker_left_term = self::getColumnTermId($linker_table, $linker_left_col, self::$record_id_term);
      $linker_fkey_term = self::getColumnTermId($linker_table, $linker_fkey_column, self::$record_id_term);
      $linker_fkey_path = $linker_table . '.' . $linker_fkey_column;

      // Some but not all linker tables contain rank, type_id, and maybe
      // other columns. These are conditionally added only if they exist in
      // the linker table, and if a term is defined for them.
      foreach (array_keys($linker_schema_def['fields']) as $column) {
        if (($column != $linker_pkey_col) and ($column != $linker_left_col) and ($column != $linker_fkey_column)) {
          $term = self::getColumnTermId($linker_table, $column, 'NCIT:C25712');
          if ($term) {
            $extra_linker_columns[$column] = $term;
          }
        }
      }
    }

    $properties = [];

    // Define the base table record id.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);

    // This property will store the Drupal entity ID of the linked chado
    // record, if one exists. Base table analysis has a special callback.
    $namespace = self::$chadostorage_namespace;
    $function = self::$drupal_entity_callback;
    if ($base_table == 'analysis') {
      $namespace = self::class;
      $function = 'phylotreeEntityLookupCallback';
    }
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'entity_id', self::$drupal_entity_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => $namespace,
      'function' => $function,
      'ftable' => self::$object_table,
      'fkey' => 'phylotree_id',
    ]);

    // Special case for the analysis table, we use a "reverse" link
    // in the path to get from analysis base table to phylotree. In
    // this case, linker_fkey_column will just be a placeholder.
    $path_root = $linker_fkey_path . '>phylotree.phylotree_id';
    if ($base_table == 'analysis') {
      $path_root = 'analysis.analysis_id>phylotree.analysis_id';
    }
    // Base table links directly.
    elseif ($base_table == $linker_table) {
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, self::$object_id, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $linker_fkey_path,
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
        'drupal_store' => FALSE,
        'path' => $base_table . '.' . $base_pkey_col . '>' . $linker_table . '.' . $linker_left_col,
      ]);

      // Define the link between the linker table and the object table.
      // E.g.:  project_contact.contact_id.
      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, $linker_fkey_column, $linker_fkey_term, [
        'action' => 'store',
        'drupal_store' => TRUE,
        'path' => $linker_table . '.' . $linker_fkey_column,
        'delete_if_empty' => TRUE,
        'empty_value' => 0,
      ]);

      // Other columns in the linker table.
      // Set in the widget, but currently not implemented in the formatter.
      // Typically these are type_id and rank, but are not present in all
      // linker tables, so they are added only if present in the linker table.
      foreach ($extra_linker_columns as $column => $term) {
        $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_' . $column, $term, [
          'action' => 'store',
          'drupal_store' => FALSE,
          'path' => $linker_table . '.' . $column,
          'as' => 'linker_' . $column,
        ]);
      }
    }

    // The object table, the destination table of the linker table
    // The phylotree name.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'phylotree_name', $name_term, $name_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $path_root . ';name',
      'as' => 'phylotree_name',
    ]);

    // The phylotree description.
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'phylotree_comment', $comment_term, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $path_root . ';comment',
      'as' => 'phylotree_comment',
    ]);

    // The phylotree type.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'phylotree_type', $type_term, $type_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $path_root . ';' . $object_table . '.type_id>cvterm.cvterm_id;name',
      'as' => 'phylotree_type',
    ]);

    // The linked analysis name.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'phylotree_analysis_name', $analysis_term, $analysis_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $path_root . ';' . $object_table . '.analysis_id>analysis.analysis_id;name',
      'as' => 'phylotree_analysis_name',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'phylotree_database_accession', $dbxref_term, $dbxref_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $path_root . ';' . $object_table . '.dbxref_id>dbxref.dbxref_id;accession',
      'as' => 'phylotree_database_accession',
    ]);

    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'phylotree_database_name', $db_term, $db_len, [
      'action' => 'read_value',
      'drupal_store' => FALSE,
      'path' => $path_root . ';' . $object_table . '.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id;name',
      'as' => 'phylotree_database_name',
    ]);

    return $properties;
  }

  /**
   * {@inheritDoc}
   *
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = FALSE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $linker_tables = $this->getLinkerTables(self::$object_table, $base_table);
    if (count($linker_tables) > 0) {
      $compatible = TRUE;
    }

    // Analysis table is handled as a special case.
    if ($base_table == 'analysis') {
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
      'table' => self::$object_table,
      'label' => 'Phylogenetic Tree',
      'termIdSpace' => 'data',
      'termAccession' => '0872',
      'description' => 'The raw data (not just an image) from which a phylogenetic tree is directly generated or plotted, such as topology, lengths (in time or in expected amounts of variance) and a confidence interval for each length.',
    ];

    // Call the parent discover() with this field's specific options.
    $field_list = parent::discover($bundle, $field_id, $field_types, $field_instances, $options);

    // Handle analysis table as a special case.
    $base_table = $bundle->getThirdPartySetting('tripal', 'chado_base_table');
    if ($base_table == 'analysis') {
      $field_list[] = [
        'name' => 'analysis_phylotree',
        'content_type' => $bundle->id(),
        'label' => $options['label'],
        'type' => self::$id,
        'description' => $options['description'],
        'cardinality' => -1,
        'required' => FALSE,
        'storage_settings' => [
          'storage_plugin_id' => 'chado_storage',
          'storage_plugin_settings' => [
            'base_table' => 'analysis',
            'linker_table' => 'analysis',
            'linker_fkey_column' => "\u{2605}reverse_link",
          ],
        ],
        'settings' => [
          'termIdSpace' => $options['termIdSpace'],
          'termAccession' => $options['termAccession'],
          'id_space_plugin_id' => 'chado_id_space',
          'vocabulary_plugin_id' => 'chado_vocabulary',
        ],
        'display' => [
          'view' => [
            'default' => [
              'region' => 'content',
              'label' => 'above',
              'weight' => 90,
            ],
          ],
          'form' => [
            'default' => [
              'region' => 'content',
              'weight' => 90,
            ],
          ],
        ],
      ];
    }

    return $field_list;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = parent::storageSettingsForm($form, $form_state, $has_data);

    // We need to include the "analysis" table as a special case
    // for the base table. Sort alphabetically after adding it.
    $elements['storage_plugin_settings']['base_table']['#options']['analysis'] = 'analysis';
    ksort($elements['storage_plugin_settings']['base_table']['#options']);
    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  protected function getLinkerTableSelectOptions($object_table, $base_table, $delimiter = '') {
    // Override the parent version of this function to includes the
    // special case of the analysis base table.
    $select_options = parent::getLinkerTableSelectOptions($object_table, $base_table);

    // For the analysis base table, the parent function determined that
    // no link is possible, but we know better since we will handle this
    // specially. The right side is just a placeholder value that only
    // the widget actually checks. u2605 = ★.
    if ($base_table == 'analysis') {
      $special_link = 'analysis' . $delimiter . "\u{2605}reverse_link";
      $select_options = [$special_link => $special_link];
    }
    return $select_options;
  }

  /**
   * Looks up the Drupal entity ID for a phylotree, given analysis_id.
   *
   * We do not support more than one tree linking to an analysis because
   * each tree should have a dedicated analysis to specify all parameters.
   * Combine related trees with a project, publication, or other content type.
   *
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   * @return int
   *   The phylotree_id value of the corresponding record, or -1 if none.
   */
  public static function phylotreeEntityLookupCallback($context) {
    // In the TripalEntity class, the preSave function will flag all falsey
    // property values for deletion when drupal_store is set to TRUE.
    // To get around this, indicate the lack of a Drupal entity with a -1.
    $entity_id = -1;
    $field_name = $context['field_name'];
    $delta = $context['delta'];
    $analysis_id = $context['values'][$field_name][$delta]['record_id']['value'] ?? NULL;
    if ($analysis_id) {
      $analysis_id = $analysis_id->getValue('value');
      if ($analysis_id) {
        // Look up phylotree records with this analysis_id.
        $chado_connection = \Drupal::service('tripal_chado.database');
        $query = $chado_connection->select('1:phylotree', 'T');
        $query->condition('T.analysis_id', $analysis_id, '=');
        $query->fields('T', ['phylotree_id']);
        $query->orderBy('phylotree_id');
        $results = $query->execute();
        $phylotree_ids = [];
        while ($result = $results->fetchObject()) {
          $phylotree_ids[] = $result->phylotree_id;
        }

        // @todo is there any way to use cardinality so this is not a problem?
        if (count($phylotree_ids) > 1) {
          \Drupal::messenger()->addWarning('More than one phylotree is linked to an analysis, published values may be incorrect');
        }

        // Look up the entity_id.
        if ($phylotree_ids[$delta] ?? NULL) {
          $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
          $id = $lookup_manager->getEntityId($phylotree_ids[$delta], NULL, NULL, 'phylotree');
          if ($id) {
            $entity_id = $id;
          }
        }
      }
    }
    return $entity_id;
  }

}
