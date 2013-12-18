<?php
$node = $variables['node'];
$project = $variables['node']->project;

// get the project description.  The first iteration of the project
// module incorrectly stored the project description in the Drupal 
// node->body field.  It should have been in the chado.projectprop 
// table.  Therefore, for backwards compatibility, we will check if the
// node->body is empty and if not we'll use that instead. Otherwise,
// we'll pull from the projectprop table.
$project_description = '';
if ($node->body) {
  $project_description = $node->body;
}
else {
  // expand the project to include the properties.
  $project = tripal_core_expand_chado_vars($project,'table','projectprop', array('return_array' => 1));  
  $projectprops = $project->projectprop;
  foreach ($projectprops as $property) {
    if($property->type_id->name == 'project_description') {
      $property = tripal_core_expand_chado_vars($property,'field','projectprop.value');
      $project_description = $property->value;      
    }
  }
  // if there is no project_description property then see if
  // there is anything in the project.description field.  This field is only 255
  // characters and isn't large enough for some project description, which is why
  // the description is stored as a property.  But if we have no property then
  // use the field
  if (!$project_description) {
    // we expect the project.description field will one day get changed to
    // a text field so, we'll expand it now so that the template won't break if the field ever does change.
    tripal_core_expand_chado_vars($project,'field','project.description');
    $project_description = $project->description;
  }
}

?>
<div id="tripal_project-base-box" class="tripal_project-info-box tripal-info-box">
  <div class="tripal_project-info-box-title tripal-info-box-title">Project Details</div>
  <div class="tripal_project-info-box-desc tripal-info-box-desc"></div>   

  <table id="tripal_project-table-base" class="tripal_project-table tripal-table tripal-table-vert">
    <tr class="tripal_project-table-even-row tripal-table-even-row">
      <th>Project Name</th>
      <td><?php print $project->name; ?></td>
    </tr>
    <tr class="tripal_project-table-odd-row tripal-table-odd-row">
      <th>Description</th>
      <td><?php print $project_description?></td>
    </tr>
  </table> 
</div>
