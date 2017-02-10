<?php
/**
* Implements hook_field_group_info().
*/

function _summary_field_group_info($bundle_name, $fields){

  //Tripal pane to nest the summary fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_summary_tripalpane|TripalEntity|'.$bundle_name.'|default';
  $field_group_tripalpane->group_name = 'group_summary_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = array(
    'label' => 'Tripal Pane Summary',
    'weight' => '1',
    'children' => array(
      0 => 'group_summary',
    ),
    'format_type' => 'tripalpane',
    'format_settings' => array(
      'label' => 'Tripal Pane Summary',
      'instance_settings' => array(
        'id' => 'tripal_ds-fieldset-group_summary_tripalpane',
        'classes' => 'group-summary-tripalpane field-group-tripalpane',
        'description' => '',
      ),
    ),
  );
  drupal_write_record('field_group', $field_group_tripalpane);

  //Write to the tripal_ds table to record the new tripal pane.
  $field_for_table = new stdClass();
  $field_for_table->tripal_ds_field_name = 'group_summary_tripalpane';
  $field_for_table->tripal_ds_field_label = 'Summary';
  $field_for_table->entity_type = 'TripalEntity';
  $field_for_table->bundle = $bundle_name;

  drupal_write_record('tripal_ds', $field_for_table);

  //Fieldset field to nest the table within.
  $field_group_fieldset = new stdClass();
  $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_fieldset->api_version = 1;
  $field_group_fieldset->identifier = 'group_summary|TripalEntity|'.$bundle_name.'|default';
  $field_group_fieldset->group_name = 'group_summary';
  $field_group_fieldset->entity_type = 'TripalEntity';
  $field_group_fieldset->bundle = $bundle_name;
  $field_group_fieldset->mode = 'default';
  $field_group_fieldset->parent_name = 'group_summary_tripalpane';
  $field_group_fieldset->data = array(
    'label' => 'Summary',
    'weight' => '5',
    'children' => array(
      0 => 'group_summary_table',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Summary',
      'instance_settings' => array(
        'id' => '',
        'classes' => 'group-summary field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsible',
    ),
  );

  drupal_write_record('field_group', $field_group_fieldset);

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_summary_table|TripalEntity|'.$bundle_name.'|default';
  $field_group->group_name = 'group_summary_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_summary';
  $field_group->data = array(
    'label' => 'Summary Table',
    'weight' => '30',
    'children' => $fields,
    'format_type' => 'table',
    'format_settings' => array(
      'label' => 'Summary Table',
      'instance_settings' => array(
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-summary-table field-group-table',
      ),
    ),
  );

  drupal_write_record('field_group', $field_group);


}
/**
* Implements hook_field_group_info().
*/

function _prop_field_group_info($bundle_name, $fields){
  //Tripal pane  to nest the fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_prop_tripalpane|TripalEntity|'.$bundle_name.'|default';
  $field_group_tripalpane->group_name = 'group_prop_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = array(
    'label' => 'Tripal Pane Properties',
    'weight' => '2',
    'children' => array(
        0 => 'group_prop',
    ),
    'format_type' => 'tripalpane',
    'format_settings' => array(
      'label' => 'Tripal Pane Properties',
      'instance_settings' => array(
        'id' => 'tripal_ds-fieldset-group_prop_tripalpane',
        'classes' => 'group-prop-tripalpane field-group-tripalpane',
        'description' => '',
      ),
    ),
  );
  drupal_write_record('field_group', $field_group_tripalpane);

  //write to the tripal_ds table to record the new tripal pane.
  $field_for_table = new stdClass();
  $field_for_table->tripal_ds_field_name = 'group_prop_tripalpane';
  $field_for_table->tripal_ds_field_label = 'Properties';
  $field_for_table->entity_type = 'TripalEntity';
  $field_for_table->bundle = $bundle_name;

  drupal_write_record('tripal_ds', $field_for_table);


  //Fieldset field to nest the table within.
  $field_group_fieldset = new stdClass();
  $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_fieldset->api_version = 1;
  $field_group_fieldset->identifier = 'group_prop|TripalEntity|'.$bundle_name.'|default';
  $field_group_fieldset->group_name = 'group_prop';
  $field_group_fieldset->entity_type = 'TripalEntity';
  $field_group_fieldset->bundle = $bundle_name;
  $field_group_fieldset->mode = 'default';
  $field_group_fieldset->parent_name = 'group_prop_tripalpane';
  $field_group_fieldset->data = array(
    'label' => 'Properties',
    'weight' => '3',
    'children' => array(
      0 => 'group_prop_table',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Properties',
      'instance_settings' => array(
        'id' => '',
        'classes' => 'group-prop field-group-fieldset',
        'description' => '',
        ),
      'formatter' => 'collapsible',
    ),
  );

  drupal_write_record('field_group', $field_group_fieldset);

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_prop_table|TripalEntity|'.$bundle_name.'|default';
  $field_group->group_name = 'group_prop_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_prop';
  $field_group->data = array(
    'label' => 'Properties Table',
    'weight' => '30',
    'children' => $fields,
    'format_type' => 'table',
    'format_settings' => array(
      'label' => 'Properties Table',
      'instance_settings' => array(
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-prop-table field-group-table',
      ),
    ),
  );

  drupal_write_record('field_group', $field_group);
}
/**
* Implements hook_field_group_info().
*/

function _data_sequence_field_group_info($bundle_name, $fields){
  //Tripal pane  to nest the fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_sequence_tripalpane|TripalEntity|'.$bundle_name.'|default';
  $field_group_tripalpane->group_name = 'group_sequence_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = array(
    'label' => 'Tripal Pane Data Sequences',
    'weight' => '2',
    'children' => array(
        0 => 'group_sequence',
    ),
    'format_type' => 'tripalpane',
    'format_settings' => array(
      'label' => 'Tripal Pane Data Sequences',
      'instance_settings' => array(
        'id' => 'tripal_ds-fieldset-group_sequence_tripalpane',
        'classes' => 'group-sequence-tripalpane field-group-tripalpane',
        'description' => '',
      ),
    ),
  );
  drupal_write_record('field_group', $field_group_tripalpane);

  //Write to the tripal_ds table to record the new tripal pane.
  $field_for_table = new stdClass();
  $field_for_table->tripal_ds_field_name = 'group_sequence_tripalpane';
  $field_for_table->tripal_ds_field_label = 'Sequence';
  $field_for_table->entity_type = 'TripalEntity';
  $field_for_table->bundle = $bundle_name;

  drupal_write_record('tripal_ds', $field_for_table);

  //Fieldset field to nest the table within.
  $field_group_fieldset = new stdClass();
  $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_fieldset->api_version = 1;
  $field_group_fieldset->identifier = 'group_sequence|TripalEntity|'.$bundle_name.'|default';
  $field_group_fieldset->group_name = 'group_sequence';
  $field_group_fieldset->entity_type = 'TripalEntity';
  $field_group_fieldset->bundle = $bundle_name;
  $field_group_fieldset->mode = 'default';
  $field_group_fieldset->parent_name = 'group_sequence_tripalpane';
  $field_group_fieldset->data = array(
    'label' => 'Sequence',
    'weight' => '3',
    'children' => array(
      0 => 'group_sequence_table',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Data Sequences',
      'instance_settings' => array(
        'id' => '',
        'classes' => 'group-sequence field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsible',
    ),
  );

  drupal_write_record('field_group', $field_group_fieldset);

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sequence_table|TripalEntity|'.$bundle_name.'|default';
  $field_group->group_name = 'group_sequence_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_sequence';
  $field_group->data = array(
    'label' => 'Data Sequences Table',
    'weight' => '30',
    'children' => $fields,
    'format_type' => 'table',
    'format_settings' => array(
      'label' => 'Data Sequences Table',
      'instance_settings' => array(
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-sequence-table field-group-table',
      ),
    ),
  );

  drupal_write_record('field_group', $field_group);
}

/**
* Implements hook_field_group_info().
* Processes all additional fields into Tripal Panes
*/

function _additional_fields_field_group_info($bundle_name, $field_label, $group_field_name, $fieldset_field_name, $field_name){
  //Write to the tripal_ds table to record the new tripal pane.
  $field_for_table = new stdClass();
  $field_for_table->tripal_ds_field_name = $group_field_name;
  $field_for_table->tripal_ds_field_label = $field_label;
  $field_for_table->entity_type = 'TripalEntity';
  $field_for_table->bundle = $bundle_name;

  drupal_write_record('tripal_ds', $field_for_table);

  //Tripal pane to nest the fieldset within.
  $field_group_fieldset = new stdClass();
  $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_fieldset->api_version = 1;
  $field_group_fieldset->identifier = $group_field_name.'|TripalEntity|'.$bundle_name.'|default';
  $field_group_fieldset->group_name = $group_field_name;
  $field_group_fieldset->entity_type = 'TripalEntity';
  $field_group_fieldset->bundle = $bundle_name;
  $field_group_fieldset->mode = 'default';
  $field_group_fieldset->parent_name = '';
  $field_group_fieldset->data = array(
    'label' => $field_label.' TripalPane',
    'weight' => '5',
    'children' => array(
      0 => $fieldset_field_name,
    ),
    'format_type' => 'tripalpane',
    'format_settings' => array(
      'label' => $field_label.' TripalPane',
      'instance_settings' => array(
        'id' => 'tripal_ds-fieldset-'.$group_field_name,
        'classes' => $group_field_name.' field-group-tripalpane',
        'description' => '',
      ),
      'formatter' => 'collapsible',
    ),
  );

  drupal_write_record('field_group', $field_group_fieldset);

  //Fieldset to nest the field within the tripal pane.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier =  $fieldset_field_name.'|TripalEntity|'.$bundle_name.'|default';
  $field_group->group_name =  $fieldset_field_name;
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name =  $group_field_name;
  $field_group->data = array(
    'label' => $field_label,
    'weight' => '30',
    'children' => array(
      0 =>$field_name,
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => $field_label,
      'instance_settings' => array(
        'label_visibility' => '1',
        'id' => '',
        'classes' => $fieldset_field_name.' field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsible',
    ),
  );

  drupal_write_record('field_group', $field_group);

}

