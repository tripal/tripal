<?php

/**
 * @file
 */

/**
 * UI controller.
 */
class TripalDataTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Tripal data types, including adding
      and removing fields and the display of fields.';

    // We don't want to let the user add new Tripal data types. They
    // are added automatically.
    //unset($items[$this->path . '/add']);
    //unset($items[$this->path . '/import']);

    drupal_debug($items);
    return $items;
  }
}
/**
 * Access callback for the entity API.
 */
function tripal_data_type_access($op, $type = NULL, $account = NULL) {
  return user_access('administer tripal data types', $account);
}

/**
 * Generates the tripal data type editing form.
 */
function tripal_data_type_form($form, &$form_state, $tripal_data_type, $op = 'edit') {

  if ($op == 'clone') {
    $tripal_data_type->label .= ' (cloned)';
    $tripal_data_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $tripal_data_type->label,
    '#description' => t('The human-readable name of this tripal data type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function tripal_data_type_form_submit(&$form, &$form_state) {
  $tripal_data_type = entity_ui_form_submit_build_entity($form, $form_state);
  $tripal_data_type->save();
  $form_state['redirect'] = 'admin/structure/tripal_data_types';
}

/**
 * Form API submit callback for the delete button.
 */
function tripal_data_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/tripal_data_types/manage/' . $form_state['tripal_data_type']->type . '/delete';
}

