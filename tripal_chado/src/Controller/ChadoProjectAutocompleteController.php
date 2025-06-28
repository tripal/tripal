<?php

namespace Drupal\tripal_chado\Controller;

use Symfony\Component\HttpFoundation\Request;

/**
 * Controller, Chado Project Autocomplete.
 */
class ChadoProjectAutocompleteController extends ChadoGenericAutocompleteController {
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
    // Project name has a unique constraint, so we don't need the pkey value
    // included in the autocomplete result.
    $this->include_pkey = FALSE;
    // This autocomplete only matches for the same starting characters.
    $this->match_operator = 'STARTS_WITH';
    // Call the generic autocomplete handler.
    return $this->handleGenericAutocomplete($request, 'project', 'name', '.', 'projectprop', $count, $type_id);
  }

  /**
   * Fetch the project id number, given a project name value.
   *
   * @param string $project
   *   Project name value. This table column has a unique constraint,
   *   so we are guaranteed either zero or one matches.
   *
   * @return integer
   *   Project id number of the project with this name name, or 0 if
   *   no matching project record was found.
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
   *   Corresponding project name of the project id number, or
   *   an empty string if no matching project record was found.
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
