<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all 
 * of the pub relationships for this node, the following function call would be made:
 * 
 *   $pub = tripal_core_expand_chado_vars($pub,'table','pub_relationship');
 * 
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the tripal_core_expand_chado_vars function is recursive and expands 
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 * 
 *   $pub->all_relationships;
 *   
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the pub is the object, and the array with
 * the key 'subject' contains relationships where the pub is the subject
 */
$pub = $variables['node']->pub;

$all_relationships = $pub->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) { ?>
  <div class="tripal_pub-data-block-desc tripal-data-block-desc"></div> <?php
  
  // first add in the subject relationships.  
  foreach ($subject_rels as $rel_type => $rels){
    foreach ($rels as $obj_type => $objects){ ?>
      <p>This <?php print $pub->type_id->name;?> is <?php print $rel_type ?> the following <b><?php print $obj_type ?></b> pub(s): <?php
       
      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Publication');
      
      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();
      
      foreach ($objects as $object){
        // link the pub to it's node
        $title = $object->record->object_id->title;
        if (property_exists($object->record, 'nid')) {
          $title = l($title, "node/" . $object->record->nid, array('attributes' => array('target' => "_blank")));
        }
        
        // get the citation
        $values = array(
          'pub_id' => $object->record->object_id->pub_id,
          'type_id' => array(
            'name' => 'Citation',
          ),
        );
        $citation = tripal_core_generate_chado_var('pubprop', $values);
        $citation = tripal_core_expand_chado_vars($citation, 'field', 'pubprop.value');
        
        $rows[] = array(
          $title . '<br>' . htmlspecialchars($citation->value),
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
           'id' => 'tripal_pub-table-relationship-object',
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
      <p>The following <b><?php print $subjects[0]->record->subject_id->type_id->name ?></b> pub(s) are <?php print $rel_type ?> this <?php print $pub->type_id->name;?>: <?php 
      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Publication');
      
      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();
      
      foreach ($subjects as $subject){
        // link the pub to it's node
        $title = $subject->record->subject_id->title;
        if (property_exists($subject->record, 'nid')) {
          $title = l($title, "node/" . $subject->record->nid, array('attributes' => array('target' => "_blank")));
        }
        
        // get the citation
        $values = array(
          'pub_id' => $subject->record->subject_id->pub_id,
          'type_id' => array(
            'name' => 'Citation',
          ),
        );
        $citation = tripal_core_generate_chado_var('pubprop', $values);
        $citation = tripal_core_expand_chado_vars($citation, 'field', 'pubprop.value');
        
        $rows[] = array(
          $title . '<br>' . htmlspecialchars($citation->value),
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
           'id' => 'tripal_pub-table-relationship-subject',
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
