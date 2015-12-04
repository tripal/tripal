<?php

/**
 * Launchpad for biological data administration.
 */
function tripal_entities_admin_view() {

  // Render the tripal entites bundle form.
  $form = drupal_get_form('tripal_entities_admin_bundles_form');
  $output = drupal_render($form) . "<br>[ Image Place Holder for Data Type Summary ]<br>";

  // Set the breadcrumb.
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  drupal_set_breadcrumb($breadcrumb);

  return $output;
}

/**
 * Provide a data listing for tripal entites (ie: biological data).
 * Note: This hook returns a rendered page but we want a form.
 */
function tripal_entities_content_view() {

  // Retrieve our data listing form and render it.
  $form = drupal_get_form('tripal_entities_content_overview_form');
  $output = drupal_render($form);

  // Set the breadcrumb.
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  drupal_set_breadcrumb($breadcrumb);

  return $output;

}

/**
 * Display a listing of Biological Data to the administrator.
 *
 * @TODO Filters and bulk operations needed to be added to this form.
 *
 * @param array $form
 * @param array $form_state
 * @return
 *   A form array describing this listing to the Form API.
 */
function tripal_entities_content_overview_form($form, &$form_state) {

  // Set the title to be informative (defaults to content for some reason).
  drupal_set_title('Biological Data');

  // Retrieve a pages list of all tripal entitles (ie: biological data).
  // This will return the 25 most recently created entities.
  $entities = db_select('tripal_entity', 'td')
    ->fields('td')
    ->orderBy('created', 'DESC')//ORDER BY created
    ->range(0,25)
    ->execute();

  $headers = array('Title', 'Vocabulary', 'Term', 'Author', 'Status', 'Updated', 'Operations');
  $rows = array();

  // For each entity retrieved add a row to the data listing.
  while ($entity = $entities->fetchObject()) {

    // Retrieve details about the term this entity is based on.
    $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $entity->cvterm_id));

    // Retrieve details about the user who created this data.
    $author = user_load($entity->uid);

    // Add information to the table.
    $rows[] = array(
      l($entity->title, 'BioData/' . $entity->id),
      $cvterm->cv_id->name . ' (' . $cvterm->dbxref_id->db_id->name . ')',
      $cvterm->name,
      l($author->name, 'user/' . $entity->uid),
      $entity->status == 1 ? 'published' : 'unpublished',
      format_date($entity->changed, 'short'),
      l('edit', 'BioData/' . $entity->id . '/edit') . '&nbsp;&nbsp;' .
      l('delete', 'BioData/' . $entity->id . '/delete')
    );
  }

  // If there are no entites created yet then add a message to the table to
  // provide guidance to administrators.
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('No biological data available.'),
        'colspan' => 7
      )
    );
  }

  // Render the data listing.
  $table_vars = array(
    'header'     => $headers,
    'rows'       => $rows,
    'attributes' => array(),
    'sticky'     => TRUE,
    'caption'    => '',
    'colgroups'  => array(),
    'empty'      => '',
  );
  $form['results'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', $table_vars),
  );

  return $form;
}

/**
 * Tripal administration form for Biological data (Admin > Tripal > Biological Data).
 *
 * @TODO Add graph showing available data types.
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function tripal_entities_admin_bundles_form($form, &$form_state) {
  $form = array();

  // Set the defaults.
  $cv_id = NULL;
  $term_name = NULL;

  // Set defaults using the form state.
  if (array_key_exists('values', $form_state)) {
    $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : NULL;
    $term_name = array_key_exists('term_name', $form_state['values']) ? $form_state['values']['term_name'] : NULL;
  }

  // Let the user select the vocabulary and tripal_entity but only if they haven't
  // already selected a tripal_entity.
  $sql = "
    SELECT CV.cv_id, CV.name
    FROM {cv} CV
    ORDER BY CV.name
  ";
  $vocabs = chado_query($sql);
  $cvs = array();
  while ($vocab = $vocabs->fetchObject()) {
    $cvs[$vocab->cv_id] = $vocab->name;
  }
  $form['cv_id'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#options' => $cvs,
    '#required' => FALSE,
    '#description' => t('Select a vocabulary to view potential data types in the chart below. Limit the chart to only published data types by selecting the checkbox.'),
    '#default_value' => $cv_id,
    '#ajax' => array(
      'callback' => "tripal_entities_admin_bundles_form_ajax_callback",
      'wrapper' => 'tripal_entities_admin_bundles_form',
      'effect' => 'fade',
      'method' => 'replace'
    )
  );

  $form['refresh_bundles'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh Data Types'),
    '#name' => 'refresh_bundles',
  );

  $form['publish_new_data'] = array(
    '#type' => 'submit',
    '#value' => t('Publish New Data'),
    '#name' => 'publish_new_data',
  );

  $form['#prefix'] = '<div id="tripal_entities_admin_bundle_form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Submit a job to populate the entity tables
 * This operation makes available data types in the database publishable
 */
function tripal_entities_admin_bundles_form_submit($form, $form_state) {
  global $user;
  if ($form_state['clicked_button']['#name'] == 'refresh_bundles') {
    tripal_add_job('Create publishable data types', 'tripal_entity', 'tripal_entities_populate_entity_tables', array(), $user->uid);
  }
  if ($form_state['clicked_button']['#name'] == 'publish_new_data') {
  }
}

/**
 * Form for creating biological data types (ie: tripal entity types).
 *
 * This form is available on the menu at Admin >> Structure >> Biological Data
 * Types
 *
 * @param array $form
 * @param array $form_state
 *
 * @return
 *    An array describing this form to the Form API.
 */
function tripal_entities_admin_add_type_form($form, &$form_state) {
  $term_name = '';
  $num_terms = 0;
  $cv_id = '';

  // Set defaults using the form state.
  if (array_key_exists('input', $form_state)) {
    if (array_key_exists('term_name', $form_state['input'])) {
      $term_name = $form_state['input']['term_name'];
    }
    if (array_key_exists('cv_id', $form_state['input'])) {
      $cv_id = $form_state['input']['cv_id'];
    }

    // If a term and cv_id are provided then we can look for the term using
    // both and we should find a unique term. If only ther term is provided
    // we can still look for a unique term but there must only be one.
    if ($term_name and !$cv_id) {
      $match = array(
        'name' => $term_name,
      );
    }
    else {
      $match = array(
        'name' => $term_name,
        'cv_id' => $cv_id,
      );
    }

    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = count($terms);
  }

  // If no term has been selected yet then provide the auto complete field.
  if ($num_terms == 0) {
    $form['term_name'] = array(
      '#title'       => t('Biological Data Type'),
      '#type'        => 'textfield',
      '#description' => t("Please enter the type of data that you want to add.
          Once added, priviledged users can add new records of the selected
          type. As you type, suggestions will be provided."),
      '#required'    => TRUE,
      '#default_value' => $term_name,
      '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/$cv_id",
    );
  }
  else {
    $form['term_name'] = array(
      '#type' => 'hidden',
      '#value' => $term_name,
    );
  }

  // If the term belongs to more than one vocabulary then add additional fields
  // to let the user select the vocabulary.
  if ($num_terms > 1) {
    $cvs = array();
    foreach ($terms as $term) {
      $cvs[$term->cv_id->cv_id] = 'Vocabulary: <b>' . $term->cv_id->name . '</b> (' . $term->cv_id->definition . ')<br>' . $term->name . ': ' . $term->definition;
    }
    $form['cv_id'] = array(
      '#type' => 'radios',
      '#title' => t('Select the appropriate vocabulary'),
      '#options' => $cvs,
    );
  }

  // Add in the button for the cases of no terms or too many.
  $form['select_button'] = array(
    '#type' => 'submit',
    '#value' => t('Use this term'),
    '#name' => 'select_cvterm'
  );

  return $form;
}

/**
 * Implements hook_validate() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_admin_add_type_form_validate($form, &$form_state) {

  // Check if this term and vocabulary is in the tripal_vocabulary usage tables.
  // If not then add it.
  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='select_cvterm') {

    // First, make sure the term is unique. If not then we can't check it.
    $term_name = NULL;
    $cv_id = NULL;
    $cvterm = NULL;
    if (array_key_exists('term_name', $form_state['values'])) {
      $term_name = $form_state['input']['term_name'];
    }
    if (array_key_exists('cv_id', $form_state['input'])) {
      $cv_id = $form_state['input']['cv_id'];
    }

    // If a term and cv_id are provided then we can look for the term using
    // both and we should find a unique term. If only ther term is provided
    // we can still look for a unique term but there must only be one.
    if ($term_name and !$cv_id) {
      $match = array(
        'name' => $term_name,
      );
    }
    else {
      $match = array(
        'name' => $term_name,
        'cv_id' => $cv_id,
      );
    }
    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
    $form_state['storage']['terms'] = $terms;

    // If we do not have any terms then the term provided by the user does not
    // exists and we need to provide an error message.
    if (count($terms) == 0) {
      form_set_error('term_name', t('The term does not exist in this database.'));
    }
    // If we have more than one term then we need to set an error so that the
    // form can provide a list of vocabularies to select from.
    if (count($terms) > 1) {
      form_set_error('term_name', t('The term is not unique. A list of vocabularies
          that contain this term. Please select the most appropriate vocabulary.'));
    }
  }
}
/**
 * Implements hook_submit() for the tripal_entities_admin_publish_form.
 *
 */
function tripal_entities_admin_add_type_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#name'] =='select_cvterm') {
    $cvterm = $form_state['storage']['terms'][0];

    $bundle_id = 'dbxref_' . $cvterm->dbxref_id->dbxref_id;

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $einfo = entity_get_info('BioData');
    if (!in_array($bundle_id, array_keys($einfo['bundles']))) {
      $error = '';
      $success = tripal_create_entity_type($cvterm, $error);
      if (!$success) {
        drupal_set_message($error, 'error');
        $form_state['redirect'] = "admin/structure/BioData";
      }
      else {
        drupal_set_message('New biological data type created.  Fields are added automatically to this type.');
        $form_state['redirect'] = "admin/structure/BioData";
      }
    }
    else {
      drupal_set_message('This type already exists.', 'warning');
    }
  }
}

/**
 * Implements hook_add_bundle_fields().
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $cvterm
 */
function tripal_entities_add_bundle_fields($entity_type_name, $bundle_name, $cvterm) {
  // Adds the fields for the base table to the entity.
  tripal_entities_add_bundle_base_fields($entity_type_name, $bundle_name, $cvterm);

  // Check to see if there are any kv-property tables associated to this
  // base table. If so, add the fields for that type of table.
  tripal_entities_add_bundle_kvproperty_adder_field($entity_type_name, $bundle_name, 'featureprop');
}

/**
 * Adds the fields for a kv-property table fields
 *
 * @param $entity_type_name
 * @param $bundle_name
 * @param $kv_table
 */
function tripal_entities_add_bundle_kvproperty_adder_field($entity_type_name, $bundle_name, $kv_table) {
  // First add a generic property field so that users can add new proeprty types.
  $field_name = $kv_table;

  // Initialize the field array.
  $field_info = array(
    'field_type' => 'kvproperty_adder',
    'widget_type' => 'tripal_fields_kvproperty_adder_widget',
    'field_settings' => array(),
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Additional Properties',
    'is_required' => 0,
  );
  tripal_add_bundle_field($field_name, $field_info, $entity_type_name, $bundle_name);
}

/**
 * Adds the fields for the base table to the entity.
 */
function tripal_entities_add_bundle_base_fields($entity_type_name, $bundle_name, $cvterm) {

  // Get the list of tables where this cvterm is used.
  $match = array('cvterm_id' => $cvterm->cvterm_id);
  $term = chado_select_record('tripal_term', array('*'), $match);
  $values = array('term_id' => $term[0]->term_id);
  $tables = chado_select_record('tripal_term_usage', array('*'), $values);

  // Iterate through the tables.
  foreach ($tables as $table) {
    $table_name = $table->data_table;
    $type_table = $table->type_table;
    $type_field = $table->field;

    // We only want to look at base tables.
    if ($table_name == 'cvterm_dbxref' || $table_name == 'cvterm_relationship' ||
        $table_name == 'cvtermpath' || $table_name == 'cvtermprop' || $table_name == 'chadoprop' ||
        $table_name == 'cvtermsynonym' || preg_match('/_relationship$/', $table_name) ||
        preg_match('/_cvterm$/', $table_name)) {
      continue;
    }

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
      $field_info = tripal_entities_get_table_column_field_default($table_name, $schema, $column_name);


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

      // If this field is a foreign key field then we will have a special custom
      // field provided by Tripal.
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
}

/**
 * Returns a $field_info array for a field based on a databaes column.
 *
 */
function tripal_entities_get_table_column_field_default($table_name, $schema, $column_name) {
  $details = $schema['fields'][$column_name];

  // Create an array with information about this field.
  $field_info = array(
    'field_type' => '',
    'widget_type' => '',
    'field_settings' => array(
      'chado_table' => $table_name,
      'chado_column' => $column_name,
    ),
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => ucwords(preg_replace('/_/', ' ', $column_name)),
    'is_required' => 0,
  );

  // Alter the field info array depending on the column details.
  switch($details['type']) {
    case 'char':
      $field_info['field_type'] = 'text';
      $field_info['widget_type'] = 'text_textfield';
      $field_info['field_settings']['max_length'] = $details['length'];
      break;
    case 'varchar':
      $field_info['field_type'] = 'text';
      $field_info['widget_type'] = 'text_textfield';
      $field_info['field_settings']['max_length'] = $details['length'];
      break;
    case 'text':
      $field_info['field_type'] = 'text';
      $field_info['widget_type'] = 'text_textarea';
      $field_info['field_settings']['max_length'] = 17179869184;
      break;
    case 'blob':
      // not sure how to support a blob field.
      continue;
      break;
    case 'int':
      $field_info['field_type'] = 'number_integer';
      $field_info['widget_type'] = 'number';
      break;
    case 'float':
      $field_info['field_type'] = 'number_float';
      $field_info['widget_type'] = 'number';
      $field_info['field_settings']['precision'] = 10;
      $field_info['field_settings']['scale'] = 2;
      $field_info['field_settings']['decimal_separator'] = '.';
      break;
    case 'numeric':
      $field_info['field_type'] = 'number_decimal';
      $field_info['widget_type'] = 'number';
      break;
    case 'serial':
      // Serial fields are most likely not needed as a field.
      break;
    case 'boolean':
      $field_info['field_type'] = 'list_boolean';
      $field_info['widget_type'] = 'options_onoff';
      $field_info['field_settings']['allowed_values'] = array(0 => "No", 1 => "Yes");
      break;
    case 'datetime':
      // Use the Drupal Date and Date API to create the field/widget
      $field_info['field_type'] = 'datetime';
      $field_info['widget_type'] = 'date_select';
      $field_info['widget_settings']['increment'] = 1;
      $field_info['widget_settings']['tz_handling'] = 'none';
      $field_info['widget_settings']['collapsible'] = TRUE;

      // TODO: Add settings so that the minutes increment by 1.
      // And turn off the timezone, as the Chado field doesn't support it.
      break;
  }

  return $field_info;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function tripal_entities_admin_access_form($form, &$form_state) {
  $form = array();

  return $form;
}
