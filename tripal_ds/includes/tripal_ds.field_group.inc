<?php
/**
 * Implements hook_field_group_info().
 */

function tripal_ds_overview_field_group_info($bundle_name, $fields){

    //fieldset field to nest the table within
    $field_group_fieldset = new stdClass();
    $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
    $field_group_fieldset->api_version = 1;
    $field_group_fieldset->identifier = 'group_overview|TripalEntity|'.$bundle_name.'|default';
    $field_group_fieldset->group_name = 'group_overview';
    $field_group_fieldset->entity_type = 'TripalEntity';
    $field_group_fieldset->bundle = $bundle_name;
    $field_group_fieldset->mode = 'default';
    $field_group_fieldset->parent_name = '';
    $field_group_fieldset->data = array(
        'label' => 'Overview',
        'weight' => '2',
        'children' => array(
            0 => 'group_overview_table',
        ),
        'format_type' => 'fieldset',
        'format_settings' => array(
            'label' => 'Overview',
            'instance_settings' => array(
                'id' => '',
                'classes' => 'group-overview field-group-fieldset',
                'description' => '',
                'effect' => 'bounceslide',
            ),
            'formatter' => 'open',
        ),
    );

    drupal_write_record('field_group', $field_group_fieldset);

    //table of fields
    $field_group = new stdClass();
    $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
    $field_group->api_version = 1;
    $field_group->identifier = 'group_overview_table|TripalEntity|'.$bundle_name.'|default';
    $field_group->group_name = 'group_overview_table';
    $field_group->entity_type = 'TripalEntity';
    $field_group->bundle = $bundle_name;
    $field_group->mode = 'default';
    $field_group->parent_name = 'group_overview';
    $field_group->data = array(
        'label' => '',
        'weight' => '30',
        'children' => $fields,
        'format_type' => 'table',
        'format_settings' => array(
            'label' => '',
            'instance_settings' => array(
                'label_visibility' => '3',
                'desc' => '',
                'first_column' => '',
                'second_column' => '',
                'empty_label_behavior' => '1',
                'table_row_striping' => 0,
                'always_show_field_label' => 0,
                'classes' => 'group-overview-table field-group-table table table-striped',
            ),
        ),
    );
    watchdog('debug', '<pre>tripal_ds_overview_field_group_info  $field_group: '. print_r($field_group, TRUE) .'</pre>');

    drupal_write_record('field_group', $field_group);
}
/**
 * Implements hook_field_group_info().
 */

function tripal_ds_prop_field_group_info($bundle_name, $fields){

    //fieldset field to nest the table within
    $field_group_fieldset = new stdClass();
    $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
    $field_group_fieldset->api_version = 1;
    $field_group_fieldset->identifier = 'group_prop|TripalEntity|'.$bundle_name.'|default';
    $field_group_fieldset->group_name = 'group_prop';
    $field_group_fieldset->entity_type = 'TripalEntity';
    $field_group_fieldset->bundle = $bundle_name;
    $field_group_fieldset->mode = 'default';
    $field_group_fieldset->parent_name = '';
    $field_group_fieldset->data = array(
        'label' => 'Prop',
        'weight' => '3',
        'children' => array(
            0 => 'group_prop_table',
        ),
        'format_type' => 'fieldset',
        'format_settings' => array(
            'label' => 'Prop',
            'instance_settings' => array(
                'id' => '',
                'classes' => 'group-prop field-group-fieldset',
                'description' => '',
                'effect' => 'bounceslide',
            ),
            'formatter' => 'open',
        ),
    );

    drupal_write_record('field_group', $field_group_fieldset);

    //table of fields
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
        'label' => '',
        'weight' => '30',
        'children' => $fields,
        'format_type' => 'table',
        'format_settings' => array(
            'label' => '',
            'instance_settings' => array(
                'label_visibility' => '3',
                'desc' => '',
                'first_column' => '',
                'second_column' => '',
                'empty_label_behavior' => '1',
                'table_row_striping' => 0,
                'always_show_field_label' => 0,
                'classes' => 'group-prop-table field-group-table table table-striped',
            ),
        ),
    );
    watchdog('debug', '<pre>tripal_ds_prop_field_group_info  $field_group: '. print_r($field_group, TRUE) .'</pre>');

    drupal_write_record('field_group', $field_group);
}
/**
 * Implements hook_field_group_info().
 */

function tripal_ds_data_sequence_field_group_info($bundle_name, $fields){

    //fieldset field to nest the table within
    $field_group_fieldset = new stdClass();
    $field_group_fieldset->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially*/
    $field_group_fieldset->api_version = 1;
    $field_group_fieldset->identifier = 'group_sequence|TripalEntity|'.$bundle_name.'|default';
    $field_group_fieldset->group_name = 'group_sequence';
    $field_group_fieldset->entity_type = 'TripalEntity';
    $field_group_fieldset->bundle = $bundle_name;
    $field_group_fieldset->mode = 'default';
    $field_group_fieldset->parent_name = '';
    $field_group_fieldset->data = array(
        'label' => 'Data Sequences',
        'weight' => '2',
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
                'effect' => 'bounceslide',
            ),
            'formatter' => 'open',
        ),
    );

    drupal_write_record('field_group', $field_group_fieldset);

    //table of fields
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
        'label' => '',
        'weight' => '30',
        'children' => $fields,
        'format_type' => 'table',
        'format_settings' => array(
            'label' => '',
            'instance_settings' => array(
                'label_visibility' => '3',
                'desc' => '',
                'first_column' => '',
                'second_column' => '',
                'empty_label_behavior' => '1',
                'table_row_striping' => 0,
                'always_show_field_label' => 0,
                'classes' => 'group-sequence-table field-group-table table table-striped',
            ),
        ),
    );
    watchdog('debug', '<pre>tripal_ds_sequence_field_group_info  $field_group: '. print_r($field_group, TRUE) .'</pre>');

    drupal_write_record('field_group', $field_group);
}
