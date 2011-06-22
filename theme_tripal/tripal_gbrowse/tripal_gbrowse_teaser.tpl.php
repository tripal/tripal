<?php
// Developed by: Chad N.A Krilow at The University of Saskatchewan
//
// Purpose: Provide layout and content for the Tripal Gbrowse details.Represents
// what the user will see if the there is no information in the node
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes including nid, gbrowse_id.
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<div id="tripal_gbrowse-base-box" class="tripal_gbrowse-info-box tripal-info-box">
  <div class="tripal_gbrowse-info-box-title tripal-info-box-title">
  
  	<!-- Title -->  	
    <?php print l($node->gbrowse->gbrowse_name, 'node/'.$node->nid); ?>

		
  </div>
  <div class="tripal_gbrowse-info-box-desc tripal-info-box-desc"></div>

<p>This is a representation of a created GBrowse instance.<p>
<?php print l('See More Details', 'node/'.$node->nid); ?>

 
</div>