<?php
$featuremap = $variables['node']->featuremap;

// expand featuremap to include pubs 
$options = array('return_array' => 1);
$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featuremap_pub', $options);
$featuremap_pubs = $featuremap->featuremap_pub; 


if (count($featuremap_pubs) > 0) { ?>
  <div class="tripal_featuremap_pub-data-block-desc tripal-data-block-desc"></div> <?php 

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Year', 'Publication');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  foreach ($featuremap_pubs as $featuremap_pub) {
    $pub = $featuremap_pub->pub_id;
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.title');
    $citation = $pub->title;  // use the title as the default citation
    
    // get the citation for this pub if it exists
    $values = array(
      'pub_id' => $pub->pub_id, 
      'type_id' => array(
        'name' => 'Citation',
      ),
    );
    $options = array('return_array' => 1);
    $citation_prop = tripal_core_generate_chado_var('pubprop', $values, $options); 
    if (count($citation_prop) == 1) {
      $citation_prop = tripal_core_expand_chado_vars($citation_prop, 'field', 'pubprop.value');
      $citation = $citation_prop[0]->value;
    }
    
    // if the publication is synced then link to it
    if (property_exists($pub, 'nid')) {
      // replace the title with a link
      $link = l($pub->title, 'node/' . $pub->nid ,array('attributes' => array('target' => '_blank')));
      $patterns = array(
        '/(\()/', '/(\))/', 
        '/(\])/', '/(\[)/',
        '/(\{)/', '/(\})/',
        '/(\+)/', '/(\.)/', '/(\?)/', 
      );
      $fixed_title = preg_replace($patterns, "\\\\$1", $pub->title);
      $citation = preg_replace('/' . $fixed_title . '/', $link, $citation);
    }
    
    $rows[] = array(
      $pub->pyear,
      $citation,
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
      'id' => 'tripal_featuremap-table-publications',
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
