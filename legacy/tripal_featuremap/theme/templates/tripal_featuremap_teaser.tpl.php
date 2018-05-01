<?php
$node = $variables['node'];
$featuremap = $variables['node']->featuremap;
$featuremap = chado_expand_var($featuremap, 'field', 'featuremap.description'); ?>

<div class="tripal_featuremap-teaser tripal-teaser">
    <div class="tripal-featuremap-teaser-title tripal-teaser-title"><?php
      print l($node->title, "node/$node->nid", ['html' => TRUE]); ?>
    </div>
    <div class="tripal-featuremap-teaser-text tripal-teaser-text"><?php
      print substr($featuremap->description, 0, 650);
      if (strlen($featuremap->description) > 650) {
        print "... " . l("[more]", "node/$node->nid");
      } ?>
    </div>
</div>