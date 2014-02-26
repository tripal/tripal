<?php
/**
 * @file
 * Implementation of hooks to create a feature content type
 */

/**
 * Implements hook_node_info().
 *
 * Provide information to drupal about the node types that we're creating
 *  in this module
 *
 * @ingroup tripal_feature
 */
function tripal_feature_node_info() {
  $nodes = array();

  $nodes['chado_feature'] = array(
    'name'        => t('Feature'),
    'base'        => 'chado_feature',
    'description' => t('A feature from the chado database'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    'chado_node_api' => array(
      'base_table' => 'feature',
      'hook_prefix' => 'chado_feature',
      'record_type_title' => array(
        'singular' => t('Feature'),
        'plural' => t('Features')
      ),
      'sync_filters' => array(
        'type_id' => TRUE,
        'organism_id' => TRUE
      ),
    )
  );
  return $nodes;
}

/**
 * Implementation of hook_form().
 *
 * @ingroup tripal_feature
 */
function chado_feature_form($node, &$form_state) {

  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing feature
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $feature      = null;
  $feature_id   = null;
  $uniquename   = '';
  $fname        = '';
  $feature_type = '';
  $organism_id  = '';
  $residues     = '';
  $is_obsolete  = '';
  $analyses     = '';
  $references   = '';
  $synonyms     = '';

  // if we are editing an existing node then the feature is already part of the node
  if (property_exists($node, 'feature')) {
    $feature = $node->feature;
    $feature = chado_expand_var($feature, 'field', 'feature.residues');
    $feature_id   = $feature->feature_id;
    $uniquename   = $feature->uniquename;
    $fname        = $feature->name;
    $feature_type = $feature->type_id->name;
    $organism_id  = $feature->organism_id->organism_id;
    $residues     = $feature->residues;
    $is_obsolete  = $feature->is_obsolete;

    // get the synonyms from a previous post
    $synonyms = '';
    if(property_exists($node, 'synonyms')) {
      $synonyms = $node->synonyms;
    }

    // get synonyms from the database if we don't already have them
    if (!$synonyms) {
      $options = array('return_array' => 1);
      $feature = chado_expand_var($feature, 'table', 'feature_synonym', $options);
      $feature_synonyms = (isset($feature->feature_synonym)) ? $feature->feature_synonym : array();
      foreach ($feature_synonyms as $index => $synonym) {
        $synonyms .= $synonym->synonym_id->name . "\n";
      }
    }
    // keep track of the feature id
    $form['feature_id'] = array(
      '#type' => 'value',
      '#value' => $feature_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state) and isset($form_state['values']['uniquename'])) {
    $uniquename   = $form_state['values']['uniquename'];
    $fname        = $form_state['values']['fname'];
    $feature_type = $form_state['values']['feature_type'];
    $organism_id  = $form_state['values']['organism_id'];
    $residues     = $form_state['values']['residues'];
    $is_obsolete  = $form_state['values']['is_obsolete'];
    $synonyms     = $form_state['values']['synonyms'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $uniquename   = $form_state['input']['uniquename'];
    $fname        = $form_state['input']['fname'];
    $feature_type = $form_state['input']['feature_type'];
    $organism_id  = $form_state['input']['organism_id'];
    $residues     = $form_state['input']['residues'];
    $is_obsolete  = array_key_exists('is_obsolete', $form_state['input']) ? $form_state['input']['is_obsolete'] : FALSE;
    $synonyms     = $form_state['input']['synonyms'];
  }

  $form['fname']= array(
    '#type' => 'textfield',
    '#title' => t('Feature Name'),
    '#required' => TRUE,
    '#default_value' => $fname,
    '#description' => t('Enter the name used by humans to refer to this feature.'),
    '#maxlength' => 255
  );
  $form['uniquename']= array(
    '#type' => 'textfield',
    '#title' => t('Unique Feature Name'),
    '#required' => TRUE,
    '#default_value' => $uniquename,
    '#description' => t('Enter a unique name for this feature.  This name must be unique for the organism and feature type.'),
    '#maxlength' => 255
  );
  
  $type_options = tripal_get_cvterm_default_select_options('feature', 'type_id', 'feature types');
  $type_options[0] = 'Select a Type';
  $type_cv = tripal_get_default_cv('feature', 'type_id');
  $cv_id = $type_cv->cv_id;

  $form['feature_type'] = array(
   '#title'       => t('Feature Type'),
   '#type'        => 'textfield',
   '#description' => t("Choose the feature type."),
   '#required'    => TRUE,
   '#default_value' => $feature_type,
   '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/$cv_id",
  );

  // get the list of organisms
  $sql = "SELECT * FROM {Organism} ORDER BY genus, species";
  $org_rset = chado_query($sql);
  $organisms = array();
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['organism_id'] = array(
    '#title'       => t('Organism'),
    '#type'        => t('select'),
    '#description' => t("Choose the organism with which this feature is associated"),
    '#required'    => TRUE,
    '#default_value' => $organism_id,
    '#options'     => $organisms,
  );

  // Get synonyms
  $syn_text = '';
  if ($synonyms) {
    if (is_array($synonyms)) {
      foreach ($synonyms as $synonym) {
        $syn_text .= "$synonym->name\n";
      }
    }
    else {
      $syn_text = $synonyms;
    }
  }
  $form['synonyms']= array(
    '#type' => 'textarea',
    '#title' => t('Synonyms'),
    '#required' => FALSE,
    '#default_value' => $syn_text,
    '#description' => t('Enter alternate names (synonmys) for this feature to help in searching and identification. You may enter as many alternate names as needed each on different lines.'),
  );

  $form['residues']= array(
    '#type' => 'textarea',
    '#title' => t('Residues'),
    '#required' => FALSE,
    '#default_value' => $residues,
    '#description' => t('Enter the nucelotide sequences for this feature'),
  );

  $checked = '';
  if ($is_obsolete == 't') {
    $checked = '1';
  }
  $form['is_obsolete']= array(
    '#type' => 'checkbox',
    '#title' => t('Is Obsolete'),
    '#required' => FALSE,
    '#default_value' => $checked,
    '#description' => t('Check this box if this sequence should be retired'),
  );

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('featureprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'featureprop',      // the name of the prop table
    'chado_id' => $feature_id,              // the value of feature_id for this record
    'cv_id' => $cv_id                       // the cv.cv_id of the cv governing featureprop.type_id
  );
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'feature_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'feature_id',   // the name of the key in your base chado table
    'base_key_value' => $feature_id       // the value of feature_id for this record
  );
  chado_add_node_form_dbxrefs($form, $form_state, $details);
  
  // TODO: For some reason adding a relationship to the form breaks AJAX
  // for features (works for other node type)... need to debug
  /*
  // RELATIONSHIPS FORM
  //---------------------------------------------
  $relationship_cv = tripal_get_default_cv('feature_relationship', 'type_id');
  $cv_id = $relationship_cv ? $relationship_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'feature_relationship',
    'base_table' => 'feature',
    'base_foreign_key' => 'feature_id',
    'base_key_value' => $feature_id,
    'nodetype' => 'feature',
    'cv_id' => $cv_id
  );
  chado_add_node_form_relationships($form, $form_state, $details);
  */

  return $form;
}

/**
 * Implementation of hook_validate().
 *
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @ingroup tripal_feature
 */
function chado_feature_validate($node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a feature_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'feature_id') and $node->feature_id != 0) {
    return;
  }

  // remove surrounding white-space on submitted values
  $node->uniquename   = trim($node->uniquename);
  $node->fname        = trim($node->fname);
  $node->feature_type = trim($node->feature_type);
  $node->residues     = trim($node->residues);
  
  // Validating for an update
  if (property_exists($node, 'nid')) {

    // make sure the feature type is a real sequence ontology term
    $type = tripal_cv_get_cvterm_by_name($node->feature_type, NULL, 'sequence');
    if (!$type) {
      form_set_error('feature_type', t("The feature type is not a valid name from the Sequence Ontology."));
    }

    // if this is an update, we want to make sure that a different feature for
    // the organism doesn't already have this uniquename. We don't want to give
    // two sequences the same uniquename
    if (property_exists($node, 'feature_id') and $node->feature_id != 0) {
      $sql = "
        SELECT *
        FROM {feature} F
          INNER JOIN {cvterm} CVT ON F.type_id = CVT.cvterm_id
        WHERE
          F.uniquename     = :uname AND
          F.organism_id    = :organism_id AND
          CVT.name         = :cvtname AND
          NOT f.feature_id = :feature_id
      ";
      $args = array(':uname' => $node->uniquename, ':organism_id' => $node->organism_id,
        ':cvtname' => $node->feature_type, ':feature_id' => $node->feature_id);
      $result = chado_query($sql, $args)->fetchObject();
      if ($result) {
        form_set_error('uniquename', t("Feature update cannot proceed. The feature name '$node->uniquename' is not unique for this organism. Please provide a unique name for this feature."));
      }
    }
  }
  // Validating for an insert
  else {

    // make sure the feature type is a real sequence ontology term
    $type = tripal_cv_get_cvterm_by_name($node->feature_type, NULL, 'sequence');
    if (!$type) {
      form_set_error('feature_type', t("The feature type is not a valid name from the Sequence Ontology."));
    }

    // if this is an insert then we just need to make sure this name doesn't
    // already exist for this organism if it does then we need to throw an error
    $sql = "
      SELECT *
      FROM {feature} F
        INNER JOIN {cvterm} CVT ON F.type_id = CVT.cvterm_id
      WHERE
        F.uniquename  = :name AND
        F.organism_id = :organism_id AND
        CVT.name      = :cvtname
    ";
    $args = array(':name' => $node->uniquename, ':organism_id' => $node->organism_id, ':cvtname' => $node->feature_type);
    $result = chado_query($sql, $args)->fetchObject();
    if ($result) {
      form_set_error('uniquename', t("Feature insert cannot proceed. The feature name '$node->uniquename' already exists for this organism. Please provide a unique name for this feature."));
    }
  }
}

/**
 * Implement hook_node_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 *  @param $node
 *  The node on which the operation is to be performed, or, if it does not yet exist, the
 *  type of node to be created
 *
 *  @param $op
 *  The operation to be performed
 *
 *  @param $account
 *  A user object representing the user for whom the operation is to be performed
 *
 *  @return
 *  If the permission for the specified operation is not set then return FALSE. If the
 *  permission is set then return NULL as this allows other modules to disable
 *  access.  The only exception is when the $op == 'create'.  We will always
 *  return TRUE if the permission is set.
 *
 * @ingroup tripal_feature
 */
function chado_feature_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_feature') {
    if ($op == 'create') {
      if (!user_access('create chado_feature content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
  
    if ($op == 'update') {
      if (!user_access('edit chado_feature content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_feature content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_feature content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
  }
  return NODE_ACCESS_IGNORE;
}

/**
 * Implements hook_insert().
 *
 *  When a new chado_feature node is created we also need to add information
 *  to our chado_feature table.  This function is called on insert of a new node
 *  of type 'chado_feature' and inserts the necessary information.
 *
 * @ingroup tripal_feature
 */
function chado_feature_insert($node) {

  $node->uniquename   = trim($node->uniquename);
  $node->fname        = trim($node->fname);
  $node->feature_type = trim($node->feature_type);
  $node->residues     = trim($node->residues);

  // remove spaces, newlines from residues
  $residues = preg_replace("/[\n\r\s]/", "", $node->residues);
  $obsolete = 'FALSE';
  if ($node->is_obsolete) {
    $obsolete = 'TRUE';
  }

  // get the feature type id
  $values = array(
    'cv_id' => array(
      'name' => 'sequence'
    ),
    'name' => $node->feature_type
  );
  $type = chado_select_record('cvterm', array('cvterm_id'), $values);

  $feature_id = '';

  // if there is a feature_id in the $node object then this must be a sync so
  // we can skip adding the feature as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'feature_id')) {
    $values = array(
      'organism_id' => $node->organism_id,
      'name' => $node->fname,
      'uniquename' => $node->uniquename,
      'residues' => $residues,
      'seqlen' => drupal_strlen($residues),
      'is_obsolete' => $obsolete,
      'type_id' => $type[0]->cvterm_id,
      'md5checksum' => md5($residues)
    );
    $feature = chado_insert_record('feature', $values);
    if (!$feature) {
      drupal_set_message(t('Unable to add feature.'), 'warning');
      tripal_report_error('tripal_feature', TRIPAL_WARNING, 'Insert feature: Unable to create feature where values: %values',
      array('%values' => print_r($values, TRUE)));
      return;
    }
    $feature_id = $feature['feature_id'];

    // add the genbank accession and synonyms
    chado_feature_add_synonyms($node->synonyms, $feature_id);

    // * Properties Form *
    $details = array(
      'property_table' => 'featureprop',   // the name of the prop table
      'base_table' => 'feature',           // the name of your chado base table
      'foreignkey_name' => 'feature_id',   // the name of the key in your base table
      'foreignkey_value' => $feature_id    // the value of the feature_id key
    );
    chado_update_node_form_properties($node, $details);

    // * Additional DBxrefs Form *
    $details = array(
      'linking_table' => 'feature_dbxref',   // the name of your _dbxref table
      'foreignkey_name' => 'feature_id',     // the name of the key in your base table
      'foreignkey_value' => $feature_id      // the value of the feature_id key
    );
    chado_update_node_form_dbxrefs($node, $details);

  }
  else {
    $feature_id = $node->feature_id;
  }

  // Make sure the entry for this feature doesn't already exist in the
  // chado_feature table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('feature', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->feature_id = $feature_id;
    drupal_write_record('chado_feature', $record);
  }
}

/**
 * Implements hook_update().
 *
 * @ingroup tripal_feature
 */
function chado_feature_update($node) {

  $node->uniquename   = trim($node->uniquename);
  $node->fname        = trim($node->fname);
  $node->feature_type = trim($node->feature_type);
  $node->residues     = trim($node->residues);


  $residues = preg_replace("/[\n\r\s]/", "", $node->residues);
  $obsolete = 'FALSE';
  if ($node->is_obsolete) {
    $obsolete = 'TRUE';
  }

  // get the feature type id
  $values = array(
    'cv_id' => array(
      'name' => 'sequence'
    ),
    'name' => $node->feature_type
  );
  $type = chado_select_record('cvterm', array('cvterm_id'), $values);

  $feature_id = chado_get_id_from_nid('feature', $node->nid) ;

  if (sizeof($type) > 0) {
    $match = array(
      'feature_id' => $feature_id,
    );
    $values = array(
      'organism_id' => $node->organism_id,
      'name' => $node->fname,
      'uniquename' => $node->uniquename,
      'residues' => $residues,
      'seqlen' => drupal_strlen($residues),
      'is_obsolete' => $obsolete,
      'type_id' => $type[0]->cvterm_id,
      'md5checksum' => md5($residues)
    );
    $options = array('return_record' => TRUE);
    $status = chado_update_record('feature', $match, $values, $options);

    // add the genbank synonyms
    chado_feature_add_synonyms($node->synonyms, $feature_id);

    // * Properties Form *
    $details = array(
      'property_table' => 'featureprop',   // the name of the prop table
      'base_table' => 'feature',           // the name of your chado base table
      'foreignkey_name' => 'feature_id',   // the name of the key in your base table
      'foreignkey_value' => $feature_id    // the value of the feature_id key
    );
    chado_update_node_form_properties($node, $details);

    // * Additional DBxrefs Form *
    $details = array(
      'linking_table' => 'feature_dbxref',   // the name of your _dbxref table
      'foreignkey_name' => 'feature_id',     // the name of the key in your base table
      'foreignkey_value' => $feature_id      // the value of the feature_id key
    );
    chado_update_node_form_dbxrefs($node, $details);

  }
  else {
    drupal_set_message(t('Unable to update feature.'), 'warning');
    tripal_report_error('tripal_feature', TRIPAL_WARNING,
     'Update feature: Unable to update feature where values: %values',
     array('%values' => print_r($values, TRUE))
    );
  }


}

/**
 * Implements hook_delete().
 *
 * @ingroup tripal_feature
 */
function chado_feature_delete($node) {

  $feature_id  = chado_get_id_from_nid('feature', $node->nid);

  // if we don't have a library id for this node then this isn't a node of
  // type chado_library or the entry in the chado_library table was lost.
  if (!$feature_id) {
    return;
  }

  // remove the drupal content
  $sql_del = "DELETE FROM {chado_feature} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));


  // Remove data from feature tables of chado database.  This will
  // cause a cascade delete and remove all data in referencing tables
  // for this feature
  chado_query("DELETE FROM {feature} WHERE feature_id = :feature_id", array(':feature_id' => $feature_id));

  drupal_set_message(t("The feature and all associated data were removed from") .
  "chado");

}

/**
 * Add synonyms to a feature
 *
 * @param $synonyms
 *   A string containing synonyms separated by a return character
 * @param $feature_id
 *   The feature to attach the synonyms to
 *
 * @ingroup tripal_feature
 */
function chado_feature_add_synonyms($synonyms, $feature_id) {

  // separate synomys by carriage returns
  $synonyms = preg_replace("/[\n\r]+/", " ", $synonyms);
  // split the synonyms into an array based on a space as the delimieter
  $syn_array = array();
  $syn_array = explode(" ", $synonyms);


  // remove any old synonyms
  $feature_syn_dsql = "DELETE FROM {feature_synonym} WHERE feature_id = :feature_id";
  if (!chado_query($feature_syn_dsql, array(':feature_id' => $feature_id))) {
    tripal_report_error('tripal_feature', TRIPAL_ERROR, "Could not remove synonyms from feature. ", array());
    return;
  }

  // return if we don't have any synonmys to add
  if (!$synonyms) {
    return;
  }
  // iterate through each synonym and add it to the database
  foreach ($syn_array as $syn) {
    // skip this item if it's empty
    if (!$syn) {
      break;
    }

    // check to see if we have this accession number already in the database
    // if so then don't add it again. it messes up drupal if the insert fails.
    // It is possible for the accession number to be present and not the feature
    $synonym_sql = "SELECT synonym_id FROM {synonym} WHERE name = :name";
    $synonym = chado_query($synonym_sql, array(':name' => $syn))->fetchObject();
    if (!$synonym) {
      $synonym_isql = "
        INSERT INTO {synonym} (name, synonym_sgml, type_id)
        VALUES (:name, :synonym_sgml,
          (SELECT cvterm_id
           FROM {cvterm} CVT
             INNER JOIN {cv} ON CVT.cv_id = CV.cv_id
           WHERE CV.name = 'feature_property' and CVT.name = 'synonym')
          )
      ";
      if (!chado_query($synonym_isql, array(':name' => $syn, ':synonym_sgml' => $syn))) {
        tripal_report_error('tripal_feature', "Could not add synonym. ", array(), TRIPAL_WARNING);
        return;
      }
      // now get the synonym we just added
      $synonym_sql = "SELECT synonym_id FROM {synonym} WHERE name = :name";
      $synonym = chado_query($synonym_sql, array(':name' => $syn))->fetchObject();
    }

    // now add in our new sysnonym
    $feature_syn_isql = "
      INSERT INTO {feature_synonym} (synonym_id,feature_id,pub_id)
      VALUES (:synonym_id, :feature_id, :pub_id)";
    $args = array(':synonym_id' => $synonym->synonym_id, ':feature_id' => $feature_id, ':pub_id'=> 1);
    if (!chado_query($feature_syn_isql, $args)) {
      tripal_report_error('tripal_feature', "Could not associate synonym with feature. ", array(), TRIPAL_WARNING);
      return;
    }
  }
}

/**
 * Implements hook_load().
 *
 * When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_feature
 */
function chado_feature_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // find the feature and add in the details
    $feature_id = chado_get_id_from_nid('feature', $nid);
    
    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$feature_id) {
      continue;
    }

    // build the feature variable
    $values = array('feature_id' => $feature_id);
    $feature = chado_generate_var('feature', $values);
    $nodes[$nid]->feature = $feature;

    // by default, the titles are saved using the unique constraint.  We will
    // keep it the same, but remove the duplicate name if the unique name and name
    // are identical. This doesn't change the title saved in the database, just what is shown
    // to the user on the page
    $title_type = variable_get('chado_feature_title', 'unique_constraint');
    if ($title_type == 'unique_constraint') {
      if (strcmp($feature->name, $feature->uniquename)==0) {
        $node->title = $feature->name . " (" . $feature->type_id->name . ") " . $feature->organism_id->genus . " " . $feature->organism_id->species ;
      }
      // in previous version of Tripal, the feature title was simply the unique name.
      // so, we recreate the title just to be sure all of our feature pages are consistent
      else {
        $node->title = $feature->name . ", " . $feature->uniquename . " (" . $feature->type_id->name . ") " . $feature->organism_id->genus . " " . $feature->organism_id->species ;
      }
    }
    // set the title to be the feature name or uniquename as configured
    if ($title_type == 'feature_name') {
      $node->title = $feature->name;
    }
    if ($title_type == 'feature_unique_name') {
      $node->title = $feature->uniquename;
    }
  }
}

/**
 * Implements hook_node_presave().
 * Acts on all content types.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_node_presave($node) {

  // set the title to ensure it is always unique
  switch ($node->type) {
    case 'chado_feature':
      // for a form submission the fields are part of the node object
      // but for a sync the fields are in an object of the node
      $name = '';
      $uname = '';
      $type = '';
      $organism_id = null;
      if(property_exists($node, 'uniquename')) {
        $organism_id = $node->organism_id;
        $name        = $node->name;
        $uname       = $node->uniquename;
        $type        = $node->feature_type;
      }
      else if (property_exists($node, 'feature')) {
        $organism_id = $node->feature->organism_id;
        $name        = $node->feature->name;
        $uname       = $node->feature->uniquename;
        $type_id     = $node->feature->type_id;
        $values = array('cvterm_id' => $type_id);
        $ftype = chado_select_record('cvterm', array('name'), $values);
        $type = $ftype[0]->name;
      }

      $values = array('organism_id' => $organism_id);
      $organism = chado_select_record('organism', array('genus', 'species'), $values);
      $node->title = "$name, $uname ($type) " . $organism[0]->genus . ' ' . $organism[0]->species;

      if ($name == $uname) {
        $node->title = "$name ($type) " . $organism[0]->genus . ' ' . $organism[0]->species;
      }
      break;
  }
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_node_insert($node) {

  // set the URL path after inserting.  We do it here because we do not
  // know the feature_id in the presave
  switch ($node->type) {
    case 'chado_feature':

      // remove any previous alias
      db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => "node/$node->nid"));

      // set the URL for this feature page
      $url_alias = tripal_feature_get_feature_url($node);
      $path_alias = array("source" => "node/$node->nid", "alias" => $url_alias);
      path_save($path_alias);
      break;
  }
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_node_update($node) {

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_feature':
      // remove any previous alias
      db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => "node/$node->nid"));

      // set the URL for this feature page
      $url_alias = tripal_feature_get_feature_url($node);
      $path_alias = array("source" => "node/$node->nid", "alias" => $url_alias);
      path_save($path_alias);
      break;
  }
}

/**
 * Return the url alias for a feature
 *
 * @param $node
 *   A node object containing at least the feature_id and nid
 * @param $url_alias
 *   Optional.  This should be the URL alias syntax string that contains
 *   placeholders such as [id], [genus], [species], [name], [uniquename],
 *   and [type].  These placeholders will be substituted for actual values.
 *   If this parameter is not provided then the value of the
 *   chado_feature_url_string Drupal variable will be used.
 *
 * @ingroup tripal_feature
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
  $feature = chado_select_record('feature', array('*'), $values);
  if (!$feature) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find feature when setting URL alias for feature: %id", array('%id' => $node->feature_id));
    return FALSE;
  }
  $feature = (object) $feature[0];

  // get the organism
  $values = array('organism_id' => $feature->organism_id);
  $organism  = chado_select_record('organism', array('*'), $values);
  if (!$organism) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find organism when setting URL alias for feature: %id", array('%id' => $node->feature_id));
    return FALSE;
  }
  $genus = preg_replace('/\s/', '_', strtolower($organism[0]->genus));
  $species = preg_replace('/\s/', '_', strtolower($organism[0]->species));

  // get the type
  $values = array('cvterm_id' => $feature->type_id);
  $cvterm = chado_select_record('cvterm', array('name'), $values);
  if (!$cvterm) {
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot find type when setting URL alias for feature: %id", array('%id' => $node->feature_id));
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
    tripal_report_error('trp-seturl', TRIPAL_ERROR, "Cannot set alias longer than 128 characters: %alias.", array('%alias' => $url_alias));
    return "node/" . $node->nid;
  }

  return $url_alias;
}

/**
 * Implements hook_node_view().
 * Acts on all content types.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_feature':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_feature_alignments'] = array(
          '#markup' => theme('tripal_feature_alignments', array('node' => $node)),
          '#tripal_toc_id'    => 'alignments',
          '#tripal_toc_title' => 'Alignments',
        );
        $node->content['tripal_feature_analyses'] = array(
          '#markup' => theme('tripal_feature_analyses', array('node' => $node)),
          '#tripal_toc_id'    => 'analyses',
          '#tripal_toc_title' => 'Analyses',
        );
        $node->content['tripal_feature_base'] = array(
          '#markup' => theme('tripal_feature_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_feature_properties'] = array(
          '#markup' => theme('tripal_feature_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_feature_publications'] = array(
          '#markup' => theme('tripal_feature_publications', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
        $node->content['tripal_feature_references'] = array(
          '#markup' => theme('tripal_feature_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_feature_relationships'] = array(
          '#markup' => theme('tripal_feature_relationships', array('node' => $node)),
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );
        $node->content['tripal_feature_seqence'] = array(
          '#markup' => theme('tripal_feature_sequence', array('node' => $node)),
          '#tripal_toc_id'    => 'sequence',
          '#tripal_toc_title' => 'Sequence',
        );
        $node->content['tripal_feature_synonyms'] = array(
          '#markup' => theme('tripal_feature_synonyms', array('node' => $node)),
          '#tripal_toc_id'    => 'synonyms',
          '#tripal_toc_title' => 'Synonyms',
        );
        $node->content['tripal_feature_terms'] = array(
          '#markup' => theme('tripal_feature_terms', array('node' => $node)),
          '#tripal_toc_id'    => 'terms',
          '#tripal_toc_title' => 'Annotated Terms',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_feature_teaser'] = array(
          '#markup' => theme('tripal_feature_teaser', array('node' => $node)),
        );
      }
      break;
    case 'chado_organism':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_organism_feature_counts'] = array(
          '#markup' => theme('tripal_organism_feature_counts', array('node' => $node)),
          '#tripal_toc_id'    => 'feature_counts',
          '#tripal_toc_title' => 'Feature Summary',
        );
        $node->content['tripal_organism_feature_browser'] = array(
          '#markup' => theme('tripal_organism_feature_browser', array('node' => $node)),
          '#tripal_toc_id'    => 'feature_browser',
          '#tripal_toc_title' => 'Feature Browser',
        );
      }
      break;
      // TODO: handle these node types. Should we also have a feature browser?
    case 'chado_library':
      break;
    case 'chado_stock':
      break;
    case 'chado_analysis':
      break;
  }
}


