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

    // Get the form for this bundle (content type).  Then iterate through
    // the fields and include only those that are Chado fields.
/*     $bundle_name = 'bio-data_' . $term_id;
    $entity = entity_get_controller('TripalEntity')->create(array('bundle' => $bundle_name, 'term_id' => $term_id));
    field_attach_form('TripalEntity', $entity, $form['filters'], $form_state);
    foreach (element_children($form['filters']) as $field_name) {
      $field = field_info_field($field_name);
      if ($field and $field['storage']['type'] != 'field_chado_storage') {
        unset($form['filters'][$field_name]);
      }
      // Ignore the KVPoperty field as this is just a field to
      // allow the user to add new properties which isn't appropriate
      // here.
      if ($field['type'] == 'kvproperty_adder') {
        unset($form['filters'][$field_name]);
      }
      // None of the fields should be required.

    } */

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
    $args = array($bundle_name);
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

/**
 *
 */
function tripal_chado_publish_records($bundle_name, $job_id = NULL) {
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
  $bundle_id = $bundle->id;
  $table = tripal_get_bundle_variable('chado_table', $bundle_id);
  $column = tripal_get_bundle_variable('chado_column', $bundle_id);
  $cvterm_id = tripal_get_bundle_variable('chado_cvterm_id', $bundle_id);

  // Get the table information
  $table_schema = chado_get_schema($table);
  $pkey_field = $table_schema['primary key'][0];

  $where = '';
  if ($table != 'analysis' and $table != 'organism') {
    $where .= "AND $column = $cvterm_id";
  }

  $sql = "
    SELECT $pkey_field as record_id
    FROM {" . $table . "} T
    LEFT JOIN public.chado_entity CE on CE.record_id = T.$pkey_field
    AND CE.data_table = '$table'
    WHERE CE.record_id IS NUll $where
  ";
  $records = chado_query($sql);
  $num_published = 0;
  try {
    while($record = $records->fetchObject()) {

      $record_id = $record->record_id;
      $ec = entity_get_controller('TripalEntity');
      $entity = $ec->create(array(
        'bundle' => $bundle_name,
        'term_id' => $bundle->term_id,
      ));
      $entity->save();

      $record = array(
        'entity_id' => $entity->id,
        'record_id' => $record_id,
        'data_table' => $table,
        'type_table' => $table,
        'field' => $column,
      );
      $success = drupal_write_record('chado_entity', $record);

      $entity = entity_load('TripalEntity', array($entity->id));
      $entity = reset($entity);
      $title_format = tripal_get_title_format($bundle);
      $title = tripal_replace_tokens($title_format, $entity, $bundle);
      $ec->setTitle($entity, $title);
      $num_published++;
    }
  }
  catch (Exception $e) {
    $error = $e->getMessage();
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not publish record: @error", array('@error' => $error));
    drupal_set_message('Failed publishing record. See recent logs for more details.', 'error');
    return FALSE;
  }
  drupal_set_message("Succesfully published $num_published " . $bundle->label . " record(s).");
}