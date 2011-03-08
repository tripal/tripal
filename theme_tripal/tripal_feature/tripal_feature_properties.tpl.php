<?php
$properties = $variables['tripal_feature']['properties'];
$feature = $variables['node']->feature;
?>
<div id="tripal_feature-properties-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Properties</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->featurename ?>' has these properties</div>
  <?php if(count($properties) > 0){ ?>
  <table class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Synonym</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($properties as $result){
      $class = 'tripal_feature-table-odd-row tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal_feature-table-odd-row tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $result->name?></td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no properties for this feature</div> 
  <?php }?>
</div>
