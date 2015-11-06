<?php

/**
 *
 */
function tripal_entities_add_page() {
  $item = menu_get_item();

  $content = system_admin_menu_block($item);

  // Bypass the node/add listing if only one content type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('tripal_entities_add_list', array('content' => $content));

}

/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_tripal_entities_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any biological types yet. Go to the <a href="@create-content">content type creation page</a> to add a new content type.', array('@create-content' => url('admin/structure/BioData/add'))) . '</p>';
  }
  return $output;
}

/**
 *
 */
function tripal_entities_entity_form($form, &$form_state, $dbxref_id = '', $entity = NULL) {

  $bundle_id = 'dbxref_' . $dbxref_id;

  // Add a vertical tabs element
  $form['ev_tabs'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 999,
  );

  // If the entity doesn't exist then create one.
  if (!$entity) {
    $entity = entity_get_controller('BioData')->create(array('bundle' => $bundle_id));
    field_attach_form('BioData', $entity, $form, $form_state);

    $form['add_button'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#name' => 'add_data',
      '#weight' => 1000
    );
  }
  else {
    field_attach_form('BioData', $entity, $form, $form_state);
    $form['update_button'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#name' => 'update_data',
      '#weight' => 1000
    );
    $form['delete_button'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#name' => 'delete_data',
      '#weight' => 1001
    );
  }

  // The entity object must be added to the $form_state in order for
  // the Entity API to work. It must have a key of the entity name.
  $form_state['BioData'] = $entity;

  $form['#prefix'] = "<div id='$bundle_id-entity-form'>";
  $form['#suffix'] = "</div>";

  return $form;

}
/**
 * An Ajax callback for the tripal_entities_entity_form.
 */
function tripal_entities_entity_form_ajax_callback($form, $form_state) {
  // return the form so Drupal can update the content on the page
  return $form;
}

/**
 * Implements hook_validate() for the tripal_entities_entity_form.
 */
function tripal_entities_entity_form_validate($form, &$form_state) {

  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='add_data') {
    $entity = $form_state['BioData'];
    field_attach_form_validate('BioData', $entity, $form, $form_state);
  }
}


/**
 * Implements hook_submit() for the tripal_entities_entity_form.
 */
function tripal_entities_entity_form_submit($form, &$form_state) {
  $entity = $form_state['BioData'];

  if ($form_state['clicked_button']['#name'] =='cancel') {
    $form_state['redirect'] = "BioData/" . $entity->id;
  }

  if ($form_state['clicked_button']['#name'] =='update_data' or
      $form_state['clicked_button']['#name'] =='add_data') {

    $entityform = entity_ui_controller('BioData')->entityFormSubmitBuildEntity($form, $form_state);
    if ($entityform->save()) {
      $form_state['redirect'] = "BioData/" . $entity->id;
    }
    else {
      drupal_set_message('Cannot save entity', 'error');
    }
  }
  if ($form_state['clicked_button']['#name'] =='delete_data') {
    $form_state['redirect'] = 'BioData/' . $entity->id .'/delete';
  }
}

/**
 * Form callback: confirmation form for deleting a tripal_entity.
 *
 * @param $tripal_entity The
 *          tripal_entity to delete
 *
 * @see confirm_form()
 */
function tripal_entities_entity_delete_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;
  $form['#submit'][] = 'tripal_entities_entity_delete_form_submit';

  $form = confirm_form($form,
      t('Click the delete button below to confirm deletion of the record titled: %title',
          array('%title' => $entity->title)), 'admin/content/tripal_entity',
      '<p>' .t('This action cannot be undone.') .'</p>', t('Delete'), t('Cancel'), 'confirm');

  return $form;
}

/**
 * Submit callback for tripal_entity_delete_form
 */
function tripal_entities_entity_delete_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];

  $entity_controller = new TripalEntityController($entity->type);
  if ($entity_controller->delete($entity)) {
    drupal_set_message(t('The record title "%name" has been deleted.', array('%name' => $entity->title)));
    $form_state['redirect'] = 'admin/content/tripal_entitys';
  }
  else {
    drupal_set_message(t('The tripal_entity %name was not deleted.', array('%name' => $entity->title)), "error");
  }
}





