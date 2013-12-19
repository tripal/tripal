<?php
$analysis = $node->analysis;

// expand the analysis to include the properties.
$analysis = tripal_core_expand_chado_vars($analysis,'table', 'analysisprop', array('return_array' => 1));
$analysisprops = $analysis->analysisprop;
$properties = array();
if (is_array($analysisprops)) {
  foreach ($analysisprops as $property) {
    $property = tripal_core_expand_chado_vars($property,'field','analysisprop.value');
    $properties[] = $property;
  }
}

if (count($properties) > 0) { ?>
  <div id="tripal_analysis-properties-box" class="tripal_analysis-info-box tripal-info-box">
    <div class="tripal_analysis-info-box-title tripal-info-box-title">More Details</div>
    <div class="tripal_analysis-info-box-desc tripal-info-box-desc">Additional information about this analysis:</div>
    <table class="tripal_analysis-table tripal-table tripal-table-horz">
      <tr>
        <th>Property Name</th>
        <th>Value</th>
      </tr> <?php
      $i = 0;
      foreach ($properties as $property) {
        $class = 'tripal_analysis-table-odd-row tripal-table-odd-row';
        if ($i % 2 == 0 ) {
           $class = 'tripal_analysis-table-odd-row tripal-table-even-row';
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
