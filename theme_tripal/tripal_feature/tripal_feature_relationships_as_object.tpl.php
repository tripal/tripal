<?php

$object_relationships = $variables['tripal_feature']['object_relationships'];
$feature = $variables['node']->feature;

?>
<div id="tripal_feature-object_relationships-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Object Relationships</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->featurename ?>' has an object relationship with the following</div>
  <?php if(count($object_relationships) > 0){ ?>
  <table id="tripal_feature-object_relationships-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Name</th>
      <th>Type</th>
      <th>Relationship</th>
      <th>Position</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($object_relationships as $result){   
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
      $subject_name = $result->subject_name;
      if(!$subject_name){
         $subject_name = $result->subject_uniquename;
      }?>
      <tr class="<?php print $class ?>">
        <td>
           <?php if(isset($result->subject_nid)){
                  print "<a href=\"" . url("node/$result->subject_nid") . "\">$result->subject_name ($result->subject_type)</a> ";
           } else {
                  print "$subject_name";
           }?>     
        </td>
        <td><?php print $result->subject_type?></td>
        <td><b><?php print $result->rel_type?></b></td>
        <td>
           <?php
           $featurelocs = $result->featurelocs;
           if($featurelocs){
              foreach($featurelocs as $src => $attrs){
                 print "$attrs->src_name ($attrs->src_cvname):$attrs->fmin $attrs->fmax</br>";
              } 
           }?> 
        </td>
      </tr>
    <?php } ?>
  </table>
  <?php }?>
</div>

