<?php

class TripalFieldWidget {

  // The default label for this field.
  public static $default_label = 'Tripal Field.';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['no_widget'];


  /**
   * Instantiates a new TripalFieldWidget object.
   *
   * @param $field
   *   An array containing the field data as returned by field_info_field()
   * @param $instance
   *   (Optional). Set the instance of this field when one is available. This
   *   is necessary when working with instance specific functions such as the
   *   formatterSettingsForm, widgetForm, etc.
   */
  public function __construct($field, $instance = NULL) {
    $this->field = $field;
    $this->instance = $instance;
  }

  /**
   * Provides information about the widgets provided by this field.
   *
   * This function corresponds to the hook_field_widget_info() function of
   * the Drupal Field API.
   *
   * This is a static function as it provides default values for all of the
   * widgets for this field type, and thus we don't need an instantiated
   * object to provide this information.
   *
   * @return
   *   An associative array with key/value pairs compatible with those from the
   *   hook_field_widget_info() function of the Drupal Field API.
   */
  public static function info() {
    $class = get_called_class();
    return [
      'label' => $class::$default_label,
      'field types' => $class::$field_types,
    ];
  }

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

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['#field'] = $this->field;
    $widget['#instance'] = $this->instance;
    $widget['#theme'] = 'tripal_field_default';
    $widget['#element_validate'] = ['tripal_field_widget_form_validate'];
  }

  /**
   * Performs validation of the widget form.
   *
   * Use this validate to ensure that form values are entered correctly.
   *
   * The 'value' key of this field must be set in the $form_state['values']
   * array anytime data is entered by the user.  It may be the case that there
   * are other fields for helping select a value. In the end those helper
   * fields must be used to set the 'value' field.
   *
   * TROUBLESHOOTING:
   *  If your widget doesn't appear to be saving data, check the following:
   *   - Is $form_state['values'] set to a TRUE value (e.g. a string)?
   *   - Do you have the keys needed by the storage backend? For example,
   *     ChadoFields need `chado-[tablename]__[columnname]` keys for all
   *     chado columns. Look at the storage backend for more documentation.
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

  }

  /**
   * Performs validation of the widget form when setting defaults.
   *
   * Use this validate to ensure that form values are entered correctly when
   * a user edits the defaults on the field edit page (available from the
   * "managed fields" section of the Content type page.
   */
  public function validateDefaults($element, $form, &$form_state, $langcode, $delta) {

  }

  /**
   * Performs extra commands when the entity form is submitted.
   *
   * Drupal typically does not provide a submit hook for fields.  The
   * TripalField provides one to allow for behind-the-scenes actions to
   * occur.   This function should never be used for updates, deletes or
   * inserts for the Chado table associated with the field.  Rather, the
   * storage backend should be allowed to handle inserts, updates deletes.
   * However, it is permissible to perform inserts, updates or deletions within
   * Chado using this function.  Those operations can be performed if needed but
   * on other tables not directly associated with the field.
   *
   * An example is the chado.feature_synonym table.  The chado_linker__synonym
   * field allows the user to provide a brand new synonym and it must add it
   * to the chado.synonym table prior to the record in the
   * chado.feature_synonym table.  This insert occurs in the widgetFormSubmit
   * function.
   *
   * @param $entity_type
   *    The type of $entity.
   * @param $entity
   *    The entity for the operation.
   * @param $field
   *    The field structure for the operation.
   * @param $instance
   *    The instance structure for $field on $entity's bundle.
   * @param $langcode
   *    The language associated with $items.
   * @param $items
   *    $entity->{$field['field_name']}[$langcode], or an empty array if unset.
   * @param $form
   *    The submitted form array.
   * @param $form_state .
   *    The form state array.
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }

  /**
   * The theme function for the widget.
   *
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
   *
   * @return
   *   A drupal renderable array or HTML or an empty string if no
   *   theming is to be applied.
   */
  public function theme($element) {
  }
}
