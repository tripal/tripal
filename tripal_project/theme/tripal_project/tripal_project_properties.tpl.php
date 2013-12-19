<?php
$project = $node->project;

// expand the project to include the properties.
$project = tripal_core_expand_chado_vars($project,'table', 'projectprop', array('return_array' => 1));
$projectprops = $project->projectprop;
$properties = array();
foreach ($projectprops as $property) {
  // we want to keep all properties but the project_description as that
  // property is shown on the base template page.
  if($property->type_id->name != 'project_description') {
    $property = tripal_core_expand_chado_vars($property,'field','projectprop.value');
    $properties[] = $property;
  }
}

if (count($properties) > 0) { ?>
  <div id="tripal_project-properties-box" class="tripal_project-info-box tripal-info-box">
    <div class="tripal_project-info-box-title tripal-info-box-title">Properties</div>
    <div class="tripal_project-info-box-desc tripal-info-box-desc">Properties for this project include:</div>
    <table class="tripal_project-table tripal-table tripal-table-horz">
      <tr>
        <th>Property Name</th>
        <th>Value</th>
      </tr> <?php
      $i = 0;
      foreach ($properties as $property) {
        $class = 'tripal_project-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal_project-table-odd-row tripal-table-even-row';
        }
        $i++; 
        ?>
        <tr class="<?php print $class ?>">
          <td><?php print ucfirst(preg_replace('/_/', ' ', $property->type_id->name)) ?></td>
          <td><?php print $property->value ?></td>
        </tr><?php 
      } ?>
    </table>
  </div> <?php
}
