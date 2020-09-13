<?php

class sio__references extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'References';

  // The default description for this field.
  public static $description = 'Records references by this publication.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SIO',
    // The name of the term.
    'term_name' => 'references',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '000631',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'pub',
    // The primary key column of hte table in Dhado.
    'chado_column' => 'pub_id',
    // The base table.
    'base_table' => 'pub',
  ];

  // The default widget for this field.
  public static $default_widget = 'sio__references_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sio__references_formatter';

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => [],
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

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = [
      'value' => '',
    ];

    // Iterate through all of the _pub tables and look for any that have
    // linked to this record. If so then add them.
    $chado_tables = chado_get_table_names(TRUE);
    $delta = 0;
    foreach ($chado_tables as $chado_table) {
      $matches = [];
      if (preg_match('/^(.+?)_pub$/', $chado_table, $matches)) {
        $reference_table = $matches[1];

        // Find the base table this links to and get the fk columns that map it.
        $schema = chado_get_schema($chado_table);
        $fkeys = $schema['foreign keys'];
        foreach ($fkeys as $linked_table => $fk_details) {
          if ($linked_table == $reference_table) {
            $fkleft = array_keys($fk_details['columns'])[0];
            $fkright = $fk_details['columns'][$fkleft];
          }

        }
        // Iterate through all of the records in the linker table that
        // match the given pub ID.
        $records = chado_generate_var($chado_table, ['pub_id' => $chado_record->pub_id], ['return_array' => TRUE]);
        foreach ($records as $record) {
          // We want to add a 'type' and 'name' element to the values (at a
          // minimum) for each of the records.  Unfortunately, every base table
          // is different and their may not be an easy to identify name,
          // so... we'll do the best we can.
          $entity->{$field_name}['und'][$delta]['value'] = [];

          // First get the type of record.
          if (property_exists($record->$fkleft, 'type_id') and $record->$fkleft->type_id) {
            $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $record->$fkleft->type_id->name;
          }
          else {
            // If there's not a type_id column then see if the table is mapped
            // to a type.
            $mapping = db_select('chado_cvterm_mapping', 'CVM')
              ->fields('CVM')
              ->condition('chado_table', $reference_table)
              ->execute()
              ->fetchObject();
            if ($mapping) {
              $cvterm = chado_get_cvterm(['cvterm_id' => $mapping->cvterm_id]);
              $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $cvterm->name;
            }
          }

          // Add in the name and uniquename (identifier) if those fields exist.
          if (property_exists($record->$fkleft, 'name')) {
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $record->$fkleft->name;
          }
          if (property_exists($record->$fkleft, 'uniquename')) {
            $entity->{$field_name}['und'][$delta]['value']['data:0842'] = $record->$fkleft->name;
          }

          // If this records is also a published entity then include that.
          if (property_exists($record->$fkleft, 'entity_id')) {
            $entity->{$field_name}['und'][$delta]['value']['entity'] = 'TripalEntity:' . $record->$fkleft->entity_id;
          }

          // If this is the organism table then we will create the name
          // specially.
          if (property_exists($record->$fkleft, 'genus')) {
            $name = '<i>' . $record->$fkleft->genus . ' ' . $record->$fkleft->species . '</i>';
            if (property_exists($record->$fkleft, 'infraspecific_name')) {
              if ($record->$fkleft->type_id) {
                $name .= ' ' . $record->$fkleft->type_id->name;
              }
              $name .= ' ' . $record->$fkleft->infraspecific_name;
            }
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $name;
          }
          $delta++;
        }
      }
    }
  }

}
