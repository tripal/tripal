<?php
// Developed by: Chad N.A Krilow at The University of Saskatchewan
//
// Purpose: Provide layout and content for the GBrowse database details. This
//   includes database related fields in the tripal_gbrowse_instances table.
//
// Note: This template controls the layout/content for the default tripal_gbrowse node
//   template (node-tripal_grbowse.tpl.php) and the Database Details Block
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

?>
<div id="tripal_gbrowse-database-box" class="tripal_gbrowse-info-box tripal-info-box">
  <div class="tripal_gbrowse-info-box-title tripal-info-box-title"> Data Base Details</div>
  <div class="tripal_gbrowse-info-box-desc tripal-info-box-desc"></div>

   <?php //if the gbrowse is deleted/removed issue a warning
   if(strcmp($gbrowse->is_obsolete,'t')==0){ 
   ?>
      <div class="tripal_gbrowse-obsolete">This GBrowse Instance is obsolete or has been deleted/removed</div>
   <?php }?>
   <table id="tripal_gbrowse-base-table" class="tripal_gbrowse-table tripal-table tripal-table-vert">
      <tr class="tripal_gbrowse-table-even-row tripal-table-even-row">
        <th>Data Base Name</th>
        <td><?php print $node->gbrowse->database_name; ?></td>
      </tr>
      <tr class="tripal_gbrowse-table-odd-row tripal-table-odd-row">
        <th nowrap>Database User Name</th>
        <td><?php print $node->gbrowse->database_user; ?></td>
      </tr>
      <tr class="tripal_gbrowse-table-even-row tripal-table-even-row">
        <th>User Password</th>
        <td><?php print $node->gbrowse->user_password; ?></td>
      </tr>     	                                
   </table>
</div>
