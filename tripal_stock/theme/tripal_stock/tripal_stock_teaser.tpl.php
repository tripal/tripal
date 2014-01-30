<?php
$node  = $variables['node'];
$stock = $variables['node']->stock;
$stock = tripal_core_expand_chado_vars($stock,'field','stock.description'); ?>

<div class="tripal_stock-teaser tripal-teaser"> 
  <div class="tripal-stock-teaser-title tripal-teaser-title"><?php 
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-stock-teaser-text tripal-teaser-text"><?php 
    print substr($stock->description, 0, 650);
    if (strlen($stock->description) > 650) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>