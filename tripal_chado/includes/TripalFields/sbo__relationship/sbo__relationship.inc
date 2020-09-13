<?php

class sbo__relationship extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Relationship';

  // The default description for this field.
  public static $description = 'Relationships between records.';

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
    'term_name' => 'Relationship',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000374',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // Inidates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should ahve auto_attach set to FALSE so tha their values can be
    // attached asyncronously.
    'auto_attach' => FALSE,
    // Settings to help the site admin control how relationship types and
    // valid subject/objects can be selected by the user.
    'relationships' => [
      'option1_vocabs' => '',
      'option2_vocab' => '',
      'option2_parent' => '',
      'relationship_types' => '',
    ],
    // The number of items to show on a page.
    'items_per_page' => 10,
  ];

  // The default widget for this field.
  public static $default_widget = 'sbo__relationship_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sbo__relationship_formatter';

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

  // An array of columns to use as the "name" of the subject and object.
  // For example, for the feature table, this will be the name,
  // whereas, for the organism table this will be the genus & species.
  protected $base_name_columns;

  // One of 'type_id', or 'table_name'. Not all base tables have a type_id so
  // this setting allows us to better handle these cases.
  protected $base_type_column;

  // This field depends heavily on the schema of the relationship and base
  // table. The following variables cache the schema to greatly speed up
  // this field.
  // Note: both are ChadoSchema objects.
  protected $schema;

  protected $base_schema;

  // The column which indicated the subject/object_id in the current
  // relationship table. This allows us to support exceptions in the common
  // chado naming conventions.
  protected $subject_id_column;

  protected $object_id_column;

  /**
   * @see TripalField::elements()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          'SIO:000493' => [
            'searchable' => FALSE,
            'name' => 'relationship_clause',
            'label' => 'Relationship Clause',
            'help' => 'An English phrase describing the relationships.',
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'local:relationship_subject' => [
            'searchable' => FALSE,
            'name' => 'relationship_subject',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:complexType',
            'readonly' => FALSE,
            'required' => TRUE,
            'elements' => [
              'rdfs:type' => [
                'name' => 'type',
                'searchable' => TRUE,
                'label' => 'Relationship Subject Type',
                'help' => 'The subject\'s data type in a relationship clause',
                'operations' => ['eq', 'ne', 'contains', 'starts'],
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ],
              'schema:name' => [
                'name' => 'name',
                'searchable' => TRUE,
                'label' => 'Relationship Subject Name',
                'help' => 'The subject\'s name in a relationship clause',
                'operations' => ['eq', 'ne', 'contains', 'starts'],
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ],
              'entity' => [
                'searchable' => FALSE,
                'sortable' => FALSE,
              ],
            ],
          ],
          'local:relationship_type' => [
            'searchable' => TRUE,
            'name' => 'relationship_type',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          'local:relationship_object' => [
            'searchable' => FALSE,
            'name' => 'relationship_object',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:complexType',
            'readonly' => FALSE,
            'required' => TRUE,
            'elements' => [
              'rdfs:type' => [
                'searchable' => TRUE,
                'name' => 'object_type',
                'label' => 'Relationship Object Type',
                'help' => 'The objects\'s data type in a relationship clause',
                'operations' => ['eq', 'ne', 'contains', 'starts'],
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ],
              'schema:name' => [
                'searchable' => TRUE,
                'name' => 'object_name',
                'label' => 'Relationship Object Name',
                'help' => 'The objects\'s name in a relationship clause',
                'operations' => ['eq', 'ne', 'contains', 'starts'],
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ],
              'entity' => [
                'searchable' => FALSE,
                'sortable' => FALSE,
              ],
            ],
          ],
        ],
      ],
    ];
  }

  /**
   * Extends TripalField::__construct().
   */
  public function __construct($field, $instance) {
    parent::__construct($field, $instance);

    $reltable = $instance['settings']['chado_table'];
    $base_table = $instance['settings']['base_table'];

    // First, initialize the schema's.
    $this->schema = new ChadoSchema();
    $this->schema = $this->schema->getTableSchema($reltable);
    $this->base_schema = new ChadoSchema();
    $this->base_schema = $this->base_schema->getTableSchema($base_table);

    // Determine the subject_id/object_id column names.
    foreach ($this->schema['foreign keys'][$base_table]['columns'] AS $lcolum => $rcolum) {
      if (preg_match('/^subject_.*id/', $lcolum)) {
        $this->subject_id_column = $lcolum;
      }
      else {
        if (preg_match('/^object_.*id/', $lcolum)) {
          $this->object_id_column = $lcolum;
        }
      }
    }

    // Determine the name and type columns.
    $this->base_name_columns = [];
    $this->base_type_column = 'table_name';
    switch ($instance['settings']['chado_table']) {

      // TODO: note that Chado 1.4 will add types to, at least,
      // project and analysis, at which point you should use the default instead.
      case 'acquisition_relationship':
      case 'analysis_relationship':
      case 'biomaterial_relationship':
      case 'cell_line_relationship':
      case 'quantification_relationship':
      case 'element_relationship':
      case 'project_relationship':
      case 'pub_relationship':
        $this->base_type_column = 'table_name';
        $this->base_name_columns = ['name'];
      break;

      case 'organism_relationship':
        $this->base_name_columns = ['genus', 'species'];
        $this->base_type_column = 'table_name';
        break;
      case 'phylonode_relationship':
        $this->base_name_columns = ['label'];
        $this->base_type_column = 'table_name';
        break;

      case 'contact':
        $this->base_name_columns = ['name'];
        $this->base_type_column = 'type_id';
        break;
      default:
        // @todo update this to use the schema.
        $this->base_name_columns = ['name'];
        $this->base_type_column = 'type_id';
    }
  }

  /**
   * Retrive the subject from the current relationship.
   *
   * @param $relationship
   *   A single expanded relationship from a variable generated by
   *   chado_generate_var(). At a minimum, if will have a subject, object and
   *   type which should be expanded to the appropriate type of record
   *   depending on the content type this widget is attached to.
   *
   * @return
   *   An array of information for the subject of the $relationship.
   */
  private function getRelationshipSubject($relationship) {
    $name = [];

    foreach ($this->base_name_columns as $column) {
      $name[] = $relationship->{$this->subject_id_column}->{$column};
    }

    // Retrieve the type.
    $type = $this->instance['settings']['base_table'];
    if (($this->base_type_column != 'table_name') AND isset($relationship->{$this->subject_id_column}->{$this->base_type_column})) {
      $type_object = $relationship->{$this->subject_id_column}->{$this->base_type_column};
      if (isset($type_object->name)) {
        $type = $type_object->name;
      }
      elseif (isset($type_object->uniquename)) {
        $type = $type_object->uniquename;
      }
    }

    $record = [
      'rdfs:type' => $type,
      'schema:name' => implode(' ', $name),
    ];

    // If the object has a uniquename then add that in for reference.
    if (property_exists($relationship->{$this->subject_id_column}, 'uniquename')) {
      $record['data:0842'] = $relationship->{$this->subject_id_column}->uniquename;
    }

    // If the object has an organism then add that in for reference.
    if (property_exists($relationship->{$this->subject_id_column}, 'organism_id')
      AND is_object($relationship->{$this->subject_id_column}->organism_id)) {
      $record['OBI:0100026'] = $relationship->{$this->subject_id_column}->organism_id->genus . ' ' . $relationship->{$this->subject_id_column}->organism_id->species;
    }

    // Add in the TripalEntity ids if the object is published.
    if (property_exists($relationship->{$this->subject_id_column}, 'entity_id')) {
      $entity_id = $relationship->{$this->subject_id_column}->entity_id;
      $record['entity'] = 'TripalEntity:' . $entity_id;
    }

    return $record;
  }

  /**
   * Retrieve the object from the current relationship.
   *
   * @param $relationship
   *   A single expanded relationship from a variable generated by
   *   chado_generate_var(). At a minimum, if will have a subject, object and
   *   type which should be expanded to the appropriate type of record
   *   depending on the content type this widget is attached to.
   *
   * @return
   *   An array of information for the object of the $relationship.
   */
  private function getRelationshipObject($relationship) {
    $name = [];

    // Retrieve the name (may be multiple parts).
    foreach ($this->base_name_columns as $column) {
      $name[] = $relationship->{$this->object_id_column}->{$column};
    }

    // Retrieve the Type.
    $type = $this->instance['settings']['base_table'];
    if (($this->base_type_column != 'table_name') AND isset($relationship->{$this->object_id_column}->{$this->base_type_column})) {
      $type_object = $relationship->{$this->object_id_column}->{$this->base_type_column};
      if (isset($type_object->name)) {
        $type = $type_object->name;
      }
      elseif (isset($type_object->uniquename)) {
        $type = $type_object->uniquename;
      }
    }

    $record = [
      'rdfs:type' => $type,
      'schema:name' => implode(' ', $name),
    ];

    // If the object has a unqiuename then add that in for reference.
    if (property_exists($relationship->{$this->object_id_column}, 'uniquename')) {
      $record['data:0842'] = $relationship->{$this->object_id_column}->uniquename;
    }

    // If the object has an organism then add that in for reference.
    if (property_exists($relationship->{$this->object_id_column}, 'organism_id')
      AND is_object($relationship->{$this->object_id_column}->organism_id)) {
      $record['OBI:0100026'] = $relationship->{$this->object_id_column}->organism_id->genus . ' ' . $relationship->{$this->object_id_column}->organism_id->species;
    }

    // Add in the TripalEntity ids if the object is published.
    if (property_exists($relationship->{$this->object_id_column}, 'entity_id')) {
      $entity_id = $relationship->{$this->object_id_column}->entity_id;
      $record['entity'] = 'TripalEntity:' . $entity_id;
    }

    return $record;
  }

  /**
   * Load a specific relationship as indicated by $delta.
   * This function is called by the load method below.
   *
   * Note: The relationship is loaded by adding it the the entitiy values.
   *
   * @param $relationship
   *   A single expanded relationship from a variable generated by
   *   chado_generate_var(). At a minimum, if will have a subject, object and
   *   type which should be expanded to the appropriate type of record
   *   depending on the content type this widget is attached to.
   * @param $entity
   *   The entity the widget is attached to.
   * @param $delta
   *   An integer indicating the specific relationship to load. This is usually
   *   the rank from the relationship table (if there is one).
   */
  private function loadRelationship($relationship, &$entity, $delta) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    $rel_acc = $relationship->type_id->dbxref_id->db_id->name . ':' . $relationship->type_id->dbxref_id->accession;
    $rel_type = $relationship->type_id->name;
    $verb = $this->get_rel_verb($rel_type);

    $pkey = $this->schema['primary key'][0];
    $subject_id_key = $this->subject_id_column;
    $object_id_key = $this->object_id_column;
    // @todo grab these separately like it was before.
    $subject_pkey = $object_pkey = $this->base_schema['primary key'][0];

    $entity->{$field_name}['und'][$delta]['value'] = [
      'local:relationship_subject' => $this->getRelationshipSubject($relationship),
      'local:relationship_type' => $relationship->type_id->name,
      'local:relationship_object' => $this->getRelationshipObject($relationship),
    ];

    // Add the clause to the values array.  The clause is a written version
    // of the relationships.
    $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
    $subject_type = $entity->{$field_name}['und'][$delta]['value']['local:relationship_subject']['rdfs:type'];
    $subject_name = $entity->{$field_name}['und'][$delta]['value']['local:relationship_subject']['schema:name'];
    $object_type = $entity->{$field_name}['und'][$delta]['value']['local:relationship_object']['rdfs:type'];
    $object_name = $entity->{$field_name}['und'][$delta]['value']['local:relationship_object']['schema:name'];


    // Remember the current entity could be either the subject or object!
    // Example: The genetic_marker, MARKER1 , derives from the sequence_variant, VARIANT1.
    // The above relationship will be shown both on marker and variant pages
    // and as such both subject and object names need to be shown.
    $clause = 'The ' . $subject_type . ', ' .
      $subject_name . ', ' . $verb . ' ' . $rel_type_clean . ' ' .
      $object_type . ', ' . $object_name . '.';
    $entity->{$field_name}['und'][$delta]['value']['SIO:000493'] = $clause;

    // Adding a label allows us to provide a single text value for the
    // entire field. It is this text value that can be used in tab/csv
    // downloaders.
    $entity->{$field_name}['und'][$delta]['value']['rdfs:label'] = $clause;

    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__' . $pkey] = $relationship->$pkey;
    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__' . $subject_id_key] = $relationship->$subject_id_key->$subject_pkey;
    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__type_id'] = $relationship->type_id->cvterm_id;
    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__' . $object_id_key] = $relationship->$object_id_key->$object_pkey;

    // For the widget to work properly we will preform values.
    $entity->{$field_name}['und'][$delta]['type_name'] = $relationship->type_id->name;
    $entity->{$field_name}['und'][$delta]['subject_name'] = $subject_name . ' [id: ' . $relationship->$subject_id_key->$subject_pkey . ']';
    $entity->{$field_name}['und'][$delta]['object_name'] = $object_name . ' [id: ' . $relationship->$object_id_key->$object_pkey . ']';
    if (array_key_exists('value', $this->schema['fields'])) {
      $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__value'] = $relationship->value;
    }
    if (array_key_exists('rank', $this->schema['fields'])) {
      $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__rank'] = $relationship->rank;
    }
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $settings = $this->field['settings'];

    $record = $entity->chado_record;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $rel_table = $field_table;

    // Get the PKey for this table
    $pkey = $this->schema['primary key'][0];
    // Not all tables have the columns named 'subject_id' and 'object_id'.
    // some have variations on that name and we need to determine what they are.
    $subject_id_key = $this->subject_id_column;
    $object_id_key = $this->object_id_column;

    // If we don't have a chado record return before creating a stub for this field!
    if (!$record) {
      return;
    }

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $subject_id_key => '',
      'chado-' . $field_table . '__' . $object_id_key => '',
      'chado-' . $field_table . '__type_id' => '',
      // These elements don't need to follow the naming scheme above
      // because we don't need the chado_field_storage to try and
      // save these values.
      'object_name' => '',
      'subject_name' => '',
      'type_name' => '',
    ];

    // If the table has rank and value fields then add those to the default
    // value array.
    if (array_key_exists('value', $this->schema['fields'])) {
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__value'] = '';
    }
    if (array_key_exists('rank', $this->schema['fields'])) {
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__rank'] = '';
    }

    // Expand the object to include the relationships.
    $options = [
      'return_array' => 1,
      // we don't want to fully recurse we only need information about the
      // relationship type and the object and subject
      'include_fk' => [
        'type_id' => 1,
        $object_id_key => [
          'type_id' => 1,
          'organism_id' => 1,
        ],
        $subject_id_key => [
          'type_id' => 1,
          'organism_id' => 1,
        ],
      ],
    ];
    if (array_key_exists('rank', $this->schema['fields'])) {
      $options['order_by'] = ['rank' => 'ASC'];
    }
    $record = chado_expand_var($record, 'table', $rel_table, $options);
    if (!$record->$rel_table) {
      return;
    }

    // Load the subject relationships
    $i = 0;
    if (isset($record->$rel_table->$subject_id_key)) {
      $srelationships = $record->$rel_table->$subject_id_key;
      foreach ($srelationships as $relationship) {
        $this->loadRelationship($relationship, $entity, $i);
        $i++;
      }
    }

    // Load the object relationships
    if (isset($record->$rel_table->$object_id_key)) {
      $orelationships = $record->$rel_table->$object_id_key;
      foreach ($orelationships as $relationship) {
        $this->loadRelationship($relationship, $entity, $i);
        $i++;
      }
    }
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {

    $alias = $this->field['field_name'];
    $chado_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];
    $operator = $condition['operator'];

    // Bulid the list of expected elements that will be provided.
    $field_term_id = $this->getFieldTermID();
    $rel_subject = $field_term_id . ',local:relationship_subject';
    $rel_subject_type = $rel_subject . ',' . 'rdfs:type';
    $rel_subject_name = $rel_subject . ',' . 'schema:name';
    $rel_subject_identifier = $rel_subject . ',' . 'data:0842';
    $rel_type = $field_term_id . ',local:relationship_type';
    $rel_object = $field_term_id . ',local:relationship_object';
    $rel_object_type = $rel_object . ',' . 'rdfs:type';
    $rel_object_name = $rel_object . ',' . 'schema:name';
    $rel_object_identifier = $rel_object . ',' . 'data:0842';

    // Filter by the name of the subject or object.
    if ($condition['column'] == $rel_subject_name) {
      $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.object_id");
      $this->queryJoinOnce($query, $base_table, 'base2', "base2.$bpkey = $alias.subject_id");
      $query->condition("base2.name", $condition['value'], $operator);
    }
    if ($condition['column'] == $rel_object_name) {
      $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.subject_id");
      $this->queryJoinOnce($query, $base_table, 'base2', "base2.$bpkey = $alias.object_id");
      $query->condition("base2.name", $condition['value'], $operator);
    }

    // Filter by unique name of the subject or object.
    // If this table has a uniquename!
    if (isset($this->schema['fields']['uniquename'])) {
      if ($condition['column'] == $rel_subject_identifier) {
        $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.object_id");
        $this->queryJoinOnce($query, $base_table, 'base2', "base2.$bpkey = $alias.subject_id");
        $query->condition("base2.uniquename", $condition['value'], $operator);
      }
      if ($condition['column'] == $rel_object_identifier) {
        $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.subject_id");
        $this->queryJoinOnce($query, $base_table, 'base2', "base2.$bpkey = $alias.object_id");
        $query->condition("base2.uniquename", $condition['value'], $operator);
      }
    }

    // Filter by the type of the subject or object
    if ($condition['column'] == $rel_subject_type) {
      $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.object_id");
      $this->queryJoinOnce($query, $base_table, 'base2', "base2.$bpkey = $alias.subject_id");
      $this->queryJoinOnce($query, 'cvterm', 'SubjectCVT', "SubjectCVT.cvterm_id = base2.type_id");
      $this->queryJoinOnce($query, "SubjectCVT.name", $condition['value'], $operator);
    }
    if ($condition['column'] == $rel_object_type) {
      $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.subject_id");
      $this->queryJoinOnce($query, $base_table, 'base2', "base2.$bpkey = $alias.object_id");
      $this->queryJoinOnce($query, 'cvterm', 'ObjectCVT', "ObjectCVT.cvterm_id = base2.type_id");
      $query->condition("ObjectCVT.name", $condition['value'], $operator);
    }

    // Filter by relationship type
    if ($condition['column'] == 'relationship.relationship_type') {
      // This filter commented out because it's way to slow...
      //       $this->queryJoinOnce($query, $chado_table, $alias, "base.$bpkey = $alias.subject_id OR base.$bpkey = $alias.object_id");
      //       $this->queryJoinOnce($query, 'cvterm', 'RelTypeCVT', "RelTypeCVT.cvterm_id = $alias.type_id");
      //       $query->condition("RelTypeCVT.name", $condition['value'], $operator);
    }
  }

  /**
   * A helper function to define English verbs for relationship types.
   *
   * @param $rel_type
   *   The vocabulary term name for the relationship.
   *
   * @return
   *   The verb to use when creating a sentence of the relationship.
   */
  private function get_rel_verb($rel_type) {
    $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
    $verb = '';
    switch ($rel_type_clean) {
      case 'integral part of':
      case 'instance of':
        $verb = 'is an';
        break;
      case 'proper part of':
      case 'transformation of':
      case 'genome of':
      case 'part of':
        $verb = 'is a';
      case 'position of':
      case 'sequence of':
      case 'variant of':
        $verb = 'is a';
        break;
      case 'derives from':
      case 'connects on':
      case 'contains':
      case 'finishes':
      case 'guides':
      case 'has origin':
      case 'has part':
      case 'has quality':
      case 'is a':
      case 'is a maternal parent of':
      case 'is a paternal parent of':
      case 'is consecutive sequence of':
      case 'maximally overlaps':
      case 'overlaps':
      case 'starts':
        break;
      default:
        $verb = 'is';
    }

    return $verb;
  }

  /**
   *
   * @see TripalField::settingsForm()
   */
  public function instanceSettingsForm() {
    $element = parent::instanceSettingsForm();

    $element['items_per_page'] = [
      '#type' => 'textfield',
      '#title' => 'Items per Page',
      '#description' => t('The number of items that should appear on each page.  A pager is provided if more than this number of items exist.'),
      '#default_value' => $this->instance['settings']['items_per_page'],
    ];

    //$element = parent::instanceSettingsForm();
    $element['relationships'] = [
      '#type' => 'fieldset',
      '#title' => 'Allowed Relationship Types',
      '#description' => t('There are three ways that relationship types
          can be limited for users who have permission to add new relationships.
          Please select the most appropriate for you use case.  By default
          all vocabularies are provided to the user which allows use of any
          term for the relationship type.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    ];
    //     $element['instructions'] = array(
    //       '#type' => 'item',
    //       '#markup' => 'You may provide a list of terms that will be available in a select box
    //         as the relationship types. This select box will replace the vocabulary select box if the
    //         following value is set.'
    //     );
    $vocs = chado_get_cv_select_options();
    $element['relationships']['option1'] = [
      '#type' => 'item',
      '#title' => 'Option #1',
      '#description' => t('Use this option to limit the vocabularies that a user .
        could use to specify relationship types. With this option any term in .
        the vocabulary can be used for the relationship type. You may select
        more than one vocabulary.'),

    ];
    $element['relationships']['option1_vocabs'] = [
      '#type' => 'select',
      '#multiple' => TRUE,
      '#options' => $vocs,
      '#size' => 6,
      '#default_value' => $this->instance['settings']['relationships']['option1_vocabs'],
      // TODO add ajax here so that the relationship autocomplete below works
    ];
    $element['relationships']['option2'] = [
      '#type' => 'item',
      '#title' => '<b>Option #2</b>',
      '#description' => 'Some vocabularies are heirarchichal (an ontology). Within this
         heirarchy groups of related terms typically fall under a common parent. If you
         wish to limit the list of terms that a user can use for the relationship type,
         you can provide the parent term here.  Then, only that term\'s children will
         be avilable for use as a relationship type.',
    ];
    $element['relationships']['option2_vocab'] = [
      '#type' => 'select',
      '#description' => 'Specify Default Vocabulary',
      '#multiple' => FALSE,
      '#options' => $vocs,
      '#default_value' => $this->instance['settings']['relationships']['option2_vocab'],
      '#ajax' => [
        'callback' => "sbo__relationship_instance_settings_form_ajax_callback",
        'wrapper' => 'relationships-option2-parent',
        'effect' => 'fade',
        'method' => 'replace',
      ],
    ];
    $element['relationships']['option2_parent'] = [
      '#type' => 'textfield',
      '#description' => 'Specify a Heirarchical Parent Term',
      '#default_value' => $this->instance['settings']['relationships']['option2_parent'],
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
      '#prefix' => '<div id=relationships-option2-parent>',
      '#suffix' => '</div>',
    ];
    $element['relationships']['option3'] = [
      '#type' => 'item',
      '#title' => 'Option #3',
      '#description' => 'Provide terms separated by a new line. The term provided should be
        unique and distinguishable by the name. You can use a bar | to separate a vocabulary
        and a term to allow more specific assignment.',
    ];
    $element['relationships']['relationship_types'] = [
      '#type' => 'textarea',
      '#default_value' => $this->instance['settings']['relationships']['relationship_types'],
    ];

    return $element;
  }

  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function instanceSettingsFormValidate($form, &$form_state) {
    // Get relationships settings
    $settings = $form_state['values']['instance']['settings']['relationships'];
    $form_state['values']['instance']['settings']['relationships']['relationship_types'] = trim($settings['relationship_types']);

    // Make sure only one option is selected
    $option1test = $settings['option1_vocabs'];
    $option1 = isset($settings['option1_vocabs']) && array_pop($option1test);
    $option2 = (isset($settings['option2_vocab']) && $settings['option2_vocab']) || $settings['option2_parent'];
    $option3 = isset($settings['relationship_types']) && trim($settings['relationship_types']);
    if ($option1 && ($option2 || $option3) == 1 ||
      $option2 && ($option1 || $option3) == 1 ||
      $option3 && ($option1 || $option2) == 1) {
      form_set_error("instance][settings][relationships", t("Only one option is allowed to limit the relationship types."));
      return;
    }

    // For option3, make sure the supplied types are valid cvterms
    if ($option3) {
      $rel_types = explode(PHP_EOL, $settings['relationship_types']);
      foreach ($rel_types AS $type) {
        $type = trim($type);
        // Ignore empty lines
        if ($type == '') {
          continue;
        }
        // Find the matching cvterm
        $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name";
        $results = chado_query($sql, [':name' => $type]);
        $terms = [];
        while ($obj = $results->fetchObject()) {
          $terms[] = $obj;
        }
        // Don't save the form  if a term can not be found or it matches more than one cvterm
        $cv = '';
        if (count($terms) == 0) {
          // If a term can not be found, maybe the type contains '|', parse it as 'vocabulary|cvterm'
          if (strpos($type, '|')) {
            $tmp = explode('|', $type, 2);
            $type = trim($tmp[1]);
            $cv = chado_get_cv(['name' => trim($tmp[0])]);
            if ($cv) {
              $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name AND cv_id = :cv_id";
              $results = chado_query($sql, [
                ':name' => $type,
                ':cv_id' => $cv->cv_id,
              ]);
              while ($obj = $results->fetchObject()) {
                $terms[] = $obj;
              }
            }
            else {
              $cv = $tmp[0];
            }
          }
          if (count($terms) != 1) {
            $message = "The term '@type' can not be found.";
            $token = ['@type' => $type];
            if ($cv) {
              $message = "The term '@type' can not be found within the vocabulary '@vocab'.";
              $token['@vocab'] = $cv;
            }
            form_set_error("instance][settings][relationships][relationship_types",
              t($message, $token));
          }
        }
        else {
          if (count($terms) > 1) {
            // If a type matches more than one term, parse it as 'vocabulary|cvterm' and try again
            if (strpos($type, '|')) {
              $tmp = explode('|', $type, 2);
              $type = trim($tmp[1]);
              $cv = chado_get_cv(['name' => trim($tmp[0])]);
              if ($cv) {
                $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name AND cv_id = :cv_id";
                $results = chado_query($sql, [
                  ':name' => $type,
                  ':cv_id' => $cv->cv_id,
                ]);
                while ($obj = $results->fetchObject()) {
                  $terms[] = $obj;
                }
              }
            }
            if (count($terms) != 1) {
              form_set_error("instance][settings][relationships][relationship_types",
                t("The term '@type' matches more than one term. Please specify its vocabulary in the format of 'vocabulary|@type'.", ['@type' => $type]));
            }
          }
        }
      }
    }

    // For option2: Make sure the parent term is a valid cvterm
    if ($option2) {
      $cv_id = $settings['option2_vocab'];
      $supertype = $settings['option2_parent'];
      $term = chado_get_cvterm([
        'name' => trim($supertype),
        'cv_id' => $cv_id,
      ]);
      // Tripal cv autocomplete also allow cvterm synonyms, if the parent term doesn't match
      // a cvterm, try cvtermsynonym
      if (!$term) {
        $synonym = chado_get_cvterm(
          [
            'synonym' => [
              'name' => trim($supertype),
            ],
          ]
        );
        if ($synonym && $synonym->cv_id->cv_id == $cv_id) {
          $term = $synonym;
        }
      }
      if (!isset($term->cvterm_id)) {
        form_set_error("instance][settings][relationships][option2_parent",
          t("The term '@type' is not a valid term for the vocabulary selected.", ['@type' => $supertype]));
      }
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
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Grab the chado record_id for this entity.
    $chado_record_id = NULL;
    if ($entity AND isset($entity->chado_record_id)) {
      $chado_record_id = $entity->chado_record_id;
    }

    // Validate each releationship.
    foreach ($items as $delta => $item) {
      $item_errors = $this->validateItem($item, $chado_record_id);
      if (!empty($item_errors)) {
        $errors[$field_name][$delta][$langcode] = $item_errors;
      }
    }
  }

  /**
   * Validate a Single relationship.
   *
   * @param $item
   *   A single item from the $items array passed to TripalField::validate().
   *
   * @return
   *   An array of errors where each has a:
   *     - error: this is an error code which is the name of the field.
   *     - message: A message to show the user describing the problem.
   */
  public function validateItem($item, $chado_record_id = NULL) {
    $errors = [];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // 'nd_reagent_relationship' and 'project_relationship' have different column names from
    // subject_id/object_id. Do a pattern matching to get the column names.
    $subject_id_key = $this->subject_id_column;
    $object_id_key = $this->object_id_column;

    $subject_id = $item['chado-' . $field_table . '__' . $subject_id_key];
    $object_id = $item['chado-' . $field_table . '__' . $object_id_key];
    $type_id = $item['chado-' . $field_table . '__type_id'];
    $type_id = isset($item['type_id']) ? $item['chado-' . $field_table . '__type_id'] : $type_id;
    $type_name = isset($item['type_name']) ? $item['type_name'] : '';
    $voc_id = isset($item['vocabulary']) ? $item['vocabulary'] : '';
    $subject_name = isset($item['subject_name']) ? $item['subject_name'] : '';
    $object_name = isset($item['object_name']) ? $item['object_name'] : '';

    // If the row is empty then just continue, there's nothing to validate.
    if (!$type_id and !$type_name and !$subject_name and !$object_name) {
      return;
    }

    // Check: Make sure we have values for all of the fields.
    if (!$type_name && !$type_id) {
      $errors[] = [
        'error' => 'sbo__relationship',
        'message' => t("Please provide the type of relationship."),
        'element' => 'type',
      ];
    }
    if (!$subject_name) {
      $errors[] = [
        'error' => 'sbo__relationship',
        'message' => t("Please provide the subject of the relationship."),
        'element' => 'subject',
      ];
    }
    if (!$object_name) {
      $errors[] = [
        'error' => 'sbo__relationship',
        'message' => t("Please provide the object of the relationship."),
        'element' => 'object',
      ];
    }

    // Check: Cvterm exists.
    if (!$type_id AND !$type_name) {
      $errors[] = [
        'error' => 'sbo__relationship',
        'message' => t("We were unable to find the type you specified. Please check spelling and that the term already exists."),
        'element' => 'type',
      ];
    }
    elseif ($type_name AND $voc_id) {
      $val = [
        'cv_id' => $voc_id,
        'name' => $type_name,
      ];
      $cvterm = chado_generate_var('cvterm', $val);

      if (!isset($cvterm->cvterm_id)) {

        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("We were unable to find the type you specified. Please check spelling and that the term already exists."),
          'element' => 'type',
        ];

      }
    }


    // Before submitting this form we need to make sure that our subject_id and
    // object_ids are real records.  There are two ways to get the record, either
    // just with the text value or with an [id: \d+] string embedded.  If the
    // later we will pull it out.
    $subject_id = '';
    $fkey_rcolumn = $this->schema['foreign keys'][$base_table]['columns'][$subject_id_key];
    $matches = [];
    if (preg_match('/\[id: (\d+)\]/', $subject_name, $matches)) {
      $subject_id = $matches[1];
      $values = [$fkey_rcolumn => $subject_id];
      $subject = chado_select_record($base_table, [$fkey_rcolumn], $values);
      if (count($subject) == 0) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The subject record cannot be found using the specified id (e.g. [id: xx])."),
          'element' => 'subject',
        ];
      }
    }
    else {
      // Otherwise we need to look it up using the name field determined in the
      // constructor for the current field. There may be more then one name field
      // (e.g. organism: genus + species) so we want to check both.
      $sql = 'SELECT ' . $fkey_rcolumn . ' FROM {' . $base_table . '} WHERE ' . implode('||', $this->base_name_columns) . '=:keyword';
      $subject = chado_query($sql, [':keyword' => $subject_name])->fetchAll();
      if (count($subject) == 0 AND $chado_record_id) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The subject record cannot be found. Please check spelling."),
          'element' => 'subject',
        ];
      }
      elseif (count($subject) > 1) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The subject is not unique and therefore the relationship cannot be made."),
          'element' => 'subject',
        ];
      }
    }

    // Now check for a matching object.
    $object_id = '';
    $fkey_rcolumn = $this->schema['foreign keys'][$base_table]['columns'][$object_id_key];
    $matches = [];
    if (preg_match('/\[id: (\d+)\]/', $object_name, $matches)) {
      $object_id = $matches[1];
      $values = [$fkey_rcolumn => $object_id];
      $object = chado_select_record($base_table, [$fkey_rcolumn], $values);
      if (count($subject) == 0 AND $chado_record_id) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The object record cannot be found using the specified id (e.g. [id: xx])."),
          'element' => 'object',
        ];
      }
    }
    else {
      // Otherwise we need to look it up using the name field determined in the
      // constructor for the current field. There may be more then one name field
      // (e.g. organism: genus + species) so we want to check both.
      $sql = 'SELECT ' . $fkey_rcolumn . ' FROM {' . $base_table . '} WHERE ' . implode('||', $this->base_name_columns) . '=:keyword';
      $object = chado_query($sql, [':keyword' => $object_name]);
      if (count($object) == 0) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The object record cannot be found. Please check spelling."),
          'element' => 'object',
        ];
      }
      elseif (count($object) > 1) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The object is not unique and therefore the relationship cannot be made."),
          'element' => 'object',
        ];
      }
    }

    // Make sure that either our object or our subject refers to the base record.
    if ($object_id AND $subject_id) {
      if ($object_id != $chado_record_id and $subject_id != $chado_record_id) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("Either the subject or the object in the relationship must refer to this record."),
          'element' => 'row',
        ];
      }
    }

    // Make sure that the object and subject are not both the same thing.
    if ($object_id AND $subject_id) {
      if ($object_id == $subject_id) {
        $errors[] = [
          'error' => 'sbo__relationship',
          'message' => t("The subject and the object in the relationship cannot both refer to the same record."),
          'element' => 'row',
        ];
      }
    }

    return $errors;
  }


  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {

  }

  /**
   * Retrieve the schema's
   */
  public function getRelTableSchema() {
    return $this->schema;
  }

  public function getBaseTableSchema() {
    return $this->base_schema;
  }

  /**
   * Retrieve the subject/object key columns.
   */
  public function getSubjectIdColumn() {
    return $this->subject_id_column;
  }

  public function getObjectIdColumn() {
    return $this->object_id_column;
  }

  /**
   * Retrieve the base name columns.
   */
  public function getBaseNameColumns() {
    return $this->base_name_columns;
  }

  /**
   * Retrieve the base type column.
   */
  public function getBaseTypeColumn() {
    return $this->base_type_column;
  }
}
