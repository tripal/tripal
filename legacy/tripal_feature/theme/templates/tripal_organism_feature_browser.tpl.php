<?php

$organism = $variables['node']->organism;

// get the list of available sequence ontology terms for which
// we will build drupal pages from features in chado.  If a feature
// is not one of the specified typse we won't build a node for it.
$allowed_types = variable_get('chado_browser_feature_types');
$allowed_types = preg_replace("/[\s\n\r]+/", " ", $allowed_types);
$so_terms = explode(' ', $allowed_types);

// Don't show the browser if there are no terms
if (count($so_terms) > 0) {

  // get the feature_id's of the feature that belong to this organism.  But we only
  // want 25 and we want a pager to let the user cycle between pages of features.
  // so we, use the chado_select_record API function to get the results and
  // generate the pager.  The function is smart enough to know which page the user is
  // on and retrieves the proper set of features
  $element = 0;        // an index to specify the pager if more than one is on the page
  $num_per_page = 25;  // the number of features to show per page
  $values = [
    'organism_id' => $organism->organism_id,
    'type_id' => [
      'name' => $so_terms,
    ],
  ];
  $columns = ['feature_id'];
  $options = [
    'pager' => [
      'limit' => $num_per_page,
      'element' => $element,
    ],
    'order_by' => ['name' => 'ASC'],
  ];
  $results = chado_select_record('feature', $columns, $values, $options);

  // now that we have all of the feature IDs, we want to expand each one so that we
  // have all of the neccessary values, including the node ID, if one exists, and the
  // cvterm type name.
  $features = [];
  foreach ($results as $result) {
    $values = ['feature_id' => $result->feature_id];
    $options = [
      'include_fk' => [
        'type_id' => 1,
      ],
    ];
    $features[] = chado_generate_var('feature', $values, $options);
  }

  if (count($features) > 0) { ?>
      <div class="tripal_organism-data-block-desc tripal-data-block-desc">The
          following browser provides a quick view for new visitors. Use the
          searching mechanism to find specific features.
      </div> <?php

    // the $headers array is an array of fields to use as the colum headers.
    // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $headers = ['Feature Name', 'Unique Name', 'Type'];

    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $rows = [];

    // let admins know they can customize the terms that appear in the list
    print tripal_set_message("Administrators, you can specify the feature types " .
      "that should appear in this browser or remove it from the list of resources " .
      "by navigating to the " .
      l("Tripal feature settings page", "admin/tripal/legacy/tripal_feature/configuration", ['attributes' => ['target' => '_blank']]),
      TRIPAL_INFO,
      ['return_html' => 1]
    );

    foreach ($features as $feature) {
      $fname = $feature->name;
      if (property_exists($feature, 'nid')) {
        $fname = l($fname, "node/$feature->nid", ['attributes' => ['target' => '_blank']]);
      }
      $rows[] = [
        $fname,
        $feature->uniquename,
        $feature->type_id->name,
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
        'id' => 'tripal_organism-table-features',
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
    // here we add the paramter 'block' => 'feature_browser'. This is because
    // the pager is not on the default block that appears. When the user clicks a
    // page number we want the browser to re-appear with the page is loaded.
    // We remove the 'pane' parameter from the original query parameters because
    // Drupal won't reset the parameter if it already exists.
    $get = $_GET;
    unset($_GET['pane']);
    $pager = [
      'tags' => [],
      'element' => $element,
      'parameters' => [
        'pane' => 'feature_browser',
      ],
      'quantity' => $num_per_page,
    ];
    print theme_pager($pager);
    $_GET = $get;

    print tripal_set_message("
      Administrators, please note that the feature browser will be retired in
      a future version of Tripal.",
      TRIPAL_INFO,
      ['return_html' => 1]);
  }
}


