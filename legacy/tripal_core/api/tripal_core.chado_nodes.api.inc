<?php
/**
 * @file
 * API to handle much of the common functionality implemented when creating a
 *   drupal node type.
 */

/**
 * @defgroup tripal_legacy_chado_node_api Legacy Chado Nodes
 * @ingroup tripal_legacy_api
 * @{
 * Many Tripal modules implement Drupal node types as a means of displaying
 *   chado records individually through Drupal as a single web page. In order
 *   to do this, many of the same drupal hooks are implemented and the code
 *   between modules is actually quite similar. This API aims to abstract much
 *   of the common functionality in order to make it easier for new Tripal
 *   modules to implement drupal node types and to centralize the maintenance
 *   effort as much as possible.
 *
 * A generic sync form has been created. See chado_node_sync_form() for
 * instructions on how to implement this form in your module.
 *
 * Many of the base chado tables also have associated prop, _dbxref and
 *   _relationship tables. Generic mini-forms have been created to help you
 *   handle these forms. To implement this functionality you call the mini-form
 *   from your module node form and then call the associated update functions
 *   from both your hook_insert and hook_update. The functions of interest are
 *   as follows:
 *   - chado_add_node_form_properties() and chado_update_node_form_properties()
 *     to provide an interface for adding/removing properties
 *   - chado_add_node_form_dbxrefs() and chado_update_node_form_dbxrefs()
 *     to provide an interface for adding/removing additional database
 *   references
 *   - chado_add_node_form_relationships() and
 *   chado_update_node_form_relationships() to provide an interface for
 *   adding/removing relationships between chado records from your base table
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
 *   The chado table the chado record is from
 * @param $nid
 *   The value of the primary key of node
 * @param $linking_table
 *   The Drupal table linking the chado record to it's node.
 *   This field is optional and defaults to chado_$table
 *
 * @return
 *   The chado id of the associated chado record
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_get_id_from_nid($table, $nid, $linking_table = NULL) {
  if (empty($linking_table)) {
    $linking_table = 'chado_' . $table;
  }

  $sql = "SELECT " . $table . "_id as id FROM {$linking_table} WHERE nid = :nid";
  return db_query($sql, [':nid' => $nid])->fetchField();
}

/**
 *  Get node id for a chado feature/organism/analysis. E.g, if you want to
 *  get the node id for an analysis, use:
 *  $nid = chado_get_nid_from_id ('analysis', $analysis_id)
 *  Likewise,
 *  $nid = chado_get_nid_from_id ('organism', $organism_id)
 *  $nid = chado_get_nid_from_id ('feature', $feature_id)
 *
 * @param $table
 *   The chado table the id is from
 * @param $id
 *   The value of the primary key from the $table chado table (ie: feature_id)
 * @param $linking_table
 *   The Drupal table linking the chado record to it's node.
 *   This field is optional and defaults to chado_$table
 *
 * @return
 *   The nid of the associated node
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_get_nid_from_id($table, $id, $linking_table = NULL) {
  if (empty($linking_table)) {
    $linking_table = 'chado_' . $table;
  }

  $sql = "SELECT nid FROM {" . $linking_table . "} WHERE " . $table . "_id = :" . $table . "_id";
  return db_query($sql, [":" . $table . "_id" => $id])->fetchField();
}

/**
 * Determine the chado base table for a given content type
 *
 * @param $content_type
 *    The machine name of the content type (node type) you want to
 *    determine the base chado table of
 * @param $module
 *    (Optional) The machine-name of the module implementing the
 *    content type
 *
 * @return
 *    The name of the chado base table for the specified content type
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_get_base_table($content_type, $module = FALSE) {

  if ($module) {
    $node_info = call_user_func($details['module'] . '_node_info');
  }
  else {
    $node_types = module_invoke_all('node_info');

    if (isset($node_types[$content_type])) {
      $node_info = $node_types[$content_type];
    }
    else {
      return FALSE;
    }
  }

  if (isset($node_info['chado_node_api']['base_table'])) {
    return $node_info['chado_node_api']['base_table'];
  }
  else {
    return FALSE;
  }

}

/**
 * @section
 * Common Functionality for Properties, Dbxrefs and relationships chado node API
 */

/**
 * Validate the Triggering element from a node form.
 *
 * We are going to inspect the post to determine what PHP knows is the
 * triggering element and if it doesn't agree with Drupal then we are actually
 * going to change it in Drupal.
 *
 * This fixes an obscure bug triggered when a property is added and then
 * a relationship removed, Drupal thinks the first property remove button was
 * clicked and instead removes a property (not a relationship) and renders the
 * new property table in the relationship table page space.
 *
 * NOTE: Many Drupal issues state that this problem is solved if the #name
 * of the button is unique (which it is in our case) but we are still
 * experiencing incorrectly determined triggering elements so we need to handle
 * it ourselves.
 */
function chado_validate_node_form_triggering_element($form, &$form_state) {

  // We are going to inspect the post to determine what PHP knows is the triggering
  // element and if it doesn't agree with Drupal then we are actually going to
  // change it in Drupal.
  if ($_POST['_triggering_element_name'] != $form_state['triggering_element']['#name']) {
    $form_state['triggering_element']['#name'] = $_POST['_triggering_element_name'];
  }

}

/**
 * Validate Adding Subtables entries from the node forms.
 * Supported subtables: Properties, Relationships, Additional DBxrefs.
 *
 * @param array $form
 * @param array $form_state
 */
function chado_add_node_form_subtables_add_button_validate($form, &$form_state) {

  // Based on triggering element call the correct validation function
  // ASUMPTION #1: each of the buttons must have property, dbxref or relationship
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        chado_add_node_form_properties_add_button_validate($form, $form_state);
        break;
      case 'dbxrefs':
        chado_add_node_form_dbxrefs_add_button_validate($form, $form_state);
        break;
      case 'relationships':
        chado_add_node_form_relationships_add_button_validate($form, $form_state);
        break;
    }
  }
}

/**
 * Add subtable entries to the node forms.
 * Supported subtables: Properties, Relationships, Additional DBxrefs.
 *
 * @param array $form
 * @param array $form_state
 */
function chado_add_node_form_subtables_add_button_submit($form, &$form_state) {

  // Based on triggering element call the correct submit function
  // ASUMPTION #1: each of the buttons must have properties, dbxrefs or relationships
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        chado_add_node_form_properties_add_button_submit($form, $form_state);
        break;
      case 'dbxrefs':
        chado_add_node_form_dbxrefs_add_button_submit($form, $form_state);
        break;
      case 'relationships':
        chado_add_node_form_relationships_add_button_submit($form, $form_state);
        break;
    }
  }

  // This is needed to ensure the form builder function is called for the node
  // form in order for any of these changes to be seen.
  $form_state['rebuild'] = TRUE;
}

/**
 * Validate Removing Subtables entries from the node forms.
 * Supported subtables: Properties, Relationships, Additional DBxrefs.
 *
 * Since Removing isn't associated with any user input the only thing we
 * need to validate is that Drupal has determined the triggering element
 * correctly. That said, we will call each subtables associated validate
 * function just incase there is some case-specific validation we do not know
 * of or have not anticipated.
 *
 * @param array $form
 * @param array $form_state
 */
function chado_add_node_form_subtables_remove_button_validate($form, &$form_state) {

  // We need to validate the trigerring element since Drupal has known
  // issues determining this correctly when there are multiple buttons
  // with the same label.
  chado_validate_node_form_triggering_element($form, $form_state);

  // Based on triggering element call the correct validation function
  // ASUMPTION #1: each of the buttons must have property, dbxref or relationship
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        chado_add_node_form_properties_remove_button_validate($form, $form_state);
        break;
      case 'dbxrefs':
        chado_add_node_form_dbxrefs_remove_button_validate($form, $form_state);
        break;
      case 'relationships':
        chado_add_node_form_relationships_remove_button_validate($form, $form_state);
        break;
    }
  }
}

/**
 * Remove subtable entries to the node forms.
 * Supported subtables: Properties, Relationships, Additional DBxrefs.
 *
 * @param array $form
 * @param array $form_state
 */
function chado_add_node_form_subtables_remove_button_submit($form, &$form_state) {

  // Based on triggering element call the correct submit function
  // ASUMPTION #1: each of the buttons must have properties, dbxrefs or relationships
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        chado_add_node_form_properties_remove_button_submit($form, $form_state);
        break;
      case 'dbxrefs':
        chado_add_node_form_dbxrefs_remove_button_submit($form, $form_state);
        break;
      case 'relationships':
        chado_add_node_form_relationships_remove_button_submit($form, $form_state);
        break;
    }
  }

  // This is needed to ensure the form builder function is called for the node
  // form in order for any of these changes to be seen.
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax function which returns the section of the form to be re-rendered
 * for either the properties, dbxref or relationship sub-sections.
 *
 * @ingroup tripal_legacy_core
 */
function chado_add_node_form_subtable_ajax_update($form, &$form_state) {

  // We need to validate the trigerring element since Drupal has known
  // issues determining this correctly when there are multiple buttons
  // with the same label.
  chado_validate_node_form_triggering_element($form, $form_state);

  // Based on triggering element render the correct part of the form.
  // ASUMPTION: each of the buttons must have property, dbxref or relationship
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        return $form['properties']['property_table'];
        break;
      case 'dbxrefs':
        return $form['addtl_dbxrefs']['dbxref_table'];
        break;
      case 'relationships':
        return $form['relationships']['relationship_table'];
        break;
    }
  }
}

/**
 * @section
 * Sync Form
 */

/**
 * Generic Sync Form to aid in sync'ing (create drupal nodes linking to chado
 * content) any chado node type.
 *
 * To use this you need to add a call to it from your hook_menu() and
 * add some additional information to your hook_node_info(). The Following code
 * gives an example of how this might be done:
 *
 * @code
 *
 * function modulename_menu() {
 *
 * //  the machine name of your module
 * $module_name = 'tripal_example';
 *
 * // the base specified in hook_node_info
 * $node_type = 'chado_example';
 *
 * // This menu item will be a tab on the admin/tripal/legacy/tripal_example
 *   page
 * // that is not selected by default
 * $items['admin/tripal/legacy/tripal_example/sync'] = array(
 * 'title' => ' Sync',
 * 'description' => 'Sync examples from Chado with Drupal',
 * 'page callback' => 'drupal_get_form',
 * 'page arguments' => array('chado_node_sync_form', $module_name, $node_type),
 * 'access arguments' => array('administer tripal examples'),
 * 'type' => MENU_LOCAL_TASK,
 * 'weight' => 0
 * );
 *
 * return $items;
 * }
 *
 * function modulename_node_info() {
 * return array(
 * 'chado_example' => array(
 * 'name' => t('example'),
 * 'base' => 'chado_example',
 * 'description' => t('A Chado example is a collection of material that can be
 *   sampled and have experiments performed on it.'),
 * 'has_title' => TRUE,
 * 'locked' => TRUE,
 *
 * // this is what differs from the regular Drupal-documented hook_node_info()
 * 'chado_node_api' => array(
 * 'base_table' => 'example',            // The name of the chado base table
 * 'hook_prefix' => 'chado_example',     // Usually the name of the node type
 * 'linking_table' => 'chado_example',   // Specifies the linking table used
 * // to map records to Drupal nodes.
 * // if 'linking_table' is not specified
 * // it defaults to the node_type name.
 * 'record_type_title' => array(
 * 'singular' => t('Example'),         // Singular human-readable title
 * 'plural' => t('Examples')           // Plural human-readable title
 * ),
 * 'sync_filters' => array( // filters for syncing
 * 'type_id'     => TRUE,     // TRUE if there is an example.type_id field
 * 'organism_id' => TRUE,     // TRUE if there is an example.organism_id field
 * 'checkboxes'  => array('name')  // If the 'checkboxes' key is present then
 *   the
 * // value must be an array of column names in
 * // base table. The values from these columns will
 * // be retreived, contentated with a space delimeter
 * // and provided in a list of checkboxes
 * // for the user to choose which to sync.
 * ),
 * )
 * ),
 * );
 * }
 * @endcode
 *
 * For more information on how you can override some of this behaviour while
 *   still benifiting from as much of the common architecture as possible see
 *   the following functions: hook_chado_node_sync_create_new_node(),
 *   hook_chado_node_sync_form(), hook_chado_node_sync_select_query().
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_sync_form($form, &$form_state) {
  $form = [];

  if (isset($form_state['build_info']['args'][0])) {
    $module = $form_state['build_info']['args'][0];
    $node_type = $form_state['build_info']['args'][1];
    $node_info = call_user_func($module . '_node_info');

    // If a linking table is set in the node_info array then use that,
    // otherwise ues the node_type as the linking table.
    if (array_key_exists('linking_table', $node_info[$node_type]['chado_node_api'])) {
      $linking_table = $node_info[$node_type]['chado_node_api']['linking_table'];
    }
    else {
      $linking_table = 'chado_' . $node_info[$node_type]['chado_node_api']['base_table'];
    }
    $args = $node_info[$node_type]['chado_node_api'];
    $form_state['chado_node_api'] = $args;
  }

  $form['linking_table'] = [
    '#type' => 'hidden',
    '#value' => $linking_table,
  ];

  $form['node_type'] = [
    '#type' => 'hidden',
    '#value' => $node_type,
  ];

  // define the fieldsets
  $form['sync'] = [
    '#type' => 'fieldset',
    '#title' => 'Sync ' . $args['record_type_title']['plural'],
    '#descrpition' => '',
  ];

  $form['sync']['description'] = [
    '#type' => 'item',
    '#value' => t("%title_plural of the types listed " .
      "below in the %title_singular Types box will be synced (leave blank to sync all types). You may limit the " .
      "%title_plural to be synced by a specific organism. Depending on the " .
      "number of %title_plural in the chado database this may take a long " .
      "time to complete. ",
      [
        '%title_singular' => $args['record_type_title']['singular'],
        '%title_plural' => $args['record_type_title']['plural'],
      ]),
  ];

  if ($args['sync_filters']['type_id']) {
    $form['sync']['type_ids'] = [
      '#title' => t('%title_singular Types',
        [
          '%title_singular' => $args['record_type_title']['singular'],
          '%title_plural' => $args['record_type_title']['plural'],
        ]),
      '#type' => 'textarea',
      '#description' => t("Enter the names of the %title_singular types to sync. " .
        "Leave blank to sync all %title_plural. Separate each type with a comma " .
        "or new line. Pages for these %title_singular " .
        "types will be created automatically for %title_plural that exist in the " .
        "chado database. The names must match " .
        "exactly (spelling and case) with terms in the ontologies",
        [
          '%title_singular' => strtolower($args['record_type_title']['singular']),
          '%title_plural' => strtolower($args['record_type_title']['plural']),
        ]),
      '#default_value' => (isset($form_state['values']['type_id'])) ? $form_state['values']['type_id'] : '',
    ];
  }

  // get the list of organisms
  if ($args['sync_filters']['organism_id']) {
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $results = chado_query($sql);
    $organisms[] = '';
    foreach ($results as $organism) {
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
    }
    $form['sync']['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#description' => t("Choose the organism for which %title_plural types set above will be synced.",
        [
          '%title_singular' => $args['record_type_title']['singular'],
          '%title_plural' => $args['record_type_title']['plural'],
        ]),
      '#options' => $organisms,
      '#default_value' => (isset($form_state['values']['organism_id'])) ? $form_state['values']['organism_id'] : 0,
    ];
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
      $columns = $args['sync_filters']['checkboxes'];
      $select_cols = '';
      foreach ($columns as $column) {
        $select_cols .= $base_table . '.' . $column . "|| ' ' ||";
      }
      // Remove trailing || ' ' ||
      $select_cols = substr($select_cols, 0, -9);
      $base_table_id = $base_table . '_id';

      $select = [$base_table . '.' . $pkey, $select_cols . ' as value'];
      $joins = [];
      $where_clauses = [];
      $where_args = [];

      // Allow module to update the query.
      $hook_query_alter = $node_type . '_chado_node_sync_select_query';
      if (function_exists($hook_query_alter)) {
        $update = call_user_func($hook_query_alter, [
          'select' => $select,
          'joins' => $joins,
          'where_clauses' => $where_clauses,
          'where_args' => $where_args,
        ]);
        // Now add in any new changes
        if ($update and is_array($update)) {
          $select = $update['select'];
          $joins = $update['joins'];
          $where_clauses = $update['where_clauses'];
          $where_args = $update['where_args'];
        }
      }

      // Build Query, we do a left join on the chado_xxxx table in the Drupal schema
      // so that if no criteria are specified we only get those items that have not
      // yet been synced.
      $query = "SELECT " . implode(', ', $select) . ' ' .
        'FROM {' . $base_table . '} ' . $base_table . ' ' . implode(' ', $joins) . ' ' .
        "  LEFT JOIN [" . $linking_table . "] CT ON CT.$base_table_id = $base_table.$base_table_id " .
        "WHERE CT.$base_table_id IS NULL";

      // extend the where clause if needed
      $where = '';
      $sql_args = [];
      foreach ($where_clauses as $category => $items) {
        $where .= ' AND (';
        foreach ($items as $item) {
          $where .= $item . ' OR ';
        }
        $where = substr($where, 0, -4); // remove the trailing 'OR'
        $where .= ') ';
        $sql_args = array_merge($sql_args, $where_args[$category]);
      }

      if ($where) {
        $query .= $where;
      }
      $query .= " ORDER BY $base_table." . implode(", $base_table.", $columns);
      $results = chado_query($query, $sql_args);

      $values = [];
      foreach ($results as $result) {
        $values[$result->$pkey] = $result->value;
      }
      if (count($values) > 0) {
        $form['sync']['ids'] = [
          '#title' => 'Avaliable ' . $args['record_type_title']['plural'],
          '#type' => 'checkboxes',
          '#options' => $values,
          '#default_value' => (isset($form_state['values']['ids'])) ? $form_state['values']['ids'] : [],
          '#suffix' => '</div><br>',
          '#prefix' => t("The following  %title_plural have not been synced. Check those to be synced or leave all unchecked to sync them all.",
              [
                '%title_singular' => strtolower($args['record_type_title']['singular']),
                '%title_plural' => strtolower($args['record_type_title']['plural']),
              ]) . '<div style="height: 200px; overflow: scroll">',
        ];
      }
      else {
        $form['sync']['no_ids'] = [
          '#markup' => "<p>There are no " . strtolower($args['record_type_title']['plural']) . " to sync.</p>",
        ];
      }
    }
  }
  // if we provide a list of checkboxes we shouldn't need a max_sync
  else {
    $form['sync']['max_sync'] = [
      '#type' => 'textfield',
      '#title' => t('Maximum number of records to Sync'),
      '#description' => t('Leave this field empty to sync all records, regardless of number'),
      '#default_value' => (isset($form_state['values']['max_sync'])) ? $form_state['values']['max_sync'] : '',
    ];
  }

  $form['sync']['button'] = [
    '#type' => 'submit',
    '#value' => t('Sync ' . $args['record_type_title']['plural']),
    '#weight' => 3,
  ];


  $form['cleanup'] = [
    '#type' => 'fieldset',
    '#title' => t('Clean Up'),
  ];
  $form['cleanup']['description'] = [
    '#markup' => t("<p>With Drupal and chado residing in different databases " .
      "it is possible that nodes in Drupal and " . strtolower($args['record_type_title']['plural']) . " in Chado become " .
      "\"orphaned\".  This can occur if a node in Drupal is " .
      "deleted but the corresponding chado records is not and/or vice " .
      "versa. Click the button below to resolve these discrepancies.</p>"),
    '#weight' => -10,
  ];
  $form['cleanup']['cleanup_batch_size'] = [
    '#type' => 'textfield',
    '#title' => t('Batch Size'),
    '#description' => t('The number of records to analyze together in a batch. If you are having memory issues you might want to decrease this number.'),
    '#default_value' => variable_get('chado_node_api_cleanup_batch_size', 25000),
  ];
  $form['cleanup']['button'] = [
    '#type' => 'submit',
    '#value' => 'Clean up orphaned ' . strtolower($args['record_type_title']['plural']),
    '#weight' => 2,
  ];

  // Allow each module to alter this form as needed
  $hook_form_alter = $args['hook_prefix'] . '_chado_node_sync_form';
  if (function_exists($hook_form_alter)) {
    $form = call_user_func($hook_form_alter, $form, $form_state);
  }

  return $form;
}

/**
 * Generic Sync Form Validate
 *
 * @ingroup tripal_legacy_core
 */
function chado_node_sync_form_validate($form, &$form_state) {

  if (empty($form_state['values']['cleanup_batch_size'])) {
    $form_state['values']['cleanup_batch_size'] = 25000;
    drupal_set_message('You entered a Batch Size of 0 for Cleaning-up orphaned nodes. Since this is not valid, we reset it to the default of 25,000.', 'warning');
  }
  elseif (!is_numeric($form_state['values']['cleanup_batch_size'])) {
    form_set_error('cleanup_batch_size', 'The batch size must be a postitive whole number.');
  }
  else {
    // Round the value just to make sure.
    $form_state['values']['cleanup_batch_size'] = abs(round($form_state['values']['cleanup_batch_size']));
  }
}

/**
 * Generic Sync Form Submit
 *
 * @ingroup tripal_legacy_core
 */
function chado_node_sync_form_submit($form, $form_state) {

  global $user;

  if (preg_match('/^Sync/', $form_state['values']['op'])) {
    // get arguments
    $args = $form_state['chado_node_api'];
    $module = $form_state['chado_node_api']['hook_prefix'];
    $base_table = $form_state['chado_node_api']['base_table'];
    $linking_table = $form_state['values']['linking_table'];
    $node_type = $form_state['values']['node_type'];

    // Allow each module to hijack the submit if needed
    $hook_form_hijack_submit = $args['hook_prefix'] . '_chado_node_sync_form_submit';
    if (function_exists($hook_form_hijack_submit)) {
      return call_user_func($hook_form_hijack_submit, $form, $form_state);
    }

    // Get the types separated into a consistent string
    $types = [];
    if (isset($form_state['values']['type_ids'])) {
      // seperate by new line or comma.
      $temp_types = preg_split("/[,\n\r]+/", $form_state['values']['type_ids']);

      // remove any extra spacing around the types
      for ($i = 0; $i < count($temp_types); $i++) {
        // skip empty types
        if (trim($temp_types[$i]) == '') {
          continue;
        }
        $types[$i] = trim($temp_types[$i]);
      }
    }

    // Get the ids to be synced
    $ids = [];
    if (array_key_exists('ids', $form_state['values'])) {
      foreach ($form_state['values']['ids'] as $id => $selected) {
        if ($selected) {
          $ids[] = $id;
        }
      }
    }

    // get the organism to be synced
    $organism_id = FALSE;
    if (array_key_exists('organism_id', $form_state['values'])) {
      $organism_id = $form_state['values']['organism_id'];
    }

    // Job Arguments
    $job_args = [
      'base_table' => $base_table,
      'max_sync' => (!empty($form_state['values']['max_sync'])) ? $form_state['values']['max_sync'] : FALSE,
      'organism_id' => $organism_id,
      'types' => $types,
      'ids' => $ids,
      'linking_table' => $linking_table,
      'node_type' => $node_type,
    ];

    $title = "Sync " . $args['record_type_title']['plural'];
    tripal_add_job($title, $module, 'chado_node_sync_records', $job_args, $user->uid);
  }
  if (preg_match('/^Clean up orphaned/', $form_state['values']['op'])) {
    $module = $form_state['chado_node_api']['hook_prefix'];
    $base_table = $form_state['chado_node_api']['base_table'];
    $linking_table = $form_state['values']['linking_table'];
    $node_type = $form_state['values']['node_type'];
    $job_args = [
      $base_table,
      $form_state['values']['cleanup_batch_size'],
      $linking_table,
      $node_type,
    ];
    variable_set('chado_node_api_cleanup_batch_size', $form_state['values']['cleanup_batch_size']);
    tripal_add_job($form_state['values']['op'], $module, 'chado_cleanup_orphaned_nodes', $job_args, $user->uid);
  }
}

/**
 * Generic function for syncing records in Chado with Drupal nodes.
 *
 * @param $base_table
 *   The name of the Chado table containing the record that should be synced
 * @param $max_sync
 *   Optional: A numeric value to indicate the maximum number of records to
 *   sync.
 * @param $organism_id
 *   Optional: Limit the list of records to be synced to only those that
 *   are associated with this organism_id. If the record is not associated
 *   with an organism then this field is not needed.
 * @param $types
 *   Optional: Limit the list of records to be synced to only those that
 *   match the types listed in this array.
 * @param $ids
 *   Optional:  Limit the list of records to bye synced to only those whose
 *   primary key value matches the ID provided in this array.
 * @param $linking_table
 *   Optional: Tripal maintains "linking" tables in the Drupal schema
 *   to link Drupal nodes with Chado records.  By default these tables
 *   are named as 'chado_' . $base_table.  But if for some reason the
 *   linking table is not named in this way then it can be provided by this
 *   argument.
 * @param $node_type
 *   Optional: Tripal maintains "linking" tables in the Drupal schema
 *   to link Drupal nodes with Chado records. By default, Tripal expects that
 *   the node_type and linking table are named the same. However, if this
 *   is not the case, you can provide the node type name here.
 * @param $job_id
 *   Optional. Used by the Trpial Jobs system when running this function
 *   as a job. It is not needed othewise.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_node_sync_records($base_table, $max_sync = FALSE,
                                 $organism_id = FALSE, $types = [], $ids = [],
                                 $linking_table = FALSE, $node_type = FALSE, $job_id = NULL) {

  global $user;
  $base_table_id = $base_table . '_id';

  if (!$linking_table) {
    $linking_table = 'chado_' . $base_table;
  }
  if (!$node_type) {
    $node_type = 'chado_' . $base_table;
  }

  print "\nSync'ing $base_table records.  ";

  // START BUILDING QUERY TO GET ALL RECORD FROM BASE TABLE THAT MATCH
  $select = ["$base_table.*"];
  $joins = [];
  $where_clauses = [];
  $where_args = [];

  // If types are supplied then handle them
  $restrictions = '';
  if (count($types) > 0) {
    $restrictions .= "  Type(s): " . implode(', ', $types) . "\n";

    $select[] = 'cvterm.name as cvtname';
    $joins[] = "LEFT JOIN {cvterm} cvterm ON $base_table.type_id = cvterm.cvterm_id";
    foreach ($types as $type) {
      $sanitized_type = str_replace(' ', '_', $type);
      $where_clauses['type'][] = "cvterm.name = :type_name_$sanitized_type";
      $where_args['type'][":type_name_$sanitized_type"] = $type;
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
    $organism = chado_select_record('organism', ['*'], ['organism_id' => $organism_id]);
    $restrictions .= "  Organism: " . $organism[0]->genus . " " . $organism[0]->species . "\n";

    $select[] = 'organism.*';
    $joins[] = "LEFT JOIN {organism} organism ON organism.organism_id = $base_table.organism_id";
    $where_clauses['organism'][] = 'organism.organism_id = :organism_id';
    $where_args['organism'][':organism_id'] = $organism_id;
  }

  // Allow module to add to query
  $hook_query_alter = $node_type . '_chado_node_sync_select_query';
  if (function_exists($hook_query_alter)) {
    $update = call_user_func($hook_query_alter, [
      'select' => $select,
      'joins' => $joins,
      'where_clauses' => $where_clauses,
      'where_args' => $where_args,
    ]);
    // Now add in any new changes
    if ($update and is_array($update)) {
      $select = $update['select'];
      $joins = $update['joins'];
      $where_clauses = $update['where_clauses'];
      $where_args = $update['where_args'];
    }
  }
  // Build Query, we do a left join on the chado_xxxx table in the Drupal schema
  // so that if no criteria are specified we only get those items that have not
  // yet been synced.
  $query = "
    SELECT " . implode(', ', $select) . ' ' .
    'FROM {' . $base_table . '} ' . $base_table . ' ' . implode(' ', $joins) . ' ' .
    "  LEFT JOIN [" . $linking_table . "] CT ON CT.$base_table_id = $base_table.$base_table_id " .
    "WHERE CT.$base_table_id IS NULL ";

  // extend the where clause if needed
  $where = '';
  $sql_args = [];
  foreach ($where_clauses as $category => $items) {
    $where .= ' AND (';
    foreach ($items as $item) {
      $where .= $item . ' OR ';
    }
    $where = substr($where, 0, -4); // remove the trailing 'OR'
    $where .= ') ';
    $sql_args = array_merge($sql_args, $where_args[$category]);
  }

  if ($where) {
    $query .= $where;
  }
  $query .= " ORDER BY " . $base_table_id;

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

  // Iterate through records that need to be synced
  $count = $results->rowCount();
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }

  print "\n$count $base_table records found.\n";

  $i = 0;
  $transaction = db_transaction();
  print "\nNOTE: Syncing is performed using a database transaction. \n" .
    "If the sync fails or is terminated prematurely then the entire set of \n" .
    "synced items is rolled back and will not be found in the database\n\n";
  try {
    $percent = 0;
    foreach ($results as $record) {
      // Update the job status every 1% features.
      if ($job_id and $i % $interval == 0) {
        $percent = sprintf("%.2f", (($i + 1) / $count) * 100);
        print "Syncing $base_table " . ($i + 1) . " of $count (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
        tripal_set_job_progress($job_id, intval(($i / $count) * 100));
      }

      // Check if the record is already in the chado linking table
      // (ie: check to see if it is already linked to a node).
      $result = db_select($linking_table, 'lnk')
        ->fields('lnk', ['nid'])
        ->condition($base_table_id, $record->{$base_table_id}, '=')
        ->execute()
        ->fetchObject();

      if (empty($result)) {
        // Create generic new node.
        $new_node = new stdClass();
        $new_node->type = $node_type;
        $new_node->uid = $user->uid;
        $new_node->{$base_table_id} = $record->{$base_table_id};
        $new_node->$base_table = $record;
        $new_node->language = LANGUAGE_NONE;

        // TODO: should we get rid of this hook and use hook_node_presave() instead?
        // allow base module to set additional fields as needed
        $hook_create_new_node = $node_type . '_chado_node_sync_create_new_node';
        if (function_exists($hook_create_new_node)) {
          $new_node = call_user_func($hook_create_new_node, $new_node, $record);
        }

        // Validate and Save New Node
        $form = [];
        $form_state = [];
        node_validate($new_node, $form, $form_state);

        if (!form_get_errors()) {
          $node = node_submit($new_node);
          // If there are memory leaks on the node_save it is probably
          // caused by the hook_node_insert() function.
          node_save($node);
        }
        else {
          throw new Exception(t("Failed to insert $base_table: %title", ['%title' => $new_node->title]));
        }
      }
      $i++;
    }
    print "\n\nComplete!\n";
  } catch (Exception $e) {
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    watchdog_exception('trp-fsync', $e);
    print "FAILED: Rolling back database changes...\n";
  }
}

/**
 * This function is a wrapper for the chado_cleanup_orphaned_nodes function.
 * It breaks up the work of chado_cleanup_orphaned_nodes into smaller pieces
 * that are more managable for servers that may  have low php memory settings.
 *
 * @param $table
 *   The name of the table that corresonds to the node type we want to clean up.
 * @param $nentries
 *   Optional. The number of entries to parse at one time (ie: the batch size).
 *   Set to zero if no limit is needed.
 * @param $linking_table
 *   Optional. The name of the linking table that maps Drupal nodes to Chado
 *   records. This is only required if the linking table name is not of the
 *   form: chado_[table] where [table] is the value provided to the $table
 *   argument.
 * @param $node_type
 *   Optional. The name of the node type for the records.  This is only
 *   required if the node type is not of the form: chado_[table] where
 *   [table] is the value provided to the $table.
 * @param $job_id
 *   Optional. This should be the job id from the Tripal jobs system. Typically,
 *   only the Tripal jobs system will use the argument.
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_cleanup_orphaned_nodes($table, $nentries = 25000,
                                      $linking_table = NULL, $node_type = NULL, $job_id = NULL) {

  // The max number of records either as nodes or linked records.
  $count = 0;
  // Will hold the number of nodes of this type.
  $ncount = 0;
  // Will hold the number of linked records.
  $clcount = 0;

  if (!$node_type) {
    $node_type = 'chado_' . $table;
  }
  if (!$linking_table) {
    $linking_table = 'chado_' . $table;
  }
  // Find the number nodes of type chado_$table and find the number of entries
  // in chado_$table; keep the larger of the two numbers.
  $dsql = "SELECT COUNT(*) FROM {node} WHERE type = :node_type";
  $ndat = db_query($dsql, [':node_type' => $node_type]);
  $temp = $ndat->fetchObject();
  $ncount = $temp->count;
  $clsql = "SELECT COUNT(*) FROM {" . $linking_table . "}";
  $cdat = db_query($clsql);
  $clcount = $cdat->fetchObject();
  if ($ncount < $clcount) {
    $count = $clcount;
  }
  else {
    $count = $ncount;
  }

  $transaction = db_transaction();
  print "\nNOTE: This operation is performed using a database transaction. \n" .
    "If it fails or is terminated prematurely then the entire set of \n" .
    "changes is rolled back and will not be found in the database\n\n";
  try {
    $m = ceil($count / $nentries);
    for ($i = 0; $i < $m; $i++) {
      $offset = ($nentries * $i);
      chado_cleanup_orphaned_nodes_part($table, $job_id, $nentries, $offset,
        $linking_table, $node_type);
    }
  } catch (Exception $e) {
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    watchdog_exception('trp-fsync', $e);
    print "FAILED: Rolling back database changes...\n";
  }
  return '';
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
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_cleanup_orphaned_nodes_part($table, $job_id = NULL, $nentries,
                                           $offset, $linking_table, $node_type) {

  $count = 0;

  // Retrieve all of the entries in the linker table for a given node type
  // and place into an array.
  print "Verifying $linking_table records...\n";
  $cnodes = [];
  $clsql = "
    SELECT *
    FROM {" . $linking_table . "} LT
    ORDER BY LT.nid LIMIT $nentries OFFSET $offset";
  $res = db_query($clsql);
  foreach ($res as $node) {
    $cnodes[$count] = $node;
    $count++;
  }

  // Iterate through all of the $linking_table entries and remove those
  // that don't have a node or don't have a $table record.
  $deleted = 0;
  if ($count > 0) {
    $i = 0;
    $interval = intval($count * 0.01);
    if ($interval < 1) {
      $interval = 1;
    }
    foreach ($cnodes as $linker) {
      // Update the job status every 1% analyses
      if ($job_id and $i % $interval == 0) {
        $percent = sprintf("%.2f", ($i / $count) * 100);
        tripal_set_job_progress($job_id, intval($percent));
        print "Percent complete: $percent%. Memory: " . number_format(memory_get_usage()) . " bytes.\n";
      }

      // See if the node exits, if not remove the entry from linking table table.
      $nsql = "SELECT * FROM {node} WHERE nid = :nid AND type = :node_type";
      $results = db_query($nsql, [
        ':nid' => $linker->nid,
        ':node_type' => $node_type,
      ]);
      $node = $results->fetchObject();
      if (!$node) {
        $deleted++;
        db_query("DELETE FROM {" . $linking_table . "} WHERE nid = :nid", [':nid' => $linker->nid]);
        //print "$linking_table missing node.... DELETING where nid=".$linker->nid." $linking_table entry.\n";
      }

      // Does record in chado exists, if not remove entry from $linking_table.
      $table_id = $table . "_id";
      $lsql = "SELECT * FROM {" . $table . "} where " . $table_id . " = :chado_id";
      $results = chado_query($lsql, [":chado_id" => $linker->$table_id]);
      $record = $results->fetchObject();
      if (!$record) {
        $deleted++;
        $sql = "DELETE FROM {" . $linking_table . "} WHERE " . $table_id . " = :chado_id";
        db_query($sql, [":chado_id" => $linker->$table_id]);
        //print "$linking_table missing $table.... DELETING where $table_id=".$linker->$table_id." $linking_table entry.\n";
      }
      $i++;
    }
    $percent = sprintf("%.2f", ($i / $count) * 100);
    tripal_set_job_progress($job_id, intval($percent));
    print "Percent complete: $percent%. Memory: " . number_format(memory_get_usage()) . " bytes.\n";
  }
  print "\nDeleted $deleted record(s) from $linking_table missing either a node or chado entry.\n";

  // Build the SQL statements needed to check if nodes point to valid record.
  print "Verifying nodes...\n";
  $dsql = "
    SELECT *
    FROM {node}
    WHERE type = :node_type
    ORDER BY nid
    LIMIT $nentries OFFSET $offset
  ";

  $dsql_args = [':node_type' => $node_type];
  $nodes = [];
  $res = db_query($dsql, $dsql_args);
  $count = 0;
  foreach ($res as $node) {
    $nodes[$count] = $node;
    $count++;
  }

  // Iterate through all of the nodes and delete those that don't
  // have a corresponding entry in the linking table.
  $deleted = 0;
  if ($count > 0) {
    $i = 0;
    $interval = intval($count * 0.01);
    if ($interval < 1) {
      $interval = 1;
    }
    foreach ($nodes as $node) {
      // update the job status every 1%
      if ($job_id and $i % $interval == 0) {
        $percent = sprintf("%.2f", ($i / $count) * 100);
        tripal_set_job_progress($job_id, intval($percent));
        print "Percent complete: $percent%. Memory: " . number_format(memory_get_usage()) . " bytes.\r";
      }

      // check to see if the node has a corresponding entry
      // in the $linking_table table. If not then delete the node.
      $csql = "SELECT * FROM {" . $linking_table . "} WHERE nid = :nid ";
      $results = db_query($csql, [':nid' => $node->nid]);
      $link = $results->fetchObject();
      if (!$link) {
        // Checking node_access creates a memory leak. Commenting out for now
        // assuming that this code can only be run by a site administrator
        // anyway.
        //         if (node_access('delete', $node)) {
        $deleted++;
        node_delete($node->nid);
        //         }
        //         else {
        //           print "\nNode missing in $linking_table table.... but cannot delete due to improper permissions (node $node->nid)\n";
        //         }
      }
      $i++;
    }
    $percent = sprintf("%.2f", ($i / $count) * 100);
    tripal_set_job_progress($job_id, intval($percent));
    print "Percent complete: $percent%. Memory: " . number_format(memory_get_usage()) . " bytes.\r";
    print "\nDeleted $deleted node(s) that did not have corresponding $linking_table entries.\n";
  }

  return '';
}

/**
 * Create New Node
 *
 * Note: For your own module, replace hook in the function name with the
 * machine-name of your chado node type (ie: chado_feature).
 *
 * @param $new_node :
 *   a basic new node object
 * @param $record :
 *   the record object from chado specifying the biological data for this node
 *
 * @return
 *   A node object containing all the fields necessary to create a new node
 *   during sync
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function hook_chado_node_sync_create_new_node($new_node, $record) {

  // Add relevant chado details to the new node object. This really only
  // needs to be the fields from the node used during node creation
  // including values used to generate the title, etc. All additional chado
  // data will be added via nodetype_load when the node is later used
  $new_node->uniquename = $record->uniquename;

  return $new_node;
}

/**
 * Alter the Chado node sync form.
 *
 * This might be necessary if you need additional filtering options for
 * choosing which chado records to sync or even if you just want to further
 * customize the help text provided by the form.
 *
 * Note: For your own module, replace hook in the function name with the
 * machine-name of your chado node type (ie: chado_feature).
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function hook_chado_node_sync_form($form, &$form_state) {

  // Change or add to the form array as needed.
  // Any changes should be made in accordance with the Drupal Form API.

  return $form;
}

/**
 * Bypass chado node api sync form submit.
 *
 * Allows you to use this function as your own submit.
 *
 * This might be necessary if you want to add additional arguments to the
 * tripal job or to call your own sync'ing function if the generic
 * chado_node_sync_records() is not sufficient.
 *
 * Note: For your own module, replace hook in the function name with the
 * machine-name of your chado node type (ie: chado_feature).
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function hook_chado_node_sync_form_submit($form, $form_state) {

  global $user;

  $job_args = [
    // The base chado table (ie: feature).
    $base_table,
    // The maximum number of records to sync or FALSE for sync all that match.
    $max_sync,
    // The organism_id to restrict records to or FALSE if not to restrict by organism_id.
    $organism_id,
    // A string with the cvterm.name of the types to restrict to separated by |||
    $types,
  ];

  // You should register a tripal job
  tripal_add_job(
  // The title of the job -be descriptive.
    $title,
    // The name of your module.
    $module,
    // The chado node api sync function.
    'chado_node_sync_records',
    // An array with the arguments to pass to the above function.
    $job_args,
    // The user who submitted the job.
    $user->uid
  );

}


/**
 * Alter the query that retrieves records to be sync'd (optional)
 *
 * This might be necessary if you need fields from other chado tables to
 * create your node or if your chado node type only supports a subset of a
 * given table (ie: a germplasm node type might only support node creation for
 * cerain types of stock records in which case you would need to filter the
 * results to only those types).
 *
 * Note: For your own module, replace hook in the function name with the
 * machine-name of your chado node type (ie: chado_feature).
 *
 * @param $query
 *   An array containing the following:
 *    'select': An array of select clauses
 *    'joins:  An array of joins (ie: a single join could be
 *      'LEFT JOIN {chadotable} alias ON base.id=alias.id')
 *    'where_clauses: An array of where clauses which will all be AND'ed
 *      together. Use :placeholders for values.
 *    'where_args: An associative array of arguments to be subbed in to the
 *      where clause where the
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function hook_chado_node_sync_select_query($query) {

  // You can add fields to be selected. Be sure to prefix each field with the
  // tale name.
  $query['select'][] = 'example.myfavfield';

  // Provide any join you may need to the joins array. Be sure to wrap the
  // table name in curly brackets.
  $query['joins'][] = 'LEFT JOIN {exampleprop} PROP ON PROP.example_id=EXAMPLE.example_id';

  // The category should be a unique id for a group of items that will be
  // concatenated together via an SQL 'OR'.  By default the $where_clases
  // variable will come with categories of 'id', 'organism' and 'type'.
  // you can add your own unique category or alter the contents of the existing
  // categories.  Be sure to make sure the category doesn't already exist
  // in the $query['where_clauses']
  $category = 'my_category';

  // Provide any aditionall where clauses and their necessary arguments.
  // Be sure to prefix the field with the table name.   Be sure that the
  // placeholder is unique across all categories (perhaps add a unique
  // prefix/suffix).
  $query['where_clauses'][$category][] = 'example.myfavfield = :favvalue';
  $query['where_args'][$category][':favvalue'] = 'awesome-ness';

  // Must return the updated query
  return $query;
}
