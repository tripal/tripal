<?php
$node = $variables['node'];
$analysis = $variables['node']->analysis;
$analysis = tripal_core_expand_chado_vars($analysis,'field','analysis.description'); ?>

<div class="tripal_analysis_blast-teaser tripal-teaser"> 
  <div class="tripal-analysis-blast-teaser-title tripal-teaser-title"><?php 
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-analysis-blast-teaser-text tripal-teaser-text"><?php 
    print substr($analysis->description, 0, 650);
    if (strlen($analysis->description) > 650) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>