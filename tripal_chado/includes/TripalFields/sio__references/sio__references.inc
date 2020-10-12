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
    // The number of items to show on a page.
    'items_per_page' => 10,
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
        $ref_schema = chado_get_schema($reference_table);
        $ref_pkey = $ref_schema['primary key'][0];
        $select = "SELECT REF.* ";
        $from = "FROM {" . $chado_table . "} LINK
            INNER JOIN {" . $reference_table . "} REF on LINK.$fkleft = REF.$fkright
        ";
        if (array_key_exists('type_id', $ref_schema['fields'])) {
          $select .= ", CVT.name as type_name";
          $from .= "INNER JOIN {cvterm} CVT on REF.type_id = CVT.cvterm_id ";
        }
        // Iterate through all of the records in the linker table that
        // match the given pub ID.
        $sql = "$select $from WHERE LINK.pub_id = :pub_id";
        $args = [':pub_id' => $chado_record->pub_id];
        $options = [
          'return_array' => TRUE,
          'include_fk' => [
            $fkright => [
              'type_id' => []
            ],
          ],
        ];

        // Get the mapping of the refrence table to a CV term.
        $ref_mapping = db_select('chado_cvterm_mapping', 'CVM')
          ->fields('CVM')
          ->condition('chado_table', $reference_table)
          ->execute()
          ->fetchObject();
        $ref_type = NULL;
        if ($ref_mapping) {
          $ref_type = chado_get_cvterm(['cvterm_id' => $ref_mapping->cvterm_id]);
        }

        //$records = chado_generate_var($chado_table, ['pub_id' => $chado_record->pub_id], $options);
        $records = chado_query($sql, $args);
        while($record = $records->fetchObject()) {
        //foreach ($records as $record) {

          // We want to add a 'type' and 'name' element to the values (at a
          // minimum) for each of the records.  Unfortunately, every base table
          // is different and there may not be an easy to identify name,
          // so... we'll do the best we can.
          $entity->{$field_name}['und'][$delta]['value'] = [];

          // First get the type of record.
          if (property_exists($record, 'type_name')) {
            $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $record->type_name;
          }
          else {
            if ($ref_type) {
              $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $ref_type->name;
            }
          }

          // Add in the name and uniquename (identifier) if those fields exist.
          if (property_exists($record, 'name')) {
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $record->name;
          }
          if (property_exists($record, 'uniquename')) {
            $entity->{$field_name}['und'][$delta]['value']['data:0842'] = $record->name;
          }

          // If this records is also a published entity then include that.
          $entity_id = chado_get_record_entity_by_table($reference_table, $record->$ref_pkey);
          if ($entity_id) {
            $entity->{$field_name}['und'][$delta]['value']['entity'] = 'TripalEntity:' . $entity_id;
          }

          // If this is the organism table then we will create the name
          // specially.
          if (property_exists($record, 'genus')) {
            $name = '<i>' . $record->genus . ' ' . $record->species . '</i>';
            if (property_exists($record, 'infraspecific_name')) {
              if ($record->$fkleft->type_id) {
                $name .= ' ' . $record->type_name;
              }
              $name .= ' ' . $record->infraspecific_name;
            }
            $entity->{$field_name}['und'][$delta]['value']['schema:name'] = $name;
            $entity->{$field_name}['und'][$delta]['value']['rdfs:type'] = $ref_type->name;
          }
          $delta++;
        }
      }
    }
  }

}
