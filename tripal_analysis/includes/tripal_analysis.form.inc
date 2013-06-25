<?php 
/**
 *  When editing or creating a new node of type 'chado_analysis' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_form(&$node, $form_state = NULL) {
  tripal_core_ahah_init_form();
  $form = array();

  $analysis = $node->analysis;

  // add in the description column. It is a text field and may not be included
  // if the text is too big.
  $analysis = tripal_core_expand_chado_vars($analysis, 'field', 'analysis.description');

  // get form defaults
  $analysis_id = $node->analysis_id;
  if (!$analysis_id) {
    $analysis_id = $analysis->analysis_id;
  }
  $analysisname = $node->analysisname;
  if (!$analysisname) {
    $analysisname = $analysis->name;
  }
  $program = $node->program;
  if (!$program) {
    $program = $analysis->program;
  }
  $programversion = $node->programversion;
  if (!$programversion) {
    $programversion = $analysis->programversion;
  }
  $algorithm = $node->algorithm;
  if (!$algorithm) {
    $algorithm = $analysis->algorithm;
  }
  $sourcename = $node->sourcename;
  if (!$sourcename) {
    $sourcename = $analysis->sourcename;
  }
  $sourceversion = $node->sourceversion;
  if (!$sourceversion) {
    $sourceversion = $analysis->sourceversion;
  }
  $sourceuri = $node->sourceuri;
  if (!$sourceuri) {
    $sourceuri = $analysis->sourceuri;
  }
  $timeexecuted = $node->timeexecuted;
  if (!$timeexecuted) {
    $timeexecuted = $analysis->timeexecuted;
  }
  $description = $node->description;
  if (!$description) {
    $description = $analysis->description;
  }
  
  // on AHAH callbacks we want to keep a list of all the properties that have been removed
  // we'll store this info in a hidden field and retrieve it here
  $d_removed = $form_state['values']['removed'];

  // get the number of new fields that have been aded via AHAH callbacks
  $num_new = $form_state['values']['num_new'] ? $form_state['values']['num_new'] : 0;

  // initialze default properties array. This is where we store the property defaults
  $d_properties = array();
  
  
  $form['title']= array(
      '#type' => 'hidden',
      '#default_value' => $node->title,
  );
  $form['analysis_id']= array(
      '#type' => 'hidden',
      '#default_value' => $analysis_id,
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
      '#title' => t('Program'),
      '#required' => TRUE,
      '#default_value' => $program,
      '#description' => t("Program name, e.g. blastx, blastp, sim4, genscan."),
  );
  $form['programversion']= array(
      '#type' => 'textfield',
      '#title' => t('Program Version'),
      '#required' => TRUE,
      '#default_value' => $programversion,
      '#description' => t("Version description, e.g. TBLASTX 2.0MP-WashU [09-Nov-2000]. Enter 'n/a' if no version is available."),
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
    $default_time = time();
    $year = format_date($default_time, 'custom', 'Y');
    $month = format_date($default_time, 'custom', 'n');
    $day = format_date($default_time, 'custom', 'j');
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
  while ($prop = db_fetch_object($prop_types)) {
  	$properties_select[$prop->cvterm_id] = $prop->name;
  	$properties_list[$prop->cvterm_id] = $prop;
  }
  
  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Analysis Details'),
    '#description' => t('You may add additional 
      properties by
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
  
  // add in the properties from the analysisprop table
  $num_properties += chado_analysis_node_form_add_analysisprop_table_props($form, $form_state, $analysis_id, $d_properties, $d_removed);
  
  // add in any new properties that have been added by the user through an AHAH callback
  $num_new = chado_analysis_node_form_add_new_props($form, $form_state, $d_properties, $d_removed);
  
  // add an empty row of field to allow for addition of a new property
  chado_analysis_node_form_add_new_empty_props($form, $properties_select);

  return $form;
}

/**
 * Validates the user input before creating an analysis node
 *
 * @ingroup tripal_analysis
 */
function chado_analysis_validate($node, &$form) {
   // use the analysis parent to validate the node
  tripal_analysis_validate($node, $form);
}

/**
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_validate($node, &$form) {
  
 
  // Only nodes being updated will have an nid already
  if (!is_null($node->nid)) {    
    // CASE A: We are validating a form for updating an existing node
    
    // get the existing node    
    $values = array('analysis_id' => $node->analysis_id);      
    $result = tripal_core_chado_select('analysis', array('*'), $values);
    $analysis = $result[0];
      
    // if the name has changed make sure it doesn't conflict with an existing name
    if($analysis->name != $node->analysisname) {
      $values = array('name' => $node->analysisname);
      $result = tripal_core_chado_select('analysis', array('analysis_id'), $values);
      if($result and count($result) > 0) {
        form_set_error('analysisname', 'Cannot update the analysis with this analysis name. An analysis with this name already exists.');
        return;
      }  
    }
    
    // if the unique constraint has changed check to make sure it doesn't conflict with an
    // existing record
    if($analysis->program != $node->program or $analysis->programversion != $node->programversion or 
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
  else{
    // To differentiate if we are syncing or creating a new analysis altogther, see if an
    // analysis_id already exists
    if ($node->analysis_id and $node->analysis_id != 0) {
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
      if($result and count($result) > 0) {
        form_set_error('analysisname', 'Cannot add the analysis with this analysis name. An analysis with this name already exists.');
        return;
      }
    }
  }
}

/*
 *
 */
function chado_analysis_node_form_add_new_empty_props(&$form, $properties_select) {

  // add one more blank set of property fields
  $form['properties']['table']['new']["new_id"] = array(
    '#type'          => 'select',
    '#options'       => $properties_select,
    '#ahah' => array(
      'path'    => "tripal_analysis/properties/description",
      'wrapper' => 'tripal-analysis-new_value-desc',
      'event'   => 'change',
      'method'  => 'replace',          
  ),
  );
  $form['properties']['table']['new']["new_value"] = array(
    '#type'          => 'textarea',
    '#default_value' => '',
    '#cols'          => 5,
    '#rows'          => $rows,
    '#description'   => '<div id="tripal-analysis-new_value-desc"></div>'
    );
    $form['properties']['table']['new']["add"] = array(
    '#type'         => 'image_button',      
    '#value'        => t('Add'),
    '#src'          => drupal_get_path('theme', 'tripal') . '/images/add.png',
    '#ahah' => array(
      'path'    => "tripal_analysis/properties/add",
      'wrapper' => 'tripal-analysis-edit-properties-table',
      'event'   => 'click',
      'method'  => 'replace',          
    ),
    '#attributes' => array('onClick' => 'return false;'),
    );
}
/*
 *
 */
function chado_analysis_node_form_add_new_props(&$form, $form_state, &$d_properties, &$d_removed) {
   
  // first, add in all of the new properties that were added through a previous AHAH callback
  $j = 0;
  $num_properties++;

  // we need to find the
  if ($form_state['values']) {
    foreach ($form_state['values'] as $element_name => $value) {
      if (preg_match('/new_value-(\d+)-(\d+)/', $element_name, $matches)) {
        $new_id = $matches[1];
        $rank = $matches[2];

        // skip any properties that the user requested to delete through a previous
        // AHAH callback or through the current AHAH callback
        if($d_removed["$new_id-$rank"]) {
          continue;
        }
        if($form_state['post']['remove-' . $new_id . '-' . $rank]) {
          $d_removed["$new_id-$rank"] = 1;
          continue;
        }

        // get this new_id information
        $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), array('cvterm_id' => $new_id));

        // add it to the $d_properties array
        $d_properties[$new_id][$rank]['name']  = $cvterm->name;
        $d_properties[$new_id][$rank]['id']    = $new_id;
        $d_properties[$new_id][$rank]['value'] = $value;
        $d_properties[$new_id][$rank]['definition']  = $cvterm->definition;
        $num_properties++;

        // determine how many rows we need in the textarea
        $rows = 1;


        // add the new fields
        $form['properties']['table']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
          '#type'          => 'item',
          '#value'         => $cvterm[0]->name
        );
        $form['properties']['table']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
          '#type'          => 'textarea',
          '#default_value' => $value,
          '#cols'          => 50,
          '#rows'          => $rows,
          '#description'   => $cvterm->definition,
        );

        $form['properties']['table']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
          '#type'         => 'image_button',
          '#value'        => t('Remove'),
          '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
          '#ahah' => array(
            'path'    => "tripal_analysis/properties/minus/$new_id/$rank",
            'wrapper' => 'tripal-analysis-edit-properties-table',
            'event'   => 'click',
            'method'  => 'replace',
        ),
          '#attributes' => array('onClick' => 'return false;'),
        );
      }
    }
  }


  // second add in any new properties added during this callback
  if($form_state['post']['add']) {
    $new_id = $form_state['values']['new_id'];
    $new_value = $form_state['values']['new_value'];

    // get the rank by counting the number of entries
    $rank = count($d_properties[$new_id]);

    // get this new_id information
    $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), array('cvterm_id' => $new_id));

    // add it to the $d_properties array
    $d_properties[$new_id][$rank]['name']  = $cvterm->name;
    $d_properties[$new_id][$rank]['id']    = $new_id;
    $d_properties[$new_id][$rank]['value'] = $value;
    $d_properties[$new_id][$rank]['definition']  = $cvterm->definition;
    $num_properties++;

    // determine how many rows we need in the textarea
    $rows = 1;

    // add the new fields
    $form['properties']['table']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
      '#type'          => 'item',
      '#value'         => $cvterm[0]->name
    );
    $form['properties']['table']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $new_value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $cvterm->definition,
    );

    $form['properties']['table']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
      '#type'         => 'image_button',
      '#value'        => t('Remove'),
      '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
      '#ahah' => array(
        'path'    => "tripal_analysis/properties/minus/$new_id/$rank",
        'wrapper' => 'tripal-analysis-edit-properties-table',
        'event'   => 'click',
        'method'  => 'replace',
    ),
      '#attributes' => array('onClick' => 'return false;'),
    );

  }

  return $num_properties;
}
/*
 *
 */
function chado_analysis_node_form_add_analysisprop_table_props(&$form, $form_state, $analysis_id, &$d_properties, &$d_removed) {

  // get the properties for this analysis
  $num_properties = 0;

  if(!$analysis_id) {
    return $num_properties;
  }

  $sql = "
    SELECT CVT.cvterm_id, CVT.name, CVT.definition, PP.value, PP.rank
    FROM {analysisprop} PP
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = PP.type_id
      INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
    WHERE PP.analysis_id = %d and CV.name = 'analysis_property'
    ORDER BY CVT.name, PP.rank
  ";
  $analysis_props = chado_query($sql, $analysis_id);
  while ($prop = db_fetch_object($analysis_props)) {

    $type_id = $prop->cvterm_id;
    $rank = count($d_properties[$type_id]);
    
    // skip any properties that the user requested to delete through a previous
    // AHAH callback or through the current AHAH callback
    if($d_removed["$type_id-$rank"]) {
      continue;
    }
    if($form_state['post']['remove-' . $type_id . '-' . $rank]) {
      $d_removed["$type_id-$rank"] = 1;
      continue;
    }

    $d_properties[$type_id][$rank]['name']  = $prop->name;
    $d_properties[$type_id][$rank]['id']    = $type_id;
    $d_properties[$type_id][$rank]['value'] = $prop->value;
    $d_properties[$type_id][$rank]['definition']  = $prop->definition;
    $num_properties++;

    $form['properties']['table'][$type_id][$rank]["prop_id-$type_id-$rank"] = array(
      '#type'          => 'item',
      '#value'         => $prop->name,
    );
    $form['properties']['table'][$type_id][$rank]["prop_value-$type_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $prop->value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $prop->definition,
    );

    $form['properties']['table'][$type_id][$rank]["remove-$type_id-$rank"] = array(
      '#type'         => 'image_button',
      '#value'        => t('Remove'),
      '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
      '#ahah' => array(
        'path'    => "tripal_analysis/properties/minus/$type_id/$rank",
        'wrapper' => 'tripal-analysis-edit-properties-table',
        'event'   => 'click',
        'method'  => 'replace',
    ),
      '#attributes' => array('onClick' => 'return false;'),
    );
  }
  return $num_properties;
}
/*
 *
 */
function tripal_analysis_theme_node_form_properties($form) {
  $rows = array();

  if ($form['properties']) {

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
    drupal_render($form['properties']['table']['new']['new_value']),
    drupal_render($form['properties']['table']['new']['add']),
    );
  }

  $headers = array('Property Type','Value', '');
  return theme('table', $headers, $rows);
}

/*
 *
 */
function tripal_analysis_property_add() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();

  // we only want to return the properties as that's all we'll replace with this AHAh callback
  $data = tripal_analysis_theme_node_form_properties($form);

  // bind javascript events to the new objects that will be returned
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )
  );
}
/*
 *
 */
function tripal_analysis_property_delete() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();

  // we only want to return the properties as that's all we'll replace with this AHAh callback
  $data = tripal_analysis_theme_node_form_properties($form);

  // bind javascript events to the new objects that will be returned
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )
  );
}
/*
 *
 */
function tripal_analysis_property_get_description() {
  $new_id = $_POST['new_id'];

  $values = array('cvterm_id' => $new_id);
  $cvterm = tripal_core_chado_select('cvterm', array('definition'), $values);

  $description = '&nbsp;';
  if ($cvterm[0]->definition) {
    $description = $cvterm[0]->definition;
  }
  drupal_json(
    array(
      'status' => TRUE,
      'data'   => '<div id="tripal-analysis-new_value-desc">' . $description . '</div>',
    )
  );
}
/*
 *
 */
function theme_chado_analysis_node_form($form) {

  $properties_table = tripal_analysis_theme_node_form_properties($form);
  $markup .= $properties_table;

  $form['properties']['table'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
    '#prefix' => '<div id="tripal-analysis-edit-properties-table">',
    '#suffix' => '</div>',
  );
  $form['buttons']['#weight'] = 50;
  return drupal_render($form);
}