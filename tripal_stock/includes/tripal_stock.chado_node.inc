<?php
/**
 * @file Stock Node Functionality
 */

/**
 * Implements hook_node_info().
 * Registers a stock node type
 *
 * @return
 *   An array describing various details of the node
 *
 * @ingroup tripal_stock
 */
function tripal_stock_node_info() {
  return array(
    'chado_stock' => array(
      'name'        => t('Stock'),
      'base'        => 'chado_stock',
      'description' => t('A Chado Stock is a collection of material that can be sampled and have experiments performed on it.'),
      'has_title'   => TRUE,
      'locked'      => TRUE,
      'chado_node_api' => array(
        'base_table' => 'stock',
        'hook_prefix' => 'chado_stock',
        'record_type_title' => array(
          'singular' => t('Stock'),
          'plural' => t('Stocks')
        ),
        'sync_filters' => array(
          'type_id' => TRUE,
          'organism_id' => TRUE
        ),
      )
    ),
  );
}

/**
 * Implements hook_load().
 * Prepares the chado_stock node
 *
 * @param $node
 *   The basic node containing all variables common to all nodes
 *
 * @return
 *   A stock node containing all the variables from the basic node and all stock specific variables
 *
 * D7 @todo: Make optimizations to take advantage of $nodes
 *
 * @ingroup tripal_stock
 */
function chado_stock_load($nodes) {

  $new_nodes = array();
  foreach ($nodes as $nid => $node) {
    
    // get the stock details from chado
    $stock_id = chado_get_id_from_nid('stock', $nid);
    
    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$stock_id) {
      continue;
    }

    // build the variable with all the stock details
    $values = array('stock_id' => $stock_id);
    $stock = chado_generate_var('stock', $values);
    
    // add in the uniquename and the description as these are both text fields
    $stock = chado_expand_var($stock, 'field', 'stock.uniquename');
    $stock = chado_expand_var($stock, 'field', 'stock.description');

    // by default, the titles are saved using the unique constraint.  We will
    // keep it the same, but remove the duplicate name if the unique name and name
    // are identical
    $title_type = variable_get('chado_stock_title', 'unique_constraint');
    if($title_type == 'unique_constraint') {
      if (strcmp($stock->name, $stock->uniquename)==0) {
        $node->title = $stock->name . " (" . $stock->type_id->name . ") " . $stock->organism_id->genus . " " . $stock->organism_id->species ;
      }
      // in previous version of Tripal, the stock title was simply the unique name.
      // so, we recreate the title just to be sure all of our stock pages are consistent
      else {
        $node->title = $stock->name . ", " . $stock->uniquename . " (" . $stock->type_id->name . ") " . $stock->organism_id->genus . " " . $stock->organism_id->species ;
      }
    }
    // set the title to be the stock name or uniquename as configured
    if($title_type == 'stock_name') {
      $node->title = $stock->name;
    }
    if($title_type == 'stock_unique_name') {
      $node->title = $stock->uniquename;
    }

    // add this to the node
    $node->stock = $stock;
    $new_nodes[$nid] = $node;

  }

  return $new_nodes;
}

/**
 * Implements hook_form().
 * Creates the main Add/Edit/Delete Form for chado stocks
 *
 * Parts to be added by this form
 *     name,
 *     uniquename,
 *     description,
 *     type => select from cvterm with key cvterm_id,
 *     organism => select from available with key organism_id
 *     main_db_reference => accession, version, description, db_name(select from dropdown)
 *
 * @param $node
 *   An empty node object on insert OR the current stock node object on update
 * @param $form_state
 *   The current state of the form
 *
 * @return
 *   A description of the form to be rendered by drupal_get_form()
 *
 * @ingroup tripal_stock
 */
function chado_stock_form($node, $form_state) {

  /* I don't think we need this... commenting out but leaving just in case
  // If existing stock then expand all fields needed using the chado API
  if (isset($node->nid)) {
    $fields_needed = array('stock.uniquename', 'stock.name', 'stock.stock_id', 'stock.type_id', 'stock.organism_id', 'stock.description', 'stock.dbxref_id', 'dbxref.accession', 'dbxref.description', 'dbxref.db_id', 'db.db_id');
    foreach ($fields_needed as $field_name) {
      // Check to see if it's excluded and expand it if so
      if (isset($node->expandable_fields)) {
        if (in_array($field_name, $node->expandable_fields)) {
          $node = chado_expand_var($node, 'field', $field_name);
        }
      }
    }
  }
  */
  //TODO: @lacey can you take a look at the above code?

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing stock
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $sname = '';
  $uniquename = '';
  $stock_id = 0;
  $type_id = 0;
  $organism_id = 0;
  $sdescription = '';
  $dbxref_accession = '';
  $dbxref_description = '';
  $dbxref_database = 0;

  // 1) if we are editing an existing node then the stock is already part of the node
  if (property_exists($node, 'stock')) {
    $sname = $node->stock->name;
    $uniquename = $node->stock->uniquename;
    $stock_id = $node->stock->stock_id;
    $type_id = $node->stock->type_id->cvterm_id;
    $organism_id = $node->stock->organism_id->organism_id;
    $sdescription = $node->stock->description;
    if (isset($node->stock->dbxref_id->db_id)) {
      $dbxref_accession = $node->stock->dbxref_id->accession;
      $dbxref_description = $node->stock->dbxref_id->description;
      $dbxref_database = $node->stock->dbxref_id->db_id->db_id;
    }
  }

  // 2) if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $sname = $form_state['values']['sname'];
    $uniquename = $form_state['values']['uniquename'];
    $stock_id = $form_state['values']['stock_id'];
    $type_id = $form_state['values']['type_id'];
    $organism_id = $form_state['values']['organism_id'];
    $sdescription = $form_state['values']['description'];
    $dbxref_accession = $form_state['values']['accession'];
    $dbxref_description = $form_state['values']['db_description'];
    $dbxref_database = $form_state['values']['database'];
  }

  // 3) if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $sname = $form_state['input']['sname'];
    $uniquename = $form_state['input']['uniquename'];
    $stock_id = $form_state['input']['stock_id'];
    $type_id = $form_state['input']['type_id'];
    $organism_id = $form_state['input']['organism_id'];
    $sdescription = $form_state['input']['description'];
    $dbxref_accession = $form_state['input']['accession'];
    $dbxref_description = $form_state['input']['db_description'];
    $dbxref_database = $form_state['input']['database'];
  }
  
  $form['names'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stock Name')
  );

  $form['names']['sname'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $sname,
    '#required'       => TRUE
  );

  $form['names']['uniquename'] = array(
    '#type' => 'textfield',
    '#title' => t('Unique Name'),
    '#default_value' => $uniquename,
    '#required'       => TRUE
  );

  $form['names']['stock_id'] = array(
    '#type' => 'hidden',
    '#value' => $stock_id,
  );

  $form['details'] = array(
    '#type' => 'fieldset',
    '#title' =>  t('Stock Details')
  );

  $type_options = tripal_get_cvterm_default_select_options('stock', 'type_id', 'stock types');
  $type_options[0] = 'Select a Type';
  
  $form['details']['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Type of Stock'),
    '#options' => $type_options,
    '#default_value' => $type_id,
    '#required'    => TRUE,
  );


  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $org_rset = chado_query($sql);
  $organisms = array();
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['details']['organism_id'] = array(
    '#type' => 'select',
    '#title' => t('Source Organism for stock'),
    '#default_value' => $organism_id,
    '#options' => $organisms,
    '#required'    => TRUE
  );


  $form['details']['stock_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Notes'),
    '#default_value' => $sdescription,
    '#description' => t('Briefly enter any notes on the above stock. This should not include phenotypes or genotypes.'),
  );

  $form['database_reference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stock Database Reference')
  );

  $form['database_reference']['accession'] = array(
    '#type' => 'textfield',
    '#title' => t('Accession'),
    '#default_value' => $dbxref_accession,
  );

  $form['database_reference']['db_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description of Database Reference'),
    '#default_value' => $dbxref_description,
    '#description' => t('Optionally enter a description about the database accession.')
  );

  $db_options = tripal_db_get_db_options();
  $db_options[0] = 'Select a Database';
  $form['database_reference']['database'] = array(
    '#type' => 'select',
    '#title' => t('Database'),
    '#options' => $db_options,
    '#default_value' => $dbxref_database
  );

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('stockprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'stockprop',
    'chado_id' => $stock_id,
    'cv_id' => $cv_id
  );
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'stock_dbxref',
    'base_foreign_key' => 'stock_id',
    'base_key_value' => $stock_id
  );
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  $relationship_cv = tripal_get_default_cv('stock_relationship', 'type_id');
  $cv_id = $relationship_cv ? $relationship_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'stock_relationship',
    'base_table' => 'stock',
    'base_foreign_key' => 'stock_id',
    'base_key_value' => $stock_id,
    'nodetype' => 'stock',
    'cv_id' => $cv_id
  );
  chado_add_node_form_relationships($form, $form_state, $details);

  return $form;
}

/**
 * Implements hook_validate().
 * Validate the input from the chado_stock node form
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 * @param $form
 *   A description of the form to be rendered by drupal_get_form()
 *
 * @ingroup tripal_stock
 */
function chado_stock_validate(&$node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a stock_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'stock_id') and $node->stock_id != 0) {
    return;
  }

  // remove surrounding whitespace
  $node->uniquename = trim($node->uniquename);
  $node->sname = trim($node->sname);
  $node->stock_description = trim($node->stock_description);
  $node->accession = trim($node->accession);
  $node->db_description = trim($node->db_description);
  
  $int_in_chado_sql = "SELECT count(*) as count FROM {:table} WHERE :column = :value";
  $string_in_chado_sql = "SELECT count(*) as count FROM {:table} WHERE :column = :value";

  // if this is an update, we want to make sure that a different stock for
  // the organism doesn't already have this uniquename. We don't want to give
  // two sequences the same uniquename
  if (property_exists($node, 'nid')) {
    $sql = "
      SELECT *
      FROM {stock} S
        INNER JOIN {cvterm} CVT ON S.type_id = CVT.cvterm_id
      WHERE
        uniquename = :uname AND organism_id = :organism_id AND
        CVT.name = :cvtname AND NOT stock_id = :stock_id
    ";
    $result = chado_query($sql, array(':uname' => $node->uniquename,
      ':organism_id' => $node->organism_id, ':cvtname' => $node->type_id,
      ':stock_id' => $node->stock_id))->fetchObject();
    if ($result) {
      form_set_error('uniquename', t("Stock update cannot proceed. The stock name '$node->uniquename' is not unique for this organism. Please provide a unique name for this stock."));
    }
  }

  // if this is an insert then we just need to make sure this name doesn't
  // already exist for this organism if it does then we need to throw an error
  else {
    $sql = "
      SELECT *
      FROM {Stock} S
        INNER JOIN {cvterm} CVT ON S.type_id = CVT.cvterm_id
      WHERE uniquename = :uname AND organism_id = :organism_id AND CVT.name = :cvtname";
    $result = chado_query($sql, array(':uname' => $node->uniquename,
      ':organism_id' => $node->organism_id, ':cvtname' => $node->type_id))->fetchObject();
    if ($result) {
      form_set_error('uniquename', t("Stock insert cannot proceed. The stock name '$node->uniquename' already exists for this organism. Please provide a unique name for this stock."));
    }
  }


  // Check Type of Stock is valid cvterm_id in chado ( $form['values']['details']['type_id'] )
  if ($node->type_id == 0) {
    form_set_error('type_id', 'Please select a type of stock.');
  }
  else {
    $replace = array(':table' => 'cvterm', ':column' => 'cvterm_id');
    $new_sql = str_replace(array_keys($replace),$replace,$int_in_chado_sql);
    $num_rows = chado_query($new_sql, array(':value' => $node->type_id))->fetchObject();
    if ( $num_rows->count != 1) {
      form_set_error('type_id', "The type you selected is not valid. Please choose another one. (CODE:$num_rows)"); }
  }

  // Check Source Organism is valid organism_id in chado ( $form['values']['details']['organism_id'] )
  if ( $node->organism_id == 0) {
    form_set_error('organism_id', 'Please select a source organism for this stock');
  }
  else {
    $replace = array(':table' => 'organism', ':column' => 'organism_id');
    $new_sql = str_replace(array_keys($replace),$replace,$int_in_chado_sql);
    $num_rows = chado_query($new_sql, array(':value' => $node->organism_id))->fetchObject();
    if ( $num_rows->count != 1 ) {
      form_set_error('organism_id', "The organism you selected is not valid. Please choose another one. (CODE:$num_rows)"); }
  }

  // Check if Accession also database
  if ($node->accession != '') {
    if ($node->database == 0) {
      // there is an accession but no database selected
      form_set_error('database', 'You need to enter both a database and an accession for that database in order to add a database reference.');
    }
  }
  else {
    if ($node->database > 0) {
      // there is a database selected but no accession
      form_set_error('accession', 'You need to enter both a database and an accession for that database in order to add a database reference.');
    }
  }

  // Check database is valid db_id in chado ( $form['values']['database_reference']['database'] )
  if ( $node->database > 0) {
    $replace = array(':table' => 'db', ':column' => 'db_id');
    $new_sql = str_replace(array_keys($replace),$replace,$int_in_chado_sql);
    $num_rows = chado_query($new_sql, array(':value' => $node->database))->fetchObject();
    if ($num_rows->count != 1) {
      form_set_error('database', 'The database you selected is not valid. Please choose another one.'); }
  }
}

/**
 * Implements hook_insert().
 * Inserts data from chado_stock_form() into drupal and chado
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 *
 * @return
 *   TRUE if the node was successfully inserted into drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_stock
 */
function chado_stock_insert($node) {
  
  $node->uniquename   = trim($node->uniquename);
  $node->sname        = trim($node->sname);

  // if there is an stock_id in the $node object then this must be a sync so
  // we can skip adding the stock to chado as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'stock_id')) {
  
    // before we can add the stock, we must add the dbxref if one has been
    // provided by the user.
    $dbxref_status = 0;
    if (!empty($node->accession) ) {
      if (!empty($node->database) ) {
        $values = array(
          'db_id' => $node->database,
          'accession' => $node->accession,
        );
        if (!tripal_core_chado_select('dbxref', array('dbxref_id'), $values)) {
          $values['description'] = $node->db_description;
          $values['version'] = '1';
          $dbxref_status = chado_insert_record('dbxref', $values);
          if (!$dbxref_status) {
            drupal_set_message(t('Unable to add database reference to this stock.'), 'warning');
            tripal_report_error('tripal_stock', TRIPAL_WARNING,
              'Insert Stock: Unable to create dbxref where values:%values',
              array('%values' => print_r($values, TRUE)));
          }
        }
        else {
          $dbxref_status = 1;
        }
      }
    }

    // create stock including the dbxref
    $stock = '';
    if ($dbxref_status) {
      $values = array(
        'dbxref_id' => array(
          'db_id' => $node->database,
          'accession' => $node->accession
        ),
        'organism_id' => $node->organism_id,
        'name' => $node->sname,
        'uniquename' => $node->uniquename,
        'description' => $node->stock_description,
        'type_id' => $node->type_id
      );
      $stock = chado_insert_record('stock', $values);
    }
    // create a stock without a dbxref
    else {
      $values = array(
        'organism_id' => $node->organism_id,
        'name'        => $node->sname,
        'uniquename'  => $node->uniquename,
        'description' => $node->stock_description,
        'type_id'     => $node->type_id
      );
      $stock = chado_insert_record('stock', $values);
    }

    if (is_array($stock)) {
      $stock_added = TRUE;
      $stock_id = $stock['stock_id'];
    }
    else {
      $stock_added = FALSE;
    }

    if ($stock_added) {

      // Now add properties
      $details = array(
        'property_table' => 'stockprop',
        'base_table' => 'stock',
        'foreignkey_name' => 'stock_id',
        'foreignkey_value' => $stock_id
      );
      chado_update_node_form_properties($node, $details);

      // Now add the additional references
      $details = array(
        'linking_table' => 'stock_dbxref',
        'foreignkey_name' => 'stock_id',
        'foreignkey_value' => $stock_id
      );
      chado_update_node_form_dbxrefs($node, $details);

      // Now add in relationships
      $details = array(
        'relationship_table' => 'stock_relationship',
        'foreignkey_value' => $node->stock_id
      );
      chado_update_node_form_relationships($node, $details);
    }
  } //end of adding stock to chado
  else {
    // stock already exists since this is a sync
    $stock_added = TRUE;
    $stock['stock_id'] = $node->stock_id;
  }

  // if the stock creation was succesful then add the URL and the entry in the
  // chado_stock table
  if ($stock_added) {

    // add the entry to the chado_stock table
    db_insert('chado_stock')->fields(array(
      'nid' => (int) $node->nid,
      'vid' => (int) $node->vid,
      'stock_id' => (int) $stock['stock_id']
    ))->execute();

  }
  else {
    drupal_set_message(t('Error during stock creation.'), 'error');
    tripal_report_error('tripal_stock', TRIPAL_WARNING,
      'Insert Stock: Unable to create stock where values:%values',
      array('%values' => print_r($values, TRUE)));
    return FALSE;
  }
}

/**
 * Implements hook_update().
 * Handles Editing/Updating of main stock info
 *
 * NOTE: Currently just writes over all old data
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 *
 * @return
 *   TRUE if the node was successfully updated in drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_stock
 */
function chado_stock_update($node) {
  
  $node->uniquename   = trim($node->uniquename);
  $node->sname        = trim($node->sname);

  if ($node->revision) {
    // there is no way to handle revisions in Chado but leave
    // this here just to make not we've addressed it.
  }

  //update dbxref
  $dbxref_status = NULL;
  $dbxref_present = FALSE;
  if ($node->database) {
    $dbxref_present = TRUE;
    if ($node->accession) {
      $dbxref_mode = '';
      $stock = chado_select_record(
        'stock',
        array('dbxref_id', 'type_id'),
        array('stock_id' => $node->stock_id)
      );

      if ($stock[0]->dbxref_id) {
        $values = array(
          'db_id' => $node->database,
          'accession' => $node->accession,
          'description' => $node->db_description
        );
        $dbxref_status = chado_update_record(
          'dbxref',
          array('dbxref_id' => $stock[0]->dbxref_id),
          $values
        );
        $dbxref_mode = 'Update';
      }
      else {
        if ($stock[0]->type_id) {
          //create the dbxref
          //used the type_id as a control to check we have a stock but not a dbxref
          $values = array(
            'db_id' => $node->database,
            'accession' => $node->accession,
            'description' => $node->db_description,
            'version' => '1',
          );
          $dbxref_status = chado_insert_record(
            'dbxref',
            $values
          );
          $dbxref_mode = 'Create';
        }
        else {
          drupal_set_message(t('Unable to find stock to Update'), 'error');
          tripal_report_error('tripal_stock', TRIPAL_ERROR,
            'Stock Update: Unable to find stock to update using values: %values',
            array('%values', print_r($values, TRUE))
          );
          return FALSE;
        }
      }
    }

    if (!$dbxref_status) {
      tripal_report_error('tripal_stock', TRIPAL_WARNING,
        'Stock Update: Unable to %mode main stock dbxref with values: %values',
        array('%values' => print_r($values, TRUE), '%mode' => $dbxref_mode));
    }
  }

  //can't change stock id which is all thats stored in drupal thus only update chado
  $update_values = array(
    'organism_id' => $node->organism_id,
    'name' => $node->sname,
    'uniquename' => $node->uniquename,
    'description' => $node->stock_description,
    'type_id' => $node->type_id,
  );
  if ($dbxref_present) {
    if ($dbxref_status) {
      $update_values['dbxref_id'] = array(
        'db_id' => $node->database,
        'accession' => $node->accession
      );
    }
  }
  $status = chado_update_record('stock', array('stock_id' => $node->stock_id), $update_values);


  if (!$status) {
    drupal_set_message(t('Unable to update stock'), 'error');
    tripal_report_error('tripal_stock', TRIPAL_ERROR,
      'Stock Update: Unable to update stock using match values: %mvalues and update values: %uvalues',
      array('%mvalues' => print_r(array('stock_id' => $node->stock_id), TRUE), '%uvalues' => print_r($update_values, TRUE))
    );
  }
  else {
    // set the URL for this stock page
    $values = array('stock_id' => $node->stock_id);
    $stock = chado_select_record('stock', array('*'), $values);
  }

  // now update the properties
  if ($node->stock_id > 0) {

    $details = array(
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_properties($node, $details);

  }

  // now update the additional dbxrefs
  if ($node->stock_id > 0) {
    $details = array(
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_dbxrefs($node, $details);
  }

  // now update relationships
  if ($node->stock_id > 0) {
    $details = array(
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_relationships($node, $details);
  }
}

/**
 * Implements hook_delete().
 * Handles deleting of chado_stocks
 *
 * NOTE: Currently deletes data -no undo or record-keeping functionality
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 *
 * @return
 *   TRUE if the node was successfully deleted from drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_stock
 */
function chado_stock_delete($node) {

  // Set stock in chado: is_obsolete = TRUE
  chado_query("DELETE FROM {stock} WHERE stock_id = :stock_id", array(':stock_id' => $node->stock->stock_id));

  //remove drupal node and all revisions
  db_query("DELETE FROM {chado_stock} WHERE nid = :nid", array(':nid' => $node->nid));
}

/**
 * Used by Tripal Chado Node API during sync'ing of nodes
 *
 * @ingroup tripal_stock
 */
function chado_stock_chado_node_sync_create_new_node($new_node, $record) {

  $new_node->organism_id = $record->organism_id;
  $new_node->sname = $record->name;
  $new_node->uniquename = $record->uniquename;
  $new_node->type_id = $record->type_id;

  return $new_node;
}

/**
 * Implements hook_node_presave(). Acts on all content types.
 *
 * @ingroup tripal_stock
 */
function tripal_stock_node_presave($node) {

  switch ($node->type) {
    case 'chado_stock':
      // for a form submission the fields part of the node object
      // but for a sync the fields are in an object of the node
      $organism_id = null;
      $sname = '';
      $uniquename = '';
      $type = '';
      if(property_exists($node, 'organism_id')) {
        $organism_id = $node->organism_id;
        $sname       = $node->sname;
        $uniquename  = $node->uniquename;
        $type_id     = $node->type_id;
        $values = array('cvterm_id' => $node->type_id);
        $cvterm = chado_select_record('cvterm', array('name'), $values);
        $type = $cvterm[0]->name;
      }
      else if (property_exists($node, 'stock')) {
        $organism_id = $node->stock->organism_id;
        $sname       = $node->stock->name;
        $uniquename  = $node->stock->uniquename;
        $type        = $node->stock->type_id->name;
      }
      $values = array('organism_id' => $organism_id);
      $organism = chado_select_record('organism', array('genus','species'), $values);
      $node->title = "$sname, $uniquename ($type) " . $organism[0]->genus . ' ' . $organism[0]->species;
      if ($sname == $uniquename) {
        $node->title = "$sname ($type) " . $organism[0]->genus . ' ' . $organism[0]->species;
      }
      break;
  }
}

/**
 * Implements hook_node_view(). Acts on all content types.
 *
 * @ingroup tripal_stock
 */
function tripal_stock_node_view($node, $view_mode, $langcode) {

  switch ($node->type) {
    case 'chado_stock':
      if ($view_mode == 'full') {
        $node->content['tripal_stock_base'] = array(
          '#markup' => theme('tripal_stock_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_stock_collections'] = array(
          '#markup' => theme('tripal_stock_collections', array('node' => $node)),
          '#tripal_toc_id'    => 'collections',
          '#tripal_toc_title' => 'Stock Collections',
        );
        $node->content['tripal_stock_properties'] = array(
          '#markup' => theme('tripal_stock_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_stock_references'] = array(
          '#markup' => theme('tripal_stock_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_stock_relationships'] = array(
          '#markup' => theme('tripal_stock_relationships', array('node' => $node)),
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );
        $node->content['tripal_stock_synonyms'] = array(
          '#markup' => theme('tripal_stock_synonyms', array('node' => $node)),
          '#tripal_toc_id'    => 'synonyms',
          '#tripal_toc_title' => 'Synonyms',
        );
        $node->content['tripal_stock_publications'] = array(
          '#markup' => theme('tripal_stock_publications', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_stock_teaser'] = array(
          '#markup' => theme('tripal_stock_teaser', array('node' => $node)),
        );
      }
      break;
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism_stocks'] = array(
          '#markup' => theme('tripal_organism_stocks', array('node' => $node)),
          '#tripal_toc_id'    => 'stocks',
          '#tripal_toc_title' => 'Stocks',
        );
      }
      break;
  }
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_stock
 */
function tripal_stock_node_insert($node) {

  // set the URL path after inserting.  We do it here because we do not
  // know the stock_id in the presave
  switch ($node->type) {
    case 'chado_stock':

      // remove any previous alias
      db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => "node/$node->nid"));

      // set the URL for this stock page
      $url_alias = tripal_stock_get_stock_url($node);
      $path_alias = array("source" => "node/$node->nid", "alias" => $url_alias);
      path_save($path_alias);
      break;
  }
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 *
 * @ingroup tripal_stock
 */
function tripal_stock_node_update($node) {

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_stock':
      // remove any previous alias
      db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => "node/$node->nid"));

      // set the URL for this stock page
      $url_alias = tripal_stock_get_stock_url($node);
      $path_alias = array("source" => "node/$node->nid", "alias" => $url_alias);
      path_save($path_alias);
      break;
  }
}
/**
 * Return the url alias for a stock
 *
 * @param $node
 *   A node object containing at least the stock_id and nid
 * @param $url_alias
 *   Optional.  This should be the URL alias syntax string that contains
 *   placeholders such as [id], [genus], [species], [name], [uniquename],
 *   and [type].  These placeholders will be substituted for actual values.
 *   If this parameter is not provided then the value of the
 *   chado_stock_url_string Drupal variable will be used.
 *
 * @ingroup tripal_stock
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
  $stock = chado_select_record('stock', array('*'), $values);
  if (!$stock) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find stock when setting URL alias for stock: %id", array('%id' => $node->stock_id));
    return FALSE;
  }
  $stock = (object) $stock[0];

  // get the organism
  $values = array('organism_id' => $stock->organism_id);
  $organism  = chado_select_record('organism', array('*'), $values);
  if (!$organism) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find organism when setting URL alias for stock: %id", array('%id' => $node->stock_id));
    return FALSE;
  }
  $genus = preg_replace('/\s/', '_', strtolower($organism[0]->genus));
  $species = preg_replace('/\s/', '_', strtolower($organism[0]->species));

  // get the type
  $values = array('cvterm_id' => $stock->type_id);
  $cvterm = chado_select_record('cvterm', array('name'), $values);
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