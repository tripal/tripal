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
   *   An array containing the elements of the URL path. Each level of the
   *   URL appears in a separate element of the array. The service type and
   *   version are automatically removed from the array.
   */
  protected $path;

  /**
   * The set of paramters provided to the sesrvice. These are the values
   * that would occur in a URL after the question mark in an HTTP GET or
   * the data items of an HTTP POST.
   */
  protected $params;

  // --------------------------------------------------------------------------
  //                             CONSTRUCTORS
  // --------------------------------------------------------------------------
  /**
   * Implements the constructor.
   */
  public function __construct() {
    $this->resource = new TripalWebServiceResource();
    $this->path = array();
    $this->params = array();
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

    // Get the data array and set the IRIs fore each ID.
    $data = $this->getData();
    $this->setIDs($data);

    return array_merge($json_ld, $data);
  }

  /**
   * A recursive function for setting the IRI (i.e. JSON-LD @id property).
   *
   * @param $data
   *   An array of data as returned by the $this->getData() function
   */
  protected function setIDs(&$data) {
    global $base_url;

    if(array_key_exists('@id', $data)) {
      $class = get_class($this);
      $version = $this->getVersion();
      $type = $class::$type;
      $data['@id'] = $base_url . '/web-services/' . $type . '/' . $version . '/' . $data['@id'];
    }
    foreach ($data as $key => $val) {
      if (is_array($val)) {
        $this->setIDs($data[$key]);
      }
    }
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
    $this->resource = new TripalWebServiceResource();
    $this->resource->addContextItem('error', 'rdfs:error');
    $this->resource->addProperty('error', $message);
  }
}