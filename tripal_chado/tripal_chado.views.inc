<?php

/**
 * Describe various Tripal Core systems to Views
 *
 * @ingroup tripal
 */
function tripal_chado_views_data() {
  $data = [];

  // Custom Tables.
  tripal_chado_views_data_custom_tables($data);
  return $data;
}


/**
 * Adds the Tripal Custom Tables View
 * 
 * @param array $data
 *   A data array for creating a view.
 *   
 */
function tripal_chado_views_data_custom_tables(&$data) {
  $data['tripal_custom_tables'] = [];
  $data['tripal_custom_tables']['table'] = [];
  $data['tripal_custom_tables']['table']['group'] = t('Tripal Custom Tables');
  $data['tripal_custom_tables']['table']['provider'] = 'tripal';
  $data['tripal_custom_tables']['table']['base'] = [
      'field' => 'table_id',
      'title' => t('Tripal Custom Tables'),
      'help' => t('The Custom Table Management system for Tripal.'),
      'weight' => 10,
  ];
    
  // Table ID
  $data['tripal_custom_tables']['table_id'] = [
    'title' => t('Table ID'),
    'help' => t('The table key.'),
    'field' => [
      'id' => 'numeric',
    ],
    'filter' => [
      'id' => 'numeric',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'argument' => [
      'id' => 'numeric',
    ],
  ];
  
  // Table Name
  $data['tripal_custom_tables']['table_name'] = [
    'title' => t('Table Name'),
    'help' => t('The name of the table.'),
    'field' => [
      'id' => 'standard',
    ],
    'sort' => [
      'id' => 'standard'
    ],
    'filter' => [
      'id' => 'string',
    ],
    'argument' => [
      'id' => 'string',
    ],
  ]; 

  $data['tripal_custom_tables']['edit_link'] = array(
    'title' => t('Edit'),
    'help' => t('Clickable link to edit a custom table'),
    'field' => [
      'id' => 'chado_custom_tables_edit_link'
    ],   
  );  

  $data['tripal_custom_tables']['delete_link'] = array(
    'title' => t('Delete'),
    'help' => t('Clickable link to delete a custom table'),
    'field' => [
      'id' => 'chado_custom_tables_delete_link'
    ],   
  );
}
