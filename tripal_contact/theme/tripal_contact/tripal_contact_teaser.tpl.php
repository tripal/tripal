<?php
$node = $variables['node'];
$contact = $variables['node']->contact; ?>

<div class="tripal_contact-teaser tripal-teaser"> 
  <div class="tripal-contact-teaser-title tripal-teaser-title"><?php 
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-contact-teaser-text tripal-teaser-text"><?php 
    if ($contact->description) {
      print substr($contact->description, 0, 650);
      if (strlen($contact->description) > 650) {
        print "... " . l("[more]", "node/$node->nid");
      } 
    } 
    else {
      print $node->title; 
    } ?>
  </div>
</div>