<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Drupal;
use Symfony\Component\HttpFoundation\Request;

class TripalFileQuotaUserAutocomplete extends ControllerBase{

  /**
   * Autocomplete function for listing existing users on the site.
   *
   * @return JsonResponse of users that match the query in the textfield
   **/
  public function index(Request $request) {
    $matches = [];

    $cleaned = Drupal::database()->escapeLike($request->query->get('q'));

    $ids = Drupal::entityQuery('user')
      ->accessCheck(TRUE)
      ->condition('name', '%' . $cleaned . '%', 'LIKE')
      ->execute();

    $users = Drupal\user\Entity\User::loadMultiple($ids);

    foreach ($users as $row) {
      $name = $row->getAccountName();
      $matches[] = $name;
    }

    return new JsonResponse($matches);
  }

}
