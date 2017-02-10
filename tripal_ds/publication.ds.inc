<?php
/**
 * @file
 * publication.ds.inc
 */

/**
 * Implements hook_ds_layout_settings_info().
 */
function publication_ds_layout_settings_info() {
  $export = array();

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'TripalEntity|bio_data_45|default';
  $ds_layout->entity_type = 'TripalEntity';
  $ds_layout->bundle = 'bio_data_45';
  $ds_layout->view_mode = 'default';
  $ds_layout->layout = 'ds_2col';
  $ds_layout->settings = array(
    'regions' => array(
      'left' => array(
        0 => 'tpub__abstract',
      ),
      'right' => array(
        1 => 'group_citation',
        2 => 'tpub__citation',
        3 => 'group_properties',
        4 => 'group_references',
        5 => 'sio__references',
        6 => 'group_property_table',
        7 => 'tpub__publication_type',
        8 => 'tpub__doi',
        9 => 'sbo__database_cross_reference',
      ),
    ),
    'fields' => array(
      'tpub__abstract' => 'left',
      'group_citation' => 'right',
      'tpub__citation' => 'right',
      'group_properties' => 'right',
      'group_references' => 'right',
      'sio__references' => 'right',
      'group_property_table' => 'right',
      'tpub__publication_type' => 'right',
      'tpub__doi' => 'right',
      'sbo__database_cross_reference' => 'right',
    ),
    'classes' => array(),
    'wrappers' => array(
      'left' => 'div',
      'right' => 'div',
    ),
    'layout_wrapper' => 'div',
    'layout_attributes' => '',
    'layout_attributes_merge' => 1,
    'layout_link_attribute' => '',
    'layout_link_custom' => '',
    'layout_disable_css' => 0,
  );
  $export['TripalEntity|bio_data_45|default'] = $ds_layout;

  return $export;
}
