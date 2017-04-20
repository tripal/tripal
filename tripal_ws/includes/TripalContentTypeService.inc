<?php

class TripalContentTypeService extends TripalWebService {

  public static $label = 'Content Types';
  public static $description = 'Provides acesss to the biological and ' .
    'ancilliary data available on this site. Each content type represents ' .
    'biological data that is defined in a controlled vocabulary (e.g. ' .
    'Sequence Ontology term: gene (SO:0000704)).'
  public static $name = 'content';

  public function __construct() {
    parent::__construct();

    // Iterate through all of the entitie types (bundles) and add them as
    // supported classes.
    $bundles = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->orderBy('tb.label', 'ASC')
      ->execute();

    // Iterate through the terms and add an entry in the collection.
    $i = 0;
    while ($bundle = $bundles->fetchObject()) {
      $entity =  entity_load('TripalTerm', array('id' => $bundle->term_id));
      $term = reset($entity);
      $vocab = $term->vocab;

      // Get the bundle description. If no description is provided then
      // use the term definition
      $description = tripal_get_bundle_variable('description', $bundle->id);
      if (!$description) {
        $description = $term->definition;
      }
      // Add the bundle as a content type.
//       $response['member'][] = array(
//         '@id' => url($api_url . '/content/' . urlencode($bundle->label), array('absolute' => TRUE)),
//         '@type' => $term->name,
//         'label' => $bundle->label,
//         'description' => $description,
//       );

      $operations = array();
      $properties = array();
      $this->addSupportedClass($term->name, $bundle->label, $description, $operations, $properties);
      $this->addContextItem($term->name, $term->url);
    }
  }
}