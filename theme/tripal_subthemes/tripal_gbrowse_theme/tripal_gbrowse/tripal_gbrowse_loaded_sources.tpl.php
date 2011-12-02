<?php
// Developed by: Chad N.A Krilow at The University of Saskatchewan
//
// Variables Available:
//   - $node: a standard object which contains includes GBrowse Databse specific fields such as  
//			 database_name, database_user, user_password, etc.
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?>  

<?php

$gbrowse = $node->gbrowse;
$node->sources = tripal_gbrowse_getloaded_sources($node->gbrowse);

?>
<div id="tripal_gbrowse-source-box" class="tripal_gbrowse-info-box tripal-info-box">
  <div class="tripal_gbrowse-info-box-title tripal-info-box-title"> Loaded Sources</div>
  <div class="tripal_gbrowse-info-box-desc tripal-info-box-desc">A source can be either a library or analysis or any other grouping of features.</div>

   <?php //if the gbrowse is deleted/removed issue a warning
   if(strcmp($gbrowse->is_obsolete,'t')==0){ 
   ?>
      <div class="tripal_gbrowse-obsolete">This GBrowse Library is obsolete or has been deleted/removed</div>
   <?php }?>
   <table id="tripal_gbrowse-base-table" class="tripal_gbrowse-table tripal-table tripal-table-vert">
   <tr><th>Name</th></tr>
   
   
   <?php 
   if(!empty($node->sources)){
   	foreach($node->sources as $key => $source){
   ?>
      <tr class="tripal_gbrowse-table-even-row tripal-table-even-row">
        <td><?php print $source; ?></td>
      </tr>
		<?php }} //end of foreach library & end of if statement ?>  	                                
   </table>
</div>