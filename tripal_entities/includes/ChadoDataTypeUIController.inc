<?php

/**
 * @file
 */

/**
 * UI controller.
 */
class ChadoDataTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Chado data types, including adding
      and removing fields and the display of fields.';

    // We don't want to let the user add new Chado data types. They
    // are added automatically.
     unset($items[$this->path . '/add']);
     unset($items[$this->path . '/import']);

    return $items;
  }
}
/**
 * Access callback for the entity API.
 */
function chado_data_type_access($op, $type = NULL, $account = NULL) {
  return user_access('administer chado data types', $account);
}

/**
 * Generates the chado data type editing form.
 */
function chado_data_type_form($form, &$form_state, $chado_data_type, $op = 'edit') {

  if ($op == 'clone') {
    $chado_data_type->label .= ' (cloned)';
    $chado_data_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $chado_data_type->label,
    '#description' => t('The human-readable name of this chado data type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function chado_data_type_form_submit(&$form, &$form_state) {
  $chado_data_type = entity_ui_form_submit_build_entity($form, $form_state);
  $chado_data_type->save();
  $form_state['redirect'] = 'admin/structure/chado_data_types';
}

/**
 * Form API submit callback for the delete button.
 */
function chado_data_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/chado_data_types/manage/' . $form_state['chado_data_type']->type . '/delete';
}

