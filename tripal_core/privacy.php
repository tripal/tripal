<?php
/*
 *  Perform permission check by node_id only if 'node_privacy_byrole' module is enabled
 */
function tripal_check_permission_by_node_id ($nid) {
	if (module_exists('node_privacy_byrole')) {
		global $user;
		$roles = $user->roles;
		$node_access = 0;
		foreach ($roles AS $rid => $role) {
			$p_sql = "SELECT grant_view FROM {node_access} WHERE nid=%d AND gid = %d";
			$access = db_result(db_query($p_sql,$nid, $rid));
			if ($access == 1) {
				$node_access = 1;
				break;
			}
		}
		if ($node_access == 1 || $user->uid == 1) {
			return TRUE;
		} else {
			return FALSE;
		}
	
	// If 'node_privacy_byrole' module is not enabled, return TRUE;
	} else {
		return TRUE;
	}
}