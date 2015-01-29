<?php 


function tripal_core_chado_hal_api() {
  global $base_url;

  // Set some initial variables.
  $response = array();
  $result = array();
  $status = 'success';
  $version = 'v0.1';
  $message = '';
  $api_url = "$base_url/ws/chado/$version";
  $page_limit = 25;
  $pager_id = 0;
  
  // Lump everything ito a try block so that if there is a problem we can
  // throw an error and have that returned in the response.
  try {
    $id = 0;
    $action = '';
    
    // GET THE BASE TABLE TO QUERY
    // The table name is always specifid as the 3rd argument in the 
    // current Drupal path.
    $table_name = arg(3);
    if (!chado_table_exists($table_name)) {
      throw new Exception("Table, '$table_name', is not a valid table.");
    }
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    
    // GET THE RECORD AND THE ACTION TO PERFORM
    // If the fourth argument is numeric then the user is requesting a
    // record from the table.  Otherwise the users is specifying an
    // action to perform on the table.
    if (is_numeric(arg(4))) {
      $id = arg(4);
      if (arg(5)) {
        $action = arg(5);
      }
    }
    else {
      $action = arg(4);
    }
    
    // Get any URL query arguments
    $query = drupal_get_query_parameters();
    
    switch ($action) {
      case 'schema':
        $result = $schema;
        break;
      case 'add':
      case 'edit':
      case 'delete':
        throw new Exception("Action, '$action', is currently not supported.");
        break;
      default: 
        // Specify the values for selecing records.
        $values = array();
        if ($id) {
          $values[$pkey] = $id;
        }

        // Specify the options for retrieving data.
        $options = array(
          'return_array' => 1,
          'pager' => array(
            'limit' => $page_limit,
            'element' => $pager_id
          ),
        );
        
        // Generate the chado variable.
        $var = chado_generate_var($table_name, $values, $options);
  
        // If we have more than one record returned then this is a collection and
        // we should create the appropriate JSON for a collection.
        if (count($var) > 1) {
          
          // Get the total number of records
          $total = chado_pager_get_count($pager_id);
          $curr_page = array_key_exists('page', $query) ? $query['page'] : 0;
          
          $first_page = '0';
          $last_page = ceil($total / $page_limit) - 1;
          $result['_links']['first'] = array('href' => "$api_url/$table_name");
          if ($curr_page > 0) {
            $prev_page = $curr_page - 1;
            if ($prev_page != $first_page) {
              $result['_links']['previous'] = array('href' => "$api_url/$table_name?page=$prev_page");
            }
            else {
              $result['_links']['previous'] = $result['_links']['first'];
            }
          }
          if ($curr_page < $last_page) { 
            $next_page = $curr_page + 1;
            $result['_links']['next'] = array('href' => "$api_url/$table_name?page=$next_page");
          }
          if ($last_page > $first_page) {
            $result['_links']['last'] = array('href' => "$api_url/$table_name?page=$last_page");
          }

          // Add the number of elements for this collection
          $result['count'] = count($var);
          $result['total'] = (integer) $total;
          $result['current_page'] = (integer) $curr_page;
          $result['items_per_page'] = $page_limit;
          
          // Do any expansion requested.
          if ($action == 'expand') {
            $var = tripal_core_chado_ws_api_expand_object($var, $query);
          }
    
          // recursively reformat the expanded objects to match HAL requirements.
          foreach ($var as $item) {
            $table_name = $item->tablename;
    
            $item = tripal_core_chado_ws_api_object_format($item, $schema, $api_url, $query);
            $result['_embedded'][$table_name][] = $item;
          }
        }
        // If we only have one record then add it as a single record to the JSON.
        else {
          $item = $var[0];
          
          // Do any expansion requested.
          if ($action == 'expand') {
            $item = tripal_core_chado_ws_api_expand_object($item, $query);
          }
    
          // recursively reformat the expanded objects to match HAL requirements.
          $item = tripal_core_chado_ws_api_object_format($item, $schema, $api_url, $query);
          $result = $item;
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
  $response['source'] = array(
      'site_name' => variable_get('site_name', "Unspecified"),
      'site_url' => $base_url,
      'site_slogan' => variable_get('site_slogan', "Unspecified"),
      'site_email' =>  variable_get('site_mail', "Unspecified"),
  );
  $response['result']  = $result;
  print drupal_json_output($response);
}

/**
 * 
 * @param unknown $object
 * @param unknown $query
 */
function tripal_core_chado_ws_api_expand_object($var, $query) {

  $page_limit = 25;
  $pager_id = 0;
  $options = array(
    'return_array' => 1,
    'pager' => array(
      'limit' => $page_limit,
      'element' => $pager_id
    ),
  );

  // If the user has requested to expand any fields then do that
  if ($query['table']) {
    $expand_tables = explode(',', $query['table']);
    foreach($expand_tables as $table) {
      // Does the table exist?
      if(!chado_table_exists($table)) {
        throw new Exception("Table, '$table', is not a valid table and thus cannot be expanded.");
      }
  
      // Expand the variable.
      $var = chado_expand_var($var, 'table', $table, $options);
  
      // if the table could not be expanded then the chado_expand_var
      // function just returns an empty record but sets the table name
      // in the object. For the JSON, we still want to create an _embedded
      // record so we have to create a stdClass object and set the
      // table name.
      if(!isset($var[0]->$table)) {
        $var[0]->$table = new stdClass();
        $var[0]->$table->tablename = $table;
      }
    }
  }
  if ($query['field']) {
    $expand_fields = explode(',', $query['field']);
    foreach($expand_fields as $field) {
      // TODO: check to make sure the field exists
      $var = chado_expand_var($var, 'field', $field);
    }
  }
  if ($query['fkey']) {
    $expand_fkeys = explode(',', $query['fkey']);
    foreach($expand_fkeys as $fkey) {
      // TODO: check to make sure the fkey exists
      $var = chado_expand_var($var, 'foreign_key', $fkey);
    }
  }
  
  return $var;
}

/**
 * 
 * @param $object
 * @param $schema
 * @param $api_url
 */
function tripal_core_chado_ws_api_object_format($object, $schema, $api_url, $query) {

  global $base_url;
  $table_name = $object->tablename;
  $pkey = $schema['primary key'][0];
  $id = $object->$pkey;

  // Add the self link first
  $object->_links = array('self' => array('href' => "$api_url/$table_name/$id"));
  
  // Add the links for the table.
  $object->_links["schema"] = array('href' => "$api_url/$table_name/schema");
  
  // Add links for editing, insert, delete but only if user has permission.
  // TODO: how do we set permissions?
  $object->_links["add"] = array('href' => "$api_url/$table_name/add");

  // Add the links if an id is available.
  if ($id) {
    $object->_links["edit"] = array('href' => "$api_url/$table_name/$id/edit");
    $object->_links["delete"] = array('href' => "$api_url/$table_name/$id/delete");
  }
    
  // Add the link to the Drupal page if a node exists.
  if (property_exists($object, 'nid')) {
    $object->_links["view"] = array('href' => $base_url . url("node/$object->nid"));
    // Unset the node ID because it's really only needed within the context
    // of the local Drupal site.
    unset($object->nid);
  }
  
  // It doesn't make sense to allow expansion of node information outside
  // of the context of the local Drupal site so remove this object.
  unset($object->expandable_nodes);
  
  // Only include links for expanding if the option to exclude them has not
  // been passed.
  if (!array_key_exists('show_expansion', $query)) {
     unset($object->expandable_fields);
     unset($object->expandable_foreign_keys);
     unset($object->expandable_tables);
  }
  
  // Deal with the expandable tables/fields/fkeys/nodes. Sometimes there are
  // specified in PHP with explicit numerical indexes and sometimes not. But,
  // the JSON converter will maintain the indexes if present which creates
  // an inconsistent look. So, we use the array_values function to just
  // get the list. 
  if (array_key_exists('expandable_tables', $object)) {
    $object->expandable_tables = array_values($object->expandable_tables);
    if (count($object->expandable_tables) > 0) {
      $object->_links["expand_table"][] = array('href' => "$api_url/$table_name/expand?table={table}[,{table}...]");
      $object->_links["expand_table"][] = array('href' => "$api_url/$table_name/$id/expand?table={table}[,{table}...]");
    }
    else {
      unset($object->expandable_tables);
    }
  }
  if (array_key_exists('expandable_fields', $object)) {
    $object->expandable_fields = array_values($object->expandable_fields);
    if (count($object->expandable_fields) > 0) {
      $object->_links["expand_field"][] = array('href' => "$api_url/$table_name/expand?field={field}[,{field}...]");
      $object->_links["expand_field"][] = array('href' => "$api_url/$table_name/$id/expand?field={field}[,{field}...]");
    }
    else {
      unset($object->expandable_fields);
    }
  }
  if (array_key_exists('expandable_foreign_keys', $object)) {
    $object->expandable_foreign_keys = array_values($object->expandable_foreign_keys);
    if (count($object->expandable_foreign_keys) > 0) {
      $object->_links["expand_fkey"][] = array('href' => "$api_url/$table_name/expand?fkey={fkey}[,{fkey}...]");
      $object->_links["expand_fkey"][] = array('href' => "$api_url/$table_name/$id/expand?fkey={fkey}[,{fkey}...]");
    }
    else {
      unset($object->expandable_foreign_keys);
    }
  }
  
  // iterate through the items in the object and see if they in turn are
  // objects.  If so, then recurse.
  foreach ($object as $key => $value) {
    
    // If any values are objects then recurse and format them correctly.
    if (is_object($value)) {

      $table_name = $value->tablename;
      $schema = chado_get_schema($table_name);
      if ($schema) {
        // Replace the object with the actual value if it exists.  If there is
        // no key value then this is probably an expanded table so just unset
        if (property_exists($value, $key)) {
          $object->$key = $value->$key;
        }
        else {
          unset($object->$key);
        }
        // Recursively format the object.
        $value = tripal_core_chado_ws_api_object_format($value, $schema, $api_url, $query);
        // Add the object as an "_embedded" object of the JSON.
        if (property_exists($object,'_embedded') and 
            array_key_exists($table_name, $object->_embedded)) {
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
      else {
        throw new Exception("Table, '$table_name', is not a valid table.");
      } 
    }
  }
  
  if (array_key_exists('no_links', $query)) {
    unset($object->_links);
  }
  
  return $object;
}