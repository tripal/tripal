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
   * @see TripalWebService::handleRequest()
   */
  public function handleRequest() {

    $this->resource->addContextItem('vocab', $this->getServicePath());
    $this->resource->addContextItem('ApiDocumentation', 'hydra:ApiDocumentation');
    $this->resource->addContextItem('supportedClass', 'hydra:supportedClass');
    $this->resource->setType('ApiDocumentation');

    // Iterate through all of the web services and get their documentation
    $services = tripal_get_web_services();
    foreach ($services as $service_class) {
      tripal_load_include_web_service_class($service_class);
      $service = new $service_class($this->base_path);
      $supported_classes = $service->getSupportedClasses();
      foreach ($supported_classes as $supported) {
        $this->resource->addProperty('supportedClass', $supported);
      }
    }

    // Add in the generic supported classes.
    $entry_class = new TripalWebServiceResource($this->base_path);
    $entry_class->addContextItem('vocab', $this->getServicePath());
    $entry_class->addContextItem('label', 'rdfs:label');
    $entry_class->setID('vocab:EntryPoint');
    $entry_class->setType('hydra:Class');
    $entry_class->addProperty('label', 'Entry Point');
    $entry_class->addContextItem('description', 'rdfs:comment');
    $entry_class->addProperty('description', 'The main entry point or homepage of the API');
    $entry_class->addContextItem('subClassOf', array(
      "@id" => "rdfs:subClassOf",
      "@type" => "@id"
    ));
    $entry_class->addProperty('subClassOf', NULL);
    $entry_class->addContextItem('supportedOperation', 'hydra:supportedOperation');
    $entry_class->addContextItem('supportedProperty', 'hydra:supportedProperty');


    $operations = array();

    $get_op = new TripalWebServiceResource($this->getServicePath());
    $get_op->addContextItem('method', 'hydra:method');
    $get_op->addProperty('method', 'GET');
    $get_op->addContextItem('statusCodes', 'hydra:statusCodes');
    $get_op->addProperty('statusCodes', array());
    $get_op->addContextItem('label', 'rdfs:label');
    $get_op->addProperty('label', "The APIs main entry point.");
    $get_op->addContextItem('description', 'rdfs:comment');
    $get_op->addProperty('description', NULL);
    $get_op->addContextItem('expects', array(
      "@id" => "hydra:expects",
      "@type" => "@id"
    ));
    $get_op->addProperty('expects', NULL);
    $get_op->addContextItem('returns', array(
      "@id" => "hydra:returns",
      "@type" => "@id"
    ));
    $get_op->addProperty('returns', "vocab:EntryPoint");
    $get_op->setID('_:entry_point');
    $get_op->setType('hydra:Operation');

    $operations[] = $get_op;
    $entry_class->addProperty('supportedOperation', $operations);


    $this->resource->addProperty('supportedClass', $entry_class);
  }
}