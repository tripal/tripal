<?php

class TripalWebService {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------
  /**
   * The human-readable label for this web service.
   */
  public static $label = 'Base WebService';

  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'This is the base class for Tripal web services as is not meant to be used on it\'s own';

  /**
   * A machine-readable type for this service. This name must be unique
   * among all Tripal web services and is used to form the URL to access
   * this service.
   */
  public static $type = 'services';


  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  /**
   * The resource that will be returned by the webservice given the
   * arguments provided.  This is a private
   */
  protected $resource;

  /**
   * An array containing the elements of the URL path. Each level of the
   * URL appears in a separate element of the array. The service type and
   * version are automatically removed from the array.
   */
  protected $path;

  /**
   * The set of paramters provided to the sesrvice. These are the values
   * that would occur in a URL after the question mark in an HTTP GET or
   * the data items of an HTTP POST.
   */
  protected $params;

  /**
   * The URL at which Tripal web services are found.  This is used
   * for creating the IRI for resources.
   */
  protected $base_path;

  /**
   * The list of documented classes used by this service. For the web service
   * to be discoverable all of the entity classes and their collections
   * must be defined.
   */
  protected $documentation;

  // --------------------------------------------------------------------------
  //                             CONSTRUCTORS
  // --------------------------------------------------------------------------
  /**
   * Implements the constructor.
   */
  public function __construct($base_path) {
    if (!$base_path) {
      throw new Exception('Please provide a $base_path argument when creating a new TripalWebService.');
    }

    // Create a default resource so that the service always some something.
    $this->resource = new TripalWebServiceResource($base_path);

    // Intialize the private members variables.
    $this->path = [];
    $this->params = [];
    $this->base_path = $base_path;
    $this->documentation = [];

    $this->addDocClass([
      "id" => "http://www.w3.org/ns/hydra/core#Resource",
      "term" => 'hydra:Resource',
      "title" => "Resource",
      'subClassOf' => NULL,
    ]);

  }

  // --------------------------------------------------------------------------
  //                          OVERRIDEABLE FUNCTIONS
  // --------------------------------------------------------------------------


  /**
   * Responds to the request argument provided to the service.
   *
   * This function should be implemented by a TripalWebService child class.
   *
   */
  public function handleRequest() {
    // TODO: make sure the $this->path and $this->params are set before
    // continuing.
  }

  // --------------------------------------------------------------------------
  //                     CLASS FUNCTIONS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  /**
   * Sets the URL path for the resource being called.
   *
   * @param $path
   *   An array containing the elements of the URL path. Each level of the
   *   URL appears in a separate element of the array. The service type and
   *   version are automatically removed from the array. For example, a
   *   URL of the type http://localhost/web-services/content/v0.1/Gene/sequence
   *   will result in a $path array containing the following:
   *
   * @code
   *     array(
   *       'Gene',
   *       'sequence',
   *     );
   * @endcode
   *
   * @param unknown $path
   */
  public function setPath($path) {
    $this->path = $path;
  }

  /**
   * Sets the parameters for the resource.
   *
   * @param $params
   *   The set of paramters provided to the sesrvice. These are the values
   *   that would occur in a URL after the question mark in an HTTP GET or
   *   the data items of an HTTP POST.
   */
  public function setParams($params) {
    $this->params = $params;
  }

  /**
   * Retrieves the version number for this web service.
   *
   * Each web service must have version number built into the name of the
   * class. The version number appears at the end of the class name, begins
   * with a lower-case 'v' and is followed by two numbers (major and minor) that
   * are separated by an underscore.  This function identifies the version
   * from the class name and returns it here in a human-readable format.
   *
   * @param $sanitize
   *   Set to TRUE to convert the period to underscore.
   *
   * @return
   *   The version number for this web service.
   */
  public function getVersion($sanitize = FALSE) {

    $class = get_class($this);
    $major_version = '';
    $minor_version = '';

    if (preg_match('/v(\d+)_(\d+)$/', $class, $matches)) {
      $major_version = $matches[1];
      $minor_version = $matches[2];
      if ($sanitize) {
        return 'v' . $major_version . '_' . $minor_version;
      }
      return 'v' . $major_version . '.' . $minor_version;
    }
    return '';
  }


  /**
   * Retrieves the context section of the response.
   *
   * The JSON-LD response constists of two sections the '@context' section
   * and the data section.  This function only returns the context section
   * of the response.
   *
   * @return
   *   An associative array containing the context section of the response.
   */
  public function getContext() {
    return $this->resource->getContext();
  }


  /**
   * Returns the full web service response.
   *
   * The response includes both the @context and data sections of the
   * JSON-LD response.
   *
   * @return
   *   An associative array containing that can be converted to JSON.
   */
  public function getResponse() {
    $class = get_called_class();

    $context_filename = $class::$type . '.' . $this->getVersion(TRUE);
    foreach ($this->path as $element) {
      $context_filename .= '.' . $element;
    }
    $context_filename .= '.json';
    $context_filename = strtolower($context_filename);
    $context_dir = 'public://tripal/ws/context';
    $context_file_path = $context_dir . '/' . $context_filename;

    // Make sure the user directory exists
    if (!file_prepare_directory($context_dir, FILE_CREATE_DIRECTORY)) {
      throw new Exception('Could not access the tripal/ws/context directory on the server for storing web services context files.');
    }

    $context = $this->resource ? $this->resource->getContext() : [];
    $context = [
      '@context' => $context,
    ];
    $cfh = fopen($context_file_path, "w");
    if (flock($cfh, LOCK_EX)) {
      fwrite($cfh, json_encode($context));
      flock($cfh, LOCK_UN);
      fclose($cfh);
    }
    else {
      throw new Exception(t('Error locking file: @file.', ['@file' => $context_file_path]));
    }

    $fid = db_select('file_managed', 'fm')
      ->fields('fm', ['fid'])
      ->condition('uri', $context_file_path)
      ->execute()
      ->fetchField();

    // Save the context file so Drupal can manage it and remove the file later.
    if (!$fid) {
      $context_file = new stdClass();
      $context_file->uri = $context_file_path;
      $context_file->filename = $context_filename;
      $context_file->filemime = 'application/ld+json';
      $context_file->uid = 0;
      file_save($context_file);
    }

    $type = $this->resource ? $this->resource->getType() : 'unknown';
    $json_ld = [
      '@context' => file_create_url($context_file_path),
      '@id' => '',
      '@type' => $type,
    ];
    $data = $this->getData();
    return array_merge($json_ld, $data);
  }

  /**
   * Retrieves the service URL for this service.
   */
  public function getServicePath() {
    $class = get_class($this);
    $version = $this->getVersion();
    $type = $class::$type;
    return $this->base_path . '/' . $type . '/' . $version;
  }

  /**
   * Retrieves the data section of the response.
   *
   * The JSON-LD response constists of two sections the '@context' section
   * and the data section.  This function only returns the data section
   * of the response.
   *
   * @return
   *   An associative array containing the data section of the response.
   */
  public function getData() {

    if ($this->resource) {
      return $this->resource->getData();
    }
    return [];
  }

  /**
   * Sets the resource to be returned by this web service.
   *
   * @param $resource .
   *   An implementation of a TripalWebServiceResource.
   */
  public function setResource($resource) {
    // Make sure the $service provides is a TripalWebService class.
    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception("Cannot add a new resource to this web service as it is not a TripalWebServiceResource.");
    }

    $this->resource = $resource;
  }


  /**
   * Set an error for the service.
   *
   * @param $message
   *   The error message to report.
   */
  public function setError($message) {
    $this->resource = new TripalWebServiceResource($this->base_path);
    $this->resource->setID('error');
    $this->resource->addContextItem('error', 'rdfs:error');
    $this->resource->setType('error');
    $this->resource->addProperty('error', $message);
  }

  /**
   * Retrieves an array contining the supported classes.
   *
   * Supported classe are resources provided by this web services and the
   * operations supported by those classes.
   *
   * @return
   *   An array of TripalWebServiceResource objects that follow the Hydra
   *   documentation for documenting supported classes.
   */
  public function getDocumentation() {
    return $this->documentation;
  }

  /**
   *  Defines an entity class for the web services.
   *
   *  A class defines a resource including information about its properties
   *  and the actions that can be performed.  Each class as a unique @id,
   *  title, type and description.  The $details parameter is used to provide
   *  this information.  Additionally, a resource may support Create, Read
   *  Update and Delete (CRUD) operations.  Those are defined using the
   *  $ops argument. Finally, resources may have properties (or fields). These
   *  properties should be defined using the $props arugment.
   *
   * @param $details .
   *    An array of key/value pairs providing the details for the class. Valid
   *    keys include:
   *      - id: The unique IRI for this class.
   *      - term: the accession for the term for this class.
   *      - title:  The title for the resource that this Class represents.
   *      - description: (Optional). A description of the resource.
   *      - subClassOf: (Optional). If this class is a subclass of another
   *        class then the value should be the @id of the parent class. This
   *        defaults to hydra:Resource. If no subClass is desired, set it
   *        to NULL.
   *      - type: (Optional). Indicates the type of class. Defaults to
   *        hydra:Class
   * @param $ops
   *    If the resource supports CRUD functionality then those functions should
   *    be defined using this argument.  This is an associative array with
   *    the following keys: GET, POST, PUT, DELETE. These keys, if provided,
   *    indicate that a resource of this type can be retrieved (GET),
   *    created (POST), updated (PUT) or deleted (DELETE).  The value for each
   *    key should be an associative array that supports the following keys:
   *      = type: the @id that determines the type of operation.  This type
   *        should be specific to the resource, and it need not be a term
   *        from a real vocabulary.  Use the '_:' prefix for the term. For
   *        example, for an 'Event' resource with a GET method, an appropriate
   *        type would be '_:event_retrieve'.
   *      - label: A label that describes the operation in the
   *        context of the resource.
   *      - description: A description for the operation.  Can be set to NULL
   *        for no description.
   *      - expects: The information expected by the Web API.
   *      - returns: The @id of a Class that will be returned by
   *        the operation. Set to NULL if there is no return value.
   *      - statusCodes: An array of status codes that could be returned when
   *        an error occurs.  Each element of the array is an associative
   *        array with two key/value pairs: 'code' and 'description'.  Set the
   *        'code' to be the HTTP code that can be returned on error and the
   *        'description' to an error message appropriate for the error. For
   *        example an HTTP 404 error means "Not Found".  You can sepecify this
   *        code and provide a description appropriate for the method and
   *        class.
   * @param $props .
   *    Defines the list of properties that the resource provides.  This is an
   *    array of properties where each property is an associative array
   *    containing the following keys:
   *      - type:  The @id of the type of property. Alternatively, this can
   *        be an instance of a TripalWebServiceResource when the property
   *        must support operations such as a property of type hydra:Link.
   *      - title:  (Optional). The human-readable title of this property. If
   *        this key is absent then the title will be set to the term's title.
   *      - description:  (Optional). A short description of this property. If
   *        this key is absent then the description will be set to the term's
   *        description.
   *      - required:  Set to TRUE to indicate if this property is a required
   *        field when creating a new resource.
   *      - readable:  Set to TRUE to indicate that the client can retrieve the
   *        property's value?
   *        altered by an update.
   *      - writeable: Set to TRUE if the client can alter the value of the
   *        property.
   *      - domain: the @id of the rdfs:domain.
   *      - range:  the @id of the rdfs:range.
   */
  protected function addDocClass($details = [], $ops = [], $props = []) {
    $supported_class = new TripalWebServiceResource($this->getServicePath());

    // Add the context which all classes will need
    $supported_class->addContextItem('description', 'rdfs:comment');
    $supported_class->addContextItem('subClassOf', 'hydra:subClassOf');
    $supported_class->addContextItem('description', 'rdfs:comment');
    $supported_class->addContextItem('label', 'rdfs:label');

    // Set the Class ID.
    $class_id = $details['id'];
    $accession = $details['term'];
    if ($accession != $class_id) {
      $supported_class->addContextItem($accession, $class_id);
    }
    $supported_class->setID($accession);

    // Set the class Type.
    if (array_key_exists('type', $details)) {
      $supported_class->setType($details['type']);
    }
    else {
      $supported_class->setType('hydra:Class');
    }

    // Set title and description.
    $supported_class->addProperty('hydra:title', $details['title']);
    $supported_class->addProperty('hydra:description', array_key_exists('description', $details) ? $details['description'] : NULL);

    // Set the sub class.
    if (array_key_exists('subClassOf', $details)) {
      if ($details['subClassOf']) {
        $supported_class->addProperty('subClassOf', $details['subClassOf']);
      }
    }
    else {
      $supported_class->addProperty('subClassOf', 'hydra:Resource');
    }

    // Now add the supported operations.
    $class_ops = [];
    foreach ($ops as $op => $op_details) {
      $class_ops[] = $this->generateDocClassOp($supported_class, $op, $op_details);
    }
    $supported_class->addContextItem('supportedOperation', 'hydra:supportedOperation');
    $supported_class->addProperty('supportedOperation', $class_ops);

    // Now add in the supported proprerties.
    $class_props = [];
    foreach ($props as $prop) {
      $class_props[] = $this->generateDocClassProp($supported_class, $prop);
    }
    $supported_class->addContextItem('supportedProperty', 'hydra:supportedProperty');
    $supported_class->addProperty('supportedProperty', $class_props);

    $this->documentation[] = $supported_class;
  }

  /**
   * A helper function for the addClass() function for generating a property.
   */
  private function generateDocClassProp($supported_class, $prop) {
    $supported_class->addContextItem('property', [
      "@id" => "hydra:property",
      "@type" => "@id",
    ]);
    $supported_class->addContextItem('domain', [
      "@id" => "rdfs:domain",
      "@type" => "@id",
    ]);
    $supported_class->addContextItem('range', [
      "@id" => "rdfs:range",
      "@type" => "@id",
    ]);
    $supported_class->addContextItem('readable', 'hydra:readable');
    $supported_class->addContextItem('writeable', 'hydra:writeable');
    $supported_class->addContextItem('required', 'hydra:required');
    $supported_class->addContextItem('tripal_formatters', 'tripal:tripal_formatters');
    $class_prop = [
      'property' => $prop['type'],
      'hydra:title' => $prop['title'],
      'hydra:description' => array_key_exists('description', $prop) ? $prop['description'] : NULL,
      'required' => array_key_exists('required', $prop) ? $prop['required'] : NULL,
      'readable' => array_key_exists('readonly', $prop) ? $prop['readonly'] : NULL,
      'writeable' => array_key_exists('writeonly', $prop) ? $prop['writeonly'] : NULL,
    ];
    if (array_key_exists('domain', $prop)) {
      $class_prop['domain'] = $prop['domain'];
    }
    if (array_key_exists('range', $prop)) {
      $class_prop['range'] = $prop['range'];
    }
    if (array_key_exists('operations', $prop)) {
      $class_prop['supportedOperation'] = [];
      foreach ($prop['operations'] as $op => $op_details) {
        $class_prop['supportedOperation'][] = $this->generateOp($supported_class, $op, $op_details);
      }
    }
    if (array_key_exists('tripal_formatters', $prop)) {
      $class_prop['tripal_formatters'] = array_keys($prop['tripal_formatters']);
    }
    return $class_prop;
  }

  /**
   * A helper function for the addClass() function for generating an operation.
   */
  private function generateDocClassOp($supported_class, $op, $op_details) {

    if ($op != 'GET' and $op != 'PUT' and $op != 'DELETE' and $op != 'POST') {
      throw new Exception(t('The method, @method, provided to the TripalWebService::addClass function is not an oppropriate operations.', ['@method' => $op]));
    }

    if (!array_key_exists('type', $op_details)) {
      throw new Exception(t('Please provide a type in the operations array passed to the TripalWebService::addClass() function: @details', ['@details' => print_r($op_details, TRUE)]));
    }
    if (!array_key_exists('label', $op_details)) {
      throw new Exception(t('Please provide a label in the operations array passed to the TripalWebService::addClass() function: @details', ['@details' => print_r($op_details, TRUE)]));
    }

    $class_op = new TripalWebServiceResource($this->base_path);
    $class_op->setID($op_details['type']);
    switch ($op) {
      case 'GET':
        $class_op->setType('hydra:Operation');
        break;
      case 'POST':
        $class_op->setType('http://schema.org/AddAction');
        break;
      case 'DELETE':
        $class_op->setType('http://schema.org/DeleteAction');
        break;
      case 'PUT':
        $class_op->setType('http://schema.org/UpdateAction');
        break;
    }

    $class_op->addContextItem('method', 'hydra:method');
    $class_op->addContextItem('label', 'rdfs:label');
    $class_op->addContextItem('description', 'rdfs:comment');
    $class_op->addContextItem('expects', [
      "@id" => "hydra:expects",
      "@type" => "@id",
    ]);
    $class_op->addContextItem('returns', [
      "@id" => "hydra:returns",
      "@type" => "@id",
    ]);
    $class_op->addContextItem('statusCodes', 'hydra:statusCodes');
    $class_op->addContextItem('code', 'hydra:statusCode');

    $class_op->addProperty('method', $op);
    $class_op->addProperty('label', array_key_exists('label', $op_details) ? $op_details['label'] : 'Retrieves an instance of this resource');
    $class_op->addProperty('description', array_key_exists('description', $op_details) ? $op_details['description'] : NULL);
    $status_codes = array_key_exists('statusCodes', $op_details) ? $op_details['statusCodes'] : [];
    foreach ($status_codes as $code) {
      if (array_key_exists('code', $code) and array_key_exists('description', $code)) {
        $class_op->addProperty('statusCodes', $code);
      }
      else {
        throw new Exception(t('The status code provided to TripalWebService::addClass function is improperly formatted @code.', ['@code' => print_r($code, TRUE)]));
      }
    }
    if (count($status_codes) == 0) {
      $class_op->addProperty('statusCodes', []);
    }
    $class_op->addProperty('expects', array_key_exists('expects', $op_details) ? $op_details['expects'] : NULL);
    $class_op->addProperty('returns', array_key_exists('returns', $op_details) ? $op_details['returns'] : ' "http://www.w3.org/2002/07/owl#Nothing",');

    return $class_op;
  }

  /**
   * Converts a term array into an value appropriate for an @id or @type.
   *
   * @param $term
   *   The term array.
   * @param $santize
   *   An array of keywords indicating how to santize the key.  By default,
   *   no sanitizing occurs.  The two valid types are 'lowercase', and 'spacing'
   *   where 'lowercase' converts the term name to all lowercase and
   *   'spacing' replaces any spaces with underscores.
   *
   * @return
   *   The id (the term name but with spaces replaced with underscores).
   */
  protected function getContextTerm($term, $sanitize = []) {
    if (!$term) {
      $backtrace = debug_backtrace();
      throw new Exception('getContextTerm: Please provide a non NUll or non empty $term.');

    }
    if (!$term['name']) {
      throw new Exception('getContextTerm: The provided term does not have a name: ' . print_r($term, TRUE));
    }

    $key = $term['name'];
    $key_adj = $key;
    if (in_array('spacing', $sanitize)) {
      $key_adj = preg_replace('/ /', '_', $key_adj);
    }
    if (in_array('lowercase', $sanitize)) {
      $key_adj = strtolower($key_adj);
    }
    return $key_adj;
  }

  /**
   * Adds a term to the '@context' section for a given resource.
   *
   * @param $resource
   *   A TripalWebServiceResource instance.
   * @param $term
   *   The term array.
   * @param $santize
   *   An array of keywords indicating how to santize the key.  By default,
   *   no sanitizing occurs.  The two valid types are 'lowercase', and 'spacing'
   *   where 'lowercase' converts the term name to all lowercase and
   *   'spacing' replaces any spaces with underscores.
   *
   * @return
   *   The id (the term name but with spaces replaced with underscores).
   */
  protected function addContextTerm($resource, $term, $sanitize = []) {
    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception('addContextTerm: Please provide a $resource of type TripalWebServiceResource.');
    }

    if (!$term) {
      $backtrace = debug_backtrace();
      throw new Exception('addContextTerm: Please provide a non NUll or non empty $term.');

    }
    if (!$term['name']) {
      throw new Exception('addContextTerm: The provided term does not have a name: ' . print_r($term, TRUE));
    }

    // Make sure the vocab is present
    $vocab = $term['vocabulary'];
    $this->addContextVocab($resource, $vocab);

    // Sanitize the term key
    $key_adj = $this->getContextTerm($term, $sanitize);

    // Create the compact IRI
    $compact_iri = $vocab['short_name'] . ':' . $term['accession'];

    // If the full naming is indicated in the service parameters then
    // set the full name of the keys to include the vocab short name.
    if (array_key_exists('full_keys', $this->params)) {
      //$key_adj = $vocab['short_name'] . ':' . $key_adj;
    }

    $iri = $term['url'];

    $resource->addContextItem($key_adj, $compact_iri);
    $resource->addContextItem($compact_iri, $iri);

    return $key_adj;
  }

  /**
   * Adds a vocabulary to the '@context' section for a given resource.
   *
   * @param $resource
   *   A TripalWebServiceResource instance.
   * @param $vocab
   *   The vocabulary array.
   */
  protected function addContextVocab($resource, $vocab) {

    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception('addContextVocab: Please provide a $resource of type TripalWebServiceResource.');
    }

    $key = $vocab['short_name'];
    // The URL here should be the URL prefix not the database home
    // page.  But unfortunately, the URL prefix can't be guaranteed to be
    // a true prefix. Not all databases place by the rules.  For this reason,
    // we can never use JSON-LD compact IRIs. :-(
    $iri = $vocab['sw_url'];
    $resource->addContextItem($key, $iri);
  }

  /**
   * Adds a key/value property to the given resource.
   *
   * @param $resource
   *   A TripalWebServiceResource instance.
   * @param $term
   *   The term array for the key.
   * @param $value
   *   The value to add.
   * @param $santize
   *   An array of keywords indicating how to santize the key.  By default,
   *   no sanitizing occurs.  The two valid types are 'lowercase', and 'spacing'
   *   where 'lowercase' converts the term name to all lowercase and
   *   'spacing' replaces any spaces with underscores.
   *
   * @return $key
   *   The key (the term name but with spaces replaced with underscores).
   */
  public function addResourceProperty($resource, $term, $value, $sanitize = []) {
    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception('addProperty: Please provide a $resource of type TripalWebServiceResource.');
    }

    // First add the term
    $key = $this->addContextTerm($resource, $term, $sanitize);

    // Then add the property.
    $resource->addProperty($key, $value);
  }

  /**
   * Sets the type for the given resource.
   *
   * The type exist in the context of the web service.
   *
   * @param $resource
   *   A TripalWebServiceResource instance.
   * @param $type
   *   The type
   * @param $santize
   *   An array of keywords indicating how to santize the key.  By default,
   *   no sanitizing occurs.  The two valid types are 'lowercase', and 'spacing'
   *   where 'lowercase' converts the term name to all lowercase and
   *   'spacing' replaces any spaces with underscores.
   */
  public function setResourceType($resource, $term, $sanitize = ['spacing']) {

    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception('addProperty: Please provide a $resource of type TripalWebServiceResource.');
    }
    // First add the term
    $key = $this->addContextTerm($resource, $term, $sanitize);

    // Then set the type
    $resource->setType($key);
  }
}
