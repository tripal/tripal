<?php
$node = $variables['node'];
$organism = $variables['node']->organism;

?>
<div id="tripal_organism-image-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_analysis_interpro-info-box-title tripal-info-box-title"><?php $organism->genus." ".$organism->species?> Image</div>
  <div class="tripal_analysis_interpro-info-box-desc tripal-info-box-desc"></div>
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
</div>
