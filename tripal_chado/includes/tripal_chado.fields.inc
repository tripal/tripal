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
    $field_type = $file->name;
    module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
    if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
      $field_obj = new $field_type();
      $info[$field_type] = $field_obj->field_info();
    }
  }
  return $info;

}

/**
 * Implements hook_field_create_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_field_create_info($entity_type, $bundle, $term) {

  $bundle_name = $bundle->name;

  // The details array is used to pass to the TripalEntity->create_info()
  // function to provide more details about how the bundle is used by this
  // module.
  $details = array();

  // This array will hold details that map the bundle to tables in Chado.
  $bundle_data = array();

  // Get the cvterm that corresponds to this TripalTerm object.
  $vocab = entity_load('TripalVocab', array($term->vocab_id));
  $vocab = reset($vocab);
  $match = array(
    'dbxref_id' => array(
      'db_id' => array(
        'name' => $vocab->vocabulary,
      ),
      'accession' => $term->accession
    ),
  );
  $cvterm = chado_generate_var('cvterm', $match);

  // The organism table does not have a type_id so we won't ever find
  // a record for it in the tripal_cv_defaults table.
  if ($cvterm->name == 'organism') {
    $details = array(
      'chado_cv_id' => $cvterm->cv_id->cv_id,
      'chado_cvterm_id' => $cvterm->cvterm_id,
      'chado_table' => 'organism',
      'chado_type_table' => 'organism',
      'chado_type_column' =>  '',
    );
  }
  // The analysis table does not have a type_id so we won't ever find
  // a record for it in the tripalcv_defaults table.
  else if ($cvterm->name == 'analysis') {
    $details = array(
      'chado_cv_id' => $cvterm->cv_id->cv_id,
      'chado_cvterm_id' => $cvterm->cvterm_id,
      'chado_table' => 'analysis',
      'chado_type_table' => 'analysis',
      'chado_type_column' =>  '',
    );
  }
  else if ($cvterm->name == 'project') {
    $details = array(
      'chado_cv_id' => $cvterm->cv_id->cv_id,
      'chado_cvterm_id' => $cvterm->cvterm_id,
      'chado_table' => 'project',
      'chado_type_table' => 'project',
      'chado_type_column' =>  '',
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
      $details = array(
        'chado_cv_id' => $cvterm->cv_id->cv_id,
        'chado_cvterm_id' => $cvterm->cvterm_id,
        'chado_table' => $default->table_name,
        'chado_type_table' => $default->table_name,
        'chado_type_column' =>  $default->field_name,
      );
    }
  }

  // Save the mapping information so that we can reuse it when we need to
  // look things up for later (such as the hook_create_instance_info() function.
  tripal_set_bundle_variable('chado_cv_id', $bundle->id, $details['chado_cv_id']);
  tripal_set_bundle_variable('chado_cvterm_id', $bundle->id, $details['chado_cvterm_id']);
  tripal_set_bundle_variable('chado_table', $bundle->id, $details['chado_table']);
  tripal_set_bundle_variable('chado_type_table', $bundle->id, $details['chado_type_table']);
  tripal_set_bundle_variable('chado_type_column', $bundle->id, $details['chado_type_column']);

  $base_fields = tripal_chado_field_create_base('create_info', $entity_type, $bundle, $details);
  $custom_fields = tripal_chado_field_create_info_custom($entity_type, $bundle, $details);
  return array_merge($base_fields, $custom_fields);
}

/**
 * A helper function for the tripal_chado_field_create_info() function.
 *
 * This function adds in the custom fields info by instantiating the class
 * for the custom field, calling the create_info() function and
 * returning the info array.
 *
 * @param $entity_type
 *   The type of entity (e.g TripalEntity)
 * @param $bundle
 *   The bundle object.
 * @param $details
 *   An array containing the mapping of the bundle to the Chado table.
 */
function tripal_chado_field_create_info_custom($entity_type, $bundle, $details) {
  $info = array();

  // Find all of the files in the tripal_chado/includes/fields directory.
  $fields_path = drupal_get_path('module', 'tripal_chado') . '/includes/fields';
  $field_files = file_scan_directory($fields_path, '/^chado_.*\.inc$/');

  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    $field_type = $file->name;
    module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
    if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
      $field_obj = new $field_type();
      $result = $field_obj->create_info($entity_type, $bundle, $details);
      if (is_array($result)) {
        $info[$result['field_name']] = $result;
      }
    }
  }
  return $info;
}

/**
 * Retrieves either the create_info or create_instance_info arrays.
 *
 * The logic for creating the fields for the base table is so similar for
 * both the create_info and create_instance_info arrays they are both
 * handled by this function to prevent duplication of code.
 *
 * @param $step
 *   Set to 'create_info' to retrun the create_info array or
 *   'create_instance_info' to return the create_instance_info array.
 * @param $entity_type
 *   The type of entity (e.g TripalEntity)
 * @param $bundle
 *   The bundle object.
 * @param $details
 *   An array containing the mapping of the bundle to the Chado table.
 *
 * @return
 *   An array compabile with the tripal_chado_field_create_info() and
 *   tripal_chado_field_create_instance_info() functions.
 */
function tripal_chado_field_create_base($step, $entity_type, $bundle, $details) {
  $table_name = $details['chado_table'];
  $type_table = $details['chado_type_table'];
  $type_field = $details['chado_type_column'];

  // Iterate through the columns of the table and see if fields have been
  // created for each one. If not, then create them.
  $schema = chado_get_schema($table_name);
  $columns = $schema['fields'];
  $fields = array();
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
    $field_info = array();
    if ($step == 'create_info') {
      $field_info = tripal_chado_field_create_info_base_defaults($field_name,
        $table_name, $schema, $column_name);
    }
    if ($step == 'create_instance_info') {
      $field_info = tripal_chado_field_create_instance_info_base_defaults($bundle->name,
        $field_name, $table_name, $schema, $column_name);
    }

    // TODO: add in a call to drupal_alter to allow other modules to change
    // the field settings.

    // Add the field to the bundle.
    $fields[$field_name] = $field_info;
  }
  return $fields;
}
/**
 * A helper function for the tripal_chado_field_create_info() function.
 *
 * This function generates the default chado_info array for a column in
 * a base table of Chado.  All of fields returned by this function use
 * default Drupal fields to manage the data in Chado columns.  For
 * custom handling of columns there are custom TripalEntity extensions that
 * are added by the tripal_chado_field_create_info_custom() function.  A
 * custom field will superceed any default base field of the same name
 * provided here.
 *
 * @param $field_name
 *   The name for the new field.
 * @param $table_name
 *   The Chado table
 * @param $schema
 *   The Drupal schema array for the Chado table.
 * @param $column_name
 *   The name of the column in the Chado table.
 * @return
 *   An associative array compatible with the tripal_chado_field_create_info()
 *   function.
 */
function tripal_chado_field_create_info_base_defaults($field_name, $table_name,
    $schema, $column_name) {

  $details = $schema['fields'][$column_name];

  // Set some defaults for the field.
  $field = array(
    'field_name' => $field_name,
    'type' => '',
    'cardinality' => 1,
    'locked' => FALSE,
    'storage' => array(
      'type' => 'field_chado_storage',
    ),
    'settings' => array(
      'chado_table' => $table_name,
      'chado_column' => $column_name,
      'semantic_web' => '',
    ),
  );

  // Alter the field info array depending on the column details.
  switch($details['type']) {
    case 'char':
      $field['type'] = 'text';
      $field['settings']['max_length'] = $details['length'];
      break;
    case 'varchar':
      $field['type'] = 'text';
      $field['settings']['max_length'] = $details['length'];
      break;
    case 'text':
      $field['type'] = 'text';
      $field['settings']['max_length'] = 17179869184;
      $field['settings']['text_processing'] = 1;
      break;
    case 'blob':
      // not sure how to support a blob field.
      continue;
      break;
    case 'int':
      $field['type'] = 'number_integer';
      break;
    case 'float':
      $field['type'] = 'number_float';
      $field['settings']['precision'] = 10;
      $field['settings']['scale'] = 2;
      $field['settings']['decimal_separator'] = '.';
      break;
    case 'numeric':
      $field['type'] = 'number_decimal';
      break;
    case 'serial':
      // Serial fields are most likely not needed as a field.
      break;
    case 'boolean':
      $field['type'] = 'list_boolean';
      $field['settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
      break;
    case 'datetime':
      // Use the Drupal Date and Date API to create the field/widget
      $field['type'] = 'datetime';
      break;
  }

  // Set some default semantic web information
  if ($column_name == 'uniquename') {
    $field['settings']['text_processing'] = 0;
  }
  //
  // PUB TABLE
  //
  elseif ($table_name == 'pub' and $column_name == 'uniquename') {
    $field['type'] = 'text';
    $field['settings']['text_processing'] = 0;
  }

  //
  // ANALYSIS TABLE
  //
  elseif ($table_name == 'analysis' and $column_name == 'sourceuri') {
    $field['type'] = 'text';
    $field['settings']['text_processing'] = 0;
  }

  return $field;
}

/**
 * Implements hook_field_create_instance_info().
 *
 * This is a Tripal defined hook that supports integration with the
 * TripalEntity field.
 */
function tripal_chado_field_create_instance_info($entity_type, $bundle, $term) {

  // Get the details about the mapping of this bundle to the Chado table:
  $details = array(
    'chado_cv_id' => tripal_get_bundle_variable('chado_cv_id', $bundle->id),
    'chado_cvterm_id' => tripal_get_bundle_variable('chado_cvterm_id', $bundle->id),
    'chado_table' => tripal_get_bundle_variable('chado_table', $bundle->id),
    'chado_type_table' => tripal_get_bundle_variable('chado_type_table', $bundle->id),
    'chado_type_column' => tripal_get_bundle_variable('chado_type_column', $bundle->id),
  );

  $base_fields = tripal_chado_field_create_base('create_instance_info', $entity_type, $bundle, $details);
  $custom_fields = tripal_chado_field_create_instance_info_custom($entity_type, $bundle, $details);
  return array_merge($base_fields, $custom_fields);

}
/**
 * A helper function for the tripal_chado_field_create_instance_info() function.
 *
 * This function generates the default chado_instance_info array for a column in
 * a base table of Chado.  All of fields returned by this function use
 * default Drupal fields to manage the data in Chado columns.  For
 * custom handling of columns there are custom TripalEntity extensions that
 * are added by the tripal_chado_field_create_info_custom() function.  A
 * custom field will superceed any default base field of the same name
 * provided here.
 *
 * @param $bundle_name
 *   The name of the bundle to which this field will be attached.
 * @param $field_name
 *   The name for the new field.
 * @param $table_name
 *   The Chado table
 * @param $schema
 *   The Drupal schema array for the Chado table.
 * @param $column_name
 *   The name of the column in the Chado table.
 * @return
 *   An associative array compatible with the tripal_chado_field_create_info()
 *   function.
 */
function tripal_chado_field_create_instance_info_base_defaults($bundle_name,
    $field_name, $table_name, $schema, $column_name) {

  $details = $schema['fields'][$column_name];

  $field =  array(
      'field_name' => $field_name,
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle_name,
      'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
      'description' => '',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
        ),
      ),
    );

  // Determine if the field is required.
  if (array_key_exists('not null', $details) and $details['not null'] === TRUE) {
    $field_info['required'] = TRUE;
  }

  // Alter the field info array depending on the column details.
  switch($details['type']) {
    case 'char':
      $field['widget']['type'] = 'text_textfield';
      break;
    case 'varchar':
      $field['widget']['type'] = 'text_textfield';
      break;
    case 'text':
      $field['widget']['type'] = 'text_textarea';
      $field['widget']['settings']['format'] = filter_default_format();
      break;
    case 'blob':
      // not sure how to support a blob field.
      continue;
      break;
    case 'int':
      $field['widget']['type'] = 'number';
      break;
    case 'float':
      $field['widget']['type'] = 'number';
      break;
    case 'numeric':
      $field['widget']['type'] = 'number';
      break;
    case 'serial':
      // Serial fields are most likely not needed as a field.
      break;
    case 'boolean':
      $field['widget']['type'] = 'options_onoff';
      break;
    case 'datetime':
      $field['widget']['type'] = 'date_select';
      $field['widget']['settings']['increment'] = 1;
      $field['widget']['settings']['tz_handling'] = 'none';
      $field['widget']['settings']['collapsible'] = TRUE;

      // TODO: Add settings so that the minutes increment by 1.
      // And turn off the timezone, as the Chado field doesn't support it.
      break;
  }

  // Set some default semantic web information
  if ($column_name == 'uniquename') {
    $field['widget_type'] = 'text_textfield';
  }
  elseif ($field['label'] == 'Timeaccessioned') {
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
  elseif ($table_name == 'organism' and $column_name == 'comment') {
    $field['label'] = 'Description';
  }
  //
  // PUB TABLE
  //
  elseif ($table_name == 'pub' and $column_name == 'uniquename') {
    $field['widget_type'] = 'text_textfield';
  }

  //
  // ANALYSIS TABLE
  //
  elseif ($table_name == 'analysis' and $column_name == 'program') {
    $field['description'] = 'The program name (e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package then provide a very brief description of the pipeline, workflow or method.';
    $field['label'] = 'Program, Pipeline, Workflow or Method Name.';
  }
  elseif ($table_name == 'analysis' and $column_name == 'sourceuri') {
    $field['widget_type'] = 'text_textfield';
    $field['label'] = 'Source URL';
    $field['description'] = 'The URL where the original source data was derived.  Ideally, this should link to the page where more information about the source data can be found.';
  }
  elseif ($table_name == 'analysis' and $column_name == 'sourcename') {
    $field['label'] = 'Source Name';
    $field['description'] = 'The name of the source data. This could be a file name, data set or a small description for how the data was collected. For long descriptions use the larger description field.';
  }
  elseif ($table_name == 'analysis' and $column_name == 'sourceversion') {
    $field['label'] = 'Source Version';
    $field['description'] = 'If hte source data set has a version include it here.';
  }
  elseif ($table_name == 'analysis' and $column_name == 'algorithm') {
    $field['label'] = 'Source Version';
    $field['description'] = 'The name of the algorithm used to produce the dataset if different from the program.';
  }
  elseif ($table_name == 'analysis' and $column_name == 'programversion') {
    $field['label'] = 'Program Version';
    $field['description'] = 'The version of the program used to perform this analysis. (e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter "n/a" if no version is available or applicable.';
  }
  //
  // PROJECT TABLE
  //
  elseif ($table_name == 'project' and $column_name == 'description') {
    $field['label'] = 'Short Description';
  }

  return $field;
}

/**
 * A helper function for the tripal_chado_field_create_instance_info() function.
 *
 * This function adds in the custom fields info by instantiating the class
 * for the custom field, calling the create_instance_info() function and
 * returning the info array.
 *
 * @param $entity_type
 *   The type of entity (e.g TripalEntity)
 * @param $bundle
 *   The bundle object.
 * @param $details
 *   An array containing the mapping of the bundle to the Chado table.
 */
function tripal_chado_field_create_instance_info_custom($entity_type, $bundle, $details) {
  $info = array();

  // Find all of the files in the tripal_chado/includes/fields directory.
  $fields_path = drupal_get_path('module', 'tripal_chado') . '/includes/fields';
  $field_files = file_scan_directory($fields_path, '/^chado_.*\.inc$/');

  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    $field_type = $file->name;
    module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
    if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
      $field_obj = new $field_type();
      $result = $field_obj->create_instance_info($entity_type, $bundle, $details);
      if (is_array($result)) {
        $info[$result['field_name']] = $result;
      }
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
      if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
        $field_obj = new $field_type();
        $widgets[$field_type . '_widget'] = $field_obj->widget_info();
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
      if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
        $field_obj = new $field_type();
        $formatters[$field_type . '_formatter'] = $field_obj->formatter_info();
      }
    }
  }
  return $formatters;
}

/**
 * Implements hook_field_settings_form()
 */
function tripal_chado_field_settings_form($field, $instance, $has_data) {
  $form = '';
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);

  if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
    $field_obj = new $field_type();
    $form = $field_obj->settings_form($field, $instance, $has_data);
  }
  return $form;
}
/**
 * Implements hook_field_formatter_settings_summary().
 */
function tripal_chado_field_formatter_settings_summary($field, $instance, $view_mode) {

  $summary = '';
  $field_type = $field['type'];
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);

  if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
    $field = new $field_type();
    $summary = $field->formatter_settings_summary($field, $instance, $view_mode);
  }
  return $summary;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function tripal_chado_field_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {
  $element = array();

  $field_type = $field['type'];
  form_load_include($form_state, 'inc', 'tripal_chado', 'includes/fields/' . $field_type);
  module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);

  if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
    $field_obj = new $field_type();
     $element = $field_obj->formatter_settings_form($field, $instance, $view_mode, $form, $form_state);
  }

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

  if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
    $field_obj = new $field_type();
    $field_obj->formatter_view($element, $entity_type, $entity, $field, $instance, $langcode, $items, $display);
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

  if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
    $field_obj = new $field_type();
    $field_obj->widget_form($widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  }

  return $widget;
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
function tripal_chado_field_validate($entity_type, $entity, $field, $instance,
     $langcode, $items, &$errors) {
  $field_type = $field['type'];
  if ($field['storage']['type'] == 'field_chado_storage') {
    module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
    if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
      $field_obj = new $field_type();
      $field_obj->validate($entity_type, $entity, $field, $instance,
        $langcode, $items, $errors);
    }
  }
}

/**
 * Implements hook_field_validate()
 */
function tripal_chado_field_submit($entity_type, $entity, $field, $instance,
    $langcode, &$items, $form, &$form_state) {

  $field_type = $field['type'];
  if ($field['storage']['type'] == 'field_chado_storage') {
    module_load_include('inc', 'tripal_chado', 'includes/fields/' . $field_type);
    if (preg_match('/^chado/', $field_type) and class_exists($field_type)) {
      $field_obj = new $field_type();
      $field_obj->submit($entity_type, $entity, $field, $instance,
          $langcode, $items, $form, $form_state);
    }
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










