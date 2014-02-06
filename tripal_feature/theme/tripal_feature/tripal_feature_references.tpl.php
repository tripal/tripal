<?php
$feature = $variables['node']->feature;
$references = array();

// First, get the dbxref record from feature record itself if one exists
if ($feature->dbxref_id) {
  $feature->dbxref_id->is_primary = 1;  // add this new property so we know it's the primary reference
  $references[] = $feature->dbxref_id;
}

// Second, expand the feature object to include the records from the feature_dbxref table
$options = array('return_array' => 1);
$feature = tripal_core_expand_chado_vars($feature, 'table', 'feature_dbxref', $options);
$feature_dbxrefs = $feature->feature_dbxref;
if (count($feature_dbxrefs) > 0 ) {
  foreach ($feature_dbxrefs as $feature_dbxref) {    
    if($feature_dbxref->dbxref_id->db_id->name == 'GFF_source'){
      // check to see if the reference 'GFF_source' is there.  This reference is
      // used to if the Chado Perl GFF loader was used to load the features   
    }
    else {
      $references[] = $feature_dbxref->dbxref_id;
    }
  }
}


if(count($references) > 0){ ?>
  <div class="tripal_feature-data-block-desc tripal-data-block-desc">External references for this <?php print $feature->type_id->name ?></div><?php
   
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Dababase', 'Accession');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();

  foreach ($references as $dbxref){
  
    // skip the GFF_source entry as this is just needed for the GBrowse chado adapter 
    if ($dbxref->db_id->name == 'GFF_source'){
       continue;  
    } 
    $dbname = $dbxref->db_id->name; 
    if ($dbxref->db_id->url) { 
      $dbname = l($dbname, $dbxref->db_id->url, array('attributes' => array('target' => '_blank')));
    } 
    
    $accession = $dbxref->accession; 
    if ($dbxref->db_id->urlprefix) { 
      $accession = l($accession, $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));
    } 
    if (property_exists($dbxref, 'is_primary')) {
      $accession .= " <i>(primary cross-reference)</i>";
    }
    $rows[] = array(
      $dbname,
      $accession
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
      'id' => 'tripal_feature-table-references',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table); 
}?>

