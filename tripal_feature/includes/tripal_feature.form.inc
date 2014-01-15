<?php 

/**
 * Implementation of hook_form
 * 
 * @param  $node
 * @param  $param
 * 
 * @return 
 *   Drupal form array
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
    $feature = tripal_core_expand_chado_vars($feature, 'field', 'feature.residues');
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
      $feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_synonym', $options);
      $feature_synonyms = $feature->feature_synonym;      
      foreach ($feature_synonyms as $index => $synonym) {
        $synonyms .= $synonym->synonym_id->name . "\n";
      }
    }
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
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

  // We need to pass above variables for preview to show
  $form['feature'] = array(
    '#type' => 'value',
    '#value' => $feature
  );

  // keep track of the feature id if we have one.  If we do have one then
  // this would indicate an update as opposed to an insert.
  $form['feature_id'] = array(
    '#type' => 'value',
    '#value' => $feature_id,
  );
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
  // get the sequence ontology CV ID
  $values = array('name' => 'sequence');
  $cv = tripal_core_chado_select('cv', array('cv_id'), $values);
  $cv_id = $cv[0]->cv_id;

  $form['feature_type'] = array(
   '#title'       => t('Feature Type'),
   '#type'        => 'textfield',
   '#description' => t("Choose the feature type."),
   '#required'    => TRUE,
   '#default_value' => $feature_type,
   '#autocomplete_path' => "admin/tripal/tripal_cv/cvterm/auto_name/$cv_id",
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
  return $form;
}

/**
 * Implementation of hook_validate
 * 
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @param $node
 *  
 *
 * @ingroup tripal_feature
 */
function chado_feature_validate($node, &$form_state) {
  // remove surrounding white-space on submitted values
  $node->uniquename   = trim($node->uniquename);
  $node->fname        = trim($node->fname);
  $node->feature_type = trim($node->feature_type); 
  $node->residues     = trim($node->residues);
  
  $result = 0;

  // CASE A: if the nid exists then this is an update
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
  else {
    // To differentiate if we are syncing or creating a new feature altogther, see if a feature_id already exists
    if (property_exists($node, 'feature_id') and $node->feature_id != 0) {
      // CASE B: Synchronizing a node from chado to drupal
      // we don't need to do anything.
    }
    else {
      // CASE C: We are validating a form for inserting a new node
      
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
}