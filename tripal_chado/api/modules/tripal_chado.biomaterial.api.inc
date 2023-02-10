<?php
/**
 * @file
 * Provides API functions specifically for managing biomaterial
 * records in Chado.
 */

/**
 * @defgroup tripal_biomaterial_api Chado Biomaterial
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing biomaterial
 * records in Chado.
 * @}
 */

/**
 * Used for autocomplete in forms for identifying biomaterials
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_biomaterial_api
 */
function chado_autocomplete_biomaterial($string = '') {
  $items = [];
  $sql = "
    SELECT
      B.biomaterial_id as id, B.name
    FROM {biomaterial} B
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
