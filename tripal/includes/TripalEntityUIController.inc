<?php

/**
 * UI controller.
 */
class TripalEntityUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    $items = [];

    // Set this on the object so classes that extend hook_menu() can use it.
    $this->id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_object';
    $id_count = count(explode('/', $this->path));

    // The content menu.
    $items[$this->path] = [
      'title' => 'Tripal Content',
      'page callback' => 'tripal_content_view',
      'file' => 'includes/tripal.admin.inc',
      'file path' => drupal_get_path('module', 'tripal'),
      'access arguments' => ['access tripal content overview'],
      'type' => MENU_LOCAL_TASK,
      'weight' => -9,
    ];

    $items['bio_data/add'] = [
      'title' => 'Add Tripal Content',
      'page callback' => 'tripal_add_page',
      'access callback' => '_tripal_entity_add_access',
    ];

    // Add a menu item for creating each bundle
    $bundles = array_keys($this->entityInfo['bundles']);
    foreach ($bundles as $bundle_name) {
      $matches = [];
      if (preg_match('/^bio_data_(.*?)$/', $bundle_name, $matches)) {
        $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
        if (!$bundle) {
          throw new Exception(t("Cannot find bundle that matches: %bundle_name",
            ['%bundle_name' => $bundle_name]));
        }
        $term_id = $matches[1];
        // Get the term for this bundle
        $term = entity_load('TripalTerm', ['id' => $term_id]);
        $term = reset($term);
        $default_description = $term->definition ? $term->definition : '';
        // Set a custom page for adding new tripal data entities.
        $items['bio_data/add/' . $term->id] = [
          'title' => ucfirst($bundle->label),
          'description' => tripal_get_bundle_variable('description', $bundle->id, $default_description),
          'page callback' => 'drupal_get_form',
          'page arguments' => ['tripal_entity_form', 2],
          'access arguments' => ['create bio_data_' . $term->id],
        ];
      }
    }

    // Link for viewing a tripal data type.
    $items['bio_data/%'] = [
      'title callback' => 'tripal_entity_title',
      'title arguments' => [1],
      'page callback' => 'tripal_view_entity',
      'page arguments' => [1],
      'access callback' => 'tripal_entity_access',
      'access arguments' => ['view', 1],
      'type' => MENU_CALLBACK,
    ];

    // 'View' tab for an individual entity page.
    $items['bio_data/%/view'] = [
      'title' => 'View',
      'page callback' => 'tripal_view_entity',
      'page arguments' => [1],
      'access callback' => 'tripal_entity_access',
      'access arguments' => ['view', 1],
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    ];

    // 'Edit' tab for an individual entity page.
    $items['bio_data/' . $wildcard . '/edit'] = [
      'title' => 'Edit',
      'page callback' => 'drupal_get_form',
      'page arguments' => ['tripal_entity_form', NULL, 1],
      'access callback' => 'tripal_entity_access',
      'access arguments' => ['edit', 1],
      'type' => MENU_LOCAL_TASK,
      'weight' => -8,
    ];
    $items['bio_data/' . $wildcard . '/reload'] = [
      'title' => 'Reload',
      'page callback' => 'tripal_entity_reload',
      'page arguments' => [1],
      'access arguments' => ['administer tripal'],
      'type' => MENU_LOCAL_TASK,
      'weight' => 10,
    ];
    $items['bio_data/' . $wildcard . '/unpublish'] = [
      'title' => 'Unpublish',
      'page callback' => 'drupal_get_form',
      'page arguments' => ['tripal_entity_unpublish_form', 1],
      'access callback' => 'tripal_entity_access',
      'access arguments' => ['unpublish', 1],
      'type' => MENU_CALLBACK,
      'weight' => 11,
    ];
    $items['bio_data/' . $wildcard . '/delete'] = [
      'title' => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => ['tripal_entity_delete_form', 1],
      'access callback' => 'tripal_entity_access',
      'access arguments' => ['delete', 1],
      'type' => MENU_CALLBACK,
      'weight' => 12,
    ];
    return $items;
  }
}

/**
 * Menu callback to display an entity.
 *
 * As we load the entity for display, we're responsible for invoking a number
 * of hooks in their proper order.
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 * @see hook_entity_view_alter()
 */
function tripal_view_entity($entity, $view_mode = 'full') {
  if (!is_object($entity)) {
    $id = intval($entity);
    if ($id === 0) {
      return drupal_not_found();
    }

    $entities = tripal_load_entity('TripalEntity', [$id]);
    if (empty($entities)) {
      return drupal_not_found();
    }

    $entity = reset($entities);
  }

  $controller = entity_get_controller($entity->type);
  $content = $controller->view([$entity->id => $entity]);
  drupal_set_title($entity->title);
  if ($entity->status == 0) {
    drupal_set_message('This page is currently unpublished', 'warning');
  }
  return $content;
}

/**
 * Provide a data listing for tripal entites (ie: biological data).
 *
 * This function is a callback in a menu item which is set in the
 * TripalEntityUIController class.
 */
function tripal_content_view() {

  // Retrieve our data listing form and render it.
  $form = drupal_get_form('tripal_content_overview_form');
  $output = drupal_render($form);

  // Set the breadcrumb.
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  drupal_set_breadcrumb($breadcrumb);

  return $output;

}

/**
 * Display a listing of Tripal entities.
 *
 * @TODO Filters and bulk operations needed to be added to this form.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return
 *   A form array describing this listing to the Form API.
 */
function tripal_content_overview_form($form, &$form_state) {
  global $user;

  // Set form defaults.  The $_SESSION contains the last known selection
  // by this user. That should be overridden if the $_GET variable contains
  // values.
  $etype = '';
  $status = '';
  if (array_key_exists('tripal_content_overview', $_SESSION)) {
    $etype = $_SESSION['tripal_content_overview']['type'];
    $status = $_SESSION['tripal_content_overview']['status'];
  }
  $etype = array_key_exists('type', $_GET) ? $_GET['type'] : $etype;
  $status = array_key_exists('status', $_GET) ? $_GET['status'] : $status;

  $headers = [
    'title' => [
      'data' => t('Title'),
      'type' => 'property',
      'specifier' => 'title',
    ],
    'Type',
    'Term',
    'Author',
    'status' => [
      'data' => t('Status'),
      'type' => 'property',
      'specifier' => 'status',
    ],
    'changed' => [
      'data' => t('Updated'),
      'type' => 'property',
      'specifier' => 'changed',
      'sort' => 'desc',
    ],
    'Operations',
  ];
  $rows = [];

  // Get the Options arrays used by the fields below. We need to build them
  // here because we will use them both for creating the item list below
  // and for the fields.
  $etypes = db_select('tripal_bundle', 'tb')
    ->fields('tb', ['name', 'label'])
    ->execute()
    ->fetchAllKeyed();
  $etypes = ['0' => 'any'] + $etypes;
  $statuses = [
    '0' => 'any',
    'status-1' => 'published',
    'status-0' => 'not published',
  ];

  // Build the list of existing filters.
  $items = [];
  if ($etype) {
    $items[] = 'where <strong>type</strong> is <strong>' . $etypes[$etype] . '</strong>';
  }
  if ($status) {
    $items[] = 'where <strong>status</strong> is <strong>' . $statuses[$status] . '</strong>';
  }
  // Add 'and' to the beginning of every filter after the first
  if (count($items) > 1) {
    for ($i = 1; $i < count($items); $i++) {
      $items[$i] = 'and ' . $items[$i];
    }
  }
  $list = theme_item_list([
    'items' => $items,
    'title' => '',
    'type' => 'ul',
    'attributes' => [],
  ]);

  // Set the title to be informative (defaults to content for some reason).
  drupal_set_title('Tripal Content');

  $form['filter'] = [
    '#type' => 'fieldset',
    '#title' => 'Show only items where',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div class="exposed-filters">',
    '#suffix' => '</div>',
  ];
  if (count($items) > 0) {
    $form['filter']['filters'] = [
      '#type' => 'item',
      '#markup' => $list,
    ];
  }
  if (!$status) {
    $form['filter']['status'] = [
      '#type' => 'select',
      '#title' => 'Status',
      '#options' => $statuses,
    ];
  }
  else {
    $form['filter']['status'] = [
      '#type' => 'value',
      '#value' => $status,
    ];
  }

  if (!$etype) {
    $form['filter']['type'] = [
      '#type' => 'select',
      '#title' => 'Content Type',
      '#options' => $etypes,
    ];
  }
  else {
    $form['filter']['type'] = [
      '#type' => 'value',
      '#value' => $etype,
    ];
  }

  $form['filter']['buttons'] = [
    '#type' => 'value',
    '#prefix' => '<div id="edit-actions" class="container-inline form-actions form-wrapper">',
    '#suffix' => '</div>',
  ];
  if (count($items) > 0) {
    if (count($items) < 2) {
      $form['filter']['buttons']['refinebtn'] = [
        '#type' => 'submit',
        '#value' => 'Refine',
        '#name' => 'refine',
      ];
    }
    $form['filter']['buttons']['filterbtn'] = [
      '#type' => 'submit',
      '#value' => 'Reset',
      '#name' => 'reset',
    ];
  }
  else {
    $form['filter']['buttons']['filterbtn'] = [
      '#type' => 'submit',
      '#value' => 'Filter',
      '#name' => 'filter',
    ];
  }

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'TripalEntity');
  if ($etype) {
    $query->entityCondition('bundle', $etype);
  }
  if ($status) {
    if ($status == 'status-1') {
      $query->propertyCondition('status', 1);
    }
    if ($status == 'status-0') {
      $query->propertyCondition('status', 0);
    }
  }

  // Find out the total number of records and determine what page we're on, and
  // initialize the pager.
  $cquery = clone $query;
  $cquery->count();
  $num_records = $cquery->execute();

  // Calculate the range and create a pager.
  $query->pager(25);
  $query->tableSort($headers);
  $results = $query->execute();
  $pager = theme('pager');


  // For each entity retrieved add a row to the data listing.
  if (!isset($results['TripalEntity'])) {
    $results['TripalEntity'] = [];
  }
  foreach ($results['TripalEntity'] as $entity_id => $stub) {
    $vocabulary = '';
    $term_name = '';

    // We don't need all of the attached fields for an entity so, we'll
    // not use the entity_load() function.  Instead just pull it from the
    // database table.
    $equery = db_select('tripal_entity', 'TE');
    $equery->join('tripal_bundle', 'TB', 'TE.bundle = TB.name');
    $equery->fields('TB', ['label']);
    $equery->fields('TE');
    $equery->condition('TE.id', $entity_id);
    $entity = $equery->execute()->fetchObject();
    if (!$entity) {
      continue;
    }

    // Get the term
    $term = entity_load('TripalTerm', ['id' => $entity->term_id]);
    $term = reset($term);
    if ($term) {
      $term_name = $term->name;
      $vocab = entity_load('TripalVocab', ['id' => $term->vocab_id]);
      $vocab = reset($vocab);
      $vocabulary = $vocab->vocabulary;
    }

    // Retrieve details about the user who created this data.
    $author = user_load($entity->uid);

    // Build the action links
    $links = '';
    if (entity_access('edit', 'TripalEntity', $entity, $user)) {
      $links .= '&nbsp;&nbsp;' . l('edit', 'bio_data/' . $entity->id . '/edit');
    }
    if (entity_access('unpublish', 'TripalEntity', $entity, $user)) {
      $links .= '&nbsp;&nbsp;' . l('unpublish', 'bio_data/' . $entity->id . '/unpublish');
    }

    // Add information to the table.
    $rows[] = [
      l($entity->title, 'bio_data/' . $entity->id),
      $entity->label,
      $vocabulary . ':' . $term_name,
      l($author->name, 'user/' . $entity->uid),
      $entity->status == 1 ? 'published' : 'unpublished',
      format_date($entity->changed, 'short'),
      $links,
    ];
  }

  // If there are no entites created yet then add a message to the table to
  // provide guidance to administrators.
  if (empty($rows)) {
    $rows[] = [
      [
        'data' => t('No content can be found.'),
        'colspan' => 7,
      ],
    ];
  }

  // Render the data listing.
  $table_vars = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  $output = "<div><strong>Found $num_records records</strong></div>" . $pager . theme('table', $table_vars) . $pager;
  $form['results'] = [
    '#type' => 'markup',
    '#markup' => $output,
  ];

  return $form;
}

/**
 *
 */
function tripal_content_overview_form_validate($form, &$form_state) {
}

/**
 *
 */
function tripal_content_overview_form_submit($form, &$form_state) {
  // Always just rebuild the form on submit.  that will update the
  // result table using the filters provided.
  $form_state['rebuild'] = TRUE;

  // Save the current user filter settings.
  if ($form_state['clicked_button']['#name'] == 'filter' or
    $form_state['clicked_button']['#name'] == 'refine') {
    $_SESSION['tripal_content_overview']['type'] = array_key_exists('type', $form_state['values']) ? $form_state['values']['type'] : '';
    $_SESSION['tripal_content_overview']['status'] = array_key_exists('status', $form_state['values']) ? $form_state['values']['status'] : '';
  }
  if ($form_state['clicked_button']['#name'] == 'reset') {
    unset($_SESSION['tripal_content_overview']);
  }
}

/**
 * Clears the cache of a given entity to force a reload.
 *
 * @param $entity_id
 *   The entity_id of the entity to reload.
 */
function tripal_entity_reload($entity) {
  $entity_id = $entity->id;
  $cid = 'field:TripalEntity:' . $entity_id . ':';
  cache_clear_all($cid, 'cache_field', TRUE);

  $sql = "SELECT count(*) FROM cache_field WHERE cid like :cid";
  $count = db_query($sql, [':cid' => $cid . '%'])->fetchField();

  if (!isset($count) or $count > 0) {
    drupal_set_message('Failed to clear the cache for this entity.');
  }
  else {
    // We must reload the entity.  If we don't then other processes that
    // depended on a fully loaded entity object will mess up.
    $entity = tripal_load_entity('TripalEntity', [$entity->id]);

    drupal_set_message('Cache cleared, entity reloaded');
  }
  drupal_goto('bio_data/' . $entity_id);
}

/**
 *
 */
function tripal_entity_form($form, &$form_state, $term_id = '', $entity = NULL) {
  global $user;
  $bundle_name = 'bio_data_' . $term_id;

  // Add a vertical tabs element
  $form['entity_form_vtabs'] = [
    '#type' => 'vertical_tabs',
    '#weight' => 999,
  ];

  // If the entity doesn't exist then create one.
  if (!$entity) {
    $entity = entity_get_controller('TripalEntity')->create([
      'bundle' => $bundle_name,
      'term_id' => $term_id,
    ]);
    field_attach_form('TripalEntity', $entity, $form, $form_state);

    $form['add_button'] = [
      '#type' => 'submit',
      '#value' => t('Save'),
      '#name' => 'add_data',
      '#weight' => 1000,
    ];
  }
  else {
    field_attach_form('TripalEntity', $entity, $form, $form_state);

    $form['update_button'] = [
      '#type' => 'submit',
      '#value' => t('Update'),
      '#name' => 'update_data',
      '#weight' => 1000,
    ];

    if (entity_access('unpublish', 'TripalEntity', $entity, $user)) {
      $form['unpublish_button'] = [
        '#type' => 'submit',
        '#value' => t('Unpublish'),
        '#name' => 'unpublish_data',
        '#weight' => 1002,
        '#attributes' => ['style' => 'float: right'],
      ];
    }
    // Put the delete button on the far-right so that it's harder
    // to accidentally click it.
    if (entity_access('delete', 'TripalEntity', $entity, $user)) {
      $form['delete_button'] = [
          '#type' => 'submit',
          '#value' => t('Delete'),
          '#name' => 'delete_data',
          '#weight' => 1003,
          '#attributes' => ['style' => 'float: right'],
      ];
    }
  }
  $form['additional_settings'] = [
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  ];

  // Node author information for administrators
  $form['author'] = [
    '#type' => 'fieldset',
    '#access' => user_access('administer nodes'),
    '#title' => t('Authoring information'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#attributes' => [
      'class' => ['TripalEntity-form-author'],
    ],
    '#attached' => [
      'js' => [
        drupal_get_path('module', 'node') . '/node.js',
        [
          'type' => 'setting',
          'data' => ['anonymous' => variable_get('anonymous', t('Anonymous'))],
        ],
      ],
    ],

    '#weight' => 90,
  ];

  $account = user_load($entity->uid);
  $form['author']['author_name'] = [
    '#type' => 'textfield',
    '#title' => t('Authored by'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => !empty($account->name) ? $account->name : '',
    '#weight' => -1,
    '#description' => t('Leave blank for %anonymous.', ['%anonymous' => variable_get('anonymous', t('Anonymous'))]),
  ];
  $form['author']['author_date'] = [
    '#type' => 'textfield',
    '#title' => t('Authored on'),
    '#maxlength' => 25,
    '#description' => t('Format: %time. The date format is YYYY-MM-DD and ' .
      '%timezone is the time zone offset from UTC. Leave blank to use the ' .
      'time of form submission.',
      [
        '%time' => !empty($entity->created) ? date('Y-m-d H:i:s O', $entity->created) : format_date($entity->created, 'custom', 'Y-m-d H:i:s O'),
        '%timezone' => !empty($entity->created) ? date('O', $entity->created) : format_date($entity->created, 'custom', 'O'),
      ]
    ),
    '#default_value' => !empty($entity->created) ? date('Y-m-d H:i:s O', $entity->created) : '',
  ];
  // Node options for administrators
  $form['options'] = [
    '#type' => 'fieldset',
    '#access' => user_access('administer nodes'),
    '#title' => t('Publishing options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#attributes' => [
      'class' => ['node-form-options'],
    ],
    '#attached' => [
      'js' => [drupal_get_path('module', 'node') . '/node.js'],
    ],
    '#weight' => 95,
  ];
  $form['options']['status'] = [
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => $entity->status,
  ];


  $form['cancel_button'] = [
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#name' => 'cancel_data',
    '#weight' => 1001,
    '#limit_validation_errors' => [['']],
  ];

  // The entity object must be added to the $form_state in order for
  // the Entity API to work. It must have a key of the entity name.
  $form_state['TripalEntity'] = $entity;

  $form['#prefix'] = "<div id='$bundle_name-entity-form'>";
  $form['#suffix'] = "</div>";
  $form['#submit'][] = 'tripal_entity_form_submit';
  return $form;
}

/**
 * An Ajax callback for the tripal_entity_form.
 */
function tripal_entity_form_ajax_callback($form, $form_state) {
  // return the form so Drupal can update the content on the page
  return $form;
}

/**
 * Implements hook_validate() for the tripal_entity_form.
 */
function tripal_entity_form_validate($form, &$form_state) {

  // If the user is cancelling or deleting the entity then don't validate.
  if (array_key_exists('clicked_button', $form_state) and
    ($form_state['clicked_button']['#name'] == 'cancel_data' or
      $form_state['clicked_button']['#name'] == 'delete_data')) {
    return;
  }
  // For adds and updates, perform validation.
  $entity = $form_state['TripalEntity'];
  field_attach_form_validate('TripalEntity', $entity, $form, $form_state);

  $username = $form_state['values']['author_name'];
  $user = user_load_by_name($username);
  if (!$user) {
    form_set_error('author_name', 'Please provide a valid author name.');
  }

  try {
    $create_date = new DateTime($form_state['values']['author_date']);
  } catch (Exception $e) {
    form_set_error('author_date', 'Please provide a valid authored on date.');
  }
}

/**
 * Implements hook_submit() for the tripal_entity_form.
 */
function tripal_entity_form_submit($form, &$form_state) {
  $entity = $form_state['TripalEntity'];
  global $user;

  if ($form_state['clicked_button']['#name'] == 'cancel_data') {
    if (property_exists($entity, 'id')) {
      $form_state['redirect'] = "bio_data/" . $entity->id;
    }
    else {
      $form_state['redirect'] = 'bio_data/add';
    }
    return;
  }

  if ($form_state['clicked_button']['#name'] == 'delete_data') {
    if (entity_access('delete', 'TripalEntity', $entity, $user)) {
      $form_state['redirect'] = 'bio_data/' . $entity->id . '/delete';
    }
    return;
  }
  if ($form_state['clicked_button']['#name'] == 'unpublish_data') {
    if (entity_access('unpublish', 'TripalEntity', $entity, $user)) {
      $form_state['redirect'] = 'bio_data/' . $entity->id . '/unpublish';
    }
    return;
  }

  $username = $form_state['values']['author_name'];
  $user = user_load_by_name($username);
  $entity->uid = $user->uid;

  $create_date = $form_state['values']['author_date'];
  $entity->created = $create_date;

  $published = $form_state['values']['status'];
  $entity->status = $published;

  // Allow the fields to perform actions prior to submit.
  $instances = field_info_instances('TripalEntity', $entity->bundle);
  $langcode = 'und';
  foreach ($instances as $field_name => $instance) {
    $entity_type = $instance['entity_type'];
    if ($entity_type == 'TripalEntity' and array_key_exists($field_name, $form)) {
      foreach ($form[$field_name][$langcode] as $delta => $field_form) {
        if (!preg_match('/^\d+$/', $delta)) {
          continue;
        }
        $widget_type = $instance['widget']['type'];
        if (tripal_load_include_field_class($widget_type)) {
          $field = $field_form['#field'];
          $widget = new $widget_type($field, $instance);
          $widget->submit($form, $form_state, $entity_type, $entity, $langcode, $delta);
        }
      }
    }
  }

  $entityform = entity_ui_controller('TripalEntity')->entityFormSubmitBuildEntity($form, $form_state);
  if ($entityform->save()) {
    $form_state['redirect'] = "bio_data/" . $entity->id;
  }
  else {
    drupal_set_message('Cannot save entity', 'error');
  }
}


/**
 * Provides a list of TripalEntity types (bundles) for the user to add.
 *
 * This function is a callback in a menu item which is set in the
 * TripalEntityUIController class.
 */
function tripal_add_page() {

  // The content array to be returned.
  $content = [];

  $content['instructions'] = [
    '#type' => 'markup',
    '#markup' => 'This page provides links for creating pages for Tripal ' .
      'supported content types. These content types are organized by categories. ' .
      'Please note, however, that the categorization is the most common use ' .
      'for a given type. Some types may be useful in other "categories" as well.',
    '#weight' => -15,
  ];


  // Get the list of categories.
  $select = "
    SELECT TBV.value as category
    FROM tripal_bundle_variables TBV
      INNER JOIN tripal_variables TV on TV.variable_id = TBV.variable_id
    WHERE TV.name = 'bundle_category'
  ";
  $categories = db_query($select);


  // Build the fieldsets for the categories.
  $fieldsets = [];
  $category_weight = 1;
  while ($category = $categories->fetchField()) {
    $machine_name = preg_replace('/[^\w]/', '_', $category);
    $fieldsets[$machine_name . '_fieldset'] = [
      '#type' => 'fieldset',
      '#title' => $category,
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#attributes' => [
        'class' => ['collapsible', 'tripal-content-list'],
      ],
      '#attached' => [
        'js' => ['misc/collapse.js', 'misc/form.js'],
      ],
      '#weight' => $category == 'General' ? -10 : $category_weight++,
    ];
  }

  // Create the "other" fieldset, and set it's weight to 100 so it goes to
  // the bottom.
  $fieldsets['Other_fieldset'] = [
    '#type' => 'fieldset',
    '#title' => 'Other',
    '#weight' => 100,
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#attributes' => [
      'class' => ['collapsible'],
    ],
    '#attached' => [
      'js' => ['misc/collapse.js', 'misc/form.js'],
    ],
  ];


  // Get the list of bundles and iterate through them.
  $select = "SELECT id, name, label FROM tripal_bundle ORDER BY label";
  $bundles = db_query($select);
  while ($bundle = $bundles->fetchObject()) {

    // Lookup the bundle category.
    $sql = "
      SELECT TBV.value as category
      FROM tripal_bundle TB
        INNER JOIN tripal_bundle_variables TBV on TB.id = TBV.bundle_id
        INNER JOIN tripal_variables TV on TV.variable_id = TBV.variable_id
      WHERE TV.name = 'bundle_category' and TB.id = :id;
    ";

    $category = db_query($sql, [':id' => $bundle->id])->fetchField();
    if (!$category) {
      $category = 'Other';
    }

    $machine_name = preg_replace('/[^\w]/', '_', $category);
    $bundle = tripal_load_bundle_entity(['id' => $bundle->id]);
    if (!$bundle) {
      continue;
    }
    if (user_access('create ' . $bundle->name)) {
      if (!array_key_exists($machine_name . '_fieldset', $content)) {
        $content[$machine_name . '_fieldset'] = $fieldsets[$machine_name . '_fieldset'];
      }
      $content[$machine_name . '_fieldset'][$bundle->name] = [
        '#type' => 'item',
        '#markup' => l($bundle->label, 'bio_data/add/' . $bundle->id),
        '#description' => $bundle->term->definition,
      ];
    }
  }

  // Now iterate through the fieldsets and set their weight
  return $content;
}

/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_tripal_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = tripal_set_message(
      t('This page is for adding Tripal content to your site. However, before you can add data you have to specify what types of data your site will support. For example, if you want to add genes to be displayed to users, you must first create a data type "gene".'),
      TRIPAL_INFO,
      ['return_html' => TRUE]
    );
    $output .= '<p>' . t('You have not created any biological data types yet. ' .
        'Go to the <a href="@create-content">data type creation page</a> to ' .
        'add a new data type.',
        ['@create-content' => url('admin/structure/bio_data/add')]) . '</p>';
  }
  return $output;
}

/**
 * Form callback: confirmation form for unpublishing a tripal_entity.
 *
 * @param $tripal_entity
 *   The tripal_entity to delete
 *
 * @see confirm_form()
 */
function tripal_entity_unpublish_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;
  $form['#submit'][] = 'tripal_entity_unpublish_form_submit';

  $form = confirm_form($form,
    t('Unpublish the record "%title"?  This will delete the Drupal record, but not the underlying (eg Chado) record.',
      ['%title' => $entity->title]), 'admin/content/bio_data',
    '<p>' . t('This action cannot be undone.') . '</p>',
      t('Yes, Unpublish'), t('No, Cancel'), 'confirm');

  return $form;
}

/**
 * Submit callback for tripal_entity_unpublish_form
 */
function tripal_entity_unpublish_form_submit($form, &$form_state) {
  global $user;
  $entity = $form_state['entity'];

  if (!entity_access('unpublish', 'TripalEntity', $entity, $user)) {
    drupal_set_message(t('You do not have permission to unpublish this content.'), "error");
    $form_state['redirect'] = 'admin/content/bio_data';
    return;
  }

  $entity_controller = new TripalEntityController($entity->type);

  if ($entity_controller->unpublish([$entity->id])) {
    drupal_set_message(t('The record "%name" has been unpublished. The record, however, remains in the database and you can republish it later.', ['%name' => $entity->title]));
    $form_state['redirect'] = 'admin/content/bio_data';
  }
  else {
    drupal_set_message(t('The record "%name" was not unpublished.', ['%name' => $entity->title]), "error");
  }
}

/**
 * Form callback: confirmation form for deleting a tripal_entity.
 *
 * @param $tripal_entity
 *   The tripal_entity to delete
 *
 * @see confirm_form()
 */
function tripal_entity_delete_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;
  $form['#submit'][] = 'tripal_entity_delete_form_submit';

  $form = confirm_form($form,
      t('Delete the record "%title"?',
          ['%title' => $entity->title]), 'admin/content/bio_data',
      '<p>' . t('This action cannot be undone. It will result in this record being unpublished and completely removed from the database. Any downstream records that depend on this entry will also be unpublished and completely removed as well. Please delete with caution!') . '</p>', t('Yes, Delete'), t('No, Cancel'), 'confirm');

  return $form;
}

/**
 * Submit callback for tripal_entity_unpublish_form
 */
function tripal_entity_delete_form_submit($form, &$form_state) {
  global $user;
  $entity = $form_state['entity'];

  if (!entity_access('delete', 'TripalEntity', $entity, $user)) {
    drupal_set_message(t('You do not have permission to delete this content.'), "error");
    $form_state['redirect'] = 'admin/content/bio_data';
    return;
  }

  $entity_controller = new TripalEntityController($entity->type);

  if ($entity_controller->delete([$entity->id])) {
    drupal_set_message(t('The record "%name" was deleted along with any dependent, downstream records.  Deleted records are no longer in the database. However, the site may still have published records. Therefore, a job has been added to remove any published but orphaned records.', ['%name' => $entity->title]));
    $form_state['redirect'] = 'admin/content/bio_data';
  }
  else {
    drupal_set_message(t('The record "%name" was not deleted.', ['%name' => $entity->title]), "error");
    $form_state['redirect'] = 'admin/content/bio_data';
  }
}

/**
 * A helper function for checking if a user can add Tripal Content.
 *
 * This function is a callback for the bio_data/add menu path.
 */
function _tripal_entity_add_access() {
  global $user;
  $types = tripal_get_content_types();
  foreach ($types as $type) {
    if (user_access('create ' . $type->name, $user)) {
      return TRUE;
    }
  }
  return FALSE;

}
