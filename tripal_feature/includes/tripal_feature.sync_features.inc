<?php

/**
 * @file
 * @todo Add file header description
 */

/**
 *
 */
function tripal_feature_sync_form() {

  $form['description'] = array(
  '#markup' => t("Add feature types, optionally select an organism and " .
     "click the 'Sync all Features' button to create Drupal " .
     "content for features in chado. Only features of the types listed " .
     "below in the Feature Types box will be synced. You may limit the " .
     "features to be synced by a specific organism. Depending on the " .
     "number of features in the chado database this may take a long " .
     "time to complete. "),
  );

  $form['feature_types'] = array(
    '#title'       => t('Feature Types'),
    '#type'        => 'textarea',
    '#description' => t("Enter the names of the feature types to sync.  Pages for these feature " .
       "types will be created automatically for features that exist in the " .
       "chado database.  The names listed here should be spearated by " .
       "spaces or entered separately on new lines. The names must match " .
       "exactly (spelling and case) with terms in the sequence ontology"),
    '#required'    => TRUE,
    '#default_value' => variable_get('chado_sync_feature_types', 'gene mRNA'),
  );

  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $orgs = tripal_organism_get_synced();
  $organisms[] = '';
  foreach ($orgs as $organism) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['organism_id'] = array(
    '#title'       => t('Organism'),
    '#type'        => t('select'),
    '#description' => t("Choose the organism for which features types set above will be synced. Only organisms which also have been synced will appear in this list."),
    '#options'     => $organisms,
  );


  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Sync all Features'),
    '#weight' => 3,
  );
  
  get_tripal_feature_admin_form_cleanup_set($form);

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function get_tripal_feature_admin_form_cleanup_set(&$form) {
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['cleanup']['description'] = array(
    '#markup' => t("With Drupal and chado residing in different databases " .
        "it is possible that nodes in Drupal and features in Chado become " .
        "\"orphaned\".  This can occur if a feature node in Drupal is " .
        "deleted but the corresponding chado feature is not and/or vice " .
        "versa.  The Cleanup function will also remove nodes for features " .
        "that are not in the list of allowed feature types as specified " .
        "above.  This is helpful when a feature type needs to be " .
        "removed but was previously present as Drupal nodes. " .
        "Click the button below to resolve these discrepancies."),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned features'),
    '#weight' => 2,
  );
}

/**
 *
 */
function tripal_feature_sync_form_validate($form, &$form_state) {
  $organism_id   = $form_state['values']['organism_id'];
  $feature_types = $form_state['values']['feature_types'];
}
/**
 *
 */
function tripal_feature_sync_form_submit($form, &$form_state) {

  global $user;

  $organism_id   = $form_state['values']['organism_id'];
  $feature_types = $form_state['values']['feature_types'];
  
  switch ($form_state['values']['op']) {
    case  t('Sync all Features') :
      $job_args = array(0, $organism_id, $feature_types);
      if ($organism_id) {
        $organism = tripal_core_chado_select('organism', array('genus', 'species'), array('organism_id' => $organism_id));
        $title = "Sync all features for " . $organism[0]->genus . " " . $organism[0]->species;
      }
      else {
        $title = t('Sync all features for all synced organisms');
      }
      variable_set('chado_sync_feature_types', $feature_types);    
      tripal_add_job($title, 'tripal_feature', 'tripal_feature_sync_features', $job_args, $user->uid);
      break;
      
    case t('Clean up orphaned features') :
      $job_args = array();
      tripal_add_job('Cleanup orphaned features', 'tripal_feature',
      'tripal_features_cleanup', $job_args, $user->uid);
      break;
  }
}
/**
 *  
 * @param $na 
 *   Tripal expects all jobs to have at least one argument. For this function
 *   we don't need any, so we have this dummy argument as a filler
 * @param $job_id
 */
function tripal_feature_set_urls($na = NULL, $job = NULL) {
  
  // begin the transaction
  db_query("BEGIN");
      
  print "\nNOTE: Setting of URLs is performed using a database transaction. \n" .
        "If the load fails or is terminated prematurely then the entire set of \n" .
        "new URLs will be rolled back and no changes will be made\n\n";
  
  // get the number of records we need to set URLs for
  $csql = "SELECT count(*) FROM {chado_feature}";
  $num_nodes = db_query($csql)->fetchField();
    
  // calculate the interval at which we will print an update on the screen
  $num_set = 0;
  $num_per_interval = 100;
  
  // prepate the statements which will quickly add url alias. Because these
  // are not Chado tables we must manually prepare them 
  $psql = "
    PREPARE del_url_alias_by_src (text) AS
    DELETE FROM {url_alias} WHERE source = \$1
  ";
  db_query($psql);
  $psql = "
    PREPARE ins_url_alias_nisrds (text, text) AS
    INSERT INTO url_alias (source, alias) VALUES (\$1, \$2)
  ";
  db_query($psql);
  
  // get the URL alias syntax string
  $url_alias = variable_get('chado_feature_url_string', '/feature/[genus]/[species]/[type]/[uniquename]'); 
  if (!$url_alias) {
    $url_alias = '/feature/[genus]/[species]/[type]/[uniquename]';
  } 
  $url_alias = preg_replace('/^\//', '', $url_alias); // remove any preceeding forward slash
  
  
  // get the list of features that have been synced
  $sql = "SELECT * FROM {chado_feature}";
  $nodes = db_query($sql);  
  while ($node = $nodes->fetchObject()) {
   
    // get the URL alias
    $src = "node/$node->nid";
    $dst = tripal_feature_get_feature_url($node, $url_alias);
    if (!$dst) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK"); 
      return; 
    }    
    
    // if the src and dst is the same (the URL alias couldn't be set)
    // then skip to the next one. There's nothing we can do about this one.
    if ($src == $dst) {
      continue;
    }
    
    // remove any previous alias and then add the new one
    $success = db_query("EXECUTE del_url_alias_by_src(:source)", array(':source' => $src));    
    if (!$success) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK");
      watchdog('trp-seturl', "Failed Removing URL Alias: %source", array('%source' => $src), WATCHDOG_ERROR);
      return;
    }
    $success = db_query("EXECUTE ins_url_alias_nisrds(:source, :alias)", array(':source' => $src, ':alias' => $dst));
    if (!$success) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK");
      watchdog('trp-seturl', "Failed Adding URL Alias: %alias", array('%alias' => $dst), WATCHDOG_ERROR);
      return;
    }

    // update the job status every 1% features
    if ($job and $num_set % $num_per_interval == 0) {
      $percent = ($num_set / $num_nodes) * 100;
      tripal_job_set_progress($job, intval($percent));
      $percent = sprintf("%.2f", $percent);
      print "Setting URLs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
      
    }
    $num_set++;
  }
  $percent = ($num_set / $num_nodes) * 100;
  tripal_job_set_progress($job, intval($percent));
  $percent = sprintf("%.2f", $percent);
  print "Setting URLs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
  print "\nDone. Set " . number_format($num_set) . " URLs\n";
  
  // unprepare the statements
  db_query('DEALLOCATE "del_url_alias_by_src"');
  db_query('DEALLOCATE "ins_url_alias_nisrds"');
  
  db_query("COMMIT");
}
/**
 * 
 * @param $node
 *   A node object containing at least the feature_id and nid
 * @param $url_alias
 *   Optional.  This should be the URL alias syntax string that contains
 *   placeholders such as [id], [genus], [species], [name], [uniquename],
 *   and [type].  These placeholders will be substituted for actual values.
 *   If this parameter is not provided then the value of the 
 *   chado_feature_url_string Drupal variable will be used.
 */
function tripal_feature_get_feature_url($node, $url_alias = NULL) {

  // get the starting URL alias
  if (!$url_alias) {
    $url_alias = variable_get('chado_feature_url_string', '/feature/[genus]/[species]/[type]/[uniquename]'); 
    if (!$url_alias) {
      $url_alias = '/feature/[genus]/[species]/[type]/[uniquename]';
    } 
    $url_alias = preg_replace('/^\//', '', $url_alias); // remove any preceeding forward slash
  }

  // get the feature 
  $values = array('feature_id' => $node->feature_id); 
  $options = array('statement_name' => 'sel_feature_id');       
  $feature = tripal_core_chado_select('feature', array('*'), $values, $options);
  if (!$feature) {
    watchdog('trp-seturl', "Cannot find feature when setting URL alias for feature: %id", array('%id' => $node->feature_id), WATCHDOG_ERROR);
    return FALSE;  
  }
  $feature = (object) $feature[0];
  
  // get the organism
  $values = array('organism_id' => $feature->organism_id);
  $options = array('statement_name' => 'sel_organism_id');
  $organism  = tripal_core_chado_select('organism', array('*'), $values, $options);  
  if (!$organism) {
    watchdog('trp-seturl', "Cannot find organism when setting URL alias for feature: %id", array('%id' => $node->feature_id), WATCHDOG_ERROR);
    return FALSE;  
  }
  $genus = preg_replace('/\s/', '_', strtolower($organism[0]->genus));
  $species = preg_replace('/\s/', '_', strtolower($organism[0]->species)); 

  // get the type
  $values = array('cvterm_id' => $feature->type_id);
  $options = array('statement_name' => 'sel_cvterm_id');
  $cvterm = tripal_core_chado_select('cvterm', array('name'), $values, $options);
  if (!$cvterm) {
    watchdog('trp-seturl', "Cannot find type when setting URL alias for feature: %id", array('%id' => $node->feature_id), WATCHDOG_ERROR);
    return FALSE;  
  }
  $type = preg_replace('/\s/', '_', $cvterm[0]->name);
  
  // now substitute in the values
  $url_alias = preg_replace('/\[id\]/', $feature->feature_id, $url_alias);
  $url_alias = preg_replace('/\[genus\]/', $genus, $url_alias);
  $url_alias = preg_replace('/\[species\]/', $species, $url_alias);
  $url_alias = preg_replace('/\[type\]/', $type, $url_alias);
  $url_alias = preg_replace('/\[name\]/', $feature->name, $url_alias);
  $url_alias = preg_replace('/\[uniquename\]/', $feature->uniquename, $url_alias);
 
  // the dst field of the url_alias table is only 128 characters long. 
  // if this is the case then simply return the node URL, we can't set this one
  if (strlen($url_alias) > 128) {
    watchdog('trp-seturl', "Cannot set alias longer than 128 characters: %alias.", array('%alias' => $url_alias), WATCHDOG_ERROR);
    return "node/" . $node->nid;
  }
  
  return $url_alias;
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_sync_features($max_sync = 0, $organism_id = NULL,
  $feature_types = NULL, $job_id = NULL) {
  
  global $user;
    
  // get the list of available sequence ontology terms for which
  // we will build drupal pages from features in chado.  If a feature
  // is not one of the specified typse we won't build a node for it.
  if (!$feature_types) {
    $allowed_types = variable_get('chado_sync_feature_types', 'gene mRNA');
  }
  else {
    $allowed_types = $feature_types;
  }
  $allowed_types = preg_replace("/[\s\n\r]+/", " ", $allowed_types);

  print "Looking for features of type: $allowed_types\n";

  $so_terms = split(' ', $allowed_types);
  $where_cvt = "";
  $args = array();
  $i = 0;
  foreach ($so_terms as $term) {
    $where_cvt .= "CVT.name = :term$i OR ";
    $args[":term$i"] = $term;
    $i++;
  }
  $where_cvt = drupal_substr($where_cvt, 0, drupal_strlen($where_cvt)-3);  # strip trailing 'OR'

  // get the list of organisms that are synced and only include features from
  // those organisms
  $orgs = tripal_organism_get_synced();
  $where_org = "";
  $i = 0;
  foreach ($orgs as $org) {
    if ($organism_id) {
      if ($org->organism_id and $org->organism_id == $organism_id) {
        $where_org .= "F.organism_id = :org_id$i OR ";
        $args[":org_id$i"] = $org->organism_id;
      }
    }
    else {
      if ($org->organism_id) {
        $where_org .= "F.organism_id = :org_id$i OR ";
        $args[":org_id$i"] = $org->organism_id;
      }
    }
    $i++;
  }
  $where_org = drupal_substr($where_org, 0, drupal_strlen($where_org)-3);  # strip trailing 'OR'

  // get the list of features that we will sync
  $sql = "
    SELECT F.*, CVT.name as cvtname, O.genus, O.species 
    FROM {feature} F 
      INNER JOIN {cvterm} CVT ON F.type_id     = CVT.cvterm_id 
      INNER JOIN {cv} CV      ON CV.cv_id      = CVT.cv_id
      INNER JOIN {organism} O ON O.organism_id = F.organism_id 
      LEFT JOIN public.chado_feature CF ON CF.feature_id = F.feature_ID
    WHERE 
     ($where_cvt) AND ($where_org) AND CV.name = 'sequence' AND
     CF.feature_id IS NULL
    ORDER BY feature_id";
  
  print_r($sql);
  print_r($args);
  $results = chado_query($sql, $args);
  

  // Iterate through features that need to be synced
  $count = $results->rowCount();
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }
  $i = 0;  
  $transaction = db_transaction();
  try {
    //  tripal_feature_set_vocabulary();
    print "Loading feature $i of $count (0.00%). Memory: " . number_format(memory_get_usage()) . " bytes\r";
    foreach ($results as $feature) {
      // update the job status every 1% features
      if ($job_id and $i % $interval == 0) {
        $percent = sprintf("%.2f", ($i / $count) * 100);        
        print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
        tripal_job_set_progress($job_id, intval(($i/$count)*100));
      }
      $new_node = new stdClass();
      $new_node->type = 'chado_feature';
      $new_node->uid = $user->uid;
      $new_node->feature_id = $feature->feature_id;
      
      // set these values as they are needed for constructing the title and
      // the match the names of the fields in the feature form
      $new_node->organism_id = $feature->organism_id;
      $new_node->fname = $feature->name;
      $new_node->uniquename = $feature->uniquename;
      $new_node->feature_type = $feature->cvtname;
      
      $form = array(); // dummy variable
      $form_state = array(); // dummy variable
      node_validate($new_node, $form, $form_state);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
      }
      else {
        watchdog('tfeature_sync', "Unable to create feature node. ID: %feature_id, Name: %name.",
          array('%feature_id' => $feature->feature_id, '%name' => $feature->uniquename), WATCHDOG_WARNING);      
      }
      $i++;
    }
  }
  catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    watchdog_exception('trp-fsync', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
}



/**
 *
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_feature
 */
function tripal_features_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('feature', $job_id);

}