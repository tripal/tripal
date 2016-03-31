<?php

/**
 *
 */
function tripal_ws_rest() {

  global $base_url;
  $ws_args = func_get_args();

  // The web services should never be cached.
  drupal_page_is_cacheable(FALSE);

  // Set some initial variables.
  $response = array();
  $status = 'success';
  $version = 'v0.1';
  $message = '';
  $api_url = $base_url . '/ws/' . $version;
  $page_limit = 25;
  $pager_id = 0;

  // Set some defaults for the response.
  $response['@context'] =  array();

  // Lump everything ito a try block so that if there is a problem we can
  // throw an error and have that returned in the response.
  try {

    // The services is the first argument
    $service = (count($ws_args) > 0) ? $ws_args[0] : '';

    switch ($service) {
      case 'doc':
        tripal_ws_handle_doc_service($api_url, $response);
        break;
      case 'content':
        tripal_ws_handle_content_service($api_url, $response, $ws_args);
        break;
      case 'vocab':
        tripal_ws_handle_vocab_service($api_url, $response, $ws_args);
        break;
      default:
        tripal_ws_handle_no_service($api_url, $response);
    }
  }
  catch (Exception $e) {
    watchdog('tripal_ws', $e->getMessage(), array(), WATCHDOG_ERROR);
    $message = $e->getMessage();
    $status = 'error';
  }

  // The responses follow a similar format as the AGAVE API with a
  // status, message, version and all data in the 'result' object.
/*   $response['status']  = $status;
  $response['message'] = $message;
  $response['api_version'] = $version;
  $response['source'] = array(
    'site_name' => variable_get('site_name', 'Unspecified'),
    'site_url' => $base_url,
    'site_slogan' => variable_get('site_slogan', 'Unspecified'),
    'site_email' =>  variable_get('site_mail', 'Unspecified'),
  ); */

  // Rather than use the drupal_json_output() funciton we manually specify
  // content type because we want it to be 'ld+json'.
  drupal_add_http_header('Content-Type', 'application/ld+json');
  print drupal_json_encode($response);
}


/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_handle_content_service($api_url, &$response, $ws_args) {

  // Get the content type.
  $ctype     = (count($ws_args) > 1) ? $ws_args[1] : '';
  $entity_id = (count($ws_args) > 2) ? $ws_args[2] : '';

  // If we have no content type then list all of the available content types.
  if (!$ctype) {
    tripal_ws_get_content_types($api_url, $response);
  }
  // If we don't have an entity ID then show a paged list of entities with
  // the given type.
  else if ($ctype and !$entity_id) {
    tripal_ws_get_content_type($api_url, $response, $ws_args, $ctype);
  }
  // If we have a content type and an entity ID then show the entity
  else {
    tripal_ws_get_content($api_url, $response, $ws_args, $ctype, $entity_id);
  }
}
/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_handle_vocab_service($api_url, &$response, $ws_args) {

  // Get the vocab name.
  $namespace = (count($ws_args) > 1) ? $ws_args[1] : '';
  $accession = (count($ws_args) > 2) ? $ws_args[2] : '';

  // If we have no $namespace type then list all of the available vocabs.
  if (!$namespace) {
    tripal_ws_get_vocabs($api_url, $response);
  }
  // If we don't have a $namespace then show a paged list of terms.
  else if ($namespace and !$accession) {
    tripal_ws_get_vocab($api_url, $response, $ws_args, $namespace);
  }
  // If we have a content type and an entity ID then show the entity
  else if ($namespace and $accession) {
    tripal_ws_get_term($api_url, $response, $ws_args, $namespace, $accession);
  }
}

/**
 *
 * @param $api_url
 * @param $response
 */
function tripal_ws_get_vocabs($api_url, &$response) {
  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for tihs resource.
  $response['@id'] = $api_url . '/vocab';

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = 'Content Types';
  $response['member'] = array();

  $vocabs = db_select('tripal_vocab', 'tv')
    ->fields('tv')
    ->execute();
  // Iterate through the vocabularies and add an entry in the collection.
  $i = 0;
  while ($vocab = $vocabs->fetchObject()) {
    $term =
    // Add the bundle as a content type.
    $response['member'][] = array(
      '@id' => $api_url . '/vocab/' . urlencode($vocab->namespace),
      '@type' => 'vocabulary',
      'namespace' => $vocab->namespace,
    );
    $i++;
  }
  $response['totalItems'] = $i;

  //$response['totalItems'] = $i;

  // Lastly, add in the terms used into the @context section.
  $response['@context']['Collection'] = 'hydra:Collection';
  $response['@context']['totalItems'] = 'hydra:totalItems';
  $response['@context']['member'] = 'hydra:member';
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['description'] = 'hydra:description';
}

/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_get_vocab($api_url, &$response, $ws_args, $namespace) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['schema'] = 'https://schema.org/';

  // Next add in the ID for tihs resource.
  $response['@id'] = $api_url . '/vocab/' . $namespace;

  // Get the vocabulary
  $vocab = tripal_load_vocab_entity(array('namespace' => $namespace));

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = $namespace . " vocabulary collection";
  $response['comment'] = 'The following list of terms may not be the full ' .
    'list for the vocabulary.  The terms listed here are only those ' .
    'that have associated content on this site.';

  // Get the list of terms for this vocab.
  $query = db_select('tripal_term', 'tt')
    ->fields('tt', array('id'))
    ->condition('vocab_id', $vocab->id)
    ->orderBy('accession', 'DESC');

  // Iterate through the entities and add them to the list.
  $terms = $query->execute();
  $i = 0;
  while($term = $terms->fetchObject()) {
    $term = tripal_load_term_entity(array('term_id' => $term->id));
    $response['member'][] = array(
      '@id' => $api_url . '/vocab/' . urlencode($namespace) . '/' .  urlencode($term->accession),
      '@type' => 'vocabulary_term',
      'namespace' => $vocab->namespace,
      'accession' => $term->accession,
      'name' => $term->name,
      'definition' => $term->definition,
    );
    $i++;
  }
  $response['totalItems'] = $i;

  // Lastly, add in the terms used into the @context section.
  $response['@context']['Collection'] = 'hydra:Collection';
  $response['@context']['totalItems'] = 'hydra:totalItems';
  $response['@context']['member'] = 'hydra:member';
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['comment'] = 'rdfs:comment';
  $response['@context']['itemPage'] = 'schema:itemPage';

}

/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_get_term($api_url, &$response, $ws_args, $namespace, $accession) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['schema'] = 'https://schema.org/';

  // Get the term.
  $term = tripal_load_term_entity(array('namespace' => $namespace, 'accession' => $accession));

  // Next add in the ID and Type for this resources.
  $response['@id'] = $api_url . '/vocab/' . urlencode($namespace) . '/' . urlencode($accession);
  $response['@type'] = 'vocabulary_term';
  $response['label'] = $term->name;
  $response['namespace'] = $namespace;
  $response['accession'] = $accession;
  $response['name'] = $term->name;
  $response['definition'] = $term->definition;

  if ($term->url) {
    $response['URL'] = $term->url;
  }

  // Lastly, add in the terms used into the @context section.
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['itemPage'] = 'schema:itemPage';
}
/**
 * Provides a collection (list) of all of the content types.
 *
 * @param $api_url
 * @param $response
 */
function tripal_ws_get_content_types($api_url, &$response) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for tihs resource.
  $response['@id'] = $api_url . '/content';

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = 'Content Types';
  $response['member'] = array();

  // Get the list of published terms (these are the bundle IDs)
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->orderBy('tb.label', 'ASC')
    ->execute();
  $terms = array();

  // Iterate through the terms and add an entry in the collection.
  $i = 0;
  while ($bundle = $bundles->fetchObject()) {
    $entity =  entity_load('TripalTerm', array('id' => $bundle->term_id));
    $term = reset($entity);
    $vocab = $term->vocab;

    if (!array_key_exists($vocab->namespace, $response['@context'])) {
      // If there is no URL prefix then use this API's vocabulary API
      if ($term->urlprefix) {
        $response['@context'][$vocab->namespace] = $term->urlprefix;
      }
      else {
        $response['@context'][$vocab->namespace] = $api_url . '/vocab/' . $vocab->namespace . '/';
      }
    }
    // Get the bundle description. If no description is provided then
    // use the term definition
    $description = tripal_get_bundle_variable('description', $bundle->id);
    if (!$description) {
      $description = $term->definition;
    }
    // Add the bundle as a content type.
    $response['member'][] = array(
      '@id' => $api_url . '/content/' . urlencode($bundle->label),
      '@type' => $vocab->namespace . ':' . $term->accession,
      'label' => $bundle->label,
      'description' => $description,
    );
    $i++;
  }
  $response['totalItems'] = $i;

  // Lastly, add in the terms used into the @context section.
  $response['@context']['Collection'] = 'hydra:Collection';
  $response['@context']['totalItems'] = 'hydra:totalItems';
  $response['@context']['member'] = 'hydra:member';
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['description'] = 'hydra:description';
}

/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_get_content_type($api_url, &$response, $ws_args, $ctype) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['schema'] = 'https://schema.org/';

  // Next add in the ID for tihs resource.
  $response['@id'] = $api_url . '/content/' . $ctype;

  // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
  $bundle = tripal_load_bundle_entity(array('label' => $ctype));
  $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
  $term = reset($term);
  $vocab = $term->vocab;

  if (!array_key_exists($vocab->namespace, $response['@context'])) {
    // If there is no URL prefix then use this API's vocabulary API
    if ($term->urlprefix) {
      $response['@context'][$vocab->namespace] = $term->urlprefix;
    }
    else {
      $response['@context'][$vocab->namespace] = $api_url . '/vocab/' . $vocab->namespace . '/';
    }
  }

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = $bundle->label . " collection";

  // Get the list of entities for this bundle.
  $query = new EntityFieldQuery;
  $query->entityCondition('entity_type', 'TripalEntity')
    ->entityCondition('bundle', $bundle->name)
    ->propertyOrderBy('title', 'DESC')
    ->pager(10);

  // Iterate through the entities and add them to the list.
  $results = $query->execute();
  $i = 0;
  if (isset($results['TripalEntity'])) {
    $entities = entity_load('TripalEntity', array_keys($results['TripalEntity']));
    foreach ($entities as $entity) {
      $response['member'][] = array(
        '@id' => $api_url . '/content/' . urlencode($ctype) . '/' .  $entity->id,
        '@type' => $vocab->namespace . ':' . $term->accession,
        'label' => $entity->title,
        'itemPage' => url('/bio_data/' . $entity->id, array('absolute' => TRUE)),
      );
      $i++;
    }
  }
  $response['totalItems'] = $i;

  // Lastly, add in the terms used into the @context section.
  $response['@context']['Collection'] = 'hydra:Collection';
  $response['@context']['totalItems'] = 'hydra:totalItems';
  $response['@context']['member'] = 'hydra:member';
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['itemPage'] = 'schema:itemPage';

}

/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_get_content($api_url, &$response, $ws_args, $ctype, $entity_id) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['schema'] = 'https://schema.org/';

  // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
  $bundle = tripal_load_bundle_entity(array('label' => $ctype));
  $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
  $term = reset($term);
  $vocab = $term->vocab;

  if (!array_key_exists($vocab->namespace, $response['@context'])) {
    // If there is no URL prefix then use this API's vocabulary API
    if ($term->urlprefix) {
      $response['@context'][$vocab->namespace] = $term->urlprefix;
    }
    else {
      $response['@context'][$vocab->namespace] = $api_url . '/vocab/' . $vocab->namespace . '/';
    }
  }

  // Get the TripalEntity and attach all the fields.
  $entity = entity_load('TripalEntity', array('id' => $entity_id));
  field_attach_load('TripalEntity', $entity);
  $entity = reset($entity);


  // Next add in the ID and Type for this resources.
  $response['@id'] = $api_url . '/content/' . $ctype . '/' . $entity_id;
  $response['@type'] = $vocab->namespace . ':' . $term->accession;
  $response['label'] = $entity->title;
  $response['itemPage'] = url('/bio_data/' . $bundle->id, array('absolute' => TRUE));

  // Get information about the fields attached to this bundle and sort them
  // in the order they were set for the display.
  // TODO: should we allow for custom ordering of fields for web services
  // or use the default display ordering?
  $fields = field_info_instances('TripalEntity', $bundle->name);
  uasort($fields, function($a, $b) {
    $a_weight = (is_array($a) && isset($a['display']['default']['weight'])) ? $a['display']['default']['weight'] : 0;
    $b_weight = (is_array($b) && isset($b['display']['default']['weight'])) ? $b['display']['default']['weight'] : 0;

    if ($a_weight == $b_weight) {
      return 0;
    }
    return ($a_weight < $b_weight) ? -1 : 1;
  });

  // Iterate throught the fields and add each value to the response.
  //$response['fields'] = $fields;
  foreach ($fields as $field_name => $field) {
    $field_info = field_info_field($field_name);
    $items = field_get_items('TripalEntity', $entity, $field_name);

    // By default, the label for the key in the output should be the
    // term from the vocabulary that the field is assigned. But in the
    // case that the field is not assigned a term, we must use the field name.
    $key = $field['field_name'];
    if (array_key_exists('semantic_web', $field['settings'])) {
      if (array_key_exists('type', $field['settings']['semantic_web']) and
          $field['settings']['semantic_web']['type']) {
        $key = $field['settings']['semantic_web']['type'];
      }
    }

    // Get the semantic web settings for this field.
    $field_type = '';
    if (array_key_exists('semantic_web', $field['settings'])) {
      $field_type = $field['settings']['semantic_web']['type'];
      if ($field_type) {
        $ns = $field['settings']['semantic_web']['ns'];
        $nsurl = $field['settings']['semantic_web']['nsurl'];
        $response['@context'][$ns] = $nsurl;
        $response['@context'][$key] = $ns . ':' .$field_type;
      }
    }

    // Skip hidden fields.
    if ($field['display']['default']['type'] == 'hidden') {
      continue;
    }

    // We want to allow the field to format itself for web services if it
    // wants to.  The file can do so if it implements a '_ws_formatter'
    // function.
    $function = $field_info['type'] . '_ws_formatter';
    module_load_include('inc', $field['display']['default']['module'], 'includes/fields/' . $field_info['type']);
    if (function_exists($function)) {
      $values = array();
      $function($values, $entity->type, $entity, $field_info, $field, $items);

      // Iterate through the key/values returned and handle hashed keys
      $add_vals = array();
      for ($i = 0; $i < count($values); $i++) {
        foreach ($values[$i] as $skey => $svalue) {
          // If the key is '#entity' then this should like to another
          // services.
          if ($skey == '#entity') {
            $sentity = $svalue;
            $sbundle = tripal_load_bundle_entity(array('name' => $sentity->bundle));
            $sterm = tripal_load_term_entity(array('term_id' => $sbundle->term_id));
            $vocab = $sterm->vocab;
            $add_vals['@id'] = $api_url . '/content/' . urlencode($sterm->name) . '/' . $sentity->id;
            $add_vals['@type'] = $vocab->namespace . ':' . $sterm->name;
            unset($values[$i][$skey]);
          }
        }
        $values[$i] = array_merge($add_vals, $values[$i]);
      }


      if (count($values) == 0) {
        $response[$key] = '';
      }
      if (count($values) == 1) {
        $response[$key] = $values[0];
      }
      else {
        $response[$key] = $values;
      }
    }
    // If a function doesn't exist then just show the default value is is.
    else {
      // Get the values based on cardinality
      if (count($items) == 1) {
        $values = $items[0]['value'];
      }
      // If cardinality is greater than 1 then the value should be an array
      else {
        $values = array();
        for ($i = 0; $i < count($items); $i++) {
          $values[] = $items[$i]['value'];
        }
      }
      $response[$key] = $values;
    }
  }

 //$response['fields'] = $fields;

  // Lastly, add in the terms used into the @context section.
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['itemPage'] = 'schema:itemPage';
}



/**
 * Provides the Hydra compatible apiDocumentation page that describes this API.
 *
 * @param $api_url
 * @param $response
 */
function tripal_ws_handle_doc_service($api_url, &$response) {
  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for tihs resource.
  $site_name = variable_get('site_name', '');
  $response['@id'] = $api_url . '/doc/';
  $response['title'] =  $site_name . ": RESTful Web Services API";
  $response['entrypoint'] = $api_url;
  $response['description'] = "A fully queryable REST API using JSON-LD and " .
      "discoverable using the WC3 Hydra specification.";

  // Lastly, add in the terms used into the @context section.
  $response['@context']['title'] = 'hydra:title';
  $response['@context']['entrypoint'] = array(
    "@id" => "hydra:entrypoint",
    "@type" => "@id",
  );
  $response['@context']['description'] = 'hydra:description';
}

/**
 * This function specifies the types of resources avaiable via the API.
 *
 * @param $api_url
 * @param $response
 * @param $ws_args
 */
function tripal_ws_handle_no_service($api_url, &$response) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['dc'] = 'http://purl.org/dc/dcmitype/';
  $response['@context']['schema'] = 'https://schema.org/';

  // Next add in the ID for tihs resource.
  $response['@id'] = $api_url;


  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = 'Services';
  $response['member'] = array();

  // Start the list.
  $response['member'][] = array(
    '@id' => $api_url . '/content/',
    '@type' => 'Service',
    'label' => 'Content Types',
    'description' => 'Provides acesss to the biological and ' .
      'ancilliary data available on this site. Each content type ' .
      'represents biological data that is defined in a controlled vocabulary '.
      '(e.g. Sequence Ontology term: gene (SO:0000704)).',
  );
  $response['member'][] = array(
    '@id' => $api_url . '/doc/',
    '@type' => 'Service',
    'label' => 'API Documentation',
    'description' => 'The WC3 Hydra compatible documentation for this API.',
  );
  $response['member'][] = array(
    '@id' => $api_url . '/vocab/',
    '@type' => 'Service',
    'label' => 'Vocabulary',
    'description' => 'Defines in-house locally defined vocabulary terms that ' .
      'have been added specifically for this site.  These terms are typically ' .
      'added because no other appropriate term exists in another community-vetted '.
      'controlled vocabulary.',
  );

  $response['totalItems'] = count($response['member']);

  $response['@context']['Collection'] = 'hydra:Collection';
  $response['@context']['totalItems'] = 'hydra:totalItems';
  $response['@context']['member'] = 'hydra:member';
  $response['@context']['Service'] = 'dc:Service';
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['description'] = 'hydra:description';
}