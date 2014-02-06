<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all 
 * of the project relationships for this node, the following function call would be made:
 * 
 *   $project = tripal_core_expand_chado_vars($project,'table','project_relationship');
 * 
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the tripal_core_expand_chado_vars function is recursive and expands 
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 * 
 *   $project->all_relationships;
 *   
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the project is the object, and the array with
 * the key 'subject' contains relationships where the project is the subject
 */
$project = $variables['node']->project;

$all_relationships = $project->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) { ?>
  <div class="tripal_project-data-block-desc tripal-data-block-desc">This project is related to the following other projects:</div> <?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Relationship');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  // first add in the subject relationships.  
  foreach ($subject_rels as $rel_type => $objects){ 
    foreach ($objects as $object){
      // link the project to it's node
      $object_name = $object->record->object_project_id->name;
      if (property_exists($object->record, 'nid')) {
        $object_name = l($object_name, "node/" . $object->record->nid, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array(
        "$project->name is a \"$rel_type\" of $object_name",
      ); 
    }
  }
  
  // second add in the object relationships.  
  foreach ($object_rels as $rel_type => $subjects){
    foreach ($subjects as $subject){
      // link the project to it's node
      $subject_name = $subject->record->subject_project_id->name;
      if (property_exists($subject->record, 'nid')) {
        $subject_name = l($subject_name, "node/" . $subject->record->nid, array('attributes' => array('target' => "_blank")));
      }
      $rows[] = array(
        "$subject_name is a \"$rel_type\" of $project->name",
      ); 
    }
  }
  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_project-table-relationship-subject',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
   
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table); 
}
