<?php
$node = $variables['node'];
$project = $variables['node']->project;

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
      <td><i><?php print $project->description; ?></i></td>
    </tr>
  </table> 
</div>
