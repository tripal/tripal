<?php
$node = $variables['node'];
$pub = $variables['node']->pub;

// get the citation
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'Citation',
  ),
);
$citation = tripal_core_generate_chado_var('pubprop', $values); 
$citation = tripal_core_expand_chado_vars($citation, 'field', 'pubprop.value'); 

// get the abstract
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'Abstract',
  ),
);
$abstract = tripal_core_generate_chado_var('pubprop', $values); 
$abstract = tripal_core_expand_chado_vars($abstract, 'field', 'pubprop.value');
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
    print l($pub->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-pub-teaser-text tripal-teaser-text"><?php 
    print $teaser_text; ?>
  </div>
</div>