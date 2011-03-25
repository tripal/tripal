<?php
// Copyright 2010 University of Saskatchewan (Lacey-Anne Sanderson)
//
// Purpose: Provide layout and content for stock properties. This includes all
//   fields in the stockprop table with the stock_id of the current stock
//   supplemented with extra details for the type to provide human-readable
//   output
//
// Note: This template controls the layout/content for the default stock node
//   template (node-chado_stock.tpl.php) and the Stock Properties Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes stock
//       specific fields such as stock_name, uniquename, stock_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   - $node->properties: an array of stock property objects where each object
//       the following fields: stockprop_id, type_id, type, value, rank
//       and includes synonyms
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<?php
  $properties = $node->stock->stockprop;
  if (!$properties) {
    $properties = array();
  } elseif (!is_array($properties)) { 
    $properties = array($properties); 
  }
?>

<div id="tripal_stock-properties-box" class="tripal_stock-info-box tripal-info-box">
  <div class="tripal_stock-info-box-title tripal-info-box-title">Properties</div>
  <div class="tripal_stock-info-box-desc tripal-info-box-desc">Properties for the stock '<?php print $node->stock->name ?>' include:</div>
	<?php if(count($properties) > 0){ ?>
  <table class="tripal_stock-table tripal-table tripal-table-horz">
  <tr><th>Type</th><th>Value</th></tr>
	<?php	// iterate through each property
		$i = 0;
		foreach ($properties as $result){
		  $class = 'tripal_stock-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_stock-table-odd-row tripal-table-even-row';
      }
			print '<tr class="'.$class.'"><td>'.$result->type_id->name.'</td><td>'.$result->value.'</td></tr>';
			$i++;
		} ?>
		</table>
	<?php } else {
	  print '<b>There are no properties for the current stock.</b>';
	} ?>
</div>