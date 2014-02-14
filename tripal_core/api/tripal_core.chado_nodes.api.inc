<?php
/**
 * @file
 * API to handle much of the common functionality implemented when creating a drupal node type.
 */

/**
 * @defgroup tripal_chado_node_api Chado Node API
 * @ingroup tripal_chado_api
 * @{
 * Many Tripal modules implement Drupal node types as a means of displaying chado
 * records individually through Drupal as a single web page. In order to do this, many of
 * the same drupal hooks are implemented and the code between modules is actually quite
 * similar. This API aims to abstract much of the common functionality in order to make
 * it easier for new Tripal modules to implement drupal node types and to centralize the
 * maintenance effort as much as possible.
 *
 * A generic sync form has been created. See chado_node_sync_form() for
 * instructions on how to implement this form in your module.
 *
 * Many of the base chado tables also have associated prop, _dbxref and _relationship
 * tables. Generic mini-forms have been created to help you handle these forms. To
 * implement this functionality you call the mini-form from your module node form and
 * then call the associated update functions from both your hook_insert and hook_update.
 * The functions of interest are as follows:
 *   - chado_add_node_form_properties() and chado_update_node_form_properties()
 *     to provide an interface for adding/removing properties
 *   - chado_add_node_form_dbxrefs() and chado_update_node_form_dbxrefs()
 *     to provide an interface for adding/removing additional database references
 *   - chado_add_node_form_relationships() and chado_update_node_form_relationships()
 *     to provide an interface for adding/removing relationships between chado records
 *     from your base table
 * @}
 */

/**
 * Get chado id for a node. E.g, if you want to get 'analysis_id' from the
 * 'analysis' table for a synced 'chado_analysis' node, (the same for
 * organisms and features):
 * $analysis_id = chado_get_id_from_nid ('analysis', $node->nid)
 * $organism_id = chado_get_id_from_nid ('organism', $node->nid)
 * $feature_id  = chado_get_id_from_nid ('feature', $node->nid)
 *
 * @param $table
 * @param $nid
 *
 * @ingroup tripal_chado_node_api
 */
function chado_get_id_from_nid($table, $nid) {
  $sql = "SELECT " . $table . "_id as id FROM {chado_$table} WHERE nid = :nid";
  return db_query($sql, array(':nid' => $nid))->fetchField();
}

/**
 *  Get node id for a chado feature/organism/analysis. E.g, if you want to
 *  get the node id for an analysis, use:
 *  $nid = chado_get_nid_from_id ('analysis', $analysis_id)
 *  Likewise,
 *  $nid = chado_get_nid_from_id ('organism', $organism_id)
 *  $nid = chado_get_nid_from_id ('feature', $feature_id)
 *
 *  @ingroup tripal_chado_node_api
 */
function chado_get_nid_from_id($table, $id) {
  $sql = "SELECT nid FROM {chado_$table} WHERE " . $table . "_id = :" . $table . "_id";
  return db_query($sql, array(":" . $table . "_id" => $id))->fetchField();
}

/**
 * Generic Sync Form to aid in sync'ing (create drupal nodes linking to chado content)
 * any chado node type.
 *
 * To use this you need to add a call to it from your hook_menu() and
 * add some additional information to your hook_node_info(). The Following code gives an
 * example of how this might be done:
 * @code

  function modulename_menu() {

    $module_name = 'tripal_example';        // the machine name of your module
    $linking_table = 'chado_example';       // the base specified in hook_node_info
    // This menu item will be a tab on the admin/tripal/chado/tripal_example page
    // that is not selected by default
    $items['admin/tripal/chado/tripal_example/sync'] = array(
      'title' => ' Sync',
      'description' => 'Sync examples from Chado with Drupal',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('chado_node_sync_form', $module_name, $linking_table),
      'access arguments' => array('administer tripal examples'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0
    );

    return $items;
  }

  function modulename_node_info() {
    return array(
      'chado_example' => array(
        'name' => t('example'),
        'base' => 'chado_example',
        'description' => t('A Chado example is a collection of material that can be sampled and have experiments performed on it.'),
        'has_title' => TRUE,
        'locked' => TRUE,

        // this is what differs from the regular Drupal-documented hook_node_info()
        'chado_node_api' => array(
          'base_table' => 'example',            // the name of the chado base table
          'hook_prefix' => 'chado_example',     // usually the name of the node type
          'record_type_title' => array(
            'singular' => t('Example'),         // Singular human-readable title
            'plural' => t('Examples')           // Plural human-readable title
          ),
          'sync_filters' => array( // filters for syncing
            'type_id'     => TRUE,     // TRUE if there is an example.type_id field
            'organism_id' => TRUE,     // TRUE if there is an example.organism_id field
            'checkboxes'  => array('name')  // If the 'checkboxes' key is present then the
                                            // value must be an array of column names in
                                            // base table. The values from these columns will
                                            // be retreived, contentated with a space delimeter
                                            // and provided in a list of checkboxes
                                            // for the user to choose which to sync.
          ),
        )
      ),
    );
  }
 * @endcode
 *
 * For more information on how you can override some of this behaviour while still
 * benifiting from as much of the common architecture as possible see the following
 * functions: hook_chado_node_sync_create_new_node(), hook_chado_node_sync_form(),
 * hook_chado_node_sync_select_query().
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_sync_form($form, &$form_state) {
  $form = array();

  if (isset($form_state['build_info']['args'][0])) {
    $module = $form_state['build_info']['args'][0];
    $linking_table = $form_state['build_info']['args'][1];
    $node_info = call_user_func($module . '_node_info');
    $args = $node_info[$linking_table]['chado_node_api'];
    $form_state['chado_node_api'] = $args;
  }

  // define the fieldsets
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => 'Sync ' . $args['record_type_title']['plural'],
    '#descrpition' => '',
  );

  $form['sync']['description'] = array(
  '#type' => 'item',
  '#value' => t("%title_plural of the types listed ".
     "below in the %title_singular Types box will be synced (leave blank to sync all types). You may limit the ".
     "%title_plural to be synced by a specific organism. Depending on the ".
     "number of %title_plural in the chado database this may take a long ".
     "time to complete. ",
     array(
      '%title_singular' => $args['record_type_title']['singular'],
      '%title_plural' => $args['record_type_title']['plural']
    )),
  );

  if ($args['sync_filters']['type_id']) {
    $form['sync']['type_ids'] = array(
      '#title'       => t('%title_singular Types',
         array(
          '%title_singular' => $args['record_type_title']['singular'],
          '%title_plural' => $args['record_type_title']['plural']
      )),
      '#type'        => 'textarea',
      '#description' => t("Enter the names of the %title_singular types to sync. " .
         "Leave blank to sync all %title_plural. Separate each type with a comma ".
         "or new line. Pages for these %title_singular ".
         "types will be created automatically for %title_plural that exist in the ".
         "chado database. The names must match ".
         "exactly (spelling and case) with terms in the ontologies",
         array(
          '%title_singular' => strtolower($args['record_type_title']['singular']),
          '%title_plural' => strtolower($args['record_type_title']['plural'])
        )),
      '#default_value' => (isset($form_state['values']['type_id'])) ? $form_state['values']['type_id'] : '',
    );
  }

  // get the list of organisms
  if ($args['sync_filters']['organism_id']) {
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $results = chado_query($sql);
    $organisms[] = '';
    foreach ($results as $organism) {
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
    }
    $form['sync']['organism_id'] = array(
      '#title'       => t('Organism'),
      '#type'        => t('select'),
      '#description' => t("Choose the organism for which %title_plural types set above will be synced.",
         array(
          '%title_singular' => $args['record_type_title']['singular'],
          '%title_plural' => $args['record_type_title']['plural']
        )),
      '#options'     => $organisms,
      '#default_value' => (isset($form_state['values']['organism_id'])) ? $form_state['values']['organism_id'] : 0,
    );
  }
  // get the list of organisms
  if (array_key_exists('checkboxes', $args['sync_filters'])) {
    // get the base schema
    $base_table = $args['base_table'];
    $table_info = chado_get_schema($base_table);

    // if the base table does not have a primary key or has more than one then
    // we can't proceed, otherwise, generate the checkboxes
    if (array_key_exists('primary key', $table_info) and count($table_info['primary key']) == 1) {
      $pkey = $table_info['primary key'][0];
      $columns  = $args['sync_filters']['checkboxes'];
      $select_cols = implode("|| ' ' ||", $columns);

      // get non-synced records
      $sql = "
        SELECT BT.$pkey as id, $select_cols as value
        FROM {" . $base_table . "} BT
          LEFT JOIN public.$linking_table LT ON LT.$pkey = BT.$pkey
        WHERE LT.$pkey IS NULL
        ORDER BY value ASC
      ";
      $results = chado_query($sql);
      $values = array();
      foreach ($results as $result) {
        $values[$result->id] = $result->value;
      }
      if (count($values) > 0) {
        $form['sync']['ids'] = array(
          '#title'         => 'Avaliable ' . $args['record_type_title']['plural'],
          '#type'          => 'checkboxes',
          '#description'   => t("The above  %title_plural have not been synced. Check those to be synced or leave all unchecked to sync them all.",
            array(
              '%title_singular' => strtolower($args['record_type_title']['singular']),
              '%title_plural'   => strtolower($args['record_type_title']['plural'])
            )),
          '#options'       => $values,
          '#default_value' => (isset($form_state['values']['ids'])) ? $form_state['values']['ids'] : array(),
          '#prefix'        => '<div style="height: 200px; overflow: scroll">',
          '#suffix'        => '</div><br>',
        );
      }
      else {
        $form['sync']['no_ids'] = array(
          '#markup' => "<p>There are no " .  strtolower($args['record_type_title']['plural']) . " to sync.</p>",
        );
      }
    }
  }
  // if we provide a list of checkboxes we shouldn't need a max_sync
  else {
    $form['sync']['max_sync'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum number of records to Sync'),
      '#description' => t('Leave this field empty to sync all records, regardless of number'),
      '#default_value' => (isset($form_state['values']['max_sync'])) ? $form_state['values']['max_sync'] : '',
    );
  }

  $form['sync']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Sync ' . $args['record_type_title']['plural']),
    '#weight' => 3,
  );


  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#markup' => t("<p>With Drupal and chado residing in different databases " .
        "it is possible that nodes in Drupal and " . strtolower($args['record_type_title']['plural']) . " in Chado become " .
        "\"orphaned\".  This can occur if a node in Drupal is " .
        "deleted but the corresponding chado records is not and/or vice " .
        "versa. Click the button below to resolve these discrepancies.</p>"),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => 'Clean up orphaned ' . strtolower($args['record_type_title']['plural']),
    '#weight' => 2,
  );

  // Allow each module to alter this form as needed
  $hook_form_alter = $args['hook_prefix'] . '_chado_node_sync_form';
  if (function_exists($hook_form_alter)) {
    $form = call_user_func($hook_form_alter, $form, $form_state);
  }

  return $form;
}

/**
 * Generic Sync Form Submit
 *
 * @ingroup tripal_core
 */
function chado_node_sync_form_submit($form, $form_state) {

  global $user;

  if (preg_match('/^Sync/', $form_state['values']['op'])) {
    // get arguments
    $args = $form_state['chado_node_api'];
    $module = $form_state['chado_node_api']['hook_prefix'];
    $base_table = $form_state['chado_node_api']['base_table'];

    // Allow each module to hijack the submit if needed
    $hook_form_hijack_submit = $args['hook_prefix'] . '_chado_node_sync_form_submit';
    if (function_exists($hook_form_hijack_submit)) {
      return call_user_func($hook_form_hijack_submit, $form, $form_state);
    }

    // Get the types separated into a consistent string
    $types = array();
    if (isset($form_state['values']['type_ids'])) {
      // seperate by new line or comma.
      $temp_types = preg_split("/[,\n\r]+/", $form_state['values']['type_ids']);

      // remove any extra spacing around the types
      for($i = 0; $i < count($temp_types); $i++) {
        // skip empty types
        if (trim($temp_types[$i]) == '') {
          continue;
        }
        $types[$i] = trim($temp_types[$i]);
      }
    }

    // Get the ids to be synced
    $ids = array();
    if (array_key_exists('ids', $form_state['values'])){
      foreach ($form_state['values']['ids'] as $id => $selected) {
        if ($selected) {
          $ids[] = $id;
        }
      }
    }

    // get the organism to be synced
    $organism_id = FALSE;
    if (array_key_exists('organism_id',  $form_state['values'])) {
      $organism_id = $form_state['values']['organism_id'];
    }

    // Job Arguments
    $job_args = array(
      'base_table' => $base_table,
      'max_sync' => (!empty($form_state['values']['max_sync'])) ? $form_state['values']['max_sync'] : FALSE,
      'organism_id' => $organism_id,
      'types' => $types,
      'ids' => $ids,
    );

    $title = "Sync " . $args['record_type_title']['plural'];
    tripal_add_job($title, $module, 'chado_node_sync_records', $job_args, $user->uid);
  }
  if (preg_match('/^Clean up orphaned/', $form_state['values']['op'])) {
    $module = $form_state['chado_node_api']['hook_prefix'];
    $base_table = $form_state['chado_node_api']['base_table'];
    $job_args = array($base_table);
    tripal_add_job($form_state['values']['op'], $module, 'chado_cleanup_orphaned_nodes', $job_args, $user->uid);
  }
}

/**
 * Actual Sync Function. Works on a group of records
 *
 * @ingroup tripal_chado_node_api
 */
function chado_node_sync_records($base_table, $max_sync = FALSE, $organism_id = FALSE,
    $types = array(), $ids = array(), $job_id = NULL) {

  global $user;
  $base_table_id = $base_table . '_id';

  print "\nSync'ing $base_table records.  ";

  // START BUILDING QUERY TO GET ALL RECORD FROM BASE TABLE THAT MATCH
  $select = array("$base_table.*");
  $joins = array();
  $where_clauses = array();
  $where_args = array();

  // If types are supplied then handle them
  $restrictions = '';
  if (count($types) > 0) {
    $restrictions .= "  Type(s): " . implode(', ',$types) . "\n";

    $select[] = 'cvterm.name as cvtname';
    $joins[] = "LEFT JOIN {cvterm} cvterm ON $base_table.type_id = cvterm.cvterm_id";
    foreach ($types as $type) {
      $where_clauses['type'][] = "cvterm.name = :type_name_$type";
      $where_args['type'][":type_name_$type"] = $type;
    }
  }

  // if IDs have been supplied
  if ($ids) {
    $restrictions .= "  Specific Records: " . count($ids) . " recored(s) specified.\n";
    foreach ($ids as $id) {
      $where_clauses['id'][] = "$base_table.$base_table_id = :id_$id";
      $where_args['id'][":id_$id"] = $id;
    }
  }

  // If Organism is supplied
  if ($organism_id) {
    $organism = chado_select_record('organism', array('*'), array('organism_id' => $organism_id));
    $restrictions .= "  Organism: " . $organism[0]->genus . " " . $organism[0]->species . "\n";

    $select[] = 'organism.*';
    $joins[] = "LEFT JOIN {organism} organism ON organism.organism_id = $base_table.organism_id";
    $where_clauses['organism'][] = 'organism.organism_id = :organism_id';
    $where_args['organism'][':organism_id'] = $organism_id;
  }

  // Allow module to add to query
  $hook_query_alter = 'chado_' . $base_table . '_chado_node_sync_select_query';
  if (function_exists($hook_query_alter)) {
    call_user_func($hook_query_alter, $select, $joins, $where_clauses, $where_args);
  }

  // Build Query, we do a left join on the chado_xxxx table in the Drupal schema
  // so that if no criteria are specified we only get those items that have not
  // yet been synced.
  $query = "
    SELECT " . implode(', ',$select) . ' ' .
    'FROM {' . $base_table . '} ' . $base_table . ' ' . implode(' ', $joins) . ' '.
    "  LEFT JOIN public.chado_$base_table CT ON CT.$base_table_id = $base_table.$base_table_id " .
    "WHERE CT.$base_table_id IS NULL AND";

  // extend the where clause if needed
  $where = '';
  $sql_args = array();
  if (count($where_clauses['type']) > 0) {
    $where .= '(' . implode(' OR ', $where_clauses['type']) . ') AND';
    $sql_args = array_merge($sql_args, $where_args['type']);
  }
  if (count($where_clauses['organism']) > 0) {
    $where .= '(' . implode(' OR ', $where_clauses['organism']) . ') AND';
    $sql_args = array_merge($sql_args, $where_args['organism']);
  }
  if (count($where_clauses['id']) > 0) {
    $where .= '(' . implode(' OR ', $where_clauses['id']) . ') AND';
    $sql_args = array_merge($sql_args, $where_args['id']);
  }
  if ($where) {
    $query .= $where;
  }
  $query = substr($query, 0, -4); // remove the trailing 'AND'
  $query .- " ORDER BY " . $base_table_id;

  // If Maximum number to Sync is supplied
  if ($max_sync) {
    $query .= " LIMIT $max_sync";
    $restrictions .= "  Limited to $max_sync records.\n";
  }

  if ($restrictions) {
    print "Records matching these criteria will be synced: \n$restrictions";
  }
  else {
    print "\n";
  }

  // execute the query
  $results = chado_query($query, $sql_args);

  // Iterate through features that need to be synced
  $count = $results->rowCount();
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }

  print "\n$count $base_table records found.\n";

  $i = 0;
  $transaction = db_transaction();
  try {
    foreach ($results as $record) {

      print "\nLoading $base_table " . ($i + 1) . " of $count ($base_table_id=".$record->{$base_table_id}.")...";

      // update the job status every 1% features
      if ($job_id and $i % $interval == 0) {
        $percent = sprintf("%.2f", ($i / $count) * 100);
        print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\n";
        tripal_set_job_progress($job_id, intval(($i/$count)*100));
      }

      // Check if it is in the chado linking table (ie: check to see if it is already linked to a node)
      $result = db_select('chado_'.$base_table, 'lnk')
        ->fields('lnk',array('nid'))
        ->condition($base_table_id, $record->{$base_table_id}, '=')
        ->execute()
        ->fetchObject();

      if (!empty($result)) {
        print " Previously Sync'd";
      }
      else {

        // Create generic new node
        $new_node = new stdClass();
        $new_node->type = 'chado_' . $base_table;
        $new_node->uid = $user->uid;
        $new_node->{$base_table_id} = $record->{$base_table_id};
        $new_node->$base_table = $record;

        // TODO: should we get rid of this hook and use hook_node_presave() instead?
        // allow base module to set additional fields as needed
        $hook_create_new_node = 'chado_' . $base_table . '_chado_node_sync_create_new_node';
        if (function_exists($hook_create_new_node)) {
          $new_node = call_user_func($hook_create_new_node, $new_node, $record);
        }

        // Validate and Save New Node
        $form = array();
        $form_state = array();
        node_validate($new_node, $form, $form_state);
        if (!form_get_errors()) {
          $node = node_submit($new_node);
          node_save($node);
          print " Node Created (nid=".$node->nid.")";
        }
        else {
          watchdog('trp-fsync', "Failed to insert $base_table: %title", array('%title' => $new_node->title), WATCHDOG_ERROR);
        }
      }
      $i++;
    }

    print "\n\nComplete!\n";
  }
  catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    watchdog_exception('trp-fsync', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }

}

/**
 * This function will delete Drupal nodes for any sync'ed table (e.g.
 * feature, organism, analysis, stock, library) if the chado record has been
 * deleted or the entry in the chado_[table] table has been removed.
 *
 * @param $table
 *   The name of the table that corresonds to the node type we want to clean up.
 * @param $job_id
 *   This should be the job id from the Tripal jobs system.  This function
 *   will update the job status using the provided job ID.
 *
 * @ingroup tripal_chado_node_api
 */
function chado_cleanup_orphaned_nodes($table, $job_id = NULL) {
  $count = 0;

  // build the SQL statments needed to check if nodes point to valid analyses
  $dsql = "SELECT * FROM {node} WHERE type = 'chado_" . $table . "' order by nid";
  $nsql = "SELECT * FROM {node} WHERE nid = :nid";
  $csql = "SELECT * FROM {chado_" . $table . "} WHERE nid = :nid ";
  $clsql= "SELECT * FROM {chado_" . $table . "}";
  $lsql = "SELECT * FROM {" . $table . "} where " . $table . "_id = :" . $table . "_id ";

  // load into nodes array
  print "Getting nodes\n";
  $nodes = array();
  $res = db_query($dsql);
  foreach ($res as $node) {
    $nodes[$count] = $node;
    $count++;
  }

  // load the chado_$table into an array
  print "Getting chado_$table\n";
  $cnodes = array();
  $res = db_query($clsql);
  foreach ($res as $node) {
    $cnodes[$count] = $node;
    $count++;
  }
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }

  // iterate through all of the chado_$table entries and remove those
  // that don't have a node or don't have a $table record in chado.libary
  print "Verifying all chado_$table Entries\n";
  $deleted = 0;
  foreach ($cnodes as $nid) {

    // update the job status every 1% analyses
    if ($job_id and $i % $interval == 0) {
      tripal_set_job_progress($job_id, intval(($i / $count) * 100));
    }

    // see if the node exits, if not remove the entry from the chado_$table table
    $results = db_query($nsql, array(':nid' => $nid->nid));
    $node = $results->fetchObject();
    if (!$node) {
      $deleted++;
      db_query("DELETE FROM {chado_" . $table . "} WHERE nid = :nid", array(':nid' => $nid->nid));
      $message = "chado_$table missing node.... DELETING: $nid->nid";
      watchdog('tripal_core', $message, array(), WATCHDOG_WARNING);
    }

    // see if the record in chado exist, if not remove the entry from the chado_$table
    $table_id = $table . "_id";
    $results = chado_query($lsql, array(":" . $table . "_id" => $nid->$table_id));
    $record = $results->fetchObject();
    if (!$record) {
      $deleted++;
      $sql = "DELETE FROM {chado_" . $table . "} WHERE " . $table . "_id = :" . $table . "_id";
      db_query($sql, array(":" . $table . "_id" => $nid->$table_id));
      $message = "chado_$table missing $table.... DELETING entry.";
      watchdog('tripal_core', $message, array(), WATCHDOG_WARNING);
    }
    $i++;
  }
  print "\t$deleted chado_$table entries missing either a node or chado entry.\n";

  // iterate through all of the nodes and delete those that don't
  // have a corresponding entry in chado_$table
  $deleted = 0;
  foreach ($nodes as $node) {

    // update the job status every 1% libraries
    if ($job_id and $i % $interval == 0) {
      tripal_set_job_progress($job_id, intval(($i / $count) * 100));
    }

    // check to see if the node has a corresponding entry
    // in the chado_$table table. If not then delete the node.
    $results = db_query($csql, array(":nid" => $node->nid));
    $link = $results->fetchObject();
    if (!$link) {
      if (node_access('delete', $node)) {
        $deleted++;
        $message = "Node missing in chado_$table table.... DELETING node $node->nid";
        watchdog("tripal_core", $message, array(), WATCHDOG_WARNING);
        node_delete($node->nid);
      }
      else {
        $message = "Node missing in chado_$table table.... but cannot delete due to improper permissions (node $node->nid)";
        watchdog("tripal_core", $message, array(), WATCHDOG_WARNING);
      }
    }
    $i++;
  }
  print "\t$deleted nodes did not have corresponding chado_$table entries.\n";

  return '';
}

/**
 * Create New Node
 *
 * Note: For your own module, replace hook in the function name with the machine-name of
 * your chado node type (ie: chado_feature).
 *
 * @param $new_node:
 *   a basic new node object
 * @param $record:
 *   the record object from chado specifying the biological data for this node
 *
 * @return
 *   A node object containing all the fields necessary to create a new node during sync
 *
 * @ingroup tripal_chado_node_api
 */
function hook_chado_node_sync_create_new_node($new_node, $record) {

  // Add relevant chado details to the new node object
  // This really only needs to be the fields from the node used during node creation
  // including values used to generate the title, etc.
  // All additional chado data will be added via nodetype_load when the node is later used
  $new_node->uniquename = $record->uniquename;

  return $new_node;
}

/**
 * Alter the sync form (optional)
 *
 * This might be necessary if you need additional filtering options for choosing which
 * chado records to sync or even if you just want to further customize the help text
 * provided by the form.
 *
 * Note: For your own module, replace hook in the function name with the machine-name of
 * your chado node type (ie: chado_feature).
 *
 * @ingroup tripal_chado_node_api
 */
function hook_chado_node_sync_form($form, $form_state) {

  // Change or add to the form array as needed
  // Any changes should be made in accordance with the Drupal Form API

  return $form;
}

/**
 * Bypass chado node api sync form submit (optional). Allows you to use this function
 * as your own submit.
 *
 * This might be necessary if you want to add additional arguements to the tripal job or
 * to call your own sync'ing function if the generic chado_node_sync_records() is not
 * sufficient.
 *
 * Note: For your own module, replace hook in the function name with the machine-name of
 * your chado node type (ie: chado_feature).
 *
 * @ingroup tripal_chado_node_api
 */
function hook_chado_node_sync_form_submit ($form, $form_state) {

  global $user;

  $job_args = array(
    $base_table,      // the base chado table (ie: feature)
    $max_sync,        // the maximum number of records to sync or FALSE for sync all that match
    $organism_id,     // the organism_id to restrict records to or FALSE if not to restrict by organism_id
    $types            // A string with the cvterm.name of the types to restrict to separated by |||
  );

  // You should register a tripal job
  tripal_add_job(
    $title,                                   // the title of the job -be descriptive
    $module,                                  // the name of your module
    'chado_node_sync_records',    // the chado node api sync function
    $job_args,                                // an array with the arguments to pass to the above function
    $user->uid                                // the user who submitted the job
  );

}


/**
 * Alter the query for the chado database which gets the chado records to be sync'd (optional)
 *
 * This might be necessary if you need fields from other chado tables to create your node
 * or if your chado node type only supports a subset of a given table (ie: a germplasm node
 * type might only support node creation for cerain types of stock records in which case
 * you would need to filter the results to only those types).
 *
 * Note: For your own module, replace hook in the function name with the machine-name of
 * your chado node type (ie: chado_feature).
 *
 * @param $select:
 *   An array of select clauses
 * @param $joins:
 *   An array of joins (ie: a single join could be 'LEFT JOIN {chadotable} alias ON base.id=alias.id')
 * @param $where_clauses:
 *   An array of where clauses which will all be AND'ed together. Use :placeholders for values.
 * @param $where_args:
 *   An associative array of arguments to be subbed in to the where clause where the
 *   key = :placeholder and the value is the actual argument to be subbed in.
 *
 * @ingroup tripal_chado_node_api
 */
function hook_chado_node_sync_select_query (&$select, &$joins, &$where_clauses, &$where_args) {

  // You can add fields to be selected
  $select[] = 'example.myfavfield';

  // Or joins to important tables
  $joins[] = 'LEFT JOIN {exampleprop} PROP ON PROP.example_id=EXAMPLE.example_id';

  // Or filter the query using where clauses
  $where_clauses[] = 'example.myfavfield = :favvalue';
  $where_args[':favvalue'] = 'awesome-ness';
}