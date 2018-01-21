<?php
/**
 * Implements hook_vocab_storage_info().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_storage_info() {
  return array(
    'term_chado_storage' => array(
      'label' => t('Chado'),
      'module' => 'tripal_chado',
      'description' => t('Integrates terms stored in the local Chado database
          with Tripal entities.'),
      'settings' => array(),
    ),
  );
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

  if (chado_table_exists('db2cv_mview')) {
    $sql = "
       SELECT DB.name as short_name, DB.description, DB.url, DB.urlprefix,
         array_to_string(array_agg(DBCVM.cvname), ', ') as name
       FROM {db} DB
        INNER JOIN {db2cv_mview} DBCVM ON DBCVM.db_id = DB.db_id
       WHERE DB.name = :name
       GROUP BY DB.name, DB.description, DB.url, DB.urlprefix
    ";
  }
  // For backwards compatibility.  Tripal sites above 3.0-beta2 don't need this.
  else {
    $sql = "
     SELECT DB.name as short_name, CV.name as name, DB.description, DB.url, DB.urlprefix
     FROM {db} DB
      LEFT JOIN {dbxref} DBX on DBX.db_id = DB.db_id
      LEFT JOIN {cvterm} CVT on CVT.dbxref_id = DBX.dbxref_id
      LEFT JOIN {cv} CV on CV.cv_id = CVT.cv_id
     WHERE
      DB.name = :name
     LIMIT 1 OFFSET 0
  ";
  }
  $result = chado_query($sql, array(':name' => $vocabulary));
  $result = $result->fetchAssoc();

  if (!$result['name']) {
    $result['name'] = $result['short_name'];
  }
  $sw_url = $result['urlprefix'];
  if ($sw_url) {
    $sw_url = preg_replace('/\{db\}/', $result['short_name'], $sw_url);
    $sw_url = preg_replace('/\{accession\}/', '', $sw_url);
    $sw_url = url($sw_url, array('absolute' => TRUE));
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
  $terms = array();


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
  $cvs = chado_query($sql, array(':dbname' => $vocabulary));
  while ($cv = $cvs->fetchObject()) {
    $sql = "
      SELECT cvterm_id
      FROM {cv_root_mview} CRM
      WHERE CRM.cv_id = :cv_id
    ";
    $results = chado_query($sql, array(':cv_id' => $cv->cv_id));
    while($cvterm_id = $results->fetchField()) {
      $match = array('cvterm_id' => $cvterm_id);
      $cvterm = chado_generate_var('cvterm', $match);
      $terms[] = _tripal_chado_format_term_description($cvterm);
    }
  }
  return $terms;
}
/**
 * Implements hook_vocab_get_term_children().
 *
 * This hook is created by the Tripal module and is not a Drupal hook.
 */
function tripal_chado_vocab_get_term_children($vocabulary, $accession) {
  $terms = array();

  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cvtermpath')) {
    return FALSE;
  }

  // Get the parent cvterm.
  $match = array(
    'dbxref_id' => array(
      'db_id' => array(
        'name' => $vocabulary,
      ),
      'accession' => $accession,
    ),
  );
  $cvterm = chado_generate_var('cvterm', $match);
  if (!$cvterm) {
    return FALSE;
  }
  $cvterm = chado_expand_var($cvterm, 'field', 'cvterm.definition');


  // Get the children
  $sql = "
    SELECT subject_id
    FROM {cvterm_relationship} CVTR
    WHERE object_id = :object_id
  ";
  $results = chado_query($sql, array(':object_id' => $cvterm->cvterm_id));
  while($cvterm_id = $results->fetchField()) {
    $match = array('cvterm_id' => $cvterm_id);
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

  // It's possible that Chado is not available (i.e. it gets renamed
  // for copying) but Tripal has already been prepared and the
  // entities exist.  If this is the case we don't want to run the
  // commands below.
  if (!chado_table_exists('cvterm')) {
    return FALSE;
  }

  $match = array(
    'dbxref_id' => array(
      'db_id' => array(
        'name' => $vocabulary,
      ),
      'accession' => $accession,
    ),
  );
  $cvterm = chado_generate_var('cvterm', $match);
  if (!$cvterm) {
    return FALSE;
  }
  $cvterm = chado_expand_var($cvterm, 'field', 'cvterm.definition');

  return _tripal_chado_format_term_description($cvterm);
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
    $sw_url = url($sw_url, array('absolute' => TRUE));
  }

  $term = array(
    'vocabulary' => array(
      'name' => $cvterm->cv_id->name,
      'short_name' => $cvterm->dbxref_id->db_id->name,
      'description' =>  $cvterm->dbxref_id->db_id->description,
      'url' => $url,
      'urlprefix' => $urlprefix,
      'sw_url' => $sw_url,
    ),
    'accession'  => $cvterm->dbxref_id->accession,
    'name'       => $cvterm->name,
    'url'        => tripal_get_dbxref_url($cvterm->dbxref_id),
    'definition' => (isset($cvterm->definition)) ? $cvterm->definition : '',
  );
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
  $values = array(
    'name' => $vocabulary,
    'description' => $details['vocab']['description'],
    'url' => $details['vocab']['url'],
    // TODO: deal with the URL prefix
  );
  $options = array('update_existing' => TRUE);
  tripal_insert_db($values, $options);


  // Second make sure the term is added.
  $term = tripal_insert_cvterm(array(
    'id' => $vocabulary . ':' . $accession,
    'name' => $details['name'],
    'definition' => $details['definition'],
    'cv_name' => $details['vocab']['name'],
  ));

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