<?php

/**
 *  Provides the content for the Controlled vocabulary home page.
 */
function tripal_vocabulary_lookup_page() {
  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  drupal_set_breadcrumb($breadcrumb);
  $vocabs = tripal_get_vocabularies();

  $rows = array();
  foreach ($vocabs as $vocabulary) {
    $rows[] = array(
      l($vocabulary['short_name'], 'cv/lookup/' . $vocabulary['short_name']),
      $vocabulary['name'],
      $vocabulary['description'],
    );
  }

  $headers = array('Short Name', 'Vocabulary Name(s)', 'Description');
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  $content = array(
    'description' => array(
      '#type' => 'markup',
      '#markup' => '<p>The following controlled vocabularies are used on this site. Click a vocabulary short name for more details.</p>',
    ),
    'vocab_table' => array(
      '#type' => 'item',
      '#markup' => theme_table($table),
    ),
  );
  return $content;
}

/**
 * Provides the content for a single controlled vocabulary.
 */
function tripal_vocabulary_lookup_vocab_page($vocabulary) {

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Controlled Vocabularies', 'cv/lookup');
  drupal_set_breadcrumb($breadcrumb);

  $vocab = tripal_get_vocabulary_details($vocabulary);
  if ($vocab['description']) {
    drupal_set_title($vocab['description']);
  }
  else {
    drupal_set_title($vocabulary);
  }

  // If we can't find the term then just return a message.
  if (!$vocab) {
    drupal_set_message('The vocabulary cannot be found on this site', 'error');
    return '';
  }

  $headers = array();
  $rows = array();
  $vocab_name = $vocab['name'];
  if ($vocab['url']) {
    $vocab_name = l($vocab['name'], $vocab['url'], array('attributes' => array('target' => '_blank')));
  }

  $short_name = $vocab['short_name'];
  $vocab_desc = $vocab['description'];
  $rows[] = array(
    array(
      'data' => 'Vocabulary Name(s)',
      'header' => TRUE,
      'width' => '20%',
    ),
    $vocab_name,
  );
  $rows[] = array(
    array(
      'data' => 'Short Name',
      'header' => TRUE,
      'width' => '20%',
    ),
    $short_name,
  );
  $rows[] = array(
    array(
      'data' => 'Description',
      'header' => TRUE,
      'width' => '20%',
    ),
    $vocab_desc,
  );

  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  $root_terms = tripal_get_vocabulary_root_terms($vocabulary);
  $items = tripal_vocabulary_lookup_term_children_format($root_terms);

  if (count($root_terms) == 0) {
    $items = '<p>Sometimes a vocabulary may not be fully loaded, but individual terms are. In this case the term browser is not available.</p>';
  }

  drupal_add_js(array(
    'tripal' => array(
      'cv_lookup' => array(
        'vocabulary' => $vocabulary,
      ),
    ),
  ), 'setting');

  $content = array(
    'vocab_table' => array(
      '#type' => 'item',
      '#title' => 'Details',
      '#markup' => theme_table($table),
    ),
    'vocab_browser' => array(
      '#type' => 'item',
      '#title' => 'Term Browser',
      '#markup' => '<p>Sometimes the full ontology may not be loaded into this site, but only terms needed by this site. The following allows browsing of loaded terms</p>' .
         $items,
    ),
  );

  // Add support for our custom tree viewer
  drupal_add_css(drupal_get_path('module', 'tripal') . '/theme/css/tripal.cv_lookup.css');
  drupal_add_js(drupal_get_path('module', 'tripal') . '/theme/js/tripal.cv_lookup.js', 'file');

  return $content;
}

/**
 * A helper function to format an array of terms into a list for the web page.
 *
 * @param $children
 *   A list of children terms.
 */
function tripal_vocabulary_lookup_term_children_format($children) {
  $items = '<ul id="tripal-cv-lookup-tree">';
  foreach ($children as $child) {
    $grand = tripal_get_term_children($child['vocabulary']['short_name'], $child['accession']);
    $num_grand = count($grand);
    $items .= '<li vocabulary = "' . $child['vocabulary']['short_name'] . '" ' .
        'accession = "' . $child['accession'] . '" ' .
        'children = "' . $num_grand . '" ' .
        'state = "closed" '.
        'class = "cv-lookup-tree-node">';
    $class = 'tree-node-closed';
    if ($num_grand == 0) {
      $class = 'tree-node-single';
    }
    $items .= '<i class = "tree-node-icon ' . $class . '"></i>';
    $items .= l($child['name'], 'cv/lookup/' . $child['vocabulary']['short_name'] . '/' . $child['accession'], array('attributes' => array('target' => '_blank'))) . ' (' . $num_grand . ')';
    $items .= '</li>';
  }
  $items .= '</ul>';

  if (count($children)== 0) {
    $items ='';
  }

  return $items;
}

/**
 * An ajax callback to get the children of a term.
 *
 * @param $vocabulary
 *   The short name of the vocabulary (e.g. SO, GO, etc.)
 * @param $accession
 *   The term accession.
 *
 * @return
 *   A JSON array compatible with the JSTree library.
 *   https://www.jstree.com/docs/json/
 */
function tripal_vocabulary_lookup_term_children_ajax($vocabulary, $accession) {

  $term = tripal_get_term_details($vocabulary, $accession);
  $children = tripal_get_term_children($vocabulary, $accession);
  $response = array(
    'vocabulary' => $vocabulary,
    'accession' => $accession,
    'content' => tripal_vocabulary_lookup_term_children_format($children)
  );
  drupal_json_output($response);
}

/**
 *
 * @param $vocabulary
 * @param $accession
 *
 * @return
 */
function tripal_vocabulary_lookup_term_page($vocabulary, $accession) {

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Controlled Vocabularies', 'cv/lookup');
  $breadcrumb[] = l($vocabulary, 'cv/lookup/' . $vocabulary);
  drupal_set_breadcrumb($breadcrumb);

  $term = tripal_get_term_details($vocabulary, $accession);
  drupal_set_title($term['name']);


  // If we can't find the term then just return a message.
  if (!$term) {
    drupal_set_message('The term cannot be found on this site', 'error');
    return '';
  }

  // Build the Term table.
  $headers = array();
  $rows = array();
  $term_name = $term['name'];
  $accession = $term['vocabulary']['short_name'] . ':' . $term['accession'];
  if ($term['url']) {
    $term_name = l($term['name'], $term['url'], array('attributes' => array('target' => '_blank')));
  }
  $rows[] = array(
    array(
      'data' => 'Term',
      'header' => TRUE,
      'width' => '20%',
    ),
    $accession,
  );
  $rows[] = array(
    array(
      'data' => 'Name',
      'header' => TRUE,
      'width' => '20%',
    ),
    $term_name,
  );
  $rows[] = array(
    array(
      'data' => 'Definition',
      'header' => TRUE,
      'width' => '20%',
    ),
    $term['definition'],
  );


  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => FALSE,
    'caption' => 'Term Details',
    'colgroups' => array(),
    'empty' => '',
  );
  $content = theme_table($table);


  $rows = array();
  $vocab_name = $term['vocabulary']['name'];
  if ($term['vocabulary']['url']) {
    $vocab_name = l($term['vocabulary']['name'], $term['vocabulary']['url'], array('attributes' => array('target' => '_blank')));
  }
  $short_name = $term['vocabulary']['short_name'];
  $vocab_desc = $term['vocabulary']['description'];
  $rows[] = array(
    array(
      'data' => 'Name',
      'header' => TRUE,
      'width' => '20%',
    ),
    $vocab_name,
  );
  $rows[] = array(
    array(
      'data' => 'Short Name',
      'header' => TRUE,
      'width' => '20%',
    ),
    $short_name,
  );
  $rows[] = array(
    array(
      'data' => 'Description',
      'header' => TRUE,
      'width' => '20%',
    ),
    $vocab_desc,
  );

  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => FALSE,
    'caption' => 'Term Vocabulary details',
    'colgroups' => array(),
    'empty' => '',
  );
  $content .=  theme_table($table);

  drupal_add_js(array(
    'tripal' => array(
      'cv_lookup' => array(
        'vocabulary' => $vocabulary,
        'accession' => $accession,
      ),
    ),
  ), 'setting');

  return $content;
}