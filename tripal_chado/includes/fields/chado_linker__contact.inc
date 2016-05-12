<?php

class chado_linker__contact extends TripalField {

  public function field_info() {
    return array(
      'label' => t('Contacts'),
      'description' => t('Associates an indviddual or organization with
          this record.'),
      'default_widget' => 'chado_linker__contact_widget',
      'default_formatter' => 'chado_linker__contact_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
 function widget_info() {
    return array(
      'label' => t('Contacts'),
      'field types' => array('chado_linker__contact'),
    );
  }

  public function formatter_info() {
    return array(
      'label' => t('Contacts'),
      'field types' => array('chado_linker__contact'),
      'settings' => array(
      ),
    );
  }
  public function attach_info($entity_type, $bundle, $settings) {
    $field_info = array();

    $table_name = $settings['data_table'];
    $type_table = $settings['type_table'];
    $type_field = $settings['field'];
    $cv_id      = $settings['cv_id'];
    $cvterm_id  = $settings['cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $contact_table = $table_name . '_contact';
    if (!chado_table_exists($contact_table)) {
      return $field_info;
    }

    $schema = chado_get_schema($contact_table);
    $pkey = $schema['primary key'][0];

    // Initialize the field array.
    $field_info = array(
      'field_name' => $table_name . '__contact',
      'field_type' => 'chado_linker__contact',
      'widget_type' => 'chado_linker__contact_widget',
      'widget_settings' => array('display_label' => 1),
      'description' => '',
      'label' => 'Contacts',
      'is_required' => 0,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'storage' => 'field_chado_storage',
      'field_settings' => array(
        'chado_table' => $contact_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => array(
          'name' => 'contact',
          'accession' => 'contact',
          'ns' => 'local',
          'nsurl' => '',
        ),
      ),
    );
    return $field_info;
  }

  public function formatter_settings_summary($field, $instance, $view_mode) {

  }

  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {

  }


  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {
        // Get the settings
        $settings = $display['settings'];
        $record = $entity->chado_record;

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
          if (array_key_exists('entity_id', $item) and $item['entity_id']) {
            $entity_id = $item['entity_id'];
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

        // once we have our table array structure defined, we call Drupal's theme_table()
        // function to generate the table.
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
  }


  public function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }

  public function load($field, $entity, $details) {
    $record = $details['record'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

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
      ),
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $contact_linkers = $record->$linker_table;
    if ($contact_linkers) {
      foreach ($contact_linkers as $i => $contact_linker) {
        $contact = $contact_linker->contact_id;
        $entity->{$field_name}['und'][$i]['value'] = array(
          '@type' => $contact->type_id->dbxref_id->db_id->name . ':' . $contact->type_id->dbxref_id->accession,
          'type' => $contact->type_id->name,
          'name' => $contact->name,
          'description' => $contact->description,
        );
        $entity->{$field_name}['und'][$i]['contact_id'] = $contact->contact_id;

        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['entity_id'] = $contact->entity_id;
          $entity->{$field_name}['und'][$i]['entity_type'] = 'TripalEntity';
        }
      }
    }

  }

  /**
   * @see TripalField::settings_form()
   */
  public function settings_form($field, $instance, $has_data) {
    $element = array();

    // TODO: add settings here.

    // Add in the semantic web fields.
    $parent_elements = parent::settings_form($field, $instance, $has_data);
    $element = array_merge($element, $parent_elements);

    return $element;
  }


}