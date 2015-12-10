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

    // Alter the Admin > Structure > Tripal Content Types menu item.
    $items[$this->path]['description'] = 'Manage biological content types that are
      added using Tripal.';

    // We don't want to let the user import new Tripal data types.
    unset($items[$this->path . '/import']);

    // Add an action link to Admin > Structure > Tripal Content Types for adding types.
    $items[$this->path . '/add'] = array(
      'title' => 'Add Tripal Content Type',
      'description' => 'Add new biological content',
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
 * Tripal content type edit form.
 *
 * @param $form
 *    The default form array. Usually empty.
 * @param $form_state
 *    Build information for the form including the entity type and submitted values.
 * @param $entityDataType
 *    A string indicating the entity type. This will always be TripalBundle.
 */
function tripal_entities_tripal_bundle_form($form, &$form_state, $entityDataType) {

  $entity_type = $form_state['build_info']['args'][0];
  if (preg_match('/bio-data_(\d+)/', $entity_type->term_id, $matches)) {
    $term = entity_load('TripalTerm', array('id' => $entity_type->term_id));
    $term = reset($term);
    $vocab = entity_load('TripalVocab', array('id' => $term->vocab_id));
    $vocab = reset($vocab);
  }

  // Add a validate and submit handler to save the data in this form.
  $form['#validate'] = array('tripal_entities_tripal_bundle_form_validate');
  $form['#submit'] = array('tripal_entities_tripal_bundle_form_submit');

  // @TODO: Move this into a css file.
  $form['#attached']['css'] = array(
    array(
      'data' => '
        .form-item select, .form-item input { width:40%; }
        th.side-header { width: 220px; }',
      'type' => 'inline',
    ),
  );

    if ($term) {
    $form['term'] = array(
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(),
        'rows' => array(
          array(array('header' => TRUE, 'data' => 'Vocabulary', 'class' => array('side-header')), $vocab->namespace),
          array(array('header' => TRUE, 'data' => 'Term', 'class' => array('side-header')), $term->name),
        )
      ))
    );
  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#description' => t('The human-readable name of this content type. This text will be
      displayed as part of the list on the <em>Add new content page</em>. It is recommended that
      this name begin with a capital letter and contain only letters, numbers, and spaces.
      This name must be unique.'),
    '#default_value' => $entity_type->label,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#description' => t('Describe this content type. The text will be displayed on the <em>Add new content page</em>.'),
    '#default_value' => tripal_get_bundle_variable('description', $entity_type->id, $term->definition),
  );
  if (!$form['description']['#default_value']) {
    //$form['description']['#default_value'] = $cvterm->definition;
  }

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  // Set Title Format.
  //-------------------------
  $title_format = tripal_get_title_format($entity_type);

  $form['set_titles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page Title options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#group' => 'additional_settings',
  );

  $form['set_titles']['explanation'] = array(
    '#type' => 'item',
    '#markup' => t('<p>The format below is used to determine the title displayed on content
      pages. This ensures all content of this type is consistent while still allowing you
      to indicate which data you want represented in the title (ie: which data would most
      identify your content).</p>
      <p>Keep in mind that it might be confusing to users if more than
      one page has the same title. We recommend you <strong>choose a combination of tokens that
      will uniquely identify your content</strong>.</p>'),
  );

  $form['set_titles']['title_format'] = array(
    '#type' => 'textarea',
    '#title' => t('Page Title Format'),
    '#description' => t('You may rearrange elements in this text box to customize the page
      titles. The available tokens are listed below. You can separate or include any text
      between the tokens.'),
    '#required' => TRUE,
    '#default_value' => $title_format,
    '#rows' => 1
  );

  $form['set_titles']['token_display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#description' => t('Copy the token and paste it into the "Custom Page Title" text field above.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $tokens = tripal_get_tokens($entity_type);
  $form['set_titles']['tokens'] = array(
    '#type' => 'hidden',
    '#value' => serialize($tokens)
  );

  $form['set_titles']['token_display']['content'] = array(
    '#type' => 'item',
    '#markup' => theme_token_list($tokens)
  );

  // Submit Buttons
  //-------------------------

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Content Type'),
    '#weight' => 100
  );

  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete Content Type'),
    '#weight' => 101
  );

  return $form;
}

/**
 * Validate: Tripal content type edit form.
 */
function tripal_entities_tripal_bundle_form_validate($form, $form_state) {

  $tokens_available = unserialize($form_state['values']['set_titles']['tokens']);
  if (preg_match_all('/(\[\w+\])/', $form_state['values']['set_titles']['title_format'], $matches)) {

    // The matches of the first and only pattern will be our tokens.
    $tokens_used = $matches[1];
    // Determine if any of the tokens used were not in the original list of available tokens.
    $tokens_missing = array_diff($tokens_used, array_keys($tokens_available));

    if ($tokens_missing) {
      $msg = t('You must only use tokens listed under available tokens. You used the following incorrect tokens: %tokens',
        array('%tokens' => implode(', ', $tokens_missing)));
      form_set_error('set_titles][title_format', $msg);
    }

  }
  else {
    $msg = t('You should use at least one token in your title format or the title for all %type pages will be the same.',
      array('%type' => $form_state['build_info']['args'][0]->label));
    form_set_error('set_titles][title_format', $msg);
  }
}

/**
 * Submit: Tripal content type edit form.
 */
function tripal_entities_tripal_bundle_form_submit($form, &$form_state) {

  if ($form_state['triggering_element']['#value'] == 'Save Content Type') {
    $bundle_entity = $form_state['build_info']['args'][0];

    // Save the label.
    $bundle_entity->label = $form_state['values']['label'];
    $bundle_entity->save();

    // Save the description.
    tripal_bundle_save_variable('description', $bundle_entity->id, $form_state['values']['description']);

    // Save the page title format.
    tripal_save_title_format(
      $bundle_entity,
      $form_state['values']['set_titles']['title_format']
    );

    $form_state['redirect'] = 'admin/structure/bio-data';
    drupal_set_message(t('Successfully saved %type content type.', array('%type' => $form_state['build_info']['args'][0]->label)));
  }
  else {
    drupal_set_message(t('%button is not supported at this time.', array('%button' => $form_state['triggering_element']['#value'])), 'warning');
  }
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

