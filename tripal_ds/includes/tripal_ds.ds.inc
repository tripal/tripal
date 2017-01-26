<?php
/**
 * Implements hook_ds_layout_settings_info().
 */
 /*function tripal_ds_ds_layout_settings_info() {
  $export = array(); 
  //$bundle_name = $bundle;
  $region_fields_right;
  $fields_string;

  //build the string for the regions and fields section from the $fields array
  $total_fields = count($fields[1][0]);
  $fields_string = implode(",", $fields[1][0]);

  for ($i=0; $i < $total_fields; $i++) { 
    //watchdog('debug', '<pre>_ds_layout_settings_info  $fields_string: '. print_r($fields[1][0][$i], TRUE) .'</pre>'); 
    $region_fields_right .= $i ." => ". $fields[1][0][$i].", ";
    $fields_string .= $fields[1][0][$i] ." => 'right', ";
    
  }

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'TripalEntity|bio_data_78|default';
  $ds_layout->entity_type = 'TripalEntity';
  $ds_layout->bundle = 'bio_data_78';
  $ds_layout->view_mode = 'default';
  $ds_layout->layout = 'tripal_ds_feature';
  $ds_layout->settings = array(
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
  $export['TripalEntity|bio_data_78|default'] = $ds_layout;
  
 if($export){
    // Insert the layout.
      db_insert('ds_layout_settings')
        ->fields(array(
          'id' => $ds_layout->id,
          'entity_type' => 'TripalEntity',
          'bundle' => $ds_layout->bundle,
          'view_mode' => 'default',
          'layout' => 'tripal_ds_feature',
        ))
        ->execute();
    }
    watchdog('debug', '<pre>_ds_layout_settings_info  $$ds_layout->settings: '. print_r($ds_layout->settings, TRUE) .'</pre>');
 if($ds_layout->settings ){
    $record = new stdClass;
    $record->id ='TripalEntity|bio_data_73|default';
    $record->entity_type = 'TripalEntity';
    $record->bundle = ;
    $record->view_mode = 'default';
    $record->settings = $ds_layout->settings;
    drupal_write_record('ds_field_settings', $record);
  }
  return $export;
}*/

/**
 * Implements hook_ds_layout_settings_info().
 */
function _ds_layout_settings_info($bundle_name, $fields) {
  $export = array(); 
  //$bundle_name = $bundle;
  $region_fields_right;
  $fields_string;

  //build the string for the regions and fields section from the $fields array
  $total_fields = count($fields[1][0]);
  $fields_string = implode(",", $fields[1][0]);

  for ($i=0; $i < $total_fields; $i++) { 
    //watchdog('debug', '<pre>_ds_layout_settings_info  $fields_string: '. print_r($fields[1][0][$i], TRUE) .'</pre>'); 
    $region_fields_right .= $i ." => ". $fields[1][0][$i].", ";
    $fields_string .= $fields[1][0][$i] ." => 'right', ";
    
  }

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'TripalEntity|'.$bundle_name.'|default';
  $ds_layout->entity_type = 'TripalEntity';
  $ds_layout->bundle = $bundle_name;
  $ds_layout->view_mode = 'default';
  $ds_layout->layout = 'tripal_ds_feature';
  $ds_layout->settings = array(
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
  $export['TripalEntity|'.$bundle_name.'|default'] = $ds_layout;

  module_load_include('inc', 'ds', 'ds.registry');
  _ds_entity_type_update('TripalEntity', $export, 'update');

  /*if($export){
    // Insert the layout.
      db_insert('ds_layout_settings')
        ->fields(array(
          'id' => $ds_layout->id,
          'entity_type' => 'TripalEntity',
          'bundle' => $ds_layout->bundle,
          'view_mode' => 'default',
          'layout' => 'tripal_ds_feature',
        ))
        ->execute();
    }
    watchdog('debug', '<pre>_ds_layout_settings_info  $$ds_layout->settings: '. print_r($ds_layout->settings, TRUE) .'</pre>');
 if($ds_layout->settings ){
    $record = new stdClass;
    $record->id ='TripalEntity|bio_data_73|default';
    $record->entity_type = 'TripalEntity';
    $record->bundle = ;
    $record->view_mode = 'default';
    $record->settings = $ds_layout->settings;
    drupal_write_record('ds_field_settings', $record);
  }*/
  return $export;
}


function tripal_ds_content_type_fields($bundle_name){
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

  //tripal_ds_ds_layout_settings_info($bundle_name, $all_fields);
  watchdog('debug', '<pre>tripal_ds_content_type_fields  $all_fields: '. print_r($all_fields, TRUE) .'</pre>');

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
