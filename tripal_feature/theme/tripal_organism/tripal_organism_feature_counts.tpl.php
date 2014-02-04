<?php

$organism = $variables['node']->organism;
$enabled = 1;
$types = array();

if(property_exists($organism, 'feature_counts')) {
  $types    = $organism->feature_counts['types'];
  $names    = $organism->feature_counts['names'];
  $enabled  = $organism->feature_counts['enabled'];
}

// only show this block if it is enabled
if ($enabled) { 
  if (count($types) > 0){ ?>
    <div id="tripal_organism-feature_counts-box" class="tripal_organism-info-box tripal-info-box">
      <div class="tripal_organism-info-box-title tripal-info-box-title">Data Type Summary</div>
      <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following data types are currently present for this organism</div> <?php
      // let admins know they can customize the terms that appear in the list
      if (user_access('access administration pages')) { 
         print theme('tripal_admin_message', array('message' => "
           Administrators, you can customize the types of terms that appear in this report by 
           navigating to the " . l('Tripal feature configuration page', 'admin/tripal/chado/tripal_feature/configuration') . "
           opening the section \"Feature Summary Report\" and adding the list of
           terms you want to appear in the list. You can rename terms as well.  To disable this report and 
           remove it from the list of resources, navigate to the " . 
           l('Tripal feature configuration page', 'admin/tripal/tripal_feature/configuration') . "
           and hide the \"Feature Summary\". To refresh the data,re-populate the " .
           l('organism_feature_count', 'admin/tripal/schema/mviews') . " materialized view.")
         ); 
      }?>
      <table id="tripal_organism-table-feature_counts" class="tripal_organism-table tripal-table tripal-table-horz">     
        <tr class="tripal_organism-table-odd-row tripal-table-even-row">
          <th>Feature Type</th>
          <th>Count</th>
        </tr> <?php
        for ($j = 0; $j < count($types); $j++) {
          $type = $types[$j];
          $name = $names[$j];
          $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
          if ($i % 2 == 0 ) {
           $class = 'tripal_organism-table-even-row tripal-table-even-row';
          }?>
          <tr class="<?php print $class ?>">
            <td><span title="<?php print $type->definition ?>"><?php print $name?></span></td>
            <td><?php print number_format($type->num_features) ?></td>
          </tr> <?php
          $i++;  
        } ?>
      </table>
      <img class="tripal_cv_chart" id="tripal_feature_cv_chart_<?php print $organism->organism_id?>" src="" border="0">
    </div><?php           
   } 
   else { 
    if (user_access('access administration pages')) { ?>
      <div id="tripal_organism-feature_counts-box" class="tripal_organism-info-box tripal-info-box">
        <div class="tripal_organism-info-box-title tripal-info-box-title">Data Type Summary</div>
        <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following data types are currently present for this organism</div> <?php 
        print theme('tripal_admin_message', array('message' => "
           Administrators, to view the feature type report:
           <ul>
              <li>Populate the " . l('organism_feature_count', 'admin/tripal/schema/mviews') ." materialized view</li>
              <li>Refresh this page</li>
           </ul> 
           To disable this report and remove it from the list of resources:
           <ul>
             <li>Navigate to the " . l('Tripal feature configuration page', 'admin/tripal/chado/tripal_feature/configuration') ." and hide the \"Feature Summary\"</li>
            </ul>
           </p>")
        ); ?> 
      </div><?php               
    }
  }
} 



