<?php
$feature = $variables['node']->feature;

// expand the feature object to include the synonyms from the feature_synonym 
// table in chado.
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_synonym', $options);
$synonyms = $feature->feature_synonym;


if(count($synonyms) > 0){ ?>
  <div id="tripal_feature-synonyms-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Synonyms</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature '<?php print $feature->name ?>' has the following synonyms</div>

    <table id="tripal_feature-synonyms-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>
        <th>Synonym</th>
      </tr> <?php
      $i = 0; 
      foreach ($synonyms as $feature_synonym){
        $class = 'tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php print $feature_synonym->synonym_id->name?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table>
  </div><?php
}
