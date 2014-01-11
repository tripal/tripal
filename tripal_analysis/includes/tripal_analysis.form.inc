<?php 
/**
 *  When editing or creating a new node of type 'chado_analysis' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_form($node, &$form_state) {
  $form = array();
  
  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing analysis
  // 2) in the $form_state['values'] array which occurs on a failed validation or 
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit 
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $analysis_id    = null;
  $analysisname   = '';
  $program        = '';
  $programversion = '';
  $algorithm      = '';
  $sourcename     = '';
  $sourceversion  = '';
  $sourceuri      = '';
  $timeexecuted   = '';
  $description    = '';
  $d_removed      = array(); // lists removed properties
  $num_new        = 0;  // the number of new rows
  
  // if we are editing an existing node then the analysis is already part of the node
  if (property_exists($node, 'analysis')) {
    $analysis = $node->analysis;
    $analysis = tripal_core_expand_chado_vars($analysis, 'field', 'analysis.description');
    $analysis_id = $analysis->analysis_id;

    // get form defaults
    $analysisname   = $analysis->name;
    $program        = $analysis->program;
    $programversion = $analysis->programversion;
    $algorithm      = $analysis->algorithm;
    $sourcename     = $analysis->sourcename;
    $sourceversion  = $analysis->sourceversion;
    $sourceuri      = $analysis->sourceuri;
    $timeexecuted   = $analysis->timeexecuted;
    $description    = $analysis->description;
     
    // set the organism_id in the form
    $form['analysis_id'] = array(
      '#type' => 'value',
      '#value' => $analysis->analysis_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax callback 
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $analysisname   = $form_state['values']['analysisname'];
    $program        = $form_state['values']['program'];
    $programversion = $form_state['values']['programversion'];
    $algorithm      = $form_state['values']['algorithm'];
    $sourcename     = $form_state['values']['sourcename'];
    $sourceversion  = $form_state['values']['sourceversion'];
    $sourceuri      = $form_state['values']['sourceuri'];
    $timeexecuted   = $form_state['values']['timeexecuted'];
    $description    = $form_state['values']['description'];
    $d_removed      = $form_state['values']['removed'];
    $num_new        = $form_state['values']['num_new'] ? $form_state['values']['num_new'] : 0;
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $analysisname   = $form_state['input']['analysisname'];
    $program        = $form_state['input']['program'];
    $programversion = $form_state['input']['programversion'];
    $algorithm      = $form_state['input']['algorithm'];
    $sourcename     = $form_state['input']['sourcename'];
    $sourceversion  = $form_state['input']['sourceversion'];
    $sourceuri      = $form_state['input']['sourceuri'];
    $timeexecuted   = $form_state['input']['timeexecuted'];
    $description    = $form_state['input']['description'];
    $d_removed      = $form_state['input']['removed'];
    $num_new        = $form_state['input']['num_new'] ? $form_state['input']['num_new'] : 0;
  }

  $form['title']= array(
    '#type' => 'value',
    '#default_value' => $node->title,
  );
  $form['instructions'] = array(
    '#markup' => t('<b>Note</b>: When adding any type of data it is good to associate it with
        an analysis so that site visitors can identify the source of the data including
        necessary materials and methods.  The fields below imply that all analyses 
        are derived from some software package. But, data can also be derived via retreival
        from an external source or an analysis pipeline with multipel software components.
        In these cases, provide values for the fields below that best makes sense
        '),
  );
  $form['analysisname']= array(
    '#type' => 'textfield',
    '#title' => t('Analysis Name'),
    '#required' => TRUE,
    '#default_value' => $analysisname,
    '#description' => t("This should be a brief name that
       describes the analysis succintly. This name will helps the user find analyses."),
  );
  $form['program']= array(
    '#type' => 'textfield',
    '#title' => t('Program, Pipeline Name or Method Name'),
    '#required' => TRUE,
    '#default_value' => $program,
    '#description' => t("Program name, e.g. blastx, blastp, sim4, genscan. If the analysis was not derived from a software package, provide a very brief description of the pipeline or method."),
  );
  $form['programversion']= array(
    '#type' => 'textfield',
    '#title' => t('Program, Pipeline  or Method version'),
    '#required' => TRUE,
    '#default_value' => $programversion,
    '#description' => t("Version description, e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter 'n/a' if no version is available or applicable."),
  );
  $form['algorithm']= array(
    '#type' => 'textfield',
    '#title' => t('Algorithm'),
    '#required' => FALSE,
    '#default_value' => $algorithm,
    '#description' => t("Algorithm name, e.g. blast."),
  );
  $form['sourcename']= array(
    '#type' => 'textfield',
    '#title' => t('Source Name'),
    '#required' => TRUE,
    '#default_value' => $sourcename,
    '#description' => t('The name of the source data.  This could be a file name, data set name or a
         small description for how the data was collected.  For long descriptions use the description field below'),
  );
  $form['sourceversion']= array(
    '#type' => 'textfield',
    '#title' => t('Source Version'),
    '#required' => FALSE,
    '#default_value' => $sourceversion,
    '#description' => t('If the source dataset has a version, include it here'),
  );
  $form['sourceuri']= array(
    '#type' => 'textfield',
    '#title' => t('Source URI'),
    '#required' => FALSE,
    '#default_value' => $sourceuri,
    '#description' => t("This is a permanent URL or URI for the source of the analysis.
       Someone could recreate the analysis directly by going to this URI and
       fetching the source data (e.g. the blast database, or the training model)."),
  );

  // Get time saved in chado
  $default_time = $timeexecuted;
  $year = preg_replace("/^(\d+)-\d+-\d+ .*/", "$1", $default_time);
  $month = preg_replace("/^\d+-0?(\d+)-\d+ .*/", "$1", $default_time);
  $day = preg_replace("/^\d+-\d+-0?(\d+) .*/", "$1", $default_time);
  // If the time is not set, use current time
  if (!$default_time) {
    $default_time = REQUEST_TIME;
    $year  = format_date($default_time, 'custom', 'Y');
    $month = format_date($default_time, 'custom', 'n');
    $day   = format_date($default_time, 'custom', 'j');
  }
  $form['timeexecuted']= array(
    '#type' => 'date',
    '#title' => t('Time Executed'),
    '#required' => TRUE,
    '#default_value' => array(
       'year' => $year,
       'month' => $month,
       'day' => $day,
    ),
  );
  $form['description']= array(
    '#type' => 'textarea',
    '#rows' => 15,
    '#title' => t('Materials & Methods (Description and/or Program Settings)'),
    '#required' => FALSE,
    '#default_value' => $description,
    '#description' => t('Please provide all necessary information to allow
       someone to recreate the analysis, including materials and methods
       for collection of the source data and performing the analysis'),
  );
  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Analysis Details'),
    '#description' => t('You may add additional properties by
      selecting a property type from the dropdown and adding text.  You may add 
      as many properties as desired by clicking the plus button on the right.  To 
      remove a property, click the minus button.  If a property is not available
      you may add it by ' . l('adding the term', 'admin/tripal/tripal_cv/cvterm/add') . '
      to the <b>analysis_property</b> vocabulary within the <b>tripal</b> database'),
  );
  $form['properties']['table'] = array(
    '#type' => 'markup',
    '#value' =>  '',
    '#prefix' => '<div id="tripal-analysis-edit-properties-table">',
    '#suffix' => '</div>',
  );
  
  // get the analysis properties
  $properties_select = array();
  $properties_select[] = 'Select a Property';
  $properties_list = array();
  $sql = "
    SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition
    FROM  {cvterm} CVT
      INNER JOIN {cv} ON CVT.cv_id = CV.cv_id
    WHERE
      CV.name = 'analysis_property' AND
      NOT CVT.is_obsolete = 1
    ORDER BY CVT.name ASC
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    $properties_select[$prop->cvterm_id] = $prop->name;
    $properties_list[$prop->cvterm_id] = $prop;
  }
  
  // this array keeps track of all properties we have and allows the functions
  // below to select the next rank if a property is dupliated
  $ranks = array();
 
  // add in the properties from the Chado analysisprop table (only pertains to existing analyses)
  if ($analysis_id) {
    chado_analysis_node_form_add_analysisprop_table_props($form, $form_state, $analysis_id, $ranks, $d_removed);
  }
  
  // add in any new properties that have been added by the user through an AHAH callback
  chado_analysis_node_form_add_new_props($form, $form_state, $ranks, $d_removed);

  // add an empty row of field to allow for addition of a new property
  chado_analysis_node_form_add_new_empty_props($form, $form_state, $properties_select);
  
  $form['#theme'] = 'chado_analysis_form';
  return $form;
}
/**
 * This function is responsible for adding a blank row to the properties table for
 * adding a new property.
 */
function chado_analysis_node_form_add_new_empty_props(&$form, &$form_state, $properties_select) {

  // get the field defaults either from $form_state['values'] or $form_state['input']
  $description = '';
  $text = '';
  $id = 0;
  if (array_key_exists('values', $form_state)) {
    $id = $form_state['values']['new_id'];
    $text = $form_state['values']['new_value'];
  }
  
  // if we have a property ID then get it's definition to display to the user
  if($id) { 
    $values = array('cvterm_id' => $id);
    $cvterm = tripal_core_chado_select('cvterm', array('definition'), $values);
  
    if ($cvterm[0]->definition) {
      $description = $cvterm[0]->definition;
    }
  }
  $rows = 1;
  
  // add one more blank set of property fields
  $form['properties']['table']['new']["new_id"] = array(
    '#type'          => 'select',
    '#options'       => $properties_select,
    '#default_value' => $id,
    '#ajax' => array(
      'callback' => "tripal_analysis_property_get_description",
      'wrapper'  => 'tripal-analysis-new_value',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
  );
  $form['properties']['table']['new']["new_value"] = array(
    '#type'           => 'textarea',
    '#default_value'  => $text,
    '#cols'           => 50,
    '#rows'           => $rows,
    '#prefix'         => '<div id="tripal-analysis-new_value">',
    '#description'    => $description,
    '#suffix'         => '</div>',
  );
  
  $form['properties']['table']['new']["add"] = array(
    '#type'    => 'button',
    '#value'   => t('Add'),
    '#name'    => 'add',
    '#ajax'      => array(
      'callback' => "tripal_analysis_property_ajax_update",
      'wrapper'  => 'tripal-analysis-edit-properties-table',
      'effect'   => 'fade',
      'method'   => 'replace',
      'prevent'  => 'click'
    ),
    // When this button is clicked, the form will be validated and submitted. 
    // Therefore, we set custom submit and validate functions to override the
    // default form submit.  In the validate function we set the form_state
    // to rebuild the form so the submit function never actually gets called,
    // but we need it or Drupal will run the default validate anyway.
    // we also set #limit_validation_errors to empty so fields that
    // are required that don't have values won't generate warnings. 
    '#submit'   => array('chado_anslysis_node_form_props_button_submit'),
    '#validate' => array('chado_anslysis_node_form_props_button_validate'),
    '#limit_validation_errors' => array(array('new_id')),
  );
}
/**
 * This function is used to rebuild the form if an ajax call is made vai a button.
 * The button causes the form to be submitted. We don't want this so we override
 * the validate and submit routines on the form button. Therefore, this function
 * only needs to tell Drupal to rebuild the form
 */
function chado_anslysis_node_form_props_button_validate($form, &$form_state){
  if (array_key_exists('triggering_element', $form_state) and 
      $form_state['triggering_element']['#name'] == 'add' and
      $form_state['input']['new_id'] == 0 ){
    form_set_error('new_id', "Please specify a property type");
    return;
  }
  $form_state['rebuild'] = TRUE; 
}
/**
 * This function is just a dummy to override the default form submit on ajax calls for buttons
 */
function chado_anslysis_node_form_props_button_submit($form, &$form_state){
  // do nothing
}
/**
 * This adds 
 */
function chado_analysis_node_form_add_new_props(&$form, &$form_state, &$ranks, &$d_removed) {
   
  // set some default values
  $j = 0;
  $num_properties = 0;
  
  $values = array();
  if (array_key_exists('values', $form_state)) {
    $values = $form_state['values'];
  }
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $values = $form_state['input'];
  }
  
  // first, add in all of the new properties that were added previously via this form
  foreach ($values as $element_name => $value) {
    if (preg_match('/new_value-(\d+)-(\d+)/', $element_name, $matches)) {
      $new_id = $matches[1];
      $rank = $matches[2];
  
      // skip any properties that the user requested to delete through a previous
      // ajax callback or through the current ajax callback
      if (array_key_exists("$new_id-$rank", $d_removed)) {
        continue;
      }
      if (array_key_exists('triggering_element', $form_state) and 
         $form_state['triggering_element']['#name'] == 'remove-' . $new_id . '-' . $rank) {
        $d_removed["$new_id-$rank"] = 1;
        continue;
      }
  
      // get this new_id information
      $args = array('cvterm_id' => $new_id);
      $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), $args);

      // add it to the $ranks array
      $ranks[$new_id][$rank]['name']  = $cvterm[0]->name;
      $ranks[$new_id][$rank]['id']    = $new_id;
      $ranks[$new_id][$rank]['value'] = $value;
      $ranks[$new_id][$rank]['definition']  = $cvterm[0]->definition;
      $num_properties++;
  
      // determine how many rows we need in the textarea
      $rows = 1;
      // add the new fields
      $form['properties']['table']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
        '#markup' => $cvterm[0]->name
      );
      $form['properties']['table']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
        '#type'          => 'textarea',
        '#default_value' => $value,
        '#cols'          => 50,
        '#rows'          => $rows,
        '#description'   => $cvterm[0]->definition,
      );
  
      $form['properties']['table']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
        '#type'         => 'button',
        '#value'        => t('Remove'),
        '#name'         => "remove-$new_id-$rank",
        '#ajax' => array(
          'callback' => "tripal_analysis_property_ajax_update",
          'wrapper'  => 'tripal-analysis-edit-properties-table',
          'effect'   => 'fade',
          'event'    => 'mousedown',
          'method'   => 'replace',
          'prevent'  => 'click'
        ),
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default form submit.  In the validate function we set the form_state
        // to rebuild the form so the submit function never actually gets called,
        // but we need it or Drupal will run the default validate anyway.
        // we also set #limit_validation_errors to empty so fields that
        // are required that don't have values won't generate warnings.
        '#submit'   => array('chado_anslysis_node_form_props_button_submit'),
        '#validate' => array('chado_anslysis_node_form_props_button_validate'),
        '#limit_validation_errors' => array(),
      );
    }
  }

  // second add in any new properties added during this callback 
  if (array_key_exists('triggering_element', $form_state) and 
      $form_state['triggering_element']['#name'] == 'add' and
      $form_state['input']['new_id'] != 0) {
    $new_id    = $form_state['input']['new_id'];
    $new_value = $form_state['input']['new_value'];    

    // get the rank by counting the number of entries
    $rank = count($ranks[$new_id]);

    // get this new_id information
    $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), array('cvterm_id' => $new_id));

    // add it to the $ranks array
    $ranks[$new_id][$rank]['name']  = $cvterm[0]->name;
    $ranks[$new_id][$rank]['id']    = $new_id;
    $ranks[$new_id][$rank]['value'] = $value;
    $ranks[$new_id][$rank]['definition']  = $cvterm[0]->definition;
    $num_properties++;

    // determine how many rows we need in the textarea
    $rows = 1;

    // add the new fields
    $form['properties']['table']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
      '#markup' => $cvterm[0]->name
    );
    $form['properties']['table']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $new_value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $cvterm[0]->definition,
    );

    $form['properties']['table']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
      '#type'         => 'button',
      '#value'        => t('Remove'),
      '#name'         => "remove-$new_id-$rank",
      '#ajax' => array(
        'callback' => "tripal_analysis_property_ajax_update",
        'wrapper'  => 'tripal-analysis-edit-properties-table',
        'effect'   => 'fade',
        'event'    => 'mousedown',
        'method'   => 'replace',
        'prevent'  => 'click'
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default form submit.  In the validate function we set the form_state
      // to rebuild the form so the submit function never actually gets called,
      // but we need it or Drupal will run the default validate anyway.
      // we also set #limit_validation_errors to empty so fields that
      // are required that don't have values won't generate warnings.
      '#submit'   => array('chado_anslysis_node_form_props_button_submit'),
      '#validate' => array('chado_anslysis_node_form_props_button_validate'),
      '#limit_validation_errors' => array(),
    );

  }

  return $num_properties;
}
/*
 *
*/
function chado_analysis_node_form_add_analysisprop_table_props(&$form, $form_state, $analysis_id, &$ranks, &$d_removed) {

  // get the properties for this analysis
  $num_properties = 0;

  if (!$analysis_id) {
    return $num_properties;
  }

  $sql = "
    SELECT CVT.cvterm_id, CVT.name, CVT.definition, PP.value, PP.rank
    FROM {analysisprop} PP
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = PP.type_id
      INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
    WHERE PP.analysis_id = :analysis_id and CV.name = 'analysis_property'
    ORDER BY CVT.name, PP.rank
  ";
  $analysis_props = chado_query($sql, array(':analysis_id' => $analysis_id));
  while ($prop = $analysis_props->fetchObject()) {

    $type_id = $prop->cvterm_id;
    $rank = 0;
    if(array_key_exists($type_id, $ranks)) {
      $rank = count($ranks[$type_id]);
    }

    // skip any properties that the user requested to delete through a previous
    // AHAH callback or through the current AHAH callback
    if (array_key_exists("$type_id-$rank", $d_removed)) {
      continue;
    }
    if (array_key_exists('triggering_element', $form_state) and 
        $form_state['triggering_element']['#name'] == 'remove-' . $type_id . '-' . $rank) {
      $d_removed["$type_id-$rank"] = 1;
      continue;
    }

    $ranks[$type_id][$rank]['name']  = $prop->name;
    $ranks[$type_id][$rank]['id']    = $type_id;
    $ranks[$type_id][$rank]['value'] = $prop->value;
    $ranks[$type_id][$rank]['definition']  = $prop->definition;
    $num_properties++;
    $rows = 1;

    $form['properties']['table'][$type_id][$rank]["prop_id-$type_id-$rank"] = array(
      '#markup'        => $prop->name,
    );
    $form['properties']['table'][$type_id][$rank]["prop_value-$type_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $prop->value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $prop->definition,
    );

    $form['properties']['table'][$type_id][$rank]["remove-$type_id-$rank"] = array(
      '#type'         => 'button',
      '#value'        => t('Remove'),
      '#name'         => "remove-$type_id-$rank",
      '#ajax' => array(
        'callback' => "tripal_analysis_property_ajax_update",
        'wrapper'  => 'tripal-analysis-edit-properties-table',
        'effect'   => 'fade',
        'event'    => 'mousedown',
        'method'   => 'replace',
        'prevent'  => 'click'
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default form submit.  In the validate function we set the form_state
      // to rebuild the form so the submit function never actually gets called,
      // but we need it or Drupal will run the default validate anyway.
      // we also set #limit_validation_errors to empty so fields that
      // are required that don't have values won't generate warnings.
      '#submit'   => array('chado_anslysis_node_form_props_button_submit'),
      '#validate' => array('chado_anslysis_node_form_props_button_validate'),
      '#limit_validation_errors' => array(),
    );
  }
  return $num_properties;
}

/**
 * Validates the user input before creating an analysis node
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_validate($node, &$form_state) {
  // use the analysis parent to validate the node
  tripal_analysis_validate($node, $form_state);
}

/**
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_validate($node, &$form_state) {
  // remove surrounding white-space on submitted values
  $node->analysisname = trim($node->analysisname);
  $node->description = trim($node->description);
  $node->program = trim($node->program);
  $node->programversion = trim($node->programversion);
  $node->algorithm = trim($node->algorithm);
  $node->sourcename = trim($node->sourcename);
  $node->sourceversion = trim($node->sourceversion);
  $node->sourceuri = trim($node->sourceuri);
 
  // Only nodes being updated will have an nid already
  if (!is_null($node->nid)) {    
    // CASE A: We are validating a form for updating an existing node
    
    // get the existing node    
    $values = array('analysis_id' => $node->analysis_id);      
    $result = tripal_core_chado_select('analysis', array('*'), $values);
    $analysis = $result[0];
      
    // if the name has changed make sure it doesn't conflict with an existing name
    if ($analysis->name != $node->analysisname) {
      $values = array('name' => $node->analysisname);
      $result = tripal_core_chado_select('analysis', array('analysis_id'), $values);
      if ($result and count($result) > 0) {
        form_set_error('analysisname', 'Cannot update the analysis with this analysis name. An analysis with this name already exists.');
        return;
      }  
    }
    
    // if the unique constraint has changed check to make sure it doesn't conflict with an
    // existing record
    if ($analysis->program != $node->program or $analysis->programversion != $node->programversion or 
       $analysis->sourcename != $node->sourcename) {
      $values = array(
        'program' => $node->program,
        'programversion' => $node->programversion,
        'sourcename' => $node->sourcename,
      );
      $result = tripal_core_chado_select('analysis', array('analysis_id'), $values);
      if ($result and count($result) > 0) {
        if ($analysis->program != $node->program) {
          $field = 'program';  
        }
        if ($analysis->programversion != $node->programversion) {
          $field = 'programversion';  
        }
        if ($analysis->sourcename != $node->sourcename) {
          $field = 'sourcename';  
        }
        form_set_error($field, 'Cannot update the analysis with this program,
          program version and source name. An analysis with these values already exists.');
        return;
      }  
    }
  }
  else {
    // To differentiate if we are syncing or creating a new analysis altogther, see if an
    // analysis_id already exists
    if (property_exists($node, 'analysis_id') and $node->analysis_id != 0) {
      // CASE B: Synchronizing a node from chado to drupal
      // we don't need to do anything.
    }
    else {
      // CASE C: We are validating a form for inserting a new node
      // The unique constraint for the chado analysis table is: program, programversion, sourcename
      $values = array(
        'program' => $node->program,
        'programversion' => $node->programversion,
        'sourcename' => $node->sourcename,
      );
      $analysis = tripal_core_chado_select('analysis', array('analysis_id'), $values);
      if ($analysis and count($analysis) > 0) {
        form_set_error('program', 'Cannot add the analysis with this program,
          program version and source name. An analysis with these values already exists.');
        return;
      }
      
      // make sure we have a unique analysis name. This is not a requirement 
      // for the analysis table but we use the analysis name for the Drupal node
      // title, so it should be unique      
      $values = array('name' => $node->analysisname);
      $result = tripal_core_chado_select('analysis', array('analysis_id'), $values);
      if ($result and count($result) > 0) {
        form_set_error('analysisname', 'Cannot add the analysis with this analysis name. An analysis with this name already exists.');
        return;
      }
    }
  }
}


/*
 *
 */
function tripal_analysis_theme_node_form_properties($form) {
  $rows = array();

  if (array_key_exists('properties', $form)) {

    // first add in the properties derived from the analysisprop table
    // the array tree for these properties looks like this:
    // $form['properties']['table'][$type_id][$rank]["prop_id-$type_id-$rank"]
    foreach ($form['properties']['table'] as $type_id => $elements) {
      // there are other fields in the properties array so we only
      // want the numeric ones those are our type_id
      if (is_numeric($type_id)) {
        foreach ($elements as $rank => $element) {
          if (is_numeric($rank)) {
            $rows[] = array(
              drupal_render($element["prop_id-$type_id-$rank"]),
              drupal_render($element["prop_value-$type_id-$rank"]),
              drupal_render($element["remove-$type_id-$rank"]),
            );
          }
        }
      }
    }

    // second, add in any new properties added by the user through AHAH callbacks
    // the array tree for these properties looks like this:
    // $form['properties']['table']['new'][$type_id][$rank]["new_id-$new_id-$rank"]
    foreach ($form['properties']['table']['new'] as $type_id => $elements) {
      if (is_numeric($type_id)) {
        foreach ($elements as $rank => $element) {
          if (is_numeric($rank)) {
            $rows[] = array(
              drupal_render($element["new_id-$type_id-$rank"]),
              drupal_render($element["new_value-$type_id-$rank"]),
              drupal_render($element["remove-$type_id-$rank"]),
            );
          }
        }
      }
    }

    // finally add in a set of blank field for adding a new property
    $rows[] = array(
      drupal_render($form['properties']['table']['new']['new_id']),
      array(
        'data' => drupal_render($form['properties']['table']['new']['new_value']),
        'width' => '60%',
      ),
      drupal_render($form['properties']['table']['new']['add']),
    );
  }
  $headers = array('Property Type', 'Value', 'Actions');
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  return theme_table($table); 
}

/**
 * Form AJAX callback for adding a blank property row
 * 
 * We only want to return the properties as that's all we'll replace with this callback
 */
function tripal_analysis_property_ajax_update($form, $form_state) {
  $properties_html = tripal_analysis_theme_node_form_properties($form); 
  $form['properties']['table'] = array(
    '#markup' => $properties_html,
    '#prefix' => '<div id="tripal-analysis-edit-properties-table">',
    '#suffix' => '</div>',
  );
  return $form['properties']['table'];
}
/**
 * Form AJAX callback for updating a property description. This 
 * function only gets called when the property drop down is changed
 * on the bottom (empty) row of properties
 */
function tripal_analysis_property_get_description($form, $form_state) {
  return $form['properties']['table']['new']["new_value"];
}

/**
 * We need to theme the analysis form so that the properties fields look good
 */
function theme_chado_analysis_form($variables) {
  $form = $variables['form'];

  $properties_table = tripal_analysis_theme_node_form_properties($form);
  $markup = $properties_table;

  $form['properties']['table'] = array(
    '#markup' => $markup,
    '#prefix' => '<div id="tripal-analysis-edit-properties-table">',
    '#suffix' => '</div>',
  );
  $form['buttons']['#weight'] = 50; 
  return drupal_render_children($form);
}