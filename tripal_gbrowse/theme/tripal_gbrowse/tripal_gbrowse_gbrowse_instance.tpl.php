<?php
// Developed by: Chad N.A Krilow at The University of Saskatchewan
//
// Purpose: Provide a in window I-Frame that displays the GBrowse instance. As well,
// a link to open the GBrowse instance in a external window is provided. If the window can not be
// opened a warning is issued
//   
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes. Here it is utilized for accessing the link to a specific GBrowse instance.
//
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>

<br />
<h3><a href="<?php print $node->gbrowse->gbrowse_link; ?>" target="_blank">Open GBrowse Instance in new Window</a></h3>
<br />

<iframe src="<?php print $node->gbrowse->gbrowse_link; ?>" width="100%" height="800">
  <p>Your browser does not support iframes.</p>
</iframe>