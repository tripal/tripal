<?php

class sbo__phenotype extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Phenotype';

  // The default description for this field.
  public static $description = 'A biochemical network can generate phenotypes or affects
      biological processes. Such processes can take place at different levels and are
      independent of the biochemical network itself.';

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
    'term_name' => 'Phenotype',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000358',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'sbo__phenotype_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sbo__phenotype_formatter';

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
    return [
      $field_term => [
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:string',
        'readonly' => TRUE,
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

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $entity->chado_table;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      $field_table . '__' . $pkey => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'phenotype_id' => '',
      // Ignore the synonym_sgml column for now.
    ];

    $linker_table = $base_table . '_phenotype';
    $options = [
      'return_array' => 1,
      'include_fk' => [
        'phenotype_id' => [
          'attr_id' => [
            'dbxref_id' => [
              'db_id' => TRUE,
            ],
          ],
        ],
        $fkey_lcolumn => TRUE,
      ],
    ];
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $phenotype_linkers = $record->$linker_table;
    if ($phenotype_linkers) {
      foreach ($phenotype_linkers as $i => $phenotype_linker) {
        $phenotype = $phenotype_linker->phenotype_id;
        $entity->{$field_name}['und'][$i]['value']['type'] = $phenotype->attr_id->name;
        $entity->{$field_name}['und'][$i]['value']['name'] = $phenotype->name;
        if ($phenotype->cvalue_id) {
          $entity->{$field_name}['und'][$i]['value']['value'] = $phenotype->cvalue_id->name;
        }
        else {
          $entity->{$field_name}['und'][$i]['value']['value'] = $phenotype->value;
        }

        $entity->{$field_name}['und'][$i][$field_table . '__' . $pkey] = $phenotype_linker->$pkey;
        $entity->{$field_name}['und'][$i][$field_table . '__' . $fkey_lcolumn] = $phenotype_linker->$fkey_lcolumn->$fkey_lcolumn;
        $entity->{$field_name}['und'][$i][$field_table . '__' . 'phenotype_id'] = $phenotype->phenotype_id;

        if ($phenotype && property_exists($phenotype, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $phenotype->entity_id;
        }
      }
    }
  }

}
