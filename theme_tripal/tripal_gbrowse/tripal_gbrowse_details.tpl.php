<?php
// Developed by: Chad N.A Krilow at The University of Saskatchewan 
//
// Purpose: Provide layout and content for the GBrowse database details. This
//   includes databse user specific fields in the tripal_gbrowse_instances table.
//
// Note: This template controls the layout/content for the default tripal_gbrowse node
//   template (node-tripal_grbowse.tpl.php) and the Database Details Block
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes. It also includes GBrowse specific fields such as gbrowse_name, 
//			 database_link, config_file.
?>

<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
?> 


<?php $gbrowse = $node->gbrowse; ?>

<div id="tripal_gbrowse-base-box" class="tripal_gbrowse-info-box tripal-info-box">
  <div class="tripal_gbrowse-info-box-title tripal-info-box-title"> GBrowse Details</div>
  <div class="tripal_gbrowse-info-box-desc tripal-info-box-desc"></div>

   <?php if(strcmp($gbrowse->is_obsolete,'t')==0){ ?>
      <div class="tripal_gbrowse-obsolete">This GBrowse Instance is obsolete or has been deleted/removed</div>
   <?php }?>
   <table id="tripal_gbrowse-base-table" class="tripal_gbrowse-table tripal-table tripal-table-vert">
      <tr class="tripal_gbrowse-table-even-row tripal-table-even-row">
        <th>GBrowse Instance</th>
        <td><?php print $node->gbrowse->gbrowse_name; ?></td>
      </tr>
      <tr class="tripal_gbrowse-table-odd-row tripal-table-odd-row">
        <th nowrap>GBrowse Instance Link</th>
        <td><?php print $node->gbrowse->gbrowse_link; ?></td>
      </tr>
      <tr class="tripal_gbrowse-table-even-row tripal-table-even-row">
        <th>Configuration File</th>
        <td><?php print $node->gbrowse->config_file; ?></td>
      </tr>     	                                
   </table>
</div>