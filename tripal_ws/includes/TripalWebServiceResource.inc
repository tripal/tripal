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
   * All resources should have a @type and an @id.  But, there
   * are times when we don't want any, such as an array of unpblished
   * properties (e.g. featureloc elements).  These can be set to
   * TRUE to disable.
   */
  protected $disable_type = FALSE;
  protected $disable_id = FALSE;


  /**
   * Exclude the @type element form the data of this resource.
   */
  public function disbleType() {
    $this->disable_type = TRUE;
  }

  /**
   * Include the @type element form the data of this resource.
   */
  public function enableType() {
    $this->disable_type = FALSE;
  }

  /**
   * Exclude the @id element form the data of this resource.
   */
  public function disableID() {
    $this->disable_id = TRUE;
  }

  /**
   * Include the @id element form the data of this resource.
   */
  public function enableID() {
    $this->disable_id = FALSE;
  }

  /**
   * Implements the constructor.
   *
   * @param  $service_path
   *
   */
  public function __construct($service_path) {

    $this->context = [];
    $this->data = [];
    $this->service_path = $service_path;

    // First, add the RDFS and Hydra vocabularies to the context.  All Tripal
    // web services should use these.
    $vocab = tripal_get_vocabulary_details('rdfs');
    $this->addContextItem('rdfs', $vocab['sw_url']);

    $vocab = tripal_get_vocabulary_details('hydra');
    $this->addContextItem('hydra', $vocab['sw_url']);

    $vocab = tripal_get_vocabulary_details('dc');
    $this->addContextItem('dc', $vocab['sw_url']);

    $vocab = tripal_get_vocabulary_details('schema');
    $this->addContextItem('schema', $vocab['sw_url']);

    $vocab = tripal_get_vocabulary_details('local');
    $this->addContextItem('local', url($vocab['sw_url'], ['absolute' => TRUE]));


    $this->data['@id'] = $service_path;
    $this->data['@type'] = '';
  }

  /**
   * Adds a term to the '@context' section for this resource.
   *
   * This function should not be called directory.  Rather, the
   * addContextTerm() and addContextVocab() functions built
   * into the TripalWebService class should be used as  these  will help
   * ensure terms are added proper for the context of the web service.
   *
   * @param $name
   *   The term name.
   * @param $iri
   *   The Internationalized Resource Identifiers or it's shortcut.
   *
   */
  public function addContextItem($name, $iri) {
    if (array_key_exists($name, $this->context)) {
      return;
    }
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
    $this->checkKey($type);
    $this->type = $type;
    $this->data['@type'] = $type;
  }

  /**
   * Sets the service path for this resource.
   *
   * By default, the service path is provided to the constructor, but
   * this function will allow it to be changed if needed.
   *
   * @param $service_path
   *   The URL for the service path.
   */
  public function setServicePath($service_path) {
    $this->service_path = $service_path;
    $this->data['@id'] = $service_path;
  }

  /**
   * Checks a key to ensure it is in the Context before being used.
   *
   * This function should be used before adding a property or type to this
   * resource.  It ensures that the key for the property is already in the
   * context.
   *
   * @param $key
   *   The key to check.
   *
   * @throws Exception
   */
  private function checkKey($key) {
    // Make sure the key is already present in the context.
    $keys = array_keys($this->context);

    // Keys that are full HTML are acceptable
    if (preg_match('/^(http|https):\/\/.*/', $key)) {
      return;
    }

    // If the key has a colon separating the vocabulary and the term then we
    // just need to make sure that the vocabulary is present.
    $matches = [];
    if (preg_match('/^(.*?):(.*?)$/', $key, $matches)) {
      $vocab = $matches[1];
      $accession = $matches[2];

      // The underscore represents the blank node. So, these keys are okay.
      if ($vocab == '_') {
        return;
      }

      // If the vocabulary is not in the.
      if (!in_array($vocab, $keys)) {
        throw new Exception(t("The key, !key, has a vocabulary that has not yet been added to the " .
          "context. Use the addContextItem() function to add the vocabulary prior to adding a value for it.", ['!key' => $key]));
      }
    }
    else {
      // If the key is not in the context then throw an error.
      if (!in_array($key, $keys)) {
        throw new Exception(t("The key, !key, has not yet been added to the " .
          "context. Use the addContextItem() function to add this key prior to adding a value for it.", ['!key' => $key]));
      }
    }
  }

  /**
   * Checks the value to make sure there are no problems with.
   *
   * Will also expand any TriaplWebServiceResource by adding their
   * context to this resource and substitute their data in place of the
   * value.
   *
   * @param $value
   *
   */
  private function checkValue(&$value) {
    if (is_array($value)) {
      foreach ($value as $k => $v) {
        // If this is an integer then this is a numeric indexed array
        // and we can just check the value.  If not, then make sure the
        // key has been added to the context.
        if (preg_match('/^\d+$/', $k)) {
          $this->checkValue($value[$k]);
        }
        else {
          if ($k != '@id' and $k != '@type') {
            $this->checkKey($k);
          }
          $this->checkValue($value[$k]);
        }
      }
    }
    else {
      // If this is a TripalWebServiceResource then get it's data and use that
      // as the new value.  Also, add in the elements context to this resource.
      if (is_a($value, 'TripalWebServiceResource') or is_subclass_of($value, 'TripalWebServiceResource')) {
        $context = $value->getContext();
        foreach ($context as $k => $v) {
          $this->addContextItem($k, $v);
        }
        $value = $value->getData();
      }
    }
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
    $this->data['@id'] = $this->getURI($id);
  }

  /**
   * Retrieves the IRI for an entity of a given ID in this web service.
   *
   * @param $id
   *   The unique identifier for the resource.
   */
  public function getURI($id) {
    // If this is a key/value pair for an id and if the vocab portion is
    // an underscore then this represents a blank node and we don't want
    // to add the full path.
    $matches = [];
    if (preg_match('/^(.*?):(.*?)$/', $id, $matches)) {
      $vocab = $matches[1];
      if ($vocab == '_') {
        return $id;
      }
      return $id;
    }
    else {
      return $this->service_path . '/' . $id;
    }
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
   * Retrieves the type of this resource.
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
   * object.  If the same key is usesd multiple times then the resulting
   * resource will be presented as an array of elements.
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

    $this->checkKey($key);
    $this->checkValue($value);

    // If this is a numeric keyed array then add each item.
    if (is_array($value) and count(array_filter(array_keys($value), 'is_int')) == count(array_keys($value))) {
      if (!array_key_exists($key, $this->data)) {
        $this->data[$key] = [];
      }
      foreach ($value as $item) {
        $this->addProperty($key, $item);
      }
      return;
    }

    // If this key doesn't exist then add the value directly to the key.
    if (!array_key_exists($key, $this->data)) {
      $this->data[$key] = $value;
    }
    // Else if it does exist then we need to make sure that the element is
    // an array and add it.
    else {
      // If this is the second element, then convert it to an array.  The
      // second test in the condition below is for for a numeric array.
      if (!is_array($this->data[$key]) or count(array_filter(array_keys($this->data[$key]), 'is_string')) > 0) {
        $element = $this->data[$key];
        $this->data[$key] = [];
        $this->data[$key][] = $element;
      }
      $this->data[$key][] = $value;
    }
  }

  /**
   * Retrieves the value of a property.
   *
   * @param $key
   *   The name of the property.
   *
   * @param
   *   The value of the property. This could be a scalar, array or
   *   a TripalWebService object.
   */
  function getProperty($key) {
    return $this->data[$key];
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
    $data = $this->data;
    if ($this->disable_id) {
      unset($data['@id']);
    }
    if ($this->disable_type) {
      unset($data['@type']);
    }
    return $data;
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
      throw new Exception("The \$resource argument provided to the TripalWebServiceResource::setContext() function must be an instance of a TripalWebServiceResource.");
    }
    $this->context = $resource->getContext();
  }

}