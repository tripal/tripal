<?php

class schema__alternate_name_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Synonyms';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__alternate_name'];

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
    $record_id = '';
    $fkey_value = (array_key_exists('#entity', $element) and $element['#entity']) ? $element['#entity']->chado_record_id : NULL;
    $synonym_id = '';
    $pub_id = '';
    $is_current = TRUE;
    $is_internal = FALSE;
    $syn_name = '';
    $syn_type = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      $record_id = $items[$delta]['chado-' . $table_name . '__' . $pkey];
      $synonym_id = $items[$delta]['chado-' . $table_name . '__synonym_id'];
      $pub_id = $items[$delta]['chado-' . $table_name . '__pub_id'];
      $is_current = $items[$delta]['chado-' . $table_name . '__is_current'];
      $is_internal = $items[$delta]['chado-' . $table_name . '__is_internal'];
      $syn_name = $items[$delta]['name'];
      $syn_type = $items[$delta]['type_id'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $synonym_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__synonym_id'];
      $pub_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'];
      $is_current = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__is_current'];
      $is_internal = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__is_internal'];
      $syn_name = $form_state['values'][$field_name]['und'][$delta]['name'];
      $syn_type = $form_state['values'][$field_name]['und'][$delta]['type_id'];
    }

    $options = [];
    $value = ['cv_id' => ['name' => 'synonym_type']];
    $op = ['return_array' => 1];
    $types = chado_generate_var('cvterm', $value, $op);
    if ($types) {
      foreach ($types AS $type) {
        $options[$type->cvterm_id] = $type->name;
      }
    }

    // Get the schema for the synonym table so we can make sure we limit the
    // size of the name field to the proper size.
    $schema = chado_get_schema('synonym');

    $widget['#prefix'] = "<span id='$table_name-$delta'>";
    $widget['#suffix'] = "</span>";
    $widget['#table_name'] = $table_name;

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    $widget['chado-' . $table_name . '__' . $pkey] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-' . $table_name . '__synonym_id'] = [
      '#type' => 'value',
      '#default_value' => $synonym_id,
    ];
    $widget['chado-' . $table_name . '__' . $fkey] = [
      '#type' => 'value',
      '#default_value' => $fkey_value,
    ];
    // TODO: add a widget for selecting a publication.
    $widget['chado-' . $table_name . '__pub_id'] = [
      '#type' => 'value',
      '#default_value' => $pub_id,
    ];
    $widget['type_id'] = [
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $options,
      '#default_value' => $syn_type,
    ];
    $widget['name'] = [
      '#type' => 'textfield',
      '#title' => t('Synonym Name'),
      '#default_value' => $syn_name,
      '#size' => 25,
    ];

    $widget['chado-' . $table_name . '__is_current'] = [
      '#type' => 'checkbox',
      '#title' => t('Is Current'),
      '#default_value' => $is_current,
      '#required' => $element['#required'],
    ];

    $widget['chado-' . $table_name . '__is_internal'] = [
      '#type' => 'checkbox',
      '#title' => t('Is Internal'),
      '#default_value' => $is_internal,
      '#required' => $element['#required'],
    ];
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->instance['settings']['chado_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    $pub_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'];
    $syn_name = $form_state['values'][$field_name]['und'][$delta]['name'];
    $syn_type = $form_state['values'][$field_name]['und'][$delta]['type_id'];

    // If the user provided a $syn_name and a $syn_type then we want to set
    // the foreign key value to be the chado_record_id.
    if ($syn_name and $syn_type) {
      // Set the synonym_id and FK value
      $synonym = chado_generate_var('synonym', [
        'name' => $syn_name,
        'type_id' => $syn_type,
      ]);
      if ($synonym) {
        $form_state['values'][$field_name]['und'][$delta]['value'] = $synonym->name;
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__synonym_id'] = $synonym->synonym_id;
      }
      else {
        $form_state['values'][$field_name]['und'][$delta]['value'] = $syn_name;
      }
      // If a publication is not provided then use the null pub.
      if (!$pub_id) {
        $pub = chado_generate_var('pub', ['uniquename' => 'null']);
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'] = $pub->pub_id;
      }
    }
    // If no snynonym name is provided then we want to delete the record.
    // To do this we have to set the value to something (here we just use
    // the arbitrary 'delete_me', and set every other element to empty other
    // than the pkey element.
    else {
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__synonym_id'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__is_internal'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__is_current'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'] = '';
    }
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $table_name = $this->instance['settings']['chado_table'];

    $syn_name = $form_state['values'][$field_name]['und'][$delta]['name'];
    $syn_type = $form_state['values'][$field_name]['und'][$delta]['type_id'];

    // If the user provided a $syn_name and a $syn_type then we want to set
    // the foreign key value to be the chado_record_id.
    if ($syn_name and $syn_type) {

      // Get the synonym. If one with the same name and type is already present
      // then use that. Otherwise, insert a new one.
      $synonym = chado_generate_var('synonym', [
        'name' => $syn_name,
        'type_id' => $syn_type,
      ]);
      if (!$synonym) {
        $synonym = chado_insert_record('synonym', [
          'name' => $syn_name,
          'type_id' => $syn_type,
          'synonym_sgml' => '',
        ]);
        $synonym = (object) $synonym;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $synonym->name;
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__synonym_id'] = $synonym->synonym_id;
      }
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  public function theme($element) {

    // These two fields were added to the widget to help identify the fields
    // for layout.
    $table_name = $element['#table_name'];

    $layout = "
      <div class=\"synonym-widget\">
        <div class=\"synonym-widget-item\">" .
      drupal_render($element['name']) . "
        </div>
        <div>" .
      drupal_render($element['type_id']) . "
        </div>
        <div class=\"synonym-widget-item\">" .
      drupal_render($element['chado-' . $table_name . '__is_internal']) . "
        </div>
        <div>" .
      drupal_render($element['chado-' . $table_name . '__is_current']) . "
        </div>
      </div>
    ";

    return $layout;
  }

}

/**
 * An Ajax callback for the synonym widget.
 */
function schema__alternate_name_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}
