<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;

/**
 * Controller routines for the Tripal Module
 */
class PubSearchQueryNameAutocompleteController extends ControllerBase {

  public function handleAutocomplete(Request $request) {
    $public = \Drupal::service('database');
    $response = [];

    // $db is database e.g. "tripal_pub_library_pubmed"
    // $q is text user is entering
    $db = $request->query->get('db');
    $q = $request->query->get('q');

    $query = $public->select('tripal_pub_library_query', 'tplq')
        ->fields('tplq');
    $andGroup = $query->andConditionGroup()
        ->condition('criteria', '"plugin_id";s:\d+:"' . $db . '"', 'REGEXP')
        ->condition('criteria', '"loader_name";s:\d+:"' . $q, 'REGEXP');
    $query->condition($andGroup);
    $results = $query->execute();
    foreach ($results as $row) {
        $criteria_data = unserialize($row->criteria);
        $loader_name = $criteria_data['loader_name'];
        $response[] = [
          'label' => $loader_name . ' (' . $row->pub_library_query_id . ')'
        ];
    }

    return new JsonResponse($response);
  }
}
