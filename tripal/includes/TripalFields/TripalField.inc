<?php

class TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Tripal Field';

  // The default description for this field.
  public static $default_description = 'The generic base class for all Tripal fields. Replace this text as appropriate for the child implementation.';

  // Provide a list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'tripal_no_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'Thing',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'Thing',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Set to TRUE if the field should be automatically attached to an entity
    // when it is loaded. Otherwise, the callee must attach the field
    // manually.  This is useful to prevent really large fields from slowing
    // down page loads.  However, if the content type display is set to
    // "Hide empty fields" then this has no effect as all fields must be
    // attached to determine which are empty.  It should always work with
    // web services.
    'auto_attach' => TRUE,
  ];

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = [
    'TripalTabDownloader',
    'TripalCSVDownloader',
  ];

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

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web serivces or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;


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

  // The term array that provides all the details about the controlled
  // vocabulary term that this field maps to.
  protected $term;

  // --------------------------------------------------------------------------
  //                     CONSTRUCTOR
  // --------------------------------------------------------------------------

  /**
   * Instantiates a new TripalField object.
   *
   * @param $field
   *   An array containing the field data as returned by field_info_field().
   * @param $instance
   *   An array containing the instance data as returned by
   *   field_instance_info().
   */
  public function __construct($field, $instance) {
    $term = NULL;
    $vocabulary = NULL;
    $accession = NULL;
    $this->field = $field;
    $this->instance = $instance;

    $class = get_called_class();

    // Use the term info defined in the class by default (assuming it's not schema:Thing ;-).
    if ($class::$default_instance_settings['term_name'] != 'Thing') {
      $vocabulary = $class::$default_instance_settings['term_vocabulary'];
      $accession = $class::$default_instance_settings['term_accession'];
    }

    // Allow the constructor to override the term info.
    $vocabulary = isset($this->instance['settings']['term_vocabulary']) ? $this->instance['settings']['term_vocabulary'] : $vocabulary;
    $accession = isset($this->instance['settings']['term_accession']) ? $this->instance['settings']['term_accession'] : $accession;

    // Assuming we have term info, load the term.
    if (!empty($vocabulary) AND !empty($accession)) {
      $this->term = tripal_get_term_details($vocabulary, $accession);
    }
    else {
      $bundle = tripal_load_bundle_entity(['name' => $instance['bundle']]);
      tripal_report_error('tripal_field', TRIPAL_ERROR,
        'Unable to instantiate the field named, ":name", due to missing vocabulary and/or accession. The term provided was: ":term". The bundle is: ":bundle".',
        [
          ':name' => $instance['field_name'],
          ':term' => $vocabulary . ':' . $accession,
          ':bundle' => $bundle->label,
        ]);
    }
    if (!$instance) {
      tripal_set_message(t('Missing instance of field "%field"', ['%field' => $field['field_name']]), TRIPAL_ERROR);
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
    $info = [
      'label' => $class::$default_label,
      'description' => $class::$default_description,
      'settings' => $class::$default_settings,
      'instance_settings' => $class::$default_instance_settings,
      'default_widget' => $class::$default_widget,
      'default_formatter' => $class::$default_formatter,
      'no_ui' => $class::$no_ui,
    ];
    return $info;
  }


  // --------------------------------------------------------------------------
  //                 DO NOT OVERRIDE THESE FUNCTIONS
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

  public function getFieldTerm() {
    return $this->term;
  }

  public function getFieldTermID() {
    $class = get_called_class();
    return $this->instance['settings']['term_vocabulary'] . ':' . $this->instance['settings']['term_accession'];
  }

  /**
   * Describes this field to Tripal web services.
   *
   * The child class need not implement this function. It has all of the details
   * provided for elements by the elementInfo() function are used to generate
   * the details needed for Views.
   *
   * @return
   *   An associative array with the keys available for searching. The value
   *   is the term array for the element.
   */
  public function webServicesData() {
    $elements = $this->elementInfo();

    $field_term = $this->getFieldTermID();
    $field_term_name = strtolower(preg_replace('/[^\w]/', '_', $this->term['name']));
    $field_details = $elements[$field_term];

    $searchable_keys = [];
    $sortable_keys = [];

    if (array_key_exists('searchable', $field_details) and $field_details['searchable']) {
      $searchable_keys[$field_term_name] = $field_term;
    }

    if (array_key_exists('sortable', $field_details) and $field_details['sortable']) {
      $sortable_keys[$field_term_name] = $field_term;
    }

    // Now add any entries for child elements.
    if (array_key_exists('elements', $field_details)) {
      $elements = $field_details['elements'];
      foreach ($elements as $element_name => $element_details) {
        $this->_addWebServiceElement($searchable_keys, $sortable_keys, $field_term_name, $field_term, $element_name, $element_details);
      }
    }

    return [
      'searchable' => $searchable_keys,
      'sortable' => $sortable_keys,
    ];
  }

  /**
   *
   * @param $searchabe_keys
   * @param $field_name
   * @param $element_name
   * @param $element_details
   */
  protected function _addWebServiceElement(&$searchable_keys, &$sortable_keys,
                                           $parent_term_name, $parent_term, $element_name, $element_details) {

    // Skip the 'entity' element, as we'll never make this searchable or
    // viewable. It's meant for linking.
    if ($element_name == 'entity') {
      return;
    }

    list($vocabulary, $accession) = explode(':', $element_name);
    $term = tripal_get_term_details($vocabulary, $accession);
    $field_term = $parent_term . ',' . $term['vocabulary']['short_name'] . ':' . $term['accession'];
    $field_term_name = $parent_term_name . '.' . strtolower(preg_replace('/[^\w]/', '_', $term['name']));

    // Is the field searchable?
    if (array_key_exists('searchable', $element_details) and $element_details['searchable']) {
      $searchable_keys[$field_term_name] = $field_term;
    }
    if (array_key_exists('sortable', $element_details) and $element_details['sortable']) {
      $sortable_keys[$field_term_name] = $field_term;
    }

    // Now add any entries for child elements.
    if (array_key_exists('elements', $element_details)) {
      $elements = $element_details['elements'];
      foreach ($elements as $element_name => $element_details) {
        $this->_addWebServiceElement($searchable_keys, $sortable_keys, $field_term_name, $field_term, $element_name, $element_details);
      }
    }
  }

  /**
   * Describes this field to Views.
   *
   * The child class need not implement this function has all of the details
   * provided for elements by the elementInfo() function are used to generate
   * the details needed for Views.
   *
   * @param $view_base_id
   *   Views was originally designed to integrate with SQL tables. And
   *   each field is associated with a table.  Because these are TripalFields
   *   and views is not directly querying the tables it doesn't make sense to
   *   associate fields with a table, but we must associate the fields with
   *   the bundle.  Each bundle is uniquely identified with the $view_base_id
   *   that is passed here.
   *
   * @return
   *   An associative array describing the data structure. Primary key is the
   *   name used internally by Views for the bundle that is provided by
   *   the $view_base_id. The returned array should be compatible with the
   *   instructions provided by the hook_views_data() function.
   */
  public function viewsData($view_base_id) {
    $data = [];
    $field_name = $this->field['field_name'];
    $field_term = $this->getFieldTermID();

    $elements = $this->elementInfo();
    $field_details = $elements[$field_term];

    // Get any titles or help text that is overriden.
    $title = ucfirst($this->instance['label']);
    if (array_key_exists('label', $field_details)) {
      $title = $field_details['label'];
    }
    $help = $this->instance['description'];
    if (array_key_exists('help', $field_details)) {
      $help = $field_details['help'];
    }

    // Build the entry for the field.
    $data[$view_base_id][$field_name] = [
      'title' => $title,
      'help' => $help,
      'field' => [
        'handler' => 'tripal_views_handler_field',
        'click sortable' => TRUE,
      ],
    ];
    // Is the field sortable?
    if (array_key_exists('sortable', $field_details) and $field_details['sortable']) {
      $data[$view_base_id][$field_name]['sort']['handler'] = 'tripal_views_handler_sort';
    }

    // Is the field searchable?
    if (array_key_exists('searchable', $field_details) and $field_details['searchable']) {
      $filter_handler = 'tripal_views_handler_filter_string';
      if (array_key_exists('type', $field_details) and $field_details['type'] == 'numeric') {
        $filter_handler = 'tripal_views_handler_filter_numeric';
      }
      $data[$view_base_id][$field_name]['filter'] = [
        'handler' => $filter_handler,
      ];
    }

    // Now add any entries for child elements.
    if (array_key_exists('elements', $field_details)) {
      $elements = $field_details['elements'];
      foreach ($elements as $element_name => $element_details) {
        $this->_addViewsDataElement($data, $view_base_id, $field_name, $element_name, $element_details);
      }
    }

    return $data;
  }

  /**
   *
   * @param unknown $data
   * @param unknown $view_base_id
   * @param unknown $parent
   * @param unknown $element_name
   * @param unknown $element_details
   */
  protected function _addViewsDataElement(&$data, $view_base_id, $parent, $element_name, $element_details) {
    // Skip the 'entity' element, as we'll never make this searchable or
    // viewable. It's meant for linking.
    if ($element_name == 'entity') {
      return;
    }

    if (!preg_match('/:/', $element_name)) {
      return;
    }

    $field_name = $parent . '.' . $element_name;
    list($vocabulary, $accession) = explode(':', $element_name);
    $term = tripal_get_term_details($vocabulary, $accession);

    // Get any titles or help text that is overriden.
    $title = ucfirst($term['name']);
    if (array_key_exists('label', $element_details)) {
      $title = $element_details['label'];
    }
    $help = $term['definition'];
    if (array_key_exists('help', $element_details)) {
      $help = $element_details['help'];
    }

    // Build the entry for the field.
    $data[$view_base_id][$field_name] = [
      'title' => $title,
      'help' => $help,
      'field' => [
        'handler' => 'tripal_views_handler_field_element',
      ],
    ];
    // Is the field sortable?
    if (array_key_exists('sortable', $element_details) and $element_details['sortable']) {
      $data[$view_base_id][$field_name]['sort']['handler'] = 'tripal_views_handler_sort';
      $data[$view_base_id][$field_name]['field']['click sortable'] = TRUE;
    }

    // Is the field searchable?
    if (array_key_exists('searchable', $element_details) and $element_details['searchable']) {
      $filter_handler = 'tripal_views_handler_filter_element_string';
      if (array_key_exists('type', $element_details) and $element_details['type'] == 'numeric') {
        $filter_handler = 'tripal_views_handler_filter_numeric';
      }
      $data[$view_base_id][$field_name]['filter'] = [
        'handler' => $filter_handler,
      ];
    }

    // Recusrively add any entries for child elements.
    if (array_key_exists('elements', $element_details)) {
      $elements = $element_details['elements'];
      foreach ($elements as $element_name => $element_details) {
        $this->_addViewsDataElement($data, $view_base_id, $field_name, $element_name, $element_details);
      }
    }
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
   * @param $entity_type
   *    The type of $entity.
   * @param $entity
   *    The entity for the operation.
   * @param $langcode
   *    The language associated with $items.
   * @param $items
   *    $entity->{$field['field_name']}[$langcode], or an empty array if unset.
   * @param $errors
   *    The array of errors (keyed by field name, language code, and delta) that
   *    have already been reported for the entity. The function should add its
   *    errors to this array. Each error is an associative array with the
   *    following keys and values:
   *      - error: An error code (should be a string prefixed with the
   *        module name).
   *      - message: The human readable message to be displayed.
   *
   */
  public function validate($entity_type, $entity, $langcode, $items, &$errors) {

  }


  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
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
  public function load($entity) {

  }

  /**
   * Provides the list of elements returned by the 'value' of the field.
   *
   * The elements provided by this function are used to integrate with
   * Drupal Views and Web services.  The return value is an associative array
   * that contains all of the elements that will be returned by the
   * 'value' of this field. If the value field returns an element which
   * is not defined here a warning will be generated.
   *
   * The array structure should contain at the top-level a key of the form
   * {db}:{accession}. This represents the term that this field belongs to.
   * The value of this top-level key is an array with the following keys:
   *   -name: this key is not actually used but is availble to improve
   *     readability of the array.  Because the key is a vocabulary term
   *     containing only the accession it's not always clear what it means.
   *     Providing a 'name' key helps other's know what the term is.
   *   -searchable:  TRUE if the element can be used for filtering the content
   *     type to which tis field is attached.  FALSE if not.
   *   -operations:  an array of filtering operations that can be used for this
   *     field.  These include: 'eq', 'ne', 'contains', 'starts', 'gt', 'lt',
   *     'gte', 'lte'.  These opertaions are applicable to strings: 'eq', 'ne',
   *     'contains', and 'starts'.  These operations are applicable for numeric
   *     values: 'gt', 'lt', 'gte', 'lte'.
   *   -label: The label (if applicable) to appear for the elmeent. The default
   *     is to use the term's name.
   *   -help: Help text (if applicable) to appear for the element. The default
   *     is to use the term's definition.
   *   -type: The data type: e.g. 'string' or 'numeric'. Default is 'string'.
   *   -sortable: TRUE if the element can be sorted.  FALSE if not.
   *   -elements:  If this field value is a simple scalar (i.e. string or
   *     number) then this key is not needed. But, if the 'value' of the
   *     field is an array with sub keys then those subkeys must be defined
   *     using this key.  The members of the element array follows the same
   *     format as the top-level key and the above subkeys can be used as well.
   *
   * The following code provides an example for describing the value elements
   * of this field.  The Tripal Chado module provides an obi__organism field
   * that attaches organism details to content types such as genes, mRNA,
   * stocks, etc.  It provides a label containing the full scientific name of
   * the organism as well as the genus, species, infraspecific name,
   * and infraspecific type. If the organism to which the field belong is
   * published then an entity ID is provided.  The following array describes
   * all of these.
   *
   * @code
   * $field_term = $this->getFieldTermID();
   * return array(
   * $field_term => array(
   * 'operations' => array('eq', 'contains', 'starts'),
   * 'sortable' => TRUE,
   * 'searchable' => TRUE,
   * 'elements' => array(
   * 'rdfs:label' => array(
   * 'searchable' => TRUE,
   * 'name' => 'scientific_name',
   * 'operations' => array('eq', 'ne', 'contains', 'starts'),
   * 'sortable' => TRUE,
   * ),
   * 'TAXRANK:0000005' => array(
   * 'searchable' => TRUE,
   * 'name' => 'genus',
   * 'operations' => array('eq', 'ne', 'contains', 'starts'),
   * 'sortable' => TRUE,
   * ),
   * 'TAXRANK:0000006' => array(
   * 'searchable' => TRUE,
   * 'name' => 'species',
   * 'operations' => array('eq', 'ne', 'contains', 'starts'),
   * 'sortable' => TRUE,
   * ),
   * 'TAXRANK:0000045' => array(
   * 'searchable' => TRUE,
   * 'name' => 'infraspecies',
   * 'operations' => array('eq', 'ne', 'contains', 'starts'),
   * 'sortable' => TRUE,
   * ),
   * 'local:infraspecific_type' => array(
   * 'searchable' => TRUE,
   * 'name' => 'infraspecific_type',
   * 'operations' => array('eq', 'ne', 'contains', 'starts'),
   * 'sortable' => TRUE,
   * ),
   * 'entity' => array(
   * 'searchable' => FALSE,
   * ),
   * ),
   * )
   * );
   * @endcode
   *
   * If a field does not have a complex nested set of values, but simply returns
   * a scalar then the default elementInfo provides default string-based
   * searchabilty.
   *
   * @return
   *   An associative array of the value elements provided by this field.
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
      ],
    ];
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
    $element = [];

    $element['#field'] = $this->field;
    $element['#instance'] = $this->instance;
    $element['#element_validate'][] = 'tripal_field_settings_form_validate';

    return $element;
  }

  /**
   *
   * @param $form
   * @param $form_state
   */
  public function settingsFormValidate($form, &$form_state) {

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
    $element = [];

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
   * After a field instance is created the following function is run.
   *
   * This function is equivalent to the hook_field_create_field() hook of
   * the Drupal Field API. This function is invoked after a new field
   * instance is created.
   */
  public function createInstance() {

  }

  /**
   * Used to filter records that match a given condition.
   *
   * Records that belong to a content type can be filtered using the fields.
   * This function should be implemented if the field  supports filtering as
   * specified in the elementInfo() function.  With this function, the query
   * object appropriate for the storage back-end is passed into the function.
   *
   * The condition array passesd in will have three values:
   *   - column:  the key indicating how the filter should occur.
   *   - op: the operation to perform (e.g. equals, contains, starts with etc.
   *   - value:  the value for filtering.
   *
   * The column used for filtering will be a comma-speperated list of
   * controlled vocabulary IDs. This comma-separate list corresponds directly
   * to the heirarchy of elements provided by the elementInfo() function.
   * For example, if a field provides organism information then it may use
   * the OBI:0100026 term for the field, and the term TAXRANK:0000005 for the
   * term to indicate the 'Genus'.  If these fields are properly organized in
   * the elementInfo() function then the "column" of the condition when
   * a user wants to search by genus will be: OBI:0100026,TAXRANK:0000005.
   *
   * @param $query
   *   A query object appropriate for the data storage backend. For example,
   *   The Tripal Chado module will provide a SelectQuery object.
   * @param $condition
   *   The field specific condition as set in the TripalFieldQuery object.
   */
  public function query($query, $condition) {

  }

  /**
   * Used to sort records that have been filtered.
   *
   * @param $query
   *   A query object appropriate for the data storage backend. For example,
   *   The Tripal Chado module will provide a SelectQuery object.
   * @param $order
   *   The field ordering as set in the TripalFieldQuery object.  This function
   *   should handle the ordering request as specified by this object.
   */
  public function queryOrder($query, $order) {

  }

  /**
   * Used to retrieve a distinct list of values already used for the current
   * field instance.
   *
   * @param $keyword
   *   A string option used to filter the distinct list. This is used when
   *   creating an autocomplete. For all distinct values, set this to NULL.
   * @param $options
   *   An array where options for how to generate this list can be specified.
   *   Supported options include:
   *     - limit: how many results to limit to (Default: 25)
   *     - label_string: a string with tokens that should be used to generate
   *   the human-readable values in the returned list.
   *
   * @return
   *   An array of values.
   */
  public function getValueList($options = [], $keyword = NULL) {

  }

}
