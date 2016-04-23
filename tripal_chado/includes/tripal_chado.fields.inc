<?php

/**
 * Implements hook_field_info().
 *
 * This function would normally provide a large info array for all of the
 * fields provided by this module.  But instead it will call a hook that
 * can be implmented within each individual field file.  This will allow
 * all of the code for a single field to be self contained in a single file.
 *
 * New fields can be added automatically by including a new file in the
 * tripal_chado/includes/fields directory.  The file must be named with a
 * 'chado_' prefix and end with a '.inc' suffix.  After adding the file,
 * the cache must be cleared.
 *
 */
function tripal_chado_field_info() {
  $info = array();

  // Find all of the files in the tripal_chado/includes/fields directory.
  $fields_path = drupal_get_path('module', 'tripal_chado') . '/includes/fields';
  $field_files = file_scan_directory($fields_path, '/^chado_.*\.inc$/');

  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    $field_name = $file->name;
    module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_name);
    $function = $field_name . '_info';
    if (function_exists($function)) {
      $info[$field_name] = $function();
    }
  }
  return $info;

}
/**
 * Implements hook_field_widget_info().
 *
 * This function would normally provide a large info array for all of the
 * widgets provided by this module.  But instead it will call a hook that
 * can be implmented within each individual field file.  This will allow
 * all of the code for a single field to be self contained in a single file.
 */
function tripal_chado_field_widget_info() {
  $widgets = array();

  $fields = field_info_fields();
  foreach ($fields as $field) {
    $field_type = $field['type'];
    if ($field['storage']['type'] == 'field_chado_storage') {
      module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
      $function = $field_type . '_widget_info';
      if (function_exists($function)) {
        $widgets[$field_type . '_widget'] = $function();
      }
    }
  }
  return $widgets;
}
/**
 * Implements hook_field_formatter_info().
 *
 * This function would normally provide a large info array for all of the
 * formatters provided by this module.  But instead it will call a hook that
 * can be implmented within each individual field file.  This will allow
 * all of the code for a single field to be self contained in a single file.
 */
function tripal_chado_field_formatter_info() {
  $formatters = array();

  $fields = field_info_fields();
  foreach ($fields as $field) {
    $field_type = $field['type'];
    if ($field['storage']['type'] == 'field_chado_storage') {
      module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
      $function = $field_type . '_formatter_info';
      if (function_exists($function)) {
        $formatters[$field_type . '_formatter'] = $function();
      }
    }
  }
  return $formatters;
}
/**
 * Implements hook_field_formatter_settings_summary().
 */
function tripal_chado_field_formatter_settings_summary($field, $instance, $view_mode) {

  $summary = '';
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  $function = $field_type . '_formatter_settings_summary';
  if (function_exists($function)) {
    $summary = $function($field, $instance, $view_mode);
  }

  return $summary;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function tripal_chado_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $element = array();

  $field_type = $field['type'];
  form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/' . $field_type);
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  $function = $field_type . '_formatter_settings_form';
  if (function_exists($function)) {
    $element = $function($field, $instance, $view_mode, $form, $form_state);
  }

  return $element;
}



/**
 * Implements hook_field_formatter_view().
 */
function tripal_chado_field_formatter_view($entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $element = array();
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
  $function = $display['type'];
  if (function_exists($function)) {
    $function($element, $entity_type, $entity, $field, $instance, $langcode, $items, $display);
  }

  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_chado_field_widget_form(&$form, &$form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $widget = $element;

  $field_name = $instance['field_name'];
  $field_type = $field['type'];
  form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/' . $field_type);
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_name);
  $function = $field_type . '_widget';
  if (function_exists($function)) {
    $function($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  }

  return $widget;
}

/**
 * Returns the values of the field from the $form_state.
 */
function tripal_chado_get_field_form_values($field_name, $form_state, $delta = 0, $child = NULL) {
  $value = NULL;
  // The form_state must have the 'values' key. If not then just return.
  if (!array_key_exists('values', $form_state)) {
    return $value;
  }

  // If the field name is not in the form_state['values'] then return.
  if (!array_key_exists($field_name, $form_state['values'])) {
    return $value;
  }

  // Iterate through the values looking for the field_name provided.
  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    if (!array_key_exists($delta, $items)) {
      continue;
    }
    $item = $items[$delta];
    if ($child){
      if(array_key_exists($child, $item) and $item[$child] != '') {
        $value = $item[$child];
      }
    }
    else {
      $value = $item['value'];
    }
  }
  return $value;
}

/**
 * Sets the values of the field from the $form_state.
 */
function tripal_chado_set_field_form_values($field_name, &$form_state, $newvalue, $delta = 0, $child = NULL) {
  // The form_state must have the 'values' key. If not then just return.
  if (!array_key_exists('values', $form_state)) {
    return FALSE;
  }

  // If the field name is not in the form_state['values'] then reutrn.
  if (!array_key_exists($field_name, $form_state['values'])) {
    return FALSE;
  }

  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    if ($child) {
      $form_state['values'][$field_name][$langcode][$delta][$child] = $newvalue;
    }
    else {
      $form_state['values'][$field_name][$langcode][$delta]['value'] = $newvalue;
    }
  }
  return TRUE;
}

/**
 * Implements hook_field_widget_form_alter().
 */
function tripal_chado_field_widget_form_alter(&$element, &$form_state, $context) {

  if (array_key_exists('#field_name', $element)) {
    $field_name = $element['#field_name'];
    $matches = array();

    if (preg_match('/(.+?)__(.+?)$/', $field_name, $matches)) {
      $tablename = $matches[1];
      $colname = $matches[2];
      $schema = chado_get_schema($tablename);

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
 * Returns a $field_info array for a field based on a database column.
 *
 */
function tripal_chado_add_bundle_fields_base__fields_defaults($table_name, $schema, $column_name) {
  $details = $schema['fields'][$column_name];

  // Create an array with information about this field.
  $field = array(
    'field_type' => '',
    'widget_type' => '',
    'description' => '',
    'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
    'is_required' => 0,
    'storage' => 'field_chado_storage',
    'widget_settings' => array(
      'display_label' => 1
    ),
    'field_settings' => array(
      // The table in Chado where this field maps to.
      'chado_table' => $table_name,
      // The column in the Chado table that this field maps to.
      'chado_column' => $column_name,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  // Alter the field info array depending on the column details.
  switch($details['type']) {
    case 'char':
      $field['field_type'] = 'text';
      $field['widget_type'] = 'text_textfield';
      $field['field_settings']['max_length'] = $details['length'];
      break;
    case 'varchar':
      $field['field_type'] = 'text';
      $field['widget_type'] = 'text_textfield';
      $field['field_settings']['max_length'] = $details['length'];
      break;
    case 'text':
      $field['field_type'] = 'text';
      $field['widget_type'] = 'text_textarea';
      $field['field_settings']['max_length'] = 17179869184;
      $field['field_settings']['text_processing'] = 1;
      $field['format'] = filter_default_format();
      break;
    case 'blob':
      // not sure how to support a blob field.
      continue;
      break;
    case 'int':
      $field['field_type'] = 'number_integer';
      $field['widget_type'] = 'number';
      break;
    case 'float':
      $field['field_type'] = 'number_float';
      $field['widget_type'] = 'number';
      $field['field_settings']['precision'] = 10;
      $field['field_settings']['scale'] = 2;
      $field['field_settings']['decimal_separator'] = '.';
      break;
    case 'numeric':
      $field['field_type'] = 'number_decimal';
      $field['widget_type'] = 'number';
      break;
    case 'serial':
      // Serial fields are most likely not needed as a field.
      break;
    case 'boolean':
      $field['field_type'] = 'list_boolean';
      $field['widget_type'] = 'options_onoff';
      $field['field_settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
      break;
    case 'datetime':
      // Use the Drupal Date and Date API to create the field/widget
      $field['field_type'] = 'datetime';
      $field['widget_type'] = 'date_select';
      $field['widget_settings']['increment'] = 1;
      $field['widget_settings']['tz_handling'] = 'none';
      $field['widget_settings']['collapsible'] = TRUE;

      // TODO: Add settings so that the minutes increment by 1.
      // And turn off the timezone, as the Chado field doesn't support it.
      break;
  }

  // Set some default semantic web information
  if ($column_name == 'name') {
    $field['field_settings']['semantic_web']['type'] = 'name';
    $field['field_settings']['semantic_web']['ns'] = 'foaf';
    $field['field_settings']['semantic_web']['nsurl'] = 'http://xmlns.com/foaf/0.1/';
  }
  if ($column_name == 'description' or $column_name == 'definition' or
      $column_name == 'comment') {
    $field['field_settings']['semantic_web']['type'] = 'description';
    $field['field_settings']['semantic_web']['ns'] = 'hydra';
    $field['field_settings']['semantic_web']['nsurl'] = 'http://www.w3.org/ns/hydra/core#';
  }

  //
  // GENERIC COLUMNS
  //
//   if ($field['field_settings']['chado_column'] == 'organism_id') {
//     $field['field_type'] = 'chado_base__organism_id';
//     $field['widget_type'] = 'chado_base__organism_id_widget';
//     $field['label'] = 'Organism';
//     $field['description'] = 'Select an organism.';
//     $field['field_settings']['semantic_web']['type'] = 'organism';
//     $field['field_settings']['semantic_web']['ns'] = 'local';
//     $field['field_settings']['semantic_web']['nsurl'] = '';

//   }
//   elseif ($field['field_settings']['chado_column'] =='dbxref_id') {
//     $field['field_type'] = 'chado_base__dbxref_id';
//     $field['widget_type'] = 'chado_base_dbxref_id_widget';
//     $field['label'] = 'Cross Reference';
//     $field['description'] = 'This record can be cross referenced with a ' .
//         'record in another online database. The primary reference is for the ' .
//         'most prominent reference.  At a minimum, the database and accession ' .
//         'must be provided.  To remove a set reference, change the database ' .
//         'field to "Select a Database".';
//   }
  if ($field['label'] == 'Timeaccessioned') {
    $field['label'] = 'Time Accessioned';
    $field['description'] = 'Please enter the time that this record was first added to the database.';
  }
  elseif ($field['label'] == 'Timelastmodified') {
    $field['label'] = 'Time Last Modified';
    $field['description'] = 'Please enter the time that this record was last modified. The default is the current time.';
  }
  //
  // ORGANISM TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'organism' and $field['field_settings']['chado_column'] == 'comment') {
    $field['label'] = 'Description';
  }
  //
  // FEATURE TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'uniquename') {
    $field['field_type'] = 'text';
    $field['widget_type'] = 'text_textfield';
    $field['field_settings']['text_processing'] = 0;
    $field['field_settings']['semantic_web']['type'] = 'name';
    $field['field_settings']['semantic_web']['ns'] = 'foaf';
    $field['field_settings']['semantic_web']['nsurl'] = 'http://xmlns.com/foaf/0.1/';
  }
  /* elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'md5checksum') {
    $field['field_type'] = 'chado_feature__md5checksum';
    $field['widget_type'] = 'chado_feature__md5checksum_widget';
    $field['label'] = 'MD5 Checksum';
    $field['description'] = 'Generate an MD5 checksum for the sequence.';
  }
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'seqlen') {
    $field['field_type'] = 'chado_feature__seqlen';
    $field['widget_type'] = 'chado_feature__seqlen_widget';
    $field['label'] = 'Seqlen';
    $field['description'] = 'The length of the residues.';
  }
  elseif ($field['field_settings']['chado_table'] == 'feature' and $field['field_settings']['chado_column'] == 'residues') {
    $field['field_type'] = 'chado_feature__residues';
    $field['widget_type'] = 'chado_feature__residues_widget';
    $field['label'] = 'Residues';
    $field['description'] = 'Please provide an IUPAC compatible residues for this feature. Spaces and new lines are allowed.';
  } */
  //
  // ANALYSIS TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'program') {
    $field['field_settings']['semantic_web']['type'] = 'SoftwareApplication';
    $field['field_settings']['semantic_web']['ns'] = 'schema';
    $field['field_settings']['semantic_web']['nsurl'] = 'https://schema.org/';
    $field['description'] = 'The program name (e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package then provide a very brief description of the pipeline, workflow or method.';
    $field['label'] = 'Program, Pipeline, Workflow or Method Name.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'sourceuri') {
    $field['field_type'] = 'text';
    $field['widget_type'] = 'text_textfield';
    $field['field_settings']['text_processing'] = 0;
    $field['label'] = 'Source URL';
    $field['description'] = 'The URL where the original source data was derived.  Ideally, this should link to the page where more information about the source data can be found.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'sourcename') {
    $field['label'] = 'Source Name';
    $field['description'] = 'The name of the source data. This could be a file name, data set or a small description for how the data was collected. For long descriptions use the larger description field.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'sourceversion') {
    $field['label'] = 'Source Version';
    $field['description'] = 'If hte source data set has a version include it here.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'algorithm') {
    $field['label'] = 'Source Version';
    $field['description'] = 'The name of the algorithm used to produce the dataset if different from the program.';
  }
  elseif ($field['field_settings']['chado_table'] == 'analysis' and $field['field_settings']['chado_column'] == 'programversion') {
    $field['label'] = 'Program Version';
    $field['description'] = 'The version of the program used to perform this analysis. (e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter "n/a" if no version is available or applicable.';
  }
  //
  // PROJECT TABLE
  //
  elseif ($field['field_settings']['chado_table'] == 'project' and $field['field_settings']['chado_column'] == 'description') {
    $field['label'] = 'Short Description';
  }
  return $field;
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
function tripal_chado_form_field_ui_display_overview_form_alter(&$form, &$form_state, $form_id) {
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
 * Implements hook_form_FORM_ID_alter().
 *
 * The field_ui_field_overview_form is used for ordering and configuring the
 * fields attached to an entity.
 *
 * This function removes the property adder field as that is really not meant
 * for users to show or manage.
 */
function tripal_chado_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {
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
function tripal_chado_field_is_empty($item, $field) {

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



/**
 * Implements hook_add_bundle_fields().
 */
function tripal_chado_add_bundle_fields($entity_type, $bundle, $term) {

  $bundle_name = $bundle->name;

  // This array will hold details that map the bundle to tables in Chado.
  $bundle_data = array();

  // Get the cvterm that corresponds to this TripalTerm object.
  $vocab = entity_load('TripalVocab', array($term->vocab_id));
  $vocab = reset($vocab);
  $match = array(
    'dbxref_id' => array(
      'db_id' => array(
        'name' => $vocab->namespace,
      ),
      'accession' => $term->accession
    ),
  );
  $cvterm = chado_generate_var('cvterm', $match);

  // The organism table does not have a type_id so we won't ever find
  // a record for it in the tripal_cv_defaults table.
  if ($cvterm->name == 'organism') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'organism',
      'type_table' => 'organism',
      'field' =>  '',
    );
  }
  // The analysis table does not have a type_id so we won't ever find
  // a record for it in the tripalcv_defaults table.
  else if ($cvterm->name == 'analysis') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'analysis',
      'type_table' => 'analysis',
      'field' =>  '',
    );
  }
  else if ($cvterm->name == 'project') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'project',
      'type_table' => 'project',
      'field' =>  '',
    );
  }
  else {
    // TODO: WHAT TO DO IF A VOCABULARY IS USED AS A DEFAULT FOR MULTIPLE
    // TABLES.
    // Look to see if this vocabulary is used as a default for any table.
    $default = db_select('tripal_cv_defaults', 't')
      ->fields('t')
      ->condition('cv_id', $cvterm->cv_id->cv_id)
      ->execute()
      ->fetchObject();
    if ($default) {
      $bundle_data = array(
        'cv_id' => $cvterm->cv_id->cv_id,
        'cvterm_id' => $cvterm->cvterm_id,
        'data_table' => $default->table_name,
        'type_table' => $default->table_name,
        'field' =>  $default->field_name,
      );
    }
  }

  // Save the mapping information so that we can reuse it when we need to
  // look things up for later for an entity
  tripal_set_bundle_variable('chado_cvterm_id', $bundle->id, $bundle_data['cvterm_id']);
  tripal_set_bundle_variable('chado_table', $bundle->id, $bundle_data['data_table']);
  tripal_set_bundle_variable('chado_column', $bundle->id, $bundle_data['field']);



  // Call the hook_attach_info() for all Chado fields to see if any of them
  // want to attach themsevles to this bundle.
  $fields = field_info_fields();
  foreach ($fields as $field) {
    $field_type = $field['type'];
    if ($field['storage']['type'] == 'field_chado_storage') {
      module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
      $function = $field_type . '_attach_info';

      if (function_exists($function)) {
        // Get the field info.
        $field_info = $function($entity_type, $bundle, $bundle_data);
        if (!is_array($field_info) or count(array_keys($field_info)) == 0) {
          continue;
        }

        $field_name = $field_info['field_name'];
        tripal_add_bundle_field($field_name, $field_info, $entity_type, $bundle_name);
      }
    }
  }

  // Adds any remaining base fields that may not have been dealt with
  // by a custom field.
  //tripal_chado_add_bundle_fields_base__fields($entity_type, $bundle_name, $bundle_data);


}

/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_linker__dbxref_field($entity_type_name, $bundle_name, $dbxref_table, $base_table) {
  // We already have a dbxref_id field.
  $field_name = $dbxref_table;
  $schema = chado_get_schema($dbxref_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__dbxref',
    'widget_type' => 'chado_linker__dbxref_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Cross References',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $dbxref_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  // If the base table has a 'dbxref_id' then change the label to
  // indicate these are secondary cross references.
  $schema = chado_get_schema($base_table);
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_info['label'] = 'Secondary Cross References';
  }
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_linker__synonym_field($entity_type_name, $bundle_name, $syn_table, $base_table) {
  // We already have a dbxref_id field.
  $field_name = $syn_table;
  $schema = chado_get_schema($syn_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__synonym',
    'widget_type' => 'chado_linker__synonym_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Synonyms',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $syn_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_linker__featureloc_field($entity_type_name, $bundle_name) {
  $field_name = 'featureloc';
  $schema = chado_get_schema('featureloc');
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__featureloc',
    'widget_type' => 'chado_linker__featureloc_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Alignments',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => 'featureloc',
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => 'feature',
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing relationships that are stored in a [base]_relationship table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_linker__relationship_field(
    $entity_type_name, $bundle_name, $rel_table, $base_table) {

  $field_name = $rel_table;
  $schema = chado_get_schema($rel_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__relationship',
    'widget_type' => 'chado_linker__relationship_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Relationsihps',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $rel_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing xrefs that are stored in a [base]_dbxref table.
 *
 * @param $entity_type
 * @param $bundle_name
 * @param $base_table
 * @param $dbxref_table
 */
function tripal_chado_add_bundle_fields_linker__pub_field($entity_type_name, $bundle_name, $pub_table, $base_table) {
  // We already have a dbxref_id field.
  $field_name = $pub_table;
  $schema = chado_get_schema($pub_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__pub',
    'widget_type' => 'chado_linker__pub_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Publications',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      // The Chado table that this field maps to.
      'chado_table' => $pub_table,
      // The column in the chado table that this field maps to.
      'chado_column' => $pkey,
      // The base table that this field is connected to.
      'base_table' => $base_table,
      'semantic_web' => array(
        // The type is the term from a vocabulary that desribes this field..
        'type' => '',
        // The namepsace for the vocabulary (e.g. 'foaf').
        'ns' => '',
        // The URL for the namespace.  It must be that the type can be
        // appended to the URL.
        'nsurl' => '',
      ),
    ),
  );

  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing properties that are stored in a prop table.
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $kv_table
 */
function tripal_chado_add_bundle_fields_linker__prop_adder_field($entity_type_name, $bundle_name, $kv_table, $base_table) {
  $field_name = $kv_table;

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__prop_adder',
    'widget_type' => 'chado_linker__prop_adder_widget',
    'field_settings' => array(
      'base_table' => $base_table,
    ),
    'storage' => 'field_chado_storage',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Properties',
    'is_required' => 0,
  );
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for managing properties that are stored in a prop table.
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $kv_table
 */
function tripal_chado_add_bundle_fields_linker__cvterm_adder_field($entity_type_name, $bundle_name, $cvterm_table, $base_table) {
  // First add a generic property field so that users can add new property types.
  $field_name = $cvterm_table;

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'chado_linker__cvterm_adder',
    'widget_type' => 'chado_linker__cvterm_adder_widget',
    'field_settings' => array(
      'base_table' => $base_table,
    ),
    'storage' => 'field_chado_storage',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Annotation Types',
    'is_required' => 0,
  );
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}
/**
 * Adds the fields for the base table to the entity.
 */
function tripal_chado_add_bundle_fields_base__fields($entity_type_name, $bundle_name, $bundle_data) {

  $table_name = $bundle_data['data_table'];
  $type_table = $bundle_data['type_table'];
  $type_field = $bundle_data['field'];

  // Iterate through the columns of the table and see if fields have been
  // created for each one. If not, then create them.
  $schema = chado_get_schema($table_name);
  $columns = $schema['fields'];
  foreach ($columns as $column_name => $details) {
    $field_name = $table_name . '__' . $column_name;

    // Skip the primary key field.
    if ($column_name == $schema['primary key'][0]) {
      continue;
    }

    // Skip the type field.
    if ($table_name == $type_table and $column_name == $type_field) {
      continue;
    }

    // Get the field defaults for this column.
    $field_info = tripal_chado_add_bundle_fields_base__fields_defaults($table_name, $schema, $column_name);


    // TODO: add in a call to drupal_alter to allow other modules to change
    // the field settings.

    // Determine if the field is required.
    if (array_key_exists('not null', $details) and $details['not null'] === TRUE) {
      $field_info['is_required'] = array_key_exists('default', $details) ? 0 : 1;
    }

    // If we don't have a field type then we don't need to create a field.
    if (!$field_info['field_type']) {
      // If we don't have a field type but it is required and doesn't have
      // a default value then we are in trouble.
      if ($field_info['is_required'] and !array_key_exists('default', $details)) {
        throw new Exception(t('The %table.%field type, %type, is not yet supported for Entity fields, but it is required,',
            array('%table' => $table_name, '%field' => $column_name, '%type' => $details['type'])));
      }
      continue;
    }

    // If this field is a foreign key field then we will have a custom field.
    $is_fk = FALSE;
    if (array_key_exists('foreign keys', $schema)) {
      foreach ($schema['foreign keys'] as $remote_table => $fk_details) {
        if (array_key_exists($column_name, $fk_details['columns'])) {
          $is_fk = TRUE;
        }
      }
    }

    // Add the field to the bundle.
    tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
  }
}

/**
 * Implements hook_field_validate().
 *
 * This function is used to validate any field. Fields with custom
 * widgets will most likely have their own validate function but for all
 * others we need a way to validate them.
 *
 */
function tripal_chado_field_validate($entity_type, $entity, $field, $instance,
    $langcode, $items, &$errors) {


}


/**
 * Implements hook_field_attach_validate().
 *
 * This function is used to validate entity-wide validation any of the fields
 * attached to an entity.
 */
function tripal_chado_field_attach_validate($entity_type, $entity, &$errors) {
  $bundle_name = $entity->bundle;
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
  $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));

  // Provide some higher-level validation for the organism type.
  if ($term->name == 'organism') {

  }
}











