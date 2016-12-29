<?php



/**
 *
 */
function tripal_ws_services_v0_1($api_url, $ws_path, $params) {

  // Set some initial variables.
  $response = array();
  $version = 'v0.1';


  // Set some defaults for the response.
  $response['@context'] =  array();

  // Lump everything ito a try block so that if there is a problem we can
  // throw an error and have that returned in the response.
  try {

    // The services is the first argument
    $service = (count($ws_path) > 0) ? $ws_path[0] : '';

    switch ($service) {
      case 'doc':
        tripal_ws_services_v0_1_handle_doc_service($api_url, $response);
        break;
      case 'content':
        tripal_ws_services_v0_1_handle_content_service($api_url, $response, $ws_path, $params);
        break;
      case 'vocab':
        tripal_ws_services_v0_1_handle_vocab_service($api_url, $response, $ws_path);
        break;
      default:
        tripal_ws_services_v0_1_handle_no_service($api_url, $response);
    }
  }
  catch (Exception $e) {
    watchdog('tripal_ws', $e->getMessage(), array(), WATCHDOG_ERROR);
    $message = $e->getMessage();
    drupal_add_http_header('Status', '400 Bad Request');

  }

  return $response;
}


/**
 *
 * @param $api_url
 * @param $response
 * @param $ws_path
 */
function tripal_ws_services_v0_1_handle_content_service($api_url, &$response, $ws_path, $params) {

  // Get the content type.
  $ctype     = (count($ws_path) > 1) ? $ws_path[1] : '';
  $entity_id = (count($ws_path) > 2) ? $ws_path[2] : '';

  // If we have no content type then list all of the available content types.
  if (!$ctype) {
    tripal_ws_services_v0_1_get_content_types($api_url, $response);
  }
  // If we don't have an entity ID then show a paged list of entities with
  // the given type.
  else if ($ctype and !$entity_id) {
   tripal_ws_services_v0_1_get_content_type($api_url, $response, $ws_path, $ctype, $params);
  }
  // If we have a content type and an entity ID then show the entity
  else {
    tripal_ws_services_v0_1_get_content($api_url, $response, $ws_path, $ctype, $entity_id, $params);
  }
}
/**
*
* @param $api_url
* @param $response
* @param $ws_path
*/
function tripal_ws_services_v0_1_handle_vocab_service($api_url, &$response, $ws_path) {

  // Get the vocab name.
  $vocabulary = (count($ws_path) > 1) ? $ws_path[1] : '';
  $accession = (count($ws_path) > 2) ? $ws_path[2] : '';

  // If we have no $vocabulary type then list all of the available vocabs.
  if (!$vocabulary) {
    tripal_ws_services_v0_1_get_vocabs($api_url, $response);
  }
  // If we don't have a $vocabulary then show a paged list of terms.
  else if ($vocabulary and !$accession) {
    tripal_ws_services_v0_1_get_vocab($api_url, $response, $ws_path, $vocabulary);
  }
  // If we have a content type and an entity ID then show the entity
  else if ($vocabulary and $accession) {
    tripal_ws_services_v0_1_get_term($api_url, $response, $ws_path, $vocabulary, $accession);
  }
}

/**
 *
 * @param $api_url
 * @param $response
 */
function tripal_ws_services_v0_1_get_vocabs($api_url, &$response) {
  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for tihs resource.
  $response['@id'] = url($api_url . '/vocab', array('absolute' => TRUE));

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
    // Add the bundle as a content type.
    $response['member'][] = array(
      '@id' => url($api_url . '/vocab/' . urlencode($vocab->vocabulary), array('absolute' => TRUE)),
      '@type' => 'vocabulary',
      'vocabulary' => $vocab->vocabulary,
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
 * @param $ws_path
 */
function tripal_ws_services_v0_1_get_vocab($api_url, &$response, $ws_path, $vocabulary) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['schema'] = 'https://schema.org/';

  // Next add in the ID for tihs resource.
  $response['@id'] = url($api_url . '/vocab/' . $vocabulary, array('absolute' => TRUE));

  // Get the vocabulary
  $vocab = tripal_load_vocab_entity(array('vocabulary' => $vocabulary));

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = vocabulary . " vocabulary collection";
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
      '@id' => url($api_url . '/vocab/' . urlencode($vocabulary) . '/' .  urlencode($term->accession), array('absolute' => TRUE)),
      '@type' => 'vocabulary_term',
      'vocabulary' => $vocab->vocabulary,
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
 * @param $ws_path
 */
function tripal_ws_services_v0_1_get_term($api_url, &$response, $ws_path, $vocabulary, $accession) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['schema'] = 'https://schema.org/';

  // Get the term.
  $term = tripal_load_term_entity(array('vocabulary' => $vocabulary, 'accession' => $accession));

  // Next add in the ID and Type for this resources.
  $response['@id'] = url($api_url . '/vocab/' . urlencode($vocabulary) . '/' . urlencode($accession), array('absolute' => TRUE));
  $response['@type'] = 'vocabulary_term';
  $response['label'] = $term->name;
  $response['vocabulary'] = $vocabulary;
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
function tripal_ws_services_v0_1_get_content_types($api_url, &$response) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for tihs resource.
  $response['@id'] = url($api_url . '/content', array('absolute' => TRUE));

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

  // Iterate through the terms and add an entry in the collection.
  $i = 0;
  while ($bundle = $bundles->fetchObject()) {
    $entity =  entity_load('TripalTerm', array('id' => $bundle->term_id));
    $term = reset($entity);
    $vocab = $term->vocab;

    $response['@context'][$term->name] = $term->url;

    // Get the bundle description. If no description is provided then
    // use the term definition
    $description = tripal_get_bundle_variable('description', $bundle->id);
    if (!$description) {
      $description = $term->definition;
    }
    // Add the bundle as a content type.
    $response['member'][] = array(
      '@id' => url($api_url . '/content/' . urlencode($bundle->label), array('absolute' => TRUE)),
      '@type' => $term->name,
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
 * @param $ws_path
 */
function tripal_ws_services_v0_1_get_content_type($api_url, &$response, $ws_path, $ctype, $params) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for this resource.
  $URL = url($api_url . '/content/' . $ctype, array('absolute' => TRUE));
  $response['@id'] = $URL;

  // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
  $bundle = tripal_load_bundle_entity(array('label' => $ctype));
  $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
  $term = reset($term);
  $response['@context'][$term->name] = $term->url;

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = $bundle->label . " collection";

  // Iterate through the fields and create a $field_mapping array that makes
  // it easier to determine which filter criteria belongs to which field. The
  // key is the label for the field and the value is the field name. This way
  // user's can use the field label or the field name to form a query.
  $field_mapping = array();
  $fields = field_info_fields();
  foreach ($fields as $field) {
    if (array_key_exists('TripalEntity', $field['bundles'])) {
      foreach ($field['bundles']['TripalEntity'] as $bundle_name) {
        if ($bundle_name == $bundle->name) {
          $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
          if (array_key_exists('term_accession', $instance['settings'])){
            $vocabulary = $instance['settings']['term_vocabulary'];
            $accession = $instance['settings']['term_accession'];
            $term = tripal_get_term_details($vocabulary, $accession);
            $key = $term['name'];
            $key = strtolower(preg_replace('/ /', '_', $key));
            $field_mapping[$key] = $field['field_name'];
            $field_mapping[$field['field_name']] = $field['field_name'];
          }
        }
      }
    }
  }
  // Convert the filters to their field names
  $new_params = array();
  foreach ($params as $param => $value) {

    // Ignore non filter parameters
    if ($param == 'page' or $param == 'limit') {
      continue;
    }

    // Break apart any operators
    $key = $param;
    $op = '=';
    $matches = array();
    if (preg_match('/^(.+);(.+)$/', $key, $matches)) {
      $key = $matches[1];
      $op = $matches[2];
    }

    // Break apart any subkeys and pull the first one out for the term name key.
    $subkeys = explode(',', $key);
    if (count($subkeys) > 0) {
      $key = array_shift($subkeys);
    }
    $column_name = $key;

    // Map the values in the filters to their appropriate field names.
    if (array_key_exists($key, $field_mapping)) {
      $field_name = $field_mapping[$key];
      if (count($subkeys) > 0) {
        $column_name .= '.' . implode('.', $subkeys);
      }
      $new_params[$field_name]['value'] = $value;
      $new_params[$field_name]['op'] = $op;
    }
    else {
      throw new Exception("The filter term, '$key', is not available for use.");
    }
  }

  // Get the list of entities for this bundle.
  $query = new TripalFieldQuery();
  $query->entityCondition('entity_type', 'TripalEntity');
  $query->entityCondition('bundle', $bundle->name);
  foreach($new_params as $field_name => $details) {
    $value = $details['value'];
    switch ($details['op']) {
      case 'eq':
        $op = '=';
        break;
      case 'gt':
        $op = '>';
        break;
      case 'gte':
        $op = '>=';
        break;
      case 'lt':
        $op = '<';
        break;
      case 'lte':
        $op = '<=';
        break;
      case 'ne':
        $op = '<>';
        break;
      case 'contains':
        $op = 'CONTAINS';
        break;
      case 'starts':
        $op = 'STARTS WITH';
        break;
      default:
        $op = '=';
    }
    // We pass in the $column_name as an identifier for any sub fields
    // that are present for the fields.
    $query->fieldCondition($field_name, $column_name, $value, $op);
  }

  // Perform the query just as a count first to get the number of records.
  $cquery = clone $query;
  $cquery->count();
  $num_records = $cquery->execute();

  if (!$num_records) {
    $num_records = 0;
  }

  // Add in the pager to the response.
  $response['totalItems'] = $num_records;
  $limit = array_key_exists('limit', $params) ? $params['limit'] : 25;

  $total_pages = ceil($num_records / $limit);
  $page = array_key_exists('page', $params) ? $params['page'] : 1;
  if ($num_records > 0) {
    $response['view'] = array(
      '@id' => $URL . "?page=$page&limit=$limit",
      '@type' => 'PartialCollectionView',
      'first' => $URL . "?page=1&limit=$limit",
      'last' => $URL . "?page=$total_pages&limit=$limit",
    );
    $prev = $page - 1;
    $next = $page + 1;
    if ($prev > 0) {
      $response['view']['previous'] = $URL . "?page=$prev&limit=$limit";
    }
    if ($next < $total_pages) {
      $response['view']['next'] = $URL . "?page=$next&limit=$limit";
    }
  }

  // Now perform the query.
  $start = ($page - 1) * $limit;
  $query->range($start, $limit);
  $results = $query->execute();

  // Iterate through the entities and add them to the list.
  $i = 0;
  foreach ($results['TripalEntity'] as $entity_id => $stub) {
   $vocabulary = '';
   $term_name = '';

   // We don't need all of the attached fields for an entity so, we'll
   // not use the entity_load() function.  Instead just pull it from the
   // database table.
   $query = db_select('tripal_entity', 'TE');
   $query->join('tripal_term', 'TT', 'TE.term_id = TT.id');
   $query->fields('TE');
   $query->fields('TT', array('name'));
   $query->condition('TE.id', $entity_id);
   $entity = $query->execute()->fetchObject();

    //$entity = tripal_load_entity('TripalEntity', array($entity->id));
    $response['member'][] = array(
      '@id' => url($api_url . '/content/' . urlencode($ctype) . '/' .  $entity->id, array('absolute' => TRUE)),
      '@type' => $entity->name,
      'label' => $entity->title,
      'itemPage' => url('/bio_data/' . $entity->id, array('absolute' => TRUE)),
    );
    $i++;
  }


  // Lastly, add in the terms used into the @context section.
  $response['@context']['Collection'] = 'hydra:Collection';
  $response['@context']['totalItems'] = 'hydra:totalItems';
  $response['@context']['member'] = 'hydra:member';
  $response['@context']['label'] = 'rdfs:label';
  $response['@context']['itemPage'] = 'schema:itemPage';

//   $response['operation'][] = array(
//     '@type' => 'hydra:CreateResourceOperation',
//     'hydra:method' => 'PUT'
//   );

//   $response['query'] = array(
//     '@id' => $response['@id'],
//     '@type' => 'IriTemplate',
//     "template" => $response['@id'] . "{?name,}",
//     "mapping" => array(
//       array(
//         "hydra:variable" => 'name',
//         "hydra:property" => 'name',
//       )
//     )
//   );
}

/**
 *
 * @param unknown $response
 * @param unknown $ws_path
 * @param unknown $ctype
 * @param unknown $entity_id
 * @param unknown $params
 */
function tripal_ws_services_v0_1_get_content_add_fields($entity, $bundle, $api_url, &$response, $ws_path, $ctype, $entity_id, $params) {


  // Get information about the fields attached to this bundle and sort them
  // in the order they were set for the display.
  $instances = field_info_instances('TripalEntity', $bundle->name);

  uasort($instances, function($a, $b) {
    $a_weight = (is_array($a) && isset($a['widget']['weight'])) ? $a['widget']['weight'] : 0;
    $b_weight = (is_array($b) && isset($b['widget']['weight'])) ? $b['widget']['weight'] : 0;

    if ($a_weight == $b_weight) {
      return 0;
    }
    return ($a_weight < $b_weight) ? -1 : 1;
  });

  // Iterate through the fields and add each value to the response.
  //$response['fields'] = $fields;
  foreach ($instances as $field_name => $instance) {

    // Ignore the content_type field provided by Tripal.
    if ($field_name == 'content_type') {
      continue;
    }

    // Skip hidden fields.
    if ($instance['display']['default']['type'] == 'hidden') {
      continue;
    }

    // Get the information about this field. It will have settings different
    // from the instance.
    $field = field_info_field($field_name);

    // By default, the label for the key in the output should be the
    // term from the vocabulary that the field is assigned. But in the
    // case that the field is not assigned a term, we must use the field name.
    $field_name = $instance['field_name'];
    $vocabulary = $instance['settings']['term_vocabulary'];
    $accession = $instance['settings']['term_accession'];
    $term = tripal_get_term_details($vocabulary, $accession);
    if ($term) {
      $key = $term['name'];
      $key_adj = strtolower(preg_replace('/ /', '_', $key));
      // The term schema:url also points to a recource so we need
      // to make sure we set the type to be '@id'.
      if ($vocabulary == 'schema' and $accession == 'url') {
        $response['@context'][$key_adj] = array(
          '@id' => $term['url'],
          '@type' => '@id',
        );
      }
      else {
        $response['@context'][$key_adj] = $term['url'];
      }
    }
    else {
      continue;
    }

    // If this field should not be attached by default then just add a link
    // so that the caller can get the information separately.
    $instance_settings = $instance['settings'];
    if (array_key_exists('auto_attach', $instance['settings']) and
        !$instance_settings['auto_attach']) {
      $response['@context'][$key_adj] = array(
        '@id' => $response['@context'][$key_adj],
        '@type' => '@id'
      );
      // Add a URL only if there are values. If there are no values then
      // don't add a URL which would make the end-user think they can get
      // that information.
      $items = field_get_items('TripalEntity', $entity, $field_name);
      if ($items and count($items) > 0 and $items[0]['value']) {
        $response[$key_adj] = url($api_url . '/content/' . $ctype . '/' . $entity->id . '/' . urlencode($key), array('absolute' => TRUE));
      }
      else {
        $response[$key_adj] = NULL;
      }

      continue;
    }

    // Get the details for this field for the JSON-LD response.
    tripal_ws_services_v0_1_get_content_add_field($key_adj, $entity, $field, $instance, $api_url, $response);
  }

  // Lastly, add in the terms used into the @context section.
  $response['@context']['label'] = 'https://www.w3.org/TR/rdf-schema/#ch_label';
  $response['@context']['itemPage'] = 'https://schema.org/ItemPage';

  //   $response['operation'][] = array(
  //     '@type' => 'hydra:DeleteResourceOperation',
  //     'hydra:method' => 'DELETE'
  //   );
  //   $response['operation'][] = array(
  //     '@type' => 'hydra:ReplaceResourceOperation',
  //     'hydra:method' => 'POST'
  //   );
}
/**
 *
 * @param unknown $field_arg
 * @param unknown $api_url
 * @param unknown $response
 * @param unknown $ws_path
 * @param unknown $ctype
 * @param unknown $entity_id
 * @param unknown $params
 */
function tripal_ws_services_v0_1_get_content_find_field($field_arg, $ctype, $entity_id) {

  $bundle = tripal_load_bundle_entity(array('label' => $ctype));
  $entity = tripal_load_entity('TripalEntity', array('id' => $entity_id));
  $entity = reset($entity);
  $term = NULL;

  // Find the field whose term matches the one provied.
  $value = array();
  $instances = field_info_instances('TripalEntity', $bundle->name);
  foreach ($instances as $instance) {
    $field_name = $instance['field_name'];
    $field = field_info_field($field_name);
    $vocabulary = $instance['settings']['term_vocabulary'];
    $accession = $instance['settings']['term_accession'];
    $temp_term = tripal_get_term_details($vocabulary, $accession);
    if ($temp_term['name'] == $field_arg) {
      return array($entity, $bundle, $field, $instance, $temp_term);
    }
  }
}
/**
 *
 * @param unknown $api_url
 * @param unknown $response
 * @param unknown $ws_path
 * @param unknown $ctype
 * @param unknown $entity_id
 * @param unknown $params
 * @return number
 */
function tripal_ws_services_v0_1_get_content($api_url, &$response, $ws_path, $ctype, $entity_id, $params) {
  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // If we have an argument in the 4th element (3rd index) then the user
  // is requesting to expand the details of a field that was not
  // initially attached to the enity.
  $field_arg = '';
  if (array_key_exists(3, $ws_path)) {

    $field_arg = urldecode($ws_path[3]);
    list($entity, $bundle, $field, $instance, $term) = tripal_ws_services_v0_1_get_content_find_field($field_arg, $ctype, $entity_id);

    // If we couldn't match this field argument to a field and entity then return
    if (!$entity or !$field) {
      return;
    }

    // Next add in the ID and Type for this resources.
    $key = $term['name'];
    $key_adj = strtolower(preg_replace('/ /', '_', $term['name']));
    $response['@context'][$key_adj] = $term['url'];
    $response['@id'] = url($api_url . '/content/' . $ctype . '/' . $entity->id . '/' . urlencode($key), array('absolute' => TRUE));

    // Attach the field and then add it's values to the response.
    field_attach_load($entity->type, array($entity->id => $entity),
      FIELD_LOAD_CURRENT, array('field_id' => $field['id']));

    tripal_ws_services_v0_1_get_content_add_field($key_adj, $entity, $field, $instance, $api_url, $response, TRUE);
    tripal_ws_services_v0_1_write_context($response, $ctype);
    return;
  }

  // If we don't have a 4th argument then we're loading the base record.
  // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
  $bundle = tripal_load_bundle_entity(array('label' => $ctype));
  $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
  $term = reset($term);
  $vocab = $term->vocab;

  // Add the vocabulary for this content type to the @context section.
  if (!array_key_exists($vocab->vocabulary, $response['@context'])) {
    // If there is no URL prefix then use this API's vocabulary API
    if (property_exists($term, 'urlprefix')) {
      $response['@context'][$vocab->vocabulary] = $term->urlprefix;
    }
    else {
      $response['@context'][$vocab->vocabulary] = url($api_url . '/vocab/' . $vocab->vocabulary . '/', array('absolute' => TRUE));
    }
  }

  // Get the TripalEntity
  $entity = tripal_load_entity('TripalEntity', array('id' => $entity_id));
  $entity = reset($entity);

  // Next add in the ID and Type for this resources.
  $response['@id'] = url($api_url . '/content/' . $ctype . '/' . $entity_id, array('absolute' => TRUE));
  $response['@type'] = $term->name;
  $response['@context'][$term->name] = $term->url;
  $response['label'] = $entity->title;
  $response['itemPage'] = url('/bio_data/' . $entity->id, array('absolute' => TRUE));

  tripal_ws_services_v0_1_get_content_add_fields($entity, $bundle, $api_url, $response, $ws_path, $ctype, $entity_id, $params);
  tripal_ws_services_v0_1_write_context($response, $ctype);
}

/**
 *
 * @param $response
 * @param $ctype
 */
function tripal_ws_services_v0_1_write_context(&$response, $ctype) {
  // Save the response '@context' into a temporary file
  $context = array('@context' => $response['@context']);
  $file_name = drupal_tempnam(file_default_scheme() . '://', 'tws_context-') . '.json';
  $context_file = file_save_data(json_encode($context), $file_name, FILE_EXISTS_REPLACE );

  // Mark the file as temporary by setting it's status
  $context_file->status = 0;
  file_save($context_file);

  // Return the response with the '@context' section replaced with the file URL.
  $response['@context'] = file_create_url($context_file->uri);

}

/**
*
*/
function tripal_ws_services_v0_1_get_content_add_field($key, $entity, $field, $instance, $api_url, &$response, $is_field_page = NULL) {
  // Get the field  settings.
  $field_name = $field['field_name'];
  $field_settings = $field['settings'];

  $items = field_get_items('TripalEntity', $entity, $field_name);
  if (!$items) {
    return;
  }

  $values = array();
  for ($i = 0; $i < count($items); $i++) {

    // If the value is an array rather than a scalar then map the sub elements
    // to controlled vocabulary terms.
    if (is_array($items[$i]['value'])) {
      $temp = array();
      foreach ($items[$i]['value'] as $k => $v) {
        $matches = array();
        if (preg_match('/^(.+):(.+)$/', $k, $matches)) {
          $vocabulary = $matches[1];
          $accession = $matches[2];
          $term = tripal_get_term_details($vocabulary, $accession);
          $key_adj = strtolower(preg_replace('/ /', '_', $term['name']));
          $temp[$key_adj] = $v != "" ? $v : NULL;
          // The term schema:url also points to a recource so we need
          // to make sure we set the type to be '@id'.
          if ($vocabulary == 'schema' and $accession == 'url') {
            $response['@context'][$key_adj] = array(
              '@id' => $term['url'],
              '@type' => '@id',
            );
          }
          else {
            $response['@context'][$key_adj] = $term['url'];
          }
        }
      }
      $values[] = $temp;
    }
    else {
      $values[] = $items[$i]['value'] != "" ? $items[$i]['value'] : NULL;
    }

    // Recurse through the values array and set the entity elemetns
    // and add the fields to the context.
    tripal_ws_services_v0_1_get_content_add_field_context($items[$i], $response, $api_url);
  }

  // If we only have one value then set the response with just the value.
  if (count($values) == 1) {
    // If the value is an array and this is the field page then all of those
    // key/value pairs should be added directly to the response.
    if (is_array($values[0])) {
      if ($is_field_page) {
        foreach ($values[0] as $k => $v) {
          $response[$k] = $v;
        }
      }
      else {
        $response[$key] = $values[0];
      }
    }
    // If the value is not an array it's a scalar so add it as is to the
    // response.
    else {
      $response[$key] = $values[0];
    }
  }

  // If we have more than one value then set the response to be a collection.
  if (count($values) > 1) {

    // If this is the field page then the Collection is added directly to the
    // response, otherwise, it's added under the field $key.
    if ($is_field_page) {
      $response['@type'] = 'Collection';
      $response['totalItems'] = count($values);
      $response['label'] = $instance['label'];
      $response['member'] = $values;
    }
    else {
      $response[$key] = array(
        '@type' => 'Collection',
        'totalItems' => count($values),
        'label' => $instance['label'],
        'member' => $values,
      );
    }
  }
}

/**
 *
 */
function tripal_ws_services_v0_1_get_content_add_field_context(&$items, &$response, $api_url) {

  foreach ($items as $key => $value) {
    if (is_array($value)) {
      tripal_ws_services_v0_1_get_content_add_field_context($items[$key], $response, $api_url);
      continue;
    }

    if ($key == 'entity') {
      list($item_etype, $item_eid) = explode(':', $items['entity']);
      if ($item_eid) {
        $item_entity = tripal_load_entity($item_etype, array($item_eid));
        $item_entity = reset($item_entity);
        $item_term = tripal_load_term_entity(array('term_id' => $item_entity->term_id));
        $items['@id'] = url($api_url . '/content/' . $item_term->name . '/' . $item_eid, array('absolute' => TRUE));
      }
      unset($items['entity']);
    }
  }
}

/**
 * Provides the Hydra compatible apiDocumentation page that describes this API.
 *
 * @param $api_url
 * @param $response
 */
function tripal_ws_services_v0_1_handle_doc_service($api_url, &$response) {
  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';

  // Next add in the ID for tihs resource.
  $site_name = variable_get('site_name', '');
  $response['@id'] = url($api_url . '/doc/', array('absolute' => TRUE));
  $response['title'] =  $site_name . ": RESTful Web Services API";
  $response['entrypoint'] = url($api_url, array('absolute' => TRUE));
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
 * @param $ws_path
 */
function tripal_ws_services_v0_1_handle_no_service($api_url, &$response) {

  // First, add the vocabularies used into the @context section.
  $response['@context']['rdfs'] = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
  $response['@context']['hydra'] = 'http://www.w3.org/ns/hydra/core#';
  $response['@context']['dc'] = 'http://purl.org/dc/dcmitype/';
  $response['@context']['schema'] = 'https://schema.org/';

  // Next add in the ID for tihs resource.
  $response['@id'] = url($api_url, array('absolute' => TRUE));

  // Start the list.
  $response['@type'] = 'Collection';
  $response['totalItems'] = 0;
  $response['label'] = 'Services';
  $response['member'] = array();

  // Start the list.
  $response['member'][] = array(
    '@id' => url($api_url . '/content/', array('absolute' => TRUE)),
    '@type' => 'Service',
    'label' => 'Content Types',
    'description' => 'Provides acesss to the biological and ' .
    'ancilliary data available on this site. Each content type ' .
    'represents biological data that is defined in a controlled vocabulary '.
    '(e.g. Sequence Ontology term: gene (SO:0000704)).',
  );
  $response['member'][] = array(
    '@id' => url($api_url . '/doc/', array('absolute' => TRUE)),
    '@type' => 'Service',
    'label' => 'API Documentation',
    'description' => 'The WC3 Hydra compatible documentation for this API.',
  );
  $response['member'][] = array(
    '@id' => url($api_url . '/vocab/', array('absolute' => TRUE)),
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