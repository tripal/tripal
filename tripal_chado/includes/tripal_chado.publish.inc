<?php

function tripal_chado_publish_form($form, &$form_state) {

  $term_id = '';
  if (array_key_exists('values', $form_state)) {
    $term_id = $form_state['values']['term_id'];
  }


  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->orderBy('label', 'ASC')
    ->execute();
  $term_ids = array();
  $term_ids[] = 'Select a Content Type';
  while ($bundle = $bundles->fetchObject()) {
    $term_ids[$bundle->term_id] = $bundle->label;
  }

  $form['term_id'] = array(
    '#type' => 'select',
    '#title' => 'Content Type',
    '#description' => t('Select a content type to publish.  Only data that
      is mapped to the selected vocabulary term will be published.'),
    '#options' => $term_ids,
    '#default_value' => $term_id,
    '#ajax' => array(
      'callback' => "tripal_chado_publish_form_ajax_callback",
      'wrapper' => "tripal-chado-publish-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  // If the user has selected a content type, then we need to
  // show some filters.
  if ($term_id) {
    $form['filters'] = array(
      '#type' => 'fieldset',
      '#title' => 'Filters',
      '#description' => t('Please provide any filters for limiting
          the records. Only those that match the filters specified
          below will be published.  To publish all records of this
          type, leave all filters blank.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );

    $form['publish_btn'] = array(
      '#type' => 'submit',
      '#name' => 'publish_btn',
      '#value' => 'Publish',
    );
  }


  $form['#prefix'] = '<div id="tripal-chado-publish-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

function tripal_chado_publish_form_validate($form, &$form_state) {

}

function tripal_chado_publish_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'publish_btn') {
    global $user;

    $term_id = $form_state['values']['term_id'];
    $bundle_name = 'bio-data_' . $term_id;
    $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
    $args = array(
      array('bundle_name' => $bundle_name),
    );
    $includes = array(
      module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.publish'),
    );
    return tripal_add_job("Publish " . $bundle->label . " records.",
      'tripal_chado', 'tripal_chado_publish_records', $args,
      $user->uid, 10, $includes);
  }
}
/**
 *
 */
function tripal_chado_publish_form_ajax_callback($form, $form_state) {
  return $form;
}

