<?php

$feature = $variables['node']->feature;

// expand the feature object to include the feature relationships.
// since there two foreign keys (object_id and subject_id) in the 
// feature_relationship table, we will access each one separately 
$feature = tripal_core_expand_chado_vars($feature,
   'table','feature_relationship', array('order_by'=>array('rank' => 'ASC')));

// get the featurelocs. if only one featureloc exists then we want to convert
// the object into an array, otherwise the value is an array
$orelationships = $feature->feature_relationship->object_id;
if (!$orelationships) {
   $orelationships = array();
} elseif (!is_array($orelationships)) { 
   $orelationships = array($orelationships); 
}
// do the same for the subject relationships
$srelationships = $feature->feature_relationship->subject_id;
if (!$srelationships) {
   $srelationships = array();
} elseif (!is_array($srelationships)) { 
   $srelationships = array($srelationships); 
}
// now combine the two
$relationships = array_merge($orelationships,$srelationships);


?>
<div id="tripal_feature-relationships-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Relationships</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">Subject relationships</div>
  <?php if(count($srelationships) > 0){ ?>
  <table id="tripal_feature-subject_relationships-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Subject</th>
      <th>Type</th>
      <th>Relationship</th>
      <th>Object</th>
      <th>Type</th>
    </tr>
    <?php
    $i = 0; 

    foreach ($srelationships as $relationship){  
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
      $subject_name = $relationship->subject_id->name;
      if(!$subject_name){
         $subject_name = $relationship->subject_id->uniquename;
      }
      $object_name = $relationship->object_id->name;
      if(!$object_name){
         $object_name = $relationship->object_id->uniquename;
      }?>
      <tr class="<?php print $class ?>">
        <td><?php print $subject_name?></td>
        <td><?php print $relationship->subject_id->type_id->name?></td>
        <td><b><?php print $relationship->type_id->name?></b></td>
        <td>
           <?php if(isset($relationship->object_id->nid)){
                  print "<a href=\"" . url("node/".$relationship->object_id->nid) . "\">$object_name</a>";
           } else {
                  print "$object_name";
           }?> 
        </td>
        <td><?php print $relationship->object_id->type_id->name?></td>
      </tr>
    <?php } ?>
  </table>
  <?php } else {?>
    <div class="tripal-no-results">There are no subject relationships for this feature</div>
  <?php } ?> 


  <br><br><div class="tripal_feature-info-box-desc tripal-info-box-desc">Object relationships</div>
  <?php if(count($orelationships) > 0){ ?>

  <table id="tripal_feature-object_relationships-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Subject</th>
      <th>Type</th>
      <th>Relationship</th>
      <th>Object</th>
      <th>Type</th>
    </tr>
    <?php
    $i = 0; 

    foreach ($orelationships as $relationship){  
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
      $subject_name = $relationship->subject_id->name;
      if(!$subject_name){
         $subject_name = $relationship->subject_id->uniquename;
      }
      $object_name = $relationship->object_id->name;
      if(!$object_name){
         $object_name = $relationship->object_id->uniquename;
      }?>
      <tr class="<?php print $class ?>">
        <td>
           <?php if(isset($relationship->subject_id->nid)){
                  print "<a href=\"" . url("node/".$relationship->subject_id->nid) . "\">$subject_name</a>";
           } else {
                  print "$subject_name";
           }?>     
        </td>
        <td><?php print $relationship->subject_id->type_id->name?></td>
        <td><b><?php print $relationship->type_id->name?></b></td>
        <td><?php  print "$object_name";?> </td>
        <td><?php print $relationship->object_id->type_id->name?></td>
      </tr>
    <?php } ?>
  </table>
  <?php } else {?>
    <div class="tripal-no-results">There are no object relationships for this feature</div>
  <?php } ?> 
</div>

