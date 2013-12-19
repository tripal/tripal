<?php

$organism = $variables['node']->organism;
$types    = $organism->feature_counts['types'];
$names    = $organism->feature_counts['names'];
$enabled  = $organism->feature_counts['enabled'];

// only show this block if it is enabled
if ($enabled) { 
  if (count($types) > 0){ ?>
    <div id="tripal_organism-feature_counts-box" class="tripal_organism-info-box tripal-info-box">
      <div class="tripal_organism-info-box-title tripal-info-box-title">Data Type Summary</div>
      <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following data types are currently present for this organism</div> <?php
      // let admins know they can customize the terms that appear in the list
      if (user_access('access administration pages')) { ?>
         <div class="tripal-no-results">Administrators, you can customize the types of terms that appear in this report by 
         navigating to the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Tripal 
         feature configuration page</a> opening the section "Feature Summary Report" and adding the list of
         terms you want to appear in the list. You can rename terms as well.  To disable this report and 
         remove it from the list of resources, navigate to the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Tripal feature configuration page</a> 
         and hide the "Feature Summary". To refresh the data,re-populate the <a href="<?php print url('admin/tripal/mviews');?>" target="_blank">organism_feature_count</a> materialized view. 
         </div><?php 
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
        <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following data types are currently present for this organism</div>
        <div class="tripal-no-results">There are no features available.
           <p><br>Administrators, to view the feature type report:
           <ul>
              <li>Populate the <a href="<?php print url('admin/tripal/mviews');?>" target="_blank">organism_feature_count</a> materialized view</li>
              <li>Refresh this page</li>
           </ul> 
           <br><br>To disable this report and remove it from the list of resources:
           <ul>
             <li>Navigate to the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Tripal feature configuration page</a> and hide the "Feature Summary"</li>
            </ul>
           </p>
           This page will not appear to site visitors unless features are present. 
         </div>
      </div><?php               
    }
  }
} 



