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
  
  // get the analysis properties
  $properties = array();
  $properties[] = 'Select a Property';
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
    $properties[$prop->cvterm_id] = $prop->name;
  }
  
  $exclude = array();
  $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the analysis_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  tripal_core_properties_form($form, $form_state, 'analysisprop', 'analysis_id', 'analysis_property',
    $properties, $analysis_id, $exclude, $instructions);

  return $form;
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