<?php
$example  = $variables['node']->example; ?>

<div class="tripal_example-teaser tripal-teaser"> 
  <div class="tripal-example-teaser-title tripal-teaser-title"><?php 
    print l("<i>$example->uniquename", "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-example-teaser-text tripal-teaser-text"> <?php
    print substr($example->description, 0, 650);
    if (strlen($example->description) > 650) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>