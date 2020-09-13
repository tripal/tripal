<?php

class schema__alternate_name extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Synonyms';

  // The default description for this field.
  public static $description = 'An alternative name (synonym or alias) to this record.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'alternateName',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'alternateName',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'schema__alternate_name_widget';

  // The default formatter for this field.
  public static $default_formatter = 'schema__alternate_name_formatter';

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;
    $base_table = $this->instance['settings']['base_table'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the PKey for this table
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
      'chado-' . $field_table . '__' . 'synonym_id' => '',
      'chado-' . $field_table . '__' . 'pub_id' => '',
      'chado-' . $field_table . '__' . 'is_current' => '',
      'chado-' . $field_table . '__' . 'is_internal' => '',
      'name' => '',
      'type_id' => '',
      // Ignore the synonym_sgml column for now.
    ];

    $linker_table = $base_table . '_synonym';
    $options = ['return_array' => 1];
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $synonyms_linker = $record->$linker_table;
    if (is_array($synonyms_linker) and count($synonyms_linker) > 0) {
      $i = 0;
      foreach ($synonyms_linker as $index => $linker) {
        $synonym = $linker->synonym_id;
        $entity->{$field_name}['und'][$i] = [
          'value' => $synonym->name,
          'chado-' . $field_table . '__' . $pkey => $linker->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn->$fkey_lcolumn,
          'chado-' . $field_table . '__' . 'synonym_id' => $synonym->synonym_id,
          'chado-' . $field_table . '__' . 'pub_id' => $linker->pub_id->pub_id,
          'chado-' . $field_table . '__' . 'is_current' => $linker->is_current,
          'chado-' . $field_table . '__' . 'is_internal' => $linker->is_internal,
          'name' => $synonym->name,
          'type_id' => $synonym->type_id->cvterm_id,
        ];
        $i++;
      }
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    $this->queryJoinOnce($query, $field_table, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn");
    $this->queryJoinOnce($query, 'synonym', $alias . '_SYN', $alias . "_SYN.synonym_id = $alias.synonym_id");
    $query->condition($alias . "_SYN.name", $condition['value']);
  }

  /**
   * @see ChadoField::query()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    $this->queryJoinOnce($query, $field_table, $alias, "base.$fkey_rcolumn = $alias.$fkey_lcolumn", "LEFT OUTER");
    $this->queryJoinOnce($query, 'synonym', $alias . '_SYN', $alias . "_SYN.synonym_id = $alias.synonym_id", "LEFT OUTER");
    $query->orderBy($alias . "_SYN.name", $order['direction']);
  }
}