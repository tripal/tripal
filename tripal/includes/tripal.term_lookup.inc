<?php

/**
 *
 * @param $form
 * @param string $form_state
 */
function tripal_vocabulary_lookup_form($form, &$form_state = NULL) {

}
/**
 *
 * @param $form
 * @param $form_state
 */
function tripal_vocabulary_lookup_form_validate($form, $form_state) {

}
/**
 *
 * @param $form
 * @param $form_state
 */
function tripal_vocabulary_lookup_form_submit($form, $form_state) {

}

/**
 *
 * @param $vocabulary
 * @param $accession
 *
 * @return
 */
function tripal_vocabulary_lookup_term_page($vocabulary, $accession) {
  $term = tripal_get_term_details($vocabulary, $accession);

  // If we can't find the term then just return a message.
  if (!$term) {
    drupal_set_message('The term cannot be found on this site', 'error');
    return '';
  }

  // Build the Term table.
  $headers = array();
  $rows = array();
  $term_name = $term['name'];
  if ($term['url']) {
    $term_name = l($term['name'], $term['url'], array('attributes' => array('target' => '_blank')));
  }
  $vocab_name = $term['vocabulary']['name'];
  if ($term['vocabulary']['url']) {
    $vocab_name = l($term['vocabulary']['name'], $term['vocabulary']['url'], array('attributes' => array('target' => '_blank')));
  }
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
      'data' => 'Vocabulary',
      'header' => TRUE,
      'width' => '20%',
    ),
    $vocab_name . ': ' . $term['vocabulary']['description']. '',
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
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  $content = theme_table($table);
  return $content;
}