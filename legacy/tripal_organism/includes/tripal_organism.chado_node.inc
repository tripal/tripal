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
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_node_info() {
  $nodes = [];
  $nodes['chado_organism'] = [
    'name' => t('Organism (Tripal v2 legacy)'),
    'base' => 'chado_organism',
    'description' => t('An organism'),
    'has_title' => TRUE,
    'locked' => TRUE,
    'chado_node_api' => [
      'base_table' => 'organism',
      'hook_prefix' => 'chado_organism',
      'record_type_title' => [
        'singular' => t('Organism'),
        'plural' => t('Organisms'),
      ],
      'sync_filters' => [
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => ['genus', 'species'],
      ],
    ],
  ];
  return $nodes;
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
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_organism') {
    if ($op == 'create') {
      if (!user_access('create chado_organism content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_organism content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_organism content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_organism content', $account)) {
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
 * @ingroup tripal_legacy_organism
 */
function chado_organism_form($node, $form_state) {
  $form = [];
  $chado_version = chado_get_version(TRUE);

  // Default values can come in the following ways:
  //
  // 1) As elements of the $node object.  This occurs when editing an existing
  //    organism.
  // 2) In the $form_state['values'] array which occurs on a failed validation
  //    or ajax callbacks from non submit form elements
  // 3) In the $form_state['input'[ array which occurs on ajax callbacks from
  //    submit form elements and the form is being rebuilt
  //
  // Set form field defaults.
  $organism = NULL;
  $organism_id = NULL;
  $abbreviation = '';
  $genus = '';
  $species = '';
  $common_name = '';
  $description = '';
  $infraspecific_name = '';
  $type_id = '';

  // We have a file upload element on the form soe we need the multipart
  // encoding type
  $form['#attributes']['enctype'] = 'multipart/form-data';

  // If the organism is part of the node object then we are editing. If not
  // we are inserting
  if (property_exists($node, 'organism')) {
    $organism = $node->organism;

    // Add in the comment since it is a text field and may not be included if
    // too big
    $organism = chado_expand_var($organism, 'field', 'organism.comment');

    // Get form defaults.
    $abbreviation = $organism->abbreviation;
    $genus = $organism->genus;
    $species = $organism->species;
    $common_name = $organism->common_name;
    $description = $organism->comment;
    // The infraspecific and type_id fields are new to Chado v1.3
    if ($chado_version > 1.2) {
      $infraspecific_name = $organism->infraspecific_name;
      $type_id = $organism->type_id->cvterm_id;
    }

    // Set the organism_id in the form.
    $form['organism_id'] = [
      '#type' => 'value',
      '#value' => $organism->organism_id,
    ];
    $organism_id = $organism->organism_id;
  }

  // If we are re constructing the form from a failed validation or ajax
  // callback then use the $form_state['values'] values.
  if (array_key_exists('values', $form_state) and isset($form_state['values']['genus'])) {
    $abbreviation = $form_state['values']['abbreviation'];
    $genus = $form_state['values']['genus'];
    $species = $form_state['values']['species'];
    $common_name = $form_state['values']['common_name'];
    $description = $form_state['values']['comment'];
    if ($chado_version > 1.2) {
      $infraspecific_name = $form_state['values']['infraspecific_name'];
      $type_id = $form_state['values']['type_id'];
    }
  }
  // If we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array.
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $abbreviation = $form_state['input']['abbreviation'];
    $genus = $form_state['input']['genus'];
    $species = $form_state['input']['species'];
    $common_name = $form_state['input']['common_name'];
    $description = $form_state['input']['comment'];
    if ($chado_version > 1.2) {
      $infraspecific_name = $form_state['input']['infraspecific_name'];
      $type_id = $form_state['input']['type_id'];
    }
  }

  $form['genus'] = [
    '#type' => 'textfield',
    '#title' => t('Genus'),
    '#required' => TRUE,
    '#default_value' => $genus,
  ];
  $form['species'] = [
    '#type' => 'textfield',
    '#title' => t('Species'),
    '#required' => TRUE,
    '#default_value' => $species,
  ];
  // The infraspecific and type_id fields are new to Chado v1.3.
  if ($chado_version > 1.2) {

    $options = ['0' => 'Select a rank'];
    $cv = tripal_get_cv(['name' => 'taxonomic_rank']);
    if (!$cv) {
      drupal_set_message('The taxonomic_rank vocabulary cannot be found, thus selects for "rank" are not available.', 'warning');
    }
    else {
      $terms = tripal_get_cvterm_select_options($cv->cv_id);

      // Unfortunately the taxonomic_rank vocabulary is not properly organized
      // such that we only include terms below 'species'. Therefore we will
      // just list them here and hope we haven't missed one.
      $valid_terms = [
        'subspecies',
        'varietas',
        'subvariety',
        'forma',
        'subforma',
      ];
      foreach ($terms as $cvterm_id => $name) {
        if (in_array($name, $valid_terms)) {
          $options[$cvterm_id] = $name;
        }
      }
    }

    $form['type_id'] = [
      '#type' => 'select',
      '#title' => t('Infraspecific Rank'),
      '#options' => $options,
      '#default_value' => $type_id,
      '#description' => t('The scientific name for any taxon
        below the rank of species. This field is used for constructing the
        full infraspecific name for the organism.'),
    ];

    $form['infraspecific_name'] = [
      '#type' => 'textfield',
      '#title' => t('Infraspecific Name'),
      '#default_value' => $infraspecific_name,
      '#description' => t("The infraspecific name for this organism. When
          diplaying the full taxonomic name, this field is appended to the
          genus, species and rank."),
    ];
  }
  $form['abbreviation'] = [
    '#type' => 'textfield',
    '#title' => t('Abbreviation'),
    '#default_value' => $abbreviation,
    '#descriptoin' => t('A short abbreviation for this species (e.g. O.sativa)'),
  ];
  $form['common_name'] = [
    '#type' => 'textfield',
    '#title' => t('Common Name'),
    '#default_value' => $common_name,
  ];
  $form['description'] = [
    '#type' => 'text_format',
    '#rows' => 15,
    '#title' => t('Description'),
    '#default_value' => $description,
  ];

  $form['organism_image'] = [
    '#type' => 'managed_file',
    '#title' => t('Organism Image'),
    '#description' => t('Add an image to display for this organism.'),
    '#progress_indicator' => 'bar',
    '#upload_location' => 'public://tripal/tripal_organism/images/',
  ];

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('organismprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = [
    'property_table' => 'organismprop',
    'chado_id' => $organism_id,
    'cv_id' => $cv_id,
  ];
  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = [
    'linking_table' => 'organism_dbxref',
    'base_foreign_key' => 'organism_id',
    'base_key_value' => $organism_id,
  ];
  // Adds the form elements to your current form.
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
 * @ingroup tripal_legacy_organism
 */
function chado_organism_validate($node, $form, &$form_state) {

  // We only want to validate when the node is saved.
  // Since this validate can be called on AJAX and Deletion of the node
  // we need to make this check to ensure queries are not executed
  // without the proper values.
  if (property_exists($node, "op") and $node->op != 'Save') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a organism_id. We don't
  // need to validate during syncing so just skip it.
  if (!property_exists($node, 'nid') and property_exists($node, 'organism_id') and $node->organism_id != 0) {
    return;
  }

  // remove any white space around values
  $node->genus = property_exists($node, 'genus') ? trim($node->genus) : '';
  $node->species = property_exists($node, 'species') ? trim($node->species) : '';
  $node->abbreviation = property_exists($node, 'abbreviation') ? trim($node->abbreviation) : '';
  $node->common_name = property_exists($node, 'common_name') ? trim($node->common_name) : '';
  $node->type_id = property_exists($node, 'type_id') ? trim($node->type_id) : '';
  $node->infraspecific_name = property_exists($node, 'infraspecific_name') ? trim($node->infraspecific_name) : '';

  if ($node->type_id and !$node->infraspecific_name) {
    form_set_error('infraspecific_name', "If a rank is provided an infraspecific name must also be provided.");
  }
  if (!$node->type_id and $node->infraspecific_name) {
    form_set_error('type_id', "Please provide a rank for the infraspecific name.");
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
    $args = [
      ':genus' => $node->genus,
      ':species' => $node->species,
      ':organism_id' => $node->organism_id,
    ];
    $result = chado_query($sql, $args)->fetchObject();
    if ($result) {
      form_set_error('genus', t("Update cannot proceed. The organism genus
        '$node->genus' and species '$node->species' is already present in the database."));
      tripal_report_error('tripal_organism', TRIPAL_WARNING,
        'Update organism: genus and species already exists: %values',
        ['%values' => "genus = $node->genus, species = $node->species"]);
    }
  }
  // Validating for an insert
  else {
    $values = [
      'genus' => $node->genus,
      'species' => $node->species,
    ];
    $organism = chado_select_record('organism', ['organism_id'], $values);
    if (sizeof($organism) > 0) {
      form_set_error('genus', 'Cannot add the organism with this genus and species.
        The organism already exists.');
      tripal_report_error('tripal_organism', TRIPAL_WARNING,
        'Insert organism: genus and species already exists: %values',
        ['%values' => "genus = $node->genus, species = $node->species"]);
    }
  }
}

/**
 *  Implements hook_insert().
 *
 *  When a new chado_organism node is created we also need to add information
 *  to our chado_organism table.  This function is called on insert of a new
 * node of type 'chado_organism' and inserts the necessary information.
 *
 * @ingroup tripal_legacy_organism
 */
function chado_organism_insert($node) {

  $chado_version = chado_get_version(TRUE);
  $organism_id = '';

  // if there is an organism_id in the $node object then this must be a sync so
  // we can skip adding the organism as it is already there, although
  // we do need to proceed with insertion into the chado/drupal linking table.
  if (!property_exists($node, 'organism_id')) {
    // remove any white space around values
    $node->genus = trim($node->genus);
    $node->species = trim($node->species);
    $node->abbreviation = trim($node->abbreviation);
    $node->common_name = trim($node->common_name);
    $node->description = trim($node->description['value']);
    if ($chado_version > 1.2) {
      $node->type_id = trim($node->type_id);
      $node->infraspecific_name = trim($node->infraspecific_name);
    }

    $values = [
      'genus' => $node->genus,
      'species' => $node->species,
      'abbreviation' => $node->abbreviation,
      'common_name' => $node->common_name,
      'comment' => $node->description,
    ];
    if ($chado_version > 1.2) {
      if ($node->type_id) {
        $values['type_id'] = $node->type_id;
      }
      if ($node->infraspecific_name) {
        $values['infraspecific_name'] = $node->infraspecific_name;
      }
    }

    $organism = chado_insert_record('organism', $values);
    if (!$organism) {
      drupal_set_message(t('Unable to add organism.', 'warning'));
      tripal_report_error('tripal_organism', TRIPAL_ERROR, 'Insert Organism: Unable to create organism where values:%values',
        ['%values' => print_r($values, TRUE)]);
      return;
    }
    $organism_id = $organism['organism_id'];

    if ($organism_id) {
      // * Properties Form *
      $details = [
        'property_table' => 'organismprop',
        // the name of the prop table
        'base_table' => 'organism',
        // the name of your chado base table
        'foreignkey_name' => 'organism_id',
        // the name of the key in your base table
        'foreignkey_value' => $organism_id
        // the value of the example_id key
      ];
      chado_update_node_form_properties($node, $details);

      // * Additional DBxrefs Form *
      $details = [
        'linking_table' => 'organism_dbxref',
        // the name of your _dbxref table
        'foreignkey_name' => 'organism_id',
        // the name of the key in your base table
        'foreignkey_value' => $organism_id
        // the value of the organism_id key
      ];
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
  if (property_exists($node, 'organism_image')) {
    chado_organism_add_image($node);
  }
}

/**
 * Implements hook_update().
 *
 * @ingroup tripal_legacy_organism
 */
function chado_organism_update($node) {

  $chado_version = chado_get_version(TRUE);

  // remove any white space around values
  $node->genus = trim($node->genus);
  $node->species = trim($node->species);
  $node->abbreviation = trim($node->abbreviation);
  $node->common_name = trim($node->common_name);
  $node->description = trim($node->description['value']);
  if ($chado_version > 1.2) {
    $node->type_id = trim($node->type_id);
    $node->infraspecific_name = trim($node->infraspecific_name);
  }

  $organism_id = chado_get_id_from_nid('organism', $node->nid);

  if ($node->revision) {
    // there is no way to handle revisions in Chado but leave
    // this here just to make not we've addressed it.
  }
  $match = [
    'organism_id' => $organism_id,
  ];
  $values = [
    'genus' => $node->genus,
    'species' => $node->species,
    'abbreviation' => $node->abbreviation,
    'common_name' => $node->common_name,
    'comment' => $node->description,
  ];
  if ($chado_version > 1.2) {
    if ($node->type_id) {
      $values['type_id'] = $node->type_id;
    }
    else {
      $values['type_id'] = '__NULL__';
    }
    if ($node->infraspecific_name) {
      $values['infraspecific_name'] = $node->infraspecific_name;
    }
    else {
      $values['infraspecific_name'] = '__NULL__';
    }
  }

  $org_status = chado_update_record('organism', $match, $values);
  if ($node->organism_image != '') {
    chado_organism_add_image($node);
  }

  // * Properties Form *
  $details = [
    'property_table' => 'organismprop',
    // the name of the prop table
    'base_table' => 'organism',
    // the name of your chado base table
    'foreignkey_name' => 'organism_id',
    // the name of the key in your base table
    'foreignkey_value' => $organism_id
    // the value of the example_id key
  ];
  chado_update_node_form_properties($node, $details);

  // * Additional DBxrefs Form *
  $details = [
    'linking_table' => 'organism_dbxref',
    // the name of your _dbxref table
    'foreignkey_name' => 'organism_id',
    // the name of the key in your base table
    'foreignkey_value' => $organism_id
    // the value of the organism_id key
  ];
  chado_update_node_form_dbxrefs($node, $details);
}

/**
 * Adds the image to the organism node and cleans up any old images.
 *
 * @param $node
 *   The node object.
 */
function chado_organism_add_image($node) {

  // If there is already an organism image, then remove it it if
  // no other modules are using it
  $fid = db_select('file_usage', 'fu')
    ->fields('fu', ['fid'])
    ->condition('module', 'tripal_organism')
    ->condition('type', 'organism_image')
    ->condition('id', $node->nid)
    ->execute()
    ->fetchField();
  if ($fid) {
    $file = file_load($fid);
    file_usage_delete($file, 'tripal_organism', 'organism_image', $node->nid);
    file_delete($file);
  }

  // Save the uploaded file
  $file = file_load($node->organism_image);
  if ($file) {
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
    file_usage_add($file, 'tripal_organism', 'organism_image', $node->nid);
  }
}

/**
 * Implements hook_delete().
 *
 * Delete organism from both drupal and chado databases. Check dependency before
 * deleting from chado.
 *
 * @ingroup tripal_legacy_organism
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
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);

  // Test dependency before deleting from chado database. If a library or
  // feature depends on this organism, don't delete it

  $sql = "SELECT feature_id FROM {feature} WHERE organism_id = :organism_id";
  $check_feature = chado_query($sql, [':organism_id' => $organism_id])->fetchObject();
  $sql = "SELECT library_id FROM {library} WHERE organism_id = :organism_id";
  $check_lib = chado_query($sql, [':organism_id' => $organism_id])->fetchObject();
  $sql = "SELECT stock_id FROM {stock} WHERE organism_id = :organism_id";
  $check_stock = chado_query($sql, [':organism_id' => $organism_id])->fetchObject();

  if (!$check_lib && !$check_feature && !$check_stock) {
    chado_delete_record('organism', ['organism_id' => $organism_id]);
  }
  else {
    drupal_set_message(t("Warning: other data depends on this organism. The organism page was removed from this site but the organism was removed from Chado."), 'warning');
  }
}

/**
 *  Implements hook_load().
 *
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_legacy_organism
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
    $values = ['organism_id' => $organism_id];
    $organism = chado_generate_var('organism', $values);


    // add in the description field
    $organism = chado_expand_var($organism, 'field', 'organism.comment');
    $nodes[$nid]->organism = $organism;

    // Now get the title
    $node->title = chado_get_node_title($node);
  }
}

/**
 * Implements hook_node_presave(). Acts on all content types.
 *
 * @param $node
 *  The node to be saved
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_node_presave($node) {
  switch ($node->type) {
    // This step is for setting the title for the Drupal node.  This title
    // is permanent and thus is created to be unique.  Title changes provided
    // by tokens are generated on the fly dynamically, but the node title
    // seen in the content listing needs to be set here. Do not call
    // the chado_get_node_title() function here to set the title as the node
    // object isn't properly filled out and the function will fail.
    case 'chado_organism':
      // when syncing the details are not present in the $node object
      // as they are when submitted via the form.  Therefore, if we do
      // not see any field values from the form, we assume this function
      // is being called for syncing, so we must set the title accordingly
      if (property_exists($node, 'genus')) {
        $node->title = $node->genus . " " . $node->species;
        if (property_exists($node, 'type_id')) {
          $cvterm = tripal_get_cvterm(['cvterm_id' => $node->type_id]);
          if ($cvterm) {
            $node->title .= $cvterm->name . " " . $node->infraspecific_name;
          }
        }
      }
      elseif (property_exists($node, 'organism')) {
        $node->title = $node->organism->genus . " " . $node->organism->species;
        if (property_exists($node, 'type_id')) {
          $node->title .= $node->organism->type_id->name . " " . $node->organism->infraspecific_name;
        }
      }
      break;
  }
}

/**
 * Implements hook_node_view().
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_node_view($node, $view_mode, $langcode) {

  switch ($node->type) {
    case 'chado_organism':

      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_organism_base'] = [
          '#theme' => 'tripal_organism_base',
          '#node' => $node,
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        ];
        $node->content['tripal_organism_properties'] = [
          '#theme' => 'tripal_organism_properties',
          '#node' => $node,
          '#tripal_toc_id' => 'properties',
          '#tripal_toc_title' => 'Properties',
        ];
        $node->content['tripal_organism_references'] = [
          '#theme' => 'tripal_organism_references',
          '#node' => $node,
          '#tripal_toc_id' => 'references',
          '#tripal_toc_title' => 'Cross References',
        ];
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_organism_teaser'] = [
          '#theme' => 'tripal_organism_teaser',
          '#node' => $node,
        ];
      }
      break;
  }
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_node_insert($node) {

  switch ($node->type) {
    case 'chado_organism':

      // find the organism and add in the details
      $organism_id = chado_get_id_from_nid('organism', $node->nid);
      $values = ['organism_id' => $organism_id];
      $organism = chado_generate_var('organism', $values);
      $node->organism = $organism;

      // Now get the title
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
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_node_update($node) {

  switch ($node->type) {
    case 'chado_organism':

      // Now get the title.
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
 * Chado organism nodes based on chado fields.
 */
function chado_organism_chado_node_default_title_format() {
  return '[organism.genus] [organism.species]';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Designates a default URL format for organism nodes.
 */
function chado_organism_chado_node_default_url_format() {
  return '/organism/[organism.genus]/[organism.species]';
}

