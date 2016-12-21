<?php

class schema__publication_widget extends TripalFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Publication';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('schema__publication');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $entity = $form['#entity'];
    $field_name = $this->field['field_name'];
    
    // Get the FK column that links to the base table.
    $table_name = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    
    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity']->chado_record_id;
    $pub_id = '';
    $uname = '';
    
    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $pkey, $record_id);
      $pub_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__pub_id', $pub_id);
      $uname = tripal_get_field_item_keyval($items, $delta, 'uniquename', $uname);
    }
    
    $schema = chado_get_schema('pub');
    
    $widget['#table_name'] = $table_name;
    $widget['#fkey_field'] = $fkey;
    $widget['#theme'] = 'schema__publication_widget';
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";
    
    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    
    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget['chado-' . $table_name . '__pub_id'] = array(
      '#type' => 'value',
      '#default_value' => $pub_id,
    );
    
    $widget['uniquename'] = array(
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
    // Get the FK column that links to the base table.
    $table_name = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    $field_name = $this->field['field_name'];
    
    // Get the field values.
    $fkey_value = isset($form_state['values'][$field_name][$langcode][$delta]['value']) ? $form_state['values'][$field_name][$langcode][$delta]['value'] : '';
    $pub_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] : '';
    $uname = isset($form_state['values'][$field_name][$langcode][$delta]['uniquename']) ? $form_state['values'][$field_name][$langcode][$delta]['uniquename'] : '';
    
    // If the user provided a uniquename then we want to set the foreign key
    // value to be the chado_record_id
    if ($uname and !$pub_id) {
      $pub = chado_generate_var('pub', array('uniquename' => $uname));
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] = $pub->pub_id;
    }
    
    // In the widgetFrom function we automatically add the foreign key
    // record.  But if the user did not provide a publication we want to take
    // it out so that the Chado field_storage infrastructure won't try to
    // write a record.
    if (!$uname and !$pub_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
    }
    
    // If the user removed the publication from the pub_uniquename field
    // then we want to clear out the rest of the hidden values.
    // Leave the primary key so the record can be deleted.
    if (!$uname and $pub_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] = '';
    }
  }
}

/**
 * An Ajax callback for the pub widget.
 */
function schema__publication_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}
/**
 * Theme function for the pub widget.
 *
 * @param $variables
 */
function theme_schema__publication_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
        drupal_render($element['uniquename']) . "
        </div>
      </div>
    ";

        return $layout;
}
