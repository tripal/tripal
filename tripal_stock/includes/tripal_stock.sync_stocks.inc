<?php

/**
 * @file
 * @todo Add file header description
 */


# This script can be run as a stand-alone script to sync all the stocks from chado to drupal
// Parameter f specifies the stock_id to sync
// -f 0 will sync all stocks

$arguments = getopt("f:t:");

if (isset($arguments['f']) and isset($arguments['t']) and $arguments['t'] == 'chado_stock') {
  $drupal_base_url = parse_url('http://www.example.com');
  $_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
  $_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
  $_SERVER['REMOTE_ADDR'] = NULL;
  $_SERVER['REQUEST_METHOD'] = NULL;

  require_once 'includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

  $stock_id = $arguments['f'];

  if ($stock_id > 0 ) {
    tripal_stock_sync_stock($stock_id);
  }
  else{
    print "syncing all stocks...\n";
    tripal_stock_sync_stocks();
  }
}
/**
 *
 */
function tripal_stock_sync_form() {

  $form['description'] = array(
  '#type' => 'item',
  '#value' => t("Stocks of the types listed ".
     "below in the Stock Types box will be synced (leave blank to sync all types). You may limit the ".
     "stocks to be synced by a specific organism. Depending on the ".
     "number of stocks in the chado database this may take a long ".
     "time to complete. "),
  );

  $form['stock_types'] = array(
    '#title'       => t('Stock Types'),
    '#type'        => 'textarea',
    '#description' => t("Enter the names of the stock types to sync. " . 
       "Leave blank to sync all stocks. Pages for these stock ".
       "types will be created automatically for stocks that exist in the ".
       "chado database.  The names listed here should be spearated by ".
       "spaces or entered separately on new lines. The names must match ".
       "exactly (spelling and case) with terms in the sequence ontology"),
    '#default_value' => variable_get('chado_sync_stock_types', ''),
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
    '#description' => t("Choose the organism for which stocks types set above will be synced. Only organisms which also have been synced will appear in this list."),
    '#options'     => $organisms,
  );


  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Sync all Stocks'),
    '#weight' => 3,
  );

  return $form;
}
/**
 *
 */
function tripal_stock_sync_form_validate($form, &$form_state) {
  $organism_id   = $form_state['values']['organism_id'];
  $stock_types = $form_state['values']['stock_types'];

  // nothing to do
}
/**
 *
 */
function tripal_stock_sync_form_submit($form, &$form_state) {

  global $user;

  $organism_id   = $form_state['values']['organism_id'];
  $stock_types = $form_state['values']['stock_types'];

  $job_args = array(0, $organism_id, $stock_types);

  if ($organism_id) {
    $organism = tripal_core_chado_select('organism', array('genus', 'species'), array('organism_id' => $organism_id));
    $title = "Sync stocks for " .  $organism[0]->genus . " " . $organism[0]->species;
  }
  else {
    $title = 'Sync stocks';
  }  

  variable_set('chado_sync_stock_types', $stock_types);

  tripal_add_job($title, 'tripal_stock', 'tripal_stock_sync_stocks', $job_args, $user->uid);
}
/**
 *  
 * @param $na 
 *   Tripal expects all jobs to have at least one argument. For this function
 *   we don't need any, so we have this dummy argument as a filler
 * @param $job_id
 */
function tripal_stock_set_urls($na = NULL, $job = NULL) {
  
  // begin the transaction
  db_query("BEGIN");
      
  print "\nNOTE: Setting of URLs is performed using a database transaction. \n" .
        "If the load fails or is terminated prematurely then the entire set of \n" .
        "new URLs will be rolled back and no changes will be made\n\n";
  
  // get the number of records we need to set URLs for
  $csql = "SELECT count(*) FROM {chado_stock}";
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
  $url_alias = variable_get('chado_stock_url_string', '/stock/[genus]/[species]/[type]/[uniquename]'); 
  if (!$url_alias) {
    $url_alias = '/stock/[genus]/[species]/[type]/[uniquename]';
  } 
  $url_alias = preg_replace('/^\//', '', $url_alias); // remove any preceeding forward slash
  
  
  // get the list of stocks that have been synced
  $sql = "SELECT * FROM {chado_stock}";
  $nodes = db_query($sql);  
  while ($node = db_fetch_object($nodes)) {
   
    // get the URL alias
    $src = "node/$node->nid";
    $dst = tripal_stock_get_stock_url($node, $url_alias);
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

    // update the job status every 1% stocks
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
 *   A node object containing at least the stock_id and nid
 * @param $url_alias
 *   Optional.  This should be the URL alias syntax string that contains
 *   placeholders such as [id], [genus], [species], [name], [uniquename],
 *   and [type].  These placeholders will be substituted for actual values.
 *   If this parameter is not provided then the value of the 
 *   chado_stock_url_string Drupal variable will be used.
 */
function tripal_stock_get_stock_url($node, $url_alias = NULL) {

  // get the starting URL alias
  if(!$url_alias) {
    $url_alias = variable_get('chado_stock_url_string', '/stock/[genus]/[species]/[type]/[uniquename]'); 
    if (!$url_alias) {
      $url_alias = '/stock/[genus]/[species]/[type]/[uniquename]';
    } 
    $url_alias = preg_replace('/^\//', '', $url_alias); // remove any preceeding forward slash
  }

  // get the stock 
  $values = array('stock_id' => $node->stock_id); 
  $options = array('statement_name' => 'sel_stock_id');       
  $stock = tripal_core_chado_select('stock', array('*'), $values, $options);
  if (!$stock) {
    watchdog('trp-seturl', "Cannot find stock when setting URL alias for stock: %id", array('%id' => $node->stock_id), WATCHDOG_ERROR);
    return FALSE;  
  }
  $stock = (object) $stock[0];
  
  // get the organism
  $values = array('organism_id' => $stock->organism_id);
  $options = array('statement_name' => 'sel_organism_id');
  $organism  = tripal_core_chado_select('organism', array('*'), $values, $options);  
  if (!$organism) {
    watchdog('trp-seturl', "Cannot find organism when setting URL alias for stock: %id", array('%id' => $node->stock_id), WATCHDOG_ERROR);
    return FALSE;  
  }
  $genus = preg_replace('/\s/', '_', strtolower($organism[0]->genus));
  $species = preg_replace('/\s/', '_', strtolower($organism[0]->species)); 

  // get the type
  $values = array('cvterm_id' => $stock->type_id);
  $options = array('statement_name' => 'sel_cvterm_id');
  $cvterm = tripal_core_chado_select('cvterm', array('name'), $values, $options);
  if (!$cvterm) {
    watchdog('trp-seturl', "Cannot find type when setting URL alias for stock: %id", array('%id' => $node->stock_id), WATCHDOG_ERROR);
    return FALSE;  
  }
  $type = preg_replace('/\s/', '_', $cvterm[0]->name);
  
  // now substitute in the values
  $url_alias = preg_replace('/\[id\]/', $stock->stock_id, $url_alias);
  $url_alias = preg_replace('/\[genus\]/', $genus, $url_alias);
  $url_alias = preg_replace('/\[species\]/', $species, $url_alias);
  $url_alias = preg_replace('/\[type\]/', $type, $url_alias);
  $url_alias = preg_replace('/\[name\]/', $stock->name, $url_alias);
  $url_alias = preg_replace('/\[uniquename\]/', $stock->uniquename, $url_alias);
 
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
 * @ingroup tripal_stock
 */
function tripal_stock_sync_stocks($max_sync = 0, $organism_id = NULL,
  $stock_types = NULL, $job_id = NULL) {
  //print "Syncing stocks (max of $max_sync)\n";
  $i = 0;

  // get the list of available sequence ontology terms for which
  // we will build drupal pages from stocks in chado.  If a stock
  // is not one of the specified typse we won't build a node for it.
  if (!$stock_types) {
    $allowed_types = variable_get('chado_sync_stock_types', '');
  }
  else {
    $allowed_types = $stock_types;
  }
  
  if ($allowed_types) {
    $allowed_types = preg_replace("/[\s\n\r]+/", " ", $allowed_types);
    print "Looking for stocks of type: $allowed_types\n";
  
    $so_terms = split(' ', $allowed_types);
    $where_cvt = "";
    foreach ($so_terms as $term) {
      $where_cvt .= "CVT.name = '$term' OR ";
    }
    $where_cvt = drupal_substr($where_cvt, 0, drupal_strlen($where_cvt)-3);  # strip trailing 'OR'
  }
  else {
    $where_cvt = '1=1';
  }

  // get the list of organisms that are synced and only include stocks from
  // those organisms
  $orgs = tripal_organism_get_synced();
  $where_org = "";
  foreach ($orgs as $org) {
    if ($organism_id) {
      if ($org->organism_id and $org->organism_id == $organism_id) {
        $where_org .= "S.organism_id = $org->organism_id OR ";
      }
    }
    else {
      if ($org->organism_id) {
        $where_org .= "S.organism_id = $org->organism_id OR ";
      }
    }
  }
  $where_org = drupal_substr($where_org, 0, drupal_strlen($where_org)-3);  # strip trailing 'OR'

  // use this SQL statement to get the stocks that we're going to upload
  $sql = "SELECT stock_id ".
        "FROM {stock} S ".
        "  INNER JOIN {cvterm} CVT ON S.type_id = CVT.cvterm_id ".
        "WHERE ($where_cvt) AND ($where_org) ".
        "ORDER BY stock_id";

  // get the list of stocks
  $results = chado_query($sql);

  // load into ids array
  $count = 0;
  $ids = array();
  while ($id = db_fetch_object($results)) {
    $ids[$count] = $id->stock_id;
    $count++;
  }

  // make sure our vocabularies are set before proceeding
//  tripal_stock_set_vocabulary();

  // pre-create the SQL statement that will be used to check
  // if a stock has already been synced.  We skip stocks
  // that have been synced
  $sql = "SELECT * FROM {chado_stock} WHERE stock_id = %d";

  // Iterate through stocks that need to be synced
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }
  $num_ids = sizeof($ids);
  $i = 0;
  foreach ($ids as $stock_id) {
    // update the job status every 1% stocks
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id, intval(($i/$count)*100));
    }
    // if we have a maximum number to sync then stop when we get there
    // if not then just continue on
    if ($max_sync and $i == $max_sync) {
      return '';
    }
    if (!db_fetch_object(db_query($sql, $stock_id))) {

      # parsing all the stocks can cause memory overruns
      # we are not sure why PHP does not clean up the memory as it goes
      # to avoid this problem we will call this script through an
      # independent system call
      print ($i + 1) . " of $num_ids Syncing stock id: $stock_id\n";
      $cmd = "php " . drupal_get_path('module', 'tripal_stock') . "/includes/tripal_stock.sync_stocks.inc -f $stock_id -t chado_stock";
      print "$cmd\n";
      system($cmd);

    }
    $i++;
  }

  return '';
}

/**
 *
 *
 * @ingroup tripal_stock
 */
function tripal_stock_sync_stock($stock_id) {
  
print "\tSyncing stock $stock_id\n";
  
global $user;
  $create_node = 1;   // set to 0 if the node exists and we just sync and not create

  // get the accession prefix
  $aprefix = variable_get('chado_stock_accession_prefix', 'SID');
  
  // if we don't have a stock_id then return
  if (!$stock_id) {
    drupal_set_message(t("Please provide a stock_id to sync"));
    return '';
  }

  // get information about this stock
  $fsql = "SELECT S.*, O.genus, O.species,CVT.name as cvname ".
          "FROM {stock} S ".
          "  INNER JOIN {cvterm} CVT ON S.type_id = CVT.cvterm_id ".
          "  INNER JOIN {organism} O ON S.organism_id = O.organism_ID ".
          "WHERE S.stock_id = %d";
  $stock = db_fetch_object(chado_query($fsql, $stock_id));
  
  /*
  // get the synonyms for this stock
  $synsql = "SELECT S.name ".
            "FROM {stock_synonym} SS ".
            "  INNER JOIN {synonym} S on SS.synonym_id = S.synonym_id ".
            "WHERE SS.stock_id = %d";
  $synonyms = chado_query($synsql, $stock_id);  
    
  // now add these synonyms to the stock object as a single string
  $synstring = '';
  while ($synonym = db_fetch_object($synonyms)) {
    $synstring .= "$synonym->name\n";
  }
  $stock->synonyms = $synstring;
  */

  // check to make sure that we don't have any nodes with this stock name as a title
  // but without a corresponding entry in the chado_stock table if so then we want to
  // clean up that node.  (If a node is found we don't know if it belongs to our stock or
  // not since stocks can have the same name/title.)
  $tsql =  "SELECT * FROM {node} N ".
           "WHERE title = '%s'";
  $cnsql = "SELECT * FROM {chado_stock} ".
           "WHERE nid = %d";
  $nodes = db_query($tsql, $stock->name);
  // cycle through all nodes that may have this title
  while ($node = db_fetch_object($nodes)) {
    $stock_nid = db_fetch_object(db_query($cnsql, $node->nid));
    if (!$stock_nid) {
      drupal_set_message(t("%stock_id: A node is present but the chado_stock entry is missing... correcting", array('%stock_id' => $stock_id)));
      node_delete($node->nid);
    }
  }

  // check if this stock already exists in the chado_stock table.
  // if we have a chado stock, we want to check to see if we have a node
  $cfsql = "SELECT * FROM {chado_stock} ".
           "WHERE stock_id = %d";
  $nsql =  "SELECT * FROM {node} N ".
           "WHERE nid = %d";
  $chado_stock = db_fetch_object(db_query($cfsql, $stock->stock_id));
  if ($chado_stock) {
    drupal_set_message(t("%stock_id: A chado_stock entry exists", array('%stock_id' => $stock_id)));
    $node = db_fetch_object(db_query($nsql, $chado_stock->nid));
    if (!$node) {
      // if we have a chado_stock but not a node then we have a problem and
      // need to cleanup
      drupal_set_message(t("%stock_id: The node is missing, but has a chado_stock entry... correcting", array('%stock_id' => $stock_id)));
      $df_sql = "DELETE FROM {chado_stock} WHERE stock_id = %d";
      db_query($df_sql, $stock_id);
    }
    else {
      drupal_set_message(t("%stock_id: A corresponding node exists", array('%stock_id' => $stock_id)));
      $create_node = 0;
    }
  }

  // if we've encountered an error then just return.
  if ($error_msg = db_error()) {
    //print "$error_msg\n";
    return '';
  }

  // if a drupal node does not exist for this stock then we want to
  // create one.  Note that the node_save call in this block
  // will call the hook_submit function which
  if ($create_node) {
    // get the organism for this stock
    $sql = "SELECT * FROM {organism} WHERE organism_id = %d";
    $organism = db_fetch_object(chado_query($sql, $stock->organism_id));

    drupal_set_message(t("%stock_id: Creating node $stock->name", array('%stock_id' => $stock_id)));
    $new_node = new stdClass();
    $new_node->type = 'chado_stock';
    $new_node->uid = $user->uid;
    $new_node->title = "$stock->name, $stock->uniquename ($stock->cvname) $organism->genus $organism->species";
    $new_node->sname = "$stock->name";
    $new_node->uniquename = "$stock->uniquename";
    $new_node->type_id = $stock->type_id;
    $new_node->organism_id = $stock->organism_id;
    $new_node->stock_id = $stock->stock_id;        
    $new_node->chado_stock_exists = TRUE;
    
    // validate the node and if okay then submit
    node_validate($new_node);
    if ($errors = form_get_errors()) {
      print "Error encountered validating new node. Cannot sync: $msg\n";
      foreach ($errors as $key => $msg) {        
        watchdog('trp-fsync', "%msg", array('%msg' => $msg), WATCHDOG_ERROR);
      }
      exit;
    }
    else {
      $node = node_submit($new_node);
      node_save($node);
    }
  }
  else {
    $node = $chado_stock;
  }

  return '';
}
