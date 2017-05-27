<?php

class TripalWebServiceResource {
  /**
   * The unique identifier for this resource.
   */
  protected $id;
  /**
   * The unique type of resource.  The type must exists in the
   * context for the web service.
   */
  protected $type;

  /**
   * The JSON-LD compatible context for this resource.
   */
  protected $context;

  /**
   * Holds the data portion of the JSON-LD response for this resource.
   */
  protected $data;


  /**
   * The URL path that the service is providing to access this resource.
   * This path plus the $id are concatenated to form the IRI for this resource.
   */
  protected $service_path;


  /**
   * Implements the constructor.
   *
   * @param TripalWebService $service
   *   An instance of a TripalWebService or class that extends it.
   */
  public function __construct($service_path) {
    $this->context = array();
    $this->data = array();
    $this->service_path = $service_path;

    // First, add the RDFS and Hydra vocabularies to the context.  All Tripal
    // web services should use these.
    $vocab = tripal_get_vocabulary_details('rdfs');
    $this->addContextItem('rdfs', $vocab['url']);

    $vocab = tripal_get_vocabulary_details('hydra');
    $this->addContextItem('hydra', $vocab['url']);

    $vocab = tripal_get_vocabulary_details('dc');
    $this->addContextItem('dc', $vocab['url']);

    $vocab = tripal_get_vocabulary_details('schema');
    $this->addContextItem('schema', $vocab['url']);

    $this->data['@id'] = $service_path;
    $this->data['@type'] = '';
  }

  /**
   * Adds a term to the '@context' section for this resource.
   *
   * @param $name
   *   The term name.
   * @param $iri
   *   The Internationalized Resource Identifiers or it's shortcut.
   */
  public function addContextItem($name, $iri) {
    // TODO: make sure that if a shortcut is used that the parent is present.
    $this->context[$name] = $iri;
  }

  /**
   * Removes a term for the '@context' section for this resource.
   *
   * @param $name
   *   The term name.
   * @param $iri
   *   The Internationalized Resource Identifiers or it's shortcut.
   */
  public function removeContextItem($name, $iri) {
    // TODO: make sure that if a shortcut is used that the parent is present.
    unset($this->context[$name]);
  }

  /**
   * Sets the resource type.
   *
   * The type exist in the context of the web service.
   *
   * @param $type
   *   The type
   */
  public function setType($type) {
    $keys = array_keys($this->context);
    if (!in_array($type, $keys)) {
      throw new Exception("The resource type, '$type', has not yet been added to the " .
          "context of the web service. Use the addContextItem() function of the web service " .
          "to add this term.");
    }
    $this->type = $type;
    $this->data['@type'] = $type;
  }

  /**
   * Set's the unique identifier for the resource.
   *
   * Every resource must have a unique Idientifer. In JSON-LD the '@id' element
   * identifies the IRI for the resource which will include the unique
   * identifier.  The TraiplWebService to which a resource is added will
   * build the IRIs but it needs the unique ID of each resource.
   *
   * @param $id
   *   The unique identifier for the resource.
   */
  public function setID($id) {
    $this->id = $id;
    $this->data['@id'] = $this->service_path . '/' . $id;
  }


  /**
   * Retrieves the unique identifier for this resource.
   *
   * Every resource must have a unique Idientifer. In JSON-LD the '@id' element
   * identifies the IRI for the resource which will include the unique
   * identifier.  The TraiplWebService to which a resource is added will
   * build the IRIs but it needs the unique ID of each resource.
   *
   * @return
   *   The unique identifier for the resource.
   */
  public function getID() {
    return $this->id;
  }

  /**
   * Retreives the type of this resource.
   *
   * @return
   *   The name of the resource.
   */
  public function getType() {
    return $this->type;
  }

  /**
   * Adds a new key/value pair to the web serivces response.
   *
   * The value must either be a scalar or another TripalWebServiceResource
   * object.
   *
   * @param unknown $key
   *   The name of the $key to add. This key must already be present in the
   *   web service context by first adding it using the addContextItem()
   *   member function.
   * @param unknown $value
   *   The value of the key which must either be a scalar or a
   *   TripalWebServiceResource instance.
   */
  public function addProperty($key, $value) {

    // Make sure the key is already present in the context.
    $keys = array_keys($this->context);
    if (!in_array($key, $keys)) {
      throw new Exception("The key, '$key', has not yet been added to the " .
          "context. Use the addContextItem() function to add this key prior to adding a value for it.");
    }
    if (is_scalar($value)) {
      $this->data[$key] = $value;
    }
    else if (!is_subclass_of($value, 'TripalWebServiceResource')) {
      $this->data[$key] = $value;
    }
    else {
      throw new Exception("The value must either be a scalar or a TripalWebServiceResource");
    }
  }

  /**
   * A recursive function that ensures all keys in an item are in the context.
   *
   * @param $key
   *   The name of the current key.
   * @param $value
   *   The avlue assigned to the current key.
   *
   * @throws Exception
   *   Throws an exception of a key is not present in the context.
   */
  private function checkDataItem($key, $value) {
    // Make sure the key is already present in the context.
    $keys = array_keys($this->context);
    if (!in_array($key, $keys)) {
      throw new Exception("The key, '$key', has not yet been added to the " .
        "context. Use the addContextItem() function to add this key prior to adding a value for it.");
    }
    // If the value is an associative array then recurse
    if (is_array($value)) {
      // Check if this is an associatave array (non-integer keys).
      if (count(array_filter(array_keys($array), 'is_string')) > 0) {
        foreach ($value as $sub_key => $sub_value) {
          $this->checkDataItem($sub_key, $sub_value);
        }
      }
    }
  }

  /**
   * Retrieves the data section of the resource.
   *
   * The JSON-LD response constists of two sections the '@context' section
   * and the data section.  This function only returns the data section
   * for this resource
   *
   * @return
   *   An associative array containing the data section of the response.
   */
  public function getData() {
    return $this->data;
  }

  /**
   * Retrieves the data section of the resource.
   *
   * The JSON-LD response constists of two sections the '@context' section
   * and the data section.  This function only returns the data section
   * for this resource
   *
   * @return
   *   An associative array containing the data section of the response.
   */
  public function getContext() {
      return $this->context;
  }

  /**
   * Copies the context from a given TripalWebService resource to this
   * resource.
   *
   * @param $resource
   */
  public function setContext($resource) {
    if (!is_a($resource, 'TripalWebServiceResource')) {
      throw new Exception("The \$resource argument must be an instance of a TripalWebServiceResource.");
    }
    $this->context = $resource->getContext();
  }

}