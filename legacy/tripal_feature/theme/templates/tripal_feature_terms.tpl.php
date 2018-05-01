<?php

$feature = $variables['node']->feature;

$options = ['return_array' => 1];
$feature = chado_expand_var($feature, 'table', 'feature_cvterm', $options);
$terms = $feature->feature_cvterm;

// order the terms by CV
$s_terms = [];
if ($terms) {
  foreach ($terms as $term) {
    $s_terms[$term->cvterm_id->cv_id->name][] = $term;
  }
}

if (count($s_terms) > 0) { ?>
    <div class="tripal_feature-data-block-desc tripal-data-block-desc">The
        following terms have been associated with
        this <?php print $node->feature->type_id->name ?>:
    </div>  <?php

  // iterate through each term
  $i = 0;
  foreach ($s_terms as $cv => $terms) {
    // the $headers array is an array of fields to use as the colum headers.
    // additional documentation can be found here
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $headers = ['Term', 'Definition'];

    // the $rows array contains an array of rows where each row is an array
    // of values for each column of the table in that row.  Additional documentation
    // can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $rows = [];

    foreach ($terms as $term) {
      $accession = $term->cvterm_id->dbxref_id->accession;
      if (is_numeric($term->cvterm_id->dbxref_id->accession)) {
        $accession = $term->cvterm_id->dbxref_id->db_id->name . ":" . $term->cvterm_id->dbxref_id->accession;
      }
      if ($term->cvterm_id->dbxref_id->db_id->urlprefix) {
        $accession = l($accession, $term->cvterm_id->dbxref_id->db_id->urlprefix . $accession, ['attributes' => ["target" => '_blank']]);
      }

      $rows[] = [
        ['data' => $accession, 'width' => '15%'],
        $term->cvterm_id->name,
      ];
    }

    // generate the link to configure a database, b ut only if the user is
    // a tripal administrator
    $configure_link = '';
    if (user_access('view ids')) {
      $db_id = $term->cvterm_id->dbxref_id->db_id->db_id;
      $configure_link = l('[configure term links]', "admin/tripal/legacy/tripal_db/edit/$db_id", ['attributes' => ["target" => '_blank']]);
    }

    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => "tripal_feature-table-terms-$i",
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => 'Vocabulary:  <b>' . ucwords(preg_replace('/_/', ' ', $cv)) . '</b> ' . $configure_link,
      'colgroups' => [],
      'empty' => '',
    ];

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    print theme_table($table);
    $i++;
  }
}
