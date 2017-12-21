<?php

class so__genotype extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Genotype';

  // The default description for this field.
  public static $description = 'Associates an indviddual or organization with
          this record.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SO',
    // The name of the term.
    'term_name' => 'Genotype',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0001027',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // The default widget for this field.
  public static $default_widget = 'so__genotype_widget';

  // The default formatter for this field.
  public static $default_formatter = 'so__genotype_formatter';

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
    return array(
      $field_term => array(
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:complexType',
        'readonly' => TRUE,
        'elements' => array(
          'rdfs:type' => array(
            'searchable' => FALSE,
            'label' => 'Genotype Type',
            'help' => 'The type of genotype.',
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ),
          'schema:name' => array(
            'label' => 'Genotype Name',
            'help' => 'The name of the genotype.',
            'searchable' => FALSE,
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ),
          'schema:description' => array(
            'label' => 'Genotype Description',
            'help' => 'A description of the genotype.',
            'searchable' => FALSE,
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ),
        )
      ),
    );
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
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      $field_table . '__' . $pkey => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'genotype_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_genotype';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'genotype_id' => array(
          'type_id' => array(
            'dbxref_id' => array(
              'db_id' => TRUE,
            ),
          ),
        ),
        $fkey_lcolumn => TRUE,
      ),
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $genotype_linkers = isset($record->$linker_table->$fkey_rcolumn) ? $record->$linker_table->$fkey_rcolumn : '';
    if ($genotype_linkers) {
      foreach ($genotype_linkers as $i => $genotype_linker) {
        $genotype = $genotype_linker->genotype_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'rdfs:type' => $genotype->type_id->name,
            'schema:name' => $genotype->name,
            'schema:description' => $genotype->description,
          ),
          $field_table . '__' . $pkey => $genotype_linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $genotype_linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'genotype_id' => $genotype->genotype_id
        );

        if ($genotype && property_exists($genotype, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $genotype->entity_id;
        }
      }
    }
  }

}