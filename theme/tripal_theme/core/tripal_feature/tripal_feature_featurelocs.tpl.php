<?php

$feature = $variables['node']->feature;

// expand the feature object to include the featureloc records.  there are
// two foreign key relationships with featureloc and feature (srcefeature_id and
// feature_id).  This will expand both
$feature = tripal_core_expand_chado_vars($feature,'table','featureloc');

// get the featurelocs. if only one featureloc exists then we want to convert
// the object into an array, otherwise the value is an array
$ffeaturelocs = $feature->featureloc->feature_id;
if (!$ffeaturelocs) {
   $ffeaturelocs = array();
} elseif (!is_array($ffeaturelocs)) { 
   $ffeaturelocs = array($ffeaturelocs); 
}
$sfeaturelocs = $feature->featureloc->srcfeature_id;
if (!$sfeaturelocs) {
   $sfeaturelocs = array();
} elseif (!is_array($sfeaturelocs)) { 
   $sfeaturelocs = array($sfeaturelocs); 
}

?>
<div id="tripal_feature-featurelocs-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Alignments</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc"><?php print $feature->name;?> is aligined to the following</div>
  <?php if(count($ffeaturelocs) > 0){ ?>
  <table id="tripal_feature-featurelocs_as_child-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Name</th>
      <th>Type</th>
      <th>Location</th>
      <th>Phase</th>
      <th>Direction</th>
    </tr>
    <?php
      $i = 0; 
      foreach ($ffeaturelocs as $featureloc){

         $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
         if($i % 2 == 0 ){
            $class = 'tripal_feature-table-odd-row tripal-table-even-row';
         } 
         $location = $featureloc->srcfeature_id->name .":". ($featureloc->fmin + 1) . ".." . $featureloc->fmax;
         if($featureloc->srcfeature_id->nid){
           $location = "<a href=\"" . url("node/".$featureloc->srcfeature_id->nid) . "\">".$featureloc->srcfeature_id->name ."</a>:".($featureloc->fmin + 1) . ".." . $featureloc->fmax ."";
         }
         ?>
         <tr class="<?php print $class ?>">
           <td><?php print $featureloc->feature_id->name;?>
           </td>
           <td><?php print $featureloc->feature_id->type_id->name ?></td>
           <td><?php print $location ?></td>
           <td><?php print $featureloc->phase ?></td>
           <td><?php 
              if($featureloc->strand == -1){
                 print "reverse";
              } 
              elseif($featureloc->strand == 1){
                 print "forward";
              } 
              elseif($featureloc->strand == 0){
                 print "N/A";
              } 
              else {
                 print $featureloc->strand;
              }?>
            </td>
         </tr>
         <?php
         $i++;  
      } ?>
    </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no alignments</div> 
  <?php }?>

  <br><br><div class="tripal_feature-info-box-desc tripal-info-box-desc">The following are aligned to <?php print $feature->name;?></div>
  <?php if(count($sfeaturelocs) > 0){ ?>
  <table id="tripal_feature-featurelocs_as_child-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Name</th>
      <th>Type</th>
      <th>Location</th>
      <th>Phase</th>
      <th>Direction</th>
    </tr>
    <?php
      $i = 0; 
      foreach ($sfeaturelocs as $featureloc){

         $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
         if($i % 2 == 0 ){
            $class = 'tripal_feature-table-odd-row tripal-table-even-row';
         } 
         $location = $featureloc->srcfeature_id->name .":". ($featureloc->fmin + 1) . ".." . $featureloc->fmax;
         ?>
         <tr class="<?php print $class ?>">
           <td><?php 
              if($featureloc->feature_id->nid){
                 print "<a href=\"" . url("node/".$featureloc->feature_id->nid) . "\">".$featureloc->feature_id->name."</a>";
              } else {
                 print $featureloc->feature_id->name;
              }?>
           </td>
           <td><?php print $featureloc->feature_id->type_id->name ?></td>
           <td><?php print $location ?></td>
           <td><?php print $featureloc->phase ?></td>
           <td><?php 
              if($featureloc->strand == -1){
                 print "reverse";
              } 
              elseif($featureloc->strand == 1){
                 print "forward";
              } 
              elseif($featureloc->strand == 0){
                 print "N/A";
              } 
              else {
                 print $featureloc->strand;
              }?>
            </td>
         </tr>
         <?php
         $i++;  
      } ?>
    </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no alignments</div> 
  <?php }?>
</div>


