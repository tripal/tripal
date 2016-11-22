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

//   $info = array();
//   $field_types = tripal_get_field_types();
//   foreach ($field_types as $field_type) {
//     $info[$field_type] = $field_type::globalInfo();
//   }
//   return $info;

  return array(
    'tripal_key_value' => array(
      'label' => 'Tripal complex field',
      'description' => 'A field specific to data managed by Tripal. ',
      'settings' => array(
        'tripal_term' => '',
      ),
      'instance_settings' => array(),
      'default_widget' => 'tripal_generic_key_value_widget',
      'default_formatter' => 'tripal_generic_key_value_formatter',
      'storage' => array(
        'type' => 'tripal_no_storage',
        'module' => 'tripal',
        'active' => TRUE
      ),
    ),
  );
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
//   $info = array();

//   $field_types = tripal_get_field_types();
//   foreach ($field_types as $field_type) {
//     $info += $field_type::widgetInfo();
//   }
//   return $info;
  return array(
    'tripal_generic_key_value_widget' => array(
      'label' => 'Generic key/value',
      'field types' => array('tripal_key_value')
    ),
  );
}

/**
 * Implements hook_field_formatter_info().
 */
function tripal_field_formatter_info() {
//   $info = array();

//   $field_types = tripal_get_field_types();
//   foreach ($field_types as $field_type) {
//     $info += $field_type::formatterInfo();
//   }
//   return $info;
  return array(
    'tripal_generic_key_value_formatter' => array(
      'label' => 'Values',
      'field types' => array('tripal_key_value'),
      'settings' => array(),
    ),
  );
}

/**
 *
 */
function tripal_create_tripalfields($entity_type, $bundle) {
  $field_name = 'content_type';
  $info = array();
  $info[$field_name] = array(
    'field_name' => $field_name,
    'type' => 'content_type',
    'cardinality' => 1,
    'locked' => FALSE,
    'storage' => array(
      'type' => 'tripal_no_storage'
    ),
    'settings' => array(
      'semantic_web' => 'rdfs:type',
    ),
  );
  return $info;
}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 */
function tripal_create_tripalfield_instance($entity_type, $bundle) {
  $field_name = 'content_type';
  $info = array();
  $info[$field_name] = array(
    'field_name' => $field_name,
    'entity_type' => 'TripalEntity',
    'bundle' => $bundle->name,
    'label' => 'Resource Type',
    'description' => '',
    'required' => FALSE,
    'settings' => array(
      'auto_attach' => TRUE,
    ),
    'widget' => array(
      'type' => 'tripal_content_type_widget',
      'settings' => array(
        'display_label' => 1,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'type' => 'tripal_content_type_formatter',
        'settings' => array(),
      ),
    ),
  );
  return $info;
}
/**
 * Implements hook_field_formatter_view().
 */
function tripal_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

//   $element = array();
//   $field_type = $field['type'];
//   $is_loaded = tripal_load_include_field_type($field_type);
//   if ($is_loaded) {
//     $tfield = new $field_type($field, $instance);
//     $tfield->formatterView($element, $entity_type, $entity, $langcode, $items, $display);
//   }
//   return $element;
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
  //dpm($form);

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
    $term = $field['settings']['tripal_term'] ? $field['settings']['tripal_term'] : 'N/A';
    $form['fields'][$field_name][] = array(
      '#markup' => $term,
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
//   //$is_loaded = tripal_load_include_field_type($field_type);
//   tripal_load_include_field_type($field_type);
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
 * @param $form
 *   The form array.  Alterations to the form can be made within this array.
 * @param $form_state
 *   The form state array.
 */
function tripal_field_instance_settings_form_alter(&$form, $form_state) {
  $field = $form['#field'];
  $instance = $form['#instance'];

  $form['tripal_additions'] = array(
    '#type' => 'fieldset',
    '#title' => 'Tripal Settings',
  );
  $form['tripal_additions']['semantic_web'] = array(
    '#type' => 'textfield',
    '#title' => 'Vocabulary Term'
  );
  $form['tripal_additions']['storage'] = array(
    '#type' => 'textfield',
    '#title' => 'Storage Backend'
  );
}
/**
 * Implements hook_instance_settings_form()
 */
function tripal_field_instance_settings_form($field, $instance) {
//   $form = array();
//   $field_type = $field['type'];
//   tripal_load_include_field_type($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->instanceSettingsForm();
//   }
//   return $form;
}
/**
 *
 */
function tripal_field_instance_settings_form_validate($form, &$form_state) {
//   $field = $form['#field'];
//   $instance = $form['#instance'];
//   $field_type = $field['type'];
//   tripal_load_include_field_type($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $form = $tfield->instanceSettingsFormValidate($form, $form_state);
//   }
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
//   tripal_load_include_field_type($field_type);
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
//   tripal_load_include_field_type($field_type);
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
//   tripal_load_include_field_type($field_type);
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
//   tripal_load_include_field_type($field_type);
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
//   $widget = $element;
//   $field_type = $field['type'];
//   tripal_load_include_field_type($field_type);
//   if (class_exists($field_type)) {
//     $tfield = new $field_type($field, $instance);
//     $tfield->widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);
//   }
//   return $widget;


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
//   $is_loaded = tripal_load_include_field_type($field_type);
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
