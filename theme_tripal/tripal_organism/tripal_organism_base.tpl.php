<?php
$node = $variables['node'];
$organism = $variables['node']->organism;

// the comment field is a database text field so we have to expand it so that
// it is included in the organism object
$organism = tripal_core_expand_chado_vars($organism,'field','organism.comment');

 
?>
<div id="tripal_organism-base-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_organism-info-box-title tripal-info-box-title">Organism Details</div>
  <div class="tripal_organism-info-box-desc tripal-info-box-desc"></div>
   <img src="<?php 
      $image_name = $organism->genus."_".$organism->species.".jpg";
      $image_dir = file_directory_path() . "/tripal/tripal_organism/images";
      $files = file_scan_directory($image_dir,$image_name);
      if(sizeof($files) > 0){
         print file_create_url("$image_dir/$image_name"); 
      } else {
         $image_file = file_directory_path() . "/tripal/tripal_organism/images/".$node->nid.".jpg";
         print file_create_url($image_file); 
      }
   ?>">
   <table id="tripal_organism-table-base" class="tripal_organism-table tripal-table tripal-table-vert">
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Common Name</th>
        <td><?php print $organism->common_name; ?></td>
      </tr>
      <tr class="tripal_organism-table-odd-row tripal-table-odd-row">
        <th>Genus</th>
        <td><?php print $organism->genus; ?></td>
      </tr>
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Species</th>
        <td><?php print $organism->species; ?></td>
      </tr>
      <tr class="tripal_organism-table-odd-row tripal-table-odd-row">
        <th>Abbreviation</th>
        <td><?php print $organism->abbreviation; ?></td>
      </tr>         	                                
   </table>
   <table  id="tripal_organism-table-description" class="tripal_organism-table tripal-table tripal-table-horz">
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <td><?php print $organism->comment; ?></td>
      </tr>        	                                
   </table>
</div>
