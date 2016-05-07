<?php

class chado_linker__synonym extends TripalField {

  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Synonyms'),
      'description' => t('Adds an alternative name (synonym or alias) to this record.'),
      'default_widget' => 'chado_linker__synonym_widget',
      'default_formatter' => 'chado_linker__synonym_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }
  /**
   * @see TripalField::attach_info()
   */
  function attach_info($entity_type, $bundle, $target) {
    $field_info = array();

    $table_name = $target['data_table'];
    $type_table = $target['type_table'];
    $type_field = $target['field'];
    $cv_id      = $target['cv_id'];
    $cvterm_id  = $target['cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $syn_table = $table_name . '_synonym';
    if (!chado_table_exists($syn_table)) {
      return $field_info;
    }

    $schema = chado_get_schema($syn_table);
    $pkey = $schema['primary key'][0];

    // Initialize the field array.
    $field_info = array(
      'field_name' => $table_name . '__synonym',
      'field_type' => 'chado_linker__synonym',
      'widget_type' => 'chado_linker__synonym_widget',
      'widget_settings' => array('display_label' => 1),
      'description' => '',
      'label' => 'Synonyms',
      'is_required' => 0,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'storage' => 'field_chado_storage',
      'field_settings' => array(
        'chado_table' => $syn_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => array(
          'name' => 'alternateName',
          'accession' => 'alternateName',
          'ns' => 'schema',
          'nsurl' => 'https://schema.org/',
        ),
      ),
    );
    return $field_info;
  }
  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Synonyms'),
      'field types' => array('chado_linker__synonym'),
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('Synonyms'),
      'field types' => array('chado_linker__synonym'),
      'settings' => array(
      ),
    );
  }
  /**
   * @see TripalField::formatter_view()
   */
  function formatter_view(&$element, $entity_type, $entity, $field,
      $instance, $langcode, $items, $display) {

    $chado_table = $field['settings']['chado_table'];
    foreach ($items as $delta => $item) {
      if ($item[$chado_table . '__synonym_id']) {
        $synonym = chado_generate_var('synonym', array('synonym_id' => $item[$chado_table . '__synonym_id']));
        $name = $synonym->name;
        if ($synonym->type_id->name != 'exact') {
          $name .= ' (<i>' . $synonym->type_id->name . '</i>)';
        }
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $name,
        );
      }
    }
  }
  /**
   * @see TripalField::widget_form()
   */
  function widget_form(&$widget, $form, $form_state, $field,
      $instance, $langcode, $items, $delta, $element) {

    $entity = $form['#entity'];
    $field_name = $field['field_name'];

    // Get the FK column that links to the base table.
    $table_name = $field['settings']['chado_table'];
    $base_table = $field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity']->chado_record_id;
    $synonym_id = '';
    $pub_id = '';
    $is_current = TRUE;
    $is_internal = FALSE;
    $syn_name = '';
    $syn_type = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      $record_id = $items[$delta]['value'];
      $fkey_value = $items[$delta][$table_name . '__' . $fkey];
      $synonym_id = $items[$delta][$table_name . '__synonym_id'];
      $pub_id = $items[$delta][$table_name . '__pub_id'];
      $is_current = $items[$delta][$table_name . '__is_current'];
      $is_internal = $items[$delta][$table_name . '__is_internal'];
      $syn_name = $items[$delta][$table_name . '--synonym__name'];
      $syn_type = $items[$delta][$table_name . '--synonym__type_id'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name);
      $fkey_value = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $fkey);
      $synonym_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__synonym_id');
      $pub_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__pub_id');
      $is_current = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_current');
      $is_internal = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_internal');
      $syn_name = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--synonym__name');
      $syn_type = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--synonym__type_id');
    }

    // Get the synonym type terms.  There shouldn't be too many.
    $cv = tripal_get_default_cv('synonym', 'type_id');
    $options = tripal_get_cvterm_select_options($cv->cv_id);

    // Get the schema for the synonym table so we can make sure we limit the
    // size of the name field to the proper size.
    $schema = chado_get_schema('synonym');

    $widget['#table_name'] = $table_name;
    $widget['#fkey_field'] = $fkey;
    $widget['#element_validate'] = array('chado_linker__synonym_widget_validate');
    $widget['#theme'] = 'chado_linker__synonym_widget';
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget[$table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );

    $widget[$table_name . '--synonym__type_id'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $options,
      '#default_value' => $syn_type,
    );
    $widget[$table_name . '--synonym__name'] = array(
      '#type' => 'textfield',
      '#title' => t('Synonym Name'),
      '#default_value' => $syn_name,
      '#size' => 25,
    );

    $widget[$table_name . '__is_current'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is Current'),
      '#default_value' => $is_current,
      '#required' => $element['#required'],
    );

    $widget[$table_name . '__is_internal'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is Internal'),
      '#default_value' => $is_internal,
      '#required' => $element['#required'],
    );

  }

  /**
   * @see TripalField::load()
   */
  function load($field, $entity, $details) {

    $record = $details['record'];
    $base_table = $field['settings']['base_table'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'synonym_id' => '',
      $field_table . '__' . 'pub_id' => '',
      $field_table . '__' . 'is_current' => TRUE,
      $field_table . '__' . 'is_internal' => '',
      $field_table . '--' . 'synonym__name' => '',
      $field_table . '--' . 'synonym__type_id' => '',
      // Ignore the synonym_sgml column for now.
    );

    $linker_table = $base_table . '_synonym';
    $options = array('return_array' => 1);
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    if (count($record->$linker_table) > 0) {
      $i = 0;
      foreach ($record->$linker_table as $index => $linker) {
        $synonym = $linker->synonym_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => $linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'synonym_id' => $synonym->synonym_id,
          $field_table . '__' . 'pub_id' => $linker->pub_id->pub_id,
          $field_table . '__' . 'is_current' => $linker->is_current,
          $field_table . '__' . 'is_internal' => $linker->is_internal,
          $field_table . '--' . 'synonym__name' => $synonym->name,
          $field_table . '--' . 'synonym__type_id' => $synonym->type_id->cvterm_id,
        );
        $i++;
      }
    }
  }
}

/**
 * Theme function for the synonym widget.
 *
 * @param $variables
 */
function theme_chado_linker__synonym_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"synonym-widget\">
        <div class=\"synonym-widget-item\">" .
        drupal_render($element[$table_name . '--synonym__name']) . "
        </div>
        <div>" .
        drupal_render($element[$table_name . '--synonym__type_id']) . "
        </div>
        <div class=\"synonym-widget-item\">" .
        drupal_render($element[$table_name . '__is_internal']) . "
        </div>
        <div>" .
        drupal_render($element[$table_name . '__is_current']) . "
        </div>
      </div>
    ";

  return $layout;
}
/**
 * Callback function for validating the chado_linker__synonym_widget.
 */
function chado_linker__synonym_widget_validate($element, &$form_state) {

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
  $fkey_value = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $fkey);
  $synonym_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__synonym_id');
  $pub_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__pub_id');
  $is_current = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_current');
  $is_internal = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_internal');
  $syn_name = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--synonym__name');
  $syn_type = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--synonym__type_id');

  // Make sure that if a synonym is provided that a type is also
  // provided.
  if ($syn_name and !$syn_type) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--synonym__type_id', t("Please set a synonym type."));
  }
  if (!$syn_name and $syn_type) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--synonym__name', t("Please set a synonym name."));
  }
  // If the user provided a cv_id and a name then we want to set the
  // foreign key value to be the chado_record_idd
  if ($syn_name and $syn_type) {

    // Get the synonym. If one with the same name and type is already present
    // then use that. Otherwise, insert a new one.
    if (!$synonym_id) {
      $synonym = chado_generate_var('synonym', array('name' => $syn_name, 'type_id' => $syn_type));
      if (!$synonym) {
        $synonym = chado_insert_record('synonym', array(
          'name' => $syn_name,
          'type_id' => $syn_type,
          'synonym_sgml' => '',
        ));
        $synonym = (object) $synonym;
      }

      // Set the synonym_id and FK value
      tripal_chado_set_field_form_values($field_name, $form_state, $synonym->synonym_id, $delta, $table_name . '__synonym_id');
      $fkey_value = $element['#entity']->chado_record_id;
      tripal_chado_set_field_form_values($field_name, $form_state, $fkey_value, $delta, $table_name . '__' . $fkey);
    }

    if (!$pub_id) {
      $pub = chado_generate_var('pub', array('uniquename' => 'null'));
      tripal_chado_set_field_form_values($field_name, $form_state, $pub->pub_id, $delta, $table_name . '__pub_id');
    }
  }
  else {
    // If the $syn_name is not set, then remove the linker FK value to the base table.
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__' . $fkey);
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__synonym_id');
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__is_internal');
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__is_current');
  }

}

/**
 * An Ajax callback for the synonym widget.
 */
function chado_linker__synonym_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}
