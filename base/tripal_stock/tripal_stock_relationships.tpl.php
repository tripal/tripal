<?php
// Copyright 2010 University of Saskatchewan (Lacey-Anne Sanderson)
//
// Purpose: Provides layout and content for Stock Relationships where
//   the current stock is the Subject of the relationships. This includes all 
//   fields in the stock_relationship table.
//
// Note: This template controls the layout/content for the default stock node
//   template (node-chado_stock.tpl.php) and the Stock Object Relationships Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes stock
//       specific fields such as stock_name, uniquename, stock_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   - $node->object_relationships: an array of stock relaionship objects 
//       where each object has the following fields: stock_relationship_id,
//       subject_id (current stock_id), type_id, type, value, rank, object
//   - $node->object_relationships->object: a stock object describing the
//       object stock with the fields: stock_id, stock_name, uniquename, 
//       description, stock_type_id, organism(object), man_db_reference(object),
//       nid (if sync'd with Drupal)
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php


// expand the stock object to include the stock relationships.
// since there two foreign keys (object_id and subject_id) in the 
// stock_relationship table, we will access each one separately 
$node = tripal_core_expand_chado_vars($node,
   'table','stock_relationship', array('order_by'=>array('rank' => 'ASC')));

 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
 
?>

<div id="tripal_stock-relationships-box" class="tripal_stock-info-box tripal-info-box"> 
<div class="tripal_stock-info-box-title tripal-info-box-title">Relationships</div> 

<?php
  /////////////////////////////////////////////////
  // Subject Relationships
  /////////////////////////////////////////////////
  
  $relationships = $node->stock->stock_relationship->subject_id;
  if (!$relationships) {
    $relationships = array();
  } elseif (!is_array($relationships)) { 
    $relationships = array($relationships); 
  }
?>

  <div class="tripal_stock-info-box-desc tripal-info-box-desc">Subject relationships</div> 
  
  <?php if(count($relationships) > 0){ ?>
  <table class="tripal_stock-table tripal-table tripal-table-horz">
    <tr>
      <th>Current Stock (Subject)</th>
      <th>Type</th>
      <th>Object</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($relationships as $result){   
      $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
      } ?>
      <tr class="<?php print $class ?>">
				<td><?php print $node->stock->name; ?></td>
				<td><?php print $result->type_id->name; ?></td>
				<?php $object = $result->object_id;
					if ($object->nid) {?>
					<td><?php print l($object->name.' ('.$object->uniquename.')', 'node/'.$object->nid); ?></td>
				<?php } else { ?>
					<td><?php print $object->name.' ('.$object->uniquename.')'; ?></td>
				<?php } ?>
      </tr>
    <?php } //end of foreach?>
  </table>
  <?php } else {
    print '<div class="tripal-no-results">There are no relationships where the current stock is the subject</div>';
  } //end of if there are object relationships ?>


<?php
  /////////////////////////////////////////////////
  // Object Relationships
  /////////////////////////////////////////////////
  
  $relationships = $node->stock->stock_relationship->object_id;
  if (!$relationships) {
    $relationships = array();
  } elseif (!is_array($relationships)) { 
    $relationships = array($relationships); 
  }
?>

  <br><br><div class="tripal_stock-info-box-desc tripal-info-box-desc">Object relationships</div>
  
  <?php if(count($relationships) > 0){ ?>
  <table class="tripal_stock-table tripal-table tripal-table-horz">
    <tr>
      <th>Subject</th>
      <th>Type</th>
      <th>Current Stock (Object)</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($relationships as $result){   
      $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
      } ?>
      <tr class="<?php print $class ?>">
				<?php $subject = $result->subject_id;
					if ($subject->nid) {?>
					<td><?php print l($subject->name.' ('.$subject->uniquename.')', 'node/'.$subject->nid); ?></td>
				<?php } else { ?>
					<td><?php print $subject->name.' ('.$subject->uniquename.')'; ?></td>
				<?php } ?>
				<td><?php print $result->type_id->name; ?></td>
				<td><?php print $node->stock->name; ?></td>
      </tr>
    <?php } //end of foreach?>
  </table>
  <?php } else {
    print '<div class="tripal-no-results">There are no relationships where the current stock is the object.</div>';
  } //end of if there are subject relationships ?>
</div>