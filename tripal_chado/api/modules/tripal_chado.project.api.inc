<?php
/**
 * @file
 * Provides API functions specifically for managing project
 * records in Chado.
 */

/**
 * @defgroup tripal_project_api Chado Project
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing project
 * records in Chado.  The project table of Chado is used for storing a variety
 * of data types besides just projects from a project collection.  Examples of
 * other records commonly stored in the project table are germplasm and
 * individuals from a breeding population.
 * @}
 */

/**
 * Used for autocomplete in forms for identifying projects
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_project_api
 */
function chado_autocomplete_project($string = '') {
  $items = [];
  $sql = "
    SELECT
      B.project_id as id, B.name
    FROM {project} B
    WHERE lower(B.name) like lower(:str)
    ORDER by B.name
    LIMIT 25 OFFSET 0
  ";
  $records = chado_query($sql, [':str' => $string . '%']);
  while ($r = $records->fetchObject()) {
    $key = "$r->name [id: $r->id]";
    $items[$key] = "$r->name";
  }

  drupal_json_output($items);
}
