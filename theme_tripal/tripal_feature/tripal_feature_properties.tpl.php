<?php
// Purpose: Provide layout and content for feature properties. This includes all
//   fields in the featureprop table with the feature_id of the current feature
//   supplemented with extra details for the type to provide human-readable
//   output
//
// Note: This template controls the layout/content for the default feature node
//   template (node-chado_feature.tpl.php) and the Feature Properties Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, type, title, taxonomy. It also includes feature
//       specific fields such as feature_name, uniquename, feature_type, synonyms,
//       properties, db_references, object_relationships, subject_relationships,
//       organism, etc.
//   - $node->properties: an array of feature property objects where each object
//       the following fields: featureprop_id, type_id, type, value, rank
//       and includes synonyms
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 dpm($node);
?>

<?php
  $feature = $node->feature;
  $feature = tripal_core_expand_chado_vars($feature,'table','featureprop');
  $properties = $feature->featureprop;
  if (!$properties) {
    $properties = array();
  } elseif (!is_array($properties)) { 
    $properties = array($properties); 
  }
?>

<div id="tripal_feature-properties-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Properties</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">Properties for the feature '<?php print $node->feature->name ?>' include:</div>
	<?php if(count($properties) > 0){ ?>
  <table class="tripal_feature-table tripal-table tripal-table-horz">
  <tr><th>Type</th><th>Value</th></tr>
	<?php	// iterate through each property
		$i = 0;
		foreach ($properties as $result){
		  $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
			print '<tr class="'.$class.'"><td>'.$result->type_id->name.'</td><td>'.$result->value.'</td></tr>';
			$i++;
		} ?>
		</table>
	<?php } else {
	  print '<div class="tripal-no-results">There are no properties for the current feature.</div>';
	} ?>
</div>
