<?php

/**
 * @file
 */

/**
 * UI controller.
 */
class TripalBundleUIController extends EntityDefaultUIController {

  public function __construct($entity_type, $entity_info) {
     parent::__construct($entity_type, $entity_info);
  }

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Tripal data types, including adding
      and removing fields and the display of fields.';

    // We don't want to let the user import new Tripal data types.
    unset($items[$this->path . '/import']);

    $items[$this->path . '/add'] = array(
      'title' => 'Add new biological data type',
      'description' => 'Add new biological data type',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entities_admin_add_type_form'),
      'access arguments' => array('administer tripal data types'),
      'file' =>  'includes/tripal_entities.admin.inc',
      'file path' => drupal_get_path('module', 'tripal_entities'),
      'type' => MENU_LOCAL_ACTION,
      'weight' => 2
    );
    return $items;
  }

  /**
   * Allows us to change the forms created by the parent class.
   */
  function hook_forms() {
    $forms = parent::hook_forms();

    // The edit form for the entity type by default expects a function,
    // named after the entity type but we can't dynamically create these
    // functions. We'll use a single form for all entity types.
    $forms[$this->entityType . '_form'] = array(
      'callback' => 'tripal_entities_tripal_bundle_form',
      'callback arguments' => array($this->entityType)
    );

    return $forms;
  }
}
/**
 *
 * @param $form
 * @param $form_state
 * @param $entity
 */
function tripal_entities_tripal_bundle_form($form, &$form_state, $entityDataType) {
  $form = array();
  $form['message'] = array(
    '#type' => 'item',
    '#markup' => 'Edit the function "tripal_entities_tripal_bundle_form()" to add a form each type. Put access controls here?',
  );
  return $form;
}
/**
 * Access callback for the entity API.
 */
function tripal_bundle_access($op, $type = NULL, $account = NULL) {
  return user_access('administer tripal data types', $account);
}

/**
 * Generates the tripal data type editing form.
 */
function tripal_bundle_form($form, &$form_state, $tripal_bundle, $op = 'edit') {

  if ($op == 'clone') {
    $tripal_bundle->label .= ' (cloned)';
    $tripal_bundle->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $tripal_bundle->label,
    '#description' => t('The human-readable name of this tripal data type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function tripal_bundle_form_submit(&$form, &$form_state) {
  $tripal_bundle = entity_ui_form_submit_build_entity($form, $form_state);
  $tripal_bundle->save();
  $form_state['redirect'] = $this->path;
}

/**
 * Form API submit callback for the delete button.
 */
function tripal_bundle_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = $this->path;
}

