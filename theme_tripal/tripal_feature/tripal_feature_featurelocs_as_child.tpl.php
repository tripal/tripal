<?php

$featurelocs_as_child = $variables['featurelocs_as_child'];
$feature = $variables['node']->feature;

?>
<div id="tripal_feature-featurelocs_as_child-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Features where <?php print $feature->featurename;?> is located</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The <?php print $feature->featurename;?> feature is located relative to the following features:</div>
  <table class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Name</th>
      <th>Type</th>
      <th>Position</th>
      <th>Phase</th>
      <th>Strand</th>
    </tr>
    <?php
      $i = 0; 
      foreach ($featurelocs_as_child as $index => $loc){
         $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
         if($i % 2 == 0 ){
            $class = 'tripal_feature-table-odd-row tripal-table-even-row';
         } 
         $locname = $loc->name;
         if($loc->nid){
           $locname = "<a href=\"" . url("node/$loc->nid") . "\">$loc->name</a> ";
         }
         ?>
         <tr class="<?php print $class ?>">
           <td><?php print $locname ?></td>
           <td><?php print $loc->cvname ?></td>
           <td><?php print $loc->src_name .":".$loc->fmin . ".." . $loc->fmax ?></td>
           <td><?php print $loc->phase ?></td>
           <td><?php print $loc->strand ?></td>
         </tr>
         <?php
         $i++;  
      } ?>
    </table>
</div>


