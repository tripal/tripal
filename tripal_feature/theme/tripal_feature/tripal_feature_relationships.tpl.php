<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all 
 * of the feature relationships for this node, the following function call would be made:
 * 
 *   $feature = tripal_core_expand_chado_vars($feature,'table','feature_relationship');
 * 
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the tripal_core_expand_chado_vars function is recursive and expands 
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 * 
 *   $feature->all_relationships;
 *   
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the feature is the object, and the array with
 * the key 'subject' contains relationships where the feature is the subject
 */
$feature = $variables['node']->feature;

$all_relationships = $feature->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) { ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc"></div> <?php
  
  // first add in the subject relationships.  
  foreach ($subject_rels as $rel_type => $rels){
    foreach ($rels as $obj_type => $objects){ ?>
      <p>This <?php print $feature->type_id->name;?> is <?php print $rel_type ?> the following <b><?php print $obj_type ?></b> feature(s): <?php
       
      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Feature Name' ,'Unique Name', 'Species', 'Type');
      
      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();
      
      foreach ($objects as $object){
        // link the feature to it's node
        $feature_name = $object->record->object_id->name;
        if (property_exists($object->record, 'nid')) {
          $feature_name = l($feature_name, "node/" . $object->record->nid, array('attributes' => array('target' => "_blank")));
        }
        // link the organism to it's node
        $organism = $object->record->object_id->organism_id;
        $organism_name = $organism->genus ." " . $organism->species;
        if (property_exists($organism, 'nid')) {
          $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
        }
        $rows[] = array(
          array('data' => $feature_name, 'width' => '30%'), 
          array('data' => $object->record->object_id->uniquename, 'width' => '30%'),
          array('data' => $organism_name, 'width' => '30%'),
          array('data' => $object->record->object_id->type_id->name, 'width' => '10%'),
        ); 
       } 
       // the $table array contains the headers and rows array as well as other
       // options for controlling the display of the table.  Additional
       // documentation can be found here:
       // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
       $table = array(
         'header' => $headers,
         'rows' => $rows,
         'attributes' => array(
           'id' => 'tripal_feature-table-relationship-object',
         ),
         'sticky' => FALSE,
         'caption' => '',
         'colgroups' => array(),
         'empty' => '',
       );
       
       // once we have our table array structure defined, we call Drupal's theme_table()
       // function to generate the table.
       print theme_table($table); ?>
       </p>
       <br><?php
     }
  }
  
  // second add in the object relationships.  
  foreach ($object_rels as $rel_type => $rels){
    foreach ($rels as $subject_type => $subjects){?>
      <p>The following <b><?php print $subjects[0]->record->subject_id->type_id->name ?></b> feature(s) are <?php print $rel_type ?> this <?php print $feature->type_id->name;?>: <?php 
      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Feature Name' ,'Unique Name', 'Species', 'Type');
      
      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();
      
      foreach ($subjects as $subject){
        // link the feature to it's node
        $feature_name = $subject->record->subject_id->name;
        if (property_exists($subject->record, 'nid')) {
          $feature_name = l($feature_name, "node/" . $subject->record->nid, array('attributes' => array('target' => "_blank")));
        }
        // link the organism to it's node
        $organism = $subject->record->subject_id->organism_id;
        $organism_name = $organism->genus ." " . $organism->species;
        if (property_exists($organism, 'nid')) {
          $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
        }
        $rows[] = array(
          array('data' => $feature_name, 'width' => '30%'),
          array('data' =>$subject->record->subject_id->uniquename, 'width' => '30%'),
          array('data' =>$organism_name, 'width' => '30%'),
          array('data' =>$subject->record->subject_id->type_id->name, 'width' => '10%'),
        ); 
       } 
       // the $table array contains the headers and rows array as well as other
       // options for controlling the display of the table.  Additional
       // documentation can be found here:
       // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
       $table = array(
         'header' => $headers,
         'rows' => $rows,
         'attributes' => array(
           'id' => 'tripal_feature-table-relationship-subject',
         ),
         'sticky' => FALSE,
         'caption' => '',
         'colgroups' => array(),
         'empty' => '',
       );
       
       // once we have our table array structure defined, we call Drupal's theme_table()
       // function to generate the table.
       print theme_table($table); ?>
       </p>
       <br><?php
     }
  }
}
