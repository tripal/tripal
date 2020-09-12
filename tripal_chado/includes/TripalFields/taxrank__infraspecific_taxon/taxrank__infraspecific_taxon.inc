<?php

class taxrank__infraspecific_taxon extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Infraspecific Taxon';

  // The default description for this field.
  public static $description = 'Specifies the infraspecific taxon of an organism.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'TAXRANK',
    // The name of the term.
    'term_name' => 'infraspecific_taxon',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000046',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'HERE_widget';

  // The default formatter for this field.
  public static $default_formatter = 'HERE_formatter';

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

    $label_term = 'rdfs:label';
    $infraspecific_name_term = chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = chado_get_semweb_term('organism', 'type_id');

    return [
      $field_term => [
        'sortable' => FALSE,
        'searchable' => TRUE,
        'readonly' => FALSE,
        'type' => 'xs:complexType',
        'elements' => [
          $label_term => [
            'name' => 'label',
            'sortable' => FALSE,
            'searchable' => TRUE,
            'label' => 'Infraspecific Full Name',
            'help' => 'The full infraspecific name including the rank and name.',
            'type' => 'xsstring',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $infraspecific_name_term => [
            'name' => 'infraspecific_name',
            'sortable' => TRUE,
            'searchable' => TRUE,
            'label' => 'Infraspecies Name',
            'help' => 'The infraspecific name of the organism below the rank of species.',
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
          $infraspecific_type_term => [
            'name' => 'infraspecific_rank',
            'sortable' => TRUE,
            'searchable' => TRUE,
            'label' => 'Infraspecific Rank',
            'help' => 'The infraspecific rank of the organism below the rank of species.',
            'type' => 'xs:string',
            'readonly' => FALSE,
            'required' => TRUE,
          ],
        ],
      ],
    ];
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $label_term = $field_term_id . ',' . 'rdfs:label';
    $infraspecific_name_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'type_id');

    if ($condition['column'] == $label_term or $condition['column'] == $field_term_id) {
      $this->queryJoinOnce($query, 'cvterm', $alias . '_cvterm', $alias . "_cvterm.cvterm_id = base.type_id");
      $query->where("CONCAT(" . $alias . "_cvterm.name, ' ', base.infraspecific_name) $operator :full_name", [':full_name' => $condition['value']]);
    }
    if ($condition['column'] == $infraspecific_name_term) {
      $query->condition('base.infraspecific_name', $condition['value'], $operator);
    }
    if ($condition['column'] == $infraspecific_type_term) {
      $this->queryJoinOnce($query, 'cvterm', $alias . '_cvterm', $alias . "_cvterm.cvterm_id = base.type_id");
      $query->condition($alias . '_cvterm.name', $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_term_id = $this->getFieldTermID();
    $label_term = $field_term_id . ',' . 'rdfs:label';
    $infraspecific_name_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'type_id');

    if ($order['column'] == $infraspecific_name_term) {
      $query->orderBy('base.infraspecific_name', $order['direction']);
    }
    if ($order['column'] == $infraspecific_type_term) {
      $this->queryJoinOnce($query, 'cvterm', $alias . '_cvterm', $alias . "_cvterm.cvterm_id = base.type_id", "LEFT OUTER");
      $query->orderBy($alias . '_cvterm.name', $order['direction']);
    }
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;
    $settings = $this->field['settings'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $entity->{$field_name}['und'][0]['value'] = '';

    if (chado_get_version() < 1.3) {
      return;
    }

    $label_term = 'rdfs:label';
    $infraspecific_name_term = chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = chado_get_semweb_term('organism', 'type_id');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-organism__infraspecific_name' => '',
      'chado-organism__type_id' => '',
    ];

    if ($record->infraspecific_name) {
      $label = $record->type_id->name . ' ' . $record->infraspecific_name;
      $entity->{$field_name}['und'][0]['value'][$label_term] = $label;
      $entity->{$field_name}['und'][0]['value'][$infraspecific_name_term] = $record->infraspecific_name;
      $entity->{$field_name}['und'][0]['chado-organism__infraspecific_name'] = $record->infraspecific_name;
    }

    if ($record->type_id) {
      $entity->{$field_name}['und'][0]['value'][$infraspecific_type_term] = $record->type_id->name;
      $entity->{$field_name}['und'][0]['chado-organism__type_id'] = $record->type_id->cvterm_id;
    }
  }
}
