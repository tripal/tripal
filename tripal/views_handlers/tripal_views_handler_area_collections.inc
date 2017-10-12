<?php

class tripal_views_handler_area_collections extends views_handler_area_result {

  function options_form(&$form, &$form_state) {
    // We have no options so we have to implement this function with
    // nothing in it.
  }
  /**
   * Implements views_handler_area_result::render().
   */
  function render($empty = FALSE) {

    // This will only work with Tripal content types and the tripal_views_query
    // plugin. So don't show anything for others.
    if ($this->query->plugin_name != 'tripal_views_query') {
      return '';
    }
    $form = drupal_get_form('tripal_views_handler_area_collections_form', $this->view, $this->query);
    return drupal_render($form);
  }
}

/**
 *
 */
function tripal_views_handler_area_collections_form($form, $form_state, $view, $query) {

  // Set form defaults.
  $collection_name = '';
  $collection_desc = '';

  // Get the bundle for this query.
  $matches = array();
  preg_match('/^(.+?)__(.+?)$/', $view->base_table, $matches);
  $vocabulary = $matches[1];
  $accession = $matches[2];
  $term = tripal_load_term_entity(array('vocabulary' => $vocabulary, 'accession' => $accession));
  $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));

  $form = array();
  $form['save_collection'] = array(
   '#type' => 'fieldset',
   '#title' => t('Save Results'),
   '#collapsible' => TRUE,
   '#collapsed' => TRUE,
   '#description' => t('A data collection is a virtual container into which you can
     save data.  You can place your search results into a data collection for
     download or use with other tools on this site that support data collections.'),
  );
  $form['save_collection']['bundle_name'] = array(
    '#type' => 'value',
    '#value' => $bundle->name,
  );
  $form['save_collection']['summary'] = array(
   '#type' => 'item',
   '#title' => 'Results Summary',
   '#markup' => t('There are @total_rows record(s) that can be added to a container.', array('@total_rows' => $view->total_rows)),
  );
  $form['save_collection']['collection_name'] = array(
   '#type' => 'textfield',
   '#title' => t('Collection Name'),
   '#description' => t('Please name this collection for future reference.'),
   '#default_value' => $collection_name,
   '#required' => TRUE,
  );
  $form['save_collection']['collection_desc'] = array(
   '#type' => 'textarea',
   '#title' => t('Description'),
   '#description' => t('Please provide a description about this data collection.'),
   '#default_value' => $collection_name,
  );

  // Get the list of fields in this view.
  $fields = $view->field;
  $field_ids = array();
  foreach ($fields as $field_name => $handler) {
    if ($field_name == 'entity_id') {
      continue;
    }
    $field = field_info_field($field_name);
    $instance = field_info_instance('TripalEntity', $field_name, $bundle->name);
    $field_ids[$field['id']] = $instance['label'];
  }
  $form['save_collection']['field_ids'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Field Selection'),
    '#description' => t('Please select the fields to include in this data collection. If you do not select any fields then all fields will be included.'),
    '#options' => $field_ids,
  );

  $form['save_collection']['button'] = array(
    '#type' => 'submit',
    '#value' => 'Save Data Collection',
    '#name' => 'save_collection'
  );

  return $form;
}


/**
 *
 */
function tripal_views_handler_area_collections_form_submit($form, $form_state) {
  global $user;

  $bundle_name = $form_state['values']['bundle_name'];
  $collection_name = $form_state['values']['collection_name'];
  $description = $form_state['values']['collection_desc'];
  $field_ids = $form_state['values']['field_ids'];
  $uid = $user->uid;

  $entities = array();

  tripal_create_collection(array(
    'uid'  => $uid,
    'collection_name' => $collection_name,
    'bundle_name' => $bundle_name,
    'ids' => $entities,
    'fields' => $field_ids,
    'description'  => $description,
  ));
}