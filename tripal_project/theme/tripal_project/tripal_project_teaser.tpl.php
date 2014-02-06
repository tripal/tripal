<?php
$node    = $variables['node'];
$project = $variables['node']->project;

// get the project description.  The first iteration of the project
// module incorrectly stored the project description in the Drupal 
// node->body field.  Also, the project.descriptin field is only 255
// characters which is not large neough. Therefore, we store the description
// in the  chado.projectprop table.  For backwards compatibility, we 
// will check if the node->body is empty and if not we'll use that instead.
// If there is data in the project.description field then we will use that, but
// if there is data in the projectprop table for a descrtion then that takes 
// precedence 
$description = '';
if (property_exists($node, 'body')) {
  $description = $node->body;
}
if (property_exists($node, 'description')) {
  $description = $project->description;
}
else {
  $projectprop = tripal_project_get_property($project->project_id, 'Project Description');
  $description = $projectprop->value;
} ?>

<div class="tripal_project-teaser tripal-teaser"> 
  <div class="tripal-project-teaser-title tripal-teaser-title"><?php 
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal-project-teaser-text tripal-teaser-text"><?php 
    print substr($description, 0, 650);
    if (strlen($description) > 650) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>