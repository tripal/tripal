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
    $this->path = array();
    $this->params = array();
    $this->base_path = $base_path;
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
   *   @code
   *     array(
   *       'Gene',
   *       'sequence',
   *     );
   *   @endcode
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
   * @param $sanatize
   *   Set to TRUE to convert the period to underscore.
   *
   * @return
   *   The version number for this web service.
   */
  public function getVersion($sanatize = FALSE) {

    $class = get_class($this);
    $major_version = '';
    $minor_version = '';

    if (preg_match('/v(\d+)_(\d+)$/', $class, $matches)) {
      $major_version = $matches[1];
      $minor_version = $matches[2];
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

    $context = $this->resource ? $this->resource->getContext() : array();
    $type = $this->resource ? $this->resource->getType() : 'unknown';
    $json_ld = array(
      '@context' => $context,
      '@id' => '',
      '@type' => $type,
    );
    $data = $this->getData();
    return array_merge($json_ld, $data);
  }

  /**
   * Retreives the service URL for this service.
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
    return array();
  }

  /**
   * Sets the resource to be returned by this web service.
   *
   * @param $resource.
   *   An implementation of a TripalWebServiceResource.
   */
  public function setResource($resource) {
    // Make sure the $servcie provides is a TripalWebServcie class.
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
  public function getSupportedClasses() {
     $supported_classes = array();
     return $supported_classes;
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
   * @return $key
   *   The key (the term name but with spaces replaced with underscores).
   */
  public function addContextTerm($resource, $term, $sanitize = array()) {
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
    $key = $term['name'];
    $key_adj = $key;
    if (in_array('spacing', $sanitize)) {
      $key_adj = preg_replace('/ /', '_', $key_adj);
    }
    if (in_array('lowercase', $sanitize)) {
      $key_adj = strtolower($key_adj);
    }

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

    if ($key_adj == 'clause subject') {
      //print_r(debug_backtrace()[2]['function']);
    }
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
  public function addContextVocab($resource, $vocab) {

    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception('addContextVocab: Please provide a $resource of type TripalWebServiceResource.');
    }

    $key = $vocab['short_name'];
    // The URL here should be the URL prefix not the database home
    // page.  But unfortunately, the URL prefix can't be guaranteed to be
    // a true prefix. Not all databases place by the rules.  For this reason,
    // we can never use JSON-LD compact IRIs. :-(
    $iri = $vocab['url'];
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
   * @return $key
   *   The key (the term name but with spaces replaced with underscores).
   */
  public function addResourceProperty($resource, $term, $value, $sanitize = array()) {
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
  public function setResourceType($resource, $term, $sanitize = array('spacing')) {

    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception('addProperty: Please provide a $resource of type TripalWebServiceResource.');
    }
    // First add the term
    $key = $this->addContextTerm($resource, $term, $sanitize);

    // Then set the type
    $resource->setType($key);
  }
}