<?php

class sbo__database_cross_reference extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Cross reference';

  // The default description for this field.
  public static $description = 'This record can be cross referenced with a record in
          another online database. This field is intended for one or more
          references.  At a minimum, the database and accession must be provided.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SBO',
    // The name of the term.
    'term_name' => 'Database Cross Reference',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000554',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The number of items to show on a page.
    'items_per_page' => 10,
    // Limit to the number of items to show in cases of large number of cross references.
    'max_items' => 10000,
  ];

  // The default widget for this field.
  public static $default_widget = 'sbo__database_cross_reference_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sbo__database_cross_reference_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;

  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {

    $field_term = $this->getFieldTermID();
    $dbname_term = chado_get_semweb_term('db', 'name');
    $accession_term = chado_get_semweb_term('dbxref', 'accession');
    $dburl_term = chado_get_semweb_term('db', 'url');

    return [
      $field_term => [
        'operations' => [],
        'label' => 'Cross Reference',
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          $dbname_term => [
            'searchable' => TRUE,
            'label' => 'Cross Reference Database Name',
            'help' => 'The name of the remote database that houses the cross reference.',
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $accession_term => [
            'searchable' => TRUE,
            'label' => 'Cross Reference Database Accession',
            'help' => 'The unique accession (identifier) in the database that houses the cross reference.',
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $dburl_term => [
            'searchable' => FALSE,
            'label' => 'Cross Reference Database URL',
            'help' => 'The URL of the database that houses the cross reference.',
            'sortable' => FALSE,
            'type' => 'xs:anyURI',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $record->tablename;

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    $dbname_term = chado_get_semweb_term('db', 'name');
    $accession_term = chado_get_semweb_term('dbxref', 'accession');
    $dburl_term = chado_get_semweb_term('db', 'url');

    // If we don't have a chado record return before creating a stub for this field!
    if (!$record) {
      return;
    }

    $linker_table = $base_table . '_dbxref';
    $options = ['return_array' => 1];

    // Build the SQL to find records associated with this publication.
    $max_items = array_key_exists('max_items', $this->instance['settings']) ? $this->instance['settings']['max_items'] : 10000;
    $sql = "SELECT REF.accession, DB.name, DB.urlprefix "
         . "FROM {".$linker_table."} LINK "
         . "INNER JOIN {dbxref} REF on LINK.dbxref_id = REF.dbxref_id "
         . "INNER JOIN {db} DB on REF.db_id = DB.db_id "
         . "WHERE LINK.$fkey_lcolumn = :id "
         // Ignore the GFF_source database. This is a weird thing required by
         // GBrowse and is added by the GFF loader. We don't want to show it.
         . "AND NOT DB.name = 'GFF_source' "
         . "ORDER BY REF.accession "  // if we hit the limit, the subset should be consistent
         . "LIMIT :limit";
    $args = [':id' => $entity->{'chado_record_id'},
             ':limit' => $max_items + 1];
    $records = chado_query($sql, $args);

    // Store the query results
    $delta = 0;
    while($record = $records->fetchObject()) {
      // Need this check to detect the case where the limit exactly equals the number of records
      if ($delta < $max_items) {
        $entity->{$field_name}['und'][$delta] = [];
        $entity->{$field_name}['und'][$delta]['value'][$dbname_term] = $record->name;
        $entity->{$field_name}['und'][$delta]['value'][$accession_term] = $record->accession;
        $entity->{$field_name}['und'][$delta]['value'][$dburl_term] = $record->urlprefix;
      }
      $delta++;
    }
    // Display a warning if we have exceeded the maximum number of cross references
    if ( $delta > $max_items ) {
      $entity->{$field_name}['und'][$delta]['value'][$dbname_term] = 'Note';
      $entity->{$field_name}['und'][$delta]['value'][$accession_term] = "Only the first $max_items cross references are shown";
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $dbxref_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $field_table = $this->instance['settings']['chado_table'];

    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];

    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $dbname_term = $field_term_id . ',' . chado_get_semweb_term('db', 'name');
    $accession_term = $field_term_id . ',' . chado_get_semweb_term('dbxref', 'accession');
    $dburl_term = $field_term_id . ',' . chado_get_semweb_term('db', 'url');

    $this->queryJoinOnce($query, $field_table, $alias, "base.$bpkey = $alias.$bpkey");
    $this->queryJoinOnce($query, 'dbxref', $alias . '_DBX', $alias . "_DBX.dbxref_id = $alias.dbxref_id");

    if ($condition['column'] == $dbname_term) {
      $this->queryJoinOnce($query, 'db', $alias . '_DB', $alias . "_DB.db_id = " . $alias . "_DBX.db_id");
      $query->condition($alias . "_DB.name", $condition['value'], $operator);
    }
    if ($condition['column'] == $accession_term) {
      $query->condition($alias . "_DBX.accession", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $dbxref_linker = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $field_table = $this->instance['settings']['chado_table'];

    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];

    $alias = $this->field['field_name'];

    $field_term_id = $this->getFieldTermID();
    $dbname_term = $field_term_id . ',' . chado_get_semweb_term('db', 'name');
    $accession_term = $field_term_id . ',' . chado_get_semweb_term('dbxref', 'accession');
    $dburl_term = $field_term_id . ',' . chado_get_semweb_term('db', 'url');

    $this->queryJoinOnce($query, $field_table, $alias, "base.$bpkey = $alias.$bpkey", "LEFT OUTER");
    $this->queryJoinOnce($query, 'dbxref', $alias . '_DBX', $alias . "_DBX.dbxref_id = $alias.dbxref_id", "LEFT OUTER");

    if ($order['column'] == $dbname_term) {
      $this->queryJoinOnce($query, 'db', $alias . '_DB', $alias . "_DB.db_id = " . $alias . "_DBX.db_id", "LEFT OUTER");
      $query->orderBy($alias . "_DB.name", $order['direction']);
    }
    if ($order['column'] == $accession_term) {
      $query->orderBy($alias . "_DBX.accession", $order['direction']);
    }
  }

  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $langcode, $items, &$errors) {

    // If we don't have an entity then we don't want to validate.  The case
    // where this could happen is when a user is editing the field settings
    // and trying to set a default value. In that case there's no entity and
    // we don't want to validate.  There will always be an entity for creation
    // and update operations of a content type.
    if (!$entity) {
      return;
    }

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->instance['settings']['chado_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];


    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $dbxref_id = $values['chado-' . $field_table . '__dbxref_id'];
      $db_id = $values['db_id'];
      $accession = $values['accession'];

      // Make sure that if a database ID is provided that an accession is also
      // provided.  Here we use the form_set_error function rather than the
      // form_error function because the form_error will add a red_highlight
      // around all of the fields in the fieldset which is confusing as it's not
      // clear to the user what field is required and which isn't. Therefore,
      // we borrow the code from the 'form_error' function and append the field
      // so that the proper field is highlighted on error.
      if (!$db_id) {
        $errors[$field_name][$delta]['und'][] = [
          'message' => t("A database must be provided for the cross reference."),
          'error' => 'sbo__database_cross_reference',
        ];
      }
      if ($db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = [
          'message' => t("A database and the accession must both be provided."),
          'error' => 'sbo__database_cross_reference',
        ];
      }
      if (!$db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = [
          'message' => t("A database and the accession must both be provided."),
          'error' => 'sbo__database_cross_reference',
        ];
      }
    }
  }
}
