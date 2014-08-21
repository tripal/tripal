<?php
$organism  = $variables['node']->organism;
$image_url  = tripal_get_organism_image($organism, $node->nid); 
$image_path = tripal_get_organism_image($organism, $node->nid, 'path');?>

<div class="tripal_organism-teaser tripal-teaser"> 
  <div class="tripal-organism-teaser-title tripal-teaser-title"><?php 
    print l("<i>$organism->genus $organism->species</i> ($organism->common_name)", "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-organism-teaser-text tripal-teaser-text"><?php 
    if (file_exists($image_path)) { ?>
      <img class="tripal-teaser-img" src="<?php print $image_url ?>" ><?php
    } 
    print substr($organism->comment, 0, 650);
    if (strlen($organism->comment) > 650) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>
