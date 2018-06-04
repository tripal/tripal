<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class sep__protocol_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Protocol';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sep__protocol'];

  /**
   * Provides the form for editing of this field.
   *
   * This function corresponds to the hook_field_widget_form()
   * function of the Drupal Field API.
   *
   * This form is diplayed when the user creates a new entity or edits an
   * existing entity.  If the field is attached to the entity then the form
   * provided by this function will be displayed.
   *
   * At a minimum, the form must have a 'value' element.  For Tripal, the
   * 'value' element of a field always corresponds to the value that is
   * presented to the end-user either directly on the page (with formatting)
   * or via web services, or some other mechanism.  However, the 'value' is
   * sometimes not enough for a field.  For example, the Tripal Chado module
   * maps fields to table columns and sometimes those columns are foreign keys
   * therefore, the Tripal Chado modules does not just use the 'value' but adds
   * additional elements to help link records via FKs.  But even in this case
   * the 'value' element must always be present in the return form and in such
   * cases it's value should be set equal to that added in the 'load' function.
   *
   * @param $widget
   * @param $form
   *   The form structure where widgets are being attached to. This might be a
   *   full form structure, or a sub-element of a larger form.
   * @param $form_state
   *   An associative array containing the current state of the form.
   * @param $langcode
   *   The language associated with $items.
   * @param $items
   *   Array of default values for this field.
   * @param $delta
   *   The order of this item in the array of subelements (0, 1, 2, etc).
   * @param $element
   * A form element array containing basic properties for the widget:
   *  - #entity_type: The name of the entity the field is attached to.
   *  - #bundle: The name of the field bundle the field is contained in.
   *  - #field_name: The name of the field.
   *  - #language: The language the field is being edited in.
   *  - #field_parents: The 'parents' space for the field in the form. Most
   *    widgets can simply overlook this property. This identifies the location
   *    where the field values are placed within $form_state['values'], and is
   *    used to access processing information for the field through the
   *    field_form_get_state() and field_form_set_state() functions.
   *  - #columns: A list of field storage columns of the field.
   *  - #title: The sanitized element label for the field instance, ready for
   *    output.
   *  - #description: The sanitized element description for the field instance,
   *    ready for output.
   *  - #required: A Boolean indicating whether the element value is required;
   *    for required multiple value fields, only the first widget's values are
   *    required.
   *  - #delta: The order of this item in the array of subelements; see
   *    $delta above
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);


    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    $protocol_term = chado_get_semweb_term($field_column, $field_table);
    $protocol_name_term = chado_get_semweb_term('protocol', 'name');
    $protocol_type_term = chado_get_semweb_term('protocol', 'type_id');    

    // Set defaults for the form elements.
    $protocol_name = '';
    $protocol_id = '';
    
    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $protocol_name = array_key_exists($protocol_name_term, $items[$delta]['value']) ? $items[$delta]['value'][$protocol_name_term] : $protocol_name;
    }
    
    // Get the list of protocols
    $protocols = [];
    $sql = "SELECT * FROM {protocol}";
    $results = chado_query($sql);
    foreach ($results as $protocol) {
      $protocols[$protocol->protocol_id] = $protocol->name;
      if ($protocol->name == $protocol_name) {
        $protocol_id = $protocol->protocol_id;
      }
    }
    
    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget[$linker_field] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $protocols,
      '#empty_option' => '- Select a Protocol -',
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
      '#default_value' => $protocol_id,
    ];
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    
    $linker_field = 'chado-' . $field_table . '__' . $field_column;    
    
    // Make sure the value is set to the organism_id
    $protocol_id = $form_state['values'][$field_name]['und'][0][$linker_field];
    $form_state['values'][$field_name]['und'][0]['value'] = $protocol_id;
  }

  /**
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
  }

}