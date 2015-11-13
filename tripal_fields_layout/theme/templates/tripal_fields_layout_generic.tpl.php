<?php

$panels = $variables['element']['#panels'];
$fields = $variables['element']['#fields'];

// Group fields into panels
foreach ($fields AS $panel_id => $field) { ?>
  <div style="border:solid 1px #CCC;margin:15px 0px;padding:15px;"><?php
    print '<h1>' . $panels[$panel_id] . '</h1>';
    print render($field);
  ?>
  </div><?php
}

