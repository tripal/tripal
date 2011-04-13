<?php

$featurelocs_as_child = $variables['tripal_feature']['featurelocs_as_child'];
$feature = $variables['node']->feature;

?>
<div id="tripal_feature-featurelocs_as_child-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Locations where <?php print $feature->featurename;?> is found</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The <?php print $feature->featurename;?> feature is located relative to the following features:</div>
  <?php if(count($featurelocs_as_child) > 0){ ?>
  <table id="tripal_feature-featurelocs_as_child-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Name</th>
      <th>Type</th>
      <th>Position</th>
      <th>Phase</th>
      <th>Direction</th>
    </tr>
    <?php
      $i = 0; 
      foreach ($featurelocs_as_child as $index => $loc){
         $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
         if($i % 2 == 0 ){
            $class = 'tripal_feature-table-odd-row tripal-table-even-row';
         } 
         $src_name = $loc->src_name;
         if($loc->snid){
           $src_name = "<a href=\"" . url("node/$loc->snid") . "\">".$loc->src_name .":".$loc->fmin . ".." . $loc->fmax ."</a> ";
         }
         ?>
         <tr class="<?php print $class ?>">
           <td><?php print $loc->name ?></td>
           <td><?php print $loc->cvname ?></td>
           <td><?php print $src_name ?></td>
           <td><?php print $loc->phase ?></td>
           <td><?php 
              if($loc->strand == -1){
                 print "reverse";
              } 
              elseif($loc->strand == 1){
                 print "forward";
              } 
              elseif($loc->strand == 0){
                 print "N/A";
              } 
              else {
                 print $loc->strand;
              }?>
            </td>
         </tr>
         <?php
         $i++;  
      } ?>
    </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no locations where this feature is found</div> 
  <?php }?>
</div>


