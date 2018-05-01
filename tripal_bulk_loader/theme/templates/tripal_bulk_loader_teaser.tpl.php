<div class="tripal_bulk_loader-teaser tripal-teaser">
    <div class="tripal-bulk_loader-teaser-title tripal-teaser-title"><?php
      print l($node->title, "node/$node->nid", ['html' => TRUE]); ?>
    </div>
    <div class="tripal-bulk_loader-teaser-text tripal-teaser-text"><?php
      print $node->title;
      print "... " . l("[more]", "node/$node->nid"); ?>
    </div>
</div> <?php 