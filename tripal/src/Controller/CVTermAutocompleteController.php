<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;

/**
 * Controller routines for the Tripal Module
 */
class CVTermAutocompleteController extends ControllerBase {

  public function handleAutocomplete(int $count = 5, Request $request) {

    $string = $request->query->get('q');
    $response = [];

    $idmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpaces = $idmanager->getCollectionList();
    foreach ($idSpaces as $name) {
      $idSpace = $idmanager->loadCollection($name);
      $terms = $idSpace->getTerms($string);
      foreach ($terms as $term_name => $term_ids) {
        foreach ($term_ids as $term_id => $term) {
          $response[] = $term_name . ' (' . $term->getIdSpace() . ':' . $term->getAccession() . ')';
        }
      }
    }
    sort($response);

    return new JsonResponse($response);
  }
}