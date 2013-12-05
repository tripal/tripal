<?php

/**
 * @file
 * @todo Add file header description
 */


# This script can be run as a stand-alone script to sync all the features from chado to drupal
// Parameter f specifies the feature_id to sync
// -f 0 will sync all features

$arguments = getopt("f:t:");

if (isset($arguments['f']) and isset($arguments['t']) and $arguments['t'] == 'chado_feature') {
  $drupal_base_url = parse_url('http://www.example.com');
  $_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
  $_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
  $_SERVER['REMOTE_ADDR'] = NULL;
  $_SERVER['REQUEST_METHOD'] = NULL;

  require_once 'includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

  $feature_id = $arguments['f'];

  if ($feature_id > 0) {
    tripal_feature_sync_feature($feature_id);
  }
  else {
    print "syncing all features...\n";
    tripal_feature_sync_features();
  }
}
/**
 *
 */
function tripal_feature_sync_form() {

  $form['description'] = array(
  '#type' => 'item',
  '#value' => t("Add feature types, optionally select an organism and ".
     "click the 'Sync all Features' button to create Drupal ".
     "content for features in chado. Only features of the types listed ".
     "below in the Feature Types box will be synced. You may limit the ".
     "features to be synced by a specific organism. Depending on the ".
     "number of features in the chado database this may take a long ".
     "time to complete. "),
  );

  $form['feature_types'] = array(
    '#title'       => t('Feature Types'),
    '#type'        => 'textarea',
    '#description' => t("Enter the names of the feature types to sync.  Pages for these feature ".
       "types will be created automatically for features that exist in the ".
       "chado database.  The names listed here should be spearated by ".
       "spaces or entered separately on new lines. The names must match ".
       "exactly (spelling and case) with terms in the sequence ontology"),
    '#required'    => TRUE,
    '#default_value' => variable_get('chado_sync_feature_types', 'gene contig'),
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

  return $form;
}
/**
 *
 */
function tripal_feature_sync_form_validate($form, &$form_state) {
  $organism_id   = $form_state['values']['organism_id'];
  $feature_types = $form_state['values']['feature_types'];

  // nothing to do
}
/**
 *
 */
function tripal_feature_sync_form_submit($form, &$form_state) {

  global $user;

  $organism_id   = $form_state['values']['organism_id'];
  $feature_types = $form_state['values']['feature_types'];

  $job_args = array(0, $organism_id, $feature_types);

  if ($organism_id) {
    $organism = tripal_core_chado_select('organism', array('genus', 'species'), array('organism_id' => $organism_id));
    $title = "Sync all features for " .  $organism[0]->genus . " " . $organism[0]->species;
  }
  else {
    $title = t('Sync all features for all synced organisms');
  }

  variable_set('chado_sync_feature_types', $feature_types);

  tripal_add_job($title, 'tripal_feature', 'tripal_feature_sync_features', $job_args, $user->uid);
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
  $num_nodes = db_result(db_query($csql));
    
  // calculate the interval at which we will print an update on the screen
  $num_set = 0;
  $num_per_interval = 100;
  
  // prepate the statements which will quickly add url alias. Because these
  // are not Chado tables we must manually prepare them 
  $psql = "
    PREPARE del_url_alias_by_src (text) AS
    DELETE FROM {url_alias} WHERE src = \$1
  ";
  db_query($psql);
  $psql = "
    PREPARE ins_url_alias_nisrds (text, text) AS
    INSERT INTO url_alias (src, dst) VALUES (\$1, \$2)
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
  while ($node = db_fetch_object($nodes)) {
   
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
    if($src == $dst) {
      continue;
    }
    
    // remove any previous alias and then add the new one
    $success = db_query("EXECUTE del_url_alias_by_src('%s')", $src);    
    if (!$success) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK");
      watchdog('trp-seturl', "Failed Removing URL Alias: %src", array('%src' => $src), WATCHDOG_ERROR);
      return;
    }
    $success = db_query("EXECUTE ins_url_alias_nisrds('%s', '%s')", $src, $dst);
    if (!$success) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK");
      watchdog('trp-seturl', "Failed Adding URL Alias: %dst", array('%dst' => $dst), WATCHDOG_ERROR);
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
  if(!$url_alias) {
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
  
  $i = 0;

  // get the list of available sequence ontology terms for which
  // we will build drupal pages from features in chado.  If a feature
  // is not one of the specified typse we won't build a node for it.
  if (!$feature_types) {
    $allowed_types = variable_get('chado_sync_feature_types', 'gene contig');
  }
  else {
    $allowed_types = $feature_types;
  }
  $allowed_types = preg_replace("/[\s\n\r]+/", " ", $allowed_types);

  print "Looking for features of type: $allowed_types\n";

  $so_terms = split(' ', $allowed_types);
  $where_cvt = "";
  foreach ($so_terms as $term) {
    $where_cvt .= "CVT.name = '$term' OR ";
  }
  $where_cvt = drupal_substr($where_cvt, 0, drupal_strlen($where_cvt)-3);  # strip trailing 'OR'

  // get the list of organisms that are synced and only include features from
  // those organisms
  $orgs = tripal_organism_get_synced();
  $where_org = "";
  foreach ($orgs as $org) {
    if ($organism_id) {
      if ($org->organism_id and $org->organism_id == $organism_id) {
        $where_org .= "F.organism_id = $org->organism_id OR ";
      }
    }
    else {
    if ($org->organism_id) {
      $where_org .= "F.organism_id = $org->organism_id OR ";
    }
    }
  }
  $where_org = drupal_substr($where_org, 0, drupal_strlen($where_org)-3);  # strip trailing 'OR'

  // use this SQL statement to get the features that we're going to upload
  $sql = "SELECT feature_id ".
        "FROM {FEATURE} F ".
        "  INNER JOIN {Cvterm} CVT ON F.type_id = CVT.cvterm_id ".
        "  INNER JOIN {CV} on CV.cv_id = CVT.cv_id ".
        "WHERE ($where_cvt) AND ($where_org) AND CV.name = 'sequence' ".
        "ORDER BY feature_id";

  // get the list of features
  $results = chado_query($sql);

  // load into ids array
  $count = 0;
  $ids = array();
  while ($id = db_fetch_object($results)) {
    $ids[$count] = $id->feature_id;
    $count++;
  }

  // make sure our vocabularies are set before proceeding
  tripal_feature_set_vocabulary();

  // pre-create the SQL statement that will be used to check
  // if a feature has already been synced.  We skip features
  // that have been synced
  $sql = "SELECT * FROM {chado_feature} WHERE feature_id = %d";

  // Iterate through features that need to be synced
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }
  $num_ids = sizeof($ids);
  $i = 0;
  foreach ($ids as $feature_id) {
    // update the job status every 1% features
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id, intval(($i/$count)*100));
    }
    // if we have a maximum number to sync then stop when we get there
    // if not then just continue on
    if ($max_sync and $i == $max_sync) {
      return '';
    }
    if (!db_fetch_object(db_query($sql, $feature_id))) {

      # parsing all the features can cause memory overruns
      # we are not sure why PHP does not clean up the memory as it goes
      # to avoid this problem we will call this script through an
      # independent system call
      print "$i of $num_ids Syncing feature id: $feature_id\n";
      $cmd = "php " . drupal_get_path('module', 'tripal_feature') . "/includes/tripal_feature.sync_features.inc -f $feature_id -t chado_feature";
      system($cmd);

    }
    $i++;
  }

  return '';
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_sync_feature($feature_id) {
  //print "\tSyncing feature $feature_id\n";

  global $user;
  $create_node = 1;   // set to 0 if the node exists and we just sync and not create

  // get the accession prefix
  $aprefix = variable_get('chado_feature_accession_prefix', 'FID');

  // if we don't have a feature_id then return
  if (!$feature_id) {
    drupal_set_message(t("Please provide a feature_id to sync"));
    return '';
  }

  // get information about this feature
  $fsql = "SELECT F.feature_id, F.name, F.uniquename,O.genus, ".
         "    O.species,CVT.name as cvname,F.residues,F.organism_id ".
         "FROM {FEATURE} F ".
         "  INNER JOIN {Cvterm} CVT ON F.type_id = CVT.cvterm_id ".
         "  INNER JOIN {Organism} O ON F.organism_id = O.organism_ID ".
         "WHERE F.feature_id = %d";
  $feature = db_fetch_object(chado_query($fsql, $feature_id));

  // get the synonyms for this feature
  $synsql = "SELECT S.name ".
            "FROM {feature_synonym} FS ".
            "  INNER JOIN {synonym} S on FS.synonym_id = S.synonym_id ".
            "WHERE FS.feature_id = %d";
  $synonyms = chado_query($synsql, $feature_id);

  // now add these synonyms to the feature object as a single string
  $synstring = '';
  while ($synonym = db_fetch_object($synonyms)) {
    $synstring .= "$synonym->name\n";
  }
  $feature->synonyms = $synstring;

  // check to make sure that we don't have any nodes with this feature name as a title
  // but without a corresponding entry in the chado_feature table if so then we want to
  // clean up that node.  (If a node is found we don't know if it belongs to our feature or
  // not since features can have the same name/title.)
  $tsql =  "SELECT * FROM {node} N ".
           "WHERE title = '%s'";
  $cnsql = "SELECT * FROM {chado_feature} ".
           "WHERE nid = %d";
  $nodes = db_query($tsql, $feature->name);
  // cycle through all nodes that may have this title
  while ($node = db_fetch_object($nodes)) {
    $feature_nid = db_fetch_object(db_query($cnsql, $node->nid));
    if (!$feature_nid) {
      drupal_set_message(t("%feature_id: A node is present but the chado_feature entry is missing... correcting", array('%feature_id' => $feature_id)));
      node_delete($node->nid);
    }
  }

  // check if this feature already exists in the chado_feature table.
  // if we have a chado feature, we want to check to see if we have a node
  $cfsql = "SELECT * FROM {chado_feature} ".
           "WHERE feature_id = %d";
  // @coder-ignore: don't need to use db_rewrite_sql() since need all nodes regardless of access control
  $nsql =  "SELECT * FROM {node} N ".
           "WHERE nid = %d";
  $chado_feature = db_fetch_object(db_query($cfsql, $feature->feature_id));
  if ($chado_feature) {
    drupal_set_message(t("%feature_id: A chado_feature entry exists", array('%feature_id' => $feature_id)));
    $node = db_fetch_object(db_query($nsql, $chado_feature->nid));
    if (!$node) {
      // if we have a chado_feature but not a node then we have a problem and
      // need to cleanup
      drupal_set_message(t("%feature_id: The node is missing, but has a chado_feature entry... correcting", array('%feature_id' => $feature_id)));
      $df_sql = "DELETE FROM {chado_feature} WHERE feature_id = %d";
      db_query($df_sql, $feature_id);
    }
    else {
      drupal_set_message(t("%feature_id: A corresponding node exists", array('%feature_id' => $feature_id)));
      $create_node = 0;
    }
  }

  // if we've encountered an error then just return.
  if ($error_msg = db_error()) {
    //print "$error_msg\n";
    return '';
  }

  // if a drupal node does not exist for this feature then we want to
  // create one.  Note that the node_save call in this block
  // will call the hook_submit function which
  if ($create_node) {
    // get the organism for this feature
    $sql = "SELECT * FROM {organism} WHERE organism_id = %d";
    $organism = db_fetch_object(chado_query($sql, $feature->organism_id));

    drupal_set_message(t("%feature_id: Creating node $feature->name", array('%feature_id' => $feature_id)));
    $new_node = new stdClass();
    $new_node->type = 'chado_feature';
    $new_node->uid = $user->uid;
    $new_node->title = "$feature->name, $feature->uniquename ($feature->cvname) $organism->genus $organism->species";
    $new_node->fname = "$feature->name";
    $new_node->uniquename = "$feature->uniquename";
    $new_node->feature_id = $feature->feature_id;
    $new_node->residues = $feature->residues;
    $new_node->organism_id = $feature->organism_id;
    $new_node->feature_type = $feature->cvname;
    $new_node->synonyms = $feature->synonyms;

    // validate the node and if okay then submit
    node_validate($new_node);
    if ($errors = form_get_errors()) {
      print "Error encountered validating new node. Cannot sync\n";
      foreach ($errors as $key => $msg) {        
        watchdog('trp-fsync', "%msg", array('%msg' => $msg), 'error');
      }
      exit;
    }
    else {
      $node = node_submit($new_node);
      node_save($node);
    }
  }
  else {
    $node = $chado_feature;
  }


  // set the taxonomy for this node
  drupal_set_message(t("%feature_id ($node->nid): setting taxonomy", array('%feature_id' => $feature_id)));
  tripal_feature_set_taxonomy($node, $feature_id);


  return '';
}

