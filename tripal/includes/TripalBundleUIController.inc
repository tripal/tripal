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
      'description' => 'Add data type content',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('tripal_admin_add_type_form'),
      'access arguments' => array('administer tripal data types'),
      'file' =>  'includes/tripal.admin.inc',
      'file path' => drupal_get_path('module', 'tripal'),
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
      'callback' => 'tripal_tripal_bundle_form',
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
function tripal_tripal_bundle_form($form, &$form_state, $entityDataType) {

  $bundle = $form_state['build_info']['args'][0];
  $term = NULL;
  $vocab = NULL;
  if (preg_match('/bio_data_(\d+)/', $bundle->name, $matches)) {
    $term = entity_load('TripalTerm', array('id' => $matches[1]));
    $term = reset($term);
    $vocab = entity_load('TripalVocab', array('id' => $term->vocab_id));
    $vocab = reset($vocab);
  }

  // Add a validate and submit handler to save the data in this form.
  $form['#validate'] = array('tripal_tripal_bundle_form_validate');
  $form['#submit'] = array('tripal_tripal_bundle_form_submit');
  $form['#bundle'] = $bundle;

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

    $rows = array(
      array(
        array(
          'header' => TRUE,
          'data' => 'Vocabulary',
          'class' => array('side-header')
        ),
        $vocab->vocabulary
      ),
      array(
        array(
          'header' => TRUE,
          'data' => 'Term',
          'class' => array('side-header')
        ),
        $term->name
      ),
      array(
        array(
          'header' => TRUE,
          'data' => 'Definition',
          'class' => array('side-header')
        ),
        $term->definition
      )
    );
    $table_vars = array(
      'header' => array(),
      'rows' => $rows,
      'attributes' => array(),
      'caption' => '',
      'sticky' => FALSE,
      'colgroups' => array(),
      'empty' => '',
    );
    $form['term'] = array(
      '#type' => 'item',
      '#title' => t('Vocabulary Term'),
      '#markup' => theme_table($table_vars)
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
    '#default_value' => $bundle->label,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#description' => t('Describe this content type. The text will be displayed on the <em>Add new content page</em>.'),
  );
  if ($term) {
    $form['description']['#default_value'] = tripal_get_bundle_variable('description', $entity_type->id, $term->definition);
  }
  else {
    $form['description']['#default_value'] = tripal_get_bundle_variable('description', $entity_type->id, '');
  }

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  // Set Title Format.
  //-------------------------
  $title_format = tripal_get_title_format($bundle);

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
    '#markup' => t('<p>The format below is used to determine the title displayed on %type content
      pages. This ensures all content of this type is consistent while still allowing you
      to indicate which data you want represented in the title (ie: which data would most
      identify your content).</p>
      <p>Keep in mind that it might be confusing to users if more than
      one page has the same title. We recommend you <strong>choose a combination of tokens that
      will uniquely identify your content</strong>.</p>',
      array('%type' => $bundle->label)),
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

  $tokens = tripal_get_entity_tokens($bundle);
  $form['set_titles']['tokens'] = array(
    '#type' => 'hidden',
    '#value' => serialize($tokens)
  );

  $form['set_titles']['token_display']['content'] = array(
    '#type' => 'item',
    '#markup' => theme_token_list($tokens),
  );

  // Set URL Alias Pattern.
  //-------------------------
  $url_pattern = tripal_get_bundle_variable('url_format', $bundle->id, '');
  if (!$url_pattern) $url_pattern = str_replace(' ', '', $term->name) . '/[TripalEntity__entity_id]';

  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('URL Path options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#group' => 'additional_settings',
  );

  $form['url']['explanation'] = array(
    '#type' => 'item',
    '#markup' => t('<p>The pattern below is used to specify the URL of %type content pages.
    This allows you to present more friendly, informative URLs to your user.</p>
    <p><strong>You must choose a combination of tokens that results in a unique path for
    each page!</strong></p>',
    array('%type' => $bundle->label)),
  );

  $form['url']['url_pattern'] = array(
    '#type' => 'textarea',
    '#title' => t('URL Alias Pattern'),
    '#description' => t('You may rearrange elements in this text box to customize the url
      alias. The available tokens are listed below. <strong>Make sure the pattern forms a
      valid, unique URL</strong>. Leave this field blank to use the original path.'),
    '#default_value' => $url_pattern,
    '#required' => TRUE,
    '#rows' => 1
  );

  $tokens = tripal_get_entity_tokens($bundle, array('required only' => TRUE));
  $form['url']['tokens'] = array(
    '#type' => 'hidden',
    '#value' => serialize($tokens)
  );

  $form['url']['token_display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#description' => t('Copy the token and paste it into the "URL Alias Pattern" text field above.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $form['url']['token_display']['content'] = array(
    '#type' => 'item',
    '#markup' => theme_token_list($tokens),
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
function tripal_tripal_bundle_form_validate($form, $form_state) {

  // VALIDATE: The only tokens used should be those we mentioned under "Available Tokens".
  // PART 1: Set Titles.
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

  // PART 2: URL Alias'
  if ($form_state['values']['url']['url_pattern']) {
    $tokens_available = unserialize($form_state['values']['url']['tokens']);
    if (preg_match_all('/(\[\w+\])/', $form_state['values']['url']['url_pattern'], $matches)) {

      // The matches of the first and only pattern will be our tokens.
      $tokens_used = $matches[1];
      // Determine if any of the tokens used were not in the original list of available tokens.
      $tokens_missing = array_diff($tokens_used, array_keys($tokens_available));

      if ($tokens_missing) {
        $msg = t('You must only use tokens listed under available tokens. You used the following incorrect tokens: %tokens',
          array('%tokens' => implode(', ', $tokens_missing)));
        form_set_error('url][url_pattern', $msg);
      }

    }
    else {
      $msg = t('You should use at least one token in your URL pattern or the URL for all %type pages will be the same.',
        array('%type' => $form_state['build_info']['args'][0]->label));
      form_set_error('url][url_pattern', $msg);
    }
  }
}

/**
 * Submit: Tripal content type edit form.
 */
function tripal_tripal_bundle_form_submit($form, &$form_state) {

  $bundle_entity = $form_state['build_info']['args'][0];

  if ($form_state['triggering_element']['#value'] == 'Save Content Type') {

    // Save the label.
    $bundle_entity->label = $form_state['values']['label'];
    $bundle_entity->save();

    // Save the description.
    tripal_set_bundle_variable('description', $bundle_entity->id, $form_state['values']['description']);

    // Save the page title format.
    tripal_save_title_format(
      $bundle_entity,
      $form_state['values']['set_titles']['title_format']
    );

    // Save the URL alias pattern if it's set.
    if ($form_state['values']['url']['url_pattern']) {
      tripal_set_bundle_variable('url_format', $bundle_entity->id, $form_state['values']['url']['url_pattern']);
    }

    $form_state['redirect'] = 'admin/structure/bio_data';
    drupal_set_message(t('Successfully saved %type content type.', array('%type' => $form_state['build_info']['args'][0]->label)));
  }
  else {
    $form_state['redirect'] = array(
      'admin/structure/bio_data/manage/' . $bundle_entity->name . '/delete',
      array('query' => array('destination' => 'admin/structure/bio_data'))
    );
  }
}

/**
 * Access callback for the entity API.
 */
function tripal_bundle_access($op, $type = NULL, $account = NULL) {
  return user_access('administer tripal data types', $account);
}

/**
 * Form for creating tripal data types.
 *
 * This form is available on the menu at Admin >> Structure >> Biological Data
 * Types. It requires that a module implmennt the vocabulary storage.  Tripal
 * knows which vocabulary storage methods are available when a module
 * implements the hook_vocab_storage_info() hook.
 *
 */
function tripal_admin_add_type_form($form, &$form_state) {

  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_select_term_form';
    if (function_exists($function)) {
      $form = $function($form, $form_state);
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
 * Implements hook_validate() for the tripal_admin_add_type_form.
 *
 */
function tripal_admin_add_type_form_validate($form, &$form_state) {
  // TODO: we need some sort of administrative interface that lets the user
  // switch to the desired vocabulary type. For now, we'll just use the
  // first one in the list.
  $stores = module_invoke_all('vocab_storage_info');
  if (is_array($stores) and count($stores) > 0) {
    $keys = array_keys($stores);
    $module = $stores[$keys[0]]['module'];
    $function = $module . '_vocab_select_term_form_validate';
    if (function_exists($function)) {
      $function($form, $form_state);
    }
  }
}
/**
 * Implements hook_submit() for the tripal_admin_add_type_form.
 *
 * The storage backend must set the
 *
 */
function tripal_admin_add_type_form_submit($form, &$form_state) {
  $vocabulary = '';
  $accession = '';
  if (array_key_exists('storage', $form_state)) {
    $storage = $form_state['storage'];
    $vocabulary = array_key_exists('vocabulary', $storage) ? $storage['vocabulary'] : '';
    $accession = array_key_exists('accession', $storage) ? $storage['accession'] : '';
    $term_name = array_key_exists('term_name', $storage) ? $storage['term_name'] : '';

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $term = tripal_load_term_entity(array('vocabulary' => $vocabulary, 'accession' => $accession));
    if (!$term) {
      $error = '';
      $args = array(
        'vocabulary' => $vocabulary,
        'accession' => $accession,
        'term_name' => $term_name,
        'form_values' => $form_state['values'],
      );
      $success = tripal_create_bundle($args, $error);
      if (!$success) {
        drupal_set_message($error, 'error');
        $form_state['redirect'] = "admin/structure/bio_data";
      }
      else {
        drupal_set_message('New data type created.');
        $form_state['redirect'] = "admin/structure/bio_data";
      }
    }
    else {
      drupal_set_message("The term '$accession' already exists as a content type.", 'warning');
    }
  }
}