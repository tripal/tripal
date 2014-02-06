<?php
$stock = $variables['node']->stock;

// expand the stock object to include the stockcollections associated with this stock
$options = array('return_array' => 1);
$stock = tripal_core_expand_chado_vars($stock, 'table', 'stockcollection_stock', $options);
$collections = $stock->stockcollection_stock;

if (count($collections) > 0) {?>
  <div class="tripal_stock-data-block-desc tripal-data-block-desc">This stock is found in the following collections.</div> <?php 
  
  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the analysis has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = array('Collection Name', 'Type', 'Contact');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = array();

  foreach ($collections as $collection_stock){ 
    // get the stock collection details
    $collection = $collection_stock->stockcollection_id; 
    $contact    = $collection->contact_id;
    
    $cname = $collection->name;
    if (property_exists($collection, 'nid')) {
      $cname = l($collection->name, "node/" . $collection->nid, array('attributes' => array('target' => '_blank')));
    }
    
    $rows[] = array(
      $cname,
      ucwords(preg_replace('/_/', ' ', $collection->type_id->name)),
      $contact->name,
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