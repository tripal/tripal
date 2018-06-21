<?php
/**
 * @file
 * Integrates many of the core database tables with drupal views
 */

/**
 * Implements of hook_views_plugins().
 */
function tripal_views_plugins() {
  return array(
    'module' => 'tripal',
    'query' => array(
      'tripal_views_query' => array(
        'title' => t('Tripal Entity Query'),
        'help' => t('Query that allows you to search with Tripal entities.'),
        'handler' => 'tripal_views_query',
        'parent' => 'views_query',
      ),
    ),
  );
}

/**
 * Describe various Tripal Core systems to Views
 *
 * @ingroup tripal
 */
function tripal_views_data() {
  $data = array();
  // Job Management System
  tripal_views_data_jobs($data);
  tripal_views_data_tripal_entity($data);
  tripal_views_data_fields($data);

  $data['views']['tripal_area_collections'] = array(
    'title' => t('Tripal Content Data Collections'),
    'help' => t('Save Tripal content search results into a data collection for downloading or use with other tools.'),
    'area' => array(
      'handler' => 'tripal_views_handler_area_collections',
    ),
  );
  return $data;
}

/**
 * Implements hook views_data_alter()
 *
 * Ensures that all fields attached to TripalEntities use the proper
 * handlers.
 */
function tripal_views_data_alter(&$data) {

  // Iterate through all of the views data and find
  // those that are associated with fields attached to 
  // Tripal entities.  For known field types (e.g. Taxonomy) we
  // can support those.
  foreach ($data as $data_table => $definition) {
    foreach ($definition as $data_column => $element) {
      if (is_array($element) and array_key_exists('field', $element) and 
          is_array($element['field']) and array_key_exists('field_name', $element['field'])) {
        $field_name = $element['field']['field_name'];
        $field = field_info_field($field_name);

        // Skip fields that aren't attached to a TripalEntity content type.
        if (!array_key_exists('TripalEntity', $field['bundles'])) {
          continue;
        }

        // Skip fields that use the Tripal Storage API.
        if (array_key_exists('tripal_storage_api', $field['storage']['settings'])) {
          continue;
        }

        //  
        // Now update views for integrating other data with our Tripal Entities.
        //
    
        // Iterate through the bundles to which this field is attached and
        // if it is a TripalField field then we'll call the viewsData function.
        $bundles = $field['bundles']['TripalEntity'];
        $result = array();
        foreach ($bundles as $bundle_name) {

          // Skip fields that aren't setup for views with this bundle.
          // Fields should be associated with the bundle's term identifier
          // (i.e. [vocab]__[accession].
          $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
          $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
          $bundle_term_id = $term->vocab->vocabulary . '__' . $term->accession;
          if (!array_key_exists($bundle_term_id, $data)) {
            continue;
          }

          // Support the taxonomy_term_reference field when it's added to a 
          // Tripal content type
          if ($field['type'] == 'taxonomy_term_reference') { 
            $data[$bundle_term_id][$field_name] = [
              'title' => t('Tagged Categories'), 
              'help' => t('Relates this Tripal content type to categories that have been assigned to it using Drupal\'s Taxonomy system.'),
              'relationship' => array(
                 'base' => $data_table,
                 'base field' => 'entity_id',
                 'relationship field' => 'entity_id',
                 'handler' => 'views_handler_relationship',
                 'label' => t('Tags'),
              ),
            ];
          } 
        }
      } 
    }
  }
}
/**
 * Integreates the Tripal fields with Views.
 */
function tripal_views_data_fields(&$data) {

  // Iterate through the fields.
  $fields = field_info_fields();
  foreach ($fields as $field) {

    // Skip fields that aren't attached to TripalEntity entities.
    if (!array_key_exists('TripalEntity', $field['bundles'])) {
      continue;
    }

    // Fields that don't connect to the Tripal Storage API should be added differently
    if (!array_key_exists('tripal_storage_api', $field['storage']['settings'])) {
      continue;
    }

    // TODO: do we really need this hook? Just substitute the code here
    // for what that hook does... it's only called in one plac.e
    // Call the hook_field_views_data() but only for the tripal module.
    // Otherwise the other modules will expect that this is an SQL-based
    // view.
    $result = (array) module_invoke('tripal', 'field_views_data', $field);

    // Set defaults for the field if no data array was returned.
    if (empty($result)) {
      // Iterate through the bundles to which this field is attached and
      // if it is a TripalField field then we'll call the viewsData function.
      $bundles = $field['bundles']['TripalEntity'];
      $result = array();
      foreach ($bundles as $bundle_name) {
        $field_name = $field['field_name'];

        // Fields should be associated with the bundle's term identifier
        // (i.e. [vocab]__[accession].
        $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
        $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
        $view_base_id = $term->vocab->vocabulary . '__' . $term->accession;

        $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
        $tfield = new TripalField($field, $instance);
        $result += $tfield->viewsData($view_base_id);
      }
    }

    drupal_alter('field_views_data', $result, $field, $module);

    if (is_array($result)) {
      $data = drupal_array_merge_deep($result, $data);
    }
  }
}
/**
 * Integrates the TripalEntity entities with Drupal Views.
 */
function tripal_views_data_tripal_entity(&$data) {

  // Get the list of all of the bundles (entity types) and add them
  // as "base tables" for views.
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->execute();

  // Iterate through the bundles.
  while ($bundle = $bundles->fetchObject()) {

    // This isn't really the table name, but because our bundle table
    // names are unique on every Tripal site we must ust a more generic
    // name.  Because we're using our own query class this should be fine.
    $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
    $table = $term->vocab->vocabulary . '__' . $term->accession;

    // Each bundle gets it's own "table".
    $data[$table]['table']['group'] = t($bundle->label);
    $data[$table]['table']['base'] = array(
      'query class' => 'tripal_views_query',
      'title' => t($bundle->label),
      'help' => t('Tripal ' . $bundle->label . ' pages'),
    );
    $data[$table]['entity_id'] = array(
      'title' => t('Entity ID'),
      'help' => t('The unique entity ID for this content type.'),
      'field' => array(
        'handler' => 'tripal_views_handler_field_entity',
      ),
      'filter' => array(
        'handler' => 'tripal_views_handler_filter',
      ),
      'sort' => array(
        'handler' => 'tripal_views_handler_sort',
      ),
    );
    $data[$table]['link'] = array(
      'title' => t('Link'),
      'help' => t('Provide a simple link to the content.'),
      'field' => array(
        'handler' => 'tripal_views_handler_field_entity_link',
      ),
    );
    $data[$table]['edit_link'] = array(
      'title' => t('Edit Link'),
      'help' => t('Provide a simple link to edit the content.'),
      'field' => array(
        'handler' => 'tripal_views_handler_field_entity_link_edit',
      ),
    );
    $data[$table]['delete_link'] = array(
      'title' => t('Delete Link'),
      'help' => t('Provide a simple link to delete the content.'),
      'field' => array(
        'handler' => 'tripal_views_handler_field_entity_link_delete',
      ),
    );
    $data[$table]['status'] = array(
      'title' => t('Published'),
      'help' => t('Whether or not the content is published.'),
      'field' => array(
        'handler' => 'tripal_views_handler_field_boolean',
        'click sortable' => TRUE,
        'output formats' => array(
          'published-notpublished' => array(t('Published'), t('Not published')),
        ),
      ),
      'filter' => array(
        'handler' => 'tripal_views_handler_filter_boolean_operator',
        'label' => t('Published'),
        'type' => 'yes-no',
        'use equal' => TRUE, // Use status = 1 instead of status <> 0 in WHERE statment
      ),
      'sort' => array(
        'handler' => 'tripal_views_handler_sort',
      ),
    );
  }
}

/**
 * Provides the data array for the tripal job management system
 *
 * @param $data
 *   Previously generated tripal views data array
 * return
 *   $data array with job management system described
 *
 * @ingroup tripal
 */
function tripal_views_data_jobs(&$data) {

  $data['tripal_jobs']['table']['group'] = t('Tripal Jobs');
  $data['tripal_jobs']['table']['base'] = array(
    'field' => 'job_id', // This is the identifier field for the view.
    'title' => t('Tripal Jobs'),
    'help' => t('The Job Management system for Tripal.'),
    'weight' => 10,
  );

  // Job ID
  $data['tripal_jobs']['job_id'] = array(
    'title' => t('Job ID'),
    'help' => t('The job primary key.'),
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

  // User ID: Submitter
  $data['tripal_jobs']['uid'] = array(
    'title' => t('Job Submitter'),
    'help' => t('The user who submitted the job.'),
    'relationship' => array(
      'base' => 'user', // The name of the table to join with.
      'base field' => 'uid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Submitting User'),
      'title' => t('Submitting User'),
      'help' => t('The user who submitted the job'),
    ),
  );

  // Job Name
  $data['tripal_jobs']['job_name'] = array(
    'title' => t('Job Name'),
    'help' => t('The name of the job.'),
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

  // Module Name
  $data['tripal_jobs']['modulename'] = array(
    'title' => t('Module Name'),
    'help' => t('The name of the module that submitted the job.'),
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

  // Callback
  $data['tripal_jobs']['callback'] = array(
    'title' => t('Callback'),
    'help' => t('The callback executed when the job runs.'),
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

  // Arguments
  $data['tripal_jobs']['arguments'] = array(
    'title' => t('Arguements'),
    'help' => t('Any arguments passed to the callback.'),
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

  // Progress
  $data['tripal_jobs']['progress'] = array(
    'title' => t('Progress'),
    'help' => t('The current progress of the job.'),
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

  // Status
  $data['tripal_jobs']['status'] = array(
    'title' => t('Status'),
    'help' => t('The current status of the job.'),
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

  // Submit Data
  $data['tripal_jobs']['submit_date'] = array(
    'title' => t('Submit Date'),
    'help' => t('The date the job was submitted.'),
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

  // Start Time
  $data['tripal_jobs']['start_time'] = array(
    'title' => t('Start Time'),
    'help' => t('The time the job started.'),
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

  // End Time
  $data['tripal_jobs']['end_time'] = array(
    'title' => t('End Time'),
    'help' => t('The time the job ended.'),
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

  // Error Message
  $data['tripal_jobs']['error_msg'] = array(
    'title' => t('Error Message '),
    'help' => t('A short description of any error the job might have had.'),
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

  // Unix Pid of the job
  $data['tripal_jobs']['pid'] = array(
    'title' => t('Job PID'),
    'help' => t('The Unix PID of the job.'),
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

  // Priority
  $data['tripal_jobs']['priority'] = array(
    'title' => t('Priority'),
    'help' => t('The priority of this job.'),
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
}
