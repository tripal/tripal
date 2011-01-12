<?php

/*************************************************************************
 * Purpose: To add basetable_nid fields to all result arrays of a view
 *   only if the basetable_nid field is added. This function will only be
 *   called if chado/drupal are not in the same database (ie: only if 
 *   a join between the base and node table isn't possible.
 *
 * @params the view object passed to hook_views_pre_render
 * @return the views object with nids added to the result array
 */
function tripal_core_add_node_ids_to_view (&$view) {

	return $view;
}