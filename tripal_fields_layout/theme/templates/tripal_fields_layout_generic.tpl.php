<?php

$panels = $variables['element']['#panels'];
$fields = $variables['element']['#fields'];

// Group fields into panels
foreach ($fields AS $panel_id => $field) { ?>
  <div id="" class="tripal-biodata-panel"> <?php
    if ($panels[$panel_id] != 'Base Content') {
      print '<h2>' . $panels[$panel_id] . '</h2>';
    }
    print render($field); ?>
  </div> <?php
}

print render($content);