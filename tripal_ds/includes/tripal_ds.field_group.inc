<?php
/**
 * Creates the Summary tripal pane and the table within that pane.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $fields
 *  Array of the machine names of the children of the field group being created.
 */
function _summary_field_group_info($bundle_name, $fields) {
  //Tripal pane to nest the summary fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_summary_tripalpane|TripalEntity|' . $bundle_name . '|default';
  $field_group_tripalpane->group_name = 'group_summary_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = [
    'label' => 'Summary',
    'weight' => '1',
    'children' => [
      0 => 'group_summary_table',
    ],
    'format_type' => 'tripalpane',
    'format_settings' => [
      'label' => 'Summary',
      'instance_settings' => [
        'id' => 'tripal_ds-fieldset-group_summary_tripalpane',
        'classes' => 'group-summary-tripalpane field-group-tripalpane',
        'description' => '',
        'hide' => 0,
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group_tripalpane);
  //Write to the tripal_ds table to record the new tripal pane.
  tripal_ds_bundle_menu_item($bundle_name, 'Summary', 'group_summary_tripalpane', 'TripalEntity');

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_summary_table|TripalEntity|' . $bundle_name . '|default';
  $field_group->group_name = 'group_summary_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_summary_tripalpane';
  $field_group->data = [
    'label' => 'Summary Table',
    'weight' => '1',
    'children' => $fields,
    'format_type' => 'table',
    'format_settings' => [
      'label' => 'Summary Table',
      'instance_settings' => [
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-summary-table field-group-table',
      ],
    ],
  ];

  drupal_write_record('field_group', $field_group);
}

/**
 * Creates the Properties Table tripal pane and the table within that pane.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $fields
 *  Array of the machine names of the children of the field group being created.
 */
function _prop_field_group_info($bundle_name, $fields) {
  //Tripal pane  to nest the fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_prop_tripalpane|TripalEntity|' . $bundle_name . '|default';
  $field_group_tripalpane->group_name = 'group_prop_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = [
    'label' => 'Properties',
    'weight' => '1',
    'children' => [
      0 => 'group_prop_table',
    ],
    'format_type' => 'tripalpane',
    'format_settings' => [
      'label' => 'Properties',
      'instance_settings' => [
        'id' => 'tripal_ds-fieldset-group_prop_tripalpane',
        'classes' => 'group-prop-tripalpane field-group-tripalpane',
        'description' => '',
        'hide' => 1,
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group_tripalpane);
  //Write to the tripal_ds table to record the new tripal pane.
  tripal_ds_bundle_menu_item($bundle_name, 'Properties', 'group_prop_tripalpane', 'TripalEntity');

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_prop_table|TripalEntity|' . $bundle_name . '|default';
  $field_group->group_name = 'group_prop_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_prop_tripalpane';
  $field_group->data = [
    'label' => 'Properties Table',
    'weight' => '1',
    'children' => $fields,
    'format_type' => 'table',
    'format_settings' => [
      'label' => 'Properties Table',
      'instance_settings' => [
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-prop-table field-group-table',
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group);
}

/**
 * Creates the Data Sequence tripal pane and the table within that pane.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $fields
 *  Array of the machine names of the children of the field group being created.
 */
function _data_sequence_field_group_info($bundle_name, $fields) {
  //Tripal pane  to nest the fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_sequence_tripalpane|TripalEntity|' . $bundle_name . '|default';
  $field_group_tripalpane->group_name = 'group_sequence_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = [
    'label' => 'Sequences',
    'weight' => '1',
    'children' => [
      0 => 'group_sequence_table',
    ],
    'format_type' => 'tripalpane',
    'format_settings' => [
      'label' => 'Sequences',
      'instance_settings' => [
        'id' => 'tripal_ds-fieldset-group_sequence_tripalpane',
        'classes' => 'group-sequence-tripalpane field-group-tripalpane ',
        'description' => '',
        'hide' => 1,
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group_tripalpane);
  //Write to the tripal_ds table to record the new tripal pane.
  tripal_ds_bundle_menu_item($bundle_name, 'Sequence', 'group_sequence_tripalpane', 'TripalEntity');

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sequence_table|TripalEntity|' . $bundle_name . '|default';
  $field_group->group_name = 'group_sequence_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_sequence_tripalpane';
  $field_group->data = [
    'label' => 'Sequences Table',
    'weight' => '1',
    'children' => $fields,
    'format_type' => 'table',
    'format_settings' => [
      'label' => 'Sequences Table',
      'instance_settings' => [
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-sequence-table field-group-table',
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group);
}


/**
 * Processes all fields other than those that belong in the summary table,
 * property table, or data sequence table.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $field_label
 *  Human readable name of the field.
 * @param $group_field_name
 *  Machine name of the field.
 * @param $field_name
 *  Machine name of the child element.
 */
function tripal_ds_additional_fields_field_group_info($bundle_name, $field_label, $group_field_name, $field_name) {
  //Write to the tripal_ds table to record the new tripal pane.
  tripal_ds_bundle_menu_item($bundle_name, $field_label, $group_field_name, 'TripalEntity');

  //Tripal pane to nest the fieldset within.
  $field_group_fieldset = new stdClass();
  $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_fieldset->api_version = 1;
  $field_group_fieldset->identifier = $group_field_name . '|TripalEntity|' . $bundle_name . '|default';
  $field_group_fieldset->group_name = $group_field_name;
  $field_group_fieldset->entity_type = 'TripalEntity';
  $field_group_fieldset->bundle = $bundle_name;
  $field_group_fieldset->mode = 'default';
  $field_group_fieldset->parent_name = '';
  $field_group_fieldset->data = [
    'label' => $field_label,
    'weight' => '1',
    'children' => [
      0 => $field_name,
    ],
    'format_type' => 'tripalpane',
    'format_settings' => [
      'label' => $field_label,
      'instance_settings' => [
        'id' => 'tripal_ds-fieldset-' . $group_field_name,
        'classes' => '' . $group_field_name . ' field-group-tripalpane',
        'description' => '',
        'hide' => 1,
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group_fieldset);
}

/**
 * Creates the tripal panes for Publications..
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $fields
 *  Array of the machine names of the children of the field group being created.
 */
function _publication_prop_field_group_info($bundle_name, $fields) {
  //Tripal pane  to nest the fieldset within.
  $field_group_tripalpane = new stdClass();
  $field_group_tripalpane->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group_tripalpane->api_version = 1;
  $field_group_tripalpane->identifier = 'group_summary_tripalpane|TripalEntity|' . $bundle_name . '|default';
  $field_group_tripalpane->group_name = 'group_summary_tripalpane';
  $field_group_tripalpane->entity_type = 'TripalEntity';
  $field_group_tripalpane->bundle = $bundle_name;
  $field_group_tripalpane->mode = 'default';
  $field_group_tripalpane->parent_name = '';
  $field_group_tripalpane->data = [
    'label' => 'Summary',
    'weight' => '1',
    'children' => [
      0 => 'group_summary_table',
    ],
    'format_type' => 'tripalpane',
    'format_settings' => [
      'label' => 'Properties',
      'instance_settings' => [
        'id' => 'tripal_ds-fieldset-group_prop_tripalpane',
        'classes' => 'group-prop-tripalpane field-group-tripalpane',
        'description' => '',
        'hide' => 0,
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group_tripalpane);
  //Write to the tripal_ds table to record the new tripal pane.
  tripal_ds_bundle_menu_item($bundle_name, 'Summary', 'group_summary_tripalpane', 'TripalEntity');

  //Table of fields.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
  $field_group->api_version = 1;
  $field_group->identifier = 'group_summary_table|TripalEntity|' . $bundle_name . '|default';
  $field_group->group_name = 'group_summary_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = $bundle_name;
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_summary_tripalpane';
  $field_group->data = [
    'label' => 'Summary Table',
    'weight' => '1',
    'children' => [
      0 => 'tpub__doi',
      1 => 'tpub__publication_date',
      2 => 'schema__additional_type',
      3 => 'tpub__abstract',
      4 => 'tpub__citation',
    ],
    'format_type' => 'table',
    'format_settings' => [
      'label' => 'Summary Table',
      'instance_settings' => [
        'label_visibility' => '1',
        'desc' => '',
        'first_column' => '',
        'second_column' => '',
        'empty_label_behavior' => '1',
        'table_row_striping' => 0,
        'always_show_field_label' => 0,
        'classes' => 'group-prop-table field-group-table',
      ],
    ],
  ];
  drupal_write_record('field_group', $field_group);
}
