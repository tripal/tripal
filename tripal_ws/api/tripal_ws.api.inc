<?php
/**
 * @file
 *
 * This file provides the Tripal Web Services API: a set of functions for
 * interacting with the Tripal Web Services.
 */

/**
 * @defgroup tripal_ws_api Tripal Web Services
 *
 * @ingroup tripal_api
 * The Tripal Web Services API provides a set of functions for interacting
 * with the Tripal Web Services.
 *
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
  if($field['type'] == 'image' and $field['module'] == 'image') {
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
 */
function tripal_get_web_services() {
  $services = array();

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
 */
function tripal_load_include_web_service_class($class) {

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    $file_path = realpath(".") . '/' . drupal_get_path('module', $module) . '/includes/TripalWebService/' . $class . '.inc';
    if (file_exists($file_path)) {
      module_load_include('inc', $module, 'includes/TripalWebService/' . $class);
      if (class_exists($class)) {
        return TRUE;
      }
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
 */
function tripal_add_site($name, $url, $version, $description) {
  $check_url = NULL;
  $check_name = NULL;
  $write_to_db = TRUE;
  // When inserting a record.
  $check_url =
    db_select('tripal_sites', 'ts')
      ->fields('ts', array('id'))
      ->condition('url', $url)
      ->condition('version', $version)
      ->execute()
      ->fetchField();

  $check_name =
    db_select('tripal_sites', 'ts')
      ->fields('ts', array('id'))
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
    ->fields(array(
        'name' => $name,
        'url' => $url,
        'version' => $version,
        'description' => $description
      ))
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
 * When passed a site_id from the tripal_sites table the full
 * site's vocab service is returned in json format
 *
 *
 */
function tripal_web_services_vocab_request($site_id) {
  // Get the site url from the tripal_sites table.
  if ($site_id) {
    $remote_site = db_select('tripal_sites', 'ts')
      ->fields('ts')
      ->condition('ts.id', $site_id)
      ->execute()
      ->fetchObject();
  }
  // Build the URL to the remote web services.
  $ws_version = $remote_site->version;
  $ws_url = $remote_site->url;
  $ws_url = trim($ws_url, '/');
  $ws_url .= '/web-services/vocab/' . $ws_version;

  // Build and make the request.
  $options = [];
  $data = drupal_http_request($ws_url, $options);

  if (!$data) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
        t('Could not connect to the remote web service.'));
    return FALSE;
  }

  // If the data object has an error then this is some sort of
  // connection error (not a Tripal web servcies error).
  if (property_exists($data, 'error')) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
        'Web Services error on remote site: %error.',
        array('%error' => $data->error));
    return FALSE;
  }

  // We got a response, so convert it to a PHP array.
  $data = drupal_json_decode($data->data);

  // Check if there was a Tripal Web Services error.
  if (array_key_exists('error', $data)) {
    $error = '</pre>' . print_r($data['error'], TRUE) . '</pre>';
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
        'Web Services error on remote site: %error.',
        array('%error' => $error));
    return FALSE;
  }

  return $data;
}

/**
 * Makes a request to a remote Tripal web services site.
 *
 * @param $query
 *   The query string. This string is added to the URL for the remote
 *   website.
 */
function tripal_web_services_remote_request($remote_site, $query) {
  $ctype = $query;
  $qdata = '';
  if (preg_match('/\?/', $query)) {
    list($ctype, $qdata) = explode('?', $query);
  }

  // Build the URL to the remote web services.
  $ws_version = $remote_site->version;
  $ws_url = $remote_site->url;
  $ws_url = trim($ws_url, '/');
  $ws_url .= '/web-services/content/' . $ws_version . '/' . $ctype;

  // Build the Query and make and substitions needed.
  //dpm($ws_url . '?' . $query);
  $options = array(
    'data' => $qdata,
  );
  $data = drupal_http_request($ws_url, $options);

  if (!$data) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
        t('Could not connect to the remote web service.'));
    return FALSE;
  }

  // If the data object has an error then this is some sort of
  // connection error (not a Tripal web servcies error).
  if (property_exists($data, 'error')) {
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
        'Web Services error on remote site: %error.',
        array('%error' => $data->error));
    return FALSE;
  }

  // We got a response, so convert it to a PHP array.
  $data = drupal_json_decode($data->data);

  // Check if there was a Tripal Web Services error.
  if (array_key_exists('error', $data)) {
    $error = '</pre>' . print_r($data['error'], TRUE) . '</pre>';
    tripal_report_error('tripal_ws', TRIPAL_ERROR,
        'Web Services error on remote site: %error.',
        array('%error' => $error));
    return FALSE;
  }

  return $data;
}