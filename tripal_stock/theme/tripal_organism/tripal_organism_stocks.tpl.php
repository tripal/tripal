<?php
$organism = $variables['node']->organism;

// expand the featuremap object to include the records from the featurepos table
// specify the number of features to show by default and the unique pager ID
$num_results_per_page = 25; 
$pager_id = 3;

// get the features aligned on this map
$options = array(  
  'return_array' => 1,
  'order_by' => array('name' => 'ASC'),
  'pager' => array(
    'limit' => $num_results_per_page, 
    'element' => $pager_id
  ),
  'include_fk' => array(
    'type_id' => 1    
  ),
);

$organism = tripal_core_expand_chado_vars($organism, 'table', 'stock', $options);
$stocks = $organism->stock;

// get the total number of records
$total_records = chado_pager_get_count($pager_id);

 
if (count($stocks) > 0) { ?>
  <div class="tripal_organism-data-block-desc tripal-data-block-desc">This organism is associated with <?php print number_format($total_records) ?> stock(s):</div> <?php 
  
  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = array('Name', 'Type');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = array();
  
  foreach ($stocks as $stock){
    $name = $stock->name;
    if (!$name) {
      $name = $stock->uniquename;
    }
    if ($stock->nid) {    
      $name = l($name, "node/$stock->nid", array('attributes' => array('target' => '_blank')));
    }

    $rows[] = array(
      $name,
      $stock->type_id->name
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
      'id' => 'tripal_organism-table-stocks',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
  
  // the $pager array values that control the behavior of the pager.  For
  // documentation on the values allows in this array see:
  // https://api.drupal.org/api/drupal/includes!pager.inc/function/theme_pager/7
  // here we add the paramter 'block' => 'features'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  $pager = array(
    'tags' => array(),
    'element' => $pager_id,
    'parameters' => array(
      'block' => 'stocks'
    ),
    'quantity' => $num_results_per_page,
  );
  print theme_pager($pager);  
} 




