<?php
$feature = $variables['node']->feature;

// expand the feature object to include the synonyms from the feature_synonym 
// table in chado.
$feature = tripal_core_expand_chado_vars($feature,'table','feature_synonym');

// get the references. if only one reference exists then we want to convert
// the object into an array, otherwise the value is an array
$synonyms = $feature->feature_synonym;
if (!$synonyms) {
   $synonyms = array();
} elseif (!is_array($synonyms)) { 
   $synonyms = array($synonyms); 
}

?>
<div id="tripal_feature-synonyms-box" class="tripal_feature-info-box tripal-info-box">
  <div class="tripal_feature-info-box-title tripal-info-box-title">Synonyms</div>
  <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->name ?>' has the following synonyms</div>
  <?php if(count($synonyms) > 0){ ?>
  <table id="tripal_feature-synonyms-table" class="tripal_feature-table tripal-table tripal-table-horz">
    <tr>
      <th>Synonym</th>
    </tr>
    <?php
    $i = 0; 
    foreach ($synonyms as $feature_synonym){
      $class = 'tripal-table-odd-row';
      if($i % 2 == 0 ){
         $class = 'tripal-table-even-row';
      }
      ?>
      <tr class="<?php print $class ?>">
        <td><?php print $feature_synonym->synonym_id->name?></td>
      </tr>
      <?php
      $i++;  
    } ?>
  </table>
  <?php } else { ?>
    <div class="tripal-no-results">There are no synonyms for this feature</div> 
  <?php }?>
</div>
