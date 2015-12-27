<?php

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