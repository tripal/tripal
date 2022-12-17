<?php
/**
 * @file
 * Provides API functions specifically for managing featuremap
 * records in Chado.
 */

/**
 * @defgroup tripal_featuremap_api Chado Featuremap
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions for working with featuremap records in Chado that
 * go beyond the generic Chado API functions.
 * @}
 */

/**
 * Used for autocomplete in forms for identifying featuremaps
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_featuremap_api
 */
function chado_autocomplete_featuremap($string = '') {
  $items = [];
  $sql = "
    SELECT
      FM.featuremap_id as id, FM.name
    FROM {featuremap} FM
    WHERE lower(FM.name) like lower(:str)
    ORDER by FM.name
    LIMIT 25 OFFSET 0
  ";
  $records = chado_query($sql, [':str' => $string . '%']);
  while ($r = $records->fetchObject()) {
    $key = "$r->name [id: $r->id]";
    $items[$key] = "$r->name";
  }

  drupal_json_output($items);
}
