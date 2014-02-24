<?php
/**
 * @file
 * Implements the organims node content type
 */

/**
 *  Implements hook_node_info().
 *
 * Provide information to drupal about the node types that we're creating
 *  in this module
 *
 * @ingroup tripal_organism
 */
function tripal_organism_node_info() {
  $nodes = array();
  $nodes['chado_organism'] = array(
    'name'        => t('Organism'),
    'base'        => 'chado_organism',
    'description' => t('An organism'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    'chado_node_api' => array(
      'base_table' => 'organism',
      'hook_prefix' => 'chado_organism',
      'record_type_title' => array(
        'singular' => t('Organism'),
        'plural' => t('Organisms')
      ),
      'sync_filters' => array(
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => array('genus', 'species'),
      ),
    )
  );
  return $nodes;
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
 * @ingroup tripal_organism
 */
function chado_organism_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_organism') {
    if ($op == 'create') {
      if (!user_access('create chado_organism', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_organism', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_organism', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_organism', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 *  Implement hook_form().
 *
 *  When editing or creating a new node of type 'chado_organism' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_organism
 */
function chado_organism_form($node, $form_state) {
  $form = array();

  // we have a file upload element on the form soe we need the multipart encoding type
  $form['#attributes']['enctype'] = 'multipart/form-data';

  // if the organism is part of the node object then we are editing. If not we are inserting
  if (property_exists($node, 'organism')) {
    $organism = $node->organism;

    // add in the comment since it is a text field and may not be included if too big
    $organism = chado_expand_var($organism, 'field', 'organism.comment');

    // get form defaults
    $abbreviation   = property_exists($node, 'abbreviation')   ? property_exists($node, 'abbreviation')   : $organism->abbreviation;
    $genus          = property_exists($node, 'genus')          ? property_exists($node, 'genus')          : $organism->genus;
    $species        = property_exists($node, 'species')        ? property_exists($node, 'species')        : $organism->species;
    $common_name    = property_exists($node, 'common_name')    ? property_exists($node, 'common_name')    : $organism->common_name;
    $description    = property_exists($node, 'description')    ? property_exists($node, 'description')    : $organism->comment;
    $organism_image = property_exists($node, 'organism_image') ? property_exists($node, 'organism_image') : '';

    // set the organism_id in the form
    $form['organism_id'] = array(
      '#type' => 'value',
      '#value' => $organism->organism_id,
    );
    $organism_id = $organism->organism_id;
  }
  else {
    // get form defaults
    $abbreviation   = property_exists($node, 'abbreviation')   ? property_exists($node, 'abbreviation')   : '';
    $genus          = property_exists($node, 'genus')          ? property_exists($node, 'genus')          : '';
    $species        = property_exists($node, 'species')        ? property_exists($node, 'species')        : '';
    $common_name    = property_exists($node, 'common_name')    ? property_exists($node, 'common_name')    : '';
    $description    = property_exists($node, 'description')    ? property_exists($node, 'description')    : '';
    $organism_image = property_exists($node, 'organism_image') ? property_exists($node, 'organism_image') : '';

    $organism_id = NULL;
  }

  $form['genus']= array(
    '#type' => 'textfield',
    '#title' => t('Genus'),
    '#required' => TRUE,
    '#default_value' => $genus,
  );
  $form['species']= array(
    '#type' => 'textfield',
    '#title' => t('Species'),
    '#required' => TRUE,
    '#default_value' => $species,
  );
  $form['abbreviation']= array(
    '#type' => 'textfield',
    '#title' => t('Abbreviation'),
    '#required' => TRUE,
    '#default_value' => $abbreviation,
  );
  $form['common_name']= array(
    '#type' => 'textfield',
    '#title' => t('Common Name'),
    '#required' => TRUE,
    '#default_value' => $common_name,
  );
  $form['description']= array(
    '#type' => 'textarea',
    '#rows' => 15,
    '#title' => t('Description'),
    '#default_value' => $description,
  );
  $form['organism_image']= array(
    '#type' => 'file',
    '#title' => t('Organism Image'),
    '#description' => 'Add an image for this organism',
    '#progress_indicator' => 'bar',
  );

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('organismprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'organismprop',      // the name of the prop table
    'chado_id' => $organism_id,              // the value of organism_id for this record
    'cv_id' => $cv_id                        // the cv.cv_id of the cv governing organismprop.type_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'organism_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'organism_id',   // the name of the key in your base chado table
    'base_key_value' => $organism_id       // the value of organism_id for this record
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  return $form;
}

/**
 * Implementation of hook_validate().
 *
 * @param $node
 * @param $form
 * @param $form_state
 *
 * @ingroup tripal_organism
 */
function chado_organism_validate($node, $form, &$form_state) {
  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a organism_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'organism_id') and $node->organism_id != 0) {
    return;
  }

  // remove any white space around values
  $node->genus        = trim($node->genus);
  $node->species      = trim($node->species);
  $node->abbreviation = trim($node->abbreviation);
  $node->common_name  = trim($node->common_name);
  $node->description  = trim($node->description);
  
  // Validating for an update
  if (property_exists($node, 'organism_id')) {
    $sql = "
      SELECT *
      FROM {organism} O
      WHERE
        genus = :genus AND
        species = :species AND NOT
        organism_id = :organism_id
    ";
    $args = array(':genus' => $node->genus, ':species' => $node->species, ':organism_id' => $node->organism_id);
    $result = chado_query($sql, $args)->fetchObject();
    if ($result) {
      form_set_error('genus', t("Update cannot proceed. The organism genus
        '$node->genus' and species '$node->species' is already present in the database."));
      tripal_report_error('tripal_organism', TRIPAL_WARNING,
        'Update organism: genus and species already exists: %values',
        array('%values' => "genus = $node->genus, species = $node->species"));
    }
  }
  // Validating for an insert
  else {
    $values = array(
      'genus' => $node->genus,
      'species' => $node->species,
    );
    $organism = chado_select_record('organism', array('organism_id'), $values);
    if (sizeof($organism) > 0) {
      form_set_error('genus', 'Cannot add the organism with this genus and species.
        The organism already exists.');
      tripal_report_error('tripal_organism', TRIPAL_WARNING,
        'Insert organism: genus and species already exists: %values',
        array('%values' => "genus = $node->genus, species = $node->species"));
    }
  }
}

/**
 *  Implements hook_insert().
 *
 *  When a new chado_organism node is created we also need to add information
 *  to our chado_organism table.  This function is called on insert of a new node
 *  of type 'chado_organism' and inserts the necessary information.
 *
 * @ingroup tripal_organism
 */
function chado_organism_insert($node) {
  // remove any white space around values
  $node->genus        = trim($node->genus);
  $node->species      = trim($node->species);
  $node->abbreviation = trim($node->abbreviation);
  $node->common_name  = trim($node->common_name);
  $node->description  = trim($node->description);

  // if there is an organism_id in the $node object then this must be a sync so
  // we can skip adding the organism as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'organism_id')) {
    $values = array(
      'genus'        => $node->genus,
      'species'      => $node->species,
      'abbreviation' => $node->abbreviation,
      'common_name'  => $node->common_name,
      'comment'      => $node->description
    );
    $organism = chado_insert_record('organism', $values);
    if (!$organism) {
      drupal_set_message(t('Unable to add organism.', 'warning'));
      tripal_report_error('tripal_organism', TRIPAL_ERROR, 'Insert Organism: Unable to create organism where values:%values',
      array('%values' => print_r($values, TRUE)));
      return;
    }
    $organism_id = $organism['organism_id'];

    if ($organism_id) {
      // * Properties Form *
      $details = array(
        'property_table' => 'organismprop',   // the name of the prop table
        'base_table' => 'organism',           // the name of your chado base table
        'foreignkey_name' => 'organism_id',   // the name of the key in your base table
        'foreignkey_value' => $organism_id    // the value of the example_id key
      );
      chado_update_node_form_properties($node, $details);

      // * Additional DBxrefs Form *
      $details = array(
        'linking_table' => 'organism_dbxref',   // the name of your _dbxref table
        'foreignkey_name' => 'organism_id',     // the name of the key in your base table
        'foreignkey_value' => $organism_id      // the value of the organism_id key
      );
      chado_update_node_form_dbxrefs($node, $details);

    }
  }
  else {
    $organism_id = $node->organism_id;
  }

  // Make sure the entry for this organism doesn't already exist in the
  // chado_organism table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('organism', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->organism_id = $organism_id;
    drupal_write_record('chado_organism', $record);
  }

  // add the image
  chado_organism_add_image($node);
}

/**
 * Implements hook_update().
 *
 * @ingroup tripal_organism
 */
function chado_organism_update($node) {
  // remove any white space around values
  $node->genus        = trim($node->genus);
  $node->species      = trim($node->species);
  $node->abbreviation = trim($node->abbreviation);
  $node->common_name  = trim($node->common_name);
  $node->description  = trim($node->description);

  $organism_id = chado_get_id_from_nid('organism', $node->nid);

  if ($node->revision) {
    // there is no way to handle revisions in Chado but leave
    // this here just to make not we've addressed it.
  }
  $match = array(
    'organism_id' => $organism_id,
  );
  $values = array(
    'genus' => $node->genus,
    'species' => $node->species,
    'abbreviation' => $node->abbreviation,
    'common_name' => $node->common_name,
    'comment' => $node->description
  );
  $org_status = chado_update_record('organism', $match, $values);

  // add the image
  chado_organism_add_image($node);

  // * Properties Form *
  $details = array(
    'property_table' => 'organismprop',   // the name of the prop table
    'base_table' => 'organism',           // the name of your chado base table
    'foreignkey_name' => 'organism_id',   // the name of the key in your base table
    'foreignkey_value' => $organism_id    // the value of the example_id key
  );
  chado_update_node_form_properties($node, $details);

  // * Additional DBxrefs Form *
  $details = array(
    'linking_table' => 'organism_dbxref',   // the name of your _dbxref table
    'foreignkey_name' => 'organism_id',     // the name of the key in your base table
    'foreignkey_value' => $organism_id      // the value of the organism_id key
  );
  chado_update_node_form_dbxrefs($node, $details);
}

/**
 * Implements hook_delete().
 *
 * Delete organism from both drupal and chado databases. Check dependency before
 * deleting from chado.
 *
 * @ingroup tripal_organism
 */
function chado_organism_delete($node) {
  $organism_id = chado_get_id_from_nid('organism', $node->nid);

  // if we don't have an organism id for this node then this isn't a node of
  // type chado_organism or the entry in the chado_organism table was lost.
  if (!$organism_id) {
    return;
  }

  // Remove data from the {chado_organism}, {node}, and {node_revisions} tables
  $sql_del = "DELETE FROM {chado_organism} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = ':nid' AND vid = ':vid'";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Test dependency before deleting from chado database. If a library or
  // feature depends on this organism, don't delete it

  $sql = "SELECT feature_id FROM {feature} WHERE organism_id = :organism_id";
  $check_feature = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();
  $sql = "SELECT library_id FROM {library} WHERE organism_id = :organism_id";
  $check_lib = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();
  $sql = "SELECT stock_id FROM {stock} WHERE organism_id = :organism_id";
  $check_stock = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();

  if (!$check_lib && !$check_feature && !$check_stock) {
    chado_delete_record('organism', array('organism_id' => $organism_id));
  }
  else {
    drupal_set_message(t("Warning: other data depends on this organism. The organism page was removed from this site but the organism was removed from Chado."), 'warning');
  }
}

/**
 * Add an image to an organims node
 *
 * @param $node
 *   The node to add an image to
 *
 * The file is specified in the $_FILES array created by Drupal
 *
 * @ingroup tripal_organism
 */
function chado_organism_add_image($node) {
  // check to see if a file was uploaded. If so then copy it to the images
  // directory for display with the organism
  if (isset($_FILES['files']) &&
  $_FILES['files']['name']['organism_image'] &&
  is_uploaded_file($_FILES['files']['tmp_name']['organism_image'])) {

    // make sure the destination directory exists
    $dest = tripal_get_files_dir() . "/tripal_organism/images";
    file_prepare_directory($dest, FILE_CREATE_DIRECTORY);

    // now move the file
    $validators = array('file_validate_is_image' => array());
    $destination = "public://tripal/tripal_organism/images/";
    $file = file_save_upload('organism_image', $validators, $destination);
    if (!$file) {
      drupal_set_message(t("Organism image was not uploaded."));
    }
    else {
      file_move($file, $destination . "/" . $node->nid . ".jpg", FILE_EXISTS_REPLACE);
    }
  }
}

/**
 *  Implements hook_load().
 *
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_organism
 */
function chado_organism_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // find the organism and add in the details
    $organism_id = chado_get_id_from_nid('organism', $nid);
    
    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$organism_id) {
      continue;
    }

    // build the organism variable
    $values = array('organism_id' => $organism_id);
    $organism = chado_generate_var('organism', $values);


    // add in the description field
    $organism = chado_expand_var($organism, 'field', 'organism.comment');
    $nodes[$nid]->organism = $organism;
  }
}

/**
 * Implements hook_node_presave(). Acts on all content types.
 *
 * @param $node
 *  The node to be saved
 *
 * @ingroup tripal_organism
 */
function tripal_organism_node_presave($node) {
  switch ($node->type) {
    case 'chado_organism':
      // when syncing the details are not present in the $node object
      // as they are when submitted via the form.  Therefore, if we do
      // not see any field values from the form, we assume this fucntion
      // is being called for syncing, so we must set the title accordingly
      if (property_exists($node, 'genus')) {
        $node->title = $node->genus  . " " . $node->species;
      }
      else if (property_exists($node, 'organism')) {
        $node->title = $node->organism->genus . " " . $node->organism->species;
      }
      break;
  }
}

/**
 * Implements hook_node_view().
 *
 * @ingroup tripal_organism
 */
function tripal_organism_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_organism':

      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_organism_base'] = array(
          '#markup'  => theme('tripal_organism_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_organism_properties'] = array(
          '#markup'  => theme('tripal_organism_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_organism_references'] = array(
          '#markup'  => theme('tripal_organism_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_organism_teaser'] = array(
          '#markup' => theme('tripal_organism_teaser', array('node' => $node)),
        );
      }
      break;
  }
}
