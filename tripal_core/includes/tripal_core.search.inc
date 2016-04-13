<?php
/**
 * @file
 * Adds support for Drupal indexing of Chado.
 * It's important to note that not all of Chado is indexed but instead
 * Only fields indicated in hook_search_include_chado_fields().
 */

/**
 * Implements hook_search_include_chado_fields().
 *
 * This hook allows Tripal Admin/modules to specify which chado fields should be indexed
 * for searching in a simple manner.
 *
 * @return
 *   An array of chado fields you would like available for indexing. Each element should
 *   be the name of the table followed by the field and separated by a period. For example.
 *   feature.uniquename to indicate the uniquename field from the feature table.
 */
function tripal_core_search_include_chado_fields() {
  return array(
    'organism.genus',
    'organism.species',
  );
}

/**
 * Implements hook_entity_property_info_alter().
 *
 * This is where we actually add the properties to the node entity in order to indicate
 * which chado fields should be indexed.
 */
function tripal_core_entity_property_info_alter(&$info) {

  // We provide a hook to allow Tripal admin to easily add fields to the search api.
  // We want to invoke all implementations of that hook now for use below.
  $fields_to_include = module_invoke_all('search_include_chado_fields');
  $fields_to_include = array_unique($fields_to_include);

  // Retrieve information for all nodes.
  // We focus on nodes at this point because we need to link search results back to
  // the entity and we have no entites for non-node chado content in Tripal2.
  $node_info = module_invoke_all('node_info');

  foreach ($node_info as $n) {

    // Now keep in mind this hook is defined for ALL THE NODE TYPES and we only want
    // to add extra support for chado so we onle care about chado node types.
    // We can distinguish chado node types from all others by the existence of
    // the 'chado_node_api' key which is used for all sorts of beautiful tripal/chado
    // node integration (ie: adding properties, relationships and dbxrefs to node forms).
    if (isset($n['chado_node_api'])) {
      $schema = chado_get_schema($n['chado_node_api']['base_table']);

      // Now we are going to start by adding some defaults. It feels safe to say, we
      // probably want to index all the "names" so we are going to look through
      // all the fields and if they contain "name" we are going to add them automatically.
      foreach ($schema['fields'] as $field_name => $details) {

        $machine_name = $n['chado_node_api']['base_table'] . '.' . $field_name;

        // Try to create a readable label.
        $label = ucwords(str_replace(array('.','_'),' ',$machine_name));

        // We want to add all name fields and any fields previously indicated to be indexed.
        if (preg_match('/name/', $field_name) OR in_array($machine_name, $fields_to_include)) {

          if (!isset($info['node']['bundles'][ $n['base'] ]['properties'][$machine_name])) {
            $info['node']['bundles'][ $n['base'] ]['properties'][$machine_name] = array(
              'label' => $label,
              'description' => $details['description'],
              'type' => ($details['type'] == 'varchar') ? 'text' : $details['type'],
              'schema field' => '[' . $machine_name . ']',
              // The following getter callback is a generic function that can retrieve
              // values for any chado field.
              'getter callback' => 'tripal_search_chado_token_getter_callback'
            );
          }
        }
      }

      // We want to add any base foreign keys. This allows you to search for all features
      // from a given organism.
      foreach ($schema['foreign keys'] as $table => $fk_details) {
        foreach ($fk_details['columns'] as $left_field => $right_field) {

          $machine_name = $n['chado_node_api']['base_table'] . '.' . $left_field;
          $field_details = $schema['fields'][$left_field];

          // Try to create a readable label.
          $label = $table . ' (' . $machine_name . ')';
          if (preg_match('/(\w+)_id/',$left_field,$matches)) {
            $label = str_replace('_', ' ', $n['chado_node_api']['base_table']);
            $label .= ' ' . str_replace('_', ' ', $matches[1]);
            $label = ucwords($label);
          }

          $pretoken = '[' . $n['chado_node_api']['base_table'] . '.' . $left_field . '>' . $table . '.' . $right_field . ']';
          $format = chado_node_get_readable_format($pretoken);
          if (!$format) $format = chado_node_get_unique_constraint_format($table);

          $info['node']['bundles'][ $n['base'] ]['properties'][$machine_name] = array(
            'label' => $label,
            'description' => $field_details['description'],
            'type' => 'text',
            'schema field' => $format,
            // The following getter callback is a generic function that can retrieve
            // values for any chado foreign key.
            'getter callback' => 'tripal_search_chado_token_getter_callback'
          );
        }
      }
    }
  }

  // Provide our own hook for altering properties to make it easier for our users.
  drupal_alter('tripal_search_properties', $info);
}

/**
 * Allows tripal admin to alter entity property information after it has. This is currently
 * being used to indicate chado fields to be indexed for search.
 *
 * NOTE: If you simply need to add a field to be indexed, use hook_search_include_chado_fields()
 * which provides the much easier method of simply listing fields to include.
 *
 * This function is most useful if you want to change the way the value is retrieved
 * (done by changing the 'getter callback') or add your own custom computed field.
 */
function hook_tripal_search_properties_alter(&$info) { }

/**
 * Implements a getter callback for chado token formats.
 *
 * A chado token format is a string containing chado tokens.
 *
 * Chado tokens are expected to follow the format of tokens auto-generated using
 *   chado_node_generate_tokens(). For example, [feature.uniquename] indicates you
 *   should return the uniquename of a feature node and [feature.organism_id>organism.species]
 *   indicates you should return the organism genus of the feature node.
 *
 * The chado token format must be stored in the 'schema field' when defining the property in
 *  hook_entity_property_info() in order for this getter to work.
 *
 * @param $data
 *   Must be a chado node object.
 */
function tripal_search_chado_token_getter_callback($data, $options, $field_name, $type, $info) {

  if (isset($info['schema field'])) {
    $format = $info['schema field'];

    // Determine which tokens were used in the format string
    if (preg_match_all('/\[[^]]+\]/', $format, $used_tokens)) {
      $used_tokens = $used_tokens[0];

      // If there are no tokens then return the format as is...
      if (empty($used_tokens)) {
        tripal_report_error(
          'tripal_search',
          TRIPAL_NOTICE,
          'Returned static text for :field since there were no tokens in the supplied format: :format',
          array(':field' => $field_name, ':format' => $format)
        );
        return $format;
      }

      // Determine our base table so we know if this is even the right node type.
      if (preg_match('/^\[(\w+)\.(\w+)/',$used_tokens[0], $matches)) {
        $base_table = $matches[1];
        $field_name = $matches[2];

        // For some weird reason nodes of all types are trying to get a value for fields
        // that we defined as specific to a given node type (ie: bundle). As such we need
        // this check here to ensure this field is actually for this node type.
        if (!isset($data->{$base_table})) return NULL;

        // Get the value of each token.
        $null_tokens = array();
        foreach ($used_tokens as $token) {
          $token_info = array(
              'name' => $info['label'],
              'table' => $base_table,
              'field' => $field_name,
              'token' => $token,
              'description' => $info['description'],
              'location' => chado_node_get_location_from_token($token),
          );

          $value = chado_get_token_value($token_info, $data);
          if (empty($value)) $null_tokens[] = $token;

          // And sub it in to the format.
          $format = str_replace($token, $value, $format);
        }

        // If none of the tokens had values then this node doesn't have this field.
        // As such we return null so the search api doesn't bother indexing an empty format.
        if (sizeof($used_tokens) == sizeof($null_tokens)) return NULL;

        // At this poin the format should have all tokens replaced for values and is
        // thus the value we want to index for this field!
        return $format;
      }
      else {
        // Not able to determine table?
        tripal_report_error(
          'tripal_search',
          TRIPAL_ERROR,
          'Unable to extract the base table from the first token (:token) for :field because it didn\'t match the expected format: [tablename.field...',
          array(':field' => $field_name, ':token' => $used_tokens[0])
        );
        return NULL;
      }
    }
    // There were no tokens?
    else {
      tripal_report_error(
        'tripal_search',
        TRIPAL_NOTICE,
        'Returned static text for :field since there were no tokens of a recognized format in the supplied format: :format',
        array(':field' => $field_name, ':format' => $format)
      );
      return $format;
    }
  }
  else {
    tripal_report_error(
      'tripal_search',
      TRIPAL_ERROR,
      'Unable to get value for :field because the schema field was not set.',
      array(':field' => $field_name)
    );
    return NULL;
  }
}

/**
 * Implements hook_modules_enabled().
 *
 * This hook is called when ANY module is enabled. This allows us to update the
 * the search api "Default node index" when any Tripal module is enabled thus allowing us
 * to catch new node types right after they're created.
 */
function tripal_core_modules_enabled($modules) {
  tripal_search_update_default_index();
}

/**
 * The Search API provides a default node index which has a number of
 * node-specific fields enabled by default. We want to ensure our
 * chado fields are also enabled by default thus making for easier
 * enabling of Tripal search.
 *
 * This function should be called whenever new nodes might have been
 * added to ensure that their fields are added as well.
 *
 * We should only modify the default node index if it has no database service yet.
 * That way we ensure we don't override user changes!
 */
function tripal_search_update_default_index() {

  // First we need the index object for the "Default node index".
  $index_id = db_query('SELECT id FROM search_api_index WHERE machine_name=:name',
    array(':name' => 'default_node_index'))->fetchField();
  if (!$index_id) {
    // ERROR
    return FALSE;
  }
  $index = search_api_index_load($index_id);

  // Collect all the fields already added to the search index.
  $changes = array('options' => $index->options);

  // Now we only want to update the index if it's both enabled and has no server indicated.
  // That way we can be reasonably sure that it was been untouched by admin users.
  if ($index->enabled == FALSE AND $index->server == NULL) {

    // We need information about all the fields available to nodes before we can
    // go crazy enabling them... That information is stored as properties of nodes
    // so we'll grab that.
    $info = entity_get_property_info('node');

    // Now we want to loop through each node type and add all the properties for the
    // chado node types.
    // Assumption #1: We are assuming that all chado node types are prefixed 'chado_'.
    foreach ($info['bundles'] as $node_type => $details) {
      if (preg_match('/^chado_/', $node_type)) {

        // Now add each chado fields to the index but only if they are not already added.
        foreach ($details['properties'] as $field_name => $field_details) {
          if (!isset($changes['options']['fields'][$field_name])) {
            $changes['options']['fields'][$field_name]['type'] = ($field_details['type'] == 'varchar') ? 'text' : $field_details['type'];

            // Furthermore if this is a name then we want to add a boost to ensure it carries
            // more weight in the search results.
            if (preg_match('/name/',$field_name)) {
              $changes['options']['fields'][$field_name]['boost'] = '3.0';
            }
          }
        }

      }
    }

    // We also want to enable highlighting to ensure an excerpt is generated since this
    // will be used in the default search view distributed with Tripal.
    if (!isset($index->options['processors']['search_api_highlighting'])) {
      $changes['options']['processors']['search_api_highlighting'] = array(
        'status' => 1,
        'weight' => 35,
        'settings' => array(
          'prefix' => '<strong>',
          'suffix' => '</strong>',
          'excerpt' => 1,
          'excerpt_length' => 256,
          'exclude_fields' => array(),
          'highlight' => 'always',
        ),
      );
    }
    else {
      $changes['options']['processors']['search_api_highlighting']['status'] = 1;
      $changes['options']['processors']['search_api_highlighting']['settings']['excerpt'] = 1;
    }

    // Finally we save all of our changes :-).
    search_api_index_edit($index_id, $changes);
    drupal_set_message('The Search API "Default Node Index" was updated.');
  }
  else {
    tripal_report_error(
      'tripal_search',
      TRIPAL_NOTICE,
      'The Search API "Default Node Index" was not updated with Tripal Fields. If you would like to enable more Tripal/Chado fields to be indexed, edit the Field Listing for the "Default Node Index" now.'
    );
  }
}

