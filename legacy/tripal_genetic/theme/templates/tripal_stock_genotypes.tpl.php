<?php
/*
 * Details about genotypes associated with stocks can be found in two ways by
 * traversing the the foreign key (FK) relationships in these ways:
 *
 *   Simple Method: stock => stock_genotype => genotype
 *   ND Method:     stock => nd_experiment_stock => nd_experiment => nd_experiment_genotype => genotype
 *
 * The tripal_natural_diversity module handles display of genotypes when stored using the
 * ND method.  This template handles display of genotype when stored using
 * the Simple Method.  If the tripal_natural_diversity module is enabled then this template
 * will not show.  You should instead see the tripal_stock.nd_genotypes.tpl.php template
 *
 */
$stock = $variables['node']->stock;

// specify the number of genotypes to show by default and the unique pager ID
$num_results_per_page = 25;
$stock_pager_id = 15;

// get the genotypes from the stock_genotype table
$options = [
  'return_array' => 1,
  'pager' => [
    'limit' => $num_results_per_page,
    'element' => $stock_pager_id,
  ],
  'fk_include' => [
    'genotype_id' => 1,
  ],
];
$stock = chado_expand_var($stock, 'table', 'stock_genotype', $options);
$stock_genotypes = $stock->stock_genotype;

// get the total number of records
$total_records = chado_pager_get_count($stock_pager_id);

// now iterate through the stock genotypes and print a paged table.
if (count($stock_genotypes) > 0) { ?>
    <div class="tripal_stock-data-block-desc tripal-data-block-desc">The
        following <?php print number_format($total_records) ?> genotype(s) have
        been recorded for this stock.
    </div> <?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Name', 'Type', 'Genotype', 'Details', 'Markers'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  foreach ($stock_genotypes as $stock_genotype) {
    $genotype = $stock_genotype->genotype_id;

    // get the genotype properties
    $options = ['return_array' => 1];
    $genotype = chado_expand_var($genotype, 'table', 'genotypeprop', $options);
    $properties = $genotype->genotypeprop;

    // add in markers associated with this genotype if any
    $options = [
      'return_array' => 1,
      'inlude_fk' => [
        'feature_id' => [
          'type_id' => 1,
        ],
      ],
    ];
    $genotype = chado_expand_var($genotype, 'table', 'feature_genotype', $options);
    $feature_genotypes = $genotype->feature_genotype;

    // show the uniquename for the genotype unless a name exists
    $name = $genotype->uniquename;
    if ($genotype->name) {
      $name = $genotype->name;
    }
    // get the genotype type
    $type = 'N/A';
    if ($genotype->type_id) {
      $type = ucwords(preg_replace('/_/', ' ', $genotype->type_id->name));
    }

    // get the genotype properties
    $details = '';
    if (count($properties) > 0) {
      foreach ($properties as $property) {
        $details .= ucwords(preg_replace('/_/', ' ', $property->type_id->name)) . ': ' . $property->value . '<br>';
      }
      $details = substr($details, 0, -4); // remove trailing <br>
    }

    // build the list of marker features.
    $feature_names = 'N/A';
    if (count($feature_genotypes) > 0) {
      $feature_names = '';
      foreach ($feature_genotypes as $feature_genotype) {
        $feature = $feature_genotype->feature_id;
        $feature_name = $feature->name . ' (' . $feature->uniquename . ')';
        if (property_exists($feature, 'nid')) {
          $feature_name = l($feature_name, 'node/' . $feature->nid, ['attributes' => ['target' => '_blank']]);
        }
        $feature_names .= $feature_name . '<br>';
      }
      $feature_names = substr($feature_names, 0, -4); // remove trailing <br>
    }

    // add the fields to the table row
    $rows[] = [
      $name,
      $type,
      $genotype->description,
      $details,
      $feature_names,
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
      'id' => 'tripal_genetic-table-genotypes',
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

  // the $pager array values that control the behavior of the pager.  For
  // documentation on the values allows in this array see:
  // https://api.drupal.org/api/drupal/includes!pager.inc/function/theme_pager/7
  // here we add the paramter 'block' => 'features'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  // We remove the 'pane' parameter from the original query parameters because
  // Drupal won't reset the parameter if it already exists.
  $get = $_GET;
  unset($_GET['pane']);
  $pager = [
    'tags' => [],
    'element' => $stock_pager_id,
    'parameters' => [
      'pane' => 'genotypes',
    ],
    'quantity' => $num_results_per_page,
  ];
  print theme_pager($pager);
  $_GET = $get;
}