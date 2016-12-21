<?php

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

  $info = array();
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
  // Make sure all fields have a 'tripal_term' setting so we can map
  // all fields to a vocabulary term for the semantic web.
  foreach ($info as $field_name => $details) {
    if(array_key_exists('settings', $details)) {
      if (!array_key_exists('tripal_term', $details['settings'])) {
        $info[$field_name]['settings']['tripal_term'] = '';
      }
    }
    else {
      $info[$field_name]['settings']['tripal_term'] = '';
    }
  }
}

/**
 * Implements hook_field_widget_info();
 */
function tripal_field_widget_info() {
  $info = array();
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
  $info = array();
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
 * This is a Triapl defined hook and is called in the TripalBundle::create()
 * function to allow modules to perform tasks when a bundle is created.
 */
function tripal_bundle_create($bundle) {
  $field_type = 'rdfs__type';
  $field_name = $bundle->name . '_' . $field_type;

  // Add the field, unless it already exists.
  if (!field_info_field($field_name)) {
    $field = field_create_field(array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'tripal_no_storage'
      ),
      'settings' => array(
      ),
    ));
  }


  // Add an instance of the field to the bundle.
  if (!field_info_instance($bundle->type, $field_name, $bundle->name)) {
    $instance = field_create_instance(array(
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle->name,
      'label' => 'Resource Type',
      'description' => 'The resource type',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'rdfs__type_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'rdfs__type_formatter',
          'settings' => array(),
        ),
      ),
    ));
  }
}

/**
 * Implements hook_field_formatter_view().
 */
function tripal_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

   $element = array();
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

  // Add the 'Storage Location' to the table header.
  $form['fields']['#header'][] = 'Term';
  $form['fields']['#header'][] = 'Supported By * ';

  // TODO: remove widgets that aren't appropriate for this entity, if the
  // type is 'tripal_key_value'.
  // Why is this sort not working!!??
  $options = $form['fields']['_add_new_field']['widget_type']['#options']['Tripal complex field'];
  asort($options);
  $form['fields']['_add_new_field']['widget_type']['#options']['Tripal complex field'] = $options;

  // Add the storage location as the final column for each field.
  $storage_info = module_invoke_all('field_storage_info');
  foreach (element_children($form['fields']) as $field_name) {


    $field = field_info_field($field_name);
    $instance = field_info_instance('TripalEntity', $field_name, $form['#bundle']);
    // For rows in the tables that aren't fields, just add an empty value
    // for the storage column.
    if (!$field) {
      $form['fields'][$field_name][] = array(
        '#markup' => '',
      );
      $form['fields'][$field_name][] = array(
        '#markup' => '',
      );
      continue;
    }
    $term_info = '';
    if (array_key_exists('term_accession', $instance['settings']) and $instance['settings']['term_accession']) {
      $term = tripal_get_term_details($instance['settings']['term_vocabulary'], $instance['settings']['term_accession']);
      $term_info = $term['name'] . ' (' . $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'] . ')';
    }
    $form['fields'][$field_name][] = array(
      '#markup' => $term_info,
    );

    $storage_type = $field['storage']['type'];
    $storage_label = array_key_exists('label', $storage_info[$storage_type]) ? $storage_info[$storage_type]['label'] : '';
    if ($storage_type == 'field_sql_storage') {
      $storage_label = 'Drupal';
    }
    if (array_key_exists('logo_url', $storage_info[$storage_type])) {
      $logo_url = $storage_info[$storage_type]['logo_url'];
      $form['fields'][$field_name][] = array(
        '#markup' => '<img class="form-field-ui-field-overview-storage-logo" src="' . $logo_url . '">',
      );
    }
    else {
      $form['fields'][$field_name][] = array(
        '#markup' => $storage_label,
      );
    }
  }
  $form['note'] = array(
    '#markup' =>  '* Fields attached to this content type can use various
      storage backends. Please be sure when you add new fields that the
      storage backend is appropriate. For example, if you use Chado, and you
      want all biological content to be stored in Chado, be sure that the
      respective fields are "supported by" Chado.',
  );
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
//   $form = array();
//   $field_type = $field['type'];
//   //$is_loaded = tripal_load_include_field_class($field_type);
//   tripal_load_include_field_class($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->globalSettingsForm($field, $instance, $has_data);
//   }
//   return $form;
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
  $form['field_term'][$language->language][0]['#process'] = array('tripal_field_instance_settings_form_process');
  $form['#submit'][] = 'tripal_field_instance_settings_form_submit';
}

/**
 * Implements a process function for the instnace settings form.
 *
 * See the comment in the tripal_field_instance_settings_form_alter() for
 * more details.
 */
function tripal_field_instance_settings_form_process($element, &$form_state, $form) {
  $field = $form['#field'];
  $instance = $form['#instance'];

  // Get the term for this instance.
  $vocabulary = $instance['settings']['term_vocabulary'];
  $accession = $instance['settings']['term_accession'];
  $term_name = $instance['settings']['term_name'];
  $is_fixed = $instance['settings']['term_fixed'];
  $term = tripal_get_term_details($vocabulary, $accession);

  // Construct a table for the vocabulary information.
  $headers = array();
  $rows = array();
  $rows[] = array(
    array(
      'data' => 'Vocabulary',
      'header' => TRUE,
      'width' => '20%',
    ),
    $term['vocabulary']['name'] . ' (' . $term['vocabulary']['short_name'] . ') ' . $term['vocabulary']['description']
  );
  $rows[] = array(
    array(
      'data' => 'Term',
      'header' => TRUE,
      'width' => '20%',
    ),
    $term['name'] . ':' . $term['accession']
  );
  $rows[] = array(
    array(
      'data' => 'Definition',
      'header' => TRUE,
      'width' => '20%',
    ),
    $term['definition']
  );
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  $element['field_term'] = array(
    '#type' => 'fieldset',
    '#title' => 'Controlled Vocabulary Term',
    '#description' => t('All fields attached to a Tripal-based content type must
        be associated with a controlled vocabulary term.  Please use caution
        when changing the term for this field as other sites may expect this term
        when querying web services.'),
    '#prefix' => '<div id = "tripal-field-term-fieldset">',
    '#suffix' => '</div>',
  );
  $element['field_term']['term_vocabulary'] = array(
    '#type' => 'value',
    '#value' => $vocabulary,
  );
  $element['field_term']['term_name'] = array(
    '#type' => 'value',
    '#value' => $term_name,
  );
  $element['field_term']['term_accession'] = array(
    '#type' => 'value',
    '#value' => $accession,
  );
  $element['field_term']['details'] = array(
    '#type' => 'item',
    '#title' => 'Current Term',
    '#markup' => theme_table($table),
  );
  $element['field_term']['new_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Change the term',
    // TODO: This autocomplete path should not use Chado.
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
  );
  $element['field_term']['select_button'] = array(
    '#type' => 'button',
    '#value' => t('Lookup Term'),
    '#name' => 'select_cvterm',
    '#ajax' => array(
      'callback' => "tripal_fields_select_term_form_ajax_callback",
      'wrapper' => "tripal-field-term-fieldset",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['new_name'] : '';
  if ($term_name) {
    $element['field_term']['instructions'] = array(
      '#type' => 'item',
      '#title' => 'Matching terms',
      '#markup' => t('Please select the term the best matches the
        content type you want to associate with this field. If the same term exists in
        multiple vocabularies you will see more than one option below.')
    );
    $match = array(
      'name' => $term_name,
    );
    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = 0;
    foreach ($terms as $term) {
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term.
      $default = FALSE;
      $attrs = array();
      if ($num_terms == 0 and count($terms) == 1) {
        $default = TRUE;
        $attrs = array('checked' => 'checked');
      }
      $element['field_term']['term-' . $term->cvterm_id] = array(
        '#type' => 'checkbox',
        '#title' =>  $term->name,
        '#default_value' => $default,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
        '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
        '<br><b>Definition:</b>  ' . $term->definition,
      );
      $num_terms++;
    }
    if ($num_terms == 0) {
      $element['field_term']['none'] = array(
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>'
      );
    }
  }
  $element['#element_validate'][] = 'tripal_field_instance_settings_form_validate';
  return $element;
}

/**
 * Implements an AJAX callback for the tripal_field_vocab_select_term_form.
 */
function tripal_fields_select_term_form_ajax_callback($form, $form_state) {
  return $form['field_term'];
}
/**
 * Implements hook_instance_settings_form()
 */
function tripal_field_instance_settings_form($field, $instance) {
//   $form = array();
//   $field_type = $field['type'];
//   tripal_load_include_field_class($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->instanceSettingsForm();
//   }
//   return $form;
}
/**
 * Validate our custom instance settings form fields.
 */
function tripal_field_instance_settings_form_validate($form, &$form_state) {
  // If the user clicked the submit button then we want set the
  // instance settings values accordingly.
  foreach ($form_state['input'] as $key => $value) {
    $matches = array();
    if (preg_match("/^term-(\d+)$/", $key, $matches) and
        $form_state['input']['term-' . $matches[1]]) {
      $cvterm_id = $matches[1];
      // TODO: this should not call a Chado function.
      $term = chado_generate_var('cvterm', array('cvterm_id' => $cvterm_id));
      $form_state['values']['instance']['settings']['term_vocabulary'] = $term->dbxref_id->db_id->name;
      $form_state['values']['instance']['settings']['term_accession'] = $term->dbxref_id->accession;
      $form_state['values']['instance']['settings']['term_name'] = $term->name;
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
function tripal_field_widget_form_validate($form, &$form_state) {
//   $entity = $form['#entity'];
//   $entity_type = $form['#entity_type'];
//   $langcode = $form['#language'];
//   $delta = $form['#delta'];
//   $field = $form['#field'];
//   $field_type = $field['type'];
//   tripal_load_include_field_class($field_type);
//   if (class_exists($field_type)) {
//     $instance = $form['#instance'];
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->widgetFormValidate($form, $form_state, $entity_type, $entity, $langcode, $delta);
//   }
}


/**
 * Implements hook_field_settings_form_validate().
 *
 * This is not an actual Drpual hook, but rather a Tripal created hook
 * to alow the TripalField objects to have a globalSettingsFormValidate()
 * member function.
 */
function tripal_field_settings_form_validate($form, &$form_state) {
//   $field = $form['#field'];
//   $instance = $form['#instance'];
//   $field_type = $field['type'];
//   tripal_load_include_field_class($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->globalSettingsFormValidate($field, $instance, $form, $form_state);
//   }
}


/**
 * Implements hook_field_formatter_settings_summary().
 */
function tripal_field_formatter_settings_summary($field, $instance, $view_mode) {

//   $summary = '';
//   $field_type = $field['type'];
//   tripal_load_include_field_class($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->formatterSettingsSummary($view_mode);
//   }
//   return $summary;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function tripal_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {

//   $form = array();
//   $field_type = $field['type'];
//   tripal_load_include_field_class($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->formatterSettingsForm($view_mode, $form, $form_state);
//   }
//   return $form;
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
    $matches = array();

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
        $element['#theme_wrappers'] = array('tripal_chado_date_combo');
      }
    }
  }
}

/**
 * Implements hook_field_validate()
 */
function tripal_field_validate($entity_type, $entity, $field, $instance,
    $langcode, $items, &$errors) {

//   $field_type = $field['type'];
//   $is_loaded = tripal_load_include_field_class($field_type);
//   if ($is_loaded) {
//     $tfield = new $field_type($field, $instance);
//     $tfield->validate($entity_type, $entity, $langcode,
//         $items, $errors);
//   }
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
 * Implements hook_field_is_empty().
 */
function tripal_field_is_empty($item, $field) {

  // If there is no value field then the field is empty.
  if (!array_key_exists('value', $item)) {
    return TRUE;
  }

  // Iterate through all of the fields and if at least one has a value
  // the field is not empty.
  foreach ($item as $form_field_name => $value) {
    if (isset($value) and $value != NULL and $value != '') {
      return FALSE;
    }
  }

  // Otherwise, the field is empty.
  return TRUE;
}
