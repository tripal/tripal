<?php
/**
 *  Provide information to drupal about the node types that we're creating
 *  in this module
 *
 * @ingroup tripal_organism
 */
function tripal_organism_node_info() {
  $nodes = array();
  $nodes['chado_organism'] = array(
    'name' => t('Organism'),
    'base' => 'chado_organism',
    'description' => t('An organism'),
    'has_title' => FALSE,
    'title_label' => t('Organism'),
    'locked' => TRUE,
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
 * Implement hook_access().
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
  if ($op == 'create') {
    if (!user_access('create chado_organism content', $account)) {
      return FALSE;
    }
    return TRUE;
  }
  if ($op == 'update') {
    if (!user_access('edit chado_organism content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'delete') {
    if (!user_access('delete chado_organism content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'view') {
    if (!user_access('access chado_organism content', $account)) {
      return FALSE;
    }
  }
  return NULL;
}
/**
 * Implementation of hook_validate
 *
 * @param $node
 * @param $form
 * @param $form_state
 *
 *  @ingroup tripal_organism
 */
function chado_organism_validate($node, $form, &$form_state) {
  // remove any white space around values
  $node->genus        = trim($node->genus);
  $node->species      = trim($node->species);
  $node->abbreviation = trim($node->abbreviation);
  $node->common_name  = trim($node->common_name);
  $node->description  = trim($node->description);


  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a organism_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'organism_id') and $node->organism_id != 0) {
    return;
  }

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
      watchdog('tripal_organism',
      'Update organism: genus and species already exists: %values',
      array('%values' => "genus = $node->genus, species = $node->species"),
      WATCHDOG_WARNING);
    }
  }
  // Validating for an insert
  else {
    $values = array(
      'genus' => $node->genus,
      'species' => $node->species,
    );
    $organism = tripal_core_chado_select('organism', array('organism_id'), $values);
    if (sizeof($organism) > 0) {
      form_set_error('genus', 'Cannot add the organism with this genus and species.
        The organism already exists.');
      watchdog('tripal_organism',
      'Insert organism: genus and species already exists: %values',
      array('%values' => "genus = $node->genus, species = $node->species"),
      WATCHDOG_WARNING);
    }
  }
}
/**
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
  if (!property_exists($node,'organism_id')) {
    $values = array(
      'genus'        => $node->genus,
      'species'      => $node->species,
      'abbreviation' => $node->abbreviation,
      'common_name'  => $node->common_name,
      'comment'      => $node->description
    );
    $organism = tripal_core_chado_insert('organism', $values);
    if (!$organism) {
      drupal_set_message(t('Unable to add organism.', 'warning'));
      watchdog('tripal_organism', 'Insert Organism: Unable to create organism where values:%values',
      array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
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
      chado_node_properties_form_update_properties($node, $details);

      // * Additional DBxrefs Form *
      $details = array(
        'linking_table' => 'organism_dbxref',   // the name of your _dbxref table
        'foreignkey_name' => 'organism_id',     // the name of the key in your base table
        'foreignkey_value' => $organism_id      // the value of the organism_id key
      );
      chado_node_additional_dbxrefs_form_update_dbxrefs($node, $details);

    }
  }
  else {
    $organism_id = $node->organism_id;
  }

  // Make sure the entry for this organism doesn't already exist in the
  // chado_organism table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_for_node('organism', $node->nid);
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
 * Update organisms
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

  $organism_id = chado_get_id_for_node('organism', $node->nid);

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
  $org_status = tripal_core_chado_update('organism', $match, $values);

  // add the image
  chado_organism_add_image($node);

  // * Properties Form *
  $details = array(
    'property_table' => 'organismprop',   // the name of the prop table
    'base_table' => 'organism',           // the name of your chado base table
    'foreignkey_name' => 'organism_id',   // the name of the key in your base table
    'foreignkey_value' => $organism_id    // the value of the example_id key
  );
  chado_node_properties_form_update_properties($node, $details);

  // * Additional DBxrefs Form *
  $details = array(
    'linking_table' => 'organism_dbxref',   // the name of your _dbxref table
    'foreignkey_name' => 'organism_id',     // the name of the key in your base table
    'foreignkey_value' => $organism_id      // the value of the organism_id key
  );
  chado_node_additional_dbxrefs_form_update_dbxrefs($node, $details);
}
/**
 * Delete organism from both drupal and chado databases. Check dependency before
 * deleting from chado.
 *
 * @ingroup tripal_organism
 */
function chado_organism_delete($node) {
  $organism_id = chado_get_id_for_node('organism', $node->nid);

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
    tripal_core_chado_delete('organism', array('organism_id' => $organism_id));
  }
  else {
    drupal_set_message(t("Warning: other data depends on this organism. The organism page was removed from this site but the organism was removed from Chado."), 'warning');
  }
}

/**
 *
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
    $dest = tripal_file_directory_path() . "/tripal_organism/images";
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
    $organism = tripal_core_expand_chado_vars($organism, 'field', 'organism.comment');

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
  $details = array(
    'property_table' => 'organismprop',      // the name of the prop table
    'base_foreign_key' => 'organism_id',     // the name of the key in your base chado table
    'base_key_value' => $organism_id,        // the value of organism_id for this record
    'cv_name' => 'organism_property'         // the cv.name of the cv governing organismprop.type_id
  );
  // Adds the form elements to your current form
  chado_node_properties_form($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'organism_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'organism_id',   // the name of the key in your base chado table
    'base_key_value' => $organism_id       // the value of organism_id for this record
  );
  // Adds the form elements to your current form
  chado_node_additional_dbxrefs_form($form, $form_state, $details);

  return $form;
}

/**
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_organism
 */
function chado_organism_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // find the organism and add in the details
    $organism_id = chado_get_id_for_node('organism', $nid);

    // build the organism variable
    $values = array('organism_id' => $organism_id);
    $organism = tripal_core_generate_chado_var('organism', $values);


    // add in the description field
    $organism = tripal_core_expand_chado_vars($organism, 'field', 'organism.comment');
    $nodes[$nid]->organism = $organism;
  }
}

/**
 *
 * @param $node
 */
function tripal_organism_node_presave($node) {
  switch ($node->type) {
    case 'chado_organism':
      // for a form submission the 'genus' field will be set,
      // for a sync, we must pull from the organism object
      if(property_exists($node, 'genus')) {
        // set the title
        $node->title = $node->genus  . " " . $node->species;
      }
      else {
        // set the title
        $node->title = $node->organism->genus . " " . $node->organism->species;
      }
      break;
  }
}
/**
 *
 * @ingroup tripal_feature
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
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_organism_teaser'] = array(
          '#markup' => theme('tripal_organism_teaser', array('node' => $node)),
        );
      }
      break;
  }
}