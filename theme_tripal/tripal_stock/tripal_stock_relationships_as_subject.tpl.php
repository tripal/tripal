<?php
// Copyright 2010 University of Saskatchewan (Lacey-Anne Sanderson)
//
// Purpose: Provides layout and content for Stock Relationships where
//   the current stock is the Object of the relationships. This includes all 
//   fields in the stock_relationship table.
//
// Note: This template controls the layout/content for the default stock node
//   template (node-chado_stock.tpl.php) and the Stock Subject Relationships Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes stock
//       specific fields such as stock_name, uniquename, stock_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   - $node->subject_relationships: an array of stock relaionship objects 
//       where each object has the following fields: stock_relationship_id,
//       subject, type_id, type, value, rank, object_id (current stock_id)
//   - $node->subject_relationships->subject: a stock object describing the
//       subject stock with the fields: stock_id, stock_name, uniquename, 
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

<div id="tripal_stock-subject_relationships-box" class="tripal_stock-info-box tripal-info-box">
  <div class="tripal_stock-info-box-title tripal-info-box-title">Subject Relationships</div>
  <div class="tripal_stock-info-box-desc tripal-info-box-desc">The stock '<?php print $node->stock_name ?>' is the object in the following relationships:</div>
  <?php if(count($node->subject_relationships) > 0){ ?>
  <table class="tripal_stock-table tripal-table tripal-table-horz">
    <tr>
      <th>Subject</th>
      <th>Type</th>
      <th>Current Stock (Object)</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($node->subject_relationships as $result){   
      $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
      } ?>
      <tr class="<?php print $class ?>">
				<?php $subject = $result->subject;
					if ($subject->nid) {?>
					<td><?php print l($subject->stock_name.' ('.$subject->uniquename.')', 'node/'.$subject->nid); ?></td>
				<?php } else { ?>
					<td><?php print $subject->stock_name.' ('.$subject->uniquename.')'; ?></td>
				<?php } ?>
				<td><?php print $result->type; ?></td>
				<td><?php print $node->stock_name; ?></td>
      </tr>
    <?php } //end of foreach?>
  </table>
  <?php } //end of if there are subject relationships ?>
</div>