<?php

/**
 * @file
 * Functions for managing Drupal Nodes corresponding to chado content
 *

/**
 * Sync'ing chado records with Drupal by creating nodes
 *
 * How to Use:
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
      'page arguments' => array('tripal_core_chado_node_sync_form', $module_name, $linking_table),
      'access arguments' => array('administer tripal examples'),
      'type' => MENU_LOCAL_TASK,
      'weight' => 0
    );

    return $items;
  }

  modulename_node_info() {
    return array(
      'chado_example' => array(
        'name' => t('example'),
        'base' => 'chado_example',
        'description' => t('A Chado example is a collection of material that can be sampled and have experiments performed on it.'),
        'has_title' => TRUE,
        'has_body' => FALSE,

        // this is what differs from the regular Drupal-documented hook_node_info()
        'chado_node_api' => array(
          'base_table' => 'example',            // the name of the chado base table
          'hook_prefix' => 'chado_example',     // usually the name of the node type
          'title' => array(
            'singular' => t('Example'),         // Singular human-readable title
            'plural' => t('Examples')           // Plural human-readable title
          ),
          'select_by' => array(                 // foreign keys present in your table
            'type_id' => TRUE,                  // TRUE if there is an example.type_id field
            'organism_id' => TRUE               // TRUE if there is an example.organism_id field
          ),
        )
      ),
    );
  }

  // Create New Node
  // @param $new_node: a basic new node object
  // @param $record: the record object from chado specifying the biological data for this node
  function chado_example_chado_node_sync_create_new_node($new_node, $record) {

    // Add relevant chado details to the new node object
    // This really only needs to be the fields from the node used during node creation
    // including values used to generate the title, etc.
    // All additional chado data will be added via nodetype_load when the node is later used
    $new_node->uniquename = $record->uniquename;

    return $new_node;
  }

  // Alter the sync form (optional)
  function chado_example_chado_node_sync_form($form, $form_state) {

    // Change or add to the form array as needed
    // Any changes should be made in accordance with the Drupal Form API

    return $form;
  }

  // Bypass chado node api sync form submit (optional)
  // Allows you to use this function as your own submit
  function chado_example_chado_node_sync_form ($form, $form_state) {

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
      'tripal_core_chado_node_sync_records',    // the chado node api sync function
      $job_args,                                // an array with the arguments to pass to the above function
      $user->uid                                // the user who submitted the job
    );

  }


  // Alter the query for the chado database which gets the chado records to be sync'd (optional)
  // @param $select: an array of select clauses
  // @param $joins: an array of joins (ie: a single join could be 'LEFT JOIN {chadotable} alias ON base.id=alias.id')
  // @param $where_clauses: an array of where clauses which will all be AND'ed together. Use :placeholders for values.
  // @param $where_args: an associative array of arguments to be subbed in to the where clause
  //           where the key = :placeholder and the value is the actual argument to be subbed in
  function chado_example_chado_node_sync_select_query (&$select, &$joins, &$where_clauses, &$where_args) {

    // You can add fields to be selected
    $select[] = 'example.myfavfield';

    // Or joins to important tables
    $joins[] = 'LEFT JOIN {exampleprop} PROP ON PROP.example_id=EXAMPLE.example_id';

    // Or filter the query using where clauses
    $where_clauses[] = 'example.myfavfield = :favvalue';
    $where_args[':favvalue'] = 'awesome-ness';
  }

 * @endcode
 */

/**
 * Generic Sync Form
 */
function tripal_core_chado_node_sync_form($form, &$form_state) {
  $form = array();

  if (isset($form_state['build_info']['args'][0])) {
    $module = $form_state['build_info']['args'][0];
    $linking_table = $form_state['build_info']['args'][1];
    $node_info = call_user_func($module . '_node_info');
    $args = $node_info[$linking_table]['chado_node_api'];
    $form_state['chado_node_api'] = $args;
  }

  $form['description'] = array(
  '#type' => 'item',
  '#value' => t("%title_plural of the types listed ".
     "below in the %title_singular Types box will be synced (leave blank to sync all types). You may limit the ".
     "%title_plural to be synced by a specific organism. Depending on the ".
     "number of %title_plural in the chado database this may take a long ".
     "time to complete. ",
     array(
      '%title_singular' => $args['title']['singular'],
      '%title_plural' => $args['title']['plural']
    )),
  );

  if ($args['select_by']['type_id']) {
    $form['type_ids'] = array(
      '#title'       => t('%title_singular Types',
         array(
          '%title_singular' => $args['title']['singular'],
          '%title_plural' => $args['title']['plural']
      )),
      '#type'        => 'textarea',
      '#description' => t("Enter the names of the %title_singular types to sync. " .
         "Leave blank to sync all %title_plural. Pages for these %title_singular ".
         "types will be created automatically for %title_plural that exist in the ".
         "chado database.  The names listed here should be spearated by ".
         "spaces or entered separately on new lines. The names must match ".
         "exactly (spelling and case) with terms in the ontologies",
         array(
          '%title_singular' => $args['title']['singular'],
          '%title_plural' => $args['title']['plural']
        )),
      '#default_value' => (isset($form_state['values']['type_id'])) ? $form_state['values']['type_id'] : '',
    );
  }

  // get the list of organisms
  if ($args['select_by']['organism_id']) {
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $orgs = tripal_organism_get_synced();
    $organisms[] = '';
    foreach ($orgs as $organism) {
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
    }
    $form['organism_id'] = array(
      '#title'       => t('Organism'),
      '#type'        => t('select'),
      '#description' => t("Choose the organism for which %title_plural types set above will
        be synced. Only organisms which also have been synced will appear in this list.",
         array(
          '%title_singular' => $args['title']['singular'],
          '%title_plural' => $args['title']['plural']
        )),
      '#options'     => $organisms,
      '#default_value' => (isset($form_state['values']['organism_id'])) ? $form_state['values']['organism_id'] : 0,
    );
  }

  $form['max_sync'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of records to Sync'),
    '#description' => t('Leave this field empty to sync all records, regardless of number'),
    '#default_value' => (isset($form_state['values']['max_sync'])) ? $form_state['values']['max_sync'] : '',
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Sync all ' . $args['title']['plural']),
    '#weight' => 3,
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
 */
function tripal_core_chado_node_sync_form_submit($form, $form_state) {

  global $user;

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
  if (isset($form_state['values']['type_ids'])) {
    $types = preg_replace("/[\s\n\r]+/", '|||', $form_state['values']['type_ids']);
  }
  else {
    $types = '';
  }

  // Job Arguments
  $job_args = array(
    'base_table' => $base_table,
    'max_sync' => (!empty($form_state['values']['max_sync'])) ? $form_state['values']['max_sync'] : FALSE,
    'organism_id' => FALSE,
    'types' => $types,
  );

  // Create Job based on presence of organism
  if (isset($form_state['values']['organism_id'])) {
    $organism_id = $form_state['values']['organism_id'];
    $organism = tripal_core_chado_select('organism', array('genus', 'species'), array('organism_id' => $organism_id));
    $title = "Sync all " . $args['title']['plural'] . " for " . $organism[0]->genus . " " . $organism[0]->species;

    $job_args['organism_id'] = $organism_id;
    tripal_add_job($title, $module, 'tripal_core_chado_node_sync_records', $job_args, $user->uid);
  }
  else {
    $title = t('Sync all ' . $args['title']['plural'] . ' for all synced organisms');
    tripal_add_job($title, $module, 'tripal_core_chado_node_sync_records', $job_args, $user->uid);
  }
}

/**
 * Actual Sync Function. Works on a group of records
 */
function tripal_core_chado_node_sync_records($base_table, $max_sync = FALSE, $organism_id = FALSE, $types = '') {

  global $user;
  $base_table_id = $base_table . '_id';

  print "\nSync'ing records from $base_table restricted to\n";

  // START BUILDING QUERY TO GET ALL RECORD FROM BASE TABLE THAT MATCH
  $select = array("$base_table.*");
  $joins = array();
  $where_clauses = array();
  $where_args = array();

  // If types are supplied then handle them
  if (!empty($types)) {
    $types = explode('|||',$types);
    print "  Types: " . implode(', ',$types) . "\n";

    $select[] = 'cvterm.name as cvtname';
    $joins[] = "LEFT JOIN {cvterm} cvterm ON $base_table.type_id = cvterm.cvterm_id";
    if (sizeof($types) == 1) {
      $where_clauses[] = 'cvterm.name = :type_name';
      $where_args[':type_name'] = $types[0];
    } elseif (sizeof($types) > 1) {
      $where_clauses[] = "cvterm.name IN ('" . implode("', '", $types) . "')";
    }
  }

  // If Organism is supplied
  if ($organism_id) {
    print "  Organism ID: $organism_id\n";

    $select[] = 'organism.*';
    $joins[] = "LEFT JOIN {organism} organism ON organism.organism_id = $base_table.organism_id";
    $where_clauses[] = 'organism.organism_id = :organism_id';
    $where_args[':organism_id'] = $organism_id;
  }

  // Allow module to add to query
  $hook_query_alter = 'chado_' . $base_table . '_chado_node_sync_select_query';
  if (function_exists($hook_query_alter)) {
    call_user_func($hook_query_alter, $select, $joins, $where_clauses, $where_args);
  }

  // Build Query
  $query = "SELECT " . implode(', ',$select)
    . ' FROM {' . $base_table . '} ' . $base_table . ' '
    . implode(' ', $joins)
    . " WHERE " . implode(' AND ', $where_clauses)
    . " ORDER BY " . $base_table_id;

  // If Maximum number to Sync is supplied
  if ($max_sync) {
    $query .= " LIMIT $max_sync";
  }

  print "\nQuery: " . preg_replace(array("/FROM/","/LEFT/","/WHERE/"), array("\nFROM","\nLEFT","\nWHERE"), $query) . "\n";

  print "Executing Query...\n";
  $results = chado_query($query, $where_args);

  // Iterate through features that need to be synced
  $count = $results->rowCount();
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }

  print "\n$count $base_table records found.\nLoading...\n";

  $i = 0;
  $transaction = db_transaction();
  try {
    foreach ($results as $record) {

      print "\nLoading $base_table $i of $count ($base_table_id=".$record->{$base_table_id}.")...";

      // update the job status every 1% features
      if ($job_id and $i % $interval == 0) {
        $percent = sprintf("%.2f", ($i / $count) * 100);
        print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\n";
        tripal_job_set_progress($job_id, intval(($i/$count)*100));
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

        // allow base module to set additional fields as needed
        $hook_create_new_node = 'chado_' . $base_table . '_chado_node_sync_create_new_node';
        $new_node = call_user_func($hook_create_new_node, $new_node, $record);

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