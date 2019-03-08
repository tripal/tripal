<?php
$node = $variables['node'];
$phylotree = $variables['node']->phylotree;
$phylotree = chado_expand_var($phylotree, 'field', 'phylotree.comment'); ?>

<div class="tripal_phylogeny_blast-teaser tripal-teaser">
    <div class="tripal-phylotree-blast-teaser-title tripal-teaser-title"><?php
      print l($node->title, "node/$node->nid", ['html' => TRUE]); ?>
    </div>
    <div class="tripal-phylotree-blast-teaser-text tripal-teaser-text"><?php
      print substr($phylotree->comment, 0, 650);
      if (strlen($phylotree->comment) > 650) {
        print "... " . l("[more]", "node/$node->nid");
      } ?>
    </div>
</div>


