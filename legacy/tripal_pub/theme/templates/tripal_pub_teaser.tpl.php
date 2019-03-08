<?php
$node = $variables['node'];
$pub = $variables['node']->pub;

// get the citation
$values = [
  'pub_id' => $pub->pub_id,
  'type_id' => [
    'name' => 'Citation',
  ],
];
$citation = chado_generate_var('pubprop', $values);
$citation = chado_expand_var($citation, 'field', 'pubprop.value');

// get the abstract
$values = [
  'pub_id' => $pub->pub_id,
  'type_id' => [
    'name' => 'Abstract',
  ],
];
$abstract = chado_generate_var('pubprop', $values);
$abstract = chado_expand_var($abstract, 'field', 'pubprop.value');
$abstract_text = '';
if ($abstract) {
  $abstract_text = htmlspecialchars($abstract->value);
  $abstract_text = substr($abstract_text, 0, 450);
  $abstract_text .= "... " . l("[more]", "node/$node->nid");
}

$teaser_text = "<ul id=\"tripal-pub-teaser-citation\"><li>" . $citation->value . "</li></ul>" . $abstract_text;
?>

<div class="tripal_pub-teaser tripal-teaser">
    <div class="tripal-pub-teaser-title tripal-teaser-title"><?php
      print l($pub->title, "node/$node->nid", ['html' => TRUE]); ?>
    </div>
    <div class="tripal-pub-teaser-text tripal-teaser-text"><?php
      print $teaser_text; ?>
    </div>
</div>