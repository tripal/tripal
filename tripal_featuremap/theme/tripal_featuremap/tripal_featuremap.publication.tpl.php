<?php
$featuremap  = $variables['node']->featuremap;

// expand featuremap to include pubs 
$featuremap = tripal_core_expand_chado_vars($featuremap, 'table', 'featuremap_pub');
$pubs = $featuremap->featuremap_pub;
$pubs = tripal_core_expand_chado_vars($pubs, 'field', 'pub.title');

if (count($pubs) > 0) { ?>

  <div id="tripal_featuremap-pub-box" class="tripal_featuremap-info-box tripal-info-box">
    <div class="tripal_featuremap-info-box-title tripal-info-box-title">Publications</div>
    <div class="tripal_featuremap-info-box-desc tripal-info-box-desc"></div> <?php 
  
    // the $headers array is an array of fields to use as the colum headers.
    // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $headers = array('Year', 'Publication');
    
    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.dr
    $rows = array();
    
    foreach ($pubs AS $pub) {
      // get the citation
      $values = array(
        'pub_id' => $pub->pub_id->pub_id,
        'type_id' => array(
          'name' => 'Citation',
        ),
      );
      $citation = tripal_core_generate_chado_var('pubprop', $values);
      $citation = tripal_core_expand_chado_vars($citation, 'field', 'pubprop.value');
      if(property_exists($pub->pub_id, 'nid')) {
        $citation->value = l($citation->value, 'node/' . $pub->pub_id->nid, array('attributes' => array('target' => '_blank')));
      }
      $rows[] = array(
        $pub->pub_id->pyear,
        $citation->value,
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
        'id' => 'tripal_pub-table-publications',
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    );
    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    print theme_table($table);?>
  </div> <?php 
}?>
