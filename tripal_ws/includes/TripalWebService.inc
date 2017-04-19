<?php

class TripalWebService {

  public static $label;
  public static $description;
  public static $version;
  public static $name;

  private $context;
  private $response;
  private $supportedClasses;


  /**
   *
   */
  public function __construct() {
    $this->context = array();
    $this->response = array();
    $this->supportedClasses = array();
    $this->possibleStates = array();

    // First, add the RDFS and Hydra vocabularies to the context.  All
    // web services should use these.
    $this->addContextItem('rdfs', 'http://www.w3.org/1999/02/22-rdf-syntax-ns#');
    $this->addContextItem('hydra', 'http://www.w3.org/ns/hydra/core#');

  }


  public function addSupportedClass($type, $title, $description, $operations = array(), $properties = array()) {
    // TODO: add some checks.

    $this->supportedClasses[] = array(
      '@type' => $type,
      'hydra:title' => $title,
      'hydra:description' => $description,
      'supportedOperation' => $operations,
      'supportedProperty' => $properties,
    );
  }


  public function getSupportedClasses() {
    return $this->supportedClasses;
  }

  public function addContextItem($name, $details) {
    $this->context[$name] = $details;
  }

  public function getContext() {
    return $this->context;
  }

  public function getDocumentation() {
     return array(
       '@context' => $this->getContext(),
       '@id' => '',
       '@type' => 'ApiDocumentation',
       'supportedClass' => $this->getSupportedClasses(),
       'supportedStatus' => $this->getSuportedStatus(),
     );
  }
}