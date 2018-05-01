<?php
$organism = $variables['node']->organism;
$image_url = tripal_get_organism_image_url($organism); ?>

<div class="tripal_organism-teaser tripal-teaser">
    <div class="tripal-organism-teaser-title tripal-teaser-title"><?php
      print l("<i>$organism->genus $organism->species</i> ($organism->common_name)", "node/$node->nid", ['html' => TRUE]); ?>
    </div>
    <div class="tripal-organism-teaser-text tripal-teaser-text"><?php
      if ($image_url) { ?>
          <img class="tripal-teaser-img" src="<?php print $image_url ?>" ><?php
      }
      print substr($organism->comment, 0, 650);
      if (strlen($organism->comment) > 650) {
        print "... " . l("[more]", "node/$node->nid");
      } ?>
    </div>
</div>
