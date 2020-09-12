<?php

class schema__publication_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Publication';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__publication'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // These fields are used when the publications come through a linker table.
    $pkey = '';
    $fkeys = '';
    $fkey = '';
    $pkey_val = '';
    $fkey_value = '';
    $linker_table = '';

    // If the field table and the base table are not the same thing then
    // we are going through a linker table.
    $pub_item_id = '';
    if ($field_table != $base_table) {
      $schema = chado_get_schema($field_table);
      $pkey = $schema['primary key'][0];
      $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
      $fkey = $fkeys[0];
      $linker_table = $base_table . '_pub';
      $pub_item_id = 'chado-' . $field_table . '__pub_id';
    }
    else {
      $pub_item_id = 'chado-' . $field_table . '__' . $field_column;
    }

    // Get the field defaults.
    $pub_id = '';
    $title = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      if ($linker_table) {
        $pkey_val = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $pkey_val);
        $fkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey, $fkey_value);
      }
      $pub_id = tripal_get_field_item_keyval($items, $delta, $pub_item_id, $pub_id);
      if ($pub_id) {
        $pub = chado_get_publication(['pub_id' => $pub_id]);
        $title = $pub->title . ' [id:' . $pub->pub_id . ']';
      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $title = $form_state['values'][$field_name]['und'][$delta]['pub_title'];
      $pub_id = $form_state['values'][$field_name]['und'][$delta]['accession'];
    }

    $widget['#table_name'] = $field_table;
    $widget['#fkey_field'] = $fkey;
    $widget['#prefix'] = "<span id='" . $field_table . "-" . $delta . "'>";
    $widget['#suffix'] = "</span>";

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    if ($linker_table) {
      $widget['chado-' . $field_table . '__' . $pkey] = [
        '#type' => 'value',
        '#default_value' => $pkey_val,
      ];
      $widget['chado-' . $field_table . '__' . $fkey] = [
        '#type' => 'value',
        '#default_value' => $fkey_value,
      ];
    }
    $widget[$pub_item_id] = [
      '#type' => 'value',
      '#default_value' => $pub_id,
    ];

    $widget['pub_title'] = [
      '#type' => 'textfield',
      '#title' => t('Publication'),
      '#default_value' => $title,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/pub',
      '#maxlength' => 100000,
    ];
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    // Get the FK column that links to the base table.
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // If the user provided a pub_title then we want to set the foreign key
    // value to be the chado_record_id
    $title = $form_state['values'][$field_name]['und'][$delta]['pub_title'];
    if ($title) {
      $matches = [];
      if (preg_match('/^.*\[id:(\d+)]$/', $title, $matches)) {
        $pub_id = $matches[1];
        $pub = chado_generate_var('pub', ['pub_id' => $pub_id]);
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__pub_id'] = $pub->pub_id;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $pub->pub_id;
      }
    }
    else {
      $form_state['values'][$field_name]['und'][$delta]['value'] = '';

      // Clear out all other values if this is a linker table except the pkey. This
      // lets the Chado storage API know that this is an update.
      if ($field_table != $base_table) {
        $schema = chado_get_schema($field_table);
        $pkey = $schema['primary key'][0];
        $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__pub_id'] = '';
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey_lcolumn] = '';
      }
      else {
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__pub_id'] = '__NULL__';
      }
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

