<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all 
 * of the stock relationships for this node, the following function call would be made:
 * 
 *   $stock = tripal_core_expand_chado_vars($stock,'table','stock_relationship');
 * 
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the tripal_core_expand_chado_vars function is recursive and expands 
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 * 
 *   $stock->all_relationships;
 *   
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the stock is the object, and the array with
 * the key 'subject' contains relationships where the stock is the subject
 */
$stock = $variables['node']->stock;

$all_relationships = $stock->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) { ?>
  <div class="tripal_stock-data-block-desc tripal-data-block-desc"></div> <?php
  
  // first add in the subject relationships.  
  foreach ($subject_rels as $rel_type => $rels){
    foreach ($rels as $obj_type => $objects){ ?>
      <p>This <?php print $stock->type_id->name;?> is <?php print $rel_type ?> the following <b><?php print $obj_type ?></b> stock(s): <?php
       
      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Stock Name' ,'Unique Name', 'Species', 'Type');
      
      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();
      
      foreach ($objects as $object){
        // link the stock to it's node
        $stock_name = $object->record->object_id->name;
        if (property_exists($object->record, 'nid')) {
          $stock_name = l($stock_name, "node/" . $object->record->nid, array('attributes' => array('target' => "_blank")));
        }
        // link the organism to it's node
        $organism = $object->record->object_id->organism_id;
        $organism_name = $organism->genus ." " . $organism->species;
        if (property_exists($organism, 'nid')) {
          $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
        }
        $rows[] = array(
          array('data' => $stock_name, 'width' => '30%'),
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
           'id' => 'tripal_stock-table-relationship-object',
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
      <p>The following <b><?php print $subjects[0]->record->subject_id->type_id->name ?></b> stock(s) are <?php print $rel_type ?> this <?php print $stock->type_id->name;?>: <?php 
      // the $headers array is an array of fields to use as the colum headers.
      // additional documentation can be found here
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $headers = array('Stock Name' ,'Unique Name', 'Species', 'Type');
      
      // the $rows array contains an array of rows where each row is an array
      // of values for each column of the table in that row.  Additional documentation
      // can be found here:
      // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
      $rows = array();
      
      foreach ($subjects as $subject){
        // link the stock to it's node
        $stock_name = $subject->record->subject_id->name;
        if (property_exists($subject->record, 'nid')) {
          $stock_name = l($stock_name, "node/" . $subject->record->nid, array('attributes' => array('target' => "_blank")));
        }
        // link the organism to it's node
        $organism = $subject->record->subject_id->organism_id;
        $organism_name = $organism->genus ." " . $organism->species;
        if (property_exists($organism, 'nid')) {
          $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
        }
        $rows[] = array(
          array('data' => $stock_name, 'width' => '30%'),
          array('data' => $subject->record->subject_id->uniquename, 'width' => '30%'),
          array('data' => $organism_name, 'width' => '30%'),
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
           'id' => 'tripal_stock-table-relationship-subject',
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
