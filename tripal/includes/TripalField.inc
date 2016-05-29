<?php


/**
 * A base class for all fields supported by Tripal.
 *
 * The Field API of Drupal defines three "levels" for fields:  field types,
 * fields, and instances of fields.  All fields must be of a specific type, and
 * the field types are typically defined using the hook_field_info() hook.
 * Normally, using Drupal's Field API, fields can be created by using the
 * field_create_field() function which defines the parameters and settings
 * for the field.  The field_create_instance() function is then used to attach
 * a field to a bundle and to set local parameters and settings for the field
 * when attached to the bundle.  There are also a variety of hooks for creating
 * widgets, formatters, customizaing settings forms, loading values, validating
 * widget forms, etc. Rather than use all of these hooks, the TripalField class
 * is used to consolidate and simplify creation and management of Fields.
 *
 * A module can extend this class to create new fields, and attach them to
 * bundles.  The class is structure to allow fields to attach themselves to
 * bundles. This is a bit different from how fields would normally be
 * attached. But allows a field to be self-aware and all of the functionality
 * for a field is self-contained in the Class implementation.  To change
 * functionality a developer need only edit the class file for a field rathaer
 * than look for all of the Field API hook that would typically be spread
 * around the module.
 *
 * This field also supports use of controlled vocabulaaries for providing
 * "types" to these fields. This is important for use with the semantic web
 * support for Tripal v3.
 *
 * AJAX callbacks, and theme functions unfortunately cannot be part of the
 * implementation of this class.  To keep all functionality for a field
 * in the same file, it is recommended that those functions, if needed, should
 * be added to the bottom of the file where the child class is housed, and each
 * TripalField child class should each be written in a separate file.
 *
 */
class TripalField {
  /**
   * Define this field type.
   *
   * @return
   * An array whose keys are field type names and whose values are arrays
   * describing the field type. The keys are the same as for the
   * hook_field_info() function, which are:
   *   - label: The human-readable name of the field type.
   *   - description: A short description for the field type.
   *   - settings: An array whose keys are the names of the settings available
   *     for the field type, and whose values are the default values for those
   *     settings.
   *   - instance_settings: An array whose keys are the names of the settings
   *     available for instances of the field type, and whose values are the
   *     default values for those settings. Instance-level settings can have
   *     different values on each field instance, and thus allow greater
   *     flexibility than field-level settings. It is recommended to put
   *     settings at the instance level whenever possible. Notable
   *     exceptions: settings acting on the schema definition, or settings
   *     that Views needs to use across field instances (for example, the
   *     list of allowed values).
   *   - default_widget: The machine name of the default widget to be used by
   *     instances of this field type, when no widget is specified in the
   *     instance definition. This widget must be available whenever the field
   *     type is available (i.e. provided by the field type module, or by a
   *     module the field type module depends on).  Valid names are those
   *     provided by the widget_info() function of this class.
   *   - default_formatter: The machine name of the default formatter to be
   *     used by instances of this field type, when no formatter is specified
   *     in the instance definition. This formatter must be available whenever
   *     the field type is available (i.e. provided by the field type module,
   *     or by a module the field type module depends on).  Valid names are
   *     those provided by the formater_info() function of this class.
   *   - no_ui: (optional) A boolean specifying that users should not be allowed
   *     to create fields and instances of this field type through the UI.
   *     Such fields can only be created programmatically with
   *     field_create_field() and field_create_instance(). Defaults to
   *     FALSE.
   */
  public function field_info() {
    return array(
    );
  }


  /**
   * Provides the information required for creating a field.
   *
   * These settings are global for every instance of the field.
   *
   * he TripalField class allows a field to decide which bundles it would
   * like to attach itself to. Therefore, the entity type, and bundle are
   * passed as arguments to allow the field to decide if it wants to be
   * attached.
   *
   * @param $entity_type
   *   The class name for the entity type (e.g. TripalEntity).
   * @param TripalBundle $bundle
   *   The TripalBundle object.  This is the bundle that is being offered
   *   to the field for attachement. The function will determine if this
   *   field should be attached to this bundle.
   * @param $details
   * @return
   *   A field definition array. The return value is identical to that
   *   provided to the field_create_info() function. The field_name and
   *   type properties are required. The semantic_web value is also required
   *   under the settings. Other properties, if omitted, will be
   *   given the following default values:
   *     - cardinality: 1
   *     - locked: FALSE. Set to TRUE if you do not want your field to be
   *       re-used or deleted.
   *     - settings: each omitted setting is given the default value
   *       defined in field_info().
   *       - semantic_web: a controlled vocabulary term written in the form
   *         [CV short name]:[accession] (e.g. SO:000745).
   *     - storage:
   *       - type: the storage backend specified in the 'field_storage_default'
   *         system variable.
   *       - settings: each omitted setting is given the default value specified
   *         in hook_field_storage_info().
   *
   *  Nothing is returned when this field will not be attached to the bundle.
   */
  public function create_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }
  }

  /**
   * Provides the information required for creating an instance of a field.
   *
   * A field instance is a field that has been created using the values provided
   * by the create_info() function of this class, and which is attached to
   * a bundle.
   *
   * The TripalField class allows a field to decide which bundles it would
   * like to attach itself to. Therefore, the entity type, and bundle are
   * passed as arguments to allow the field to decide if it wants to be
   * attached and if so then this function will provide the details to create
   * the field.  The field instance can later be attached to the bundle
   * using information provided by the create_instance_info() function.
   *
   * @param $entity_type
   *   The entity type Class to which the bundle belongs.
   * @param $bundle.
   *   An instance of a TripalBundle object.  This is the bundle to which
   *   the field can be added.
   * @param $details
   *   An associative array containing additional details provided by the
   *   calling module that can be used by the function to determine if the
   *   bundle should be attached to.
   *
   * @return
   *  A field instance definition array. The values of thie array are identical
   *  to those passed to the field_create_instance() function. The field_name,
   *  entity_type and bundle (name of the bundle) properties are required.
   *  Other properties, if omitted, will be given the following default values:
   *    - label: the field name
   *    - description: empty string
   *    - required: FALSE
   *    - default_value_function: empty string
   *    - settings: each omitted setting is given the default value specified
   *      in hook_field_info().
   *    - widget:
   *      - type: the default widget specified in field_info().
   *      - settings: each omitted setting is given the default value specified
   *        in widget_info().
   *    - display: An instance can support multiple view modes. Eeach mode must
   *      be listed as a separate key (e.g. 'default') with a the following
   *      values (and their default specified below.  If display is not
   *      included then the settings for the 'default' view mode will be added
   *      automatically, and each view mode in the definition will be completed
   *      with the following default values:
   *      - label: 'above'
   *      - type: the default formatter specified in field_info().
   *      - settings: each omitted setting is given the default value specified
   *        in formatter_info().
   *      View modes not present in the definition are left empty, and the
   *      field will not be displayed in this mode.
   *
   *  Nothing is returned when this field will not be attached to the bundle.
   */
  public function create_instance_info($entity_type, $bundle, $details) {
    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }
  }



  /**
   * A helper function to determine if this field wants to attach to a bundle.
   *
   * Because the create_info() and create_instance_info() functions both need
   * to determine if they should attach to a bundle, this function is provided
   * as a helper function to perform that check.  Any class that extends the
   * TripalField class can use this function provide a single function that
   * both create_info() and create_instance_info() can use.
   *
   * @param $entity_type
   *   The entity type Class to which the bundle belongs.
   * @param $bundle.
   *   An instance of a TripalBundle object.  This is the bundle to which
   *   the field can be added.
   * @param $details
   *   An associative array containing additional details provided by the
   *   calling module that can be used by the function to determine if the
   *   bundle should be attached to.
   *
   * @return
   *   TRUE if the field should attach to the bundle. FALSE if not.
   */
  protected function can_attach($entity_type, $bundle, $details) {

  }

  public function attach_info($entity_type, $bundle, $settings) {

  }

  /**
   *  Provides information about the widgets provided by this field.
   *
   *  This function returns an array describing the widget types implemented by
   *  the field.  The widgets created by this field are expecte to be used only
   *  by this field.
   *
   * @param $entity_type
   *   The entity type Class to which the bundle belongs.
   * @param $bundle.
   *   An instance of a TripalBundle object.  This is the bundle to which
   *   the field can be added.
   * @param $details
   *   An associative array containing additional details provided by the
   *   calling module that can be used by the function to determine if the
   *   bundle should be attached to.
   *
   * @return
   *   An associative array where the keys are widget type names. To avoid
   *   name clashes, widget type names should be prefixed with the name of
   *   the module that exposes them. The values are arrays describing the
   *   widget type, with the following key/value pairs:
   *
   *     - label: The human-readable name of the widget type.
   *     - description: A short description for the widget type.
   *     - field types: An array of field types the widget supports.
   *     - settings: An array whose keys are the names of the settings
   *       available for the widget type, and whose values are the default
   *       values for those settings.
   *     - behaviors: (optional) An array describing behaviors of the widget,
   *       with the following elements:
   *       - multiple values: One of the following constants:
   *          - FIELD_BEHAVIOR_DEFAULT: (default) If the widget allows the
   *            input of one single field value (most common case). The widget
   *            will be repeated for each value input.
   *          - FIELD_BEHAVIOR_CUSTOM: If one single copy of the widget can
   *            receive several field values. Examples: checkboxes, multiple
   *            select, comma-separated textfield.
   *        - default value: One of the following constants:
   *          - FIELD_BEHAVIOR_DEFAULT: (default) If the widget accepts
   *            default values.
   *          - FIELD_BEHAVIOR_NONE: if the widget does not support
   *            default values.
   *     - weight: (optional) An integer to determine the weight of this
   *       widget relative to other widgets in the Field UI when selecting a
   *       widget for a given field instance.
   */
  public function widget_info() {
    return array(
    );
  }
  /**
   * Provides information about the formatter for this field.
   *
   * @return
   *   An associative array with key/value paris compatible with those from the
   *   hook_field_formatter_info() function of the Drupal Field API.
   *
   */
  public function formatter_info() {
    return array(
    );
  }
  /**
   * Provides a summary of the formatter settings.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function formatter_settings_summary($field, $instance, $view_mode) {

  }

  /**
   * Provides the field's setting form.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   * @param $form
   * @param $form_state
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function formatter_settings_form($field, $instance,
      $view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   *  @param $element
   *  @param $entity_type
   *  @param $entity
   *  @param $field
   *  @param $instance
   *  @param $langcode
   *  @param $items
   *  @param $display
   *
   *  @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    foreach($items as $delta => $item) {
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
      );
    }
  }

  /**
   * Provides the form for editing of this field.
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
   * therefore, the Tripal Chado modules does not use the 'value' but adds
   * additional elements to help link records via FKs.  But even in this case
   * the 'value' element must always be present in the returne form and in such
   * cases it's value should be set equal to that added in the 'load' function.
   *
   * @param $widget
   * @param $form
   * @param $form_state
   * @param $field
   * @param $instance
   * @param $langcode
   * @param $items
   * @param $delta
   * @param $element
   *
   * @return
   *   A Drupal form. See the hook_field_widget_form() function for more information.
   */
  public function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
  }

  /**
   *  Perform validation of the widget_form when adding or editing the entity.
   *
   *  Any errors encountered should be indicatd by adding a value to the $errors
   *  array according to the instructions below.
   *
   *  @param $entity_type
   *    The type of $entity.
   *  @param $entity
   *    The entity for the operation.
   *  @param $field
   *    The field structure for the operation.
   *  @param $instance
   *    The instance structure for $field on $entity's bundle.
   *  @param $langcode
   *    The language associated with $items.
   *  @param $items
   *    $entity->{$field['field_name']}[$langcode], or an empty array if unset.
   *  @param $errors
   *    The array of errors (keyed by field name, language code, and delta) that
   *    have already been reported for the entity. The function should add its
   *    errors to this array. Each error is an associative array with the
   *    following keys and values:
   *      - error: An error code (should be a string prefixed with the
   *        module name).
   *      - message: The human readable message to be displayed.
   *
   */
  public function validate($entity_type, $entity, $field, $instance, $langcode,
      $items, &$errors) {

  }

  /**
   *  Performs extra commands when the entity form is submitted.
   *
   *  Drupal typically does not provide a submit hook for fields.  The
   *  TripalField provides one to allow for behind-the-scenes actions to
   *  occur.   This function should never be used for updates, deletes or
   *  inserts into the storage backend. Rather, the appropriate Field Storage
   *  implementation will take care of that. An example where this function
   *  may be useful would be to set values in the $items array using values
   *  of the other.
   *
   *  @param $entity_type
   *    The type of $entity.
   *  @param $entity
   *    The entity for the operation.
   *  @param $field
   *    The field structure for the operation.
   *  @param $instance
   *    The instance structure for $field on $entity's bundle.
   *  @param $langcode
   *    The language associated with $items.
   *  @param $items
   *    $entity->{$field['field_name']}[$langcode], or an empty array if unset.
   *  @param $form
   *    The submitted form array.
   *  @param $form_state.
   *    The form state array.
   */
  public function submit($entity_type, $entity, $field, $instance, $langcode,
      &$items, $form, &$form_state) {
  }
  /**
   * Loads the field values from the underlying data store.
   *
   * @param $field
   * @param $entity
   * @param $details
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the enity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($field, $entity, $details) {

  }

  /**
   * Provides a form for the 'Field Settings' of the field management page.
   *
   * This is an optional hook function and is similar to the
   * hook_field_settings_form function().
   *
   * @param $field
   *   The field structure being configured.
   * @param $instance
   *   The instance structure being configured.
   * @param $has_data
   *   TRUE if the field already has data, FALSE if not.
   */
  public function settings_form($field, $instance, $has_data) {
    $settings = $field['settings'];
    $element = array();

//     $element['semantic_web'] = array(
//       '#type' => 'textfield',
//       '#title' => 'Semantic Web',
//       '#description' => t('Each field must be associated with a term
//           from a controlled vocabulary.  This allows computer programs to understand
//           the data provided on this site.  Please be cautions changing these
//           values.  Defaults are set by Tripal and sites that use the same
//           terms can exchange information.'),
//       '#collapsed' => TRUE,
//       '#collapsible' => TRUE,
//       '#tree' => TRUE,
//     );


    return $element;
  }

  /**
   * Describes this fields "data tables" to Views.
   *
   * This function is the equivalent of the hook_views_data() function of
   * the Drupal Views API.  It provides the necessary details to allow
   * Views to integrate the field.
   *
   * @return
   *   An associative array describing the data structure of the field.
   */
  public function views_data_alter(&$data, $field, $entity_info) {

  }

}