<?php
// expand the feature object to include the phenotypes from the feature_phenotypes table in chado.
$feature = $variables['node']->feature;
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

if(count($feature_phenotypes) > 0){ 
  
  // expand the text fields
  $options = array('return_array' => 1);
  $feature = tripal_core_expand_chado_vars($feature, 'field', 'phenotype.value', $options);
  $feature = tripal_core_expand_chado_vars($feature, 'field', 'phenotype.uniquename', $options);
  $feature = tripal_core_expand_chado_vars($feature, 'field', 'phenotype.name', $options); ?>

  <div id="tripal_feature-phenotypes-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Phenotypes</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc">The feature is associated with the following phenotypes</div><?php

    // the $headers array is an array of fields to use as the colum headers.
    // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $headers = array('Attribute', 'Observed Unit', 'Value', 'Evidence Type');
    
    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $rows = array();
    foreach ($feature_phenotypes as $index => $feature_phenotype){
      $phenotype = $feature_phenotype->phenotype_id;
      $rows[] = array(
        $phenotype->attr_id->name,
        $phenotype->observable_id->name,
        $phenotype->cvalue_id ? $phenotype->cvalue_id->name : $phenotype->value,
        $phenotype->assay_id
      );
    } 
    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_feature-table-phenotypes',
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    );
    
    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    print theme_table($table); ?>
  </div> <?php
}
