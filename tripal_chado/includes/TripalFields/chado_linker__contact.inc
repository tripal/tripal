<?php

class chado_linker__contact extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Contacts';

  // The default description for this field.
  public static $default_description = 'Associates an indviddual or organization with
          this record';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // Get the settings
    $settings = $display['settings'];

    $headers = array('Name', 'Description', 'Type');
    $rows = array();

    foreach ($items as $delta => $item) {
      $contact = $item['value'];
      if (!$contact) {
        continue;
      }

      // Get the field values
      $contact_name = $contact['name'];
      $description = $contact['description'];
      $type = $contact['type'];

      // Add a link i there is an entity.
      if (array_key_exists('entity', $item['value']) and $item['value']['entity']) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $contact_name = l($contact_name, "bio_data/" . $entity_id, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array($contact_name, $description, $type);
    }
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_linker-table-contact-object',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => "",
      'colgroups' => array(),
      'empty' => 'No contacts available',
    );
    $content = theme_table($table);

    if (count($items) > 0) {
      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }


  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

  }

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $record = $details['record'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];


    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      $field_table . '__' . $pkey => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'contact_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_contact';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'contact_id' => array(
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
    $contact_linkers = $record->$linker_table;
    if ($contact_linkers) {
      foreach ($contact_linkers as $i => $contact_linker) {
        $contact = $contact_linker->contact_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'type' => $contact->type_id->name,
            'name' => $contact->name,
            'description' => $contact->description,
          ),
          // Add in the semantic web settings.  This array is expected by
          // other Tripal modules that handle semantic web for fields.
          'semantic_web' => array(
            'type' => $contact->type_id->dbxref_id->db_id->name . ':' . $contact->type_id->dbxref_id->accession,
            'name' => tripal_get_chado_semweb_term('contact', 'name'),
            'description' => tripal_get_chado_semweb_term('contact', 'description'),
          ),
          // Add in subfield mapping to Chado tables. This is used by the
          // chado_field_storage for performing queries on sub element values.
          // It should be a comma-separated list (no spacing) of the field names
          // as foreign keys are followed starting from the Chado table to which
          // this field maps.
          'chado_mapping' => array(
            'type' => 'type_id,name',
            'name' => 'contact_id,name',
            'description' => 'contact_id,name'
          ),
          $field_table . '__' . $pkey => $contact_linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $contact_linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'contact_id' => $contact->contact_id
        );

        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $contact->entity_id;
        }
      }
    }
  }
}