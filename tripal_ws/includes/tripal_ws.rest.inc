<?php

function tripal_ws_rest() {

  global $base_url;

  // Set some initial variables.
  $response = array();
  $result = array();
  $status = 'success';
  $version = 'v0.1';
  $message = '';
  $api_url = "$base_url/ws/bio-data/$version";
  $page_limit = 25;
  $pager_id = 0;

  // Lump everything ito a try block so that if there is a problem we can
  // throw an error and have that returned in the response.
  try {

    // Get the list of published terms (these are the bundle IDs)
    $bundles = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->execute();
    $terms = array();
    while ($bundle = $bundles->fetchObject()) {
      $cvterm_id = preg_replace('/^bio-data_/', '', $bundle->name);
      if ($cvterm_id) {
        $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $cvterm_id));
        $terms[$cvterm->name] = $cvterm;
      }
    }
    ksort($terms, SORT_STRING|SORT_FLAG_CASE);
    $result['content_types'] = array();
    foreach ($terms as $name => $cvterm) {
      $result['content_types'][$name] = array(
        'vocabulary' => $cvterm->cv_id->name,
        'namespace' => $cvterm->dbxref_id->db_id->name,
      );
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

