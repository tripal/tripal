<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

/**
 * Controller, Chado Project Autocomplete.
 */
class ChadoProjectAutocompleteController extends ControllerBase {
  /**
   * Controller method, autocomplete project name.
   *
   * @param Request request
   *
   * @param int $count
   *   Desired number of matching names to suggest.
   *   Default to 5 items.
   *   Must be declared in autocomplete route parameter i.e. ['count' => 5].
   *
   * @param int $type_id
   *   Project type set in projectprop.type_id to restrict projects to specific type.
   *   Default to 0, return projects regardless of type.
   *   Must be declared in autocomplete route parameter i.e. ['type_id' => 0].
   *
   * @return Json Object
   *   Matching project rows in an array where project name
   *   is both the value to the array keys label and value.
   */
  public function handleAutocomplete(Request $request, int $count = 5, int $type_id = 0) {
    // Array to hold matching project records.
    $response = [];

    if ($request->query->get('q')) {
      // Get typed in string input from the URL.
      $string = trim($request->query->get('q'));

      if (strlen($string) > 0 && $count > 0) {
        // Proceed to autocomplete when string is at least a character
        // long and result count is set to a value greater than 0.

        // Transform string as a search keyword pattern.
        $keyword = strtolower($string) . '%';

        if ($type_id > 0) {
          // Restrict to type provided by type_id in the route parameter.
          $sql  = "SELECT name FROM {1:project} AS p LEFT JOIN {1:projectprop} AS t USING (project_id)
            WHERE LOWER(p.name) LIKE :keyword AND t.type_id = :type_id ORDER BY p.name ASC LIMIT %d";
          $args = [':keyword' => $keyword, ':type_id' => $type_id];
        }
        else {
          // Match projects regardless of type.
          $sql  = "SELECT name FROM {1:project} WHERE LOWER(name) LIKE :keyword ORDER BY name ASC LIMIT %d";
          $args = [':keyword' => $keyword];
        }

        // Prepare Chado database connection and execute sql query by providing value
        // to :keyword and/or :type_id placeholder text.
        $connection = \Drupal::service('tripal_chado.database');
        $query = sprintf($sql, $count);
        $results = $connection->query($query, $args);

        // Compose response result.
        if ($results) {
          foreach($results as $record) {
            $response[] = [
              'value' => $record->name, // Value returned and value displayed by textfield.
              'label' => $record->name  // Value shown in the list of options.
            ];
          }
        }
      }
    }

    return new JsonResponse($response);
  }

  /**
   * Fetch the project id number, given a project name value.
   *
   * @param string $project
   *   Project name value.
   *
   * @return integer
   *   Project id number of the project name or 0 if no matching
   *   project record was found.
   */
  public static function getProjectId(string $project): int {
    $id = 0;

    if (!empty($project)) {
      $sql = "SELECT project_id FROM {1:project} WHERE name = :name LIMIT 1";

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection->query($sql, [':name' => $project]);

      if ($result) {
        $id = $result->fetchField();
      }
    }

    return $id;
  }

  /**
   * Fetch the project name, given a project id number.
   *
   * @param int $project
   *   Project id number value.
   *
   * @return string
   *   Corresponding project name of the project id number or
   *   empty string if no matching project record was found.
   */
  public static function getProjectName(int $project): string {
    $name = '';

    if ($project > 0) {
      $sql = "SELECT name FROM {1:project} WHERE project_id = :project_id LIMIT 1";

      $connection = \Drupal::service('tripal_chado.database');
      $result = $connection->query($sql, ['project_id' => $project]);

      if ($result) {
        $name = $result->fetchField();
      }
    }

    return $name;
  }
}
