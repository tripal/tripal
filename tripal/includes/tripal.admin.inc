<?php

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_admin_data_collection_form($form, &$form_state) {
  $form = [];

  $options = [0 => t('Disabled'), 1 => t('Enabled')];
  $form['enabled'] = [
    '#type' => 'radios',
    '#title' => t('Activate Data Collections'),
    '#options' => $options,
    '#description' => t('Data collections allow users to save results of search queries for later download or use by other tools.'),
    '#default_value' => variable_get('tripal_data_collections_enabled', 1),
    '#required' => TRUE,
  ];

  $form['lifespan'] = [
    '#type' => 'textfield',
    '#title' => t('Collection Lifespan'),
    '#description' => t('Enter the number of days that data collections exist.  
       Collections will be automatically removed after the lifespan
       period has passed.  Removal of data collections occurs when the
       sites Drupal cron executes.'),
    '#default_value' => variable_get('tripal_data_collections_lifespan', 30),
    '#required' => TRUE,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save changes'),
  ];

  return $form;
}

/**
 * Validate function for the tripal_admin_data_collection_form form.
 */
function tripal_admin_data_collection_form_validate($form, &$form_state) {
  $enable = $form_state['values']['enabled'];
  $lifespan = $form_state['values']['lifespan'];

  if (!preg_match('/^\d+$/', $lifespan)) {
    form_set_error('filespan', 'Please provide a valid numeric value for the number of hours collections exist.');
  }
  else {
    if ($lifespan == 0) {
      form_set_error('filespan', 'Please provide a positive numeric value for the number of hours collections exist.');
    }
  }
}

/**
 * Sumbit function for the tripal_admin_data_collection_form form.
 */
function tripal_admin_data_collection_form_submit($form, &$form_state) {
  $enabled = $form_state['values']['enabled'];
  $lifespan = $form_state['values']['lifespan'];
  variable_set('tripal_data_collections_enabled', $enabled);
  variable_set('tripal_data_collections_lifespan', $lifespan);
  menu_cache_clear_all();
  drupal_set_message('Settings saved.');
}

/**
 * Provides a form for importing vocabularies and their terms.
 *
 * Tripal allows for vocabularies to be stored separately from the biological
 * data. By default, the Tripal Chado module uses Chado for both the data
 * storage backend and for storing controlled vocabularies. This is a wrapper
 * function that calls the hook_vocab_import_form() hook function that allows
 * the term storage backend to provide an approprite import form.
 *
 * @param $form
 * @param $form_state
 *
 */
function tripal_vocabulary_import_form($form, &$form_state) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_import_form';
    if (function_exists($function)) {
      $form = $function($form, $form_state);
    }
    else {
      drupal_set_message("The function '$function' is not implemented. Cannot import vocabularies.", 'error');
    }
  }
  else {
    tripal_set_message('A storage backend is not enabled for managing
          the vocabulary terms used to create content.  Please enable
          a module that supports storage of vocabualary terms (e.g. tripal_chado)
          and return to create new Tripal content types.', TRIPAL_NOTICE);
  }
  return $form;
}

/**
 *
 * @param $form
 * @param $form_state
 */
function tripal_vocabulary_import_form_validate($form, &$form_state) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_import_form_validate';
    if (function_exists($function)) {
      $form = $function($form, $form_state);
    }
    else {
      drupal_set_message("The function '$function' is not implemented. Cannot import vocabularies.", 'error');
    }
  }
  else {
    tripal_set_message('A storage backend is not enabled for managing
      the vocabulary terms used to create content.  Please enable
      a module that supports storage of vocabualary terms (e.g. tripal_chado)
      and return to create new Tripal content types.', TRIPAL_NOTICE);
  }
  return $form;
}

/**
 *
 * @param $form
 * @param $form_state
 */
function tripal_vocabulary_import_form_submit($form, &$form_state) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_import_form_submit';
    if (function_exists($function)) {
      $form = $function($form, $form_state);
    }
    else {
      drupal_set_message("The function '$function' is not implemented. Cannot import vocabularies.", 'error');
    }
  }
  else {
    tripal_set_message('A storage backend is not enabled for managing
      the vocabulary terms used to create content.  Please enable
      a module that supports storage of vocabualary terms (e.g. tripal_chado)
      and return to create new Tripal content types.', TRIPAL_NOTICE);
  }
  return $form;
}