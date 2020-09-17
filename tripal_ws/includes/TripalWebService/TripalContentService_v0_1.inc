<?php

class TripalContentService_v0_1 extends TripalWebService {

  /**
   * The human-readable label for this web service.
   */
  public static $label = 'Content Types';

  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'Provides acesss to the biological and ancilliary data available on this site. Each content type represents biological data that is defined in a controlled vocabulary (e.g. Sequence Ontology term: gene (SO:0000704)).';

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
   * @see TripalWebService::getDocumentation()
   */
  public function getDocumentation() {
    // Add the classes that this resource supports.
    $this->addDocBundleClasses();
    $this->addDocContentCollectionClass();

    return parent::getDocumentation();
  }

  /**
   * @see TripalWebService::handleRequest()
   */
  public function handleRequest() {

    // Get the content type.
    $ctype = (count($this->path) > 0) ? $this->path[0] : '';
    $entity_id = (count($this->path) > 1) ? $this->path[1] : '';
    $expfield = (count($this->path) > 2) ? $this->path[2] : '';

    // is this a valid content type?
    if ($ctype) {
      // Get the list of published terms (these are the bundle IDs)
      $bquery = db_select('tripal_bundle', 'tb');
      $bquery->join('tripal_term', 'tt', 'tt.id = tb.term_id');
      $bquery->join('tripal_vocab', 'tv', 'tv.id = tt.vocab_id');
      $bquery->fields('tb', ['label']);
      $bquery->fields('tt', ['accession']);
      $bquery->fields('tv', ['vocabulary']);
      $bquery->orderBy('tb.label', 'ASC');
      $bundles = $bquery->execute();

      // Iterate through the terms convert the santized name to the real label.
      $i = 0;
      $ctype_lookup = [];
      $found = FALSE;
      while ($bundle = $bundles->fetchObject()) {
        // Check the label by replacing non alpha-numeric characters with
        // an underscore and is case-insensitive
        $label = preg_replace('/[^\w]/', '_', $bundle->label);
        if (preg_match("/^$label$/i", $ctype)) {
          $ctype = $bundle->label;
          $found = TRUE;
        }
        // Check if this is an accession.
        if ($ctype == $bundle->vocabulary . ':' . $bundle->accession) {
          $ctype = $bundle->label;
          $found = TRUE;
        }
      }

      if (!$found) {
        throw new Exception('Invalid content type: ' . $ctype);
      }
    }

    // If we have a content type then list all of the entities that belong
    // to it.
    if ($ctype and !$entity_id and !$expfield) {
      $this->doEntityList($ctype);
    }
    // If we have an entity ID then build the resource for a single entity.
    else {
      if ($ctype and $entity_id and !$expfield) {
        $this->doEntity($ctype, $entity_id);
      }
      else {
        if ($ctype and $entity_id and $expfield) {
          $this->doExpandedField($ctype, $entity_id, $expfield);
        }
        // Otherwise just list all of the available content types.
        else {
          $this->doContentTypesList();
        }
      }
    }
  }

  /**
   * Creates a resource for an expanded field of an entity.
   */
  private function doExpandedField($ctype, $entity_id, $expfield) {
    $service_path = $this->getServicePath() . '/' . preg_replace('/[^\w]/', '_', $ctype) . '/' . $entity_id;
    $this->resource = new TripalWebServiceResource($service_path);

    // Get the TripalBundle, TripalTerm and TripalVocab for this type.
    $bundle = tripal_load_bundle_entity(['label' => $ctype]);

    // Find the field that matches the field name provided by the user.
    list($field, $instance, $term) = $this->findField($bundle, $expfield);

    if (!$field) {
      throw new Exception("Could not find a matching field for the name: $expfield");
    }

    // Get the TripalEntity
    $entity = tripal_load_entity('TripalEntity', ['id' => $entity_id], FALSE, [$field['id']]);
    $entity = reset($entity);

    // If we couldn't find the entity then fail.
    if (!$entity) {
      throw new Exception("Cannot find the record with id $entity_id.");
    }

    // Check that the user has access to this entity.  If not then the
    // function call will throw an error.
    $this->checkAccess($entity);


    // Next add in the ID and Type for this resources.
    $this->setResourceType($this->resource, $term);
    $this->resource->setID(urlencode($term['name']));

    if (!property_exists($entity, $field['field_name'])) {
      // Attach the field and then add its values to the response.
      field_attach_load($entity->type, [$entity->id => $entity],
        FIELD_LOAD_CURRENT, ['field_id' => $field['id']]);
    }

    $this->addEntityField($this->resource, $term, $entity, $bundle, $field, $instance, $service_path, $expfield);
  }

  /**
   * Find the field whose term matches the one provied.
   */
  private function findField($bundle, $expfield) {

    $value = [];
    $instances = field_info_instances('TripalEntity', $bundle->name);
    foreach ($instances as $instance) {
      $field_name = $instance['field_name'];
      $field = field_info_field($field_name);
      $field_type = $field['type'];

      // Skip fields of remote data.
      if ($field_type == 'remote__data') {
        continue;
      }

      $vocabulary = $instance['settings']['term_vocabulary'];
      $accession = $instance['settings']['term_accession'];
      $temp_term = tripal_get_term_details($vocabulary, $accession);

      // See if the name matches perfectly.
      if (strtolower($temp_term['name']) == strtolower($expfield)) {
        return [$field, $instance, $temp_term];
      }

      // See if the name provided matches the field name after a bit of
      // cleanup.
      if (strtolower(preg_replace('/[^\w]/', '_', $temp_term['name'])) == strtolower($expfield)) {
        return [$field, $instance, $temp_term];
      }

      // Alternatively if the CV term accession matches then we're good too.
      if ($vocabulary . ':' . $accession == $expfield) {
        return [$field, $instance, $temp_term];
      }
    }
  }

  /**
   * Creates a resource for a single entity.
   */
  private function doEntity($ctype, $entity_id) {
    $service_path = $this->getServicePath() . '/' . preg_replace('/[^\w]/', '_', $ctype);
    $this->resource = new TripalWebServiceResource($service_path);

    // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
    $bundle = tripal_load_bundle_entity(['label' => $ctype]);
    $term = entity_load('TripalTerm', ['id' => $bundle->term_id]);
    $term = reset($term);

    // Convert the $term to a simple array
    $term = tripal_get_term_details($term->vocab->vocabulary, $term->accession);

    // Add the vocabulary to the context.
    $this->resource->addContextItem($term['name'], $term['url']);

    // Get the TripalEntity.
    $entity = tripal_load_entity('TripalEntity', ['id' => $entity_id]);
    $entity = reset($entity);

    // If we couldn't match this field argument to a field and entity then return
    if (!$entity) {
      throw new Exception("Cannot find this record.");
    }

    // Check that the user has access to this entity.  If not then the
    // function call will throw an error.
    $this->checkAccess($entity);

    $itemPage = tripal_get_term_details('schema', 'ItemPage');
    $label = tripal_get_term_details('rdfs', 'label');
    $this->resource->setID($entity_id);
    $this->setResourceType($this->resource, $term);
    $this->addResourceProperty($this->resource, $label, $entity->title);
    $this->addResourceProperty($this->resource, $itemPage, url('/bio_data/' . $entity->id, ['absolute' => TRUE]));

    // Add in the entitie's fields.
    $this->addEntityFields($this->resource, $entity, $bundle, $term, $service_path);
  }

  /**
   * Ensures that user's only have access to content they should see.
   *
   * Denies access to an entity if it is unpublished or if the user does
   * not have permission to see it.
   *
   * @param $entity
   *   The full entity object.
   *
   * @throws Exception
   */
  private function checkAccess($entity) {
    global $user;

    if (!tripal_entity_access('view', $entity, $user, 'TripalEntity')) {
      throw new Exception("Permission Denied.");
    }
    // Don't show entities that aren't published
    if ($entity->status == 0) {
      throw new Exception("This record is currently unavailable.");
    }
  }

  /**
   * Adds the fields as properties of an entity resource.
   */
  private function addEntityFields($resource, $entity, $bundle, $term, $service_path) {

    // If the entity is set to hide fields that have no values then we
    // want to honor that in the web services too.
    $hide_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id);

    // Get information about the fields attached to this bundle and sort them
    // in the order they were set for the display.
    $instances = field_info_instances('TripalEntity', $bundle->name);

    // Sort the instances by their weight.
    uasort($instances, function ($a, $b) {
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

      // If the field has the $no_data turned on then we should exclude it.
      if (tripal_load_include_field_class($field['type'])) {
        $field_class = $field['type'];
        if ($field_class::$no_data) {
          return;
        }
      }

      // Skip the remote__data field that is provided by the tripal_ws
      // module.
      if ($field['type'] == 'remote__data') {
        continue;
      }

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

      // If this field should not be attached by default then just add a link
      // so that the caller can get the information separately.
      $instance_settings = $instance['settings'];
      if (array_key_exists('auto_attach', $instance['settings']) and
        $instance_settings['auto_attach'] == FALSE) {
        // Add a URL only if there are values. If there are no values then
        // don't add a URL which would make the end-user think they can get
        // that information.
        $items = field_get_items('TripalEntity', $entity, $field_name);
        $term_key = $this->getContextTerm($term, ['lowercase', 'spacing']);
        $resource->addContextItem($term_key, $vocabulary . ':' . $accession);
        $resource->addContextItem($vocabulary . ':' . $accession, [
          '@id' => $term['url'],
          '@type' => '@id',
        ]);
        if ($items and count($items) > 0 and $items[0]['value']) {
          $this->addResourceProperty($resource, $term, $service_path . '/' . $entity->id . '/' . urlencode($term['name']), [
            'lowercase',
            'spacing',
          ]);
        }
        else {
          if ($hide_fields == FALSE) {
            $this->addResourceProperty($resource, $term, NULL, [
              'lowercase',
              'spacing',
            ]);
          }
        }
        continue;
      }

      // Get the details for this field for the JSON-LD response.
      $this->addEntityField($resource, $term, $entity, $bundle, $field, $instance, $service_path);
    }
  }

  /**
   * Adds the field as a property of the entity resource.
   */
  private function addEntityField($resource, $term, $entity, $bundle, $field, $instance,
                                  $service_path, $expfield = NULL) {

    // If the entity is set to hide fields that have no values then we
    // want to honor that in the web services too.
    $hide_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id);

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

    $values = [];
    for ($i = 0; $i < count($items); $i++) {
      if (array_key_exists('value', $items[$i])) {
        $values[$i] = $this->sanitizeFieldKeys($resource, $items[$i]['value'], $bundle, $service_path);
      }
      elseif ($field['type'] == 'image') {
        $url = file_create_url($items[$i]['uri']);
        $values[$i] = $this->sanitizeFieldKeys($resource, $url, $bundle, $service_path);
      }
      else {
        // TODO: handle this case.
      }
    }

    if (isset($field['field_permissions']['type'])) {

      global $user;

      if ($field['field_permissions']['type'] === '1') {
        // Field is private.
        if (!user_access('view ' . $field_name, $user)) {
          return;
        }
      }
      if ($field['field_permissions']['type'] === '2') {
        // Field is custom permissions: Check
        // if user has access to view this field in any entity.
        if (!user_access('view ' . $field_name, $user)) {
          return;
        }
      }
    }

    if ($hide_fields == TRUE and empty($values[0])) {
      return;
    }

    // If the field cardinality is 1
    if ($field['cardinality'] == 1) {

      // If the value is an array and this is the field page then all of those
      // key/value pairs should be added directly to the response.
      if (is_array($values[0])) {
        if ($expfield) {
          foreach ($values[0] as $k => $v) {
            $resource->addProperty($k, $v);
          }
        }
        else {
          $this->addResourceProperty($resource, $term, $values[0], [
            'lowercase',
            'spacing',
          ]);
        }
      }
      // If the value is not an array it's a scalar so add it as is to the
      // response.
      else {
        $this->addResourceProperty($resource, $term, $values[0], [
          'lowercase',
          'spacing',
        ]);
      }
    }

    // If the field cardinality is > 1 or -1 (for unlimited)
    if ($field['cardinality'] != 1) {

      // If this is the expanded field page then we need to swap out
      // the resource for a collection.
      $response = new TripalWebServiceCollection($service_path . '/' . urlencode($expfield), $this->params);
      $label = tripal_get_term_details('rdfs', 'label');
      $this->addResourceProperty($response, $label, $instance['label']);
      $i = 0;
      foreach ($values as $delta => $element) {
        $member = new TripalWebServiceResource($service_path . '/' . urlencode($expfield));
        $member->setID($i);
        // Add the context of the parent resource because all of the keys
        // were santizied and set to match the proper context.
        $member->setContext($resource);
        $this->setResourceType($member, $term);
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
        //$this->resource->addProperty($key, $response);
        $this->addResourceProperty($resource, $term, $response, [
          'lowercase',
          'spacing',
        ]);
      }
    }
  }

  /**
   * Rewrites the keys of a field's items array for use with web services.
   */
  private function sanitizeFieldKeys($resource, $value, $bundle, $service_path) {

    // If the entity is set to hide fields that have no values then we
    // want to honor that in the web services too.
    $hide_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id);

    $new_value = '';
    // If the value is an array rather than a scalar then map the sub elements
    // to controlled vocabulary terms.
    if (is_array($value)) {
      $temp = [];
      foreach ($value as $k => $v) {

        // exclude fields that have no values so we can hide them
        if (!isset($v) and $hide_fields == TRUE) {
          continue;
        }

        $matches = [];
        if (preg_match('/^(.+):(.+)$/', $k, $matches)) {
          $vocabulary = $matches[1];
          $accession = $matches[2];
          $term = tripal_get_term_details($vocabulary, $accession);
          $key = $this->addContextTerm($resource, $term, [
            'lowercase',
            'spacing',
          ]);

          if (is_array($v)) {
            $temp[$key] = $this->sanitizeFieldKeys($resource, $v, $bundle, $service_path);
          }
          else {
            $temp[$key] = $v;
          }
          $term['name'] = $key;
        }
        elseif (is_numeric($k)) {
          // Key is non-vocabulary term:
          // Index number (0, 1, 2...) of a hydra:member array.
          $temp[$k] = $this->sanitizeFieldKeys($resource, $v, $bundle, $service_path);
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
      $new_value = $value;
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
          $item_entity = tripal_load_entity($item_etype, [$item_eid]);
          $item_entity = reset($item_entity);
          $bundle = tripal_load_bundle_entity(['name' => $item_entity->bundle]);
          $items['@id'] = $this->getServicePath() . '/' . urlencode($bundle->label) . '/' . $item_eid;
        }
        unset($items['entity']);
      }
    }
  }

  /**
   * A helper function to make it easy to map between keys and their fields.
   *
   * @bundle
   *   The bundle object.  Fields attached to this bundle will be included
   *   in the mapping array.
   * @return
   *   An associative arrray that maps web services keys to fields and
   *   fields to web services keys (reciprocol).
   */
  private function getFieldMapping($bundle) {
    // Iterate through the fields and create a $field_mapping array that makes
    // it easier to determine which filter criteria belongs to which field. The
    // key is the label for the field and the value is the field name. This way
    // user's can use the field label or the field name to form a query.
    $field_mapping = [];
    $fields = field_info_fields();
    foreach ($fields as $field) {
      if (array_key_exists('TripalEntity', $field['bundles'])) {
        foreach ($field['bundles']['TripalEntity'] as $bundle_name) {
          if ($bundle_name == $bundle->name) {
            $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
            if (array_key_exists('term_accession', $instance['settings'])) {
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
    return $field_mapping;
  }

  /**
   * Gets any order by statements provided by the user.
   *
   * @field_mapping
   *   An array that maps WS keys to field names. As provided by the
   *   getFieldMapping() function.
   * @return
   *   An array of fields for ordering.
   *
   * @throws Exception
   */
  private function getOrderBy($field_mapping, $bundle) {
    $order_by = [];

    // Handle order separately.
    if (array_key_exists('order', $this->params)) {
      $order_params = $this->params['order'];
      $dir = 'ASC';

      // If the user provided more than one order statement then those are
      // separated by a semicolon.
      $items = explode(';', $order_params);
      foreach ($items as $key) {

        // The user can provide a direction by separating the field key and the
        // direction with a '|' character.
        $matches = [];
        if (preg_match('/^(.*)\|(.*)$/', $key, $matches)) {
          $key = $matches[1];
          if ($matches[2] == 'ASC' or $matches[2] == 'DESC') {
            $dir = $matches[2];
          }
          else {
            throw new Exception('Please provide "ASC" or "DESC" for the ordering direction');
          }
        }

        // Break apart any subkeys and pull the first one as this is the parent
        // field.
        $subkeys = explode(',', $key);
        if (count($subkeys) > 0) {
          $key = $subkeys[0];
        }

        if (array_key_exists($key, $field_mapping)) {
          $key_field_name = $field_mapping[$key];
          $key_field = field_info_field($key_field_name);
          $key_instance = field_info_instance('TripalEntity', $key_field_name, $bundle->name);

          // Complex fields provied by the TripalField class may have sub
          // elements that support filtering.  We need to see if the user
          // wants to filter on those.
          $field_class = $key_field['type'];
          if (tripal_load_include_field_class($field_class)) {
            // To find out which fields are sortable we'll call the
            // webServicesData() function.
            $key_field = new $field_class($key_field, $key_instance);
            $ws_data = $key_field->webServicesData();
            $sortable_keys = $ws_data['sortable'];
            $criteria = implode('.', $subkeys);
            if (array_key_exists($criteria, $sortable_keys)) {
              $order_by[$key_field_name][] = [
                'column' => $sortable_keys[$criteria],
                'dir' => $dir,
              ];
            }
            else {
              throw new Exception("The value, '$criteria', is not available for sorting.");
            }
          }
          // If this field is not a TripalField then it should just have
          // a simple value and we can query for that.
          else {
            $key_field_id = $key_instance['settings']['term_vocabulary'] . ':' . $key_instance['settings']['term_accession'];

            $order_by[$key_field_name][] = [
              'column' => $key_field_id,
              'dir' => $dir,
            ];
          }

        }
        else {
          throw new Exception("The value, '$key', is not available for sorting.");
        }
      }
    }

    // If there is no ordering that is set then set a default order.
    if (count(array_keys($order_by)) == 0) {
      $key_field_names = [];
      if (in_array('data__identifier', $field_mapping)) {
        $key_field_names['data__identifier'][] = 'identifier';
      }
      else {
        if (in_array('schema__name', $field_mapping)) {
          $key_field_names['schema__name'][] = 'name';
        }
        else {
          if (in_array('rdfs_label', $field_mapping)) {
            $key_field_names['rdfs_label'][] = 'label';
          }
          else {
            if (in_array('taxrank__genus', $field_mapping)) {
              $key_field_names['taxrank__genus'][] = 'genus';
              $key_field_names['taxrank__species'][] = 'species';
            }
          }
        }
      }
      foreach ($key_field_names as $key_field_name => $criteria) {
        $key_field = field_info_field($key_field_name);
        $key_instance = field_info_instance('TripalEntity', $key_field_name, $bundle->name);
        $key_field_id = $key_instance['settings']['term_vocabulary'] . ':' . $key_instance['settings']['term_accession'];
        $field_class = $key_field['type'];
        if (tripal_load_include_field_class($field_class)) {
          // To find out which fields are sortable we'll call the
          // webServicesData() function.
          $key_field = new $field_class($key_field, $key_instance);
          $ws_data = $key_field->webServicesData();
          $sortable_keys = $ws_data['sortable'];
          if (array_key_exists($criteria, $sortable_keys)) {
            $order_by[$key_field_name][] = [
              'column' => $sortable_keys[$criteria],
              'dir' => $dir,
            ];
          }
        }
        // If this field is not a TripalField then it should just have
        // a simple value and we can query for that.
        else {
          $order_by[$key_field_name][] = [
            'column' => $key_field_id,
            'dir' => 'ASC',
          ];
        }
      }
    }

    return $order_by;
  }

  /**
   * Gets any filter by statements provided by the user.
   *
   * @field_mapping
   *   An array that maps WS keys to field names. As provided by the
   *   getFieldMapping() function.
   *
   * @return
   *   An array of fields for filtering.
   *
   * @throws Exception
   */
  private function getFieldFilters($field_mapping, $bundle) {
    $filters = [];

    // Iterate through the paramter list provided by user.
    foreach ($this->params as $param => $value) {

      // Ignore non filter parameters.
      if ($param == 'page' or $param == 'limit' or $param == 'order' or
        $param == 'ids' or $param == 'fields') {
        continue;
      }

      // Break apart any operators
      $key = $param;
      $op = '=';
      $matches = [];
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
        $field_class = $key_field['type'];
        if (tripal_load_include_field_class($field_class)) {
          // To find out which fields are searchable we'll call the wsData()
          // function.
          $key_field = new $field_class($key_field, $key_instance);
          $ws_data = $key_field->webServicesData();
          $searchable_keys = $ws_data['searchable'];
          $criteria = implode('.', $subkeys);
          if (array_key_exists($criteria, $searchable_keys)) {
            $filters[$key_field_name][] = [
              'value' => $value,
              'op' => $op,
              'column' => $searchable_keys[$criteria],
            ];
          }
          else {
            throw new Exception("The filter term, '$criteria', is not available for use.");
          }
        }
        // If this field is not a TripalField then it should just have
        // a simple value and we can query for that.
        else {
          $key_field_id = $key_instance['settings']['term_vocabulary'] . ':' . $key_instance['settings']['term_accession'];

          $filters[$key_field_name][] = [
            'value' => $value,
            'op' => $op,
            'column' => $key_field_id,
          ];
        }
      }
      else {
        throw new Exception("The filter term, '$key', is not available for use.");
      }
    }

    // Now convert the operation for each filter to one that is compatible
    // with TripalFieldQuery.
    foreach ($filters as $key_field_name => $key_filters) {
      foreach ($key_filters as $i => $filter) {
        $op = '=';
        switch ($filters[$key_field_name][$i]['op']) {
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
        $filters[$key_field_name][$i]['op'] = $op;
      }
    }
    return $filters;
  }

  /**
   * Creates a collection of resources for a given type.
   */
  private function doEntityList($ctype) {
    $service_path = $this->getServicePath() . '/' . preg_replace('/[^\w]/', '_', $ctype);
    $this->resource = new TripalWebServiceCollection($service_path, $this->params);

    // Get the TripalBundle, TripalTerm and TripalVocab type for this type.
    $bundle = tripal_load_bundle_entity(['label' => $ctype]);
    
    // Check that the user has access to this bundle.  If not then the
    // function call will throw an error.
    if (!user_access('view ' . $bundle->name)) {
      throw new Exception("Permission Denied.");
    }
        
    $term = entity_load('TripalTerm', ['id' => $bundle->term_id]);
    $term = reset($term);

    // The type of collection is provided by our API vocabulary service.
    $vocab_service = new TripalDocService_v0_1($this->base_path);
    $this->resource->addContextItem('vocab', $vocab_service->getServicePath() . '#');
    $accession = preg_replace('/[^\w]/', '_', $bundle->label . ' Collection');
    $this->resource->addContextItem($accession, 'vocab:' . $accession);
    $this->resource->setType($accession);

    // Convert term to a simple array
    $term = tripal_get_term_details($term->vocab->vocabulary, $term->accession);

    // Set the label for this collection.
    $label = tripal_get_term_details('rdfs', 'label');
    $this->addResourceProperty($this->resource, $label, $bundle->label . " Collection");

    // For quick lookup, get the mapping of WS keys to their appropriate fields.
    $field_mapping = $this->getFieldMapping($bundle);

    // Get arrays for filters and order by statements.
    $filters = $this->getFieldFilters($field_mapping, $bundle);
    $order_by = $this->getOrderBy($field_mapping, $bundle);

    // Initialize the query to search for records for our bundle types
    // that are published.
    $query = new TripalFieldQuery();
    $query->entityCondition('entity_type', 'TripalEntity');
    $query->entityCondition('bundle', $bundle->name);
    $query->propertyCondition('status', 1);

    if (array_key_exists('ids', $this->params)) {
      $eids = explode(',', $this->params['ids']);
      if (count($eids) > 1000) {
        throw new Exception('Please provide no more than 1000 ids.');
      }
      if (!is_numeric(implode('', $eids))) {
        throw new Exception('All supplied ids must be numeric.');
      }
      $query->entityCondition('entity_id', $eids, 'IN');
    }

    // Now iterate through the filters and add those.
    foreach ($filters as $key_field_name => $key_filters) {
      foreach ($key_filters as $i => $filter) {
        $column_name = $filter['column'];
        $value = $filter['value'];
        $op = $filter['op'];
        $query->fieldCondition($key_field_name, $column_name, $value, $op);
      }
    }

    // Now set the order by.
    foreach ($order_by as $key_field_name => $key_order) {
      foreach ($key_order as $i => $order) {
        $column_name = $order['column'];
        $dir = $order['dir'];
        $query->fieldOrderBy($key_field_name, $column_name, $dir);
      }
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

    // Set the query range
    $start = ($page - 1) * $limit;
    $query->range($start, $limit);

    // Now perform the query.
    $results = $query->execute();

    $this->resource->initPager($num_records, $limit, $page);

    // Check to make sure there are results.
    $entity_ids = [];
    if (isset($results['TripalEntity']) AND is_array($results['TripalEntity'])) {
      $entity_ids = $results['TripalEntity'];
    }

    // If the user wants to include any fields in the list then those provided
    // names need to be converted to fields.
    $add_fields = [];
    $add_field_ids = [];
    if (array_key_exists('fields', $this->params)) {
      $fields = explode(',', $this->params['fields']);
      foreach ($fields as $expfield) {
        list($field, $instance, $temp_term) = $this->findField($bundle, $expfield);
        if ($field) {
          $add_fields[$expfield]['field'] = $field;
          $add_fields[$expfield]['instance'] = $instance;
          $add_fields[$expfield]['term'] = $temp_term;
          $add_field_ids[] = $field['id'];
        }
        else {
          throw new Exception(t('The field named, "!field", does not exist.', ['!field' => $expfield]));
        }
      }
    }

    // Iterate through the entities and add them to the output list.
    foreach ($entity_ids as $entity_id => $stub) {
      // We don't need all of the attached fields for an entity so, we'll
      // not use the entity_load() function.  Instead just pull it from the
      // database table.
      $query = db_select('tripal_entity', 'TE');
      $query->join('tripal_term', 'TT', 'TE.term_id = TT.id');
      $query->fields('TE');
      $query->fields('TT', ['name']);
      $query->condition('TE.id', $entity_id);
      $entity = $query->execute()->fetchObject();

      $itemPage = tripal_get_term_details('schema', 'ItemPage');
      $label = tripal_get_term_details('rdfs', 'label');
      $member = new TripalWebServiceResource($service_path);
      $member->setID($entity->id);
      $this->setResourceType($member, $term);
      $this->addResourceProperty($member, $label, $entity->title);
      $this->addResourceProperty($member, $itemPage, url('/bio_data/' . $entity->id, ['absolute' => TRUE]));

      $entity = tripal_load_entity('TripalEntity', [$entity_id], FALSE, $add_field_ids);
      $entity = $entity[$entity_id];

      // Add in any requested fields
      foreach ($add_fields as $expfield => $expfield_details) {
        $this->addEntityField($member, $expfield_details['term'], $entity,
          $bundle, $expfield_details['field'], $expfield_details['instance'],
          $service_path);
      }
      $this->resource->addMember($member);
    }
  }

  /**
   * Creates a resources that contains the list of content types.
   */
  private function doContentTypesList() {
    $service_path = $this->getServicePath();
    $service_vocab = new TripalDocService_v0_1($this->base_path);
    $this->resource = new TripalWebServiceCollection($service_path, $this->params);
    $this->resource->addContextItem('vocab', $service_vocab->getServicePath());
    $this->resource->addContextItem('Content_Collection', $service_vocab->getServicePath() . '#Content_Collection');
    $this->resource->setType('Content_Collection');

    $label = tripal_get_term_details('rdfs', 'label');
    $this->addResourceProperty($this->resource, $label, 'Content Types');

    // Get the list of published terms (these are the bundle IDs)
    $bundles = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->orderBy('tb.label', 'ASC')
      ->execute();

    // Iterate through the terms and add an entry in the collection.
    $i = 0;
    while ($bundle = $bundles->fetchObject()) {
      if (!user_access('view ' . $bundle->name)) {
        // Show only content types users have access to and skip the rest.
        continue;
      }
      
      $entity = entity_load('TripalTerm', ['id' => $bundle->term_id]);
      $term = reset($entity);
      $vocab = $term->vocab;

      // Convert the term to a simple array
      $term = tripal_get_term_details($term->vocab->vocabulary, $term->accession);

      $member = new TripalWebServiceResource($service_path);
      $member->setID(preg_replace('/[^\w]/', '_', $bundle->label));

      $vocab_service = new TripalDocService_v0_1($this->base_path);
      $member->addContextItem('vocab', $vocab_service->getServicePath() . '#');
      $accession = preg_replace('/[^\w]/', '_', $bundle->label . ' Collection');
      $member->addContextItem($accession, 'vocab:' . $accession);
      $member->setType($accession);

      $this->addResourceProperty($member, $label, $bundle->label . ' Collection');
      $member->addContextItem('description', 'rdfs:comment');
      // Get the bundle description. If no description is provided then
      // use the term definition
      $description = trim(tripal_get_bundle_variable('description', $bundle->id));
      if (!$description) {
        $description = $term['definition'];
      }
      if (!$description) {
        $description = '';
      }
      $member->addProperty('description', 'A collection of ' . $bundle->label . ' resources: ' . lcfirst($description));
      $this->resource->addMember($member);

    }
  }


  /**
   * Adds the content collection class to the document for this service.
   */
  private function addDocContentCollectionClass() {
    $details = [
      'id' => 'vocab:Content_Collection',
      'term' => 'vocab:Content_Collection',
      'title' => 'Content Collection',
    ];
    $vocab = tripal_get_vocabulary_details('hydra');
    $properties = [];
    $properties[] = [
      'type' => $vocab['sw_url'],
      'title' => 'member',
      'description' => "The list of available content types.",
      "required" => NULL,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ];
    $properties[] = [
      "type" => $vocab['sw_url'],
      "title" => "totalItems",
      "description" => "The total number of content types.",
      "required" => NULL,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ];
    $properties[] = [
      "type" => $vocab['sw_url'],
      "title" => "label",
      "description" => "The type content.",
      "required" => NULL,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ];

    $operations = [];
    $operations['GET'] = [
      'label' => 'Retrieves a collection (a list) of available content types.',
      'type' => '_:content_collection_retrieve',
      'expects' => NULL,
      'returns' => 'vocab:ContentCollection',
    ];
    $this->addDocClass($details, $operations, $properties);
  }

  /**
   * Adds classes for every content type to the documentation for this service.
   */
  private function addDocBundleClasses() {

    global $user;

    // Get the list of published terms (these are the bundle IDs)
    $bundles = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->orderBy('tb.label', 'ASC')
      ->execute();

    // Iterate through the content types and add a class for each one.
    $i = 0;
    while ($bundle = $bundles->fetchObject()) {
      $entity = entity_load('TripalTerm', ['id' => $bundle->term_id]);
      $term = reset($entity);
      $vocab = $term->vocab;

      // Get the bundle description. If no description is provided then
      // use the term definition
      $description = tripal_get_bundle_variable('description', $bundle->id);
      if (!$description) {
        $description = $term->getDefinition();
      }

      // Create the details array for the class.
      $class_id = $this->getServicePath() . '/' . urlencode($bundle->label);
      $details = [
        'id' => $term->getURL(),
        'term' => $term->getAccession(),
        'title' => preg_replace('/[^\w]/', '_', $bundle->label),
        'description' => $description,
      ];

      // Add in the supported operations for this content type.
      $operations = [];
      // If the user can view this content type.
      if (user_access('view ' . $bundle->name)) {
        $label = "Retrieves the " . $bundle->label . " resource.";
        $operations['GET'] = [
          'label' => $label,
          'description' => NULL,
          'returns' => $term->url,
          'type' => '_:' . preg_replace('/[^\w]/', '_', strtolower($bundle->label)) . '_retrieve',
        ];
      }

      // If the user can edit this content type.
      if (user_access('edit ' . $bundle->name)) {
        $label = "Update and replace the " . $bundle->label . " resource.";
        if (preg_match('/^[aeiou]/i', $bundle->label)) {
          $label = "Update and replace an " . $bundle->label . " resource.";
        }
        // TODO: add this back in when web services support this method.
        //         $operations['PUT'] = array(
        //           'label' => $label,
        //           'description' => NULL,
        //           'returns' => $term->url,
        //           'type' => '_:' . preg_replace('/[^\w]/', '_', strtolower($bundle->label)) . '_update',
        //         );
      }

      // If the user can edit this content type.
      if (user_access('delete ' . $bundle->name)) {
        $label = "Deletes the " . $bundle->label . " resource.";
        if (preg_match('/^[aeiou]/i', $bundle->label)) {
          $label = "Deletes an " . $bundle->label . " resource.";
        }
        // TODO: add this back in when web services support this method.
        //         $operations['DELETE'] = array(
        //           'label' => $label,
        //           'description' => NULL,
        //           'returns' => $term->url,
        //           'type' => '_:' . preg_replace('/[^\w]/', '_', strtolower($bundle->label)) . '_delete',
        //         );
      }

      // Add in the properties that correspond to fields in the data.
      $properties = $this->addDocBundleFieldProperties($bundle, $term);

      $this->addDocClass($details, $operations, $properties);

      // Now add the bundle collection class.
      $this->addDocBundleCollectionClass($bundle, $term);

    } // end while ($bundle = $bundles->fetchObject()) { ...
  }

  /**
   * Every content type (bundle) has fields that need to be set as properties.
   */
  private function addDocBundleFieldProperties($bundle, $bundle_term) {
    $properties = [];

    $content_type_accession = $bundle_term->vocab->vocabulary . ':' . $bundle_term->accession;

    $instances = field_info_instances('TripalEntity', $bundle->name);
    foreach ($instances as $instance) {
      // Skip deleted fields.
      if ($instance['deleted']) {
        continue;
      }
      // Skip hidden fields.
      if ($instance['display']['default']['type'] == 'hidden') {
        continue;
      }

      $accession = $instance['settings']['term_vocabulary'] . ":" . $instance['settings']['term_accession'];

      $field_name = $instance['field_name'];
      $field = field_info_field($field_name);
      $field_type = $field['type'];
      // Skip fields of remote data.
      if ($field_type == 'remote__data') {
        continue;
      }

      // Check if this field is an auto attach. If not, then we have alink and
      // we need to indicate that the link has operations.
      $proptype = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];
      if ($instance['settings']['auto_attach'] == FALSE) {

        // Create a WebServiceResource for the hydra:Link type.
        $id = $content_type_accession . '/' . $accession;
        $link = new TripalWebServiceResource($this->base_path);
        $link->setID($accession);
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
        $link->addProperty('hydra:title', $instance['label']);
        $link->addProperty('hydra:description', $instance['description']);
        //       $link->addProperty('domain', $service_path . '#EntryPoint');
        //       $link->addProperty('range', $service_class::$label);

        $ops = [];
        $op = new TripalWebServiceResource($this->base_path);

        $op->setID('_:' . $field_name . '_retrieve');
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
        $op->addProperty('label', 'Retrieves the ' . $instance['label'] . ' resource.');
        $op->addProperty('description', $instance['description']);
        $op->addProperty('expects', NULL);
        $op->addProperty('returns', $accession);
        $op->addProperty('statusCodes', []);
        $ops[] = $op;
        $link->addContextItem('supportedOperation', 'hydra:supportedOperation');
        $link->addProperty('supportedOperation', $ops);
        $proptype = $link;
      }

      $formatters = tripal_get_field_field_formatters($field, $instance);

      $property = [
        'type' => $proptype,
        'title' => $instance['label'],
        'description' => $instance['description'],
        "required" => $instance['required'] ? TRUE : FALSE,
        "readonly" => FALSE,
        "writeonly" => TRUE,
        "tripal_formatters" => $formatters,
      ];
      $properties[] = $property;
    }
    return $properties;
  }

  /**
   * Every content type (bundle) needs a collection class in the documentation.
   */
  private function addDocBundleCollectionClass($bundle, $term) {

    $accession = preg_replace('/[^\w]/', '_', $bundle->label . ' Collection');

    $details = [
      'id' => 'vocab:' . $accession,
      'term' => 'vocab:' . $accession,
      'title' => $bundle->label . ' Collection',
      'subClassOf' => 'hydra:Collection',
      'description' => 'A collection (or list) of ' . $bundle->label . ' resources.',
    ];
    $vocab = tripal_get_vocabulary_details('hydra');
    $properties = [];
    $properties[] = [
      'type' => $vocab['sw_url'],
      'title' => 'member',
      'description' => "The list of available " . $bundle->label . '(s).',
      "required" => NULL,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ];
    $properties[] = [
      "type" => $vocab['sw_url'],
      "title" => "totalItems",
      "description" => "The total number of resources.",
      "required" => NULL,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ];
    $properties[] = [
      "type" => $vocab['sw_url'],
      "title" => "label",
      "description" => "A label or name for the resource.",
      "required" => NULL,
      "readonly" => FALSE,
      "writeonly" => FALSE,
    ];

    $class_id = $this->getServicePath() . '/' . urlencode($bundle->label);
    $operations = [];
    $operations['GET'] = [
      'label' => 'Retrieves a list of all ' . $bundle->label . ' resources.',
      'description' => NULL,
      'expects' => NULL,
      'returns' => $term->url,
      'type' => '_:' . preg_replace('/[^\w]/', '_', strtolower($bundle->label)) . '_collection_retrieve',
    ];

    // If the user can create this content type then we allow a POST on the
    // collection type.
    if (user_access('create ' . $bundle->name)) {
      $label = "Creates a " . $bundle->label;
      if (preg_match('/^[aeiou]/i', $bundle->label)) {
        $label = "Creates an " . $bundle->label;
      }
      // TODO: add this back in when web services support this method.
      //       $operations['POST'] = array(
      //         'label' => $label,
      //         'description' => NULL,
      //         'expects' => $term->url,
      //         'returns' => $term->url,
      //         'type' => '_:' . preg_replace('/[^\w]/', '_', strtolower($bundle->label)) . '_create',
      //         'statusCodes' => array(
      //           array(
      //             "code" => 201,
      //             "description" => "If the " . $bundle->label . " was created successfully."
      //           ),
      //         ),
      //       );
    }
    $this->addDocClass($details, $operations, $properties);
  }

}
