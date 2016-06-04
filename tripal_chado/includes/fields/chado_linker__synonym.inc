<?php

class chado_linker__synonym extends TripalField {

  /**
   * @see TripalField::fieldInfo()
   */
  static function fieldInfo() {
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
   * @see TripalField::can_attach()
   */
  protected function setCanAttach() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $syn_table = $table_name . '_synonym';
    if (chado_table_exists($syn_table)) {
      $this->can_attach = TRUE;
      return;
    }
    $this->can_attach = FALSE;
  }
  /**
   * @see TripalField::setFieldName()
   */
  protected function setFieldName() {
    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $this->field_name = $table_name . '_synonym';
  }
  /**
   * @see TripalField::create_info()
   */
  function createInfo() {
    if (!$this->can_attach) {
      return;
    }

    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    $syn_table = $table_name . '_synonym';
    $schema = chado_get_schema($syn_table);
    $pkey = $schema['primary key'][0];

    return array(
      'field_name' => $this->field_name,
      'type' => 'chado_linker__synonym',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $syn_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => 'schema:alternateName',
      ),
    );
  }
  /**
   * @see TripalField::createInstanceInfo()
   */
  function createInstanceInfo() {
    if (!$this->can_attach) {
      return;
    }

    $table_name = $this->details['chado_table'];
    $type_table = $this->details['chado_type_table'];
    $type_field = $this->details['chado_type_column'];
    $cv_id      = $this->details['chado_cv_id'];
    $cvterm_id  = $this->details['chado_cvterm_id'];

    return array(
      'field_name' => $this->field_name,
      'entity_type' => $this->entity_type,
      'bundle' => $this->bundle->name,
      'label' => 'Synonyms',
      'description' => 'Alternate names, aliases or synonyms for this record.',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
      ),
      'widget' => array(
        'type' => 'chado_linker__synonym_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'inline',
          'type' => 'chado_linker__synonym_formatter',
          'settings' => array(),
        ),
      ),
    );
  }
  /**
   * @see TripalField::widgetInfo()
   */
  public static function widgetInfo() {
    return array(
      'chado_linker__synonym_widget' => array(
        'label' => t('Synonyms'),
        'field types' => array('chado_linker__synonym'),
      ),
    );
  }
  /**
   * @see TripalField::formatterInfo()
   */
  static function formatterInfo() {
    return array(
      'chado_linker__synonym_formatter' => array(
        'label' => t('Synonyms'),
        'field types' => array('chado_linker__synonym'),
        'settings' => array(
        ),
      ),
    );
  }
  /**
   * @see TripalField::formatterView()
   */
  static function formatterView(&$element, $entity_type, $entity, $field,
      $instance, $langcode, $items, $display) {

    $chado_table = $field['settings']['chado_table'];
    foreach ($items as $delta => $item) {
      if (array_key_exists($chado_table . '__synonym_id', $item) and
          $item[$chado_table . '__synonym_id']) {
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
   * @see TripalField::widgetForm()
   */
  public static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

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
      $record_id = $items[$delta][$table_name . '__' . $pkey];
      $fkey_value = $items[$delta][$table_name . '__' . $fkey];
      $synonym_id = $items[$delta][$table_name . '__synonym_id'];
      $pub_id = $items[$delta][$table_name . '__pub_id'];
      $is_current = $items[$delta][$table_name . '__is_current'];
      $is_internal = $items[$delta][$table_name . '__is_internal'];
      $syn_name = $items[$delta][$table_name . '__synonym_id--name'];
      $syn_type = $items[$delta][$table_name . '__synonym_id--type_id'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $pkey);
      $fkey_value = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $fkey);
      $synonym_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__synonym_id');
      $pub_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__pub_id');
      $is_current = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_current');
      $is_internal = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_internal');
      $syn_name = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__synonym_id--name');
      $syn_type = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__synonym_id--type_id');
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
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget[$table_name . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget[$table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    // TODO: add a widget for selecting a publication.
    $widget[$table_name . '__pub_id'] = array(
      '#type' => 'value',
      '#default_value' => $pub_id,
    );
    $widget[$table_name . '__synonym_id--type_id'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $options,
      '#default_value' => $syn_type,
    );
    $widget[$table_name . '__synonym_id--name'] = array(
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
  static function load($field, $entity, $details = array()) {

    $record = $details['record'];
    $base_table = $field['settings']['base_table'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    // Get the PKey for this table
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      $field_table . '__' . $pkey => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__' . 'synonym_id' => '',
      $field_table . '__' . 'pub_id' => '',
      $field_table . '__' . 'is_current' => TRUE,
      $field_table . '__' . 'is_internal' => '',
      $field_table . '__synonym_id--name' => '',
      $field_table . '__synonym_id--type_id' => '',
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
          'value' => array(
            '@type' => $synonym->type_id->dbxref_id->db_id->name . ':' . $synonym->type_id->dbxref_id->accession,
            'type' => $synonym->type_id->name,
            'name' => $synonym->name,
          ),
          $field_table . '__' . $pkey = $linker->$pkey,
          $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn->$fkey_lcolumn,
          $field_table . '__' . 'synonym_id' => $synonym->synonym_id,
          $field_table . '__' . 'pub_id' => $linker->pub_id->pub_id,
          $field_table . '__' . 'is_current' => $linker->is_current,
          $field_table . '__' . 'is_internal' => $linker->is_internal,
          $field_table . '__synonym_id--name' => $synonym->name,
          $field_table . '__synonym_id--type_id' => $synonym->type_id->cvterm_id,
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
        drupal_render($element[$table_name . '__synonym_id--name']) . "
        </div>
        <div>" .
        drupal_render($element[$table_name . '__synonym_id--type_id']) . "
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
