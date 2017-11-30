<?php

class TripalVocabService_v0_1 extends TripalWebService {

  /**
   * The human-readable label for this web service.
   */
  public static $label = 'Vocabulary';
  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'Provides access to vocabulary terms that are in use by this site.';
  /**
   * A machine-readable type for this service. This name must be unique
   * among all Tripal web services and is used to form the URL to access
   * this service.
   */
  public static $type = 'vocab';

  /**
   * The list of web services.
   */
  private $services;


  /**
   * Constructor for the TripalVocabService_v0_1 class.
   */
  public function __construct($base_path) {
    parent::__construct($base_path);

    // Get the list of services provided by this Tripal site.
    $this->services = tripal_get_web_services();
    foreach ($this->services as $sindex => $service_class) {
      // Load the class code.
      tripal_load_include_web_service_class($service_class);

      // Remove this class from the list of services.
      if ($service_class::$type == 'vocab') {
        unset($this->services[$sindex]);
      }
    }
  }

  /**
   * @see TripalWebService::handleRequest()
   */
  public function handleRequest() {
    $this->resource->addContextItem('vocab', $this->getServicePath() . '#');

    $this->resource->addContextItem('apiDocumentation', 'hydra:apiDocumentation');
    $this->resource->addContextItem('supportedClass', 'hydra:supportedClass');
    $this->resource->setType('apiDocumentation');
    $this->resource->setID('vocab/' . $this->getVersion());


    // Iterate through all of the web services and build their documentation
    foreach ($this->services as $service_class) {
      $service = new $service_class($this->base_path);
      $supported_classes = $service->getSupportedClasses();
      foreach ($supported_classes as $supported) {
        $this->resource->addProperty('supportedClass', $supported);
      }
    }

    // Add the EntryPoint class.
    $entry_point = $this->getEntryPointClass();
    $this->resource->addProperty('supportedClass', $entry_point);

    // Add in the Collection class.
    $collection = $this->getCollectionClass();
    $this->resource->addProperty('supportedClass', $collection);
  }

  /**
   * Generates the Collection class for the API documents.
   *
   * @return TripalWebServiceResource
   *
   */
  private function getCollectionClass() {
    // Add the Collection class.
    $collection = new TripalWebServiceResource($this->getServicePath());
    $collection->addContextItem('supportedOperation', 'hydra:supportedOperation');
    $collection->addContextItem('supportedProperty', 'hydra:supportedProperty');
    $collection->addContextItem('property', array(
      "@id" => "hydra:property",
      "@type" => "@id"
    ));
    $collection->setID('vocab:ContentCollection');
    $collection->setType('hydra:Class');
    $collection->addProperty('hydra:title', 'Content Collection');
    $collection->addProperty('hydra:description', '');

    $vocab = tripal_get_vocabulary_details('hydra');
    $url = preg_replace('/{accession}/', 'member', $vocab['urlprefix']);
    $collection->addProperty('supportedProperty', array(
      "property" => $url,
      "hydra:title" => "member",
      "hydra:description" => "The members of this collection.",
      "required" => null,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ));

    $url = preg_replace('/{accession}/', 'totalItems', $vocab['urlprefix']);
    $collection->addProperty('supportedProperty', array(
      "property" => $url,
      "hydra:title" => "totalItems",
      "hydra:description" => "The total number of items in the collection.",
      "required" => null,
      "readonly" => FALSE,
      "writeonly" => FALSE
    ));
    $url = preg_replace('/{accession}/', 'label', $vocab['urlprefix']);
    $collection->addProperty('supportedProperty', array(
      "property" => $url,
      "hydra:title" => "label",
      "hydra:description" => "The type of entities within the collection.",
      "required" => null,
      "readonly" => FALSE,
      "writeonly" => FALSE
    ));

    $collection_ops = array();
    $collection_op_get = new TripalWebServiceResource($this->base_path);
    $collection_op_get->setID('_:content_collection_retrieve');
    $collection_op_get->setType('hydra:Operation');
    $collection_op_get->addContextItem('method', 'hydra:method');
    $collection_op_get->addContextItem('label', 'rdfs:label');
    $collection_op_get->addContextItem('description', 'rdfs:comment');
    $collection_op_get->addContextItem('expects', array(
      "@id" => "hydra:expects",
      "@type" => "@id"
    ));
    $collection_op_get->addContextItem('returns', array(
      "@id" => "hydra:returns",
      "@type" => "@id"
    ));
    $collection_op_get->addContextItem('statusCodes', 'hydra:statusCodes');
    $collection_op_get->addProperty('statusCodes', array());
    $collection_op_get->addProperty('method', 'GET');
    $collection_op_get->addProperty('label', 'Retrieves members of the collection.');
    $collection_op_get->addProperty('description', null);
    $collection_op_get->addProperty('expects', null);
    $collection_op_get->addProperty('returns', 'vocab:ContentCollection');
    $collection_op_gets[] = $collection_op_get;

    $collection->addProperty('supportedOperation', $collection_op_gets);

    return $collection;
  }

  /**
   * Generates the EntryPoint class for the API documents.
   *
   * @return TripalWebServiceResource
   *
   */
  private function getEntryPointClass(){
    // We need to list the content types as properties of the EntryPoint.
    $entry_properties = array();

    // Add the EntryPoint Class.
    $entry_point = new TripalWebServiceResource($this->base_path);
    $entry_point->setID('vocab:EntryPoint');
    $entry_point->setType('hydra:Class');
    $entry_point->addContextItem('supportedOperation', 'hydra:supportedOperation');
    $entry_point->addContextItem('supportedProperty', 'hydra:supportedProperty');
    $entry_point->addContextItem('label', 'rdfs:label');
    $entry_point->addContextItem('description', 'rdfs:comment');
    $entry_point->addContextItem('subClassOf', array(
      "@id" => "rdfs:subClassOf",
      "@type" => "@id"
    ));
    $entry_point->addProperty('label', 'EntryPoint');
    $entry_point->addProperty('description', 'The main entry point or homepage of the API');
    $entry_point->addProperty('subClassOf', NULL);

    foreach ($this->services as $service_class) {
      $service = new $service_class($this->base_path);

      $service_prop_prop = new TripalWebServiceResource($this->getServicePath());
      $service_prop_prop->setID('vocab:EntryPoint/' . $service::$type);
      $service_prop_prop->setType('hydra:Link');
      $service_prop_prop->addContextItem('label', 'rdfs:label');
      $service_prop_prop->addContextItem('description', 'rdfs:comment');
      $service_prop_prop->addContextItem('domain', array(
        "@id" => "rdfs:domain",
        "@type" => "@id"
      ));
      $service_prop_prop->addContextItem('range', array(
        "@id" => "rdfs:range",
        "@type" => "@id"
      ));
      $service_prop_prop->addContextItem('supportedOperation', 'hydra:supportedOperation');
      $service_prop_prop->addContextItem('property', array(
        "@id" => "hydra:property",
        "@type" => "@id"
      ));
      $service_prop_prop->addProperty('label', $service_class::$label);
      $service_prop_prop->addProperty('description', $service_class::$description);
      $service_prop_prop->addProperty('domain', 'vocab:EntryPoint');
      $service_prop_prop->addProperty('range', 'hydra:Collection');

      // Add the GET operation to the property.
      // TODO: this should be part of the class.
      $service_prop_prop_ops = array();
      $service_prop_prop_op_get = new TripalWebServiceResource($this->getServicePath());
      $service_prop_prop_op_get->setID('_:collection_retrieve');
      $service_prop_prop_op_get->setType('hydra:Operation');
      $service_prop_prop_op_get->addContextItem('method', 'hydra:method');
      $service_prop_prop_op_get->addContextItem('label', 'rdfs:label');
      $service_prop_prop_op_get->addContextItem('description', 'rdfs:comment');
      $service_prop_prop_op_get->addContextItem('expects', array(
        "@id" => "hydra:expects",
        "@type" => "@id"
      ));
      $service_prop_prop_op_get->addContextItem('returns', array(
        "@id" => "hydra:returns",
        "@type" => "@id"
      ));
      $service_prop_prop_op_get->addContextItem('statusCodes', 'hydra:statusCodes');
      $service_prop_prop_op_get->addProperty('label', 'Retrieves all ' . $service_class::$label . ' entities.');
      $service_prop_prop_op_get->addProperty('description', NULL);
      $service_prop_prop_op_get->addProperty('method', 'GET');
      $service_prop_prop_op_get->addProperty('expects', null);
      $service_prop_prop_op_get->addProperty('returns', 'vocab:ContentCollection');
      $service_prop_prop_op_get->addProperty('statusCodes', array());
      $service_prop_prop_ops[] = $service_prop_prop_op_get;

      // Now add the supported operations to this property.
      $service_prop_prop->addProperty('supportedOperation', $service_prop_prop_ops);

      // Finally, create the property for the EntryPoint that corresponds
      // to this service type.
      $service_prop = array(
        'property' => $service_prop_prop,
        'hydra:title' => $service_class::$type,
        'hydra:description' => $service_class::$description,
        'required' => null,
        'readonly' => TRUE,
        'writeonly' => FALSE,
      );
      $entry_properties[] = $service_prop;
    }

    $entry_point->addProperty('supportedProperty', $entry_properties);


    $service_ops = array();
    $service_op_get = new TripalWebServiceResource($this->base_path);
    $service_op_get->setID('_:entry_point');
    $service_op_get->setType('hydra:Operation');
    $service_op_get->addContextItem('method', 'hydra:method');
    $service_op_get->addContextItem('label', 'rdfs:label');
    $service_op_get->addContextItem('description', 'rdfs:comment');
    $service_op_get->addContextItem('expects', array(
      "@id" => "hydra:expects",
      "@type" => "@id"
    ));
    $service_op_get->addContextItem('returns', array(
      "@id" => "hydra:returns",
      "@type" => "@id"
    ));
    $service_op_get->addContextItem('statusCodes', 'hydra:statusCodes');
    $service_op_get->addProperty('statusCodes', array());
    $service_op_get->addProperty('method', 'GET');
    $service_op_get->addProperty('label', 'The APIs main entry point.');
    $service_op_get->addProperty('description', null);
    $service_op_get->addProperty('expects', null);
    $service_op_get->addProperty('returns', 'vocab:EntryPoint');
    $service_ops[] = $service_op_get;

    $entry_point->addProperty('supportedOperation', $service_ops);

    return $entry_point;
  }
}