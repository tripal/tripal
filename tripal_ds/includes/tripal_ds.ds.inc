<?php
/**
 * Implements hook_ds_layout_settings_info().
 */
function _ds_layout_settings_info($bundle_name, $instances) {
  $region_right = array();
  $region_left = array();
  $region_top = array();
  $region_bottom = array();
  $fields_with_regions = array();

  $i = 0;
  foreach ($instances as $key => $instance){
      $instance_name = $instance['field_name'];

      if($instance_name=="rdfs__type"){
        array_push($region_top, $instance_name);
        $fields_with_regions[$instance_name]= 'top';

      } else {
        $instance_base_table = $instance['settings']['base_table'];
        $instance_base_chado = $instance['settings']['chado_table'];

        if ($instance_base_chado && $instance_base_table){
          if ($instance_base_chado == $instance_base_table){
              //overview section instances
              array_push($region_top, $instance_name);
              $fields_with_regions[$instance_name]= 'top';

          } elseif ($instance_base_chado != $instance_base_table){
            watchdog('debug', '<pre>_ds_layout_settings_info  $instance_base_chado: '. print_r($instance_base_chado, TRUE) .'</pre>');

            if (strpos($instance_base_chado, 'prop') !== FALSE){
              //Properties section instances
              array_push($region_bottom, $instance_name);
              $fields_with_regions[$instance_name]= 'bottom';

            } elseif (strpos($instance_base_chado, 'data__sequence') !== FALSE){
              array_push($region_left, $instance_name);
              $fields_with_regions[$instance_name]= 'left';

            } else {
              //Linker section instances
              array_push($region_right, $instance_name);
              $fields_with_regions[$instance_name]= 'right';

            }
          } 
        }
      }
    $i++;
  }

  $record = new stdClass;
  $record->id ='TripalEntity|' . $bundle_name . '|default';
  $record->entity_type = 'TripalEntity';
  $record->bundle = $bundle_name;
  $record->view_mode = 'default';
  $record->layout = 'tripal_ds_feature';
  $settings = array(
    'regions' => array(
      'top' => 
        $region_top,
      'left' => 
        $region_left,
      'right' => 
        $region_right,
      'bottom' => 
        $region_bottom,
    ),
    'fields' => 
      $fields_with_regions,
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

  watchdog('debug', '<pre>_ds_layout_settings_info  $record: '. print_r($record, TRUE) .'</pre>');

  drupal_write_record('ds_layout_settings', $record);
}
/*

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
*/
/*
 * Implements hook_ds_custom_fields_info().


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
 */