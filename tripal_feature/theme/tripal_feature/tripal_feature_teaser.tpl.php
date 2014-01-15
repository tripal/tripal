<?php
$node = $variables['node'];
$feature = $variables['node']->feature; ?>

<div class="tripal_feature-teaser tripal-teaser"> 
  <div class="tripal-feature-teaser-title tripal-teaser-title"><?php 
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-feature-teaser-text tripal-teaser-text"><?php 
    print $node->title;
    print "... " . l("[more]", "node/$node->nid"); ?>
  </div>
</div>