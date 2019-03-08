<?php
/**
 * Implements hook_vocab_storage_info().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_storage_info() {
  return [
    'term_chado_storage' => [
      'label' => t('Chado'),
      'module' => 'tripal_chado',
      'description' => t('Integrates terms stored in the local Chado database
          with Tripal entities.'),
      'settings' => [],
    ],
  ];
}

/**
 * Implements hook_vocab_get_vocabularies().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_vocabularies() {
  $vocabs = [];

  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cv')) {
    return FALSE;
  }

  // Make sure the materiailzd view is present.
  if (!chado_table_exists('db2cv_mview')) {
    drupal_set_message('Please update the database using "drush updatedb" before continuing');
    return FALSE;
  }

  $sql = "
     SELECT DB.name as short_name, DB.description, DB.url, DB.urlprefix,
       SUM(DBCVM.num_terms) as num_terms,
       array_to_string(array_agg(DBCVM.cvname), ', ') as name
     FROM {db} DB
      INNER JOIN {db2cv_mview} DBCVM ON DBCVM.db_id = DB.db_id
     GROUP BY DB.name, DB.description, DB.url, DB.urlprefix
     ORDER BY DB.name
  ";

  $results = chado_query($sql, []);
  while ($result = $results->fetchAssoc()) {
    if (!$result['name']) {
      $result['name'] = $result['short_name'];
    }
    $sw_url = $result['urlprefix'];
    if ($sw_url) {
      $sw_url = preg_replace('/\{db\}/', $result['short_name'], $sw_url);
      $sw_url = preg_replace('/\{accession\}/', '', $sw_url);
      $sw_url = url($sw_url, ['absolute' => TRUE]);
    }
    $result['sw_url'] = $sw_url;
    $vocabs[] = $result;
  }

  if (count($vocabs) == 0) {
    $link = l('populating', 'admin/tripal/storage/chado/mviews/update', ['attributes' => ['target' => '_blank']]);
    $message = t('If there are no controlled vocabularies it is mostly likely because the db2cv_mview is not populated.  Try !populating this mview.', ['!populating' => $link]);
    tripal_set_message($message, TRIPAL_NOTICE);
  }
  return $vocabs;
}

/**
 * Implements hook_vocab_get_vocabulary().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_vocabulary($vocabulary) {
  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cv')) {
    return FALSE;
  }

  // Make sure the materiailzd view is present.
  if (!chado_table_exists('db2cv_mview')) {
    drupal_set_message('Please update the database using "drush updatedb" before continuing');
    return FALSE;
  }

  $sql = "
     SELECT DB.name as short_name, DB.description, DB.url, DB.urlprefix,
       SUM(DBCVM.num_terms) as num_terms,
       array_to_string(array_agg(DBCVM.cvname), ', ') as name
     FROM {db} DB
      INNER JOIN {db2cv_mview} DBCVM ON DBCVM.db_id = DB.db_id
     WHERE DB.name = :name
     GROUP BY DB.name, DB.description, DB.url, DB.urlprefix
  ";
  $result = chado_query($sql, [':name' => $vocabulary]);
  $result = $result->fetchAssoc();
  if (!$result) {
    drupal_set_message("Could not find details about the vocabulary: $vocabulary. Note: " .
      "if this vocabulary does exist, try re-populating the db2cv_mview materialized " .
      "view at Admin > Tripal > Data Storage > Chado > Materialized views.");
    return FALSE;
  }

  if (!$result['name']) {
    $result['name'] = $result['short_name'];
  }
  $sw_url = $result['urlprefix'];
  if ($sw_url) {
    $sw_url = preg_replace('/\{db\}/', $result['short_name'], $sw_url);
    $sw_url = preg_replace('/\{accession\}/', '', $sw_url);
    $sw_url = url($sw_url, ['absolute' => TRUE]);
  }
  $result['sw_url'] = $sw_url;
  return $result;
}


/**
 * Implements hook_vocab_get_root_terms().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_root_terms($vocabulary) {
  $terms = [];


  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('db')) {
    return FALSE;
  }

  // Get the list of CV's that belong to this vocabulary and get their
  // roots.
  $sql = "
    SELECT *
    FROM {db2cv_mview} WHERE dbname = :dbname
  ";
  $cvs = chado_query($sql, [':dbname' => $vocabulary]);
  while ($cv = $cvs->fetchObject()) {
    $sql = "
      SELECT cvterm_id
      FROM {cv_root_mview} CRM
      WHERE CRM.cv_id = :cv_id
    ";
    $results = chado_query($sql, [':cv_id' => $cv->cv_id]);
    while ($cvterm_id = $results->fetchField()) {
      $match = ['cvterm_id' => $cvterm_id];
      $cvterm = chado_generate_var('cvterm', $match);
      $terms[] = _tripal_chado_format_term_description($cvterm);
    }
  }
  return $terms;
}

/**
 * Implements hook_vocab_get_terms().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_terms($vocabulary, $limit = 25, $element = 0) {
  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cvterm')) {
    return FALSE;
  }

  $sql = "
    SELECT CVT.cvterm_id
    FROM {cvterm} CVT
      INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
      INNER JOIN {db} DB on DB.db_id = DBX.db_id
    WHERE db.name = :dbname
    ORDER BY CVT.name
  ";
  $csql = "
    SELECT COUNT(CVT.cvterm_id)
    FROM {cvterm} CVT
      INNER JOIN {dbxref} DBX on DBX.dbxref_id = CVT.dbxref_id
      INNER JOIN {db} DB on DB.db_id = DBX.db_id
    WHERE db.name = :dbname
  ";
  $results = chado_pager_query($sql, [':dbname' => $vocabulary], $limit, $element, $csql);

  $terms = [];
  while ($cvterm_id = $results->fetchField()) {
    $match = ['cvterm_id' => $cvterm_id];
    $cvterm = chado_generate_var('cvterm', $match);
    $terms[] = _tripal_chado_format_term_description($cvterm);
  }
  return $terms;
}

/**
 * Implements hook_vocab_get_term_children().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_term_children($vocabulary, $accession) {
  $terms = [];

  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cvtermpath')) {
    return FALSE;
  }

  // Get the parent cvterm.
  $match = [
    'dbxref_id' => [
      'db_id' => [
        'name' => $vocabulary,
      ],
      'accession' => $accession,
    ],
  ];
  $cvterm = chado_generate_var('cvterm', $match);
  if (!$cvterm) {
    return FALSE;
  }
  $cvterm = chado_expand_var($cvterm, 'field', 'cvterm.definition');


  // Get the children.
  $sql = "
    SELECT DISTINCT CVTR.subject_id, CVT.name
    FROM {cvterm_relationship} CVTR
      INNER JOIN {cvterm} CVT on CVTR.subject_id = CVT.cvterm_id
    WHERE object_id = :object_id
    ORDER BY CVT.name
  ";
  $results = chado_query($sql, [':object_id' => $cvterm->cvterm_id]);
  while ($cvterm = $results->fetchObject()) {
    $cvterm_id = $cvterm->subject_id;
    $match = ['cvterm_id' => $cvterm_id];
    $cvterm = chado_generate_var('cvterm', $match);
    $terms[] = _tripal_chado_format_term_description($cvterm);
  }
  return $terms;
}

/**
 * Implements hook_vocab_get_term().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_term($vocabulary, $accession) {
  // Cache ID for this term:
  $cid = 'tripal_chado:term:' . $vocabulary . ':' . $accession;

  // Check the cache first. Get the term from cache if it's available
  $cache = cache_get($cid, 'cache');
  if (isset($cache->data)) {
    return $cache->data;
  }
  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cvterm')) {
    return FALSE;
  }

  $match = [
    'dbxref_id' => [
      'db_id' => [
        'name' => $vocabulary,
      ],
      'accession' => $accession,
    ],
  ];
  $cvterm = chado_generate_var('cvterm', $match);
  if (!$cvterm) {
    return FALSE;
  }
  $options = ['return_array' => TRUE];
  $cvterm = chado_expand_var($cvterm, 'field', 'cvterm.definition', $options);
  $cvterm = chado_expand_var($cvterm, 'table', 'cvterm_dbxref', $options);
  $cvterm = chado_expand_var($cvterm, 'table', 'cvtermsynonym', $options);
  $cvterm = chado_expand_var($cvterm, 'table', 'cvterm_relationship', $options);
  $cvterm = chado_expand_var($cvterm, 'table', 'cvtermprop', $options);

  // Cache the term to reduce the amount of queries sent to the database
  $term = _tripal_chado_format_term_description($cvterm);
  cache_set($cid, $term, 'cache', CACHE_TEMPORARY);
  return $term;
}

/**
 * A helper functions for the hook_vocab_xxx functions.
 *
 * @param $cvterm
 *   A cvterm object.
 */
function _tripal_chado_format_term_description($cvterm) {
  $url = $cvterm->dbxref_id->db_id->url;
  $urlprefix = $cvterm->dbxref_id->db_id->urlprefix;

  // Generate the URL that can be used for semantic web applications.
  $sw_url = $urlprefix;
  if ($sw_url) {
    $sw_url = preg_replace('/{db}/', $cvterm->dbxref_id->db_id->name, $sw_url);
    $sw_url = preg_replace('/{accession}/', '', $sw_url);
    $sw_url = url($sw_url, ['absolute' => TRUE]);
  }
  $vocabulary = tripal_chado_vocab_get_vocabulary($cvterm->dbxref_id->db_id->name);
  $term = [
    'vocabulary' => $vocabulary,
    'namespace' => $cvterm->cv_id->name,
    'accession' => $cvterm->dbxref_id->accession,
    'name' => $cvterm->name,
    'url' => chado_get_dbxref_url($cvterm->dbxref_id),
    'definition' => (isset($cvterm->definition)) ? $cvterm->definition : '',
  ];

  // Is this term in any relationships as the subject?
  if (property_exists($cvterm, 'cvterm_relationship')) {
    $relationships = $cvterm->cvterm_relationship->subject_id;
    if ($relationships) {
      foreach ($relationships as $relationship) {
        $term['relationship'][] = $relationship->type_id->name . ' ' . $relationship->object_id->dbxref_id->db_id->name . ':' . $relationship->object_id->dbxref_id->accession;
      }
    }
  }

  // Does this term have synonyms?
  if (property_exists($cvterm, 'cvtermsynonym')) {
    $synonyms = $cvterm->cvtermsynonym->cvterm_id;
    if ($synonyms) {
      foreach ($synonyms as $synonym) {
        $term['synonym'][] = $synonym->synonym;
      }
    }
  }

  // Does this term have any cross refs?
  if (property_exists($cvterm, 'cvterm_dbxref')) {
    $xrefs = $cvterm->cvterm_dbxref;
    if ($xrefs) {
      foreach ($xrefs as $xref) {
        $term['cross reference'][] = $xref->dbxref_id->db_id->name . ':' . $xref->dbxref_id->accession;
      }
    }
  }

  // Does this term have any properties?
  if (property_exists($cvterm, 'cvtermprop')) {
    $props = $cvterm->cvtermprop->cvterm_id;
    if ($props) {
      foreach ($props as $prop) {
        $term[$prop->type_id->name][] = property_exists($prop, 'value') ? $prop->value : '';
      }
    }
  }
  return $term;
}

/**
 * Implements hook_vocab_add_term().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_add_term($details) {
  $vocabulary = $details['vocab']['name'];
  $accession = $details['accession'];

  // First check to make sure the term doesn't already exist
  $term = tripal_chado_vocab_get_term($vocabulary, $accession);
  if ($term) {
    return TRUE;
  }

  // First make sure the vocabulary is added.
  $values = [
    'name' => $vocabulary,
    'description' => $details['vocab']['description'],
    'url' => $details['vocab']['url'],
    // TODO: deal with the URL prefix
  ];
  $options = ['update_existing' => TRUE];
  chado_insert_db($values, $options);


  // Second make sure the term is added.
  $term = chado_insert_cvterm([
    'id' => $vocabulary . ':' . $accession,
    'name' => $details['name'],
    'definition' => $details['definition'],
    'cv_name' => $details['vocab']['name'],
  ]);

  // Return TRUE on success.
  if (!$term) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Implements hook_vocab_import_form();
 */
function tripal_chado_vocab_import_form($form, &$form_state) {
  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader');
  return tripal_cv_obo_form($form, $form_state);
}

/**
 * Implements hook_vocab_import_form_validate();
 */
function tripal_chado_vocab_import_form_validate($form, &$form_state) {
  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader');
  return tripal_cv_obo_form_validate($form, $form_state);
}

/**
 * Implements hook_vocab_import_form_submit();
 */
function tripal_chado_vocab_import_form_submit($form, &$form_state) {
  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.obo_loader');
  return tripal_cv_obo_form_submit($form, $form_state);
}
