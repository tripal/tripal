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
      'access arguments' => array('manage tripal content types'),
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
  
  /**
   * Renders the Bundle overview table
   */
  public function overviewTable($conditions = array()) {
    $entities = entity_load($this->entityType, FALSE, $conditions);

    // Sort the entities by label.
    $sorted = [];    
    foreach ($entities as $entity) {
      $sorted[$entity->label] = $entity;
    }
    ksort($sorted, SORT_STRING|SORT_FLAG_CASE);
    
    $rows = array();
    foreach ($sorted as $entity) {
      // Get the term for this content type
      $additional_cols = [$entity->term->name . ' (' . l($entity->accession, 'cv/lookup/' . $entity->term->vocab->vocabulary . '/' . $entity->term->accession) . ')'];
      $rows[] = $this->overviewTableRow($conditions, 
        entity_id($this->entityType, $entity), $entity,
        $additional_cols);
    }
    // Assemble the right table header.
    $header = array(t('Label'));
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }
    $header[] = array(
      'data' => t('Term'),
    );
    // Add operations with the right colspan.
    $field_ui = !empty($this->entityInfo['bundle of']) && module_exists('field_ui');
    $exportable = !empty($this->entityInfo['exportable']);
    $colspan = 3;
    $colspan = $field_ui ? $colspan + 2 : $colspan;
    $colspan = $exportable ? $colspan + 1 : $colspan;
    $header[] = array(
      'data' => t('Operations'),
      'colspan' => $colspan,
    );
    
    $render = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => t('None.'),
    );
    return $render;
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
          'data' => 'Term Name',
          'class' => array('side-header')
        ),
        $term->name
      ),
      array(
        array(
          'header' => TRUE,
          'data' => 'Accession',
          'class' => array('side-header')
        ),
        $term->accession
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
    $form['description']['#default_value'] = tripal_get_bundle_variable('description', $bundle->id, $term->definition);
  }
  else {
    $form['description']['#default_value'] = tripal_get_bundle_variable('description', $bundle->id, '');
  }

  $empty_fields = tripal_get_bundle_variable('hide_empty_field', $bundle->id, '');
  $form['hide_empty_field'] = array(
    '#type' => 'select',
    '#title' => t('Field Display'),
    '#options' => array(
      'hide' => t('Hide empty fields'),
      'show' => t('Show empty fields'),
    ),
    '#description' => t('Choose either to show or hide all empty fields.  If "Show empty fields" is selected then fields will be loaded via AJAX to help speed page loads.'),
    '#default_value' => !empty($empty_fields) ? array($empty_fields,) : array('hide',),
  );

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
  $form['set_titles']['bp_explanation'] = array(
    '#type' => 'item',
    '#markup' => t('Retroactively apply the new title pattern to
        existing content by clicking the button below.',
      array('%type' => $bundle->label)),
  );
  $form['set_titles']['bulk_update'] = array(
    '#type' => 'submit',
    '#value' => t('Bulk update all titles'),
    //'#submit' => array('tripal_bulk_update_submit'),
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


  $tokens = tripal_get_entity_tokens($bundle);
  $form['url']['tokens'] = array(
    '#type' => 'hidden',
    '#value' => serialize($tokens)
  );

  $form['url']['token_display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#description' => t('Copy the token and paste it into the "URL Alias Pattern" ' . 
      'text field above. Please choose tokens that will guarantee a unique URL.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $form['url']['token_display']['content'] = array(
    '#type' => 'item',
    '#markup' => theme_token_list($tokens),
  );
  $form['url']['bp_explanation'] = array(
    '#type' => 'item',
    '#markup' => t('Retroactively apply the new url alias pattern to
        existing content by clicking the button below.',
      array('%type' => $bundle->label)),
  );
  $form['url']['bulk_update'] = array(
    '#type' => 'submit',
    '#value' => t('Bulk update all aliases'),
    //'#submit' => array('tripal_bulk_update_submit'),
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
  // VALIDATE: That there is a value passed for the hide_empty_field option. If
  // no value passed default to hide field.
  if(empty($form_state['values']['hide_empty_field'])){
    $form_state['values']['hide_empty_field'] = 'hide';
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

    // Save the page title format.
    tripal_save_title_format(
      $bundle,
      $form_state['values']['set_titles']['title_format']
    );

    // Save the URL alias pattern if it's set.
    if ($form_state['values']['url']['url_pattern']) {
      tripal_set_bundle_variable('url_format', $bundle->id, $form_state['values']['url']['url_pattern']);
    }

    // There are two submit buttons for this either updating the paths or the
    // titles.
    if ($trigger == 'Bulk update all titles') {
      $update = $form_state['input']['set_titles']['title_format'];
      $type = 'title';
      $args = array(
        'bundle_id' => $bundle->name,
        'update' => $update,
        'type' => $type
      );
      $includes = array(
        module_load_include('inc', 'tripal', 'includes/tripal.bulk_update'),
      );
      tripal_add_job('Update all aliases for content type: ' . $bundle->label, 'tripal', 
        'tripal_update_all_urls_and_titles', $args, $user->uid, 10, $includes);
    }
    elseif ($trigger == 'Bulk update all aliases'){
      $update = $form_state['input']['url']['url_pattern'];
      $type = 'alias';
      $args = array(
        'bundle_id' => $bundle->name,
        'update' => $update,
        'type' => $type
      );
      $includes = array(
        module_load_include('inc', 'tripal', 'includes/tripal.bulk_update'),
      );
      tripal_add_job('Update all aliases for content type: ' . $bundle->label, 'tripal', 
        'tripal_update_all_urls_and_titles', $args, $user->uid, 10, $includes);
    }

    $form_state['redirect'] = 'admin/structure/bio_data';
    drupal_set_message(t('Successfully saved %type content type.', array('%type' => $form_state['build_info']['args'][0]->label)));
  }

  else {
    $form_state['redirect'] = array(
      'admin/structure/bio_data/manage/' . $bundle->name . '/delete',
      array('query' => array('destination' => 'admin/structure/bio_data'))
    );
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

  $stores = module_invoke_all('vocab_storage_info');
  if (!is_array($stores) or count($stores) == 0) {
    tripal_set_message('A storage backend is not enabled for managing
          the vocabulary terms used to create content.  Please enable
          a module that supports storage of vocabualary terms (e.g. tripal_chado)
          and return to create new Tripal content types.', TRIPAL_NOTICE);
    return;
  }
  $keys = array_keys($stores);
  $module = $stores[$keys[0]]['module'];
  $function = $module . '_vocab_select_term_form';
  if (function_exists($function)) {
    $form = $function($form, $form_state);
  }

  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

  // If no term has been selected yet then provide the auto complete field.
  $form['term_name'] = array(
    '#title'       => t('Content Type'),
    '#type'        => 'textfield',
    '#description' => t("The content type must be the name of a term in
        a controlled vocabulary and the controlled vocabulary should
        already be loaded into Tripal.  For example, to create a content
        type for storing 'genes', use the 'gene' term from the
        Sequence Ontology (SO)."),
    '#required'    => TRUE,
    '#default_value' => $term_name,
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
  );
  $form['select_button'] = array(
    '#type' => 'submit',
    '#value' => t('Lookup Term'),
    '#name' => 'select_cvterm',
    '#ajax' => array(
      'callback' => "tripal_admin_add_type_form_ajax_callback",
      'wrapper' => "tripal-vocab-select-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  $form['#prefix'] = '<div id = "tripal-vocab-select-form">';
  $form['#suffix'] = '</div>';

  // If the term has been provided by the user then we want to search for
  // matching terms in the database and let them select among any matches.
  if ($term_name) {
    $submit_disabled = TRUE;
    $form['terms_list'] = array(
      '#type' => 'fieldset',
      '#title' => t('Matching Terms'),
      '#description' => t('Please select the term the best matches the
          content type you want to create. If the same term exists in
          multiple vocabularies you will see more than one option below.')
    );
    $match = array(
      'name' => $term_name,
    );
    $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = 0;
    $selected_term = '';

    // Let the user select from any matching terms. Sometimes there may be
    // more than one that match.
    foreach ($terms as $term) {
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term. 
      $default = FALSE;
      $attrs = array();
      if ($num_terms == 0 and count($terms) == 1) {
        $default = TRUE;
        $attrs = array('checked' => 'checked');
      }
      $term_element_name = 'term-' . $term->cvterm_id;
      $form['terms_list'][$term_element_name] = array(
        '#type' => 'checkbox',
        '#title' =>  $term->name,
        '#default_value' => $default,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
        '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
        '<br><b>Definition:</b>  ' . $term->definition,
        '#ajax' => array(
          'callback' => "tripal_admin_add_type_form_ajax_callback",
          'wrapper' => "tripal-vocab-select-form",
          'effect' => 'fade',
          'method' => 'replace'
        ),
      );

      if (array_key_exists('values', $form_state) and array_key_exists($term_element_name, $form_state['values']) and
          $form_state['values'][$term_element_name] == 1) {
        $selected_term = $term;
      }
      $num_terms++;
    }
    
    
    // Next find terms that are synonyms
    $match = array(
      'synonym' => $term_name,
    );
    $termsyn = chado_generate_var('cvtermsynonym', $match, array('return_array' => TRUE));
    // Let the user select from any matching terms. Sometimes there may be
    // more than one that match.
    foreach ($termsyn as $synonym) {
      $term = $synonym->cvterm_id;
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term.
      $default = FALSE;
      $attrs = array();
      if ($num_terms == 0 and count($terms) == 1) {
        $default = TRUE;
        $attrs = array('checked' => 'checked');
      }
      $term_element_name = 'term-' . $term->cvterm_id;
      $form['term_match']['terms_list'][$term_element_name] = array(
        '#type' => 'checkbox',
        '#title' =>  $term->name,
        '#default_value' => $default,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
        '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
        '<br><b>Definition:</b>  ' . $term->definition .
        '<br><b>Synonym:</b> ' . $synonym->synonym,
        '#ajax' => array(
          'callback' => "tripal_admin_add_type_form_ajax_callback",
          'wrapper' => "tripal-vocab-select-form",
          'effect' => 'fade',
          'method' => 'replace'
        ),
      );
      
      if (array_key_exists('values', $form_state) and array_key_exists($term_element_name, $form_state['values']) and
        $form_state['values'][$term_element_name] == 1) {
          $selected_term = $term;
        }
        $num_terms++;
    }
    if ($num_terms == 0) {
      $form['terms_list']['none'] = array(
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>'
      );
      return $form;
    }

    // Now let the user select where the data type will be stored.
    $form['storage'] = array(
      '#type' => 'fieldset',
      '#title' => t('Storage Settings'),
      '#description' => t('The primary record for each content of this type
          must be stored in a single storage backend. Please select the
          storage method and settings for this content type.')
    );

    // TODO: there should be a way for each storage backend to determine if
    // it can handle the content type.  Maybe certain content types aren't
    // yet supported by every storage backend.
    $default_store = 'term_chado_storage';
    $store_options = array(0 => '-- Select --');
    foreach ($stores as $store_type => $store) {
      $store_options[$store_type] = $store['label'];
    }
    if (array_key_exists('values', $form_state) and
        array_key_exists('store_select', $form_state['values'])) {
      $default_store = $form_state['values']['store_select'];
    }
    $form['storage']['store_select'] = array(
      '#type' => 'select',
      '#title' => 'Storage backend',
      '#options' => $store_options,
      '#default_value' => $default_store,
      '#ajax' => array(
        'callback' => "tripal_admin_add_type_form_ajax_callback",
        'wrapper' => "tripal-vocab-select-form",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#description' => 'Select a storage background for this content type.'
    );

    if ($default_store) {
      $selected_store_module = $stores[$store_type]['module'];

      $function = $selected_store_module . '_field_storage_bundle_mapping_form';
      if (function_exists($function)) {
        $store_form = $function($form, $form_state, $selected_term, $submit_disabled);
        $form['storage'][$store_type] = $store_form;
      }

      // Add in the button for the cases of no terms or too many.
      $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Create content type'),
        '#name' => 'use_cvterm',
        '#disabled' => $submit_disabled,
      );
    }
  }


  return $form;
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

  if (array_key_exists('clicked_button', $form_state) and
      $form_state['clicked_button']['#name'] =='use_cvterm') {

    $cvterm_id = NULL;

    // Make sure we have a cvterm selected
    $num_selected = 0;
    foreach ($form_state['values'] as $key => $value) {
      $matches = array();
      if (preg_match("/^term-(\d+)$/", $key, $matches) and
          $form_state['values']['term-' . $matches[1]]) {
        $cvterm_id = $matches[1];
        $term = chado_generate_var('cvterm', array('cvterm_id' => $cvterm_id));
        $num_selected++;
      }
    }

    if ($num_selected == 0) {
      form_set_error('', 'Please select at least one term.');
    }
    else if ($num_selected > 1) {
      form_set_error('term-' . $cvterm_id, 'Please select only one term from the list below.');
    }
    else {
      // Add the term to the form state so we can access it later.
      $form_state['term']['vocabulary'] = $term->dbxref_id->db_id->name;
      $form_state['term']['accession'] = $term->dbxref_id->accession;
      $form_state['term']['term_name'] = $term->name;

      // Call the submit hook for this form for the storage method that
      // will be responsible for this cotent type.
      $stores = module_invoke_all('vocab_storage_info');
      $selected_store_module = $stores[$store_select]['module'];
      $selected_term = $form_state['term'];

      $function = $selected_store_module . '_field_storage_bundle_mapping_form_validate';
      if (function_exists($function)) {
        $function($form, $form_state, $selected_term);
      }
    }
  }
  // For any other button click it's an AJAX call and we just want to reubild
  // the form.
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
  if (array_key_exists('term', $form_state)) {
    $selected_term = $form_state['term'];
    $store_select = $form_state['values']['store_select'];
    $vocabulary = array_key_exists('vocabulary', $selected_term) ? $selected_term['vocabulary'] : '';
    $accession = array_key_exists('accession', $selected_term) ? $selected_term['accession'] : '';
    $term_name = array_key_exists('term_name', $selected_term) ? $selected_term['term_name'] : '';

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $term = tripal_load_term_entity(array('vocabulary' => $vocabulary, 'accession' => $accession));
    if (!$term) {

      // Call the submit hook for this form for the storage method that
      // will be responsible for this cotent type.
      $stores = module_invoke_all('vocab_storage_info');
      $selected_store_module = $stores[$store_select]['module'];

      $storage_args = array();
      $function = $selected_store_module . '_field_storage_bundle_mapping_form_submit';
      if (function_exists($function)) {
        $function($form, $form_state, $term, $storage_args);
      }

      $args = array(
        'vocabulary' => $vocabulary,
        'accession' => $accession,
        'term_name' => $term_name,
        'storage_args' => $storage_args,
      );

      $bundle = tripal_create_bundle($args, $error);
      if (!$bundle) {
        drupal_set_message($error, 'error');
        $form_state['redirect'] = "admin/structure/bio_data";
      }
      else {
        drupal_set_message('New content type created!');
        drupal_set_message('Please ' . l("set the user permissions", "admin/people/permissions") . ' for this new content type.');
        tripal_admin_access($bundle);
        $form_state['redirect'] = "admin/structure/bio_data";
      }
    }
    else {
      drupal_set_message("The term '$accession' already exists as a content type.", 'warning');
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
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));

  // Identify the administrative user roles.
  $admin_role = user_role_load_by_name('administrator');
  $roles = array($admin_role->rid => $admin_role->name);
  // Define the permissions.
  $permission_for_role = array(
    'create ' . $bundle->name => TRUE,
    'view ' . $bundle->name => TRUE,
    'edit ' . $bundle->name => TRUE,
    'delete ' . $bundle->name => TRUE,
  );

  // Assign the permissions
  foreach($roles as $role => $value){
    user_role_change_permissions($role, $permission_for_role);
  }

  return TRUE;
}
