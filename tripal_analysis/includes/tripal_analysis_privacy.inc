<?php
/**
 * @file
 * Perform permission check (required node_privacy_byrole module).
 *
 * @ingroup tripal_analysis
 */

/**
 * Perform permission check by analysis_id only if 'node_privacy_byrole' module is enabled
 *
 * @param $analysis_id
 *   Which analysis to check if the user has permission to view
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_check_permission($analysis_id) {
  if (module_exists('node_privacy_byrole')) {
    global $user;
    $roles = $user->roles;
    $node_access = 0;
    foreach ($roles AS $rid => $role) {
      $p_sql = "
        SELECT grant_view
        FROM {node_access} NA
          INNER JOIN {chado_analysis} CA ON NA.nid = CA.nid
        WHERE analysis_id = :analysis_id AND gid = :gid";
      $access = db_query($p_sql, array(':analysis_id' => $analysis_id, ':gid' => $rid))->fetchField();
      if ($access == 1) {
        $node_access = 1;
        break;
      }
    }
    if ($node_access == 1 || $user->uid == 1) {
      return TRUE;
    }
    else {
      return FALSE;
    }

  // If 'node_privacy_byrole' module is not enabled, return TRUE;
  }
  else {
    return TRUE;
  }
}

/**
 * Set permissions for features associated with an analysis
 * @todo Stephen: ensure the $nid param is defined correctly
 *
 * @param $analysis_id
 *   Features associated with this analysis will have their permissions set
 * @param $nid
 *   The node ID of the analysis
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_set_feature_permission($analysis_id, $nid) {

  print "Updating feature permissions:\n";

  // Get features associated with the analysis
  $sql = "SELECT feature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id";
  $features = chado_query($sql, array(':analysis_id' => $analysis_id));

  // Convert feature_id into node_id
  $feature_nids = array();
  $counter = 0;
  $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
  while ($feature = $features->fetchObject()) {
    $feature_nids[$counter] = db_query($sql, array(':feature_id' => $feature->feature_id))->fetchField();
    $counter ++;
  }

  //Convert analysis_id into node_id
  $ana_nid = db_result(db_query("SELECT nid FROM {chado_analysis} WHERE analysis_id = :analysis_id", array(':analysis_id' => $analysis_id)));
  // Get available roles
  $roles = array_keys(user_roles());

  $interval = intval(($counter + 1) * 0.01);
  $idx_iterations = 1;

  // Update permission node by node
  foreach ($feature_nids AS $fnid) {
    if ($idx_iterations % $interval == 0) {
      $percentage = (int) ($idx_iterations / ($counter + 1) * 100);
      tripal_set_job_progress($job_id, $percentage);
      print $percentage . "% ";
    }

    db_query("DELETE FROM {node_privacy_byrole} WHERE nid = :nid AND realm = 'node_privacy_byrole_role'", array(':nid' => $fnid));
    foreach ($roles AS $rid) {
      // Get permissions of this analysis for this role
      $rsql = "SELECT * FROM {node_privacy_byrole} WHERE gid = :gid AND nid = :nid AND realm = 'node_privacy_byrole_role'";
      $ana_perm = db_query($rsql, array(':gid' => $rid, ':nid' => $ana_nid))->fetchObject();
      db_query("INSERT INTO {node_privacy_byrole} (nid, gid, realm, grant_view, grant_update, grant_delete)
                VALUES (:nid, :gid, :realm, :grant_view, :grant_update, :grant_delete)",
                array(':nid' => $fnid, ':gid' => $rid, ':realm' => 'node_privacy_byrole_role',
                      ':grant_view' => $ana_perm->grant_view,
                      ':grant_update' => $ana_perm->grant_update,
                      ':grant_delete' => $ana_perm->grant_delete));
    }
    $node = node_load($fnid);
    node_save($node);
    $idx_iterations ++;
  }
}
