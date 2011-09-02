<?php

function tripal_project_administration_description_page () {
 $output = '';
 
 
 return $output;
}

function tripal_project_configuration_page () {
  $output = '';
  
  $output .= drupal_get_form('tripal_project_sync_projects_form');
  
  return $output;
}

function tripal_project_sync_projects_form ($form_state = NULL) {
  $form = array();
  
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => 'Sync Projects'
  );
  
  $form['sync']['description'] = array(
    '#type' => 'item',
    '#value' => 'Many of the details for projects are stored in chado. Often other tripal '
      .'modules may create projects as a means of grouping data together. Sync\'ing projects '
      .'in chado created drupal pages (known as nodes) which display the data to priviledged users.'
  );
  
  $form['sync']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Sync All Projects',
    
  );
  
  return $form;
}

function tripal_project_sync_projects_form_submit ($form, &$form_state) {
  global $user;
  
  //sync'ing is done by a tripal_job that is added here
  $job_id = tripal_add_job('Sync Projects', 'tripal_project', 
	'tripal_project_sync_all_projects', array(), $user->uid);

}

function tripal_project_sync_all_projects () {

  //retrieve all projects in drupal
  $resource = db_query('SELECT project_id FROM {chado_project}');
  $drupal_projects = array();
  while ($r = db_fetch_object($resource)) {
    $drupal_projects[$r->project_id] = $r->project_id;
  }
  
  // retrieve all projects in chado
  $chado_projects = array();
  $previous_db = tripal_db_set_active('chado');
  $resource = db_query('SELECT project_id FROM project');
  tripal_db_set_active($previous_db);
  while ($r = db_fetch_object($resource)) {
    // if not already in drupal add to list to be sync'd
    if (!isset($drupal_projects[$r->project_id])) {
      $chado_projects[$r->project_id] = $r->project_id;
    }
  }
  
  print 'Number of Projects to Sync: '.sizeof($chado_projects)."\n";
  
  foreach ($chado_projects as $project_id) {
    $project = tripal_core_chado_select('project',array('name','description'),array('project_id'=>$project_id));
    print 'Project:'.print_r($project,TRUE)."\n";
    print 'Sync\'ing '.$project[0]->name.'...';
    
    // create node
    $form_state = array();
    $form_state['values']['name'] = $project[0]->name;
    $form_state['values']['description'] = $project[0]->description;
    $form_state['values']['project_id'] = $project_id;
    $form_state['clicked_button'] = 'Save';
    drupal_execute('tripal_project_node_form',$form_state, (object)array('type' => 'tripal_project'));
    $errors = form_get_errors();
    if ($errors) {
      print "Error Encountered!\n";
      print "Errors:".print_r($errors,TRUE)."\n";
    } else {
      print "Successful (".$node->nid.")\n";
    }
    
  }
  
  
}
