<?php
/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_linker__cvterm_info() {
  return array(
    'label' => t('Annotations'),
    'description' => t('This record can be annotated with terms
          from other vocabularies.'),
    'default_widget' => 'chado_linker__cvterm_widget',
    'default_formatter' => 'chado_linker__cvterm_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_attach_info().
 *
 * This is a hook provided by the tripal_Chado module. It allows the field
 * to specify which bundles it will attach to and to specify thee settings.
 *
 * @param $entity_type
 * @param $entity
 * @param $term
 *
 * @return
 *   A field array
 */
function chado_linker__cvterm_attach_info($entity_type, $bundle, $target) {
  $field_info = array();

  // This field is only attached by the chado_linker__cvterm_addr field.

  return $field_info;
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_linker__cvterm_widget_info() {
  return array(
    'label' => t('Annotations'),
    'field types' => array('chado_linker__cvterm'),
    'description' => t('This record can be annotated with terms
          from other vocabularies.'),
  );
}
/**
 * Implements hook_formatter_info.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_info() for each field
 * to specify.
 *
 */
function chado_linker__cvterm_formatter_info() {
  return array(
    'label' => t('Annotations'),
    'field types' => array('chado_linker__cvterm'),
    'settings' => array(
    ),
  );
}
/**
 *
 * @param unknown $entity_type
 * @param unknown $entity
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $display
 */
function chado_linker__cvterm_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $headers = array('Term', 'Definition', 'Is Not', 'Reference');
  $rows = array();

  $chado_table = $field['settings']['chado_table'];
  foreach ($items as $delta => $item) {
    if ($item[$chado_table . '__cvterm_id']) {
      $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $item[$chado_table . '__cvterm_id']));
      $dbxref = $cvterm->dbxref_id;

      // Build the accession.
      $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
      if ($dbxref->db_id->urlprefix) {
        $accession = l($accession, $dbxref->db_id->urlprefix . '/' . $dbxref->accession, array('attributes' => array('target' => '_blank')));
      }

      // Build the publication reference.
      $pub_ref = '';
      $pub_id = $item[$chado_table . '__pub_id'];
      if ($pub_id) {
        $pub = chado_generate_var('pub', array('pub_id' => $pub_id));
        $pub_ref = $pub->title;
      }
      $rows[] = array(
        $accession,
        $cvterm->definition,
        $item[$chado_table . '__is_not'] ? 'Yes' : '',
        '',
      );
    }
  }

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => "$chado_table-table-terms",
      'class' => 'tripal-data-table'
    ),
    'caption' => '',
    'sticky' => FALSE,
    'colgroups' => array(),
    'empty' => 'There are no annotations of this type',
  );

  $element[0] = array(
    '#type' => 'markup',
    '#markup' => theme_table($table),
  );
}
/**
 *
 * @param unknown $field_name
 * @param unknown $widget
 * @param unknown $form
 * @param unknown $form_state
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $delta
 * @param unknown $element
 */
function chado_linker__cvterm_widget(&$widget, $form, $form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $entity = $form['#entity'];
  $field_name = $field['field_name'];

  $matches = array();
  preg_match('/(.*?)__(\d+)/', $field_name, $matches);
  // If the field name is not properly formatted then we can't tell what
  // table and type this is.  So just return.
  if (count($matches) != 3) {
    return $widget;
  }
  $table_name = $matches[1];
  $cv_id = $matches[2];

  // Get the FK column that links to the base table.
  $chado_table = $field['settings']['chado_table'];
  $base_table = $field['settings']['base_table'];
  $schema = chado_get_schema($chado_table);
  $pkey = $schema['primary key'][0];
  $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
  $fkey = $fkeys[0];

  // Get the field defaults.
  $record_id = '';
  $fkey_value = $element['#entity']->chado_record_id;
  $cvterm_name = '';
  $cvterm_id = '';
  $is_not = '';
  $cvterm = NULL;

  // If the field already has a value then it will come through the $items
  // array.  This happens when editing an existing record.
  if (array_key_exists($delta, $items)) {
    $record_id = $items[$delta]['value'];
    $fkey_value = $items[$delta][$table_name . '__' . $fkey];
    $cvterm_name = $items[$delta][$table_name . '--cvterm__name'];
    $is_not = $items[$delta][$table_name . '__is_not'];
    $cvterm_id = $items[$delta][$table_name . '__cvterm_id'];
  }

  // Check $form_state['values'] to see if an AJAX call set the values.
  if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
    $record_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name);
    $fkey_value = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $fkey);
    $is_not = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_not');
    $cvterm_name = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--cvterm__name');
  }

  if ($cvterm_name) {
    $cvterm = chado_generate_var('cvterm', array('cv_id' => $cv_id, 'name' => $cvterm_name));
  }

  $schema = chado_get_schema('cvterm');
  $options = tripal_get_cv_select_options();

  $widget['#table_name'] = $chado_table;
  $widget['#fkey_field'] = $fkey;
  $widget['#element_validate'] = array('chado_linker__cvterm_widget_validate');
  $widget['#theme'] = 'chado_linker__cvterm_widget';
  $widget['#prefix'] =  "<span id='$table_name-$delta'>";
  $widget['#suffix'] =  "</span>";

  $form['value'] = array(
    '#type' => 'value',
    '#value' => $items[$delta]['value'],
  );

  $widget[$table_name . '__' . $pkey] = array(
    '#type' => 'value',
    '#default_value' => $record_id,
  );
  $widget[$table_name . '--cvterm__cv_id--cv__cv_id'] = array(
    '#type' => 'value',
    '#default_value' => $cv_id,
  );
  $widget[$table_name . '__cvterm_id'] = array(
    '#type' => 'value',
    '#default_value' => $cvterm ? $cvterm->cvterm_id : '',
  );
  $widget[$table_name . '__' . $fkey] = array(
    '#type' => 'value',
    '#default_value' => $fkey_value,
  );

  $widget[$table_name . '--cvterm__name'] = array(
    '#type' => 'textfield',
    '#title' => t('Term Name'),
    '#default_value' => $cvterm_name,
    '#required' => $element['#required'],
    '#maxlength' => array_key_exists('length', $schema['fields']['name']) ? $schema['fields']['name']['length'] : 255,
    '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/' . $cv_id,
    '#ajax' => array(
      'callback' => "chado_linker__cvterm_widget_form_ajax_callback",
      'wrapper' => "$table_name-$delta",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  $widget[$table_name . '__is_not'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is Not'),
    '#default_value' => $is_not,
    '#required' => $element['#required'],
  );

  $widget[$table_name . '--cvterm__definition'] = array(
    '#type' => 'item',
    '#markup' => '',
  );

}
/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__cvterm_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];


  return $form[$field_name]['und'][$delta];
}

/**
 * Callback function for validating the chado_linker__cvterm_widget.
 */
function chado_linker__cvterm_widget_validate($element, &$form_state) {

  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $cvterm_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__cvterm_id');
  $cv_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '--cvterm__cv_id--cv__cv_id');
  $cvterm_name = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '--cvterm__name');
  $cvterm_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__cvterm_id');
  $pub_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__pub_id');

  // If the user provided a cv_id and a name then we want to set the
  // foreign key value to be the chado_record_idd
  if ($cvterm_name) {

    $fkey_value = $element['#entity']->chado_record_id;
    tripal_chado_set_field_form_values($field_name, $form_state, $fkey_value, $delta, $table_name . '__' . $fkey);

    // Get the cvterm ID. If one is not available because it's a newly added
    // record, then we need to retrieve it and set the form element.
    if (!$cvterm_id) {
      $cvterm = tripal_get_cvterm(array('cv_id' => $cv_id, 'name' => $cvterm_name));
      tripal_chado_set_field_form_values($field_name, $form_state, $cvterm->cvterm_id, $delta, $table_name . '__cvterm_id');
    }

    if (!$pub_id) {
      $pub = chado_generate_var('pub', array('uniquename' => 'null'));
      tripal_chado_set_field_form_values($field_name, $form_state, $pub->pub_id, $delta, $table_name . '__pub_id');
    }
  }
  else {
    // If the $cv_id and name are not set, then remove the linker FK value to the base table.
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__' . $fkey);
  }

}
/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_chado_linker__cvterm_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
    <div class=\"annotation-cvterm-widget\">
      <div class=\"annotation-cvterm-widget-item\">" .
        drupal_render($element[$table_name . '--cvterm__cv_id--cv__cv_id']) . "
      </div>
      <div class=\"annotation-cvterm-widget-item\">" .
        drupal_render($element[$table_name . '--cvterm__name']) . "
      </div>
      <div class=\"annotation-cvterm-widget-item\">" .
        drupal_render($element[$table_name . '__is_not']) . "
      </div>
    </div>
  ";

  return $layout;
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 *
 * @param $field
 * @param $entity
 * @param $base_table
 * @param $record
 */
function chado_linker__cvterm_load($field, $entity, $base_table, $record) {

  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];

  $matches = array();
  preg_match('/(.*?)__(\d+)/', $field_name, $matches);
  $table_name = $matches[1];
  $cv_id = $matches[2];

  // Get the FK that links to the base record.
  $schema = chado_get_schema($field_table);
  $pkey = $schema['primary key'][0];
  $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
  $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

  // Set some defaults for the empty record.
  $entity->{$field_name}['und'][0] = array(
    'value' => '',
    $field_table . '__' . $fkey_lcolumn => '',
    $field_table . '__' . 'cvterm_id' => '',
    // The pub column is present in the cell_line_cvterm, feature_cvterm,
    // library_cvterm, phenotype_comparision_cvterm, phenotype_cvterm,
    // stock_cvterm, and stock_relationship_cvterm.
    $field_table . '__' . 'pub_id' => '',
    // The is_not column is present in feature_cvterm and stock_cvterm tables.
    $field_table . '__' . 'is_not' => '',
    // The rank column is present in the cell_line_cvterm, expression_cvterm,
    // feature_cvterm, phenotype_comparision_cvterm, phenotype_cvterm,
    // and stock_cvterm tables.
    $field_table . '__' . 'rank' => '',
    // The cvterm_type_id is present in the expression_cvterm table.
    $field_table . '--' . 'cvterm_type_id' => '',
    // The following field are to help link the cvterm.
    $field_table . '--' . 'cvterm__cv_id--cv__cv_id' => '',
    $field_table . '--' . 'cvterm__name' => '',
  );

  // Get the annotations associated with this base record for this fields type.
  $columns = array('*');
  $match = array(
    $fkey_lcolumn => $record->$fkey_rcolumn,
    'cvterm_id' => array(
      'cv_id' => $cv_id,
    ),
  );
  $options = array(
    'return_array' => TRUE,
    'order_by' => array('rank' => 'ASC')
  );
  $fcvterms = chado_select_record($field_table, $columns, $match, $options);
  for ($i = 0; $i < count($fcvterms); $i++) {
    $linker = $fcvterms[$i];
    $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $linker->cvterm_id));
    $entity->{$field_name}['und'][$i] = array(
      'value' => $linker->$pkey,
      $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn,
      $field_table . '__' . 'cvterm_id' => $linker->cvterm_id,
      $field_table . '__' . 'pub_id' => property_exists($linker, 'pub_id') ? $linker->pub_id : '',
      $field_table . '__' . 'is_not' => property_exists($linker, 'is_not') ? $linker->is_not : '',
      $field_table . '__' . 'rank' => property_exists($linker, 'rank') ? $linker->rank : '',
      $field_table . '__' . 'cvterm_type_id' => property_exists($linker, 'cvterm_type_id') ? $linker->cvterm_type_id : '',
      $field_table . '--' . 'cvterm__cv_id--cv__cv_id' => $cvterm->cv_id->cv_id,
      $field_table . '--' . 'cvterm__name' => $cvterm->name,
    );
  }
}

