<?php

/**
 * @file
 * Hooks provided by the JSON:API Response Alter module
 * for Tripal Webservices.
 */

use Symfony\Component\HttpFoundation\Response;

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alters the JSON:API response.
 *
 * @param array $jsonapi_response
 *   The decoded JSON data to be altered.
 * @param \Symfony\Component\HttpFoundation\Response $response
 *   The response.
 */
function hook_jsonapi_response_alter(array &$jsonapi_response, Response $response) {
  // print_r($jsonapi_response);
  // echo "got here for realz";

  // Detect Tripal content
  // $request_uri = $this->routeMatch->routeMatches->storage[0]["obj"]->server->parameters["REQUEST_URI"];
  $uri_parts = explode('/', $this->routeMatch->routeMatches->storage[0]["obj"]->server->parameters["REQUEST_URI"]);
  print_r($uri_parts);
  $jsonapi_response['seantest'] = 'homestark';
  $jsonapi_response['seantest2'] = 'runner';
}
