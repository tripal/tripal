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

    $table_name = $target['data_table'];
    $type_table = $target['type_table'];
    $type_field = $target['field'];
    $cv_id      = $target['cv_id'];
    $cvterm_id  = $target['cvterm_id'];

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
          'name' => '',
          'accession' => '',
          'ns' => '',
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
  }


  public function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

  }

  public function load($field, $entity, $details) {
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'contact_id' => '',
      $field_table . '--' . 'contact__name' => '',
    );

    $linker_table = $base_table . '_contact';
    $options = array(
      'return_array' => 1,
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);

    if (count($record->$linker_table) > 0) {
      $i = 0;
      foreach ($record->$linker_table as $index => $linker) {
        $contact = $linker->contact_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => $linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'contact_id' => $contact->contact_id,
          $field_table . '--' . 'contact__name' => $contact->name,
        );
        if (property_exists($entity->chado_record->$field_column, 'entity_id')) {
          $fk_entity_id = $entity->chado_record->$field_column->entity_id;
          $entity->{$field_name}['und'][$i]['entity_id'] = $fk_entity_id;
          $entity->{$field_name}['und'][$i]['entity_type'] = 'TripalEntity';
        }

        $i++;
      }
    }
  }

  public function settings_form($field, $instance, $has_data) {

  }


}