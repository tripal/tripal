<?php
$synonyms = $variables['tripal_feature']['synonyms'];
$feature = $variables['node']->feature;
?>
<div id="tripal_feature-synonyms-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Synonyms</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->featurename ?>' has the following synonyms</div>
  <?php if(count($synonyms) > 0){ ?>
  <table class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Synonym</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($synonyms as $result){
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
    <div class="tripal-no-results">There are no synonyms for this feature</div> 
  <?php }?>
</div>
