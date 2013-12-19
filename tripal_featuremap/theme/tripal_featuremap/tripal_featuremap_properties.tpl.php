<?php

$featuremap = $node->featuremap;

// expand the featuremap to include the properties.
$options = array(
  'return_array' => 1,
  'order_by' => array('rank' => 'ASC'),
);
$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featuremapprop', $options);
$featuremapprops = $featuremap->featuremapprop;
$properties = array();
if (is_array($featuremapprops)) {
  foreach ($featuremapprops as $property) {
    // skip the following properties as those are already on other templates
    if ($property->type_id->name == 'Map Dbxref')  {
      continue;
    }
    $property = tripal_core_expand_chado_vars($property,'field','featuremapprop.value');
    $properties[] = $property;
  }
}
if(count($properties) > 0){ ?>

  <div id="tripal_featuremap-properties-box" class="tripal_featuremap-info-box tripal-info-box">
    <div class="tripal_featuremap-info-box-title tripal-info-box-title">Properties</div>
    <div class="tripal_featuremap-info-box-desc tripal-info-box-desc">Properties for the featuremap '<?php print $node->featuremap->name ?>' include:</div>

    <table class="tripal_featuremap-table tripal-table tripal-table-horz">
    <tr>
      <th>Property Name</th>
      <th>Value</th>
    </tr>
	  <?php	// iterate through each property
		  $i = 0;
		  foreach ($properties as $result){
		    $result = tripal_core_expand_chado_vars($result,'field','featuremapprop.value');
		    $class = 'tripal_featuremap-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_featuremap-table-odd-row tripal-table-even-row';
        } ?>
			  <tr class="<?php print $class ?>">
  			  <td><?php print $result->type_id->name ?></td>
  			  <td><?php print urldecode($result->value) ?></td>
  			</tr> <?php
			  $i++;
		  } ?>
		</table>
  </div> <?php
}
