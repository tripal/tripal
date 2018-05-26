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
    $pkey_val = '';
    $fkey_value = '';
    $pub_id = '';
    $title = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $pkey_val =  tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $pkey, $pkey_val);
      $fkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $fkey, $fkey_value);
      $pub_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__pub_id', $pub_id);
      if ($pub_id) {
        $pub = chado_get_publication(array('pub_id' => $pub_id));
        $title =  $pub->title . ' [id:' . $pub->pub_id . ']';
      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
        array_key_exists($field_name, $form_state['values'])) {
      $title = $form_state['values'][$field_name]['und'][$delta]['pub_title'];
      $pub_id = $form_state['values'][$field_name]['und'][$delta]['accession'];
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
      '#type' => 'value',
      '#default_value' => $pkey_val,
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget['chado-' . $table_name . '__pub_id'] = array(
      '#type' => 'value',
      '#default_value' => $pub_id,
    );

    $widget['pub_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Publication'),
      '#default_value' => $title,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/pub',
      '#maxlength' => 100000,
    );
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    // Get the FK column that links to the base table.
    $table_name = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    $field_name = $this->field['field_name'];

    // Get the field values.
    $fkey_value = $form_state['values'][$field_name]['und'][$delta]['value'];
    $pub_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'];
    $title = $form_state['values'][$field_name]['und'][$delta]['pub_title'];

    // If the user provided a pub_title then we want to set the foreign key
    // value to be the chado_record_id
    if ($title) {
      $matches = array();
      if (preg_match('/^.*\[id:(\d+)]$/', $title, $matches)) {
        $pub_id = $matches[1];
        $pub = chado_generate_var('pub', array('pub_id' => $pub_id));
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'] = $pub->pub_id;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $pub->pub_id;
      }
    }

    // In the widgetFrom function we automatically add the foreign key
    // record.  But if the user did not provide a publication we want to take
    // it out so that the Chado field_storage infrastructure won't try to
    // write a record.
    if (!$title) {
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'] = '';
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  function theme($element) {

    $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
          drupal_render($element['pub_title']) . "
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

