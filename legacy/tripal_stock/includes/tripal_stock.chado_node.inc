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
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_node_info() {
  return [
    'chado_stock' => [
      'name' => t('Stock (Tripal v2 legacy)'),
      'base' => 'chado_stock',
      'description' => t('A Chado Stock is a collection of material that can be sampled and have experiments performed on it.'),
      'has_title' => TRUE,
      'locked' => TRUE,
      'chado_node_api' => [
        'base_table' => 'stock',
        'hook_prefix' => 'chado_stock',
        'record_type_title' => [
          'singular' => t('Stock'),
          'plural' => t('Stocks'),
        ],
        'sync_filters' => [
          'type_id' => TRUE,
          'organism_id' => TRUE,
        ],
      ],
    ],
  ];
}

/**
 * Implements hook_load().
 *
 * When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_legacy_stock
 */
function chado_stock_load($nodes) {

  foreach ($nodes as $nid => $node) {

    // find the stock and add in the details
    $stock_id = chado_get_id_from_nid('stock', $nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$stock_id) {
      continue;
    }

    // build the variable with all the stock details
    $values = ['stock_id' => $stock_id];
    $stock = chado_generate_var('stock', $values);
    $stock = chado_expand_var($stock, 'field', 'stock.uniquename');
    $stock = chado_expand_var($stock, 'field', 'stock.description');
    $nodes[$nid]->stock = $stock;

    // Now get the title
    $node->title = chado_get_node_title($node);
  }
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
 *     main_db_reference => accession, version, description, db_name(select
 * from dropdown)
 *
 * @param $node
 *   An empty node object on insert OR the current stock node object on update
 * @param $form_state
 *   The current state of the form
 *
 * @return
 *   A description of the form to be rendered by drupal_get_form()
 *
 * @ingroup tripal_legacy_stock
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
  if (array_key_exists('values', $form_state) AND isset($form_state['values']['uniquename'])) {
    $sname = $form_state['values']['sname'];
    $uniquename = $form_state['values']['uniquename'];
    $stock_id = $form_state['values']['stock_id'];
    $type_id = $form_state['values']['type_id'];
    $organism_id = $form_state['values']['organism_id'];
    $sdescription = $form_state['values']['stock_description'];
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
    $sdescription = $form_state['input']['stock_description'];
    $dbxref_accession = $form_state['input']['accession'];
    $dbxref_description = $form_state['input']['db_description'];
    $dbxref_database = $form_state['input']['database'];
  }

  $form['sname'] = [
    '#type' => 'textfield',
    '#title' => t('Stock Name'),
    '#description' => t('Enter a human-readable name for this stock.'),
    '#default_value' => $sname,
    '#required' => TRUE,
  ];

  $form['uniquename'] = [
    '#type' => 'textfield',
    '#title' => t('Unique Name'),
    '#default_value' => $uniquename,
    '#description' => t('Enter a unique name for this stock. This name must be unique for the organism and stock type.'),
    '#required' => TRUE,
  ];

  if ($stock_id > 0) {
    $form['stock_id'] = [
      '#type' => 'hidden',
      '#value' => $stock_id,
    ];
  }

  // TODO: Should we make this a textfield with an autocomplete field like the
  // feature type_id field?.
  $st_cv = tripal_get_default_cv("stock", "type_id");
  $type_options = tripal_get_cvterm_default_select_options('stock', 'type_id', 'stock types');
  $type_options[0] = 'Select a Type';
  $st_message = tripal_set_message("To add additional items to the stock type drop down list,
     add a term to the " .
    l($st_cv->name . " controlled vocabulary",
      "admin/tripal/loaders/chado_vocabs/chado_cv/" . $st_cv->cv_id . "/cvterm/add",
      ['attributes' => ['target' => '_blank']]
    ),
    TRIPAL_INFO, ['return_html' => TRUE]
  );

  $form['type_id'] = [
    '#type' => 'select',
    '#title' => t('Type of Stock'),
    '#description' => t('Select the stock type.'),
    '#options' => $type_options,
    '#default_value' => $type_id,
    '#required' => TRUE,
    '#suffix' => $st_message,
  ];

  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $org_rset = chado_query($sql);
  $organisms = [];
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['organism_id'] = [
    '#type' => 'select',
    '#title' => t('Organism'),
    '#default_value' => $organism_id,
    '#description' => t('Choose the organism with which this stock is associated.'),
    '#options' => $organisms,
    '#required' => TRUE,
  ];

  $form['stock_description'] = [
    '#type' => 'text_format',
    '#title' => t('Notes'),
    '#default_value' => $sdescription,
    '#description' => t('Briefly enter any notes on the above stock. This should not include phenotypes or genotypes.'),
  ];

  $form['database_reference'] = [
    '#type' => 'fieldset',
    '#title' => t('Stock Database Reference'),
    '#description' => t('If this site is not the primary location for information
        about this stock, please provide the name of the database, the accession
        and an optional description using the fields below. If the database
        is not present in the list, then please ') .
      l(t('add the database '), 'admin/tripal/legacy/tripal_db/add', ['attributes' => ['target' => '_blank']]) .
      t('then refresh this page.'),
  ];

  $db_options = tripal_get_db_select_options();
  $form['database_reference']['database'] = [
    '#type' => 'select',
    '#title' => t('Database'),
    '#options' => $db_options,
    '#default_value' => $dbxref_database,
    '#description' => t('Select the remote database.'),
  ];

  $form['database_reference']['accession'] = [
    '#type' => 'textfield',
    '#title' => t('Accession'),
    '#default_value' => $dbxref_accession,
    '#description' => t('Please enter the accession in the remote database for this stock.'),
  ];

  $form['database_reference']['db_description'] = [
    '#type' => 'textarea',
    '#title' => t('Description of Database Reference'),
    '#default_value' => $dbxref_description,
    '#description' => t('Optionally enter a description about the database accession.'),

  ];


  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('stockprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = [
    'property_table' => 'stockprop',
    'chado_id' => $stock_id,
    'cv_id' => $cv_id,
  ];
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = [
    'linking_table' => 'stock_dbxref',
    'base_foreign_key' => 'stock_id',
    'base_key_value' => $stock_id,
  ];
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  $relationship_cv = tripal_get_default_cv('stock_relationship', 'type_id');
  $cv_id = $relationship_cv ? $relationship_cv->cv_id : NULL;
  $details = [
    'relationship_table' => 'stock_relationship',
    'base_table' => 'stock',
    'base_foreign_key' => 'stock_id',
    'base_key_value' => $stock_id,
    'nodetype' => 'stock',
    'cv_id' => $cv_id,
  ];
  chado_add_node_form_relationships($form, $form_state, $details);

  return $form;
}

/**
 * Implements hook_validate().
 * Validate the input from the chado_stock node form
 *
 * @param $node
 *   The current node including fields with the form element names and
 *   submitted values
 * @param $form
 *   A description of the form to be rendered by drupal_get_form()
 *
 * @ingroup tripal_legacy_stock
 */
function chado_stock_validate(&$node, $form, &$form_state) {

  // We only want to validate when the node is saved.
  // Since this validate can be called on AJAX and Deletion of the node
  // we need to make this check to ensure queries are not executed
  // without the proper values.
  if (property_exists($node, "op") and $node->op != 'Save') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a stock_id. We don't
  // need to validate during syncing so just skip it.
  if (!property_exists($node, 'nid') and property_exists($node, 'stock_id') and $node->stock_id != 0) {
    return;
  }

  // remove surrounding whitespace
  $node->uniquename = property_exists($node, 'uniquename') ? trim($node->uniquename) : '';
  $node->sname = property_exists($node, 'sname') ? trim($node->sname) : '';
  $node->accession = property_exists($node, 'accession') ? trim($node->accession) : '';
  $node->db_description = property_exists($node, 'db_description') ? trim($node->db_description) : '';

  $int_in_chado_sql = "SELECT count(*) as count FROM {:table} WHERE :column = :value";
  $string_in_chado_sql = "SELECT count(*) as count FROM {:table} WHERE :column = :value";

  // if this is an update, we want to make sure that a different stock for
  // the organism doesn't already have this uniquename. We don't want to give
  // two sequences the same uniquename
  if (property_exists($node, 'nid') and property_exists($node, 'stock_id')) {
    $sql = "
      SELECT *
      FROM {stock} S
        INNER JOIN {cvterm} CVT ON S.type_id = CVT.cvterm_id
      WHERE
        uniquename = :uname AND organism_id = :organism_id AND
        CVT.name = :cvtname AND NOT stock_id = :stock_id
    ";
    $result = chado_query($sql, [
      ':uname' => $node->uniquename,
      ':organism_id' => $node->organism_id,
      ':cvtname' => $node->type_id,
      ':stock_id' => $node->stock_id,
    ])->fetchObject();
    if ($result) {
      form_set_error('uniquename', t("Stock update cannot proceed. The stock name '$node->uniquename' is not unique for this organism. Please provide a unique name for this stock."));
    }
  }

  // if this is an insert then we just need to make sure this name doesn't
  // already exist for this organism if it does then we need to throw an error
  elseif (!empty($node->organism_id) AND !empty($node->type_id)) {
    $sql = "
      SELECT *
      FROM {stock} S
        INNER JOIN {cvterm} CVT ON S.type_id = CVT.cvterm_id
      WHERE uniquename = :uname AND organism_id = :organism_id AND CVT.name = :cvtname";
    $result = chado_query($sql, [
      ':uname' => $node->uniquename,
      ':organism_id' => $node->organism_id,
      ':cvtname' => $node->type_id,
    ])->fetchObject();
    if ($result) {
      form_set_error('uniquename', t("Stock insert cannot proceed. The stock name '$node->uniquename' already exists for this organism. Please provide a unique name for this stock."));
    }
  }


  // Check Type of Stock is valid cvterm_id in chado ( $form['values']['details']['type_id'] )
  if ($node->type_id == 0) {
    form_set_error('type_id', 'Please select a type of stock.');
  }
  else {
    $replace = [':table' => 'cvterm', ':column' => 'cvterm_id'];
    $new_sql = str_replace(array_keys($replace), $replace, $int_in_chado_sql);
    $num_rows = chado_query($new_sql, [':value' => $node->type_id])->fetchObject();
    if ($num_rows->count != 1) {
      form_set_error('type_id', "The type you selected is not valid. Please choose another one. (CODE:$num_rows)");
    }
  }

  // Check Source Organism is valid organism_id in chado ( $form['values']['details']['organism_id'] )
  if ($node->organism_id == 0) {
    form_set_error('organism_id', 'Please select a source organism for this stock');
  }
  else {
    $replace = [':table' => 'organism', ':column' => 'organism_id'];
    $new_sql = str_replace(array_keys($replace), $replace, $int_in_chado_sql);
    $num_rows = chado_query($new_sql, [':value' => $node->organism_id])->fetchObject();
    if ($num_rows->count != 1) {
      form_set_error('organism_id', "The organism you selected is not valid. Please choose another one. (CODE:$num_rows)");
    }
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
  if ($node->database > 0) {
    $replace = [':table' => 'db', ':column' => 'db_id'];
    $new_sql = str_replace(array_keys($replace), $replace, $int_in_chado_sql);
    $num_rows = chado_query($new_sql, [':value' => $node->database])->fetchObject();
    if ($num_rows->count != 1) {
      form_set_error('database', 'The database you selected is not valid. Please choose another one.');
    }
  }
}

/**
 * Implements hook_insert().
 * Inserts data from chado_stock_form() into drupal and chado
 *
 * @param $node
 *   The current node including fields with the form element names and
 *   submitted values
 *
 * @return
 *   TRUE if the node was successfully inserted into drupal/chado; FALSE
 *   otherwise
 *
 * @ingroup tripal_legacy_stock
 */
function chado_stock_insert($node) {


  $stock_id = '';

  // if there is an stock_id in the $node object then this must be a sync so
  // we can skip adding the stock to chado as it is already there, although
  // we do need to proceed with insertion into the chado/drupal linking table.
  if (!property_exists($node, 'stock_id')) {

    $node->uniquename = trim($node->uniquename);
    $node->sname = trim($node->sname);
    $node->accession = trim($node->accession);
    $node->stock_description = trim($node->stock_description['value']);

    // before we can add the stock, we must add the dbxref if one has been
    // provided by the user.
    $dbxref = NULL;
    if (!empty($node->accession) and !empty($node->database)) {
      $values = [
        'db_id' => $node->database,
        'accession' => $node->accession,
      ];
      if (!chado_select_record('dbxref', ['dbxref_id'], $values)) {
        $values['description'] = $node->db_description;
        $values['version'] = '1';
        $dbxref = chado_insert_record('dbxref', $values);
        if (!$dbxref) {
          drupal_set_message(t('Unable to add database reference to this stock.'), 'warning');
          tripal_report_error('tripal_stock', TRIPAL_WARNING,
            'Insert Stock: Unable to create dbxref where values:%values',
            ['%values' => print_r($values, TRUE)]);
        }
      }
    }

    // create stock including the dbxref
    $stock = '';
    $values = [
      'organism_id' => $node->organism_id,
      'name' => $node->sname,
      'uniquename' => $node->uniquename,
      'description' => $node->stock_description,
      'type_id' => $node->type_id,
    ];
    if ($dbxref) {
      $values['dbxref_id'] = [
        'db_id' => $node->database,
        'accession' => $node->accession,
      ];
    }
    $stock = chado_insert_record('stock', $values);
    if (!$stock) {
      drupal_set_message(t('Unable to add stock.'), 'warning');
      tripal_report_error('tripal_stock', TRIPAL_WARNING, 'Insert stock: Unable to create stock where values: %values',
        ['%values' => print_r($values, TRUE)]);
      return;
    }
    $stock_id = $stock['stock_id'];

    // Now add properties
    $details = [
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $stock_id,
    ];
    chado_update_node_form_properties($node, $details);

    // Now add the additional references
    $details = [
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $stock_id,
    ];
    chado_update_node_form_dbxrefs($node, $details);

    // Now add in relationships
    $details = [
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $stock_id,
    ];
    chado_update_node_form_relationships($node, $details);
  }
  else {
    $stock_id = $node->stock_id;
  }

  // Make sure the entry for this stock doesn't already exist in the
  // chado_stock table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('stock', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->stock_id = $stock_id;
    drupal_write_record('chado_stock', $record);
  }
}

/**
 * Implements hook_update().
 * Handles Editing/Updating of main stock info
 *
 * NOTE: Currently just writes over all old data
 *
 * @param $node
 *   The current node including fields with the form element names and
 *   submitted values
 *
 * @return
 *   TRUE if the node was successfully updated in drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_legacy_stock
 */
function chado_stock_update($node) {

  $node->uniquename = trim($node->uniquename);
  $node->sname = trim($node->sname);
  $node->stock_description = trim($node->stock_description['value']);

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
        ['dbxref_id', 'type_id'],
        ['stock_id' => $node->stock_id]
      );

      if ($stock[0]->dbxref_id) {
        $values = [
          'db_id' => $node->database,
          'accession' => $node->accession,
          'description' => $node->db_description,
        ];
        $dbxref_status = chado_update_record(
          'dbxref',
          ['dbxref_id' => $stock[0]->dbxref_id],
          $values
        );
        $dbxref_mode = 'Update';
      }
      else {
        if ($stock[0]->type_id) {
          //create the dbxref
          //used the type_id as a control to check we have a stock but not a dbxref
          $values = [
            'db_id' => $node->database,
            'accession' => $node->accession,
            'description' => $node->db_description,
            'version' => '1',
          ];
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
            ['%values', print_r($values, TRUE)]
          );
          return FALSE;
        }
      }
    }

    if (!$dbxref_status) {
      tripal_report_error('tripal_stock', TRIPAL_WARNING,
        'Stock Update: Unable to %mode main stock dbxref with values: %values',
        ['%values' => print_r($values, TRUE), '%mode' => $dbxref_mode]);
    }
  }

  //can't change stock id which is all thats stored in drupal thus only update chado
  $update_values = [
    'organism_id' => $node->organism_id,
    'name' => $node->sname,
    'uniquename' => $node->uniquename,
    'description' => $node->stock_description,
    'type_id' => $node->type_id,
  ];
  if ($dbxref_present) {
    if ($dbxref_status) {
      $update_values['dbxref_id'] = [
        'db_id' => $node->database,
        'accession' => $node->accession,
      ];
    }
  }
  $status = chado_update_record('stock', ['stock_id' => $node->stock_id], $update_values);


  if (!$status) {
    drupal_set_message(t('Unable to update stock'), 'error');
    tripal_report_error('tripal_stock', TRIPAL_ERROR,
      'Stock Update: Unable to update stock using match values: %mvalues and update values: %uvalues',
      [
        '%mvalues' => print_r(['stock_id' => $node->stock_id], TRUE),
        '%uvalues' => print_r($update_values, TRUE),
      ]
    );
  }
  else {
    // set the URL for this stock page
    $values = ['stock_id' => $node->stock_id];
    $stock = chado_select_record('stock', ['*'], $values);
  }

  // now update the properties
  if ($node->stock_id > 0) {

    $details = [
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id,
    ];
    chado_update_node_form_properties($node, $details);

  }

  // now update the additional dbxrefs
  if ($node->stock_id > 0) {
    $details = [
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id,
    ];
    chado_update_node_form_dbxrefs($node, $details);
  }

  // now update relationships
  if ($node->stock_id > 0) {
    $details = [
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $node->stock_id,
    ];
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
 *   The current node including fields with the form element names and
 *   submitted values
 *
 * @return
 *   TRUE if the node was successfully deleted from drupal/chado; FALSE
 *   otherwise
 *
 * @ingroup tripal_legacy_stock
 */
function chado_stock_delete($node) {

  // Set stock in chado: is_obsolete = TRUE
  chado_query("DELETE FROM {stock} WHERE stock_id = :stock_id", [':stock_id' => $node->stock->stock_id]);

  //remove drupal node and all revisions
  db_query("DELETE FROM {chado_stock} WHERE nid = :nid", [':nid' => $node->nid]);
}

/**
 * Used by Tripal Chado Node API during sync'ing of nodes
 *
 * @ingroup tripal_legacy_stock
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
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_node_presave($node) {

  switch ($node->type) {
    // This step is for setting the title for the Drupal node.  This title
    // is permanent and thus is created to be unique.  Title changes provided
    // by tokens are generated on the fly dynamically, but the node title
    // seen in the content listing needs to be set here. Do not call
    // the chado_get_node_title() function here to set the title as the node
    // object isn't properly filled out and the function will fail.
    case 'chado_stock':
      // For a form submission the fields are part of the node object
      // but for a sync the fields are in an object of the node.
      $organism_id = NULL;
      $sname = '';
      $uniquename = '';
      $type = '';
      if (property_exists($node, 'organism_id')) {
        $organism_id = $node->organism_id;
        $sname = $node->sname;
        $uniquename = $node->uniquename;
        $type_id = $node->type_id;
        $values = ['cvterm_id' => $node->type_id];
        $cvterm = chado_select_record('cvterm', ['name'], $values);
        $type = $cvterm[0]->name;
      }
      else {
        if (property_exists($node, 'stock')) {
          $organism_id = $node->stock->organism_id;
          $sname = $node->stock->name;
          $uniquename = $node->stock->uniquename;
          $type = $node->stock->type_id->name;
        }
      }
      $values = ['organism_id' => $organism_id];
      $organism = chado_select_record('organism', [
        'genus',
        'species',
      ], $values);
      $node->title = "$sname, $uniquename ($type) " . $organism[0]->genus . ' ' . $organism[0]->species;

      break;
  }
}

/**
 * Implements hook_node_view(). Acts on all content types.
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_node_view($node, $view_mode, $langcode) {

  switch ($node->type) {
    case 'chado_stock':
      if ($view_mode == 'full') {
        $node->content['tripal_stock_base'] = [
          '#theme' => 'tripal_stock_base',
          '#node' => $node,
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        ];
        $node->content['tripal_stock_collections'] = [
          '#theme' => 'tripal_stock_collections',
          '#node' => $node,
          '#tripal_toc_id' => 'collections',
          '#tripal_toc_title' => 'Stock Collections',
        ];
        $node->content['tripal_stock_properties'] = [
          '#theme' => 'tripal_stock_properties',
          '#node' => $node,
          '#tripal_toc_id' => 'properties',
          '#tripal_toc_title' => 'Properties',
        ];
        $node->content['tripal_stock_references'] = [
          '#theme' => 'tripal_stock_references',
          '#node' => $node,
          '#tripal_toc_id' => 'references',
          '#tripal_toc_title' => 'Cross References',
        ];
        $node->content['tripal_stock_relationships'] = [
          '#theme' => 'tripal_stock_relationships',
          '#node' => $node,
          '#tripal_toc_id' => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        ];
        $node->content['tripal_stock_synonyms'] = [
          '#theme' => 'tripal_stock_synonyms',
          '#node' => $node,
          '#tripal_toc_id' => 'synonyms',
          '#tripal_toc_title' => 'Synonyms',
        ];
        $node->content['tripal_stock_publications'] = [
          '#theme' => 'tripal_stock_publications',
          '#node' => $node,
          '#tripal_toc_id' => 'publications',
          '#tripal_toc_title' => 'Publications',
        ];
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_stock_teaser'] = [
          '#theme' => 'tripal_stock_teaser',
          '#node' => $node,
        ];
      }
      break;
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism_stocks'] = [
          '#theme' => 'tripal_organism_stocks',
          '#node' => $node,
          '#tripal_toc_id' => 'stocks',
          '#tripal_toc_title' => 'Stocks',
        ];
      }
      break;
  }
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_node_insert($node) {

  // set the URL path after inserting.  We do it here because we do not
  // know the stock_id in the presave
  switch ($node->type) {
    case 'chado_stock':

      // We still don't have a fully loaded node object in this hook. Therefore,
      // we need to simulate one so that the right values are available for
      // the URL to be determined.
      $stock_id = chado_get_id_from_nid('stock', $node->nid);
      $stock = chado_generate_var('stock', ['stock_id' => $stock_id]);
      $stock = chado_expand_var($stock, 'field', 'stock.uniquename');
      $stock = chado_expand_var($stock, 'field', 'stock.description');
      $node->stock = $stock;

      // Set the Title.
      $node->title = chado_get_node_title($node);

      // Now use the API to set the path.
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_node_update($node) {

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_stock':

      // Set the Title.
      $node->title = chado_get_node_title($node);

      // Now use the API to set the path.
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements [content_type]_chado_node_default_title_format().
 *
 * Defines a default title format for the Chado Node API to set the titles on
 * Chado Stock nodes based on chado fields.
 */
function chado_stock_chado_node_default_title_format() {
  return '[stock.name], [stock.uniquename] ([stock.type_id>cvterm.name]) [stock.organism_id>organism.genus] [stock.organism_id>organism.species]';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Designates a default URL format for stock nodes.
 */
function chado_stock_chado_node_default_url_format() {
  return '/stock/[stock.organism_id>organism.genus]/[stock.organism_id>organism.species]/[stock.type_id>cvterm.name]/[stock.uniquename]';
}

/**
 * Implement hook_node_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 * @param $node
 *  The node on which the operation is to be performed, or, if it does not yet
 *   exist, the type of node to be created
 *
 * @param $op
 *  The operation to be performed
 *
 * @param $account
 *  A user object representing the user for whom the operation is to be
 *   performed
 *
 * @return
 *  If the permission for the specified operation is not set then return FALSE.
 *   If the permission is set then return NULL as this allows other modules to
 *   disable access.  The only exception is when the $op == 'create'.  We will
 *   always return TRUE if the permission is set.
 *
 * @ingroup tripal_legacy_stock
 */

function tripal_stock_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_stock') {
    if ($op == 'create') {
      if (!user_access('create chado_stock content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_stock content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_stock content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_stock content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}