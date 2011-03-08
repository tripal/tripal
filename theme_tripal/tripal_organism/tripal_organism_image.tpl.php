<?php
$organism = $variables['node']->organism;
?>
<div id="tripal_organism-image-box" class="tripal_organism-info-box tripal-info-box">
  <img src=<?php print file_create_url(file_directory_path() . "/tripal/tripal_organism/images/".$organism->genus."_".$organism->species.".jpg")?>>
</div>
