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
    
    $q = $request->query->get('q');
    $db = $request->query->get('db');
    $db = str_ireplace('tripal_pub_library_', '', $db);
    $response = [];

    // $response[] = $q;
    // $response[] = $db;

    $query = $public->select('tripal_pub_library_query', 'tplq')
        ->fields('tplq');
    $andGroup = $query->andConditionGroup()
        ->condition('criteria', '%' . $db . '%', 'ILIKE')
        ->condition('criteria', '%' . $q . '%', 'ILIKE');
    $query->condition($andGroup);
    $results = $query->execute();
    foreach ($results as $row) {
        $criteria_data = unserialize($row->criteria);
        $loader_name = $criteria_data['loader_name'];
        $response[] = [
            // 'value' => $row->pub_library_query_id,
            'label' => $loader_name . ' (' . $row->pub_library_query_id . ')'
        ];
    }
    // $response[] = "OK";

    return new JsonResponse($response);
  }
}