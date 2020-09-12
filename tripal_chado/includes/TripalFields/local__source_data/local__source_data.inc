<?php

class local__source_data extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Source of Data';

  // The default description for this field.
  public static $description = 'The source data used for this analysis.';

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
    'term_name' => 'source_data',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'source_data',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'local__source_data_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__source_data_formatter';

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

    $sourcename_term = chado_get_semweb_term('analysis', 'sourcename');
    $sourceversion_term = chado_get_semweb_term('analysis', 'sourceversion');
    $sourceuri_term = chado_get_semweb_term('analysis', 'sourceuri');

    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => [
          $sourcename_term => [
            'searchable' => TRUE,
            'label' => 'Data Source Name',
            'help' => 'The name of the data source used for the analysis.',
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $sourceversion_term => [
            'searchable' => TRUE,
            'label' => 'Data Source Version',
            'help' => 'If applicable, the version number of the source data used for the analysis.',
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $sourceuri_term => [
            'searchable' => FALSE,
            'label' => 'Data Source URI',
            'help' => 'If applicable, the universal resource indicator (e.g. URL) of the source data used for the analysis.',
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $sourcename_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'sourcename');
    $sourceversion_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'sourceversion');
    $sourceuri_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'sourceuri');

    if ($condition['column'] == $sourcename_term) {
      $query->condition("base.sourcename", $condition['value'], $operator);
    }
    if ($condition['column'] == $sourceversion_term) {
      $query->condition("base.sourceversion", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $field_term_id = $this->getFieldTermID();
    $sourcename_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'sourcename');
    $sourceversion_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'sourceversion');
    $sourceuri_term = $field_term_id . ',' . chado_get_semweb_term('analysis', 'sourceuri');

    if ($order['column'] == $sourcename_term) {
      $query->orderBy("base.sourcename", $order['direction']);
    }
    if ($order['column'] == $sourceversion_term) {
      $query->orderBy("base.sourceversion", $order['direction']);
    }
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $analysis = $entity->chado_record;
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $sourcename_term = chado_get_semweb_term('analysis', 'sourcename');
    $sourceversion_term = chado_get_semweb_term('analysis', 'sourceversion');
    $sourceuri_term = chado_get_semweb_term('analysis', 'sourceuri');

    $entity->{$field_name}['und'][0] = [
      'value' => [
        $sourcename_term => $analysis->sourcename,
        $sourceversion_term => $analysis->sourceversion,
        $sourceuri_term => $analysis->sourceuri,
      ],
      'chado-analysis__sourcename' => $analysis->sourcename,
      'chado-analysis__sourceversion' => $analysis->sourceversion,
      'chado-analysis__sourceuri' => $analysis->sourceuri,
    ];
  }
}