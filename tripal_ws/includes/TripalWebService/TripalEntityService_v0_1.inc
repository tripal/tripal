<?php

class TripalEntityService_v0_1 extends TripalWebService {

  /**
   * The human-readable label for this web service.
   */
  public static $label = 'Content Types';
  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'Provides acesss to the biological and ' .
    'ancilliary data available on this site. Each content type represents ' .
    'biological data that is defined in a controlled vocabulary (e.g. ' .
    'Sequence Ontology term: gene (SO:0000704)).';
  /**
   * A machine-readable type for this service. This name must be unique
   * among all Tripal web services and is used to form the URL to access
   * this service.
   */
  public static $type = 'content';


  /**
   * Implements the constructor
   */
  public function __construct($base_path) {
    parent::__construct($base_path);
  }

  /**
   * @see TripalWebService::handleRequest()
   */
  public function handleRequest() {

    // Get the content type.
    $ctype     = (count($this->path) > 0) ? $this->path[0] : '';
    $entity_id = (count($this->path) > 1) ? $this->path[1] : '';
    $expfield  = (count($this->path) > 2) ? $this->path[2] : '';

    // If we have a content type then list all of the entities that belong
    // to it.
    if ($ctype and !$entity_id and !$expfield) {
      $this->doContentTypeList($ctype);
    }
    // If we have an entity ID then build the resource for a single entity.
    else if ($ctype and $entity_id and !$expfield) {
      $this->doEntity($ctype, $entity_id);
    }
    else if ($ctype and $entity_id and $expfield) {
      $this->doExpandedField($ctype, $entity_id, $expfield);
    }
    // Otherwise just list all of the available content types.
    else {
      $this->doAllTypesList();
    }
  }

  /**
   * Creates a resource for an expanded field of an entity.
   */
  private function doExpandedField($ctype, $entity_id, $expfield) {
    $service_path = $this->getServicePath() . '/' . urlencode($ctype) . '/' . $entity_id;
    $this->resource = new TripalWebServiceResource($service_path);

    // Get the TripalBundle, TripalTerm and TripalVocab for this type.
    $bundle = tripal_load_bundle_entity(array('label' => $ctype));
    $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
    $term = reset($term);
    $vocab = $term->vocab;

    // Get the TripalEntity
    $entity = tripal_load_entity('TripalEntity', array('id' => $entity_id));
    $entity = reset($entity);

    // If we couldn't match this field argument to a field and entity then return
    if (!$entity) {
      throw new Exception("Cannot find this entity.");
    }

    list($field, $instance, $term) = $this->findField($bundle, $expfield);

    // Next add in the ID and Type for this resources.
    $key = $term['name'];
    $key_adj = strtolower(preg_replace('/ /', '_', $term['name']));
    $this->resource->addContextItem($key_adj, $term['url']);
    $this->resource->setID(urlencode($key));
    $this->resource->setType($key_adj);

    // Attach the field and then add it's values to the response.
    field_attach_load($entity->type, array($entity->id => $entity),
        FIELD_LOAD_CURRENT, array('field_id' => $field['id']));

    $this->addEntityField($key_adj, $term, $entity, $bundle, $field, $instance, $service_path, $expfield);
  }

  /**
   * Find the field whose term matches the one provied.
   */
  private function findField($bundle, $expfield) {

    $value = array();
    $instances = field_info_instances('TripalEntity', $bundle->name);
    foreach ($instances as $instance) {
      $field_name = $instance['field_name'];
      $field = field_info_field($field_name);
      $vocabulary = $instance['settings']['term_vocabulary'];
      $accession = $instance['settings']['term_accession'];
      $temp_term = tripal_get_term_details($vocabulary, $accession);
      if ($temp_term['name'] == $expfield) {
        return array($field, $instance, $temp_term);
      }
    }
  }

  /**
   * Creates a resource for a single entity.
   */
  private function doEntity($ctype, $entity_id) {
    $service_path = $this->getServicePath() . '/' . urlencode($ctype);
    $this->resource = new TripalWebServiceResource($service_path);

    // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
    $bundle = tripal_load_bundle_entity(array('label' => $ctype));
    $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
    $term = reset($term);
    $vocab = $term->vocab;

    // Add the vocabulary to the context.
    $this->resource->addContextItem($term->name, $term->url);

    // Get the TripalEntity
    $entity = tripal_load_entity('TripalEntity', array('id' => $entity_id));
    $entity = reset($entity);

    $itemPage = tripal_get_term_details('schema', 'ItemPage');
    $label = tripal_get_term_details('rdfs', 'label');
    $this->resource->setID($entity_id);
    $this->resource->setType($term->name);
    $this->resource->addContextItem('label', $label['url']);
    $this->resource->addContextItem('ItemPage', $itemPage['url']);
    $this->resource->addProperty('label', $entity->title);
    $this->resource->addProperty('ItemPage', url('/bio_data/' . $entity->id, array('absolute' => TRUE)));

    $this->addEntityFields($entity, $bundle, $term, $service_path);

//    tripal_ws_services_v0_1_get_content_add_fields($entity, $bundle, $api_url, $response, $ws_path, $ctype, $entity_id, $params);
//    tripal_ws_services_v0_1_write_context($response, $ctype);
  }

  /**
   * Adds the fields as properties of an entity resource.
   */
  private function addEntityFields($entity, $bundle, $term, $service_path) {

    // If the entity is set to hide fields that have no values then we
    // want to honor that in the web services too.
    $hide_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id, 'hide');

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

      // Skip hidden fields.
      if ($instance['display']['default']['type'] == 'hidden') {
        continue;
      }

      // Get the information about this field.
      $field = field_info_field($field_name);

      // By default, the label for the key in the output should be the
      // term from the vocabulary that the field is assigned. But in the
      // case that the field is not assigned a term, we must use the field name.
      $field_name = $instance['field_name'];
      $vocabulary = $instance['settings']['term_vocabulary'];
      $accession = $instance['settings']['term_accession'];
      $term = tripal_get_term_details($vocabulary, $accession);
      if (!$term) {
        continue;
      }
      $key = $term['name'];
      $key_adj = strtolower(preg_replace('/ /', '_', $key));

      // If this field should not be attached by default then just add a link
      // so that the caller can get the information separately.
      $instance_settings = $instance['settings'];
      if (array_key_exists('auto_attach', $instance['settings']) and
          $instance_settings['auto_attach'] == FALSE) {
        // Add a URL only if there are values. If there are no values then
        // don't add a URL which would make the end-user think they can get
        // that information.
        $items = field_get_items('TripalEntity', $entity, $field_name);
        if ($items and count($items) > 0 and $items[0]['value']) {
          $this->resource->addContextItem($key_adj, $term['url']);
          $this->resource->addProperty($key_adj, $service_path . '/' . $entity->id . '/' . urlencode($term['name']));
        }
        else {
          if ($hide_fields == 'show') {
            $this->resource->addContextItem($key_adj, $term['url']);
            $this->resource->addProperty($key_adj, NULL);
          }
        }
        continue;
      }

      // Get the details for this field for the JSON-LD response.
      $this->addEntityField($key_adj, $term, $entity, $bundle, $field, $instance, $service_path);
    }
  }

  /**
   * Adds the field as a property of the entity resource.
   */
  private function addEntityField($key, $term, $entity, $bundle, $field, $instance,
      $service_path, $expfield = NULL) {

    // If the entity is set to hide fields that have no values then we
    // want to honor that in the web services too.
    $hide_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id, 'hide');

    // Get the field  settings.
    $field_name = $field['field_name'];
    $field_settings = $field['settings'];

    $items = field_get_items('TripalEntity', $entity, $field_name);
    if (!$items) {
      return;
    }

    // Give modules the opportunity to edit values for web services. This hook
    // really should be used sparingly. Where it helps is with non Tripal fields
    // that are added to a TripalEntity content type and it doesn't follow
    // the rules (e.g. Image field).
    drupal_alter('tripal_ws_value', $items, $field, $instance);

    $values = array();
    for ($i = 0; $i < count($items); $i++) {
      $values[$i] = $this->sanitizeFieldKeys($items[$i]['value'], $bundle, $service_path);
    }

    if ($hide_fields == 'hide' and empty($values[0])) {
      return;
    }

    // If the field cardinality is 1
    if ($field['cardinality'] == 1) {

      // If the value is an array and this is the field page then all of those
      // key/value pairs should be added directly to the response.
      if (is_array($values[0])) {
        if ($expfield) {
          foreach ($values[0] as $k => $v) {
            $this->resource->addProperty($k, $v);
          }
        }
        else {
          $this->resource->addContextItem($key, $term['url']);
          $this->resource->addProperty($key, $values[0]);
        }
      }
      // If the value is not an array it's a scalar so add it as is to the
      // response.
      else {
        $this->resource->addContextItem($key, $term['url']);
        $this->resource->addProperty($key, $values[0]);
      }
    }

    // If the field cardinality is > 1
    if ($field['cardinality'] != 1) {

      // If this is the expanded field page then we need to swap out
      // the resource for a collection.
      $response = new TripalWebServiceCollection($service_path . '/' . urlencode($expfield));
      $label = tripal_get_term_details('rdfs', 'label');
      $response->addContextItem('label', $label['url']);
      $response->addProperty('label', $instance['label']);
      $i = 0;
      foreach ($values as $delta => $element) {
        $member = new TripalWebServiceResource($service_path . '/' . urlencode($expfield));
        $member->setID($i);
        // Add the context of the parent resource because all of the keys
        // were santizied and set to match the proper context.
        $member->setContext($this->resource);
        $member->setType($key);
        foreach ($element as $key => $value) {
          $member->addProperty($key, $value);
        }
        $response->addMember($member);
        $i++;
      }
      if ($expfield) {
        $this->resource = $response;
      }
      else {
        $this->resource->addProperty($key, $response);
      }
    }
  }

  /**
   * Rewrites the keys of a field's items array for use with web services.
   */
  private function sanitizeFieldKeys($value, $bundle, $service_path) {
    // If the entity is set to hide fields that have no values then we
    // want to honor that in the web services too.
    $hide_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id, 'hide');

    $new_value = '';
    // If the value is an array rather than a scalar then map the sub elements
    // to controlled vocabulary terms.
    if (is_array($value)) {
      $temp = array();
      foreach ($value as $k => $v) {
        // exclude fields that have no values so we can hide them
        if (empty($v) and $hide_fields == 'hide') {
          continue;
        }
        $matches = array();
        if (preg_match('/^(.+):(.+)$/', $k, $matches)) {
          $vocabulary = $matches[1];
          $accession = $matches[2];
          $term = tripal_get_term_details($vocabulary, $accession);

          $key_adj = strtolower(preg_replace('/ /', '_', $term['name']));
          if (is_array($v)) {
            $temp[$key_adj] = $this->sanitizeFieldKeys($v, $bundle, $service_path);
          }
          else {
            $temp[$key_adj] = $v !== "" ? $v : NULL;
          }
          $this->resource->addContextItem($key_adj, $term['url']);

        }
        else {
          // TODO: this is an error, if we get here then we have
          // a key that isn't using the proper format... what to do?
        }
      }
      $new_value = $temp;

      // Recurse through the values array and set the entity elements
      // and add the fields to the context.
      $this->sanitizeFieldEntity($new_value, $service_path);

    }
    else {
      $new_value = $value !== "" ? $value : NULL;
    }

    return $new_value;
  }

  /**
   * Rewrites any TripalEntity elements in the values array for use with WS.
   */
  private function sanitizeFieldEntity(&$items, $service_path) {

    if (!$items) {
      return;
    }
    foreach ($items as $key => $value) {
      if (is_array($value)) {
        $this->sanitizeFieldEntity($items[$key], $service_path);
        continue;
      }

      if ($key == 'entity') {
        list($item_etype, $item_eid) = explode(':', $items['entity']);
        if ($item_eid) {
          $item_entity = tripal_load_entity($item_etype, array($item_eid));
          $item_entity = reset($item_entity);
          $bundle = tripal_load_bundle_entity(array('name' => $item_entity->bundle));
          $items['@id'] = $this->getServicePath() . '/' . urlencode($bundle->label) . '/' . $item_eid;
        }
        unset($items['entity']);
      }
    }
  }

  /**
   * Creates a collection of resources for a given type.
   */
  private function doContentTypeList($ctype) {
    $service_path = $this->getServicePath() . '/' . urlencode($ctype);
    $label = tripal_get_term_details('rdfs', 'label');
    $this->resource = new TripalWebServiceCollection($service_path);
    $this->resource->addContextItem('label', $label['url']);

    // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
    $bundle = tripal_load_bundle_entity(array('label' => $ctype));
    $term = entity_load('TripalTerm', array('id' => $bundle->term_id));
    $term = reset($term);

    // Set the label for this collection.
    $this->resource->addProperty('label', $bundle->label . " collection");

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
              $fterm = tripal_get_term_details($vocabulary, $accession);
              $key = $fterm['name'];
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
    $order = array();
    $order_dir = array();
    $URL_add = array();
    foreach ($this->params as $param => $value) {
      $URL_add[] = "$param=$value";

      // Ignore non filter parameters
      if ($param == 'page' or $param == 'limit') {
        continue;
      }

      // Handle order separately
      if ($param == 'order') {
        $temp = explode(',', $value);
        foreach ($temp as $key) {
          $matches = array();
          $dir = 'ASC';
          // The user can provide a direction by separating the field key and the
          // direction with a '|' character.
          if (preg_match('/^(.*)\|(.*)$/', $key, $matches)) {
            $key = $matches[1];
            if ($matches[2] == 'ASC' or $matches[2] == 'DESC') {
              $dir = $matches[2];
            }
            else {
              // TODO: handle error of providing an incorrect direction.
            }
          }
          if (array_key_exists($key, $field_mapping)) {
            $order[$field_mapping[$key]] = $key;
            $order_dir[] = $dir;
          }
          else {
            // TODO: handle error of providing a non existing field name.
          }
        }
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

      // Break apart any subkeys and pull the first one as this is the parent
      // field.
      $subkeys = explode(',', $key);
      if (count($subkeys) > 0) {
        $key = $subkeys[0];
      }

      // Map the values in the filters to their appropriate field names.
      if (array_key_exists($key, $field_mapping)) {
        $key_field_name = $field_mapping[$key];
        $key_field = field_info_field($key_field_name);
        $key_instance = field_info_instance('TripalEntity', $key_field_name, $bundle->name);

        // Complex fields provied by the TripalField class may have sub
        // elements that support filtering.  We need to see if the user
        // wants to filter on those.
        if (tripal_load_include_field_class($key_field_name)) {
          // To find out which fields are searchable we'll call the wsData()
          // function.
          $key_field = new $key_field_name($key_field, $key_instance);
          $searchable_keys = $key_field->webServicesData();
          $criteria = implode('.', $subkeys);
          if (array_key_exists($criteria, $searchable_keys)) {
            $new_params[$key_field_name]['value'] = $value;
            $new_params[$key_field_name]['op'] = $op;
            $new_params[$key_field_name]['column'] = $searchable_keys[$criteria];
          }
          else {
            throw new Exception("The filter term, '$criteria', is not available for use.");
          }
        }
        // If this field is not a TripalField then it should just have
        // a simple value and we can query for that.
        else {
          $key_field_id = $key_instance['settings']['term_vocabulary'] . ':' . $key_instance['settings']['term_accession'];

          $new_params[$key_field_name]['value'] = $value;
          $new_params[$key_field_name]['op'] = $op;
          $new_params[$key_field_name]['column'] = $key_field_id;
        }
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
      $column_name = $details['column'];
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
    $limit = array_key_exists('limit', $this->params) ? $this->params['limit'] : 25;

    $total_pages = ceil($num_records / $limit);
    $page = array_key_exists('page', $this->params) ? $this->params['page'] : 1;

    // Set the query order
    $order_keys = array_keys($order);
    for($i = 0; $i < count($order_keys); $i++) {
      $query->fieldOrderBy($order_keys[$i], $order[$order_keys[$i]], $order_dir[$i]);
    }

    // Set the query range
    $start = ($page - 1) * $limit;
    $query->range($start, $limit);

    // Now perform the query.
    $results = $query->execute();

    $this->resource->initPager($num_records, $limit, $page);

    // Iterate through the entities and add them to the list.
    foreach ($results['TripalEntity'] as $entity_id => $stub) {
      // We don't need all of the attached fields for an entity so, we'll
      // not use the entity_load() function.  Instead just pull it from the
      // database table.
      $query = db_select('tripal_entity', 'TE');
      $query->join('tripal_term', 'TT', 'TE.term_id = TT.id');
      $query->fields('TE');
      $query->fields('TT', array('name'));
      $query->condition('TE.id', $entity_id);
      $entity = $query->execute()->fetchObject();

      $itemPage = tripal_get_term_details('schema', 'ItemPage');
      $label = tripal_get_term_details('rdfs', 'label');
      $member = new TripalWebServiceResource($service_path);
      $member->addContextItem('label', $label['url']);
      $member->addContextItem('ItemPage', $itemPage['url']);
      $member->addContextItem($term->name, $term->url);
      $member->setID($entity->id);
      $member->setType($term->name);
      $member->addProperty('label', $entity->title);
      $member->addProperty('ItemPage', url('/bio_data/' . $entity->id, array('absolute' => TRUE)));
      $this->resource->addMember($member);
    }
  }

  /**
   * Creates a resources that contains the list of content types.
   */
  private function doAllTypesList() {
    $service_path = $this->getServicePath();
    $label = tripal_get_term_details('rdfs', 'label');
    $this->resource = new TripalWebServiceCollection($service_path);
    $this->resource->addContextItem('label', $label['url']);
    $this->resource->addProperty('label', 'Content Types');

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

      // Get the bundle description. If no description is provided then
      // use the term definition
      $description = tripal_get_bundle_variable('description', $bundle->id);
      if (!$description) {
        $description = $term->definition;
      }
      $member = new TripalWebServiceResource($service_path);
      $member->addContextItem($term->name, $term->url);
      $member->addContextItem('label', $label['url']);
      $member->addContextItem('description', 'hydra:description');
      $member->setID(urlencode($bundle->label));
      $member->setType($term->name);
      $member->addProperty('label', $bundle->label);
      $member->addProperty('description', $description);
      $this->resource->addMember($member);

    }
  }
}