<?php
/**
 * Implements hook_ds_layout_settings_info().
 */
function _ds_layout_settings_info($bundle_name, $instances) {
  $export = array();
  $record = new stdClass;
  $record->id ='TripalEntity|' . $bundle_name . '|default';
  $record->entity_type = 'TripalEntity';
  $record->bundle = $bundle_name;
  $record->view_mode = 'default';
  $record->layout = 'tripal_ds_feature';
  $settings = array(
    'regions' => array(
      'top' => array(),
      'left' => array(),
      'right' => array(
        0 => 'project_contact',
        1 => 'rdfs__type',
        2 => 'sbo__database_cross_reference',
        3 => 'sbo__relationship',
        4 => 'schema__description',
        5 => 'schema__name',
        6 => 'schema__publication',
      ),
      'bottom' => array(),
    ),
    'fields' => array(
      'project_contact' => 'right',
      'rdfs__type' => 'right',
      'sbo__database_cross_reference' => 'right',
      'sbo__relationship' => 'right',
      'schema__description' => 'right',
      'schema__name' => 'right',
      'schema__publication' => 'right',
    ),
    'classes' => array(),
    'wrappers' => array(
      'top' => 'div',
      'left' => 'div',
      'right' => 'div',
      'bottom' => 'div',
    ),
    'layout_wrapper' => 'div',
    'layout_attributes' => '',
    'layout_attributes_merge' => 1,
    'layout_link_attribute' => '',
    'layout_link_custom' => '',
    'layout_disable_css' => 0,
  );
  $record->settings = $settings;
  drupal_write_record('ds_layout_settings', $record);
}


function tripal_ds_content_type_fields($bundle_name){
  dpm(debug_backtrace());
  $base_fields = array();
  $all_fields = array();
  $custom_fields = array();

  $fields = db_select('field_config_instance', 'i')
    ->fields('i', array('field_name'))
    ->condition('bundle', $bundle_name)
    ->execute()
    ->fetchAll();

  //break apart each field to grab the base table identifiers
  foreach ($fields as $fields => $field) {
    $field_parts = explode('__', $field->field_name, 2);

    if(count($field_parts) > 1) {
      $base_field = $field_parts[0];
      $nonbase_field = $field_parts[1];
    } else {
      $base_field = $field_parts[0];
    }
    array_push($base_fields, $base_field);
    array_push($all_fields, $field->field_name);

  }
  //create the left base table fields based on the previous foreach loop
  if ($base_fields) {
      $base_fields = array_unique($base_fields);
      foreach($base_fields as $base_fields => $basefield){
        $custom_field = tripal_ds_ds_custom_fields_info($basefield);
        array_push($custom_fields, $custom_field);

      }
  }

  $all_fields = array(
    array($base_fields),
    array($all_fields),
    array($custom_fields),
  );

  return $all_fields;

}

/*
 * Implements hook_ds_custom_fields_info().
 */

function tripal_ds_ds_custom_fields_info($field) {
  $export = array();

  $ds_field = new stdClass;
  $ds_field->api_version = 1;
  $ds_field->field = $field;
  $ds_field->label = $field;
  $ds_field->field_type = 6;
  $ds_field->entities = array(
    'TripalEntity' => 'TripalEntity',
  );
  $ds_field->ui_limit = '';
  $ds_field->properties = array(
    'block' => 'node|recent',
    'block_render' => '2',
  );
  $export[$field] = $ds_field;

  return $export;
}
