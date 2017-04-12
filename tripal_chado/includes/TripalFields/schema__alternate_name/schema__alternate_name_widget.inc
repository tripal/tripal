<?php

class schema__alternate_name_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Synonyms';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('schema__alternate_name');

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
    $fkey_value = array_key_exists('#entity', $element) and $element['#entity'] ? $element['#entity']->chado_record_id : NULL;
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
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__' . $pkey];
      $synonym_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__synonym_id'];
      $pub_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__pub_id'];
      $is_current = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__is_current'];
      $is_internal = $form_state['values'][$field_name]['und'][$delta]['chado-' . $table_name . '__is_internal'];
      $syn_name = $form_state['values'][$field_name]['und'][$delta]['name'];
      $syn_type = $form_state['values'][$field_name]['und'][$delta]['type_id'];
    }

    $options = array();
    $value = array('cv_id' => array('name' => 'synonym_type'));
    $op = array('return_array' => 1);
    $types = chado_generate_var('cvterm', $value, $op);
    if ($types) {
      foreach($types AS $type) {
        $options[$type->cvterm_id] = $type->name;
      }
    }

    // Get the schema for the synonym table so we can make sure we limit the
    // size of the name field to the proper size.
    $schema = chado_get_schema('synonym');

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
      '#default_value' => $record_id,
    );
    $widget['chado-' . $table_name . '__synonym_id'] = array(
      '#type' => 'value',
      '#default_value' => $synonym_id,
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    // TODO: add a widget for selecting a publication.
    $widget['chado-' . $table_name . '__pub_id'] = array(
      '#type' => 'value',
      '#default_value' => $pub_id,
    );
    $widget['type_id'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $options,
      '#default_value' => $syn_type,
    );
    $widget['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Synonym Name'),
      '#default_value' => $syn_name,
      '#size' => 25,
    );

    $widget['chado-' . $table_name . '__is_current'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is Current'),
      '#default_value' => $is_current,
      '#required' => $element['#required'],
    );

    $widget['chado-' . $table_name . '__is_internal'] = array(
      '#type' => 'checkbox',
      '#title' => t('Is Internal'),
      '#default_value' => $is_internal,
      '#required' => $element['#required'],
    );
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
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

    $record_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $pkey]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $pkey] : '';
    $fkey_value = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey]) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] : '';
    $synonym_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__synonym_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__synonym_id'] : '';
    $pub_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] : '';
    $is_current = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__is_current']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__is_current'] : '';
    $is_internal = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__is_internal']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__is_internal'] : '';
    $syn_name = isset($form_state['values'][$field_name][$langcode][$delta]['name']) ? $form_state['values'][$field_name][$langcode][$delta]['name'] : '';
    $syn_type = isset($form_state['values'][$field_name][$langcode][$delta]['type_id']) ? $form_state['values'][$field_name][$langcode][$delta]['type_id'] : '';

    // If the user provided a $syn_name and a $syn_type then we want to set
    // the foreign key value to be the chado_record_id.
    if ($syn_name and $syn_type) {

      // Get the synonym. If one with the same name and type is already present
      // then use that. Otherwise, insert a new one.
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
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__synonym_id'] = $synonym->synonym_id;

      if (!$pub_id) {
        $pub = chado_generate_var('pub', array('uniquename' => 'null'));
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] = $pub->pub_id;
      }
    }
    else {
      // If the $syn_name is not set, then remove the linker FK value to the base table.
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__synonym_id'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__is_internal'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__is_current'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__pub_id'] = '';
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  public function theme($element) {

    // These two fields were added to the widget to help identify the fields
    // for layout.
    $table_name = $element['#table_name'];
    $fkey = $element['#fkey_field'];

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
