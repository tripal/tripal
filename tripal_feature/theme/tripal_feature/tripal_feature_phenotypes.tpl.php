<?php
$feature = $variables['node']->feature;

// expand the feature object to include the phenotypes from the feature_phenotypes 
// table in chado.
$options = array(
  'return_array' => 1,
  'include_fk' => array(
    'phenotype_id' => array(
      'attr_id' => 1,
      'cvalue_id' => 1,
      'assay_id' => 1,
      'observable_id' => 1,
    )
  )
);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_phenotype', $options);
$feature_phenotypes = $feature->feature_phenotype;

// expand the text fields
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'field', 'phenotype.value', $options);
$feature = tripal_core_expand_chado_vars($feature, 'field', 'phenotype.uniquename', $options);
$feature = tripal_core_expand_chado_vars($feature, 'field', 'phenotype.name', $options);


if(count($feature_phenotypes) > 0){ ?>
  <div id="tripal_feature-phenotypes-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Phenotypes</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature is associated with the following phenotypes</div>

    <table id="tripal_feature-phenotypes-table" class="tripal_feature-table tripal-table tripal-table-horz">
      <tr>   
        <th>Attribute</th>     
        <th>Observed Unit</th>               
        <th>Value</th>
        <th>Evidence Type</th>
      </tr> <?php
      $i = 0; 
      foreach ($feature_phenotypes as $index => $feature_phenotype){
        $phenotype = $feature_phenotype->phenotype_id;
        $class = 'tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php print $phenotype->attr_id->name?></td>
          <td><?php print $phenotype->observable_id->name?></td>          
          <td><?php print $phenotype->cvalue_id ? $phenotype->cvalue_id->name : $phenotype->value ?></td>
          <td><?php print $phenotype->assay_id ?></td>
        </tr> <?php
        $i++;  
      } ?>
    </table>
  </div><?php
}
