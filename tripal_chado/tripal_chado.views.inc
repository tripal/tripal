<?php
/**
 * @file
 * Integrates many of the core database tables with drupal views
 */

/**
 * Describe various Tripal Core systems to Views
 *   for the creation of administrative views.
 *
 * @ingroup tripal
 */
function tripal_chado_views_data() {
  $data = array();

  // Custom Tables Management
  $data = tripal_chado_views_data_custom_tables($data);

  // Materialized Views Management
  $data = tripal_chado_views_data_mviews($data);

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function tripal_chado_views_data_alter(&$data) {

  // Adds integration for chado-based fields.
  tripal_chado_add_field_views_data($data);

  return $data;
}

/** 
 * Adds integration for chado-based fields.
 *
 * We can't use hook_field_view_data since this only works when the
 * storage engine is of type 'field_sql_storage' and of course,
 * ours is not. Thus we create our own implementation of field_views_data()
 * for our storage engine.
 */
function tripal_chado_add_field_views_data(&$data) {

  foreach (field_info_fields() as $field) {
    if ($field['storage']['type'] != 'field_chado_storage') {
      continue;
    }

    // Currently, we only handle integration of chado fields with TripalEntity.
    // @todo: extend this to work with other entities in the future.
    if (isset($field['bundles']['TripalEntity']) AND isset($field['settings']['chado_column'])) {

      // We currently don't support prop tables for views integration due
      // in part to the multiple values but also b/c we can't indicate which
      // type of property to show. Thus, instead of warning the user,
      // we just won't integrate it at this time.
      // @todo: Handle property fields.
      if (preg_match('/prop$/', $field['settings']['chado_table'])) {
        continue;
      }
      
      // Get some information about the chado table in order to make good
      // choices for handlers.
      $table_desc = chado_get_schema($field['settings']['chado_table']);
      $field_defn = $table_desc['fields'][ $field['settings']['chado_column'] ];
      
      // We also need to know if this field is a foreign key.
      $fk_defn = FALSE;
      foreach ($table_desc['foreign keys'] as $details) {
        foreach ($details['columns'] as $left_field => $right_field) {
          if ($left_field == $field['settings']['chado_column']) {
            $fk_defn = array(
              'left_table' => $field['settings']['chado_table'],
              'left_field' => $left_field,
              'right_table' => $details['table'],
              'right_field' => $right_field,
            );
          }
        }
      }
      

      // Unfortunatly we can't use the field label since that is set at the 
      // instance level and fields are integrated at the field level (independant of bundle).
      // Thus we will simply make the most readable and informative field name we can.
      $data['tripal_entity'][ $field['field_name'] ]['title'] = ucfirst(str_replace('_',' ',$field['settings']['chado_table']))
        . ' ' .ucfirst(str_replace('_',' ',$field['settings']['chado_column']));

      // The help should be 'Appears in: TripalEntity: gene, organism'
      // so that users know where they can use it. This requires a little extra work since all
      // we have access to at this point is bio-data_2, bio-data_4 but since that's not very
      // informative, extra work is worth it ;-).
      $entity_info = entity_get_info('TripalEntity');
      $bundle_labels = array();
      foreach ($field['bundles']['TripalEntity'] as $bundle_id) {
        $bundle_labels[] = $entity_info['bundles'][$bundle_id]['label'];
      }
      $data['tripal_entity'][ $field['field_name'] ]['help'] = 'Appears in: TripalEntity:' . implode(', ', $bundle_labels);

      // Define the field.
      $data['tripal_entity'][ $field['field_name'] ]['field']['chado_field'] = $field['settings']['chado_column'];
      $data['tripal_entity'][ $field['field_name'] ]['field']['chado_table'] = $field['settings']['chado_table'];
      $data['tripal_entity'][ $field['field_name'] ]['field']['field_name'] = $field['field_name'];
      $data['tripal_entity'][ $field['field_name'] ]['field']['entity_table'] = 'tripal_entity';
      $data['tripal_entity'][ $field['field_name'] ]['field']['entity_type'] = 'TripalEntity';
      $data['tripal_entity'][ $field['field_name'] ]['field']['bundles'] = $field['bundles']['TripalEntity'];
      $data['tripal_entity'][ $field['field_name'] ]['field']['handler'] = 'chado_views_handler_field';
      $data['tripal_entity'][ $field['field_name'] ]['field']['click sortable'] = FALSE;

      // Define the Filter.
      $data['tripal_entity'][ $field['field_name'] ]['filter']['chado_field'] = $field['settings']['chado_column'];
      $data['tripal_entity'][ $field['field_name'] ]['filter']['chado_table'] = $field['settings']['chado_table'];
      $data['tripal_entity'][ $field['field_name'] ]['filter']['field_name'] = $field['field_name'];
      $data['tripal_entity'][ $field['field_name'] ]['filter']['entity_table'] = 'tripal_entity';
      $data['tripal_entity'][ $field['field_name'] ]['filter']['entity_type'] = 'TripalEntity';
      $data['tripal_entity'][ $field['field_name'] ]['filter']['bundles'] = $field['bundles']['TripalEntity'];
      $data['tripal_entity'][ $field['field_name'] ]['filter']['handler'] = 'chado_views_handler_filter_string';
      
      // If this is a foreign key field like organism then we want a drop-down.
      // This requires a special handler.
      if ($fk_defn) {
        $data['tripal_entity'][ $field['field_name'] ]['filter']['handler'] = 'chado_views_handler_filter_fk';
        $data['tripal_entity'][ $field['field_name'] ]['filter']['foreign_key'] = $fk_defn;
      }
      if ($field_defn['type'] == 'boolean') {
        $data['tripal_entity'][ $field['field_name'] ]['filter']['handler'] = 'chado_views_handler_filter_boolean';
        $data['tripal_entity'][ $field['field_name'] ]['filter']['label'] = $field['settings']['chado_column'];
        $data['tripal_entity'][ $field['field_name'] ]['filter']['type'] = 'yes-no'; 
      }
    }    
  }
}

/**
 * Provides the data array for the tripal custom tables management
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with custom tables management described
 *
 * @ingroup tripal
 */
function tripal_chado_views_data_custom_tables($data) {

  $data['tripal_custom_tables']['table']['group'] = t('Tripal Custom Tables');
  $data['tripal_custom_tables']['table']['base'] = array(
    'field' => 'table_id', // This is the identifier field for the view.
    'title' => t('Tripal Custom Tables'),
    'help' => t('Custom Tables in Chado created by this Tripal Installation.'),
    'weight' => 10,
  );

  // Table ID
  $data['tripal_custom_tables']['table_id'] = array(
    'title' => t('Custom Table ID'),
    'help' => t('Custom table primary key.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Table Name
  $data['tripal_custom_tables']['table_name'] = array(
    'title' => t('Table Name'),
    'help' => t('The name of the table in the database.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Schema
  $data['tripal_custom_tables']['schema'] = array(
    'title' => t('Table Schema'),
    'help' => t('The schema definition of the table.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Table ID
  $data['tripal_custom_tables']['mview_id'] = array(
    'title' => t('Materialized View ID'),
    'help' => t('Foreign key to tripal_mviews table for the materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );


  return $data;
}

/**
 * Provides the data array for the tripal custom tables management
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with custom tables management described
 *
 * @ingroup tripal
 */
function tripal_chado_views_data_mviews($data) {

  $data['tripal_mviews']['table']['group'] = t('Tripal Materialized Views');
  $data['tripal_mviews']['table']['base'] = array(
    'field' => 'mview_id', // This is the identifier field for the view.
    'title' => t('Tripal Materialized Views'),
    'help' => t('Materialized Views in Chado created by this Tripal Installation.'),
    'weight' => 10,
  );

    // Implicit Join to Tripal Views
  $data['tripal_mviews']['table']['join'] = array(
    'tripal_views' => array(
      'left_field' => 'mview_id',
      'field' => 'mview_id',
    ),
  );

  // Mview ID
  $data['tripal_mviews']['mview_id'] = array(
    'title' => t('Materialized View ID'),
    'help' => t('The primary key.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // name
  $data['tripal_mviews']['name'] = array(
    'title' => t('Name'),
    'help' => t('Human-readable name of the materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // modulename
  $data['tripal_mviews']['modulename'] = array(
    'title' => t('Module Name'),
    'help' => t('The module that created the materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // mv_table
  $data['tripal_mviews']['mv_table'] = array(
    'title' => t('Table'),
    'help' => t('The database table the materialized view is stored in.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // mv_specs
  $data['tripal_mviews']['mv_specs'] = array(
    'title' => t('Specification'),
    'help' => t('Materialized View Specification.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // mv_schema
  $data['tripal_mviews']['mv_schema'] = array(
    'title' => t('Schema'),
    'help' => t('Schema definition for the materialized view table.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // indexed
  $data['tripal_mviews']['indexed'] = array(
    'title' => t('Indices'),
    'help' => t('Any indices for this materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // query
  $data['tripal_mviews']['query'] = array(
    'title' => t('Query'),
    'help' => t('The query used to populate the materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // special_index
  $data['tripal_mviews']['special_index'] = array(
    'title' => t('Special Index'),
    'help' => t('Any special indices for the materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // last_update
  $data['tripal_mviews']['last_update'] = array(
    'title' => t('Updated'),
    'help' => t('Date Last Updated.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // status
  $data['tripal_mviews']['status'] = array(
    'title' => t('Status'),
    'help' => t('The status of the materialized view.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // comment
  $data['tripal_mviews']['comment'] = array(
    'title' => t('Description'),
    'help' => t('Human-Readable Admin Description.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}
