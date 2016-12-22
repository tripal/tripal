<?php

class chado_linker__cvterm_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Chado Annotation';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__cvterm');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $field_name = $this->field['field_name'];

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
    $chado_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($chado_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity']->chado_record_id;
    $cvterm_name = '';
    $cvterm_id = '';
    $pub_id = '';
    $uname = '';
    $is_not = '';
    $cvterm = NULL;

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      $record_id = $items[$delta]['value'];
      $cvterm_name = $items[$delta]['cvterm__name'];
      $pub_id =$items[$delta]['chado-' . $table_name . '__pub_id'];
      if ($pub_id && $pub_id != 1) {
        $pub = chado_generate_var('pub', array('pub_id' => $pub_id));
        $uname = $pub->uniquename;
      }
      $is_not = $items[$delta]['chado-' . $table_name . '__is_not'];
      $cvterm_id = $items[$delta]['chado-' . $table_name . '__cvterm_id'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      // See example in chado_linker_contact.inc
      //       $record_id = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name);
      //       $fkey_value = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__' . $fkey);
      //       $is_not = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '__is_not');
      //       $cvterm_name = tripal_chado_get_field_form_values($table_name, $form_state, $delta, $table_name . '--cvterm__name');
    }

    if ($cvterm_name) {
      $cvterm = chado_generate_var('cvterm', array('cv_id' => $cv_id, 'name' => $cvterm_name));
    }

    $schema = chado_get_schema('cvterm');
    $options = tripal_get_cv_select_options();

    $widget['#table_name'] = $chado_table;
    $widget['#fkey_field'] = $fkey;
    $widget['#theme'] = 'chado_linker__cvterm_widget';
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['cv__cv_id'] = array(
      '#type' => 'value',
      '#default_value' => $cv_id,
    );
    $widget['chado-' . $table_name . '__cvterm_id'] = array(
      '#type' => 'value',
      '#default_value' => $cvterm ? $cvterm->cvterm_id : '',
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );

    $widget['cvterm__name'] = array(
      '#type' => 'textfield',
      '#title' => t('Term Name'),
      '#default_value' => $cvterm_name,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['name']) ? $schema['fields']['name']['length'] : 255,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/' . $cv_id,
      '#size' => 30
    );

    $widget['pub'] = array(
      '#type' => 'textfield',
      '#title' => t('Publication'),
      '#default_value' => $uname,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/pub',
      '#ajax' => array(
        'callback' => "schema__publication_widget_form_ajax_callback",
        'wrapper' => "$table_name-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#maxlength' => 100000,
    );

    $widget['chado-' . $table_name . '__pub_id'] = array(
      '#type' => 'value',
      '#default_value' => $pub_id ? $pub_id : 1,
    );

    $widget['chado-' . $table_name . '__is_not'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is Not'),
      '#default_value' => $is_not,
      '#required' => $element['#required'],
    );

    $widget['cvterm__definition'] = array(
      '#type' => 'item',
      '#markup' => '',
    );
  }

  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.  Note
   * this is different from the validate() function which ensures that the
   * field data meets expectations.
   *
   * @param $form
   * @param $form_state
   */
  public function validate($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $table_name = $this->instance['settings']['chado_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $base_table = $this->instance['settings']['base_table'];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$base_table]['columns'][$lfkey_field];


    // If the form ID is field_ui_field_edit_form, then the user is editing the
    // field's values in the manage fields form of Drupal.  We don't want
    // to validate it as if it were being used in a data entry form.
    if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
      return;
    }

    // If the user provided a cv_id and a name then we want to set the
    // foreign key value to be the chado_record_idd
    $cvterm_name = isset($form_state['values'][$field_name][$langcode][$delta]['cvterm__name']) ? $form_state['values'][$field_name][$langcode][$delta]['cvterm__name'] : '';

    if (!$cvterm_name) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__cvterm_id'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $lfkey_field] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] = '';
    }
  }
}
