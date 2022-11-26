<?php
/**
 * @file
 * Provides API functions specifically for managing assay
 * records in Chado.
 */

/**
 * @defgroup tripal_assay_api Chado Assay
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions for working with assay records in Chado that
 * go beyond the generic Chado API functions.
 * @}
 */

/**
 * Used for autocomplete in forms for identifying assays
 *
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_assay_api
 */
function chado_autocomplete_assay($string = '') {
  $items = [];
  $sql = "
    SELECT
      B.assay_id as id, B.name
    FROM {assay} B
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
