<?php

class chado_linker__prop extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Chado Property';

  // The default description for this field.
  public static $description = 'Add details about this property.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
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
    // The number of default rows to show. The default is 1.
    'rows' => 1,
  ];

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
    $cvterm = chado_get_cvterm([
      'dbxref_id' => [
        'db_id' => [
          'name' => $vocabulary,
        ],
        'accession' => $accession,
      ],
    ]);
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
    $entity->{$field_name}['und'][0] = [
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => $chado_record->$fkey_lcolumn,
      'chado-' . $field_table . '__value' => '',
      'chado-' . $field_table . '__type_id' => $cvterm_id,
      'chado-' . $field_table . '__rank' => '',
    ];

    // Get the properties associated with this record for the given type.
    $columns = ['*'];
    $match = [
      $fkey_lcolumn => $chado_record->{$fkey_lcolumn},
      'type_id' => $cvterm_id,
    ];
    $options = [
      'return_array' => TRUE,
      'order_by' => ['rank' => 'ASC'],
    ];
    $properties = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($properties); $i++) {
      $property = $properties[$i];
      foreach ($schema['fields'] as $fname => $details) {
        $entity->{$field_name}['und'][$i] = [
          'value' => $property->value,
          'chado-' . $field_table . '__' . $pkey => $property->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $property->$fkey_lcolumn,
          'chado-' . $field_table . '__value' => $property->value,
          'chado-' . $field_table . '__type_id' => $property->type_id,
          'chado-' . $field_table . '__rank' => $property->rank,
        ];
      }
    }

    // Ensure there are no values if there are no properties.
    // This is necessary to make sure the field is not rendered when there are no properies.
    // @todo: We should revisit this in the future as none of the other fields do this.
    //        It was added here to make it easier to detect when the field was empty
    //        but in hindsight, we would just check $entity->{$field_name}['und'][$i]['value']
    //        in the formatter.
    if (empty($properties)) {
      unset($entity->{$field_name});
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

    $cvterm = chado_get_cvterm(['id' => $vocab . ':' . $accession]);

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

    $cvterm = chado_get_cvterm(['id' => $vocab . ':' . $accession]);

    $this->queryJoinOnce($query, $prop_linker, $alias, "base.$bpkey = $alias.$bpkey AND $alias.type_id = $cvterm->cvterm_id", "LEFT OUTER");
    $query->orderBy("$alias.value", $order['direction']);
  }
}
