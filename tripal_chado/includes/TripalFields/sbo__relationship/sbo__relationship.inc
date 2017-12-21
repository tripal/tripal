<?php

class sbo__relationship extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Relationship';

  // The default description for this field.
  public static $description = 'Relationships between records.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
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
    'relationships' => array(
      'option1_vocabs' => '',
      'option2_vocab' => '',
      'option2_parent' => '',
      'relationship_types' => '',
    ),
    // The number of items to show on a page.
    'items_per_page' => 10,
  );

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


  /**
   * @see TripalField::elements()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    return array(
      $field_term => array(
        'operations' => array('eq', 'contains', 'starts'),
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => array(
          'SIO:000493' => array(
            'searchable' => FALSE,
            'name' => 'relationship_clause',
            'label' => 'Relationship Clause',
            'help' => 'An English phrase describing the relationships.',
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ),
          'local:relationship_subject' => array(
            'searchable' => FALSE,
            'name' => 'relationship_subject',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => FALSE,
            'type' => 'xs:complexType',
            'readonly' => FALSE,
            'required' => TRUE,
            'elements' => array(
              'rdfs:type' => array(
                'name' => 'type',
                'searchable' => TRUE,
                'label' => 'Relationship Subject Type',
                'help' => 'The subject\'s data type in a relationship clause',
                'operations' => array('eq', 'ne', 'contains', 'starts'),
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ),
              'schema:name' => array(
                'name' => 'name',
                'searchable' => TRUE,
                'label' => 'Relationship Subject Name',
                'help' => 'The subject\'s name in a relationship clause',
                'operations' => array('eq', 'ne', 'contains', 'starts'),
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ),
              'entity' => array(
                'searchable' => FALSE,
                'sortable' => FALSE,
              )
            ),
          ),
          'local:relationship_type' => array(
            'searchable' => TRUE,
            'name' => 'relationship_type',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ),
          'local:relationship_object' => array(
            'searchable' => FALSE,
            'name' => 'relationship_object',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => FALSE,
            'type' => 'xs:complexType',
            'readonly' => FALSE,
            'required' => TRUE,
            'elements' => array(
              'rdfs:type' => array(
                'searchable' => TRUE,
                'name' => 'object_type',
                'label' => 'Relationship Object Type',
                'help' => 'The objects\'s data type in a relationship clause',
                'operations' => array('eq', 'ne', 'contains', 'starts'),
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ),
              'schema:name' => array(
                'searchable' => TRUE,
                'name' => 'object_name',
                'label' => 'Relationship Object Name',
                'help' => 'The objects\'s name in a relationship clause',
                'operations' => array('eq', 'ne', 'contains', 'starts'),
                'sortable' => TRUE,
                'type' => 'xs:string',
                'readonly' => FALSE,
                'required' => TRUE,
              ),
              'entity' => array(
                'searchable' => FALSE,
                'sortable' => FALSE,
              )
            ),
          ),
        ),
      )
    );
  }

  private function loadRelationship($relationship, &$entity, $delta) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];

    $rel_acc = $relationship->type_id->dbxref_id->db_id->name . ':' . $relationship->type_id->dbxref_id->accession;
    $rel_type = $relationship->type_id->name;
    $verb = $this->get_rel_verb($rel_type);

    // Get the foreign keys for the subject and object tables
    $subject_fkey_table = '';
    $object_fkey_table = '';

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Not all tables have the columns named 'subject_id' and 'object_id'.
    // some have variations on that name and we need to determine what they are.
    $fkeys = $schema['foreign keys'];
    $subject_id_key = 'subject_id';
    $object_id_key = 'object_id';
    foreach ($fkeys as $fktable => $details) {
      foreach ($details['columns'] as $fkey_lcolumn => $fkey_rcolumn) {
        if (preg_match('/^subject_.*id/', $fkey_lcolumn)) {
          $subject_fkey_table = $fktable;
          $subject_id_key = $fkey_lcolumn;
        }
        if (preg_match('/^object_.*id/', $fkey_lcolumn)) {
          $object_fkey_table = $fktable;
          $object_id_key = $fkey_lcolumn;
        }
      }
    }

    // Get the schemas for the subject and object table.  These should
    // be the same as the base table but just to be safe we'll get them
    // separately.
    $subject_schema = chado_get_schema($subject_fkey_table);
    $subject_pkey = $subject_schema['primary key'][0];
    $object_schema = chado_get_schema($object_fkey_table);
    $object_pkey = $object_schema['primary key'][0];

    // Not all realtionshp tables have a name field (e.g. organism_relationship)
    // threfore in some cases we need to dig a bit deeper to get the entity
    // name and the entity type name.
    $subject_name = '';
    $subject_type = '';
    $object_name = '';
    $object_type = '';

    // The linked to table of a relationship linker table may not always
    // have a type_id or name field.  So we have to be a bit more
    // specific about how we set some variables.
    switch ($relationship->tablename) {
      case 'acquisition_relationship':
        $subject_type = 'acquisition';
        $object_type = 'acquisition';
        break;
      case 'analysis_relationship':
        $subject_type = 'analysis';
        $object_type = 'analysis';
        break;
      case 'biomaterial_relationship':
        $subject_type = 'biomaterial';
        $object_type = 'biomaterial';
        break;
      case 'cell_line_relationship':
        $subject_type = 'cell_line';
        $object_type = 'cell_line';
        break;
      case 'element_relationship':
        $subject_name = $relationship->$subject_id_key->feature_id->name;
        $object_name = $relationship->$object_id_key->feature_id->name;
        break;
      case 'organism_relationship':
        $subject_name = $relationship->$subject_id_key->genus . ' ' . $relationship->$subject_id_key->species;
        $object_name = $relationship->$object_id_key->genus . ' ' . $relationship->$object_id_key->species;
        $subject_type = 'organism';
        $object_type = 'organism';
        break;
      case 'project_relationship':
        $subject_type = 'project';
        $object_type = 'project';
        break;
      case 'phylonode_relationship':
        $subject_name = $relationship->$subject_id_key->label;
        $object_name = $relationship->$object_id_key->label;
        break;
      case 'pub_relationship':
        $subject_name = $relationship->$subject_id_key->uniquename;
        $object_name = $relationship->$object_id_key->uniquename;
        break;
      case 'quantification_relationship':
        $subject_type = 'quantification';
        $object_type = 'quantification';
        break;
      default:
        $subject_name = isset($relationship->$subject_id_key->name) ? $relationship->$subject_id_key->name : '';
        $subject_type = isset($relationship->$subject_id_key->type_id) ? $relationship->$subject_id_key->type_id->name : '';
        $object_name = isset($relationship->$object_id_key->name) ? $relationship->$object_id_key->name : '';
        $object_type = isset($relationship->$object_id_key->type_id) ? $relationship->$object_id_key->type_id->name : '';
    }

    $entity->{$field_name}['und'][$delta]['value'] = array(
      'local:relationship_subject' => array(
        'rdfs:type' => $subject_type,
        'schema:name' => $subject_name,
      ),
      'local:relationship_type' => $relationship->type_id->name,
      'local:relationship_object' => array(
        'rdfs:type' => $object_type,
        'schema:name' => $object_name,
        'entity' => 'TripalEntity:' . $entity->id,
      )
    );

    // If the subject or object have a unqiuename then add that in for refernce.
    if (property_exists($relationship->$subject_id_key, 'uniquename')) {
      $entity->{$field_name}['und'][$delta]['value']['local:relationship_subject']['data:0842'] = $relationship->$subject_id_key->uniquename;;
    }
    if (property_exists($relationship->$object_id_key, 'uniquename')) {
      $entity->{$field_name}['und'][$delta]['value']['local:relationship_object']['data:0842'] = $relationship->$object_id_key->uniquename;
    }

    // Add in the TripalEntity ids if these base records in the relationship
    // are published.
    if (property_exists($relationship->$subject_id_key, 'entity_id')) {
      $entity_id = $relationship->$subject_id_key->entity_id;
      $entity->{$field_name}['und'][$delta]['value']['local:relationship_subject']['entity'] = 'TripalEntity:' . $entity_id;
    }
    if (property_exists($relationship->$object_id_key, 'entity_id')) {
      $entity_id = $relationship->$object_id_key->entity_id;
      $entity->{$field_name}['und'][$delta]['value']['local:relationship_object']['entity'] = 'TripalEntity:' . $entity_id;
    }

    // Add the clause to the values array.  The clause is a written version
    // of the relationships.
    $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
    // Remember the current entity could be either the subject or object!
    // Example: The genetic_marker, MARKER1 , derives from the sequence_variant, VARIANT1.
    // The above relationship will be shown both on marker and variant pages
    // and as such both subject and object names need to be shown.
    $entity->{$field_name}['und'][$delta]['value']['SIO:000493'] = 'The ' . $subject_type . ', ' .
      $subject_name . ', ' . $verb . ' '  . $rel_type_clean . ' the '  .
      $object_type . ', ' . $object_name . '.';

    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__' . $pkey] = $relationship->$pkey;
    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__' . $subject_id_key] = $relationship->$subject_id_key->$subject_pkey;
    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__type_id'] = $relationship->type_id->cvterm_id;
    $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__' . $object_id_key] = $relationship->$object_id_key->$object_pkey;

    // For the widget to work properly we will preform values.
    $entity->{$field_name}['und'][$delta]['type_name'] = $relationship->type_id->name;
    $entity->{$field_name}['und'][$delta]['subject_name'] = $subject_name . ' [id: ' . $relationship->$subject_id_key->$subject_pkey . ']';
    $entity->{$field_name}['und'][$delta]['object_name'] = $object_name  . ' [id: ' . $relationship->$object_id_key->$object_pkey . ']';
    if (array_key_exists('value', $schema['fields'])) {
      $entity->{$field_name}['und'][$delta]['chado-' . $field_table . '__value'] = $relationship->value;
    }
    if (array_key_exists('rank', $schema['fields'])) {
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
    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the PKey for this table
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Not all tables have the columns named 'subject_id' and 'object_id'.
    // some have variations on that name and we need to determine what they are.
    $fkeys = $schema['foreign keys'];
    $subject_id_key = 'subject_id';
    $object_id_key = 'object_id';
    foreach ($fkeys as $fktable => $details) {
      foreach ($details['columns'] as $fkey_lcolumn => $fkey_rcolumn) {
        if (preg_match('/^subject_.*id/', $fkey_lcolumn)) {
          $subject_id_key = $fkey_lcolumn;
        }
        if (preg_match('/^object_.*id/', $fkey_lcolumn)) {
          $object_id_key = $fkey_lcolumn;
        }
      }
    }

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $subject_id_key => '',
      'chado-' . $field_table . '__' . $object_id_key => '',
      'chado-' . $field_table . '__type_id' => '',
      // These elements don't need to follow the naming scheme above
      // becasue we don't need the chado_field_storage to try and
      // save these values.
      'object_name' => '',
      'subject_name' => '',
      'type_name' => '',
    );

    // If the table has rank and value fields then add those to the default
    // value array.
    if (array_key_exists('value', $schema['fields'])) {
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__value'] = '';
    }
    if (array_key_exists('rank', $schema['fields'])) {
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__rank'] = '';
    }

    // If we have no record then just return.
    if (!$record) {
      return;
    }

    // Expand the object to include the relationships.
    $options = array(
      'return_array' => 1,
      // we don't want to fully recurse we only need information about the
      // relationship type and the object and subject
      'include_fk' => array(
        'type_id' => 1,
        $object_id_key => array(
          'type_id' => 1,
        ),
        $subject_id_key  => array(
          'type_id' => 1,
        ),
      ),
    );
    $rel_table = $base_table . '_relationship';
    $schema = chado_get_schema($rel_table);
    if (array_key_exists('rank', $schema['fields'])) {
      $options['order_by'] = array('rank' => 'ASC');
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
    $rel_subject_type =  $rel_subject . ',' . 'rdfs:type';
    $rel_subject_name =  $rel_subject . ',' . 'schema:name';
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

    $element['items_per_page'] = array(
      '#type' => 'textfield',
      '#title' => 'Items per Page',
      '#description' => t('The number of items that should appear on each page.  A pager is provided if more than this number of items exist.'),
      '#default_value' => $this->instance['settings']['items_per_page'],
    );

    //$element = parent::instanceSettingsForm();
    $element['relationships'] = array(
      '#type' => 'fieldset',
      '#title' => 'Allowed Relationship Types',
      '#description' => t('There are three ways that relationship types
          can be limited for users who have permission to add new relationships.
          Please select the most appropriate for you use case.  By default
          all vocabularies are provided to the user which allows use of any
          term for the relationship type.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );
    //     $element['instructions'] = array(
    //       '#type' => 'item',
    //       '#markup' => 'You may provide a list of terms that will be available in a select box
    //         as the relationship types. This select box will replace the vocabulary select box if the
    //         following value is set.'
    //     );
    $vocs = tripal_get_cv_select_options();
    $element['relationships']['option1'] = array(
      '#type' => 'item',
      '#title' => 'Option #1',
      '#description' => t('Use this option to limit the vocabularies that a user .
        could use to specify relationship types. With this option any term in .
        the vocabulary can be used for the relationship type. You may select
        more than one vocabulary.'),

    );
    $element['relationships']['option1_vocabs'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#options' => $vocs,
      '#size' => 6,
      '#default_value' => $this->instance['settings']['relationships']['option1_vocabs'],
      // TODO add ajax here so that the relationship autocomplete below works
    );
    $element['relationships']['option2'] = array(
      '#type' => 'item',
      '#title' => '<b>Option #2</b>',
      '#description' => 'Some vocabularies are heirarchichal (an ontology). Within this
         heirarchy groups of related terms typically fall under a common parent. If you
         wish to limit the list of terms that a user can use for the relationship type,
         you can provide the parent term here.  Then, only that term\'s children will
         be avilable for use as a relationship type.',
    );
    $element['relationships']['option2_vocab'] = array(
      '#type' => 'select',
      '#description' => 'Specify Default Vocabulary',
      '#multiple' => FALSE,
      '#options' => $vocs,
      '#default_value' => $this->instance['settings']['relationships']['option2_vocab'],
      '#ajax' => array(
        'callback' => "sbo__relationship_instance_settings_form_ajax_callback",
        'wrapper' => 'relationships-option2-parent',
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    $element['relationships']['option2_parent'] = array(
      '#type' => 'textfield',
      '#description' => 'Specify a Heirarchical Parent Term',
      '#default_value' => $this->instance['settings']['relationships']['option2_parent'],
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
      '#prefix' => '<div id=relationships-option2-parent>',
      '#suffix' => '</div>'
    );
    $element['relationships']['option3'] = array(
      '#type' => 'item',
      '#title' => 'Option #3',
      '#description' => 'Provide terms separated by a new line. The term provided should be
        unique and distinguishable by the name. You can use a bar | to separate a vocabulary
        and a term to allow more specific assignment.',
    );
    $element['relationships']['relationship_types'] = array(
      '#type' => 'textarea',
      '#default_value' => $this->instance['settings']['relationships']['relationship_types'],
    );

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
    $form_state['values']['instance']['settings']['relationships']['relationship_types']= trim($settings['relationship_types']);

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
      foreach($rel_types AS $type) {
        $type =  trim($type);
        // Ignore empty lines
        if ($type == '') {
          continue;
        }
        // Find the matching cvterm
        $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name";
        $results = chado_query($sql, array(':name' => $type));
        $terms = array();
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
            $cv = tripal_get_cv(array('name' => trim($tmp[0])));
            if($cv) {
              $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name AND cv_id = :cv_id";
              $results = chado_query($sql, array(':name' => $type, ':cv_id' => $cv->cv_id));
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
            $token = array('@type' => $type);
            if ($cv) {
              $message =  "The term '@type' can not be found within the vocabulary '@vocab'.";
              $token['@vocab'] = $cv;
            }
            form_set_error("instance][settings][relationships][relationship_types",
                t($message, $token));
          }
        }
        else if (count($terms) > 1) {
          // If a type matches more than one term, parse it as 'vocabulary|cvterm' and try again
          if (strpos($type, '|')) {
            $tmp = explode('|', $type, 2);
            $type = trim($tmp[1]);
            $cv = tripal_get_cv(array('name' => trim($tmp[0])));
            if ($cv) {
              $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name AND cv_id = :cv_id";
              $results = chado_query($sql, array(':name' => $type, ':cv_id' => $cv->cv_id));
              while ($obj = $results->fetchObject()) {
                $terms[] = $obj;
              }
            }
          }
          if(count($terms) != 1) {
            form_set_error("instance][settings][relationships][relationship_types",
              t("The term '@type' matches more than one term. Please specify its vocabulary in the format of 'vocabulary|@type'.", array('@type' => $type)));
          }
        }
      }
    }

    // For option2: Make sure the parent term is a valid cvterm
    if ($option2) {
      $cv_id = $settings['option2_vocab'];
      $supertype = $settings['option2_parent'];
      $term = tripal_get_cvterm(array(
        'name' => trim($supertype),
        'cv_id' => $cv_id,
      ));
      // Tripal cv autocomplete also allow cvterm synonyms, if the parent term doesn't match
      // a cvterm, try cvtermsynonym
      if (!$term) {
        $synonym = tripal_get_cvterm(
            array(
              'synonym' => array(
                'name' => trim($supertype),
              )
            )
            );
        if ($synonym && $synonym->cv_id->cv_id == $cv_id) {
          $term = $synonym;
        }
      }
      if (!isset($term->cvterm_id)) {
        form_set_error("instance][settings][relationships][option2_parent",
            t("The term '@type' is not a valid term for the vocabulary selected.", array('@type' => $supertype)));
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

    $schema = chado_get_schema($field_table);
    $fkeys = $schema['foreign keys'];

    // 'nd_reagent_relationship' and 'project_relationship' have different column names from
    // subject_id/object_id. Do a pattern matching to get the column names.
    $subject_id_key = 'subject_id';
    $object_id_key = 'object_id';
    foreach ($schema['foreign keys'][$base_table]['columns'] AS $lcolum => $rcolum) {
      if (preg_match('/^subject_.*id/', $lcolum)) {
        $subject_id_key = $lcolum;
      }
      else if (preg_match('/^object_.*id/', $lcolum)) {
        $object_id_key = $lcolum;
      }
    }

    foreach ($items as $delta => $item) {
      $subject_id = $item['chado-' . $field_table . '__' . $subject_id_key];
      $object_id = $item['chado-' . $field_table . '__' . $object_id_key];
      $type_id = $item['chado-' . $field_table . '__type_id'];
      $type_id = isset($item['type_id']) ? $item['chado-' . $field_table . '__type_id'] : $type_id;
      $type_name = isset($item['type_name']) ? $item['type_name'] : '';
      $subject_name = $item['subject_name'];
      $object_name = $item['object_name'];


      // If the row is empty then just continue, there's nothing to validate.
      if (!$type_id and !$type_name and !$subject_name and !$object_name) {
        continue;
      }

      // Make sure we have values for all of the fields.
      $form_error = FALSE;
      if (!$type_name && !$type_id) {
        $errors[$field_name][$delta]['und'][] = array(
          'error' => 'sbo__relationship',
          'message' => t("Please provide the type of relationship."),
        );
      }
      if ($entity and !$subject_name) {
        $errors[$field_name][$delta]['und'][] = array(
          'error' => 'sbo__relationship',
          'message' => t("Please provide the subject of the relationship."),
        );
      }
      if ($entity and !$object_name) {
        $errors[$field_name][$delta]['und'][] = array(
          'error' => 'sbo__relationship',
          'message' => t("Please provide the object of the relationship."),
        );
      }
      if ($form_error) {
        continue;
      }

      // Before submitting this form we need to make sure that our subject_id and
      // object_ids are real records.  There are two ways to get the record, either
      // just with the text value or with an [id: \d+] string embedded.  If the
      // later we will pull it out.
      $subject_id = '';
      $fkey_rcolumn = $fkeys[$base_table]['columns'][$subject_id_key];
      $matches = array();
      if ($entity) {
        if(preg_match('/\[id: (\d+)\]/', $subject_name, $matches)) {
          $subject_id =  $matches[1];
          $values = array($fkey_rcolumn => $subject_id);
          $subject = chado_select_record($base_table, array($fkey_rcolumn), $values);
          if (count($subject) == 0) {
            $errors[$field_name][$delta]['und'][] = array(
              'error' => 'sbo__relationship',
              'message' => t("The subject record cannot be found using the specified id (e.g. [id: xx])."),
            );
          }
        }
        else {
          $values = array('uniquename' => $subject_name);
          $subject = chado_select_record($base_table, array($fkey_rcolumn), $values);
          if (count($subject) == 0) {
            $errors[$field_name][$delta]['und'][] = array(
              'error' => 'sbo__relationship',
              'message' => t("The subject record cannot be found. Please check spelling."),
            );
          }
          elseif (count($subject) > 1) {
            $errors[$field_name][$delta]['und'][] = array(
              'error' => 'sbo__relationship',
              'message' => t("The subject is not unique and therefore the relationship cannot be made."),
            );
          }
        }
      }

      // Now check for a matching object.
      $object_id = '';
      $fkey_rcolumn = $fkeys[$base_table]['columns'][$object_id_key];
      $matches = array();
      if ($entity) {
        if (preg_match('/\[id: (\d+)\]/', $object_name, $matches)) {
          $object_id = $matches[1];
          $values = array($fkey_rcolumn => $object_id);
          $object = chado_select_record($base_table, array($fkey_rcolumn), $values);
          if (count($subject) == 0) {
            $errors[$field_name][$delta]['und'][] = array(
              'error' => 'sbo__relationship',
              'message' => t("The object record cannot be found using the specified id (e.g. [id: xx])."),
            );
          }
        }
        else {
          $values = array('uniquename' => $object_name);
          $object = chado_select_record($base_table, array($fkey_rcolumn), $values);
          if (count($object) == 0) {
            $errors[$field_name][$delta]['und'][] = array(
              'error' => 'sbo__relationship',
              'message' => t("The object record cannot be found. Please check spelling."),
            );;
          }
          elseif (count($object) > 1) {
            $errors[$field_name][$delta]['und'][] = array(
              'error' => 'sbo__relationship',
              'message' =>  t("The object is not unique and therefore the relationship cannot be made."),
            );
          }
        }
      }

      // Make sure that either our object or our subject refers to the base record.
      if ($entity) {
        $chado_record_id = $entity->chado_record_id;
        if ($object_id != $chado_record_id  and $subject_id != $chado_record_id) {
          $errors[$field_name][$delta]['und'][] = array(
            'error' => 'sbo__relationship',
            'message' =>  t("Either the subject or the object in the relationship must refer to this record."),
          );
        }

        // Make sure that the object and subject are not both the same thing.
        if ($object_id == $subject_id) {
          $errors[$field_name][$delta]['und'][] = array(
            'error' => 'sbo__relationship',
            'message' =>  t("The subject and the object in the relationship cannot both refer to the same record."),
          );
        }
      }
    }
  }



  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {

  }
}