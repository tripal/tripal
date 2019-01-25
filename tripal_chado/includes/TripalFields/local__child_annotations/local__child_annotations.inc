<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class local__child_annotations extends ChadoField {

  public static $default_label = 'Child Annotations';
  public static $default_description = 'All of the annotations associated with child features.';
  public static $default_widget = 'local__child_annotations_widget';
  public static $default_formatter = 'local__child_annotations_formatter';
  public static $module = 'tripal_chado';
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    'searchable_keys' => [],
  ];
  public static $download_formatters = [
    'TripalTabDownloader',
    'TripalCSVDownloader',
  ];
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'child_properties',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'child_properties',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  ];

  public static $no_ui = FALSE;
  public static $no_data = FALSE;

  /**
   * @see ChadoField::load()
   **/
  public function load($entity) {
    parent::load($entity);

    $master = $entity->data__sequence_features['und'];

    $properties = [];

//    foreach ($master as $child){
//
//      //remove all the other fields from the expand_var.
//
//    }

    //TODO: filter this to just hte annotations.
      $entity->local__child_properties['und'] = $master;

  }

  /**
   * @see ChadoField::query()
   **/
  public function query($query, $condition) {
  }

  /**
   * @see ChadoField::queryOrder()
   **/
  public function queryOrder($query, $order) {
  }

  /**
   * @see ChadoField::elementInfo()
   **/
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
      ],
    ];
  }
}
