<?php

class TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Tripal Field';

  // The default description for this field.
  public static $default_description = 'The generic base class for all Tripal fields. ' .
    'Replace this text as appropriate for the child implementation.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'tripal_no_storage',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'Thing',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'Thing',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Inidates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should ahve auto_attach set to FALSE so tha their values can be
    // attached asyncronously.
    'auto_attach' => TRUE,
  );

  // The default widget for this field.
  public static $default_widget = '';

  // The default formatter for this field.
  public static $default_formatter = '';

  // The module that manages this field.
  public static $module = 'tripal';

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = TRUE;

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
   *   An array containing the field data as returned by field_info_field().
   * @param $instance
   *   An array containing the instance data as returned by field_instance_info().
   */
  public function __construct($field, $instance) {
    $this->field = $field;
    $this->instance = $instance;

    $class = get_called_class();

    // Make sure the term exist.
    if (!$instance) {
      tripal_set_message(t('Missing instance of field "%field"', array('%field' => $field['field_name'])), TRIPAL_ERROR);
    }
    else {
      if (!array_key_exists('term_vocabulary', $instance['settings'])) {
        tripal_set_message(t('Missing controlled vocbulary for field "%field"', array('%field' => $field['field_name'])), TRIPAL_ERROR);
      }
      else {
        $vocabulary = $instance['settings']['term_vocabulary'];
        $accession = $instance['settings']['term_accession'];
        $term = tripal_get_term_details($vocabulary, $accession);
        if (!$term) {
          //throw new Error(t('Cannot create TripalField of type "%term" as that
          //    term does not exist.', array('%term' => $class::$term)));
        }
      }
    }
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
  public static function info() {
    $class = get_called_class();
    return array(
      'label' => $class::$default_label,
      'description' => $class::$default_description,
      'settings' => $class::$default_settings,
      'instance_settings' => $class::$default_instance_settings,
      'default_widget' => $class::$default_widget,
      'default_formatter' => $class::$default_formatter,
      'no_ui' => $class::$no_ui,
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
  //                            OVERRIDEABLE FUNCTIONS
  // --------------------------------------------------------------------------

  /**
   *  Perform validation of the field regardless how it is updated.
   *
   *  Any errors encountered should be indicated by adding a value to the
   *  $errors array according to the instructions below.
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
   * Provides a form for the 'Field Settings' of the field management page.
   *
   * This is an optional hook function and is similar to the
   * hook_field_settings_form function().
   *
   * @param $has_data
   *   TRUE if the field already has data, FALSE if not.
   */
  public function settingsForm($has_data) {
    $settings = $this->field['settings'];
    $element = array();

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
  public function settingsFormValidate($form, &$form_state) {

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

}
