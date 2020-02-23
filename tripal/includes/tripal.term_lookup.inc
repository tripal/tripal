<?php

/**
 *  Provides the content for the Controlled vocabulary home page.
 */
function tripal_vocabulary_lookup_page() {
  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  drupal_set_breadcrumb($breadcrumb);
  $vocabs = tripal_get_vocabularies();

  $rows = [];
  foreach ($vocabs as $vocabulary) {
    $rows[] = [
      l($vocabulary['short_name'], 'cv/lookup/' . $vocabulary['short_name']),
      $vocabulary['name'],
      $vocabulary['description'],
      [
        'data' => number_format($vocabulary['num_terms']),
        'style' => 'text-align: right;',
      ],
    ];
  }

  $headers = [
    'Short Name',
    'Vocabulary Name(s)',
    'Description',
    'Loaded Terms',
  ];
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => t('There are no controlled vocabularies'),
  ];

  $content = [
    'description' => [
      '#type' => 'markup',
      '#markup' => '<p>The following controlled vocabularies are used on this site. Click a vocabulary short name for more details.</p>',
    ],
    'vocab_table' => [
      '#type' => 'item',
      '#markup' => theme_table($table),
    ],
  ];
  return $content;
}

/**
 * Provides the content for a single controlled vocabulary.
 */
function tripal_vocabulary_lookup_vocab_page($vocabulary) {

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Controlled Vocabularies', 'cv/lookup');
  drupal_set_breadcrumb($breadcrumb);

  $vocab = tripal_get_vocabulary_details($vocabulary);
  $vocab_table = tripal_vocabulary_get_vocab_details($vocab);

  if ($vocab['description']) {
    drupal_set_title($vocabulary . ': ' . $vocab['description']);
  }
  else {
    drupal_set_title($vocabulary);
  }

  // If we can't find the term then just return a message.
  if (!$vocab) {
    drupal_set_message('The vocabulary cannot be found on this site', 'error');
    return '';
  }

  $has_root = TRUE;
  $root_terms = tripal_get_vocabulary_root_terms($vocabulary);
  // If this vocabulary doesn't have root terms then it's either not an
  // ontology or not all of the terms are loaded. In this case, let's get
  // a paged list of all terms
  if (count($root_terms) == 0) {
    $root_terms = tripal_get_vocabulary_terms($vocabulary, 25);
    $has_root = FALSE;
  }
  $items = tripal_vocabulary_lookup_term_children_format($root_terms);

  if (count($root_terms) == 0) {
    $items = '<p>This vocabulary has no terms loaded</p>';
  }
  else {
    $items = '<p>Click the + icon (if present) to expand the tree. If ' .
      'the full ontology or the term heirarchy is not loaded into this site, ' .
      'then the tree will consist of all terms at the same level. ' .
      'For some vocabularies, only a subset of terms are loaded</p>' . $items;
  }

  drupal_add_js([
    'tripal' => [
      'cv_lookup' => [
        'vocabulary' => $vocabulary,
      ],
    ],
  ], 'setting');

  $content = [
    'vocab_table' => [
      '#type' => 'item',
      '#title' => 'Details',
      '#markup' => '<p>A vocabulary is always identified by its short name ' .
        'and sometimes it may offer multiple sub-vocabularies with different ' .
        'names. Both are listed below.</p>' . $vocab_table,
    ],
    'vocab_browser' => [
      '#type' => 'item',
      '#title' => 'Term Browser',
      '#markup' => $items,
    ],
  ];
  if (!$has_root) {
    $content['pager'] = [
      '#type' => 'markup',
      '#markup' => theme('pager'),
    ];
  }

  // Add support for our custom tree viewer
  drupal_add_css(drupal_get_path('module', 'tripal') . '/theme/css/tripal.cv_lookup.css');
  drupal_add_js(drupal_get_path('module', 'tripal') . '/theme/js/tripal.cv_lookup.js', 'file');

  return $content;
}

/**
 * Generates a table view of the vocabulary.
 *
 * @param $vocab
 *   The vocabulary array.
 *
 * @return
 *    An HTML rendered table describing the vocabulary.
 */
function tripal_vocabulary_get_vocab_details($vocab) {
  $headers = [];
  $rows = [];
  $vocab_name = $vocab['name'];
  $short_name = $vocab['short_name'];
  if ($vocab['url']) {
    $short_name = l($vocab['short_name'], $vocab['url'], ['attributes' => ['target' => '_blank']]);
  }
  $vocab_desc = $vocab['description'];
  $rows[] = [
    [
      'data' => 'Short Name',
      'header' => TRUE,
      'width' => '20%',
    ],
    $short_name,
  ];
  $rows[] = [
    [
      'data' => 'Vocabulary Name(s)',
      'header' => TRUE,
      'width' => '20%',
    ],
    $vocab_name,
  ];
  $rows[] = [
    [
      'data' => 'Description',
      'header' => TRUE,
      'width' => '20%',
    ],
    $vocab_desc,
  ];
  $rows[] = [
    [
      'data' => 'Number of Terms Loaded on This Site',
      'header' => TRUE,
      'width' => '20%',
    ],
    number_format($vocab['num_terms']),
  ];

  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  return theme_table($table);
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
      'state = "closed" ' .
      'class = "cv-lookup-tree-node">';
    $class = 'tree-node-closed';
    if ($num_grand == 0) {
      $class = 'tree-node-single';
    }
    $items .= '<i class = "tree-node-icon ' . $class . '"></i>';
    $items .= l($child['name'], 'cv/lookup/' . $child['vocabulary']['short_name'] . '/' . $child['accession'], ['attributes' => ['target' => '_blank']]);
    if ($child['accession'] != $child['name']) {
      $items .= ' [' . $child['vocabulary']['short_name'] . ':' . $child['accession'] . '] ';
    }
    $items .= '</li>';
  }
  $items .= '</ul>';

  if (count($children) == 0) {
    $items = '';
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
  $response = [
    'vocabulary' => $vocabulary,
    'accession' => $accession,
    'content' => tripal_vocabulary_lookup_term_children_format($children),
  ];
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
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Controlled Vocabularies', 'cv/lookup');
  $breadcrumb[] = l($vocabulary, 'cv/lookup/' . $vocabulary);
  drupal_set_breadcrumb($breadcrumb);

  $vocab = tripal_get_vocabulary_details($vocabulary);
  $vocab_table = tripal_vocabulary_get_vocab_details($vocab);

  $term = tripal_get_term_details($vocabulary, $accession);

  // If we can't find the term then just return a message.
  if (!$term) {
    drupal_set_message('The term cannot be found on this site', 'error');
    return '';
  }

  // Build the Term table.
  $headers = [];
  $rows = [];
  $term_name = $term['name'];
  $accession = $term['vocabulary']['short_name'] . ':' . $term['accession'];
  drupal_set_title($accession);

  // Create a URL to point this term to it's source page, but only if the
  // source is not this site.
  if ($term['url'] and !preg_match('/cv\/lookup/', $term['url'])) {
    $accession = l($accession, $term['url'], ['attributes' => ['target' => '_blank']]);
  }
  $rows[] = [
    [
      'data' => 'Term',
      'header' => TRUE,
      'width' => '20%',
    ],
    $accession,
  ];
  $rows[] = [
    [
      'data' => 'Name',
      'header' => TRUE,
      'width' => '20%',
    ],
    $term_name,
  ];
  $rows[] = [
    [
      'data' => 'Definition',
      'header' => TRUE,
      'width' => '20%',
    ],
    $term['definition'],
  ];

  // Now iterate through any other columns in the term array and add those
  // details.
  foreach ($term as $key => $value) {
    if (in_array($key, [
      'name',
      'definition',
      'vocabulary',
      'accession',
      'url',
    ])) {
      continue;
    }
    // Convert thisto an array so we can alter it.
    if (!is_array($value)) {
      $new_values[] = $value;
      $value = $new_values;
    }
    // If this is a relationship key then let's try to rewrite the GO 
    // term in the relationship as a link.
    if ($key == 'relationship') {
      foreach ($value as $index => $v) {
        $matches = [];
        if (preg_match('/^(.+)\s(.+?):(.+?)$/', $v, $matches)) {
          $rel = $matches[1];
          $voc = $matches[2];
          $acc = $matches[3];
          $v = $rel . ' ' . l($voc . ':' . $acc, 'cv/lookup/' . $voc . '/' . $acc, ['attributes' => ['target' => '_blank']]);
          $t = tripal_get_term_details($voc, $acc);
          if ($t) {
            $v .= ' (' . $t['name'] . ')';
          }
          $value[$index] = $v;
        }
      }
    }
    if (count($value) > 1) {
      $value_str = theme_item_list([
        'items' => $value,
        'type' => 'ul',
        'attributes' => [],
        'title' => '',
      ]);
    }
    else {
      $value_str = $value[0];
    }
    $rows[] = [
      [
        'data' => ucfirst($key),
        'header' => TRUE,
        'width' => '20%',
      ],
      $value_str,
    ];
  }

  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  $content['cvterm'] = [
    '#type' => 'item',
    '#title' => 'Term Details',
    '#markup' => theme_table($table),
  ];

  $content['vocabulary'] = [
    '#type' => 'item',
    '#title' => 'Vocabulary Details',
    '#markup' => $vocab_table,
  ];

  drupal_add_js([
    'tripal' => [
      'cv_lookup' => [
        'vocabulary' => $vocabulary,
        'accession' => $accession,
      ],
    ],
  ], 'setting');

  return $content;
}