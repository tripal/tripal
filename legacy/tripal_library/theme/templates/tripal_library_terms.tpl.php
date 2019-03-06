<?php

$library = $variables['node']->library;

$options = ['return_array' => 1];
$library = chado_expand_var($library, 'table', 'library_cvterm', $options);
$terms = $library->library_cvterm;

// order the terms by CV
$s_terms = [];
if ($terms) {
  foreach ($terms as $term) {
    $s_terms[$term->cvterm_id->cv_id->name][] = $term;
  }
}

if (count($s_terms) > 0) { ?>
    <div class="tripal_library-data-block-desc tripal-data-block-desc">The
        following terms have been associated with
        this <?php print $node->library->type_id->name ?>:
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
        $accession,
        $term->cvterm_id->name,
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
        'id' => "tripal_library-table-terms-$i",
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => '<b>Vocabulary: ' . ucwords(preg_replace('/_/', ' ', $cv)) . '</b>',
      'colgroups' => [],
      'empty' => '',
    ];

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    print theme_table($table);
    $i++;
  }
} ?>
