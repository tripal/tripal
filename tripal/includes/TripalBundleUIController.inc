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
    $items[$this->path . '/add'] = [
      'title' => 'Add Tripal Content Type',
      'description' => 'Add data type content',
      'page callback' => 'drupal_get_form',
      'page arguments' => ['tripal_admin_add_type_form'],
      'access arguments' => ['manage tripal content types'],
      'type' => MENU_LOCAL_ACTION,
      'weight' => 2,
    ];

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
    $forms[$this->entityType . '_form'] = [
      'callback' => 'tripal_tripal_bundle_form',
      'callback arguments' => [$this->entityType],
    ];

    return $forms;
  }

  /**
   * Renders the Bundle overview table
   */
  public function overviewTable($conditions = []) {
    $entities = entity_load($this->entityType, FALSE, $conditions);

    // Sort the entities by label.
    $sorted = [];
    foreach ($entities as $entity) {
      $sorted[$entity->label] = $entity;
    }
    ksort($sorted, SORT_STRING | SORT_FLAG_CASE);

    $rows = [];
    foreach ($sorted as $entity) {
      // Get the term for this content type
      $additional_cols = [$entity->term->name . ' (' . l($entity->accession, 'cv/lookup/' . $entity->term->vocab->vocabulary . '/' . $entity->term->accession) . ')'];
      $rows[] = $this->overviewTableRow($conditions,
        entity_id($this->entityType, $entity), $entity,
        $additional_cols);
    }
    // Assemble the right table header.
    $header = [t('Label')];
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }
    $header[] = [
      'data' => t('Term'),
    ];
    // Add operations with the right colspan.
    $field_ui = !empty($this->entityInfo['bundle of']) && module_exists('field_ui');
    $exportable = !empty($this->entityInfo['exportable']);
    $colspan = 3;
    $colspan = $field_ui ? $colspan + 2 : $colspan;
    $colspan = $exportable ? $colspan + 1 : $colspan;
    $header[] = [
      'data' => t('Operations'),
      'colspan' => $colspan,
    ];

    $render = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => t('None.'),
    ];
    return $render;
  }

}

/**
 * Tripal content type edit form.
 *
 * @param $form
 *    The default form array. Usually empty.
 * @param $form_state
 *    Build information for the form including the entity type and submitted
 *   values.
 * @param $entityDataType
 *    A string indicating the entity type. This will always be TripalBundle.
 */
function tripal_tripal_bundle_form($form, &$form_state, $entityDataType) {

  $bundle = $form_state['build_info']['args'][0];
  $term = NULL;
  $vocab = NULL;
  if (preg_match('/bio_data_(\d+)/', $bundle->name, $matches)) {
    $term = entity_load('TripalTerm', ['id' => $matches[1]]);
    $term = reset($term);
    $vocab = entity_load('TripalVocab', ['id' => $term->vocab_id]);
    $vocab = reset($vocab);
  }

  // Add a validate and submit handler to save the data in this form.
  $form['#validate'] = ['tripal_tripal_bundle_form_validate'];
  $form['#submit'] = ['tripal_tripal_bundle_form_submit'];
  $form['#bundle'] = $bundle;

  // @TODO: Move this into a css file.
  $form['#attached']['css'] = [
    [
      'data' => '
        th.side-header { width: 220px; }',
      'type' => 'inline',
    ],
  ];

  if ($term) {

    $rows = [
      [
        [
          'header' => TRUE,
          'data' => 'Vocabulary',
          'class' => ['side-header'],
        ],
        $vocab->vocabulary,
      ],
      [
        [
          'header' => TRUE,
          'data' => 'Term Name',
          'class' => ['side-header'],
        ],
        $term->name,
      ],
      [
        [
          'header' => TRUE,
          'data' => 'Accession',
          'class' => ['side-header'],
        ],
        $term->accession,
      ],
      [
        [
          'header' => TRUE,
          'data' => 'Definition',
          'class' => ['side-header'],
        ],
        $term->definition,
      ],
    ];
    $table_vars = [
      'header' => [],
      'rows' => $rows,
      'attributes' => [],
      'caption' => '',
      'sticky' => FALSE,
      'colgroups' => [],
      'empty' => '',
    ];
    $form['term'] = [
      '#type' => 'item',
      '#title' => t('Vocabulary Term'),
      '#markup' => theme_table($table_vars),
    ];
  }

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#description' => t('The human-readable name of this content type. This text will be
      displayed as part of the list on the <em>Add new content page</em>. It is recommended that
      this name begin with a capital letter and contain only letters, numbers, and spaces.
      This name must be unique.'),
    '#default_value' => $bundle->label,
  ];

  $form['description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#description' => t('Describe this content type. The text will be displayed on the <em>Add new content page</em>.'),
  ];
  if ($term) {
    $form['description']['#default_value'] = tripal_get_bundle_variable('description', $bundle->id, $term->definition);
  }
  else {
    $form['description']['#default_value'] = tripal_get_bundle_variable('description', $bundle->id, '');
  }

  $hide_empty_field_var = tripal_get_bundle_variable('hide_empty_field', $bundle->id);
  if ($hide_empty_field_var != 0) {
    $hide_empty_field_var = TRUE;
  }

  $ajax_field_var = tripal_get_bundle_variable('ajax_field', $bundle->id);
  if ($ajax_field_var != 0) {
    $ajax_field_var = TRUE;
  }

  $form['hide_empty_field'] = [
    '#type' => 'checkbox',
    '#title' => t('Hide empty fields'),
    '#description' => t('Uncheck this box if you would like to show all empty fields.'),
    '#default_value' => $hide_empty_field_var,
    '#weight' => 9,
  ];

  $form['ajax_field'] = [
    '#type' => 'checkbox',
    '#title' => t('Load field using AJAX'),
    '#description' => t('Uncheck this box if you do not want field data to load by ajax, this may signifiantly increase page load times.'),
    '#default_value' => $ajax_field_var,
    '#weight' => 9,
  ];

  $form['ajax_field disclaimer'] = [
    '#type' => 'item',
    '#markup' => t(
      '<p>Please note, if both "Hide empty fields" and "Load field using AJAX" 
      are checked empty fields will be hidden using javascript which may result
       in some jumpiness of content as the page finishes loading.</p>'
    ),
    '#weight' => 10,
  ];

  $form['additional_settings'] = [
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  ];

  // Set Title Format.
  //-------------------------
  $title_format = tripal_get_title_format($bundle);

  $form['set_titles'] = [
    '#type' => 'fieldset',
    '#title' => t('Page Title options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#group' => 'additional_settings',
  ];

  $form['set_titles']['explanation'] = [
    '#type' => 'item',
    '#markup' => t('<p>The format below is used to determine the title displayed on %type content
      pages. This ensures all content of this type is consistent while still allowing you
      to indicate which data you want represented in the title (ie: which data would most
      identify your content).</p>
      <p>Keep in mind that it might be confusing to users if more than
      one page has the same title. We recommend you <strong>choose a combination of tokens that
      will uniquely identify your content</strong>.</p>',
      ['%type' => $bundle->label]),
  ];

  $form['set_titles']['title_format'] = [
    '#type' => 'textarea',
    '#title' => t('Page Title Format'),
    '#description' => t('You may rearrange elements in this text box to customize the page
      titles. The available tokens are listed below. You can separate or include any text
      between the tokens.'),
    '#required' => TRUE,
    '#default_value' => $title_format,
    '#rows' => 1,
  ];

  $form['set_titles']['token_display'] = [
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#description' => t('Copy the token and paste it into the "Custom Page Title" text field above.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  $tokens = tripal_get_entity_tokens($bundle);
  $form['set_titles']['tokens'] = [
    '#type' => 'hidden',
    '#value' => serialize($tokens),
  ];

  $form['set_titles']['token_display']['content'] = [
    '#type' => 'item',
    '#markup' => theme_token_list($tokens),
  ];
  $form['set_titles']['bp_explanation'] = [
    '#type' => 'item',
    '#markup' => t('Retroactively apply the new title pattern to
        existing content by clicking the button below.',
      ['%type' => $bundle->label]),
  ];
  $form['set_titles']['bulk_update'] = [
    '#type' => 'submit',
    '#value' => t('Bulk update all titles'),
    //'#submit' => array('tripal_bulk_update_submit'),
  ];

  // Set URL Alias Pattern.
  //-------------------------
  $url_pattern = tripal_get_bundle_variable('url_format', $bundle->id, '');
  if (!$url_pattern) {
    $url_pattern = str_replace(' ', '', $term->name) . '/[TripalEntity__entity_id]';
  }

  $form['url'] = [
    '#type' => 'fieldset',
    '#title' => t('URL Path options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#group' => 'additional_settings',
  ];

  $form['url']['explanation'] = [
    '#type' => 'item',
    '#markup' => t('<p>The pattern below is used to specify the URL of %type content pages.
    This allows you to present more friendly, informative URLs to your user.</p>
    <p><strong>You must choose a combination of tokens that results in a unique path for
    each page!</strong></p>',
      ['%type' => $bundle->label]),
  ];

  $form['url']['url_pattern'] = [
    '#type' => 'textarea',
    '#title' => t('URL Alias Pattern'),
    '#description' => t('You may rearrange elements in this text box to customize the url
      alias. The available tokens are listed below. <strong>Make sure the pattern forms a
      valid, unique URL</strong>. Leave this field blank to use the original path.'),
    '#default_value' => $url_pattern,
    '#required' => TRUE,
    '#rows' => 1,
  ];


  $tokens = tripal_get_entity_tokens($bundle);
  $form['url']['tokens'] = [
    '#type' => 'hidden',
    '#value' => serialize($tokens),
  ];

  $form['url']['token_display'] = [
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#description' => t('Copy the token and paste it into the "URL Alias Pattern" ' .
      'text field above. Please choose tokens that will guarantee a unique URL.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];

  $form['url']['token_display']['content'] = [
    '#type' => 'item',
    '#markup' => theme_token_list($tokens),
  ];
  $form['url']['bp_explanation'] = [
    '#type' => 'item',
    '#markup' => t('Retroactively apply the new url alias pattern to
        existing content by clicking the button below.',
      ['%type' => $bundle->label]),
  ];
  $form['url']['bulk_update'] = [
    '#type' => 'submit',
    '#value' => t('Bulk update all aliases'),
    //'#submit' => array('tripal_bulk_update_submit'),
  ];
  // Submit Buttons
  //-------------------------

  $form['save'] = [
    '#type' => 'submit',
    '#value' => t('Save Content Type'),
    '#weight' => 100,
  ];

  $form['delete'] = [
    '#type' => 'submit',
    '#value' => t('Delete Content Type'),
    '#weight' => 101,
  ];

  return $form;
}

/**
 * Validate: Tripal content type edit form.
 */
function tripal_tripal_bundle_form_validate($form, $form_state) {
  // VALIDATE: That there is a value passed for the hide_empty_field option. If
  // no value passed default to hide field.
  if (empty($form_state['values']['hide_empty_field'])) {
    $form_state['values']['hide_empty_field'] = TRUE;
  }
  if (empty($form_state['values']['ajax_field'])) {
    $form_state['values']['ajax_field'] = TRUE;
  }
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
        ['%tokens' => implode(', ', $tokens_missing)]);
      form_set_error('set_titles][title_format', $msg);
    }

  }
  else {
    $msg = t('You should use at least one token in your title format or the title for all %type pages will be the same.',
      ['%type' => $form_state['build_info']['args'][0]->label]);
    form_set_error('set_titles][title_format', $msg);
  }

  // PART 2: URL Alias'
  if ($form_state['values']['url']['url_pattern']) {
    $url_pattern = trim($form_state['values']['url']['url_pattern']);
    $tokens_available = unserialize($form_state['values']['url']['tokens']);

    // For backwards compatibility we should warn uses if they use the word 
    // 'feature' as this conflicts with the feature lookup tool. 
    if (preg_match('/^feature\//', $url_pattern)) {
      $msg = t('The word "feature" should not be used first in the pattern as it conflicts with the feature lookup tool provided by Tripal');
      form_set_error('url][url_pattern', $msg);
    }

    // Now make sure the tokens are valid.
    if (preg_match_all('/(\[\w+\])/', $url_pattern, $matches)) {

      // The matches of the first and only pattern will be our tokens.
      $tokens_used = $matches[1];
      // Determine if any of the tokens used were not in the original list of available tokens.
      $tokens_missing = array_diff($tokens_used, array_keys($tokens_available));

      if ($tokens_missing) {
        $msg = t('You must only use tokens listed under available tokens. You used the following incorrect tokens: %tokens',
          ['%tokens' => implode(', ', $tokens_missing)]);
        form_set_error('url][url_pattern', $msg);
      }

    }
    else {
      $msg = t('You should use at least one token in your URL pattern or the URL for all %type pages will be the same.',
        ['%type' => $form_state['build_info']['args'][0]->label]);
      form_set_error('url][url_pattern', $msg);
    }
  }
}

/**
 * Submit: Tripal content type edit form.
 */
function tripal_tripal_bundle_form_submit($form, &$form_state) {
  global $user;

  $trigger = $form_state['triggering_element']['#value'];
  $bundle = $form_state['build_info']['args'][0];

  if ($trigger == 'Save Content Type' or $trigger == 'Bulk update all aliases' or
    $trigger == 'Bulk update all titles') {

    // Save the label.
    $bundle->label = $form_state['values']['label'];
    $bundle->save();

    // Save the description.
    tripal_set_bundle_variable('description', $bundle->id, $form_state['values']['description']);

    // Save the hide_empty_field setting.
    tripal_set_bundle_variable('hide_empty_field', $bundle->id, $form_state['values']['hide_empty_field']);
    tripal_set_bundle_variable('ajax_field', $bundle->id, $form_state['values']['ajax_field']);

    // Save the page title format.
    tripal_save_title_format(
      $bundle,
      $form_state['values']['set_titles']['title_format']
    );

    // Save the URL alias pattern if it's set.
    if ($form_state['values']['url']['url_pattern']) {
      $url_pattern = trim($form_state['values']['url']['url_pattern']);
      tripal_set_bundle_variable('url_format', $bundle->id, $url_pattern);
    }

    // There are two submit buttons for this either updating the paths or the
    // titles.
    if ($trigger == 'Bulk update all titles') {
      $update = $form_state['input']['set_titles']['title_format'];
      $type = 'title';
      $args = [
        'bundle_id' => $bundle->name,
        'update' => $update,
        'type' => $type,
      ];
      $includes = [
        module_load_include('inc', 'tripal', 'includes/tripal.bulk_update'),
      ];
      tripal_add_job('Update all aliases for content type: ' . $bundle->label, 'tripal',
        'tripal_update_all_urls_and_titles', $args, $user->uid, 10, $includes);
    }
    elseif ($trigger == 'Bulk update all aliases') {
      $update = $url_pattern;
      $type = 'alias';
      $args = [
        'bundle_id' => $bundle->name,
        'update' => $update,
        'type' => $type,
      ];
      $includes = [
        module_load_include('inc', 'tripal', 'includes/tripal.bulk_update'),
      ];
      tripal_add_job('Update all aliases for content type: ' . $bundle->label, 'tripal',
        'tripal_update_all_urls_and_titles', $args, $user->uid, 10, $includes);
    }

    $form_state['redirect'] = 'admin/structure/bio_data';
    drupal_set_message(t('Successfully saved %type content type.', ['%type' => $form_state['build_info']['args'][0]->label]));
  }

  else {
    $form_state['redirect'] = [
      'admin/structure/bio_data/manage/' . $bundle->name . '/delete',
      ['query' => ['destination' => 'admin/structure/bio_data']],
    ];
  }
}

/**
 * Access callback for the entity API.
 */
function tripal_bundle_access($op, $type = NULL, $account = NULL) {
  return user_access('manage tripal content types', $account);
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

  // Make sure we have a storage backend for managing content types.
  $stores = module_invoke_all('vocab_storage_info');
  if (!is_array($stores) or count($stores) == 0) {
    tripal_set_message('A storage backend is not enabled for managing
          the vocabulary terms used to create content.  Please enable
          a module that supports storage of vocabualary terms (e.g. tripal_chado)
          and return to create new Tripal content types.', TRIPAL_NOTICE);
    return;
  }

  // Set the stage to step1 if it isn't already set.
  if (!isset($form_state['stage'])) {
    $form_state['stage'] = 'step1';
  }
  $stage = $form_state['stage'];


  // Get the selected term.
  if (array_key_exists('values', $form_state) and
    array_key_exists('term', $form_state['values'])) {
    $selected_term = $form_state['values']['term'];
  }
  else {
    $selected = tripal_get_term_lookup_form_result($form, $form_state);
    if ($selected) {
      $selected_term = $selected[0];
    }
  }

  // Get the selected storage element.
  $default_store = 'term_chado_storage';
  if (array_key_exists('values', $form_state) and
    array_key_exists('store_select', $form_state['values'])) {
    $default_store = $form_state['values']['store_select'];
  }


  // Handle the different stages:
  if ($stage == 'step1') {
    tripal_admin_add_type_form_step1($form, $form_state);
  }
  if ($form_state['stage'] == 'step2') {
    tripal_admin_add_type_form_step1_summary($form, $form_state, $selected_term);
    tripal_admin_add_type_form_step2($form, $form_state, $stores, $selected_term, $default_store);
  }
  if ($form_state['stage'] == 'step3') {
    tripal_admin_add_type_form_step1_summary($form, $form_state, $selected_term);
    tripal_admin_add_type_form_step2_summary($form, $form_state, $stores, $selected_term, $default_store);
    tripal_admin_add_type_form_step3($form, $form_state, $stores, $selected_term, $default_store);
  }

  $form['#prefix'] = '<div id = "tripal-add-type-form">';
  $form['#suffix'] = '</div>';

  return $form;
}

/**
 * Builds step1 of the tripal_admin_add_type_form()
 */
function tripal_admin_add_type_form_step1(&$form, &$form_state) {

  // Get the term name from the form_state.
  $term_name = '';
  if (array_key_exists('values', $form_state) and array_key_exists('term_name0', $form_state['values'])) {
    $term_name = $form_state['values']['term_name0'];
  }
  if (array_key_exists('input', $form_state) and array_key_exists('term_name0', $form_state['input'])) {
    $term_name = $form_state['input']['term_name0'];
  }

  // Get the term lookup form.
  $description = t("The content type must be the name of a term in
        a controlled vocabulary and the controlled vocabulary should
        already be loaded into Tripal.  For example, to create a content
        type for storing 'genes', use the 'gene' term from the
        Sequence Ontology (SO).");
  tripal_get_term_lookup_form($form, $form_state, $term_name,
    'Step 1: Content Type', $description, TRUE, '', 0,
    'tripal_admin_add_type_form_ajax_callback');

  if ($term_name) {
    $form['term_match']['step1-continue'] = [
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#name' => 'step1-continue',
    ];
  }
}

/**
 * Provides a summary of values selected in Step 1.
 */
function tripal_admin_add_type_form_step1_summary(&$form, &$form_state, $selected_term) {

  $form['term'] = [
    '#type' => 'value',
    '#value' => $selected_term,
  ];

  $form['term_summary'] = [
    '#type' => 'fieldset',
    '#title' => t('Step 1: Content Type'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
  $definition = property_exists($selected_term, 'definition') ? $selected_term->definition : '';
  $form['term_summary']['details'] = [
    '#type' => 'item',
    '#title' => t('Term'),
    '#markup' => 'Name: ' . $selected_term->name .
      '<br>Vocabulary: ' . $selected_term->cv_id->name . ' (' . $selected_term->dbxref_id->db_id->name . ') ' .
      '<br>Term ID: ' . $selected_term->dbxref_id->db_id->name . ':' . $selected_term->dbxref_id->accession . '.  ' .
      '<br>Definition:  ' . $definition,
  ];
  $form['term_summary']['step1-return'] = [
    '#type' => 'submit',
    '#value' => t('Pick a different term'),
    '#name' => 'step1-return',
  ];
}

/**
 * Builds step1 of the tripal_admin_add_type_form()
 */
function tripal_admin_add_type_form_step2(&$form, &$form_state, $stores, $selected_term, $default_store) {

  // Now let the user select where the data type will be stored.
  $form['storage'] = [
    '#type' => 'fieldset',
    '#title' => t('Step 2: Storage'),
    '#description' => t('The primary record for each content of this type
          must be stored in a single storage backend. Please select the
          storage method and settings for this content type.'),
  ];

  $store_options = [0 => '-- Select --'];
  foreach ($stores as $store_type => $store) {
    $store_options[$store_type] = $store['label'];
  }

  $form['storage']['store_select'] = [
    '#type' => 'select',
    '#title' => 'Storage backend',
    '#options' => $store_options,
    '#default_value' => $default_store,
    '#description' => 'Select a storage background for this content type.',
  ];

  if ($default_store) {
    $form['term_match']['step2-continue'] = [
      '#type' => 'submit',
      '#value' => t('Continue'),
      '#name' => 'step2-continue',
    ];
  }
}

/**
 * Provides a summary of values selected in Step 1.
 */
function tripal_admin_add_type_form_step2_summary(&$form, &$form_state, $stores, $selected_term, $default_store) {
  $default_store = $form_state['values']['store_select'];
  $selected_store_module = $stores[$default_store]['module'];

  $form['store_select'] = [
    '#type' => 'value',
    '#value' => $default_store,
  ];

  $form['store_summary'] = [
    '#type' => 'fieldset',
    '#title' => t('Step 2: Storage'),
    '#description' => '',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
  $form['store_summary']['details'] = [
    '#type' => 'item',
    '#title' => t('Storage backend'),
    '#markup' => $stores[$default_store]['label'],
  ];
  $form['term_summary']['step1-return'] = [
    '#type' => 'submit',
    '#value' => t('Pick a different term'),
    '#name' => 'step1-return',
  ];
}

/**
 * Builds step1 of the tripal_admin_add_type_form()
 */
function tripal_admin_add_type_form_step3(&$form, &$form_state, $stores, $selected_term, $default_store) {
  $default_store = $form_state['values']['store_select'];

  $selected_store_module = $stores[$default_store]['module'];

  $form['store_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Step 3: Storage Settings'),
    '#description' => '',
  ];

  $function = $selected_store_module . '_field_storage_bundle_mapping_form';
  if (function_exists($function)) {
    $store_form = $function($form, $form_state, $selected_term, $submit_disabled);
    $form['store_settings'][$default_store] = $store_form;
  }

  // Add in the button for the cases of no terms or too many.
  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Create content type'),
    '#name' => 'create-content',
    '#disabled' => $submit_disabled,
  ];
}

/**
 * Implements an AJAX callback for the tripal_chado_vocab_select_term_form.
 */
function tripal_admin_add_type_form_ajax_callback($form, $form_state) {

  return $form;
}

/**
 * Implements hook_validate() for the tripal_admin_add_type_form.
 *
 */
function tripal_admin_add_type_form_validate($form, &$form_state) {
  $stores = module_invoke_all('vocab_storage_info');
  $store_select = (isset($form_state['values']['store_select'])) ? $form_state['values']['store_select'] : NULL;
  $clicked_button = $form_state['clicked_button']['#name'];

  // Don't do validation on an ajax callback.
  if (array_key_exists('#ajax', $form_state['triggering_element'])) {
    return;
  }

  if ($clicked_button == 'step1-continue') {
    $form_state['rebuild'] = TRUE;
    $form_state['stage'] = 'step2';

    $selected = tripal_get_term_lookup_form_result($form, $form_state);
    if (count($selected) == 0) {
      form_set_error('term_match][term_name', 'Please select a vocabulary term.');
    }
    if (count($selected) > 1) {
      form_set_error('term_match][term_name', 'Please select only one vocabulary term.');
    }

  }
  if ($clicked_button == 'step1-return') {
    $form_state['rebuild'] = TRUE;
    $form_state['stage'] = 'step1';
  }
  if ($clicked_button == 'step2-continue') {

    if (!$store_select) {
      form_set_error('store_select', 'Please select only one vocabulary term.');
    }

    $form_state['rebuild'] = TRUE;
    $form_state['stage'] = 'step3';
  }

  if ($clicked_button == 'create-content') {
    // Call the submit hook for this form for the storage method that
    // will be responsible for this cotent type.
    $stores = module_invoke_all('vocab_storage_info');
    $selected_store_module = $stores[$store_select]['module'];
    $selected_term = $form_state['values']['term'];

    $function = $selected_store_module . '_field_storage_bundle_mapping_form_validate';
    if (function_exists($function)) {
      $function($form, $form_state, $selected_term);
    }
  }
  else {
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Implements hook_submit() for the tripal_admin_add_type_form.
 */
function tripal_admin_add_type_form_submit($form, &$form_state) {
  $vocabulary = '';
  $accession = '';
  if (array_key_exists('term', $form_state['values'])) {
    $selected_term = $form_state['values']['term'];
    $store_select = $form_state['values']['store_select'];
    $vocabulary = $selected_term->dbxref_id->db_id->name;
    $accession = $selected_term->dbxref_id->accession;
    $term_name = $selected_term->name;

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $term = tripal_load_term_entity([
      'vocabulary' => $vocabulary,
      'accession' => $accession,
    ]);
    if (!$term) {

      // Call the submit hook for this form for the storage method that
      // will be responsible for this cotent type.
      $stores = module_invoke_all('vocab_storage_info');
      $selected_store_module = $stores[$store_select]['module'];

      $storage_args = [];
      $function = $selected_store_module . '_field_storage_bundle_mapping_form_submit';
      if (function_exists($function)) {
        $function($form, $form_state, $term, $storage_args);
      }

      $args = [
        'vocabulary' => $vocabulary,
        'accession' => $accession,
        'term_name' => $term_name,
        'storage_args' => $storage_args,
      ];

      global $user;
      $job_id = tripal_add_job("Create content type: " . $term_name . ' (' . $vocabulary . ':' . $accession . ')',
        'tripal', 'tripal_create_bundle', [$args], $user->uid);

      if (!$job_id) {
        drupal_set_message($error, 'error');
        $form_state['redirect'] = "admin/structure/bio_data";
      }
      else {
        drupal_set_message('After the content type is created, please ' . l("set the user permissions", "admin/people/permissions") . ' for this new content type.');
        $form_state['redirect'] = "admin/structure/bio_data";
      }
    }
    else {
      drupal_set_message("The term " . $term->name . " (" . $vocabulary . ':' . $accession . ") already exists as a content type.", 'warning');
    }
  }
}

/**
 * Checks access permissions for a given entity.
 */
function tripal_admin_access($entity) {

  if ($entity) {
    $bundle_name = $entity->name;
  }
  else {
    return FALSE;
  }

  // Get the bundle object.
  $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

  if (!$bundle) {
    tripal_report_error('tripal', TRIPAL_WARNING,
      'Unable to load bundle :name when creating permissions.', [':name' => $bundle_name]);
    return FALSE;
  }

  // Get the administrative user roles.
  $admin_role = NULL;
  $admin_rid = variable_get('user_admin_role');
  if (!$admin_rid) {
    // If we couldn't identify a single role from the 'user_admin_role' variable
    // then let's get the role that is currently set to administer tripal. If
    // there is more than one then we don't really know which to choose unless
    // the default rid of '3' is present.
    $admin_roles = user_roles(FALSE, 'administer tripal');
    if (count(array_keys($admin_roles)) == 1) {
      $admin_rid = key($admin_roles);
    }
    // The rid 3 is Drupal's default for the admin user.
    else {
      if (in_array(3, array_keys($admin_roles))) {
        $admin_rid = 3;
      }
    }
  }

  // If we can't find a unique admin role then just don't add one and
  // the user will be forced to manually set permissions for the admin.
  if (!$admin_rid) {
    return FALSE;
  }

  // Define the permissions.
  $permission_for_role = [
    'create ' . $bundle->name => TRUE,
    'view ' . $bundle->name => TRUE,
    'edit ' . $bundle->name => TRUE,
    'delete ' . $bundle->name => TRUE,
  ];

  // Assign the permissions
  user_role_change_permissions($admin_rid, $permission_for_role);

  return TRUE;
}
