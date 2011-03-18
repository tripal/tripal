<?php

$subject_relationships = $variables['tripal_feature']['subject_relationships'];
$feature = $variables['node']->feature;

?>
<div id="tripal_feature-subject_relationships-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Subject Relationships</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->featurename ?>' has a subject relationship with the following</div>
  <?php if(count($subject_relationships) > 0){ ?>
  <table id="tripal_feature-subject_relationships-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Relationship</th>
      <th>Feature</th>
      <th>Type</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($subject_relationships as $result){ 
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      } ?>
      <tr class="<?php print $class ?>">
        <td><b><?php print $result->rel_type?></b></td>
        <td> 
           <?php if(isset($result->object_nid)){
             print "<a href=\"" . url("node/$result->object_nid") . "\">$result->object_name</a> ";
           } else {
             print "$result->object_name ";
           } ?>
        </td>
        <td><?php print $result->object_type?></td>          
      </tr>
      <?php
      $i++;  
    }?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no subject relationships for this feature</div> 
  <?php }?>
</div>

