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
  }
}