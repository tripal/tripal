<?php
/**
 * @file
 * Provides API functions specifically for managing study
 * records in Chado.
 */

/**
 * @defgroup tripal_study_api Chado Study
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions for working with study records in Chado that
 * go beyond the generic Chado API functions.
 * @}
 */

/**
 * Used for autocomplete in forms for identifying studies
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_study_api
 */
function chado_autocomplete_study($string = '') {
  $items = [];
  $sql = "
    SELECT
      S.study_id as id, S.name
    FROM {study} S
    WHERE lower(S.name) like lower(:str)
    ORDER by S.name
    LIMIT 25 OFFSET 0
  ";
  $records = chado_query($sql, [':str' => $string . '%']);
  while ($r = $records->fetchObject()) {
    $key = "$r->name [id: $r->id]";
    $items[$key] = "$r->name";
  }

  drupal_json_output($items);
}
