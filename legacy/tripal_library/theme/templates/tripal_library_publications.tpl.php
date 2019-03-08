<?php
$library = $variables['node']->library;

// expand library to include pubs 
$options = ['return_array' => 1];
$library = chado_expand_var($library, 'table', 'library_pub', $options);
$library_pubs = $library->library_pub;


if (count($library_pubs) > 0) { ?>
    <div class="tripal_library_pub-data-block-desc tripal-data-block-desc"></div> <?php

  // the $headers array is an array of fields to use as the colum headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = ['Year', 'Publication'];

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = [];

  foreach ($library_pubs as $library_pub) {
    $pub = $library_pub->pub_id;
    $pub = chado_expand_var($pub, 'field', 'pub.title');
    $citation = $pub->title;  // use the title as the default citation

    // get the citation for this pub if it exists
    $values = [
      'pub_id' => $pub->pub_id,
      'type_id' => [
        'name' => 'Citation',
      ],
    ];
    $options = ['return_array' => 1];
    $citation_prop = chado_generate_var('pubprop', $values, $options);
    if (count($citation_prop) == 1) {
      $citation_prop = chado_expand_var($citation_prop, 'field', 'pubprop.value');
      $citation = $citation_prop[0]->value;
    }

    // if the publication is synced then link to it
    if ($pub->nid) {
      // replace the title with a link
      $link = l($pub->title, 'node/' . $pub->nid, ['attributes' => ['target' => '_blank']]);
      $patterns = [
        '/(\()/',
        '/(\))/',
        '/(\])/',
        '/(\[)/',
        '/(\{)/',
        '/(\})/',
        '/(\+)/',
        '/(\.)/',
        '/(\?)/',
      ];
      $fixed_title = preg_replace($patterns, "\\\\$1", $pub->title);
      $citation = preg_replace('/' . $fixed_title . '/', $link, $citation);
    }

    $rows[] = [
      $pub->pyear,
      $citation,
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
      'id' => 'tripal_library-table-publications',
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
}
