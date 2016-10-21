<?php


/**
 * A base for fields attached Tripal Entities.
 *
 * This class is intended to simplify development of fields for Tripal Entities.
 * The Drupal Field API can still be used if desired, but the hope for this
 * class it to put the necessary functions in one place so that other Tripal
 * developers do not need to suffer the pain of navigating and learning the
 * Drupal Field API.
 *
 * To create a new field that can be attached to a Tripal Entity follow these
 * steps:
 * # Create a new class that inherits from TripalField
 * # Copy the editable constant variables (default_desription, default_label,
 *   default_settings and default_storage) to your class and edit as needed.
 *   Be sure not to rename these variables and be sure to keep the 'static'
 *   qualifier on them.
 * # Copy the functions you want to override. You will not need to copy the
 *   constructor, the static info() functions, or the getters and setters. In
 *   short you'll typically only need to override the Settingsform funtions,
 *   their validators and submitters, the load() function, and the widgetForm
 *   with it's validator and submitter.
 * # In your custom module implement the function hook_create_tripalfields()
 *   This function will be called anytime a new TripalEntity is created. It
 *   allows your module to create the new fields.  See the documentation for
 *   this function for creating the fields.  A field is usually only ever
 *   created once and can be reused on multiple entities.  So, even though
 *   this function is called everytime a new TripalEntity is created the fields
 *   will only be created once.
 * # In your custom module implement the function
 *   hook_create_tripalfield_instance().  This function is called anytime a
 *   new TripalEntity is created. It allows you to specify which fields are
 *   attached to an entity.  See the documentation for this hook function for
 *   more information.
 *
 */
class TripalField {



  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Tripal Field.';

  // The default description for this field.
  public static $default_description = 'The generic base class for all
       Tripal Fields. Replace this text as appropriate for the child implementation.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $default_settings = array();

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  public static $default_instance_settings  = array();

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'tripal_no_storage';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;
  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  // --------------------------------------------------------------------------
  //                     CONSTRUCTORS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------

  /**
   * Instantiates a new TripalField object.
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

  // --------------------------------------------------------------------------
  //           STATIC INFO FUNCTIONS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------

  /**
   * Provides default information about this field type
   *
   * This function corresponds to the hook_field_info() function of
   * the Drupal Field API.
   *
   * @return
   *   An array whose keys are field type names and whose values are arrays
   *   describing the field type. The keys are the same as for the
   *   hook_field_info() function.
   */
  public static function globalInfo() {
    $field_type = get_called_class();
    return array(
      'label' => $field_type::$default_label,
      'description' => $field_type::$default_description,
      'default_widget' => $field_type . '_widget',
      'default_formatter' => $field_type . '_formatter',
      'settings' => $field_type::$default_settings,
      'instance_settings' => $field_type::$default_instance_settings,
      'storage' => array(
        'type' => $field_type::$default_storage,
        'module' => 'tripal',
        'active' => TRUE
      ),
    );
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
  public static function widgetInfo() {
    $field_type = get_called_class();
    return array(
      $field_type . '_widget' => array(
        'label' => $field_type::$default_label,
        'field types' => array($field_type)
      ),
    );
  }

  /**
   * Provides information about the formatter for this field.
   *
   * This function corresponds to the hook_field_formatter_info() function of
   * the Drupal Field API.
   *
   * This is a static function as it provides default values for all of the
   * formatters for this field type, and thus we don't need an instantiated
   * object to provide this information.
   *
   * @return
   *   An associative array with key/value paris compatible with those from the
   *   hook_field_formatter_info() function of the Drupal Field API.
   *
   */
  public static function formatterInfo() {
    $field_type = get_called_class();
    return array(
      $field_type . '_formatter' => array(
        'label' => $field_type::$default_label,
        'field types' => array($field_type),
        'settings' => array(),
      ),
    );
  }

  // --------------------------------------------------------------------------
  //                 GETTERS AND SETTERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------

  /**
   * Retrives the name of this field.
   *
   * @return
   *   This field's name.
   */
  public function getFieldName() {
    return $this->field['field_name'];
  }

  public function getField() {
    return $this->field;
  }
  public function getInstance() {
    return $this->instance;
  }


  // --------------------------------------------------------------------------
  //                 OVERRIDEABLE FUNCTIONS
  // --------------------------------------------------------------------------

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
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
  public function formatterSettingsSummary($view_mode) {

  }

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
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
  public function formatterSettingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
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
   *  @param $langcode
   *  @param $items
   *  @param $display
   *
   *  @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

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
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['#field'] = $this->field;
    $widget['#element_validate'] = array('tripal_field_widget_form_validate');
  }

  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function widgetFormValidate($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }

  /**
   *  Perform validation of the field regardless how it is updated.
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
  public function validate($entity_type, $entity, $field, $items, &$errors) {

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
   * field allows the user to provide a brand new synonynm and it must add it
   * to the chado.synonym table prior to the record in the
   * chado.feature_synonym table.  This insert occurs in the widgetFormSubmit
   * function.
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
  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }

  /**
   * Loads the field values from the underlying data store.
   *
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
  public function load($entity, $details = array()) {

  }

  /**
   * Provides a form for the 'Field Settings' of an instance of this field.
   *
   * This function corresponds to the hook_field_instance_settings_form()
   * function of the Drupal Field API.
   *
   * Validation of the instance settings form is not supported by Drupal, but
   * the TripalField class does provide a mechanism for supporting validation.
   * To allow for validation of your setting form you must call the parent
   * in your child class:
   *
   * @code
   *   $element = parent::instanceSettingsForm();
   * @endcode
   *
   * Please note, the form generated with this function does not easily
   * support AJAX calls in the same way that other Drupal forms do.  If you
   * need to use AJAX you must manually alter the $form in your ajax call.
   * The typical way to handle updating the form via an AJAX call is to make
   * the changes in the form function itself but that doesn't work here.
   */
  public function instanceSettingsForm() {
    $settings = $this->instance['settings'];
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
    $element['#field'] = $this->field;
    $element['#instance'] = $this->instance;
    $element['#element_validate'][] = 'tripal_field_instance_settings_form_validate';

    return $element;
  }
  /**
   * Provides validation of the instance settings form.
   *
   * There is no equivalent function in the Drupal Field API. Validation
   * of instance settings forms in Drupal is not supported. However, the
   * TripalField provides this function to fill the gap.  See the
   * documentation for the instanceSettingsForm() function for instructions
   * to support use of this function.
   *
   * @param $form
   * @param $form_state
   */
  public function instanceSettingsFormValidate($form, &$form_state) {

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
  public function globalSettingsForm($has_data) {
    $settings = $this->field['settings'];
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
    $element['#field'] = $this->field;
    $element['#instance'] = $this->instance;
    $element['#element_validate'][] = 'tripal_field_settings_form_validate';

    return $element;
  }
  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function globalSettingsFormValidate($form, &$form_state) {

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
  public function viewsDataAlter(&$data, $entity_info) {

  }

}