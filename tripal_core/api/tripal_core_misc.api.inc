<?php 
/**
 * This function implements a pager, similar to a pager for a database query
 * using a non query.  Rather than pass a query as in pager_query function, a 
 * callback function should recieve as it's first argument an array of the 
 * argumetns passed to this function. The second argument should be the 
 * number of results to retreive, and the third argument should be the the
 * element index (same as with pager_query) function.  The callback function
 * should always return an array. If no values are available then an empty 
 * array should be returned.
 * 
 * @param $func
 *  The name of the callback function used to retrieve the results. The
 *  callback function should have three arguments: first, an array of values
 *  passed into the tripal_pager_callback; second, the start index of
 *  the paged values; third, the number of records to include.
 * @param limit
 *  The number of records to include in the paged results
 * @param element
 *  A unique integer ID if more than one pager exists on a page.  if multiple
 *  types of pagers (e.g. pager_query, chado_pager_query) are on the same
 *  page then this parameter should be unique amongst them all.
 * @param cfunc
 *  The name of the callback function used to specify the total results. The
 *  callback f unction should receive one argument: an array of values that
 *  are passed into the tripal_pager_callbck function. 
 */
function tripal_pager_callback($func, $limit = 9, $element = 0, $cfunc) {
  
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : '';
  
  // Substitute in query arguments.
  $args = func_get_args();
  $args = array_slice($args, 4);
  // Alternative syntax for '...'
  if (isset($args[0]) && is_array($args[0])) {
    $args = $args[0];
  }

  $count = $cfunc($args);  
  
  // Convert comma-separated $page to an array, used by other functions.
  $pager_page_array = explode(',', $page);

  // We calculate the total of pages as ceil(items / limit).
  $pager_total_items[$element] = $count;
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min((int)$pager_page_array[$element], ((int)$pager_total[$element]) - 1));
  
  // retrieve the results using the specified callback.
  return $func($args, $pager_page_array[$element] * $limit, $limit);
}
