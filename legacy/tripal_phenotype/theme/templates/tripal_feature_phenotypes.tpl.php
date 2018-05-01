<?php
// expand the feature object to include the phenotypes from the feature_phenotypes table in chado.
$feature = $variables['node']->feature;
$options = [
  'return_array' => 1,
  'include_fk' => [
    'phenotype_id' => [
      'attr_id' => 1,
      'cvalue_id' => 1,
      'assay_id' => 1,
      'observable_id' => 1,
    ],
  ],
];
$feature = chado_expand_var($feature, 'table', 'feature_phenotype', $options);
$feature_phenotypes = $feature->feature_phenotype;

if (count($feature_phenotypes) > 0) { ?>

    <div class="tripal_feature-data-block-desc tripal-data-block-desc">The
        feature is associated with the following phenotypes
    </div><?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Attribute', 'Observed Unit', 'Value', 'Evidence Type'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  // iterate through the feature_phenotype records
  foreach ($feature_phenotypes as $feature_phenotype) {
    $phenotype = $feature_phenotype->phenotype_id;

    // expand the text fields
    $options = ['return_array' => 1];
    $phenotype = chado_expand_var($phenotype, 'field', 'phenotype.value', $options);
    $phenotype = chado_expand_var($phenotype, 'field', 'phenotype.uniquename', $options);
    $phenotype = chado_expand_var($phenotype, 'field', 'phenotype.name', $options);

    // get the phenotype value. If the value is qualitative the cvalue_id will link to a type. 
    // If quantitative we use the value column
    $phen_value = $phenotype->value . '<br>';
    if ($phenotype->cvalue_id) {
      $phen_value .= ucwords(preg_replace('/_/', ' ', $phenotype->cvalue_id->name)) . '<br>';
    }

    $phen_value = $phenotype->cvalue_id ? $phenotype->cvalue_id->name : $phenotype->value;
    $phenotype = $feature_phenotype->phenotype_id;
    $rows[] = [
      $phenotype->attr_id->name,
      $phenotype->observable_id->name,
      $phen_value,
      $phenotype->assay_id->name,
    ];
  }
  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_feature-table-phenotypes',
      'class' => 'tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
}
