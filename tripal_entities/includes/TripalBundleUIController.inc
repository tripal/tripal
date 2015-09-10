<?php

/**
 * @file
 */

/**
 * UI controller.
 */
class TripalBundleUIController extends EntityDefaultUIController {

  public function __construct($entity_type, $entity_info) {
    // The 'bundle of' property is usually set in the hook_entity_info()
    // function for the "entity type" entity.  This allows the Entity API
    // to provide the user interface for managing fields attached to the
    // bundle.  But, we are using the same controller classes for
    // all entity types and we do not want new links for every
    // entity type (vocabulary) on the Administration >> Structure menu.
    // We just want one menu item.  So to support one menu item that
    // can handle all of the Tripal entity types, we have to set the
    // 'bundle of' property here rather than in the hook_entity_info() function.
    $bundle_of = $entity_type;
    $bundle_of = preg_replace('/_bundle/', '', $bundle_of);
    $entity_info['bundle of'] = $bundle_of;
    parent::__construct($entity_type, $entity_info);
  }

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Tripal data types, including adding
      and removing fields and the display of fields.';

    // We don't want to let the user add new Tripal data types. They
    // are added automatically.
    unset($items[$this->path . '/add']);
    unset($items[$this->path . '/import']);

    $items[$this->path . '/publish'] = array(
      'title' => 'Add new biological data type',
      'description' => 'Publish Data',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_entities_admin_publish_form'),
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

