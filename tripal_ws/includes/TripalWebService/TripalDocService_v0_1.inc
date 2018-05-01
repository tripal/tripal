<?php

class TripalDocService_v0_1 extends TripalWebService {

  /**
   * The human-readable label for this web service.
   */
  public static $label = 'API Documentation';

  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'Provides Hydra style documenation to make this RESTful webservice discoverable.';

  /**
   * A machine-readable type for this service. This name must be unique
   * among all Tripal web services and is used to form the URL to access
   * this service.
   */
  public static $type = 'doc';

  /**
   * The list of web services.
   */
  private $services;


  /**
   * Constructor for the TripalDocService_v0_1 class.
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

    // Create the vocabulary resource.
    $this->resource->addContextItem('vocab', $this->getServicePath() . '#');
    $this->resource->addContextItem('apiDocumentation', 'hydra:apiDocumentation');
    $this->resource->addContextItem('supportedClass', 'hydra:supportedClass');
    $this->resource->setType('apiDocumentation');
    $this->resource->setID('doc/' . $this->getVersion());

    // Add the EntryPoint class.
    $this->addEntryPointClass();
    foreach ($this->documentation as $supported) {
      $this->resource->addProperty('supportedClass', $supported);
    }

    // Iterate through all of the web services and build their documentation
    foreach ($this->services as $service_class) {
      $service = new $service_class($this->base_path);
      $supported_classes = $service->getDocumentation();
      foreach ($supported_classes as $supported) {
        $this->resource->addProperty('supportedClass', $supported);
      }
    }
  }

  /**
   * Generates the EntryPoint class for the API documents.
   *
   * @return TripalWebServiceResource
   *
   */
  private function addEntryPointClass() {

    $service_path = $this->getServicePath();
    $details = [
      'id' => $service_path . '#EntryPoint',
      'term' => 'vocab:EntryPoint',
      'title' => 'EntryPoint',
      'description' => 'The main entry point or homepage of the API',
      'subClassOf' => NULL,
    ];

    // Add each service as a property.
    $properties = [];
    foreach ($this->services as $service_class) {
      $service = new $service_class($this->base_path);

      // Create a WebServiceResource for the hydra:Link type.
      $link = new TripalWebServiceResource($this->base_path);
      $link->setID('vocab:EntryPoint/' . $service::$type);
      $link->setType('hydra:Link');
      $link->addContextItem('domain', [
        "@id" => "rdfs:domain",
        "@type" => "@id",
      ]);
      $link->addContextItem('range', [
        "@id" => "rdfs:range",
        "@type" => "@id",
      ]);
      $link->addContextItem('readable', 'hydra:readable');
      $link->addContextItem('writeable', 'hydra:writeable');
      $link->addContextItem('required', 'hydra:required');
      $link->addContextItem('description', 'rdfs:comment');
      $link->addContextItem('label', 'rdfs:label');
      $link->addProperty('hydra:title', $service_class::$label);
      $link->addProperty('hydra:description', $service_class::$description);
      //       $link->addProperty('domain', $service_path . '#EntryPoint');
      //       $link->addProperty('range', $service_class::$label);

      $ops = [];
      $op = new TripalWebServiceResource($this->base_path);

      $op->setID('_:' . $service::$type . '_retrieve');
      $op->setType('hydra:Operation');
      $op->addContextItem('method', 'hydra:method');
      $op->addContextItem('label', 'rdfs:label');
      $op->addContextItem('description', 'rdfs:comment');
      $op->addContextItem('expects', [
        "@id" => "hydra:expects",
        "@type" => "@id",
      ]);
      $op->addContextItem('returns', [
        "@id" => "hydra:returns",
        "@type" => "@id",
      ]);
      $op->addContextItem('statusCodes', 'hydra:statusCodes');
      $op->addProperty('method', "GET");
      $op->addProperty('label', 'Retrieves the ' . $service_class::$label . ' resource.');
      $op->addProperty('description', NULL);
      $op->addProperty('expects', NULL);
      $op->addProperty('returns', 'local:EntryPoint/' . $service::$type);
      $op->addProperty('statusCodes', []);
      $ops[] = $op;
      $link->addContextItem('supportedOperation', 'hydra:supportedOperation');
      $link->addProperty('supportedOperation', $ops);

      $property = [
        'type' => $link,
        'title' => $service_class::$label,
        'description' => $service_class::$description,
        'domain',
        'vocab:EntryPoint',
        'range',
        $service->getServicePath(),
      ];
      $properties[] = $property;
    }

    $operations = [];
    $operations['GET'] = [
      'label' => "The APIs main entry point.",
      'description' => NULL,
      'expects' => NULL,
      'returns' => $service_path . '#EntryPoint',
      'type' => '_:entry_point_retrieve',
    ];


    $this->addDocClass($details, $operations, $properties);
  }
}