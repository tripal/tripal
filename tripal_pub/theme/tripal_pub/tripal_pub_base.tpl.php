<?php
$node = $variables['node'];
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

// get the author list
$values = array(
  'pub_id' => $pub->pub_id, 
  'type_id' => array(
    'name' => 'Authors',
  ),
);
$authors = tripal_core_generate_chado_var('pubprop', $values); 
$authors = tripal_core_expand_chado_vars($authors, 'field', 'pubprop.value');

// get the first database cross-reference with a url
$options = array('return_array' => 1);
$pub = tripal_core_expand_chado_vars($pub, 'table', 'pub_dbxref', $options);
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
$url = $urls[0]->value;

?>
<div id="tripal_pub-base-box" class="tripal_pub-info-box tripal-info-box">
  <div class="tripal_pub-info-box-title tripal-info-box-title"><?php print $pub->type_id->name ?> Details</div>
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
  else { ?>
    <table id="tripal_pub-table-base" class="tripal_pub-table tripal-table tripal-table-vert">
      <tr class="tripal_pub-table-even-row tripal-table-even-row">
        <th>Title</th>
        <td><?php
          if ($url) {
            print l(htmlspecialchars($pub->title), $url, array('attributes' => array('target' => '_blank')));          
          }
          elseif ($dbxref->db_id->urlprefix) { 
            print l(htmlspecialchars($pub->title), $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => '_blank')));             
          } 
          else {
            print htmlspecialchars($pub->title); 
          }?>
        </td>
      </tr>
      <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
        <th>Authors</th>
        <td><?php print $authors->value ? $authors->value : 'N/A'; ?></td>
      </tr>
      <tr class="tripal_pub-table-even-row tripal-table-even-row">
        <th>Type</th>
        <td><?php print $pub->type_id->name; ?></td>
      </tr>
      <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
        <th nowrap>Media Title</th>
        <td><?php print $pub->series_name; ?></td>
      </tr>
      <tr class="tripal_pub-table-even-row tripal-table-even-row">
        <th>Volume</th>
        <td><?php print $pub->volume ? $pub->volume : 'N/A'; ?></td>
      </tr>
      <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
        <th>Issue</th>
        <td><?php print $pub->issue ? $pub->issue : 'N/A'; ?></td>
      </tr>
      <tr class="tripal_pub-table-even-row tripal-table-even-row">    
        <th>Year</th>
        <td><?php print $pub->pyear; ?></td>
      </tr>
      <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
        <th>Page(s)</th>
        <td><?php print $pub->pages ? $pub->pages : 'N/A'; ?></td>
      </tr>
      <tr class="tripal_pub-table-even-row tripal-table-even-row">
        <th>Citation</th>
        <td><?php print htmlspecialchars($citation->value); ?></td>
      </tr>
      <tr class="tripal_pub-table-odd-row tripal-table-odd-row">
        <th>Abstract</th>
        <td style="text-align:justify;"><?php print htmlspecialchars($abstract->value) ? $abstract->value : 'N/A'; ?></td>
      </tr>
    </table> <?php
  } ?>
</div>