<?php

$organism = $variables['node']->organism;

// get the list of available sequence ontology terms for which
// we will build drupal pages from features in chado.  If a feature
// is not one of the specified typse we won't build a node for it.
$allowed_types = variable_get('chado_browser_feature_types', 'gene mRNA');
$allowed_types = preg_replace("/[\s\n\r]+/", " ", $allowed_types);
$so_terms = explode(' ', $allowed_types);

// get the feature_id's of the feature that belong to this organism.  But we only
// want 25 and we want a pager to let the user cycle between pages of features.
// so we, use the tripal_core_chado_select API function to get the results and
// generate the pager.  The function is smart enough to know which page the user is
// on and retrieves the proper set of features
$element = 0;        // an index to specify the pager if more than one is on the page
$num_per_page = 25;  // the number of features to show per page
$values = array(
  'organism_id' => $organism->organism_id,
  'type_id' => array(
    'name' => $so_terms
  ),
);
$columns = array('feature_id');
$options = array(
  'pager' => array(
    'limit' => $num_per_page, 
    'element' => $element
   ),
  'order_by' => array('name' => 'ASC'),
);
$results = tripal_core_chado_select('feature', $columns, $values, $options);

// now that we have all of the feature IDs, we want to expand each one so that we
// have all of the neccessary values, including the node ID, if one exists, and the
// cvterm type name.
$features = array();
foreach ($results as $result) {
  $values = array('feature_id' => $result->feature_id);
  $options = array(
    'include_fk' => array(
      'type_id' => 1
    )
  );
  $features[] = tripal_core_generate_chado_var('feature', $values, $options);
}

if (count($features) > 0) { ?>
  <div class="tripal_organism-data-block-desc tripal-data-block-desc">The following browser provides a quick view for new visitors.  Use the searching mechanism to find specific features.</div> <?php
  
  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Feature Name' ,'Unique Name', 'Type');
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();
  
  // let admins know they can customize the terms that appear in the list
  print tripal_set_message("Administrators, you can specify the feature types ".
    "that should appear in this browser or remove it from the list of resources ".
    "by navigating to the ".
    l("Tripal feature settings page", "admin/tripal/chado/tripal_feature/configuration", array('attributes' => array('target' => '_blank'))),
    TRIPAL_INFO,
    array('return_html' => 1));
  
  foreach ($features as $feature){
    $fname =  $feature->name;
    if (property_exists($feature, 'nid')) {
      $fname =   l($fname, "node/$feature->nid", array('attributes' => array('target' => '_blank')));
    }
    $rows[] = array(
      $fname,
      $feature->uniquename,
      $feature->type_id->name
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
      'id' => 'tripal_organism-table-features',
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
  // here we add the paramter 'block' => 'feature_browser'. This is because
  // the pager is not on the default block that appears. When the user clicks a
  // page number we want the browser to re-appear with the page is loaded.
  $pager = array(
    'tags' => array(),
    'element' => $element,
    'parameters' => array(
      'block' => 'feature_browser'
    ),
    'quantity' => $num_per_page,
  );
  print theme_pager($pager); 
} 
else {  ?>
  <p>There are no results.</p><?php
  print tripal_set_message("
    Administrators, perform the following to show features in this browser:
    <ul>
      <li>Load features for this organism using the " .
        l("FASTA loader", 'admin/tripal/loaders/fasta_loader') . ",  ".
        l("GFF Loader",   'admin/tripal/loaders/gff3_load') . " or ".
        l("Bulk Loader",  'admin/tripal/loaders/bulk'). "</li>
      <li>Sync the features that should have pages using the ".
        l("Sync features page", 'admin/tripal/chado/tripal_feature/sync'). "</li>
      <li>Return to this page to browse features.</li>
      <li>Ensure the user " .
       l("has permission", 'admin/people/permissions') . " to view the feature content</li>
    </ul>
    <br>
    <br>
    You can specify the feature types
    that should appear in this browser or remove it from the list of resources by navigating to the " . 
    l("Tripal feature settings page", "admin/tripal/chado/tripal_feature/configuration", array('attributes' => array('target' => '_blank')))  . "
    </p>
    The feature browser will not appear to site visitors unless features are present. ",
    TRIPAL_INFO,
    array('return_html' => 1));
}



