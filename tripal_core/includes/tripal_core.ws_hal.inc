<?php 


function tripal_core_chado_hal_api() {
  global $base_url;

  // Set some initial variables.
  $response = array();
  $result = array();
  $status = 'success';
  $version = '0.1';
  $message = '';
  $api_url = "$base_url/ws/chado/$version";
  
  try {
    $table_name = arg(3);
    $schema = chado_get_schema($table_name);
    if (!$schema) {
      throw new Exception("Table $table_name is not a valid table.");
    }
  
    $limit = 25;
    $pager_id = 0;
    
    $options = array(
      'return_array' => 1,
      'pager' => array(
        'limit' => $limit, 
        'element' => $pager_id
      ),
    );
    $values = array();
  
    $var = chado_generate_var($table_name, $values, $options);
    
    
    // Links come first
    $result['_links'] = array('self' => array('href' => "$api_url/$table_name"));
    
    if (count($var) > 1) {
      $total = chado_pager_get_count($pager_id);
      // Add pager links
      $result['_links']['first'] = array('href' => "$api_url/$table_name");
      $result['_links']['prev']  = array('href' => "$api_url/$table_name?page=2");
      $result['_links']['next']  = array('href' => "$api_url/$table_name?page=4");
      $result['_links']['last']  = array('href' => "$api_url/$table_name?page=12");
      
      $result['count'] = count($var);
      $result['total'] = (integer) $total;
  
      // Collection names are always plural of the table name
      foreach ($var as $item) {
        $table_name = $item->tablename;
  
        $item = tripal_core_chado_ws_api_object_format($item, $schema, $api_url);
        $result['_embedded'][$table_name][] = $item;
      }
    }
  }
  catch (Exception $e) {
    watchdog('tripal_ws', $e->getMessage(), array(), WATCHDOG_ERROR);
    $message = $e->getMessage();
    $status = 'error';
    $result = array();
  }
  
  // The responses follow the same format as the AGAVE API with a
  // status, message, version and all data in the "result" object.
  $response['status']  = $status;
  $response['message'] = $message;
  $response['version'] = $version;
  $response['result']  = $result;
  print drupal_json_output($response);
}

/**
 * 
 * @param $object
 * @param $schema
 * @param $api_url
 */
function tripal_core_chado_ws_api_object_format($object, $schema, $api_url) {
  global $base_url;
  $table_name = $object->tablename;
  $pkey = $schema['primary key'][0];
  $id = $object->$pkey;

  // Add the self link.
  $object->_links = array('self' => array('href' => "$api_url/$table_name/$id"));
  
  // Add the schema link.
  $object->_links["schema"] = array('href' => "$api_url/$table_name/schema");
  
  // Add links for editing, insert, delete but only if user has permission.
  // TODO: how do we set permissions?
  $object->_links["add"] = array('href' => "$api_url/$table_name/add");
  $object->_links["edit"] = array('href' => "$api_url/$table_name/$id/edit");
  $object->_links["delete"] = array('href' => "$api_url/$table_name/$id/delete");
  
  // Add the link to the Drupal page.
  if ($object->nid) {
    $object->_links["view"] = array('href' => $base_url . url("node/$object->nid"));
  }
  
  // Deal with the expandable tables/fields/fkeys/nodes. Sometimes there are
  // specified in PHP with explicit numerical indexes and sometimes not. But,
  // the JSON converter will maintain the indexes if present which creates
  // an inconsistent look. So, we use the array_values function to just
  // get the list. 
  $object->expandable_tables = array_values($object->expandable_tables);
  $object->expandable_fields = array_values($object->expandable_fields);
  $object->expandable_foreign_keys = array_values($object->expandable_foreign_keys);
  $object->expandable_nodes = array_values($object->expandable_nodes);

  
  // Add the links for the expandable fields, fkeys, nodes and tables
  if (count($object->expandable_fields) > 0) {
    $object->_links["expand_field"] = array('href' => "$api_url/$table_name/$id?expand_field={field}");
  }
  else {
    unset($object->expandable_fields);
  }
  if (count($object->expandable_foreign_keys) > 0) {
    $object->_links["expand_fkey"] = array('href' => "$api_url/$table_name/$id?expand_fkey={fkey}");
  }
  else {
    unset($object->expandable_foreign_keys);
  }
  if (count($object->expandable_nodes) > 0) {
    $object->_links["expand_node"] = array('href' => "$api_url/$table_name/$id?expand_node={node}");
  }
  else {
    unset($object->expandable_nodes);
  }
  if (count($object->expandable_tables) > 0) {
    $object->_links["expand_table"] = array('href' => "$api_url/$table_name/$id?expand_table={table}");
  }
  else {
    unset($object->expandable_tables);
  }
  
  // iterate through the items in the object and see if they in turn are
  // objects.  If so, then recurse.
   foreach ($object as $key => $value) {
     if (is_object($value)) {
        $table_name = $value->tablename;
        $schema = chado_get_schema($table_name);
        if ($schema) {
          // Replace the object with the actual value.
          $object->$key = $value->$key;
          // Recursively format the object.
          $value = tripal_core_chado_ws_api_object_format($value, $schema, $api_url);
          // Add the object as an "_embedded" object of the JSON.
          if (array_key_exists($table_name, $object->_embedded)) {
            // If the element is already an array then add to it, otherwise
            // convert it into an array.
            if (is_array($object->_embedded[$table_name])) {
              $object->_embedded[$table_name][] = $value;
            }
            else {
              $first = $object->_embedded[$table_name];
              $object->_embedded[$table_name] = array();
              $object->_embedded[$table_name] = $first;
              $object->_embedded[$table_name][] = $value;
            }
          }
          // This is the first time this embedded table has been seen
          // there fore, add the value as a single element.
          else {
            $object->_embedded[$table_name] = $value;
          }
        } 
     }
   }
  
  return $object;
}