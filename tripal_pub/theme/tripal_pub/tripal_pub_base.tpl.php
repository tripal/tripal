<?php
$pub = $variables['node']->pub;

// expand the title
$pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.title');
$pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.volumetitle');

// get the citation
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'Citation',
  ),
);
$citation = tripal_core_generate_chado_var('pubprop', $values); 
$citation = tripal_core_expand_chado_vars($citation, 'field', 'pubprop.value');

// get the abstract
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'Abstract',
  ),
);
$abstract = tripal_core_generate_chado_var('pubprop', $values); 
$abstract = tripal_core_expand_chado_vars($abstract, 'field', 'pubprop.value');
$abstract_text = 'N/A';
if ($abstract) {
  $abstract_text = htmlspecialchars($abstract->value);
}

// get the author list
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'Authors',
  ),
);
$authors = tripal_core_generate_chado_var('pubprop', $values); 
$authors = tripal_core_expand_chado_vars($authors, 'field', 'pubprop.value');
$authors_list = 'N/A';
if ($authors) {
  $authors_list = $authors->value;
} 

// get the first database cross-reference with a url
$options = array('return_array' => 1);
$pub = tripal_core_expand_chado_vars($pub, 'table', 'pub_dbxref', $options);
$dbxref = NULL;
if ($pub->pub_dbxref) { 
  foreach ($pub->pub_dbxref as $index => $pub_dbxref) {
    if ($pub_dbxref->dbxref_id->db_id->urlprefix) {
      $dbxref = $pub_dbxref->dbxref_id;
    }
  }
}

// get the URL
// get the author list
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'URL',
  ),
);
$options = array('return_array' => 1);
$urls = tripal_core_generate_chado_var('pubprop', $values, $options); 
$urls = tripal_core_expand_chado_vars($urls, 'field', 'pubprop.value');
$url = '';
if (count($urls) > 0) {
  $url = $urls[0]->value; 
}?>

<div id="tripal_pub-base-box" class="tripal_pub-info-box tripal-info-box">
  <div class="tripal_pub-info-box-title tripal-info-box-title">Publication Details</div>
  <!-- <div class="tripal_pub-info-box-desc tripal-info-box-desc"></div> -->
  <?php 
  if ($pub->is_obsolete == 't') { ?>
    <div class="tripal_pub-obsolete">This publication is obsolete</div> <?php 
  }  

  // to simplify the template, we have a subdirectory named 'pub_types'.  This directory
  // should have include files each specific to a publication type. If the type is 
  // not present then the base template will be used, otherwise the template in the
  // include file is used.
  $inc_name = strtolower(preg_replace('/ /', '_', $pub->type_id->name)) . '.inc';
  $inc_path = realpath('./') . '/' . drupal_get_path('theme', 'tripal') . "/tripal_pub/pub_types/$inc_name";
  if (file_exists($inc_path)) {
    require_once "pub_types/$inc_name";  
  } 
  else { 
    // the $headers array is an array of fields to use as the colum headers. 
    // additional documentation can be found here 
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    // This table for the analysis has a vertical header (down the first column)
    // so we do not provide headers here, but specify them in the $rows array below.
    $headers = array();
    
    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
    $rows = array();
  
    // Title row
    $title = '';
    if ($url) {
      $title =  l(htmlspecialchars($pub->title), $url, array('attributes' => array('target' => '_blank')));
    }
    elseif ($dbxref and $dbxref->db_id->urlprefix) {
      $title =  l(htmlspecialchars($pub->title), $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));
    }
    else {
      $title =  htmlspecialchars($pub->title);
    }
    $rows[] = array(
      array(
        'data' => 'Title',
        'header' => TRUE
      ),
      $title,
    );
    // Authors row
    $rows[] = array(
      array(
        'data' => 'Authors',
        'header' => TRUE
      ),
      $authors_list,
    );
    // Type row
    $rows[] = array(
      array(
        'data' => 'Type',
        'header' => TRUE
      ),
      $pub->type_id->name,
    );
    // Media Title
    $rows[] = array(
      array(
        'data' => 'Type',
        'header' => TRUE
      ),
      $pub->series_name,
    );
    // Volume
    $rows[] = array(
      array(
        'data' => 'Volume',
        'header' => TRUE
      ),
      $pub->volume ? $pub->volume : 'N/A',
    );
    // Issue
    $rows[] = array(
      array(
        'data' => 'Issue',
        'header' => TRUE
      ),
      $pub->issue ? $pub->issue : 'N/A'
    );
    // Year
    $rows[] = array(
      array(
        'data' => 'Year',
        'header' => TRUE
      ),
      $pub->pyear
    );
    // Pages
    $rows[] = array(
      array(
        'data' => 'Page(s)',
        'header' => TRUE
      ),
      $pub->pages ? $pub->pages : 'N/A'
    );
    // Citation row
    $rows[] = array(
      array(
        'data' => 'Citation',
        'header' => TRUE
      ),
      htmlspecialchars($citation->value)
    );
    // Abstract
    $rows[] = array(
      array(
        'data' => 'Abstract',
        'header' => TRUE
      ),
      $abstract_text
    );

    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_pub-table-base',
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    );
    
    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    print theme_table($table);
  } ?>
</div>