<?php


/**
 * A base class for fields attached to Tripal Entities.
 *
 * The Field API of Drupal defines three "levels" for fields:  field types,
 * fields, and instances of fields. This class attempts to consolidate use
 * of all three "levels".  All fields must be of a specific type, and
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
 * bundles.  The class is structured to allow fields to specify which bundles
 * they want to "automatically" attach. This is a bit different from how fields
 * would normally be attached. But allows a field to be self-aware.
 *
 * All of  the functionality for a field is self-contained in the Class
 * implementation.  To change functionality a developer need only edit the
 * class file for a field rathaer than look for all of the Field API hook that
 * would typically be spread around the module.
 *
 * This class also supports use of controlled vocabulaaries for providing
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

  // An object containing configuration data for this field. The contents
  // of this object come directly from the field_config table of Drupal.
  protected $field;

  // --------------------------------------------------------------------------
  //                          STATIC CONSTANTS
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

  // Add any default settings elements.  If you override the fieldSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array();

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'tripal_no_storage';



  // --------------------------------------------------------------------------
  //           CONSTRUCTORS & STATIC CONSTRUCTOR HELPERS
  //
  // Child classes SHOULD NOT need to override these functions.
  // --------------------------------------------------------------------------

  /**
   * Instantiates a new TripalField object.
   *
   * @param $field
   *   If the field already exists in Drupal, then pass in the $field array
   *   for this argument.  If nothing is passed in then an "empty" field
   *   is created.  This is only useful
   *
   * The field must have already been previously created.
   */
  public function __construct($info = array()) {
    // If the field array has been passed in then just link it.
    if (array_key_exists('field', $info)) {
      $this->field = $field;
    }
    // If the field name has been passed in then retreive the form info.
    if (array_key_exists('field_name', $info)) {
      $this->field = field_info_field($field_name);
    }
    // If the field info has been passed in then create the field.
    if (array_key_exists('info', $info)) {
      $this->field = field_create_field($info['info']);
    }

    // Include any instances that have been created for this field.
    if (is_array($field) and array_key_exists('id', $this->field)) {
      $instances = db_select('field_config_instance', 'fci')
        ->fields('fci')
        ->condition('field_id', $field['id'])
        ->execute();

      while ($instance = $instances->fetchObject()) {
        $this->instances[$instance->field_name] = $instance;
      }
    }
  }

  // --------------------------------------------------------------------------
  //                      STATIC INFO FUNCTIONS
  //
  // Child classes SHOULD NOT need to override these functions.
  // --------------------------------------------------------------------------

  /**
   * Provides default information about this field type
   *
   * NOTE: this field should NOT be overridden by child classes.
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
  //                         GETTERS AND SETTERS
  //
  // Child classes SHOULD NOT need to override these functions.
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

  // --------------------------------------------------------------------------
  //                     FIELD SPECIFIC FUNCTIONS
  //
  // Child classes SHOULD NOT override these functions as needed.
  // --------------------------------------------------------------------------


  // --------------------------------------------------------------------------
  //                  OVERRIDEABLE FIELD SPECIFIC FUNCTIONS
  //
  // Child classes SHOULD override these functions as needed.
  // --------------------------------------------------------------------------

  /**
   * Performs a check to see disallow attaching of a field instance to a bundle.
   *
   * By default Tripal will try to automatically attach all TripalFields to
   * every bundle.  But this is certainly not appropriate.
   * This function should be overridden by a child to ensure it is attached
   * to only desired bundles.
   *
   * This function returns FALSE if this field should NOT be attached to the
   * bundle. This should always be honored by a child class. If the parent class
   * returns FALSE then so should the child.  Alternatively, this function
   * returns NULL if there is no good reason to deny attachment and leaves it up
   * to the child class to decide.  If the child class does not impolment
   * this function then the NULL is recognized as FALSE by Tripal and the
   * field will not be attached to the bundle.  Therefore, it is necessary
   * for the child class to implement this function and return TRUE for
   * bundles to which this field should be attached.  Because bundles names
   * use controlled vocabulary terms the child class should be able to
   * determine if it should attach.
   *
   * @param $entity_type
   *   The entity type
   * @param $bundle_name
   *   The name of the bundle
   *
   * @return
   *   FALSE if the child class should return FALSE, NULL if the child class
   *   should decide.  The child class should always return FAlSE or TRUE. If
   *   TRUE is returned then an instance of the field can be attached to the
   *   bundle.
   */
//   protected function canAttach($entity_type, $bundle_name) {
//     // Don't attach if it's already attached.
//     if (array_key_exists('bundles', $this->field) and
//         array_key_exists('TripalEntity', $this->field['bundles']) and
//         in_array($bundle_name, $this->field['bundles']['TripalEntity'])) {
//       return FALSE;
//     }

//     // Child classes should check to see if this field can be attached
//     // to the bundle.
//     return NULL;
//   }




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
  public static function formatterSettingsSummary($field, $instance, $view_mode) {

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
  public static function formatterSettingsForm($field, $instance,
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
  public static function formatterView(&$element, $entity_type, $entity,
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
  public static function widgetForm(&$widget, &$form, &$form_state, $field, $instance,
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
  public static function widgetFormValidate($entity_type, $entity, $field, $instance, $langcode,
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
  public static function widgetFormSubmit($entity_type, $entity, $field, $instance, $langcode,
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
  public static function load($field, $entity, $details = array()) {

  }


  public function instanceLoad($instance) {

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
  public static function instanceSettingsForm($field, $instance) {
    $settings = $instance['settings'];
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
    $element['#field'] = $field;
    $element['#instance'] = $instance;
    $element['#element_validate'][] = 'tripal_field_instance_settings_form_validate';

    return $element;
  }
  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public static function instanceSettingsFormValidate($field, $instance, $form, &$form_state) {

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
  public static function globalSettingsForm($field, $instance, $has_data) {
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
    $element['#field'] = $field;
    $element['#instance'] = $instance;
    $element['#element_validate'][] = 'tripal_field_settings_form_validate';

    return $element;
  }
  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public static function globalSettingsFormValidate($field, $instance, $form, &$form_state) {

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
  public static function viewsDataAlter(&$data, $field, $entity_info) {

  }

}