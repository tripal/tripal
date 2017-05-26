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
  public function __construct() {
    parent::__construct();

  }

  /**
   * @see TripalWebService::handleRequest()
   */
  public function handleRequest() {

    // Get the content type.
    $ctype     = (count($this->path) > 0) ? $this->path[0] : '';
    $entity_id = (count($this->path) > 1) ? $this->path[1] : '';

    // If we have no content type then list all of the available content types.
    if ($ctype and !$entity_id) {
      $this->doContentTypeList($ctype);
    }
    // If we don't have an entity ID then show a paged list of entities with
    // the given type.
    else if ($ctype and !$entity_id) {
    }
    // If we have a content type and an entity ID then show the entity
    else {
      $this->doAllTypesList();
    }
  }

  /**
   * Creates a collection of resources for a given type.
   */
  private function doContentTypeList($ctype) {
    $this->resource = new TripalWebServiceCollection();
    $this->resource->addContextItem('label', 'rdfs:label');

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
        $new_params[$field_name]['column'] = $column_name;
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

    //$this->resource->initPager($num_records, $params['limit']);
    $this->resource->initPager($num_records, 25);

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

      $member = new TripalWebServiceResource();
      $member->addContextItem('label', 'rdfs:label');
      $member->addContextItem('itemPage', 'schema:itemPage');
      $member->addContextItem($term->name, $term->url);
      $member->setID(urldecode($bundle->label) . '/' . $entity->id);
      $member->setType($term->name);
      $member->addProperty('label', $entity->title);
      $member->addProperty('itemPage', url('/bio_data/' . $entity->id, array('absolute' => TRUE)));
      $this->resource->addMember($member);
    }
  }

  /**
   * Creates a resources that contains the list of content types.
   */
  private function doAllTypesList() {
    $this->resource = new TripalWebServiceCollection();
    $this->resource->addContextItem('label', 'rdfs:label');
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

      $member = new TripalWebServiceResource();
      $member->addContextItem($term->name, $term->url);
      $member->addContextItem('label', 'rdfs:label');
      $member->addContextItem('description', 'hydra:description');
      $member->setID(urlencode($bundle->label));
      $member->setType($term->name);
      $member->addProperty('label', $bundle->label);
      $member->addProperty('description', $description);
      $this->resource->addMember($member);

    }
  }
}