<?php

$image_url = tripal_organism_get_image_url($organism, $node->nid); ?>

<div style="clear:both">
  <a href="<?php print url("node/" . $node->nid) ?>"><?php print $organism->genus. " " . $organism->species . ", " . $organism->common_name; ?></a>
  <img src="<?php print $image_url ?>" width="100px" height="100px" style="float: left; padding-right: 10px; padding-bottom: 5px;">   
  <?php print $organism->comment; ?>
</div>
