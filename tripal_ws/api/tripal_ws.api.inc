<?php
/**
 * @file
 *
 * This file provides the Tripal Web Services API: a set of functions for
 * interacting with the Tripal Web Services.
 */

/**
 * @defgroup tripal_ws_api Web Services
 *
 * @ingroup tripal_api
 * {@
 * The Tripal Web Services API provides a set of functions for interacting
 * with the Tripal Web Services.
 * @}
 */

/**
 * Adjust the values of a field for display in web services.
 *
 * This hook should be used sparingly. It is meant primarily to adjust 3rd
 * Party (non Tripal) fields so that they work with web
 * services.  The caller should adjust the $items array as needed.
 * This change only affects the value displayed in web services.  Web services
 * expect that every field have a 'value' element for each of the items. If a
 * field for some reason does not have a 'value' element then this hook will
 * allow setting of that element.
 *
 * @param $items
 *   The list of items for the field.
 * @param $field
 *   The field array.
 * @param $instance
 *   The field instance array.
 *
 * @ingroup tripal_ws_api
 */

function hook_tripal_ws_value(&$items, $field, $instance) {
  // The image module doesn't properly set the 'value' field, so we'll do it
  // here.
  if ($field['type'] == 'image' and $field['module'] == 'image') {
    foreach ($items as $delta => $details) {
      if ($items[$delta] and array_key_exists('uri', $items[$delta])) {
        $items[$delta]['value']['schema:url'] = file_create_url($items[$delta]['uri']);
      }
    }
  }
}

/**
 * Retrieves a list of TripalWebService implementations.
 *
 * The TripalWebService classes can be added by a site developer that wishes
 * to create a new Tripal compatible web serivce.  The class file should
 * be placed in the [module]/includes/TripalWebService directory.  Tripal will
 * support any service as long as it is in this directory and extends the
 * TripalWebService class.
 *
 * @return
 *   A list of TripalWebService names.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_web_services() {
  $services = [];

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    // Find all of the files in the tripal_chado/includes/fields directory.
    $service_path = drupal_get_path('module', $module) . '/includes/TripalWebService';
    $service_files = file_scan_directory($service_path, '/.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($service_files as $file) {
      $class = $file->name;
      module_load_include('inc', $module, 'includes/TripalWebService/' . $class);
      if (class_exists($class) and is_subclass_of($class, 'TripalWebService')) {
        $services[] = $class;
      }
    }
  }
  return $services;
}

/**
 * Loads the TripalWebService class file into scope.
 *
 * @param $class
 *   The TripalWebService class to include.
 *
 * @return
 *   TRUE if the field type class file was found, FALSE otherwise.
 *
 * @ingroup tripal_ws_api
 */
function tripal_load_include_web_service_class($class) {
  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    module_load_include('inc', $module, 'includes/TripalWebService/' . $class);
    if (class_exists($class)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Adds a new site to the web services table.
 *
 * @param $name
 *   Name of site to be included.
 * @param $url
 *   URL of site to be added.
 * @param $version
 *   Version of the API being used. default to 1
 * @param $description
 *    A description of the site and any additional info that
 *    would be helpful for admins.
 *
 * @return
 *   TRUE if the site is successfully added, FALSE otherwise.
 *
 * @ingroup tripal_ws_api
 */
function tripal_add_site($name, $url, $version, $description) {
  $check_url = NULL;
  $check_name = NULL;
  $write_to_db = TRUE;
  // When inserting a record.
  $check_url =
    db_select('tripal_sites', 'ts')
      ->fields('ts', ['id'])
      ->condition('url', $url)
      ->condition('version', $version)
      ->execute()
      ->fetchField();

  $check_name =
    db_select('tripal_sites', 'ts')
      ->fields('ts', ['id'])
      ->condition('name', $name)
      ->execute()
      ->fetchField();

  if ($check_url) {
    drupal_set_message(t('The URL and version is used by another site.'), 'error');
    $write_to_db = FALSE;
  }

  if ($check_name) {
    drupal_set_message(t('The name is used by another site.'), 'error');
    $write_to_db = FALSE;
  }
  if ($write_to_db === TRUE) {
    db_insert('tripal_sites')
      ->fields([
        'name' => $name,
        'url' => $url,
        'version' => $version,
        'description' => $description,
      ])
      ->execute();
    drupal_set_message(t('Tripal site \'' . $name . '\' has been added.'));
    return $write_to_db;
  }

  return $write_to_db;
}

/**
 * Remove a site from the web services table.
 *
 * @param $record_id
 *   ID of the record to be deleted.
 *
 * @return
 *   TRUE if the record was successfully deleted, FALSE otherwise.
 *
 * @ingroup tripal_ws_api
 */
function tripal_remove_site($record_id) {
  if ($record_id) {
    db_delete('tripal_sites')
      ->condition('id', $record_id)
      ->execute();
    drupal_set_message('The Tripal site \'' . $record_id . '\' has been removed.');
    return TRUE;
  }
  return FALSE;
}

/**
 * Constructs a URL for a remote Tripal web service.
 *
 * @param $remote_site
 *   A remote Tripal site object.
 * @param $path
 *   The web service path for the content (excluding
 *   'web-services/vX.x/content').  To retrieve the full content listing
 *   leave this paramter empty.
 * @param $query
 *   An query string to appear after the ? in a URL.
 *
 * @return
 *   The full URL within the content service.
 */
function tripal_build_remote_content_url($remote_site, $path = '', $query = '') {
  // Build the URL to the remote web services.
  $ws_version = $remote_site->version;
  $ws_url = $remote_site->url;
  $ws_url = trim($ws_url, '/');
  $ws_url .= '/web-services/content/' . $ws_version . '/' . $path;

  // Build the Query and make and substitions needed.
  if ($query) {
    $ws_url = $ws_url . '?' . $query;
  }

  return $ws_url;
}

/**
 * Makes a request to the "content" service of a remote Tripal web site.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $path
 *   The web service path for the content (excluding
 *   'web-services/vX.x/content').  To retrieve the full content listing
 *   leave this paramter empty.
 * @param $query
 *   An query string to appear after the ? in a URL.
 *
 * @return
 *   The JSON response formatted in a PHP array or FALSE if a problem occured.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_content($site_id, $path = '', $query = '') {

  if (!$site_id) {
    throw new Exception('Please provide a numeric site ID for the tripal_get_remote_content function.');
  }

  // Fetch the record for this site id.
  $remote_site = db_select('tripal_sites', 'ts')
    ->fields('ts')
    ->condition('ts.id', $site_id)
    ->execute()
    ->fetchObject();

  if (!$remote_site) {
    $data = [
      'error' => t('Could not find a remote tripal site using the id provided: !id.',
        ['!id' => $site_id]),
    ];
    _tripal_report_ws_error($data);
    return $data;
  }

  // Make the remote query.
  $ws_url = tripal_build_remote_content_url($remote_site, $path, $query);
  $data = drupal_http_request($ws_url);
  if (!$data) {
    $data = [
      'error' => t('Could not connect to the remote web service using the url: !url',
        ['!url' => $ws_url]),
    ];
    _tripal_report_ws_error($data);
    return $data;
  }

  // If the data object has an error then this is some sort of
  // connection error (not a Tripal web services error).
  if (property_exists($data, 'error')) {
    $data = [
      'error' => $data->error,
    ];
    _tripal_report_ws_error($data);
    return $data;
  }

  // We got a response, so convert it to a PHP array.
  $data = drupal_json_decode($data->data);

  // Check if there was a Tripal Web Services error.
  if (array_key_exists('error', $data)) {
    _tripal_report_ws_error($data);
  }

  return $data;
}

/**
 * A helper function for reporting an error when retrieving remote content.
 *
 * @param $data
 *   A data array containing at a minimum the 'error' key containing the
 *   error message.
 */
function _tripal_report_ws_error($data) {
  $error = '</pre>' . print_r($data['error'], TRUE) . '</pre>';
  tripal_report_error('tripal_ws', TRIPAL_ERROR,
    'Tripal remote web services reports the following error: !error.',
    ['!error' => $error]);
}

/**
 * Retrieves the JSON-LD context for any remote Tripal web service.
 *
 * @param $context_url
 *   The Full URL for the context file on the remote Tripal site. This URL
 *   can be found in the '@context' key of any response from a remote Tripal
 *   web services call.
 * @param $cache_id
 *   A unique ID for caching of this context result to speed furture
 *   queries.
 *
 * @return
 *   The JSON-LD context mapping array, or FALSE if the context could not
 *   be retrieved.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_context($context_url, $cache_id) {

  if (!$context_url) {
    throw new Exception('PLease provide a context_url for the tripal_get_remote_context function.');
  }
  if (!$cache_id) {
    throw new Exception('PLease provide unique $cache_id for the tripal_get_remote_context function.');
  }

  if ($cache = cache_get($cache_id)) {
    return $cache->data;
  }

  $context = drupal_http_request($context_url);
  if (!$context) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
      'There was a poblem retrieving the context from the remote site: !context_url.',
      ['!context_url' => $context_url]);
    return FALSE;
  }
  $context = drupal_json_decode($context->data);
  $context = $context['@context'];
  cache_set($cache_id, $context);
  return $context;
}

/**
 * Retrieves the JSON-LD context for a bundle or field on a remote Tripal site.
 *
 * The $site_id, $bundle_accession and $field_accession variables are not
 * needed to retrieve the context, but are used for caching the context to
 * make subsequent calls execute faster.  This function is meant to be used
 * only for the 'content' service provided by Tripal.
 *
 * @param $site_id
 *    The numeric site ID for the remote Tripal site.
 * @param $context_url
 *   The Full URL for the context file on the remote Tripal site. This URL
 *   can be found in the '@context' key of any response from a remote Tripal
 *   web services call.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_accession
 *   The controlled vocabulary term accession for the property (i.e. field) of
 *   the Class (i.e. content type).
 *
 * @return
 *   The JSON-LD context mapping array.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_content_context($site_id, $context_url, $bundle_accession, $field_accession = '') {
  $cache_id = substr('trp_ws_context_' . $site_id . '-' . $bundle_accession . '-' . $field_accession, 0, 254);
  $context = tripal_get_remote_context($context_url, $cache_id);
  return $context;
}

/**
 * Clears the cached remote site documentation and context.
 *
 * When querying a remote website, the site's API documenation and page context
 * is cached to make re-use of that information easier in the future. This
 * function can be used to clear those caches.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 *
 * @ingroup tripal_ws_api
 */
function tripal_clear_remote_cache($site_id) {
  if (!$site_id) {
    throw new Exception('Please provide a numeric site ID for the tripal_clear_remote_cache function.');
  }
  cache_clear_all('trp_ws_context_' . $site_id, 'cache', TRUE);
  cache_clear_all('trp_ws_doc_' . $site_id, 'cache', TRUE);
}

/**
 * Retrieves the API documentation for a remote Tripal web service.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 *
 * @return
 *   The vocabulary of a remote Tripal web service, or FALSE if an error
 *   occured.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_API_doc($site_id) {
  $site_doc = '';

  if (!$site_id) {
    throw new Exception('Please provide a numeric site ID for the tripal_get_remote_API_doc function.');
  }

  $cache_name = 'trp_ws_doc_' . $site_id;
  if ($cache = cache_get($cache_name)) {
    return $cache->data;
  }

  // Get the site url from the tripal_sites table.
  $remote_site = db_select('tripal_sites', 'ts')
    ->fields('ts')
    ->condition('ts.id', $site_id)
    ->execute()
    ->fetchObject();

  if (!$remote_site) {
    throw new Exception(t('Cannot find a remote site with id: "!id"', ['!id' => $site_id]));
  }

  // Build the URL to the remote web services.
  $ws_version = $remote_site->version;
  $ws_url = $remote_site->url;
  $ws_url = trim($ws_url, '/');
  $ws_url .= '/web-services/doc/' . $ws_version;

  // Build and make the request.
  $options = [];
  $data = drupal_http_request($ws_url, $options);

  if (!$data) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
      t('Could not connect to the remote web service.'));
    return FALSE;
  }

  // If the data object has an error then this is some sort of
  // connection error (not a Tripal web services error).
  if (property_exists($data, 'error')) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
      'Remote web services document reports the following error: !error. Using URL: !url',
      ['!error' => $error, '!url' => $ws_url]);
    return FALSE;
  }

  // We got a response, so convert it to a PHP array.
  $site_doc = drupal_json_decode($data->data);

  // Check if there was a Tripal Web Services error.
  if (array_key_exists('error', $data)) {
    $error = '</pre>' . print_r($data['error'], TRUE) . '</pre>';
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
      'Tripal Remote web services document reports the following error: !error. Using URL: !url',
      ['!error' => $error, '!url' => $ws_url]);
    return FALSE;
  }

  cache_set($cache_name, $site_doc);

  return $site_doc;
}

/**
 * Queries a remote site for an array of bulk entity ids.
 *
 * This function returns an array of "fake" entities containing values
 * for fields specified.
 *
 * @param $remote_entity_ids
 *   Array of the remote ids.
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_ids
 *   The controlled vocabulary term accessions for the fields available
 *   on the remote content type.  Any remote fields that matches these IDs will
 *   be added to the entity returned.
 *
 * @return
 *    An array of fake entity objects where the key is the entity_id and
 *    the value is the object.
 *
 * @ingroup tripal_ws_api
 */
function tripal_load_remote_entities($remote_entity_ids, $site_id, $bundle_accession, $field_ids) {

  if (!$remote_entity_ids) {
    throw new Exception('Please provide the list of remote entity ids for the tripal_load_remote_entities function.');
  }
  if (!is_array($remote_entity_ids)) {
    throw new Exception('Please provide an array for the remote entity ids for the tripal_load_remote_entities function.');
  }
  if (!$site_id) {
    throw new Exception('Please provide a numeric site ID for the tripal_load_remote_entities function.');
  }
  if (!$bundle_accession) {
    throw new Exception('Please provide the bundle accession for the tripal_load_remote_entities function.');
  }
  if (!$field_ids) {
    throw new Exception('Please provide the list of field IDs for the tripal_load_remote_entities function.');
  }
  if (!is_array($field_ids)) {
    throw new Exception('Please provide an array for the field IDs for the tripal_load_remote_entities function.');
  }

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);

  // Generate an array for the query and then execute it.
  $query = 'page=1&limit=' . count($remote_entity_ids) .
    '&ids=' . urlencode(implode(",", $remote_entity_ids)) .
    '&fields=' . urlencode(implode(",", $field_ids));

  $results = tripal_get_remote_content($site_id, $bundle_accession, $query);

  // If we encountered an error just return;
  if (array_key_exists('error', $results)) {
    return FALSE;
  }

  // Get the context JSON for this remote entity, we'll use it to map
  $context_url = $results['@context'];
  $context = tripal_get_remote_content_context($site_id, $context_url, $bundle_accession);
  if (!$context) {
    return $entity;
  }

  $total_items = $results['totalItems'];
  $members = $results['member'];

  $entities = [];
  foreach ($members as $member) {
    // Start building the fake entity.
    $entity_id = preg_replace('/^.*?(\d+)$/', '$1', $member['@id']);
    $entity = new stdClass();
    $entity->entityType = 'TripalEntity';
    $entity->entityInfo = [];
    $entity->id = $entity_id;
    $entity->type = 'TripalEntity';
    $entity->bundle = $bundle_accession;
    $entity->site_id = $site_id;

    $member = _tripal_update_remote_entity_field($member, $context, 1);
    foreach ($member as $field_id => $value) {
      $field = tripal_get_remote_field_info($site_id, $bundle_accession, $field_id);
      $instance = tripal_get_remote_field_instance_info($site_id, $bundle_accession, $field_id);
      $field_name = $field['field_name'];
      $entity->{$field_name}['und'][0]['value'] = $value;
    }

    $entities[$entity_id] = $entity;
  }
  return $entities;
}

/**
 * Queries a remote site for an entity.
 *
 * This function returns a "fake" entity containing values for all fields
 * specified.
 *
 * @param $remote_entity_id
 *   A remote entity ID.
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_ids
 *   The controlled vocabulary term accessions for the fields available
 *   on the remote content type.  Any remote fields that matches these IDs will
 *   be added to the entity returned.
 *
 * @return
 *    A fake entity object.
 *
 * @ingroup tripal_ws_api
 */
function tripal_load_remote_entity($remote_entity_id, $site_id, $bundle_accession, $field_ids) {

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);

  // Get the remote entity and create the fake entity.
  $remote_entity = tripal_get_remote_content($site_id, $bundle_accession . '/' . $remote_entity_id);

  // If we encountered an error just return;
  if (array_key_exists('error', $results)) {
    return FALSE;
  }

  // Start building the fake entity.
  $entity = new stdClass();
  $entity->entityType = 'TripalEntity';
  $entity->entityInfo = [];
  $entity->id = $remote_entity_id;
  $entity->type = 'TripalEntity';
  $entity->bundle = $bundle_accession;
  $entity->site_id = $site_id;

  // Get the context JSON for this remote entity, we'll use it to map
  $context_url = $remote_entity['@context'];
  $context = tripal_get_remote_content_context($site_id, $context_url, $bundle_accession);
  if (!$context) {
    return $entity;
  }

  // Iterate through the fields and the those values to the entity.
  foreach ($field_ids as $field_id) {

    $field = tripal_get_remote_field_info($site_id, $bundle_accession, $field_id);
    $instance = tripal_get_remote_field_instance_info($site_id, $bundle_accession, $field_id);
    $field_name = $field['field_name'];

    $field_key = '';
    foreach ($context as $k => $v) {
      if (!is_array($v) and $v == $field_id) {
        $field_key = $k;
      }
    }

    // If the field is not in this remote bundle then add an empty value.
    if (!$field_key) {
      $entity->{$field_name}['und'][0]['value'] = '';
      continue;
    }
    if (!array_key_exists($field_key, $remote_entity)) {
      $entity->{$field_name}['und'][0]['value'] = '';
      continue;
    }

    // If the key is for a field that is not "auto attached' then we need
    // to get that field through a separate call.
    $attached = TRUE;
    if (array_key_exists($field_id, $context) and is_array($context[$field_id]) and
      array_key_exists('@type', $context[$field_id]) and $context[$field_id]['@type'] == '@id') {
      $attached = FALSE;
    }

    // Set the value for this field.
    $value = '';
    if (is_array($remote_entity[$field_key])) {
      $value = _tripal_update_remote_entity_field($remote_entity[$field_key], $context, 1);
    }
    else {
      $value = $remote_entity[$field_key];
    }

    // If the field is not attached then we have to query another level.
    if (!$attached) {

      $field_data = drupal_http_request($value);
      if (!$field_data) {
        tripal_report_error('tripal_ws', TRIPAL_ERROR,
          'There was a poblem retrieving the unattached field, "!field:", for the remote entity: !entity_id.',
          ['!field' => $field_id, '!entity_id' => $remote_entity_id]);
        $value = '';
      }
      $field_data = drupal_json_decode($field_data->data);

      // Get the context for this field so we can map the keys to the
      // controlled vocabulary accessions. If it fails then skip this field.
      $field_context_url = $field_data['@context'];
      $field_context = tripal_get_remote_content_context($site_id, $field_context_url, $bundle_accession, $field_id);
      if (!$field_context) {
        continue;
      }
      $value = _tripal_update_remote_entity_field($field_data, $field_context);
    }
    $entity->{$field_name}['und'][0]['value'] = $value;
  }
  return $entity;
}

/**
 * A helper function for the tripal_get_remote_entity() function.
 *
 * This function converts the field's key elements to their
 * vocabulary term accessions.
 *
 * @param $field_data
 *   The field array as returned by web services.
 * @param $context
 *   The web service JSON-LD context for the bundle to which the field belongs.
 *
 * @ingroup tripal_ws_api
 */
function _tripal_update_remote_entity_field($field_data, $context, $depth = 0) {


  // Check if this is an array.
  if ($field_data['@type'] == 'Collection') {
    $members = [];
    foreach ($field_data['member'] as $member) {
      $next_depth = $depth + 1;
      $members[] = _tripal_update_remote_entity_field($member, $context, $next_depth);
    }

    // If we only have one item then just return it as a single item.
    // TODO: we may need to check cardinality of the field and be more
    // strict about how we return the value.
    if ($field_data['totalItems'] == 1) {
      return $members[0];
    }
    else {
      return $members;
    }
  }

  $value = [];
  foreach ($field_data as $k => $v) {
    // Skip the JSON-LD keys.
    if ($k == '@id' or $k == '@type' or $k == '@context') {
      continue;
    }
    // Find the term accession for this element, and if the key's value is an
    // array then recurse.
    $accession = $context[$k];
    if (is_array($v)) {
      $next_depth = $depth + 1;
      $subvalue = _tripal_update_remote_entity_field($v, $context, $next_depth);
      $value[$accession] = $subvalue;
    }
    else {
      $value[$accession] = $v;
    }
  }
  return $value;

}

/**
 * Behaves similar to the field_info_field() function but for remote fields.
 *
 * Returns a "fake" field info array for fields attached to content types
 * on remote Tripal sites.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_accession
 *   The controlled vocabulary term accession for the property (i.e. field) of
 *   the Class (i.e. content type).
 *
 * @return
 *   An array similar to that returned by the field_info_field function
 *   of Drupal for local fields.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_field_info($site_id, $bundle_accession, $field_accession) {

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);

  // Get the property from the document for this field.
  $property = tripal_get_remote_field_doc($site_id, $bundle_accession, $field_accession);

  // Now create the fake field and instance.
  list($vocab, $accession) = explode(':', $field_accession);
  $field_name = 'tripal_remote_site_' . $site_id . '_' . $field_accession;

  $field = [
    'field_name' => $field_name,
    'type' => $field_name,
    'storage' => [
      'type' => 'tripal_remote_site',
    ],
  ];
  return $field;
}

/**
 * Behaves similar to the field_info_instance() function but for remote fields.
 *
 * Returns a "fake" instance info array for fields attached to content types
 * on remote Tripal sites.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_accession
 *   The controlled vocabulary term accession for the property (i.e. field) of
 *   the Class (i.e. content type).
 *
 * @return
 *   An array similar to that returned by the field_info_instance function
 *   of Drupal for local fields.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_field_instance_info($site_id, $bundle_accession, $field_accession) {

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);

  // Get the property from the document for this field.
  $property = tripal_get_remote_field_doc($site_id, $bundle_accession, $field_accession);

  list($vocab, $accession) = explode(':', $field_accession);
  $field_name = 'tripal_remote_site_' . $site_id . '_' . $field_accession;

  list($vocab, $accession) = explode(':', $field_accession);
  $instance = [
    'label' => $property['hydra:title'],
    'description' => $property['hydra:description'],
    'formatters' => $property['tripal_formatters'],
    'settings' => [
      'term_vocabulary' => $vocab,
      'term_accession' => $accession,
    ],
    'field_name' => $field_name,
    'entity_type' => 'TripalEntity',
    'bundle_name' => $bundle_accession,
  ];
  return $instance;
}


/**
 * Retreive the content type information from a remote Tripal site.
 *
 * The array returned is equivalent to the Hydra Vocabulary "supportedClass"
 * stanza.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 *
 * @return
 *   A PHP array corresponding to the Hydra Class stanza (i.e. a content type).
 *   Returns NULL if the class ID cannot be found.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_content_doc($site_id, $bundle_accession) {

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);

  // Get the class that matches this bundle.
  $classes = $site_doc['supportedClass'];
  $class = NULL;
  foreach ($classes as $item) {
    if ($item['@id'] == $bundle_accession) {
      return $item;
    }
  }
  return NULL;
}

/**
 * Retrieves the field information for a content type from a remote Tripal site.
 *
 * The array returned is equivalent to the Hydra Vocabulary "supportedProperty"
 * stanza that belongs to a Hydra Class (content type).
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_accession
 *   The controlled vocabulary term accession for the property (i.e. field) of
 *   the Class (i.e. content type).
 *
 * @return
 *   A PHP array corresponding to the Hydra property stanza (field) that
 *   belongs to the given Class (i.e. a content type).  Retruns NULL if the
 *   property cannot be found.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_field_doc($site_id, $bundle_accession, $field_accession) {

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);

  $class = tripal_get_remote_content_doc($site_id, $bundle_accession);
  $properties = $class['supportedProperty'];
  foreach ($properties as $item) {
    if ($item['property'] == $field_accession) {
      return $item;
    }
  }
  return NULL;
}


/**
 * Retrieves the list of download formatters for a remote field.
 *
 * All Tripal fields support the abilty for inclusion in files that can
 * downloaded.  This function is used to identify what formatters these
 * fields are appropriate for. If those download formatter classes exist
 * on this site then the field can be used with that formatter.
 *
 * @param $site_id
 *   The numeric site ID for the remote Tripal site.
 * @param $bundle_accession
 *   The controlled vocabulary term accession for the content type
 *   on the remote Tripal site.
 * @param $field_accession
 *   The controlled vocabulary term accession for the property (i.e. field) of
 *   the Class (i.e. content type).
 *
 * @return
 *   An array of field downloader class names that are compoatible with the
 *   field and which exist on this site.
 *
 * @ingroup tripal_ws_api
 */
function tripal_get_remote_field_formatters($site_id, $bundle_accession, $field_accession) {

  $flist = [];

  // Get the site documentation (loads from cache if already retrieved).
  $site_doc = tripal_get_remote_API_doc($site_id);
  $property = tripal_get_remote_field_doc($site_id, $bundle_accession, $field_accession);
  if (!$property) {
    return $flist;
  }

  $formatters = $property['tripal_formatters'];
  foreach ($formatters as $formatter) {
    if (tripal_load_include_downloader_class($formatter)) {
      $flist[$formatter] = $formatter::$full_label;
    }
  }
  return $flist;
}
