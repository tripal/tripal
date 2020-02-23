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

    // If collections are disabled then don't show anything.
    $collections_enabled = variable_get('tripal_data_collections_enabled', 1);
    if (!$collections_enabled) {
      return '';
    }

    if (user_is_anonymous()) {
      return '';
    }

    // We need a specific form to work with Tripal content types and the tripal_views_query plugin.
    if ($this->query->plugin_name == 'tripal_views_query') {
      $form = drupal_get_form('tripal_views_handler_area_collections_form', $this->view, $this->query);
      return drupal_render($form);
    }
    // We also want to support views created via the Drupal Search API.
    elseif ($this->query->plugin_name == 'search_api_views_query') {
      $form = drupal_get_form('tripal_views_handler_area_collections_search_api_form', $this->view, $this->query);
      return drupal_render($form);
    }
    // If we don't support this type of view, then we should tell the admin
    // so they're not left scratching their head like I was ;-).
    else {
      tripal_set_message('Unfortunatly Tripal Collections are not supported for your current view type.');
    }
  }
}

/**
 * Views Area Handler Form: Tripal Collections on Tripal Entity-based Views
 *
 * @param $form
 * @param $form_state
 * @param $view
 *   The views object that this form will be rendered on.
 * @param $query
 *   The views query object generating the views results.
 */
function tripal_views_handler_area_collections_form($form, $form_state, $view, $query) {

  // Set form defaults.
  $collection_name = '';
  $collection_desc = '';

  // Get the bundle for this query.
  $matches = [];
  preg_match('/^(.+?)__(.+?)$/', $view->base_table, $matches);
  $vocabulary = $matches[1];
  $accession = $matches[2];
  $term = tripal_load_term_entity([
    'vocabulary' => $vocabulary,
    'accession' => $accession,
  ]);
  $bundle = tripal_load_bundle_entity(['term_id' => $term->id]);

  $form = [];
  $form['save_collection'] = [
    '#type' => 'fieldset',
    '#title' => t('Save Results'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('A data collection is a virtual container into which you can
      save data.  You can place your search results into a data collection for
      download or use with other tools on this site that support data collections.'),
  ];
  $form['save_collection']['bundle'] = [
    '#type' => 'value',
    '#value' => $bundle,
  ];
  $form['save_collection']['view'] = [
    '#type' => 'value',
    '#value' => unserialize(serialize($view)),
  ];
  $form['save_collection']['query'] = [
    '#type' => 'value',
    '#value' => unserialize(serialize($query->query)),
  ];
  $form['save_collection']['summary'] = [
    '#type' => 'item',
    '#title' => 'Results Summary',
    '#markup' => t('There are @total_rows record(s) that can be added to a data collection.', ['@total_rows' => $view->total_rows]),
  ];
  $form['save_collection']['collection_name'] = [
    '#type' => 'textfield',
    '#title' => t('Collection Name'),
    '#description' => t('Please name this collection for future reference.'),
    '#default_value' => $collection_name,
    '#required' => TRUE,
  ];
  $form['save_collection']['description_fset'] = [
    '#type' => 'fieldset',
    '#title' => t('Add a Description'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
  $form['save_collection']['description_fset']['collection_desc'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please provide a description about this data collection. This is meant to help you remember what is in the collection.'),
    '#default_value' => $collection_name,
  ];

  // Get the list of fields used in the view.
  $current_display = $view->current_display;
  if (array_key_exists('fields', $view->display[$current_display]->display_options)) {
    $view_fields = $view->display[$current_display]->display_options['fields'];
  }
  else {
    $view_fields = $view->display['default']->display_options['fields'];
  }

  $form['save_collection']['fields'] = [
    '#type' => 'fieldset',
    '#title' => t('Add or Update Fields'),
    '#description' => t('You may select any of the additional fields below to
      add to this data collection. Please note that different fields may be able
      to create different output file types.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  // We want to theme all of the fields, so we add this next level in the
  // form array to theme.
  $form['save_collection']['fields']['items'] = [
    '#theme' => 'tripal_views_handler_area_collections_fields_fset',
  ];


  // Get the list of fields in this view.
  $field_ids = [];
  $defaults = [];
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
    $formatters = [];
    $field_formatters = tripal_get_field_field_formatters($field, $instance);
    foreach ($field_formatters as $class_name => $label) {
      tripal_load_include_downloader_class($class_name);
      $formatters[] = $class_name::$label;
    }

    // Add the field to those supported.
    $field_ids[$instance['field_id']] = $field_label;


    // Automatically check fields that are in the view and not excluded.
    $checked = FALSE;
    if (array_key_exists($field_name, $view_fields)) {
      if (array_key_exists('exclude', $view_fields[$field_name]) and
        $view_fields[$field_name]['exclude'] == TRUE) {
        continue;
      }
      $checked = TRUE;
    }

    $form['save_collection']['fields']['items'] ['select-' . $instance['field_id']] = [
      '#type' => 'checkbox',
      '#title' => $field_label,
      '#default_value' => $checked,
    ];
    $form['save_collection']['fields']['items'] ['description-' . $instance['field_id']] = [
      '#type' => 'markup',
      '#markup' => $instance['description'],
    ];
    $form['save_collection']['fields']['items'] ['formatters-' . $instance['field_id']] = [
      '#type' => 'markup',
      '#markup' => join(', ', $formatters),
    ];
  }

  $form['save_collection']['button'] = [
    '#type' => 'submit',
    '#value' => 'Save Data Collection',
    '#name' => 'save_collection',
    '#ajax' => [
      'callback' => "tripal_views_handler_area_collections_form_ajax",
      'wrapper' => 'tripal-views-handler-area-collections',
      'effect' => 'fade',
      'method' => 'replace',
      'prevent' => 'click',
    ],
  ];
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
  $fields = [];
  $order = [];
  $children = element_children($form);
  foreach ($children as $key) {
    list($item, $field_id) = preg_split('/-/', $key);
    $fields[$field_id][$item] = $form[$key];
    if (!in_array($field_id, $order)) {
      $order[] = $field_id;
    }
  }

  // Next create a table with each field in a different row.
  $headers = ['Field', 'Description', 'Supported Files Types'];
  $rows = [];
  foreach ($order as $field_id) {
    $rows[] = [
      drupal_render($fields[$field_id]['select']),
      drupal_render($fields[$field_id]['description']),
      drupal_render($fields[$field_id]['formatters']),
    ];
  }
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  return theme_table($table);
}

/**
 * AJAX: Tripal Collections on Tripal Entity-based Views
 */
function tripal_views_handler_area_collections_form_ajax($form, $form_state) {
  return $form;
}

/**
 * Views Area Handler Form SUBMIT: Tripal Collections on Tripal Entity-based
 * Views
 */
function tripal_views_handler_area_collections_form_submit($form, $form_state) {
  global $user;

  $bundle = $form_state['values']['bundle'];
  $view = $form_state['values']['view'];
  $query = $form_state['values']['query'];
  $collection_name = trim($form_state['values']['collection_name']);
  $description = $form_state['values']['collection_desc'];
  $field_ids = array_key_exists('field_ids', $form_state['values']) ? $form_state['values']['field_ids'] : [];
  $uid = $user->uid;
  $bundle_name = $bundle->name;

  // Get the fields that have been selected
  $selected_fids = [];
  foreach ($form_state['values'] as $key => $value) {
    $matches = [];
    if (preg_match('/select-(\d+)/', $key, $matches)) {
      if ($value == 1) {
        $selected_fids[] = $matches[1];
      }
    }
  }

  // Get the entity Ids that match results
  $query->range['length'] = $view->total_rows;
  $results = $query->execute();
  $entities = [];
  foreach ($results['TripalEntity'] as $entity) {
    $entities[] = $entity->id;
  }

  $collection = tripal_create_collection([
    'uid' => $uid,
    'collection_name' => $collection_name,
    'description' => $description,
    'bundle_name' => $bundle_name,
    'ids' => $entities,
    'fields' => $selected_fids,
  ]);
}

/**
 * Views Area Handler Form: Tripal Collections on Drupal Search API-based
 *
 * @param $form
 * @param $form_state
 * @param $view
 *   The views object that this form will be rendered on.
 * @param $query
 *   The views query object generating the views results.
 */
function tripal_views_handler_area_collections_search_api_form($form, $form_state, $view, $query) {

  // Set form defaults.
  $collection_name = '';
  $collection_desc = '';

  $form['save_collection'] = [
    '#type' => 'fieldset',
    '#title' => t('Save Results'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('A data collection is a virtual container into which you can
      save data.  You can place your search results into a data collection for
      download or use with other tools on this site that support data collections.'),
  ];

  // Save the results of the full query for further processing.
  // We use clones to ensure we don't inadvertently change the current view.
  $cloned_view = clone $view;
  $cloned_query = clone $query;
  $cloned_query->set_limit(0);
  $cloned_query->execute($cloned_view);
  $form['save_collection']['results'] = [
    '#type' => 'hidden',
    '#value' => serialize($cloned_view->result),
  ];
  unset($cloned_view, $cloned_query);

  $form['save_collection']['summary'] = [
    '#type' => 'item',
    '#title' => 'Results Summary',
    '#markup' => t('There are @total_rows record(s) that can be added to a data collection.', ['@total_rows' => $view->total_rows]),
  ];

  $form['save_collection']['collection_name'] = [
    '#type' => 'textfield',
    '#title' => t('Collection Name'),
    '#description' => t('Please name this collection for future reference.'),
    '#default_value' => $collection_name,
    '#required' => TRUE,
  ];

  $form['save_collection']['description_fset'] = [
    '#type' => 'fieldset',
    '#title' => t('Add a Description'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  $form['save_collection']['description_fset']['collection_desc'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please provide a description about this data collection. This is meant to help you remember what is in the collection.'),
    '#default_value' => $collection_name,
  ];

  // @todo add ability to choose fields for the collection.

  $form['save_collection']['button'] = [
    '#type' => 'submit',
    '#value' => 'Save Data Collection',
    '#name' => 'save_collection',
    '#ajax' => [
      'callback' => "tripal_views_handler_area_collections_form_ajax",
      'wrapper' => 'tripal-views-handler-area-collections',
      'effect' => 'fade',
      'method' => 'replace',
      'prevent' => 'click',
    ],
  ];

  $form['#prefix'] = '<div id="tripal-views-handler-area-collections">';
  $form['#suffix'] = '</div>';

  return $form;
}

/**
 * Views Area Handler Form SUBMIT: Tripal Collections on Drupal Search
 * API-based Views
 */
function tripal_views_handler_area_collections_search_api_form_validate($form, $form_state) {

  // CHECK: Collection with the given name doesn't already exist.

}

/**
 * Views Area Handler Form SUBMIT: Tripal Collections on Drupal Search
 * API-based Views
 */
function tripal_views_handler_area_collections_search_api_form_submit($form, $form_state) {
  global $user;

  // Create the collection.
  $collection_details = [
    'uid' => $user->uid,
    'collection_name' => trim($form_state['values']['collection_name']),
    'description' => $form_state['values']['collection_desc'],
  ];
  // This can't be done via tripal_create_collection() since we support more
  // then a single bundle. Instead we are going to use the controller class directly.
  $collection = new TripalEntityCollection();
  $collection->create($collection_details);
  $collection_id = $collection->getCollectionID();

  // First, determine the bundle for each entity in our resultset.
  $entities = [];
  $results = unserialize($form_state['values']['results']);
  foreach ($results as $r) {

    // Retrieve the bundle for the current entity.
    // ASSUMPTION: all search results are TripalEntities.
    $wrapper = entity_metadata_wrapper('TripalEntity', $r->entity);
    $bundle = $wrapper->getBundle();

    // If this is the first time we've seen this bundle
    // then initialize the array.
    if (!isset($entities[$bundle])) {
      $entities[$bundle] = [
        'bundle_name' => $bundle,
        'ids' => [],
        'fields' => [],
      ];
    }
    // Note: $r->entity is the entity_id due to the way the search_api saves results.
    // Doing a check here just in case some search index backends store the data differently.
    if (is_int($r->entity)) {
      $entities[$bundle]['ids'][] = $r->entity;
    }
    elseif (is_object($r->entity) AND property_exists($r->entity, 'id')) {
      $entities[$bundle]['ids'][] = $r->entity->id;
    }
    else {
      tripal_report_error(
        'Tripal Data Collection',
        TRIPAL_ERROR,
        'Unable to save entity to collection. Results from the view are: ' . print_r($r, TRUE)
      );
    }
  }

  // Now add the entities to the collection based on bundle.
  foreach ($entities as $bundle_name => $bundle_col) {

    $field_ids = [];
    foreach (field_info_instances('TripalEntity', $bundle_name) as $field) {
      $field_ids[] = $field['field_id'];
    }
    $bundle_col['fields'] = $field_ids;
    $collection->addBundle($bundle_col);
  }

  // Finally, tell the user we're done and link them to the collection.
  drupal_set_message(t("Collection '%name' created with %num_recs record(s). Files for this collection have not yet been created. To create files for download or use with other tools, check the !view.",
      [
        '%name' => $collection_details['collection_name'],
        '%num_recs' => count($results),
        '!view' => l('files page', 'user/' . $user->uid . '/files'),
      ])
  );
}
