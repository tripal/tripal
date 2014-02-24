<?php
/**
 * @file
 * Implements Drupal Node hooks to create the chado_analysis node content type.
 *
 * @ingroup tripal_analysis
 */

/**
 * Implements hook_node_info().
 * Provide information to drupal about the node types that we're creating
 * in this module
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_node_info() {
  $nodes = array();
  $nodes['chado_analysis'] = array(
    'name'        => t('Analysis'),
    'base'        => 'chado_analysis',
    'description' => t('An analysis'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    'chado_node_api' => array(
      'base_table' => 'analysis',
      'hook_prefix' => 'chado_analysis',
      'record_type_title' => array(
        'singular' => t('Analysis'),
        'plural' => t('Analyses')
      ),
      'sync_filters' => array(
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => array('name'),
      ),
    )
  );
  return $nodes;
}

/**
 * Implements hook_form().
 * When editing or creating a new node of type 'chado_analysis' we need
 * a form.  This function creates the form that will be used for this.
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
    $analysis = chado_expand_var($analysis, 'field', 'analysis.description');
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

    // set the analysis_id in the form
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


  $exclude = array();
  $include = array();


  // Properties Form
  // ----------------------------------
  $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the analysis_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  $prop_cv = tripal_get_default_cv('analysisprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'analysisprop',       // the name of the prop table
    'chado_id' => $analysis_id,               // the value of analysis_id for this record
    'cv_id' => $cv_id,                        // the cv.cv_id of the cv governing analysisprop.type_id
    'fieldset_title' => 'Properties',
    'additional_instructions' => $instructions
  );
  chado_add_node_form_properties($form, $form_state, $details);

  return $form;
}

/**
 * Implements hook_validate().
 * Validates the user input before creating an analysis node
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_validate($node, $form, &$form_state) {
  // use the analysis parent to validate the node
  tripal_analysis_validate($node, $form, $form_state);
}

/**
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_validate($node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a analysis_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'analysis_id') and $node->analysis_id != 0) {
    return;
  }

  // remove surrounding white-space on submitted values
  $node->analysisname = trim($node->analysisname);
  $node->description = trim($node->description);
  $node->program = trim($node->program);
  $node->programversion = trim($node->programversion);
  $node->algorithm = trim($node->algorithm);
  $node->sourcename = trim($node->sourcename);
  $node->sourceversion = trim($node->sourceversion);
  $node->sourceuri = trim($node->sourceuri);
  
  // Validating for an update
  if (!is_null($node->nid)) {

    // get the existing node
    $values = array('analysis_id' => $node->analysis_id);
    $result = chado_select_record('analysis', array('*'), $values);
    $analysis = $result[0];

    // if the name has changed make sure it doesn't conflict with an existing name
    if ($analysis->name != $node->analysisname) {
      $values = array('name' => $node->analysisname);
      $result = chado_select_record('analysis', array('analysis_id'), $values);
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
      $result = chado_select_record('analysis', array('analysis_id'), $values);
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
  // Validating for an insert
  else {
    $values = array(
      'program' => $node->program,
      'programversion' => $node->programversion,
      'sourcename' => $node->sourcename,
    );
    $analysis = chado_select_record('analysis', array('analysis_id'), $values);
    if ($analysis and count($analysis) > 0) {
      form_set_error('program', 'Cannot add the analysis with this program,
        program version and source name. An analysis with these values already exists.');
      return;
    }

    // make sure we have a unique analysis name. This is not a requirement
    // for the analysis table but we use the analysis name for the Drupal node
    // title, so it should be unique
    $values = array('name' => $node->analysisname);
    $result = chado_select_record('analysis', array('analysis_id'), $values);
    if ($result and count($result) > 0) {
      form_set_error('analysisname', 'Cannot add the analysis with this analysis name. An analysis with this name already exists.');
      return;
    }
  }
}

/**
 * Implements hook_insert().
 * When a new chado_analysis node is created we also need to add information
 * to our chado_analysis table.  This function is called on insert of a new
 * node of type 'chado_analysis' and inserts the necessary information.
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_insert($node) {

  $node->analysisname = trim($node->analysisname);
  $node->description = trim($node->description);
  $node->program = trim($node->program);
  $node->programversion = trim($node->programversion);
  $node->algorithm = trim($node->algorithm);
  $node->sourcename = trim($node->sourcename);
  $node->sourceversion = trim($node->sourceversion);
  $node->sourceuri = trim($node->sourceuri);

  // if there is an analysis_id in the $node object then this must be a sync so
  // we can skip adding the analysis as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'analysis_id')) {

    // Create a timestamp so we can insert it into the chado database
    $time  = $node->timeexecuted;
    $month = $time['month'];
    $day   = $time['day'];
    $year  = $time['year'];
    $timestamp = $month . '/' . $day . '/' . $year;

    // insert and then get the newly inserted analysis record
    $values = array(
      'name'           => $node->analysisname,
      'description'    => $node->description,
      'program'        => $node->program,
      'programversion' => $node->programversion,
      'algorithm'      => $node->algorithm,
      'sourcename'     => $node->sourcename,
      'sourceversion'  => $node->sourceversion,
      'sourceuri'      => $node->sourceuri,
      'timeexecuted'   => $timestamp
    );
    $analysis = chado_insert_record('analysis', $values);
    if (!$analysis) {
      drupal_set_message(t('Unable to add analysis.', 'warning'));
      tripal_report_error('tripal_analysis', TRIPAL_ERROR, 'Insert analysis: Unable to create analysis where values:%values',
        array('%values' => print_r($values, TRUE)));
      return;
    }
    $analysis_id = $analysis['analysis_id'];

    // now add in the properties
    $details = array(
      'property_table' => 'analysisprop',
      'base_table' => 'analysis',
      'foreignkey_name' => 'analysis_id',
      'foreignkey_value' => $analysis_id
    );
    chado_update_node_form_properties($node, $details);
  }
  else {
    $analysis_id = $node->analysis_id;
  }

  // Make sure the entry for this analysis doesn't already exist in the
  // chado_analysis table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('analysis', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->analysis_id = $analysis_id;
    drupal_write_record('chado_analysis', $record);
  }

  // add the analysis to the node object for
  // use by other analysis modules that may be using this function
  $node->analysis = $analysis;
  $node->analysis_id = $analysis_id; // we need to set this for children
}

/**
 * Implements hook_delete().
 * Removes analysis from the chado database.
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_delete($node) {
  $analysis_id = chado_get_id_from_nid('analysis', $node->nid);

  // if we don't have an analysis id for this node then this isn't a node of
  // type chado_analysis or the entry in the chado_analysis table was lost.
  if (!$analysis_id) {
    return;
  }

  // Remove data from the {chado_analysis}, {node}, and {node_revisions} tables
  $sql_del = "DELETE FROM {chado_analysis} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  //Remove from analysis and analysisprop tables of chado database as well
  chado_query("DELETE FROM {analysis} WHERE analysis_id = :analysis_id", array(':analysis_id' => $analysis_id));
}

/**
 * Implements hook_update().
 * Update analyses
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_update($node) {
  $node->analysisname = trim($node->analysisname);
  $node->description = trim($node->description);
  $node->program = trim($node->program);
  $node->programversion = trim($node->programversion);
  $node->algorithm = trim($node->algorithm);
  $node->sourcename = trim($node->sourcename);
  $node->sourceversion = trim($node->sourceversion);
  $node->sourceuri = trim($node->sourceuri);

  // Create a timestamp so we can insert it into the chado database
  $time = $node->timeexecuted;
  $month = $time['month'];
  $day = $time['day'];
  $year = $time['year'];
  $timestamp = $month . '/' . $day . '/' . $year;

  // update the record in Chado
  $analysis_id = chado_get_id_from_nid('analysis', $node->nid);
  $match = array(
    'analysis_id' => $node->analysis_id,
  );
  $values = array(
    'name'           => $node->analysisname,
    'description'    => $node->description,
    'program'        => $node->program,
    'programversion' => $node->programversion,
    'algorithm'      => $node->algorithm,
    'sourcename'     => $node->sourcename,
    'sourceversion'  => $node->sourceversion,
    'sourceuri'      => $node->sourceuri,
    'timeexecuted'   => $timestamp,
    'analysis_id'    => $analysis_id
  );
  $status = chado_update_record('analysis', $match, $values);
  if (!$status) {
    drupal_set_message(t('Unable to update analysis.', 'warning'));
    tripal_report_error('tripal_analysis', TRIPAL_ERROR, 'Update analysis: Unable to update analysis where values: %values',
    array('%values' => print_r($values, TRUE)));
  }

  // now add in the properties by first removing any the analysis
  // already has and adding the ones we have
  $details = array(
    'property_table' => 'analysisprop',
    'base_table' => 'analysis',
    'foreignkey_name' => 'analysis_id',
    'foreignkey_value' => $analysis_id
  );
  chado_update_node_form_properties($node, $details);
}

/**
 * Implements hook_load().
 * When a node is requested by the user this function is called to allow us
 * to add auxiliary data to the node object.
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // find the analysis and add in the details
    $analysis_id = chado_get_id_from_nid('analysis', $nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$analysis_id) {
      continue;
    }
    
    // build the analysis variable
    $values = array('analysis_id' => $analysis_id);
    $analysis = chado_generate_var('analysis', $values);

    // add in the description field
    $analysis = chado_expand_var($analysis, 'field', 'analysis.description');
    $nodes[$nid]->analysis = $analysis;
  }
}

/**
 * Implements hook_access().
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
 * @ingroup tripal_analysis
 */
function chado_analysis_node_access($node, $op, $account) {

  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_analysis') {
    if ($op == 'create') {
      if (!user_access('create chado_analysis content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_analysis content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_analysis content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_analysis content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_node_view().
 * Called for all node types.
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_analysis':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_analysis_base'] = array(
          '#markup' => theme('tripal_analysis_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_analysis_properties'] = array(
          '#markup' => theme('tripal_analysis_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_analysis_teaser'] = array(
          '#markup' => theme('tripal_analysis_teaser', array('node' => $node)),
        );
      }
      break;
  }
}

/**
 * Implements hook_node_presave().
 * Called for all node types.
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_node_presave($node) {
  $name = '';
  $program = '';
  $programversion = '';
  $sourcename = '';

  // If this is an analysis of some type it will should have three required
  // fields for the Chado analysis table: program, programversion and sourcename.
  // So we will set the title for any node that has these three fields. Some extension
  // modules will use this module as a type of "inherited" class, so we don't know
  // for sure when type of analysis we have.  If this is a sync then
  if (property_exists($node, 'program') and
      property_exists($node, 'programversion') and
      property_exists($node, 'sourcename')) {
    $name           = $node->analysisname;
    $program        = $node->program;
    $programversion = $node->programversion;
    $sourcename     = $node->sourcename;
    // now construct the title
    $node->title = "$program ($programversion) $sourcename";
    if ($name) {
      $node->title = $name;
    }
  }
  else if (property_exists($node, 'analysis')) {
    $name           = $node->analysis->name;
    $program        = $node->analysis->program;
    $programversion = $node->analysis->programversion;
    $sourcename     = $node->analysis->sourcename;
    // now construct the title
    $node->title = "$program ($programversion) $sourcename";
    if ($name) {
      $node->title = $name;
    }
  }
}
