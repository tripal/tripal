<?php

class schema__publication_widget extends ChadoFieldWidget {
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

    $field_name = $this->field['field_name'];

    // Get the FK column that links to the base table.
    $table_name = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $fkey_value = (array_key_exists('#entity', $element) and is_object($element['#entity'])) ? $element['#entity']->chado_record_id : NULL;
    $pkey_value = '';
    $pub_id = '';
    $uname = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $pkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $pkey, $pkey_value);
      $pub_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__pub_id', $pub_id);
      $uname = tripal_get_field_item_keyval($items, $delta, 'uniquename', $uname);
    }
    // We also need to take into account the case where a field is changed by a user.
    // In this case the info is in the form state.
    if (isset($form_state['values']) AND !empty($form_state['values'])) {
      // If the user entered a new publication then we only have the uniquename...
      // so we need to query the db for the rest of the information.
      if (empty($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'])) {
        $uname = $form_state['values'][$field_name][$langcode][$delta]['uniquename'];
        $pub_id = chado_query('SELECT pub_id FROM {pub} WHERE uniquename=:uniquename',
          array(':uniquename' => $uname))->fetchField();
      }
      // If the user changed an existing publication...
      // then we have to update the pub_id and reset the pkey.
      // We can tell if this element was changed by comparing it to the triggering element.
      $current_value = $form_state['values'][$field_name][$langcode][$delta]['value'];
      $triggering_value = $form_state['triggering_element']['#value'];
      if ($triggering_value == $current_value) {
        $uname = $form_state['values'][$field_name][$langcode][$delta]['uniquename'];
        $pub_id = chado_query('SELECT pub_id FROM {pub} WHERE uniquename=:uniquename',
          array(':uniquename' => $uname))->fetchField();
      }
    }

    $schema = chado_get_schema('pub');

    $widget['#table_name'] = $table_name;
    $widget['#fkey_field'] = $fkey;
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'hidden',
      '#value' => $pkey_value,
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'hidden',
      '#value' => $fkey_value,
    );
    $widget['chado-' . $table_name . '__pub_id'] = array(
      '#type' => 'hidden',
      '#value' => $pub_id,
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
   *
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $base_table = $this->instance['settings']['base_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $chado_column = $this->instance['settings']['chado_column'];
    $instance = $this->instance;

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    $value = $form_state['values'][$field_name]['und'][$delta]['uniquename'];
    $form_state['values'][$field_name]['und'][$delta]['value'] = $value;

    // If the user removed the publication then we want to clear out the other
    // fields except the pkey value. If the pkey field is present and the value
    // is present then the chado storage backend will delete the record.
    if (!$value) {
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__pub_id'] = '';
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  function theme($element) {

    $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
          drupal_render($element['uniquename']) . "
        </div>
      </div>
    ";

    return $layout;
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

