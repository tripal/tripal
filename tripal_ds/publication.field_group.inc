<?php
/**
 * @file
 * publication.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function publication_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_citation|TripalEntity|bio_data_45|default';
  $field_group->group_name = 'group_citation';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = 'bio_data_45';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Citation',
    'weight' => '1',
    'children' => array(
      0 => 'tpub__citation',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Citation',
      'instance_settings' => array(
        'id' => '',
        'classes' => 'group-citation field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups[''] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_properties|TripalEntity|bio_data_45|default';
  $field_group->group_name = 'group_properties';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = 'bio_data_45';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Properties',
    'weight' => '2',
    'children' => array(
      0 => 'group_property_table',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Properties',
      'instance_settings' => array(
        'id' => '',
        'classes' => 'group-properties field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups[''] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_property_table|TripalEntity|bio_data_45|default';
  $field_group->group_name = 'group_property_table';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = 'bio_data_45';
  $field_group->mode = 'default';
  $field_group->parent_name = 'group_properties';
  $field_group->data = array(
    'label' => '',
    'weight' => '30',
    'children' => array(
      0 => 'sbo__database_cross_reference',
      1 => 'tpub__doi',
      2 => 'tpub__publication_type',
    ),
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
        'classes' => 'group-property-table field-group-table',
      ),
    ),
  );
  $field_groups[''] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_references|TripalEntity|bio_data_45|default';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'TripalEntity';
  $field_group->bundle = 'bio_data_45';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '3',
    'children' => array(
      0 => 'sio__references',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'References',
      'instance_settings' => array(
        'id' => '',
        'classes' => 'group-references field-group-fieldset',
        'description' => 'The following data housed by this site is referred  to by this publication.',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups[''] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Citation');
  t('Properties');
  t('References');

  return $field_groups;
}
