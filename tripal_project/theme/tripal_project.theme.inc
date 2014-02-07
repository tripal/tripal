<?php
/**
 *
 *
 * @ingroup tripal_project
 */
function tripal_project_preprocess_tripal_project_relationships(&$variables) {
  $project = $variables['node']->project;
  
  // expand the project object to include the project relationships.
  $options = array(
    'return_array' => 1,
    // we don't want to fully recurse we only need information about the 
    // relationship type and the object and subject projects
    'include_fk' => array(
      'type_id'    => 1,
      'object_project_id'  => 1,
      'subject_project_id' => 1,
    ),
  );
  $project = chado_expand_var($project, 'table', 'project_relationship', $options);
  
  // get the subject relationships
  $srelationships = $project->project_relationship->subject_project_id;
  $orelationships = $project->project_relationship->object_project_id;
  
  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();
  
  // iterate through the object relationships
  if ($orelationships) {
    foreach ($orelationships as $relationship) {      
       $rel = new stdClass(); 
       $rel->record = $relationship;
       
       // get the relationship and child types
       $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));

       // get the node id of the subject
       $sql = "SELECT nid FROM {chado_project} WHERE project_id = :project_id";
       $n = db_query($sql, array(':project_id' => $relationship->subject_project_id->project_id))->fetchObject();
       if ($n) {
          $rel->record->nid = $n->nid;
       }
  
       if (!array_key_exists($rel_type, $relationships['object'])) {
         $relationships['object'][$rel_type][] = $rel;
       }
    }
  }
  
  // now add in the subject relationships
  if ($srelationships) {
    foreach ($srelationships as $relationship) {
       $rel = new stdClass(); 
       $rel->record = $relationship;
       
       $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
       
       // get the node id of the subject
       $sql = "SELECT nid FROM {chado_project} WHERE project_id = :project_id";
       $n = db_query($sql, array(':project_id' => $relationship->object_project_id->project_id))->fetchObject();
       if ($n) {
          $rel->record->nid = $n->nid;
       }
       
       if (!array_key_exists($rel_type, $relationships['subject'])) {
         $relationships['subject'][$rel_type][] = $rel;
       }
    }
  }
  $project->all_relationships = $relationships;
}