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
    $this->resource->addContextItem('vocab', $this->getServicePath() . '#');

    $this->resource->addContextItem('ApiDocumentation', 'hydra:ApiDocumentation');
    $this->resource->addContextItem('supportedClass', 'hydra:supportedClass');
    $this->resource->setType('ApiDocumentation');
    $this->resource->setID('vocab/' . $this->getVersion());

    // We need to list the content types as properties of the EntryPoint.
    $properties = array();


    // Iterate through all of the web services and get their documentation
    $services = tripal_get_web_services();
    foreach ($services as $service_class) {
      tripal_load_include_web_service_class($service_class);
      $service = new $service_class($this->base_path);
      $supported_classes = $service->getSupportedClasses();
      foreach ($supported_classes as $supported) {
        $this->resource->addProperty('supportedClass', $supported);
      }
      if ($service_class::$type != 'vocab') {
        $event_prop = new TripalWebServiceResource($this->getServicePath());
        $event_prop->addProperty('hydra:title', $service_class::$type);
        $event_prop->addProperty('hydra:description', $service_class::$description);
        $event_prop->addContextItem('required', 'hydra:reqiured');
        $event_prop->addProperty('required', NULL);
        $event_prop->addContextItem('readonly', 'hydra:readonly');
        $event_prop->addProperty('readonly', TRUE);
        $event_prop->addContextItem('writeonly', 'hydra:writeonly');
        $event_prop->addProperty('writeonly', FALSE);
        $event_prop->addContextItem('property', array(
          "@id" => "hydra:property",
          "@type" => "@id"
        ));
        //$event_prop->setID('');
        //$event_prop->setType('');
        $prop = new TripalWebServiceResource($this->getServicePath());
        $prop->setID('vocab:EntryPoint/' . $service_class::$type);
        $prop->setType('hydra:Link');
        $prop->addContextItem('label', 'rdfs:label');
        $prop->addProperty('label', $service_class::$label);
        $prop->addContextItem('description', 'rdfs:comment');
        $prop->addProperty('description', $service_class::$description);
        $prop->addContextItem('domain', array(
          "@id" => "rdfs:domain",
          "@type" => "@id"
        ));
        $prop->addProperty('domain', 'vocab:EntryPoint');
        $prop->addContextItem('range', array(
          "@id" => "rdfs:range",
          "@type" => "@id"
        ));
        $prop->addProperty('range', 'hydra:Collection');

        $prop_ops = array();
        $prop_op = new TripalWebServiceResource($this->getServicePath());
        $prop_op->setID('_:event_collectionretrieve');
        $prop_op->setType('hydra:Operation');
        $prop_op->addContextItem('method', 'hydra:method');
        $prop_op->addProperty('method', 'GET');
        $prop_op->addContextItem('label', 'rdfs:label');
        $prop_op->addProperty('label', 'Retrieves all ' . $service_class::$label . ' entities.');
        $prop_op->addContextItem('description', 'rdfs:comment');
        $prop_op->addProperty('description', NULL);
        $prop_op->addContextItem('expects', array(
          "@id" => "hydra:expects",
          "@type" => "@id"
        ));
        $prop_op->addProperty('expects', NULL);
        $prop_op->addContextItem('returns', array(
          "@id" => "hydra:returns",
          "@type" => "@id"
        ));
        $prop_op->addProperty('returns', 'hydra:Collection');
        $prop_op->addContextItem('statusCodes', 'hydra:statusCodes');
        $prop_op->addProperty('statusCodes', array());
        $prop_ops[] = $prop_op;

        $prop->addContextItem('supportedOperation', 'hydra:supportedOperation');
        $prop->addProperty('supportedOperation', $prop_ops);
        $event_prop->addProperty('property', $prop);
        $properties[] = $event_prop;
      }
    }

    // Add in the generic supported classes.
    $entry_class = new TripalWebServiceResource($this->base_path);
    $entry_class->addContextItem('label', 'rdfs:label');
    $entry_class->setID($this->getServicePath() . '#EntryPoint');
    $entry_class->setType('hydra:Class');
    $entry_class->addProperty('label', 'EntryPoint');
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
    $entry_class->addProperty('supportedProperty', $properties);
    $this->resource->addProperty('supportedClass', $entry_class);
  }
}