<?php
$pub = $variables['node']->pub;
$stocks = array();

// get the stocks that are associated with this publication.  But we only
// want 25 and we want a pager to let the user cycle between pages of stocks.
// so we, use the tripal_core_chado_select API function to get the results and
// generate the pager.  The function is smart enough to know which page the user is
// on and retrieves the proper set of stocks

$element = 5;        // an index to specify the pager this must be unique amongst all pub templates
$num_per_page = 25;  // the number of stocks to show per page$num_results_per_page = 25; 

// get the stocks from the stock_pub table
$options = array(  
  'return_array' => 1,
  'pager' => array(
    'limit'   => $num_per_page, 
    'element' => $element
  ),
);

$pub = tripal_core_expand_chado_vars($pub, 'table', 'stock_pub', $options);
$stock_pubs = $pub->stock_pub;
if (count($stock_pubs) > 0 ) {
  foreach ($stock_pubs as $stock_pub) {    
    $stocks[] = $stock_pub->stock_id;
  }
}

// get the total number of records
$total_records = chado_pager_get_count($element);

if(count($stocks) > 0){ ?>
  <div class="tripal_pub-data-block-desc tripal-data-block-desc">This publication contains information about <?php print number_format($total_records) ?> stocks:</div> <?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Stock Name', 'Uniquenaem', 'Type');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($stocks as $stock){
     $stock_name = $stock->name;
     if (property_exists($stock, 'nid')) {
       $stock_name = l($stock_name, 'node/' . $stock->nid, array('attributes' => array('target' => '_blank')));
     }
     
     $rows[] = array(
       $stock_name,
       $stock->uniquename,
       $stock->type_id->name,
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
      'id' => 'tripal_pub-table-stocks',
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
  // here we add the paramter 'block' => 'stocks'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  $pager = array(
    'tags' => array(),
    'element' => $element,
    'parameters' => array(
      'block' => 'stocks'
    ),
    'quantity' => $num_per_page,
  );
  print theme_pager($pager); 
}

