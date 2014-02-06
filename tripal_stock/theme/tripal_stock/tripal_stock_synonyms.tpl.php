<?php
// there is no stock_synonym table, analogous to the stock_synonym table.
// Therefore, synonyms have been stored in the stockprop table with a type 
// of 'synonym' or 'alias'.
$stock = $node->stock;
$synonyms = array();

// expand the stock object to include the stockprop records
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stockprop', $options);
$stockprops = $stock->stockprop;

// iterate through all of the properties and pull out only the synonyms
if ($stockprops) {
  foreach ($stockprops as $stockprop){    
    if($stockprop->type_id->name == 'synonym' or $stockprop->type_id->name == 'alias'){
      $synonyms[] = $stockprop;
    }
  }
}

if(count($synonyms) > 0){ ?>
  <div class="tripal_stock-data-block-desc tripal-data-block-desc">The stock '<?php print $stock->name ?>' has the following synonyms</div> <?php
  
  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = array('Synonym');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = array();
  foreach ($synonyms as $property){
    $rows[] = array(
      $property->value,
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
      'id' => 'tripal_stock-table-synonyms',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
}
