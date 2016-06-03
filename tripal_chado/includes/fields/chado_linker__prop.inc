<?php

class chado_linker__prop extends TripalField {
  /**
   * @see TripalField::fieldInfo()
   */
  static function fieldInfo() {
    return array(
      'label' => t('Add a Property'),
      'description' => t('Add details about this property.'),
      'default_widget' => 'chado_linker__prop_widget',
      'default_formatter' => 'chado_linker__prop_formatter',
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
    // This field is only attached by the chado_linker__prop_addr field.
    $this->can_attach = FALSE;
  }
  /**
   * @see TripalField::create_info()
   */
  function createInfo() {
    if (!$this->can_attach) {
      return;
    }

    // This field is only attached by the chado_linker__prop_addr field.

  }
  /**
   * @see TripalField::createInstanceInfo()
   */
  function createInstanceInfo() {
    if (!$this->can_attach) {
      return;
    }

    // This field is only attached by the chado_linker__prop_addr field.

  }
  /**
   * @see TripalField::widgetInfo()
   */
  public static function widgetInfo() {
    return array(
      'chado_linker__prop_widget' => array(
        'label' => t('Property'),
        'field types' => array('chado_linker__prop'),
      ),
    );
  }
  /**
   * @see TripalField::formatterInfo()
   */
  static function formatterInfo() {
    return array(
      'chado_linker__prop_formatter' => array(
        'label' => t('Property'),
        'field types' => array('chado_linker__prop'),
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

    $field_name = $field['field_name'];
    $chado_table = $field['settings']['chado_table'];

    $properties = array();
    foreach ($items as $delta => $item) {
      $properties[] = $item[$chado_table . '__value'];
    }
    $content = implode(', ', $properties);

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }
  /**
   * @see TripalField::widgetForm()
   */
  static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $entity = $form['#entity'];
    $field_name = $field['field_name'];

    // Get the record and table mapping info.
    $chado_table = $entity->chado_table;
    $chado_column = $entity->chado_column;
    $chado_record = $entity->chado_record;

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    // If the field name is not properly formatted then we can't tell what
    // table and type this is.  So just return.
    if (count($matches) != 3) {
      return $widget;
    }
    $table_name = $matches[1];
    $cvterm_id = $matches[2];

    // Get the name of the pkey field for this property table and the name
    // of the FK field that links to the base table.
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $lfkey_field = key($schema['foreign keys'][$chado_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$chado_table]['columns'][$lfkey_field];

    // Get the field defaults.
    $fk_value = '';
    $propval = '';
    if (array_key_exists($delta, $items)) {
      $propval = $items[$delta][$table_name . '__value'];
    }
    if ($chado_record) {
      $fk_value = $chado_record->$rfkey_field;
    }


    // The group of elements all-together need some extra functionality
    // after building up the full list (like draggable table rows).
    $widget['#theme'] = 'field_multiple_value_form';
    $widget['#title'] = $element['#title'];
    $widget['#description'] = $element['#description'];
    $widget['#field_name'] = $element['#field_name'];
    $widget['#language'] = $element['#language'];
    $widget['#weight'] = isset($element['#weight']) ? $element['#weight'] : 0;
    $widget['#element_validate'] = array('chado_linker__prop_widget_validate');
    $widget['#cardinality'] = 1;

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => $items[$delta]['value'],
    );

    $widget[$table_name . '__' . $pkey] = array(
      '#type' => 'hidden',
      '#default_value' => !empty($items[$delta]['value']) ? $items[$delta]['value'] : '',
    );
    $widget[$table_name . '__' . $lfkey_field] = array(
      '#type' => 'hidden',
      '#value' => $fk_value,
    );
    $widget[$table_name . '__value'] = array(
      '#type' => 'textfield',
      '#default_value' => $propval,
    );
    $widget[$table_name . '__type_id'] = array(
      '#type' => 'hidden',
      '#value' => $cvterm_id,
    );
    $widget[$table_name . '__rank'] = array(
      '#type' => 'hidden',
      '#value' => $delta,
    );
    return $widget;
  }


  /**
   * @see TripalField::validate()
   */
  static function widgetFormValidate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
    $field_name = $element['#field_name'];
    $delta = $element['#delta'];
    $entity = $element['#entity'];
    $matches = array();

    // Get the record and table mapping info.
    $chado_table = $entity->chado_table;
    $chado_column = $entity->chado_column;

    // Get the table name and cvterm_id for this field.
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    $table_name = $matches[1];
    $cvterm_id = $matches[2];

    // Get the name of the pkey field for this property table and the name
    // of the FK field that links to the base table.
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $lfkey_field = key($schema['foreign keys'][$chado_table]['columns']);

    // If we don't have a property value then we need to set all other fields
    // to be empty so that when the module tries to save the field on the
    // entity it won't try to save a partial record.
    $pkey_val = tripal_chado_get_field_form_values($field_name, $form_state, $delta);
    $prop_value = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . "__value");
    $fk_val = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__' . $lfkey_field);
    $type_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $table_name . '__type_id');

    if (!$prop_value) {
      tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__' . $lfkey_field);
      tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__value');
      tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__type_id');
      tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__rank');
    }
    else {
      $rank = tripal_chado_get_field_form_values($field_name, $form_state, $delta, '_weight');
      tripal_chado_set_field_form_values($field_name, $form_state, $rank, $delta, $table_name . '__rank');
    }
    // Remove the properties for this record. We will re-add it. Otherwise,
    // if we change ranks, we wind up with multiple records in the property table.
    if ($pkey_val) {
      $match = array(
        $pkey => $pkey_val
      );
      chado_delete_record($table_name, $match);
    }
  }
  /**
   * @see TripalField::load()
   */
  function load($field, $entity, $details) {

    $field_name = $details['field_name'];
    $field_type = $details['type'];
    $field_table = $details['settings']['chado_table'];
    $field_column = $details['settings']['chado_column'];

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    $table_name = $matches[1];
    $cvterm_id = $matches[2];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      $field_table . '__' . $pkey => '',
      $field_table . '__' . $fkey_lcolumn => '',
      $field_table . '__value' => '',
      $field_table . '__type_id' => '',
      $field_table . '__rank' => '',
    );

    // Get the properties associated with this base record for this fields
    // given type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $record->$fkey_rcolumn,
      'type_id' => $cvterm_id,
    );
    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC')
    );
    $properties = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($properties); $i++) {
      $property = $properties[$i];
      foreach ($schema['fields'] as $fname => $details) {
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(),
          $field_table . '__' . $pkey => $property->$pkey,
          $field_table . '__' . $fkey_lcolumn => $property->$fkey_lcolumn,
          $field_table . '__value' => $property->value,
          $field_table . '__type_id' => $property->type_id,
          $field_table . '__rank' => $property->rank,
        );
      }
    }
  }
}
/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function chado_linker__prop_widget_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  return $form[$field_name];
}