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
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<div id="tripal_stock-object_relationships-box" class="tripal_stock-info-box tripal-info-box">
  <div class="tripal_stock-info-box-title tripal-info-box-title">Object Relationships</div>
  <div class="tripal_stock-info-box-desc tripal-info-box-desc">The stock '<?php print $node->stock_name ?>' is the subject in the following relationships:</div>
  <?php if(count($node->object_relationships) > 0){ ?>
  <table class="tripal_stock-table tripal-table tripal-table-horz">
    <tr>
      <th>Current Stock (Subject)</th>
      <th>Type</th>
      <th>Object</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($node->object_relationships as $result){   
      $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
      } ?>
      <tr class="<?php print $class ?>">
				<td><?php print $node->stock_name; ?></td>
				<td><?php print $result->type; ?></td>
				<?php $object = $result->object;
					if ($object->nid) {?>
					<td><?php print l($object->stock_name.' ('.$object->uniquename.')', 'node/'.$object->nid); ?></td>
				<?php } else { ?>
					<td><?php print $object->stock_name.' ('.$object->uniquename.')'; ?></td>
				<?php } ?>
      </tr>
    <?php } //end of foreach?>
  </table>
  <?php } //end of if there are object relationships ?>
</div>