<?php

class chado_linker__prop extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Chado Property';

  // The default description for this field.
  public static $description = 'Add details about this property.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'property',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'property',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The primary key column of hte table in Dhado.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  );

  // The default widget for this field.
  public static $default_widget = 'chado_linker__prop_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__prop_formatter';

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $vocabulary = $this->instance['settings']['term_vocabulary'];
    $accession = $this->instance['settings']['term_accession'];
    $cvterm = tripal_get_cvterm(array(
      'dbxref_id' => array(
        'db_id' => array(
          'name' => $vocabulary,
        ),
        'accession' => $accession,
      ),
    ));
    $cvterm_id = $cvterm->cvterm_id;

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    if (!isset($schema['foreign keys'][$base_table]['columns'])) {
      return;
    }
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => $chado_record->$fkey_lcolumn,
      'chado-' . $field_table . '__value' => '',
      'chado-' . $field_table . '__type_id' => $cvterm_id,
      'chado-' . $field_table . '__rank' => '',
    );

    // Get the properties associated with this record for the given type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $chado_record->{$fkey_lcolumn},
      'type_id' => $cvterm_id,
    );
    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC')
    );
    $properties = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($properties); $i++) {
      $property = $properties[$i];
      foreach ($schema['fields'] as $fname => $details) {
        $entity->{$field_name}['und'][$i] = array(
          'value' => $property->value,
          'chado-' . $field_table . '__' . $pkey => $property->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $property->$fkey_lcolumn,
          'chado-' . $field_table . '__value' => $property->value,
          'chado-' . $field_table . '__type_id' => $property->type_id,
          'chado-' . $field_table . '__rank' => $property->rank,
        );
      }
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $prop_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $vocab = $this->instance['settings']['term_vocabulary'];
    $accession = $this->instance['settings']['term_accession'];

    $cvterm = tripal_get_cvterm(array('id' => $vocab . ':' . $accession));

    $this->queryJoinOnce($query, $prop_linker, $alias, "base.$bpkey = $alias.$bpkey");
    $query->condition("$alias.type_id", $cvterm->cvterm_id);
    $query->condition("$alias.value", $condition['value'], $operator);

  }

  /**
   * @see ChadoField::query()
   */
  public function queryOrder($query, $order) {
    $prop_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];
    $alias = $this->field['field_name'];

    $vocab = $this->instance['settings']['term_vocabulary'];
    $accession = $this->instance['settings']['term_accession'];

    $cvterm = tripal_get_cvterm(array('id' => $vocab . ':' . $accession));

    $this->queryJoinOnce($query, $prop_linker, $alias, "base.$bpkey = $alias.$bpkey AND $alias.type_id = $cvterm->cvterm_id", "LEFT OUTER");
    $query->orderBy("$alias.value", $order['direction']);
  }
}