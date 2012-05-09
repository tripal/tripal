<?php

/*
 *  Perform permission check by analysis_id only if 'node_privacy_byrole' module is enabled
 */
function tripal_analysis_check_permission ($analysis_id) {
	if (module_exists('node_privacy_byrole')) {
		global $user;
		$roles = $user->roles;
		$node_access = 0;
		foreach ($roles AS $rid => $role) {
			$p_sql = "SELECT grant_view FROM {node_access} NA INNER JOIN {chado_analysis} CA ON NA.nid = CA.nid WHERE analysis_id=%d AND gid = %d";
			$access = db_result(db_query($p_sql,$analysis_id, $rid));
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

/*
 * Set permissions for features associated with an analysis
 */
function tripal_analysis_set_feature_permission ($analysis_id, $nid) {
	
	print "Updating feature permissions:\n";
	
	// Get features associated with the analysis
	$sql = "SELECT feature_id FROM {analysisfeature} WHERE analysis_id = %d";
	$previous_db = db_set_active('chado');
	$features = db_query($sql, $analysis_id);
	db_set_active($previous_db);
	
	// Convert feature_id into node_id
	$feature_nids = array ();
	$counter = 0;
	$sql = "SELECT nid FROM {chado_feature} WHERE feature_id = %d";
	while ($feature = db_fetch_object($features)) {
		$feature_nids [$counter] = db_result(db_query($sql, $feature->feature_id));
		$counter ++;
	}
	
	//Convert analysis_id into node_id
	$ana_nid = db_result(db_query("SELECT nid FROM {chado_analysis} WHERE analysis_id = $analysis_id"));
	// Get available roles
	$roles = array_keys(user_roles());
	
	$interval = intval(($counter + 1) * 0.01);
	$idx_iterations = 1;
	
	// Update permission node by node
	foreach ($feature_nids AS $fnid) {
		if ($idx_iterations % $interval == 0) {
			$percentage = (int) ($idx_iterations / ($counter + 1) * 100);
			tripal_job_set_progress($job_id, $percentage);
			print $percentage."% ";
		}

		db_query("DELETE FROM {node_privacy_byrole} WHERE nid = %d AND realm = 'node_privacy_byrole_role'", $fnid);
		foreach($roles AS $rid) {
			// Get permissions of this analysis for this role
			$rsql = "SELECT * FROM {node_privacy_byrole} WHERE gid = %d AND nid = %d AND realm = 'node_privacy_byrole_role'";
			$ana_perm = db_fetch_object(db_query($rsql, $rid, $ana_nid));
			db_query("INSERT INTO {node_privacy_byrole} (nid, gid, realm, grant_view, grant_update, grant_delete)
                  VALUES (%d, %d, '%s', %d, %d, %d)", $fnid, $rid, 'node_privacy_byrole_role', $ana_perm->grant_view,
                  $ana_perm->grant_update, $ana_perm->grant_delete);
		}
		$node = node_load($fnid);
		node_save($node);
		$idx_iterations ++;
	}	
}