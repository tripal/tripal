<?php

/**
 * Implements hook_field_create_instance().
 */
function tripal_field_create_instance($instance) {
  $field = field_info_field($instance['field_name']);
  $field_class = $field['type'];
  if (tripal_load_include_field_class($field_class)) {
    $field = new $field_class($field, $instance);
    return $field->createInstance();
  }
}

/**
 * Implements hook_field_info().
 *
 * We want the Tripal module to handle all TripalFields.  This will allow
 * other modules to be more easily disabled/enabled because Drupal won't
 * let a module be disabled if it supports fields that are actively attached
 * to bundles.  Therefore any module that provides a new TripalField will be
 * discovered and listed for Drupal by this function.
 */
function tripal_field_info() {

  $info = [];
  $field_types = tripal_get_field_types();
  foreach ($field_types as $field_type) {
    $info[$field_type] = $field_type::info();
  }
  return $info;
}

/**
 * Implements hook_info_alter().
 */
function tripal_field_info_alter(&$info) {
  foreach ($info as $field_name => $details) {
    // Make sure all fields have a term setting so we can map
    // all fields to a vocabulary term for the semantic web.
    if (array_key_exists('instance_settings', $details)) {
      if (!array_key_exists('term_vocabulary', $details['instance_settings'])) {
        $info[$field_name]['instance_settings']['term_vocabulary'] = '';
      }
      if (!array_key_exists('term_name', $details['instance_settings'])) {
        $info[$field_name]['instance_settings']['term_name'] = '';
      }
      if (!array_key_exists('term_accession', $details['instance_settings'])) {
        $info[$field_name]['instance_settings']['term_accession'] = '';
      }
      if (!array_key_exists('term_fixed', $details['instance_settings'])) {
        $info[$field_name]['instance_settings']['term_fixed'] = FALSE;
      }
      if (!array_key_exists('auto_attach', $details['instance_settings'])) {
        $info[$field_name]['instance_settings']['auto_attach'] = TRUE;
      }
    }
    else {
      $info[$field_name]['instance_settings']['term_vocabulary'] = '';
      $info[$field_name]['instance_settings']['term_name'] = '';
      $info[$field_name]['instance_settings']['term_accession'] = '';
      $info[$field_name]['instance_settings']['term_fixed'] = FALSE;
      $info[$field_name]['instance_settings']['auto_attach'] = TRUE;
    }
  }
}

/**
 * Implements hook_field_widget_info();
 */
function tripal_field_widget_info() {
  $info = [];
  $widgets = tripal_get_field_widgets();
  foreach ($widgets as $widget) {
    $info[$widget] = $widget::info();
  }
  return $info;
}

/**
 * Implements hook_field_widget_info_alter();
 */
function tripal_field_widget_info_alter(&$info) {

}

/**
 * Implements hook_field_formatter_info().
 */
function tripal_field_formatter_info() {
  $info = [];
  $formatters = tripal_get_field_formatters();
  foreach ($formatters as $formatter) {
    $info[$formatter] = $formatter::info();
  }
  return $info;
}

/**
 * Implements hook_field_formatter_info_alter();
 */
function tripal_field_formatter_info_alter(&$info) {

}

/**
 * Implements hook_bundle_create().
 *
 * This is a Tripal defined hook and is called in the
 * TripalBundleController::create()
 * function to allow modules to perform tasks when a bundle is created.
 */
function tripal_bundle_create($bundle, $args) {
  $field_type = 'rdfs__type';
  $field_name = 'rdfs__type';

  // Add the field, unless it already exists.
  if (!field_info_field($field_name)) {
    $field = field_create_field([
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'tripal_no_storage',
      ],
    ]);
  }


  // Add an instance of the field to the bundle.
  if (!field_info_instance($bundle->type, $field_name, $bundle->name)) {
    $instance = field_create_instance([
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => 'Resource Type',
      'description' => 'The resource type',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'term_vocabulary' => 'rdfs',
        'term_accession' => 'type',
        'term_name' => 'type',
      ],
      'widget' => [
        'type' => 'rdfs__type_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'rdfs__type_formatter',
          'settings' => [],
        ],
      ],
    ]);
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function tripal_field_formatter_view($entity_type, $entity, $field,
                                     $instance, $langcode, $items, $display) {

  // Don't show any fields that don't have a controlled vocabulary term in
  // the database.
  $vocabulary = $instance['settings']['term_vocabulary'];
  $accession = $instance['settings']['term_accession'];
  $term = tripal_get_term_details($vocabulary, $accession);
  if (!$term) {
    tripal_set_message(t("The controlled vocabulary term, ':term (:term_name)', assigned to the
        field, ':field', is not in the database. The field cannot be shown.
        Please add the term and the field will appear below. ",
      [
        ':field' => $field['field_name'],
        ':term' => $vocabulary . ":" . $accession,
        ':term_name' => $instance['settings']['term_name'],
      ]),
      TRIPAL_WARNING);
    return;
  }

  $element = [];
  $formatter_class = $display['type'];
  $is_loaded = tripal_load_include_field_class($formatter_class);
  if ($is_loaded) {
    $formatter = new $formatter_class($field, $instance);
    $formatter->view($element, $entity_type, $entity, $langcode, $items, $display);
  }
  return $element;
}

/**
 * Simple provides a message indicating that the field cannot be deleted.
 *
 * This function is used in the tripal_menu_alter() function.  We alter the
 * menu created for managing fields to use this call back which
 * prints a message that the field cannot be deleted.
 */
function tripal_field_no_delete() {
  drupal_set_message('This field cannot be removed.', 'warning');
  return '';
}

/**
 *
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_field_overview_form_ is used for adding and reordering the
 * fields attached to a bundle.  It also includes edit and delete links and
 * links for editing field types and widgets.
 *
 * This alter function is used to add a new 'Supported By' column to
 * the table to let the user know where fields are storing their data.
 */
function tripal_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {

  $used_terms = [];

  // If this isn't a TripalEntity content type then skip this form.
  if ($form['#entity_type'] != 'TripalEntity') {
    return;
  }

  // Add the 'Storage Location' to the table header.
  $form['fields']['#header'][] = 'Term';
  $form['fields']['#header'][] = 'Supported By * ';

  // Add the storage location as the final column for each field.
  $storage_info = module_invoke_all('field_storage_info');
  foreach (element_children($form['fields']) as $field_name) {

    $field = field_info_field($field_name);
    $instance = field_info_instance('TripalEntity', $field_name, $form['#bundle']);

    // Warn users if a field is missing a term.
    if ($instance and $instance['entity_type'] == 'TripalEntity' and
      array_key_exists('settings', $instance) and is_array($instance['settings']) and
      (!array_key_exists('term_vocabulary', $instance['settings']) or !$instance['settings']['term_vocabulary'])) {
      tripal_report_error('tripal_fields', TRIPAL_WARNING,
        'The field, !field, is missing a controlled vocabulary term. Please edit the field and set a term, otherwise this field may not work properly.',
        ['!field' => $field_name],
        ['drupal_set_message' => TRUE]);
    }
    // Warn users if any of the terms are not unique.
    if ($instance and array_key_exists('settings', $instance) and is_array($instance['settings']) and
      array_key_exists('term_vocabulary', $instance['settings'])) {
      $term = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];
      if (array_key_exists($term, $used_terms)) {
        $used_terms[$term][] = $field_name;
        tripal_report_error('tripal_fields', TRIPAL_WARNING,
          'The term !term is in use by multiple fields: !fields. ' .
          'This is not allowed. Every field must have a different controlled vocabulary term. ' .
          'Please correct the term assignments.',
          ['!term' => $term, '!fields' => implode(', ', $used_terms[$term])],
          ['drupal_set_message' => TRUE]);
      }
      $used_terms[$term][] = $field_name;
    }


    // For rows in the tables that aren't fields, just add an empty value
    // for the storage column.
    if (!$field) {
      $form['fields'][$field_name][] = [
        '#markup' => '',
      ];
      $form['fields'][$field_name][] = [
        '#markup' => '',
      ];
      continue;
    }
    $term_info = '';
    if (array_key_exists('term_accession', $instance['settings']) and $instance['settings']['term_accession']) {
      $term = tripal_get_term_details($instance['settings']['term_vocabulary'], $instance['settings']['term_accession']);
      $term_info = $term['name'] . ' (' . $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'] . ')';
    }
    $form['fields'][$field_name][] = [
      '#markup' => $term_info,
    ];

    $storage_type = $field['storage']['type'];
    $storage_label = array_key_exists('label', $storage_info[$storage_type]) ? $storage_info[$storage_type]['label'] : '';
    if ($storage_type == 'field_sql_storage') {
      $storage_label = 'Drupal';
    }
    if (array_key_exists('logo_url', $storage_info[$storage_type])) {
      $logo_url = $storage_info[$storage_type]['logo_url'];
      $form['fields'][$field_name][] = [
        '#markup' => '<img class="form-field-ui-field-overview-storage-logo" src="' . $logo_url . '">',
      ];
    }
    else {
      $form['fields'][$field_name][] = [
        '#markup' => $storage_label,
      ];
    }
  }
  $form['note'] = [
    '#markup' => '* Fields attached to this content type can use various
      storage backends. Please be sure when you add new fields that the
      storage backend is appropriate. For example, if you use Chado, and you
      want all biological content to be stored in Chado, be sure that the
      respective fields are "supported by" Chado.',
  ];

  $form['#submit'] = array_merge(['tripal_form_field_ui_field_overview_form_submit'], $form['#submit']);
}

/**
 * A submit function for the field_ui_field_overview_form.
 *
 */
function tripal_form_field_ui_field_overview_form_submit($form, &$form_state) {
  $form_values = $form_state['values']['fields'];
  $admin_path = _field_ui_bundle_admin_path('TripalEntity', $form['#bundle']);
  $destinations = [];

  // If the form Field UI form is adding a new field to the bundle we want
  // to preempt Drupal from creating the field in it's field_sql_storage
  // backend. We want to create it.
  if (!empty($form_values['_add_new_field']['field_name'])) {
    try {
      // Is the field type a TripalField? If so then we want
      // to pass of creation of the field to the module that manages that field.
      $type = $form_values['_add_new_field']['type'];
      if (tripal_load_include_field_class($type)) {
        $module = $type::$module;

        $function = $module . '_bundle_create_user_field';
        $bundle = tripal_load_bundle_entity(['name' => $form['#bundle']]);
        $field_name = $form_values['_add_new_field']['field_name'];

        // If the module implements the hook then we'll have it create the
        // field and instance.
        if (function_exists($function)) {
          $function($form_values['_add_new_field'], $bundle);
        }
        // Otherwise, we should make a good attempt on our own.
        // Especially in the case of fields added via the Library directory.
        else {
          $new_field = $form_values['_add_new_field'];

          // Create the field.
          $field = [
            'field_name' => $new_field['field_name'],
            'type' => $new_field['type'],
            'cardinality' => FIELD_CARDINALITY_UNLIMITED, // @hard-coded
            'locked' => FALSE,
            'storage' => [
              'type' => $type::$default_settings['storage'],
            ],
          ];
          field_create_field($field);

          // Then create the instance.
          $instance = [
            'field_name' => $new_field['field_name'],
            'entity_type' => $bundle->type,
            'bundle' => $bundle->name,
            'label' => $new_field['label'],
            'description' => $type::$default_description,
            'required' => FALSE,
            'settings' => [
              'auto_attach' => $type::$default_instance_settings['auto_attach'],
            ],
            'widget' => [
              'type' => $new_field['widget_type'],
              'settings' => [],
            ],
            'display' => [
              'default' => [
                'label' => 'hidden',
                'type' => $type::$default_formatter,
                'settings' => [],
              ],
            ],
          ];
          field_create_instance($instance);
        }
        $destinations[] = $admin_path . '/fields/' . $field_name . '/field-shcef';
        $destinations[] = $admin_path . '/fields/' . $field_name;

        // Store new field information for any additional submit handlers.
        $form_state['fields_added']['_add_new_field'] = $field_name;

        // Unset the the _add_new_field entry so Drupal doesn't try to
        // Create the field.
        unset($form_state['values']['fields']['_add_new_field']);
        drupal_set_message('Please set the controlled vocabulary that best describes the data of this field. See the "Controlled Vocabulary Term" section below.', 'warning');
      }
    } catch (Exception $e) {
      drupal_set_message(t('There was a problem creating field %label: !message', [
        '%label' => $instance['label'],
        '!message' => $e->getMessage(),
      ]), 'error');
    }

    if ($destinations) {
      $destination = drupal_get_destination();
      $destinations[] = $destination['destination'];
      unset($_GET['destination']);
      $form_state['redirect'] = field_ui_get_destinations($destinations);
    }
    else {
      drupal_set_message(t('Your settings have been saved.'));
    }
  }
}

/**
 * Implements hook_module_implements_alter()
 *
 * We want our edits to the field_ui_field_overview_form form to occur after
 * all modules have implemented their changes.
 */
function tripal_module_implements_alter(&$implementations, $hook) {
  if ($hook == 'form_alter') {
    $group = $implementations['tripal'];
    unset($implementations['tripal']);
    $implementations['tripal'] = $group;
  }
}


/**
 * Implements hook_field_settings_form()
 */
function tripal_field_settings_form($field, $instance, $has_data) {
  $field_class = $field['type'];
  if (tripal_load_include_field_class($field_class)) {
    $field = new $field_class($field, $instance);
    return $field->settingsForm($has_data);
  }
}

/**
 * Implements hook_instance_settings_form()
 */
function tripal_field_instance_settings_form($field, $instance) {
  $field_class = $field['type'];
  if (tripal_load_include_field_class($field_class)) {
    $field = new $field_class($field, $instance);
    return $field->instanceSettingsForm();
  }
}

/**
 * Validates the TripalField instance settings form.
 *
 * This function is called because the TripalField::instanceSettingsForm()
 * adds it to the form element.  By default, Drupal does not provide a
 * validate hook for the instance settings form.
 */
function tripal_field_instance_settings_form_validate($element, &$form_state, $form) {
  $field = $element['#field'];
  $instance = $element['#instance'];

  $field_class = $field['type'];
  if (tripal_load_include_field_class($field_class)) {
    $field = new $field_class($field, $instance);
    return $field->instanceSettingsFormValidate($form, $form_state);
  }
}

/**
 * Allows for altering of a field's instance setting form.
 *
 * This appears to be a Drupal hook but is actually a custom function created
 * by this module. It is called by the tripal_form_alter() function of this
 * module.
 *
 * Here we put additional form elements for any field, regardless if it is
 * a tripalField or not.
 *
 * @param $form
 *   The form array.  Alterations to the form can be made within this array.
 * @param $form_state
 *   The form state array.
 */
function tripal_field_instance_settings_form_alter(&$form, $form_state) {
  global $language;

  // It's not possible to add AJAX to a form element in the hook_form_alter
  // function.  To make it work we have to add a process function. Inisde
  // of that process function is where the form additions get added that use
  // Ajax.
  $form['field_term'][$language->language][0]['#process'] = ['tripal_field_instance_settings_form_alter_process'];
  $form['#submit'][] = 'tripal_field_instance_settings_form_submit';
}

/**
 * Implements a process function for the instance settings form.
 *
 * See the comment in the tripal_field_instance_settings_form_alter() for
 * more details.
 */
function tripal_field_instance_settings_form_alter_process($element, &$form_state, $form) {
  $field = $form['#field'];
  $instance = $form['#instance'];

  // Get the term for this instance.
  $vocabulary = '';
  $accession = '';
  $term_name = '';
  $term = NULL;
  if (array_key_exists('settings', $instance) and
    array_key_exists('term_vocabulary', $instance['settings'])) {
    $vocabulary = $instance['settings']['term_vocabulary'];
    $accession = $instance['settings']['term_accession'];
    $term_name = $instance['settings']['term_name'];
    $term = tripal_get_term_details($vocabulary, $accession);
  }

  // Construct a table for the vocabulary information.
  $headers = [];
  $rows = [];
  $rows[] = [
    [
      'data' => 'Vocabulary',
      'header' => TRUE,
      'width' => '20%',
    ],
    $term['vocabulary']['name'] . ' (' . $vocabulary . ') ' . $term['vocabulary']['description'],
  ];
  $rows[] = [
    [
      'data' => 'Term',
      'header' => TRUE,
      'width' => '20%',
    ],
    $vocabulary . ':' . $accession,
  ];
  $rows[] = [
    [
      'data' => 'Name',
      'header' => TRUE,
      'width' => '20%',
    ],
    $term['name'],
  ];
  $rows[] = [
    [
      'data' => 'Definition',
      'header' => TRUE,
      'width' => '20%',
    ],
    $term['definition'],
  ];
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  $description = t('All fields attached to a Tripal-based content type must
        be associated with a controlled vocabulary term.  Please use caution
        when changing the term for this field as other sites may expect this term
        when querying web services.');
  if (array_key_exists('term_fixed', $instance['settings']) and $instance['settings']['term_fixed']) {
    $description = t('All fields attached to a Tripal-based content type must
        be associated with a controlled vocabulary term. This field mapping is
        required and cannot be changed');
  }
  $element['term_vocabulary'] = [
    '#type' => 'value',
    '#value' => $vocabulary,
  ];
  $element['term_name'] = [
    '#type' => 'value',
    '#value' => $term_name,
  ];
  $element['term_accession'] = [
    '#type' => 'value',
    '#value' => $accession,
  ];
  $element['field_term'] = [
    '#type' => 'fieldset',
    '#title' => 'Controlled Vocabulary Term',
    '#description' => $description,
    '#prefix' => '<div id = "tripal-field-term-fieldset">',
    '#suffix' => '</div>',
  ];
  $element['field_term']['details'] = [
    '#type' => 'item',
    '#title' => 'Current Term',
    '#markup' => theme_table($table),
  ];

  // If this field mapping is fixed then don't let the user change it.
  if (!array_key_exists('term_fixed', $instance['settings']) or $instance['settings']['term_fixed'] != TRUE) {
    $element['field_term']['new_name'] = [
      '#type' => 'textfield',
      '#title' => 'Change the term',
      // TODO: This autocomplete path should not use Chado.
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
    ];
    $element['field_term']['select_button'] = [
      '#type' => 'button',
      '#value' => t('Lookup Term'),
      '#name' => 'select_cvterm',
      '#ajax' => [
        'callback' => "tripal_fields_select_term_form_ajax_callback",
        'wrapper' => "tripal-field-term-fieldset",
        'effect' => 'fade',
        'method' => 'replace',
      ],
    ];
  }

  // If a new term name has been specified by the user then give some extra
  // fields to clarify the term.
  $term_name = '';
  if (array_key_exists('values', $form_state) and array_key_exists('new_name', $form_state['values'])) {
    $term_name = array_key_exists('values', $form_state) ? $form_state['values']['new_name'] : '';
  }
  if (array_key_exists('input', $form_state) and array_key_exists('new_name', $form_state['input'])) {
    $term_name = array_key_exists('input', $form_state) ? $form_state['input']['new_name'] : '';
  }
  if ($term_name) {
    $element['field_term']['instructions'] = [
      '#type' => 'item',
      '#title' => 'Matching terms',
      '#markup' => t('Please select the term the best matches the
        content type you want to associate with this field. If the same term exists in
        multiple vocabularies you will see more than one option below.'),
    ];
    $match = [
      'name' => $term_name,
    ];
    $terms = chado_generate_var('cvterm', $match, ['return_array' => TRUE]);
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = 0;
    foreach ($terms as $term) {
      // Save the user a click, by setting the default value as 1 if there's
      // only one matching term.
      $default = FALSE;
      $attrs = [];
      if ($num_terms == 0 and count($terms) == 1) {
        $default = TRUE;
        $attrs = ['checked' => 'checked'];
      }
      $element['field_term']['term-' . $term->cvterm_id] = [
        '#type' => 'checkbox',
        '#title' => $term->name,
        '#default_value' => $default,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
          '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
          '<br><b>Definition:</b>  ' . $term->definition,
      ];
      $num_terms++;
    }
    if ($num_terms == 0) {
      $element['field_term']['none'] = [
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>',
      ];
    }
  }
  $element['#element_validate'][] = 'tripal_field_instance_settings_form_alter_validate';
  return $element;
}

/**
 * Implements an AJAX callback for the tripal_field_vocab_select_term_form.
 */
function tripal_fields_select_term_form_ajax_callback($form, $form_state) {
  return $form['field_term'];
}

/**
 * Validate our custom instance settings form fields.
 */
function tripal_field_instance_settings_form_alter_validate($form, &$form_state) {

  // If the user clicked the submit button then we want set the
  // instance settings values accordingly.

  if (array_key_exists('clicked_button', $form_state) and $form_state['clicked_button']['#executes_submit_callback'] == TRUE) {
    $has_default = FALSE;
    if ($form_state['values']['term_vocabulary']) {
      $form_state['values']['instance']['settings']['term_vocabulary'] = $form_state['values']['term_vocabulary'];
      $form_state['values']['instance']['settings']['term_accession'] = $form_state['values']['term_accession'];
      $form_state['values']['instance']['settings']['term_name'] = $form_state['values']['term_name'];
      $has_default = TRUE;
    }

    $started_new_term = FALSE;
    if (array_key_exists('new_name', $form_state['values']) and $form_state['values']['new_name']) {
      $started_new_term = TRUE;
    }

    $num_selected = 0;
    $selected_term = FALSE;
    foreach ($form_state['input'] as $key => $value) {

      $matches = [];

      if (preg_match("/^term-(\d+)$/", $key, $matches) and
        $form_state['input']['term-' . $matches[1]]) {
        $cvterm_id = $matches[1];
        // TODO: this should not call a Chado function, but the autocomplete
        // currently uses chado cvterm IDs.
        $term = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
        if ($term) {
          $form_state['values']['instance']['settings']['term_vocabulary'] = $term->dbxref_id->db_id->name;
          $form_state['values']['instance']['settings']['term_accession'] = $term->dbxref_id->accession;
          $form_state['values']['instance']['settings']['term_name'] = $term->name;
          $selected_term = TRUE;
          $num_selected++;
          $has_default = TRUE;
        }
      }
    }

    // Make sure this term is not already used.
    $bundle_name = $form_state['values']['instance']['bundle'];
    $existing_instances = field_info_instances('TripalEntity', $bundle_name);
    $field_term_id = $form_state['values']['instance']['settings']['term_vocabulary'] . ':' . $form_state['values']['instance']['settings']['term_accession'];
    $field_name = $form_state['values']['instance']['field_name'];
    foreach ($existing_instances as $existing_name => $existing_instance) {
      $existing_term_id = $existing_instance['settings']['term_vocabulary'] . ':' . $existing_instance['settings']['term_accession'];
      if ($existing_term_id == $field_term_id and $field_name != $existing_name) {
        form_set_error('term-', t('The term, !term, is already in use on this content type.  A term can only be used once per content type. Please choose a different term.',
          ['!term' => $field_term_id]));
      }
    }

    if ($num_selected > 1) {
      form_set_error('term-', 'Please select only one term
          from the "Controlled Vocabulary Term" section below.');
    }
    if ($started_new_term and !$selected_term) {
      form_set_error('term-', 'Please select a controlled vocabulary term for
          from the "Controlled Vocabulary Term" section below.');
    }
    if (!$has_default) {
      form_set_error('new_name', 'Fields attached to this content type must ' .
        'be associated with a controlled vocabulary term. Please ' .
        'provide one below.');
    }
  }
}

/**
 * Custom submit function for instance settings form.
 */
function tripal_field_instance_settings_form_submit($form, &$form_state) {

}

/**
 *
 */
function tripal_field_widget_form_validate($element, &$form_state, $form) {
  $field = $element['#field'];
  $instance = $element['#instance'];

  $langcode = (isset($element['#language'])) ? $element['#language'] : LANGUAGE_NONE;
  $delta = (isset($element['#delta'])) ? $element['#delta'] : 0;

  $widget_class = $instance['widget']['type'];
  tripal_load_include_field_class($widget_class);
  if (class_exists($widget_class)) {
    $widget = new $widget_class($field, $instance);

    // Set the validation function for this field widget depending on the
    // form displaying the widget.
    if ($form['#form_id'] == 'field_ui_field_edit_form') {
      $widget->validateDefaults($element, $form, $form_state, $langcode, $delta);
    }
    else {
      $widget->validate($element, $form, $form_state, $langcode, $delta);
    }
  }
}


/**
 * Implements hook_field_settings_form_validate().
 *
 * This function is called because the TripalField::settingsForm()
 * adds it to the form element.  By default, Drupal does not provide a
 * validate hook for the settings form.
 */
function tripal_field_settings_form_validate($element, &$form_state, $form) {
  $field = $element['#field'];
  $instance = $element['#instance'];

  $field_class = $field['type'];
  if (tripal_load_include_field_class($field_class)) {
    $field = new $field_class($field, $instance);
    $field->settingsFormValidate($form, $form_state);
  }
}


/**
 * Implements hook_field_formatter_settings_summary().
 */
function tripal_field_formatter_settings_summary($field, $instance, $view_mode) {

  $formatter_class = $instance['display']['default']['type'];
  if (tripal_load_include_field_class($formatter_class)) {
    $formatter = new $formatter_class($field, $instance);
    return $formatter->settingsSummary($view_mode);
  }
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function tripal_field_formatter_settings_form($field, $instance,
                                              $view_mode, $form, &$form_state) {

  $formatter_class = $instance['display']['default']['type'];
  if (tripal_load_include_field_class($formatter_class)) {
    $formatter = new $formatter_class($field, $instance);
    $elements = $formatter->settingsForm($view_mode, $form, $form_state);
  }

  return $elements;
}


/**
 * Implements hook_field_widget_form().
 */
function tripal_field_widget_form(&$form, &$form_state, $field,
                                  $instance, $langcode, $items, $delta, $element) {

  $widget_class = $instance['widget']['type'];
  tripal_load_include_field_class($widget_class);
  if (class_exists($widget_class)) {
    $widget = new $widget_class($field, $instance);
    $widget->form($element, $form, $form_state, $langcode, $items, $delta, $element);
  }
  return $element;
}


/**
 * Implements hook_field_widget_form_alter().
 */
function tripal_field_widget_form_alter(&$element, &$form_state, $context) {

  if (array_key_exists('#field_name', $element)) {
    $field_name = $element['#field_name'];
    $matches = [];

    if (preg_match('/(.+?)__(.+?)$/', $field_name, $matches)) {
      $tablename = $matches[1];
      $colname = $matches[2];
      $schema = chado_get_schema($tablename);
      if (!$schema) {
        return;
      }

      // The timelastmodified field exists in many Chado tables.  We want
      // the form element to update to the most recent time rather than the time
      // in the database.
      if ($colname == 'timelastmodified' and $schema['fields'][$colname]['type'] == 'datetime') {
        // We want the default value for the field to be the current time.
        $element['#default_value']['value'] = format_date(time(), 'custom', "Y-m-d H:i:s", 'UTC');
        $element['#date_items']['value'] = $element['#default_value']['value'];
      }
      // We want the date combo fieldset to be collaspible so we will
      // add our own theme_wrapper to replace the one added by the date
      // module.
      if (array_key_exists($colname, $schema['fields']) and $schema['fields'][$colname]['type'] == 'datetime') {
        $element['#theme_wrappers'] = ['tripal_chado_date_combo'];
      }
    }
  }
}

/**
 * Implements hook_field_validate()
 */
function tripal_field_validate($entity_type, $entity, $field, $instance,
                               $langcode, $items, &$errors) {

  $field_type = $field['type'];
  if (tripal_load_include_field_class($field_type)) {
    $tfield = new $field_type($field, $instance);
    $tfield->validate($entity_type, $entity, $langcode, $items, $errors);
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_display_overview_form is used for formatting the display
 * or layout of fields attached to an entity and shown on the entity view page.
 *
 * This function removes the cvterm class and property adder field as those are
 * really not meant for users to show or manage.
 */
function tripal_form_field_ui_display_overview_form_alter(&$form, &$form_state, $form_id) {
  // Remove the kvproperty_addr field as it isn't ever displayed. It's just used
  // on the add/edit form of an entity for adding new property fields.
  $fields_names = element_children($form['fields']);
  foreach ($fields_names as $field_name) {
    $field_info = field_info_field($field_name);
    if ($field_info['type'] == 'kvproperty_adder') {
      unset($form['fields'][$field_name]);
    }
    if ($field_info['type'] == 'cvterm_class_adder') {
      unset($form['fields'][$field_name]);
    }
  }
}

/**
 * Theme function for all TripalFieldWidget objects.
 *
 * @param $variables
 */
function theme_tripal_field_default($variables) {
  $element = $variables['element'];
  $field = $element['#field'];
  $instance = $element['#instance'];
  $widget_class = $element['#field_name'] . '_widget';
  $langcode = $element['#language'];
  $delta = $element['#delta'];
  tripal_load_include_field_class($widget_class);
  if (class_exists($widget_class)) {
    $widget = new $widget_class($field, $instance);
    return $widget->theme($element);
  }
}
