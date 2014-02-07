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
    $organism = chado_select_record('organism', array('genus', 'species'), array('organism_id' => $organism_id));
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
  $num_nodes = db_query($csql)->fetchField();

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
  while ($node = $nodes->fetchObject()) {

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
    $success = db_query("EXECUTE del_url_alias_by_src(':src')", array(':src' => $src));
    if (!$success) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK");
      tripal_report_error('trp-seturl', TRIPAL_ERROR, "Failed Removing URL Alias: %src", array('%src' => $src));
      return;
    }
    $success = db_query("EXECUTE ins_url_alias_nisrds(:src, :dst)", array(':src' => $src, ':dst' => $dst));
    if (!$success) {
      db_query('DEALLOCATE "del_url_alias_by_src"');
      db_query('DEALLOCATE "ins_url_alias_nisrds"');
      db_query("ROLLBACK");
      tripal_report_error('trp-seturl', TRIPAL_ERROR, "Failed Adding URL Alias: %dst", array('%dst' => $dst));
      return;
    }

    // update the job status every 1% stocks
    if ($job and $num_set % $num_per_interval == 0) {
      $percent = ($num_set / $num_nodes) * 100;
      tripal_set_job_progress($job, intval($percent));
      $percent = sprintf("%.2f", $percent);
      print "Setting URLs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";

    }
    $num_set++;
  }
  $percent = ($num_set / $num_nodes) * 100;
  tripal_set_job_progress($job, intval($percent));
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
  $stock = chado_select_record('stock', array('*'), $values, $options);
  if (!$stock) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find stock when setting URL alias for stock: %id", array('%id' => $node->stock_id));
    return FALSE;
  }
  $stock = (object) $stock[0];

  // get the organism
  $values = array('organism_id' => $stock->organism_id);
  $options = array('statement_name' => 'sel_organism_id');
  $organism  = chado_select_record('organism', array('*'), $values, $options);
  if (!$organism) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find organism when setting URL alias for stock: %id", array('%id' => $node->stock_id));
    return FALSE;
  }
  $genus = preg_replace('/\s/', '_', strtolower($organism[0]->genus));
  $species = preg_replace('/\s/', '_', strtolower($organism[0]->species));

  // get the type
  $values = array('cvterm_id' => $stock->type_id);
  $options = array('statement_name' => 'sel_cvterm_id');
  $cvterm = chado_select_record('cvterm', array('name'), $values, $options);
  if (!$cvterm) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find type when setting URL alias for stock: %id", array('%id' => $node->stock_id));
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
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot set alias longer than 128 characters: %alias.", array('%alias' => $url_alias));
    return "node/" . $node->nid;
  }

  return $url_alias;
}
