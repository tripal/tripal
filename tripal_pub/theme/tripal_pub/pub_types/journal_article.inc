<?php 
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
    'header' => TRUE,
    'width' => '20%',
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
    'data' => 'Journal Name',
    'header' => TRUE,
    'nowrap' => 'nowrap'
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
// allow site admins to see the pub ID
if (user_access('administer tripal')) {
  // Pub ID
  $rows[] = array(
    array(
      'data' => 'Pub ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $pub->pub_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
  );
}
// Is Obsolete Row
if($pub->is_obsolete == TRUE){
  $rows[] = array(
    array(
      'data' => '<div class="tripal_pub-obsolete">This publication is obsolete</div>',
      'colspan' => 2
    ),
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
if ($abstract_text) { ?>
  <p><b>Abstract</b></p>
  <div style="text-align: justify"><?php print $abstract_text; ?></div> <?php 
} 