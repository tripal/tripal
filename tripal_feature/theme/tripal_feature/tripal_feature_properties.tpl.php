<?php

$feature = $node->feature;
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'featureprop', $options);
$properties = $feature->featureprop;


if(count($properties) > 0){ ?>

  <div id="tripal_feature-properties-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Properties</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">Properties for the feature '<?php print $node->feature->name ?>' include:</div>

    <table class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Property Name</th>
      <th>Value</th>
    </tr>
	  <?php	// iterate through each property
		  $i = 0;
		  foreach ($properties as $result){
		    $result = tripal_core_expand_chado_vars($result,'field','featureprop.value');
		    $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_feature-table-odd-row tripal-table-even-row';
        } ?>
			  <tr class="<?php print $class ?>">
  			  <td><?php print ucfirst(preg_replace('/_/', ' ', $result->type_id->name)) ?></td>
  			  <td><?php print urldecode($result->value) ?></td>
  			</tr> <?php
			  $i++;
		  } ?>
		</table>
  </div> <?php
}
