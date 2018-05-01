<?php
$node = $variables['node'];
$library = $variables['node']->library;

// get the library description. IT uses a tern name of 'Library Description'
$record = [
  'table' => 'library',
  'id' => $library->library_id,
];
$property = [
  'type_name' => 'Library Description',
  'cv_name' => 'library_property',
];
$libprop = chado_get_property($record, $property);
$description = $libprop->value; ?>

<div class="tripal_library-teaser tripal-teaser">
    <div class="tripal-library-teaser-title tripal-teaser-title"><?php
      print l($node->title, "node/$node->nid", ['html' => TRUE]); ?>
    </div>
    <div class="tripal-library-teaser-text tripal-teaser-text"><?php
      print substr($description, 0, 650);
      if (strlen($description) > 650) {
        print "... " . l("[more]", "node/$node->nid");
      } ?>
    </div>
</div>