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

    // If collections are dispabled then don't show anything.
    $collections_enabled = variable_get('tripal_data_collections_enabled', 1);
    if (!$collections_enabled) {
      return '';
    }

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
  $form['save_collection']['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );
  $form['save_collection']['view'] = array(
    '#type' => 'value',
    '#value' => unserialize(serialize($view))
  );
  $form['save_collection']['query'] = array(
    '#type' => 'value',
    '#value' => unserialize(serialize($query->query))
  );
  $form['save_collection']['summary'] = array(
    '#type' => 'item',
    '#title' => 'Results Summary',
    '#markup' => t('There are @total_rows record(s) that can be added to a data collection.', array('@total_rows' => $view->total_rows)),
  );
  $form['save_collection']['collection_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection Name'),
    '#description' => t('Please name this collection for future reference.'),
    '#default_value' => $collection_name,
    '#required' => TRUE,
  );
  $form['save_collection']['description_fset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a Description'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['save_collection']['description_fset']['collection_desc'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please provide a description about this data collection. This is meant to help you remember what is in the collection.'),
    '#default_value' => $collection_name,
  );

  // Get the list of fields used in the view.
  $current_display = $view->current_display;
  if (array_key_exists('fields', $view->display[$current_display]->display_options)) {
    $view_fields = $view->display[$current_display]->display_options['fields'];
  }
  else {
    $view_fields = $view->display['default']->display_options['fields'];
  }

  $form['save_collection']['fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add or Update Fields'),
    '#description' => t('You may select any of the additional fields below to
      add to this data collection. Please note that different fields may be able
      to create different output file types.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // We want to theme all of the fields, so we add this next level in the
  // form array to theme.
  $form['save_collection']['fields']['items'] = array(
    '#theme' => 'tripal_views_handler_area_collections_fields_fset'
  );


  // Get the list of fields in this view.
  $field_ids = array();
  $defaults = array();
  $fields = field_info_instances('TripalEntity', $bundle->name);
  foreach ($fields as $field_name => $instance) {
    $field = field_info_field($field_name);
    $field_type = $field['type'];
    if ($instance['field_name'] == 'entity_id') {
      continue;
    }
    // Skip hidden fields.
    if ($instance['display']['default']['type'] == 'hidden') {
      continue;
    }
    $field_label = $instance['label'];

    // Add in in any non CSV or Tab formatters to the label.
    $formatters = array();
    $field_formatters = tripal_get_field_field_formatters($field, $instance);
    foreach ($field_formatters as $class_name => $label) {
      tripal_load_include_downloader_class($class_name);
      $formatters[] = $class_name::$label;
    }

    // Add the field to those supported.
    $field_ids[$instance['field_id']]= $field_label;


    // Automatically check fields that are in the view and not excluded.
    $checked = FALSE;
    if (array_key_exists($field_name, $view_fields)) {
      if (array_key_exists('exclude', $view_fields[$field_name]) and
          $view_fields[$field_name]['exclude'] == TRUE) {
        continue;
      }
      $checked = TRUE;
    }

    $form['save_collection']['fields']['items'] ['select-' . $instance['field_id']] = array(
      '#type' => 'checkbox',
      '#title' => $field_label,
      '#default_value' => $checked,
    );
    $form['save_collection']['fields']['items'] ['description-' . $instance['field_id']] = array(
      '#type' => 'markup',
      '#markup' => $instance['description']
    );
    $form['save_collection']['fields']['items'] ['formatters-' . $instance['field_id']] = array(
      '#type' => 'markup',
      '#markup' => join(', ', $formatters)
    );
  }

  $form['save_collection']['button'] = array(
    '#type' => 'submit',
    '#value' => 'Save Data Collection',
    '#name' => 'save_collection',
    '#ajax' => array(
      'callback' => "tripal_views_handler_area_collections_form_ajax",
      'wrapper' => 'tripal-views-handler-area-collections',
      'effect'   => 'fade',
      'method'   => 'replace',
      'prevent'  => 'click'
    ),
  );
  $form['#prefix'] = '<div id="tripal-views-handler-area-collections">';
  $form['#suffix'] = '</div>';
  return $form;
}
/**
 * Theme the fields section of the tripal_views_handler_area_collections form.
 *
 * @ingroup tripal_pub
 */
function theme_tripal_views_handler_area_collections_fields_fset($variables) {
  $form = $variables['form'];

  // Organize the elements by the same field id
  $fields = array();
  $order = array();
  $children =  element_children($form);
  foreach ($children as $key) {
    list($item, $field_id) = preg_split('/-/', $key);
    $fields[$field_id][$item] = $form[$key];
    if (!in_array($field_id, $order)) {
      $order[] = $field_id;
    }
  }

  // Next create a table with each field in a different row.
  $headers = array('Field', 'Description', 'Supported Files Types');
  $rows = array();
  foreach ($order as $field_id) {
    $rows[] = array(
      drupal_render($fields[$field_id]['select']),
      drupal_render($fields[$field_id]['description']),
      drupal_render($fields[$field_id]['formatters'])
    );
  }
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  return theme_table($table);
}

/**
 *
 */
function tripal_views_handler_area_collections_form_ajax($form, $form_state) {
  return $form;
}
/**
 *
 */
function tripal_views_handler_area_collections_form_submit($form, $form_state) {
  global $user;

  $bundle = $form_state['values']['bundle'];
  $view = $form_state['values']['view'];
  $query = $form_state['values']['query'];
  $collection_name = trim($form_state['values']['collection_name']);
  $description = $form_state['values']['collection_desc'];
  $field_ids = array_key_exists('field_ids', $form_state['values']) ? $form_state['values']['field_ids'] : array();
  $uid = $user->uid;
  $bundle_name = $bundle->name;

  // Get the fields that have been selected
  $selected_fids = array();
  foreach ($form_state['values'] as $key => $value) {
    $matches = array();
    if (preg_match('/select-(\d+)/', $key, $matches)) {
      if ($value == 1) {
        $selected_fids[] = $matches[1];
      }
    }
  }

  // Get the entity Ids that match results
  $query->range['length'] = $view->total_rows;
  $results = $query->execute();
  $entities = array();
  foreach ($results['TripalEntity'] as $entity) {
    $entities[] = $entity->id;
  }
  $collection = tripal_create_collection(array(
    'uid'  => $uid,
    'collection_name' => $collection_name,
    'description'  => $description,
    'bundle_name' => $bundle_name,
    'ids' => $entities,
    'fields' => $selected_fids,
  ));
}
