<?php
$organism = $variables['node']->organism;
?>
<div id="tripal_organism-image-box" class="tripal_organism-info-box tripal-info-box">
  <div class="tripal_analysis_interpro-info-box-title tripal-info-box-title"><?php $organism->genus." ".$organism->species?> Image</div>
  <div class="tripal_analysis_interpro-info-box-desc tripal-info-box-desc"></div>
  <img src="<?php print file_create_url(file_directory_path() . '/tripal/tripal_organism/images/'.$organism->genus.'_'.$organism->species.'.jpg')?>">
</div>
