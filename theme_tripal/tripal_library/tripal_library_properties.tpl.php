<?php
// Purpose: Provide layout and content for library properties. This includes all
//   fields in the libraryprop table with the library_id of the current library
//   supplemented with extra details for the type to provide human-readable
//   output
//
// Note: This template controls the layout/content for the default library node
//   template (node-chado_library.tpl.php) and the library Properties Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes library
//       specific fields such as library_name, uniquename, library_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   - $node->properties: an array of library property objects where each object
//       the following fields: libraryprop_id, type_id, type, value, rank
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
  $properties = $node->library->libraryprop;
  if (!$properties) {
    $properties = array();
  } elseif (!is_array($properties)) { 
    $properties = array($properties); 
  }
?>

<div id="tripal_library-properties-box" class="tripal_library-info-box tripal-info-box">
  <div class="tripal_library-info-box-title tripal-info-box-title">Properties</div>
  <div class="tripal_library-info-box-desc tripal-info-box-desc">Properties for the library '<?php print $node->library->name ?>' include:</div>
	<?php if(count($properties) > 0){ ?>
  <table class="tripal_library-table tripal-table tripal-table-horz">
  <tr><th>Type</th><th>Value</th></tr>
	<?php	// iterate through each property
		$i = 0;
		foreach ($properties as $result){
		  $class = 'tripal_library-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_library-table-odd-row tripal-table-even-row';
      }
			print '<tr class="'.$class.'"><td>'.$result->type_id->name.'</td><td>'.$result->value.'</td></tr>';
			$i++;
		} ?>
		</table>
	<?php } else {
	  print '<div class="tripal-no-results">There are no properties for the current library.</div>';
	} ?>
</div>
