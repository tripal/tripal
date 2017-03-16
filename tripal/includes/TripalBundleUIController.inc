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

  // TODO: the Chado specific function calls should not be here.  They need
  // to be genericized.
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
    $selected_term_id = '';
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
        $selected_term_id = $term->cvterm_id;
      }
      $num_terms++;
    }
    if ($num_terms == 0) {
      $form['terms_list']['none'] = array(
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>'
      );
    }
    $form['storage'] = array(
      '#type' => 'fieldset',
      '#title' => t('Storage Settings'),
      '#description' => t('The primary record for each content of this type
          must be stored in a single storage backend. Please select the
          storage method and settings for this content type.')
    );

    // TODO: we should have a default store setting so that user's have
    // fewer clicks.

    // TODO: there should be a way for each storage backend to determine if
    // it can handle the content type.  Maybe certain content types aren't
    // yet supported by every sotrage backend.
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
      if ($store_type == 'term_chado_storage') {
        // TODO: Move this Chado-specific code out of there.
        $default_table = '';
        if (array_key_exists('base_chado_table', $form_state['values'])
            and $form_state['values']['base_chado_table']) {
          $default_table = $form_state['values']['base_chado_table'];
        }
        else {
          $mapped_table = chado_get_cvterm_mapping(array('cvterm_id' => $selected_term_id));
          if ($mapped_table) {
            $default_table = $mapped_table->chado_table;
          }
        }

        $form['storage'][$store_type]['selected_cvterm_id'] = array(
          '#type' => 'value',
          '#value' => $selected_term_id,
        );

        $base_tables = chado_get_base_tables();
        $options = array(0 => '-- Select table --');
        foreach ($base_tables AS $tablename) {
          $options[$tablename] = $tablename;
        }
        $form['storage'][$store_type]['base_chado_table'] = array(
          '#type' => 'select',
          '#title' => 'Chado table',
          '#options' => $options,
          '#description' => 'Select the Chado table into which the primary records for this content type will be stored.',
          '#default_value' => $default_table,
          '#ajax' => array(
            'callback' => "tripal_admin_add_type_form_ajax_callback",
            'wrapper' => "tripal-vocab-select-form",
            'effect' => 'fade',
            'method' => 'replace'
          ),
        );
        $form_state['input']['base_chado_table'] = $default_table;

        if ($default_table) {

          // There are a few places in form below where the type columns
          // can be used to identify a record. So, we'll create the
          // select options here so it can be used multiple places.  We
          // only want to include in our column options those fields that
          // are foreign keys to the cvterm table
          $schema = chado_get_schema($default_table);
          $column_options = array('0' => '--Select column--');
          $cvt_fkeys = array_keys($schema['foreign keys']['cvterm']['columns']);
          foreach ($schema['fields'] as $column_name => $column_details) {
            if (in_array($column_name, $cvt_fkeys)) {
              $column_options[$column_name] = $column_name;
            }
          }

          $default_has_all = 'No';
          if (array_key_exists('chado_table_has_all', $form_state['values'])
              and $form_state['values']['chado_table_has_all']) {
            $default_has_all = $form_state['values']['chado_table_has_all'];
          }
          $form['storage'][$store_type]['chado_table_has_all'] = array(
            '#type' => 'radios',
            '#options' => array(
              'Yes' => 'Yes',
              'No' => 'No'
            ),
            '#title' => 'Are all records in the "' . $default_table .
              '" table of type "'. $term_name . '"?',
            '#description' => 'Select "Yes" if the "' .
              $default_table . '" table houses more that just data of type "' .
              $term_name . '".',
            '#default_value' => $default_has_all,
            '#ajax' => array(
              'callback' => "tripal_admin_add_type_form_ajax_callback",
              'wrapper' => "tripal-vocab-select-form",
              'effect' => 'fade',
              'method' => 'replace'
            ),
          );

          // If the table does not contain just one single type of record.
          if ($default_has_all == 'No') {

            $default_type_column = '';
            if (array_key_exists('type_id', $schema['fields'])) {
              $default_type_column = 'type_id';
            }

            // If the table doesn't have an obvious type_id column then
            // let the user specify how record types are identified.
            if ($default_type_column) {

              $default_has_type = 'Yes';
              if (array_key_exists('chado_table_has_type', $form_state['values'])
                  and $form_state['values']['chado_table_has_type']) {
                $default_has_type = $form_state['values']['chado_table_has_type'];
              }
              $form['storage'][$store_type]['chado_table_has_type'] = array(
                '#type' => 'radios',
                '#title' => 'Do you want to use the "' . $default_table . '.' .
                   $default_type_column . '" to distinguish between content types?',
                '#options' => array(
                  'Yes' => 'Yes',
                  'No' => 'No'
                ),
                '#description' => t('Many tables in Chado use a "type_id" column
                    to identify specific data types. For example, the "feature.type_id"
                    column is used to distinguish between different types of
                    genomic features that are all housed in the feature table.
                    But there is sometimes a cvterm linker table where additional
                    attributes for the record can be set.  Here you can specify if you
                    want to use the type_id column or select another way
                    to identify records in the "' . $default_table . '" table that
                    are of this type'),
                '#default_value' => $default_has_type,
                '#ajax' => array(
                  'callback' => "tripal_admin_add_type_form_ajax_callback",
                  'wrapper' => "tripal-vocab-select-form",
                  'effect' => 'fade',
                  'method' => 'replace'
                ),
              );

              // If the type_id column is not wanted to differentiate between
              // different records of this type then find out if the user wants
              // to use a linker cvterm table, but only if one exists
              $linker_table = $default_table . '_cvterm';
              if ($default_has_type == "Yes") {
                $submit_disabled = FALSE;
              }
              else if ($default_has_type == 'No' and chado_table_exists($linker_table)) {
                $default_use_linker = 'Yes';
                if (array_key_exists('chado_type_use_linker', $form_state['values'])
                    and $form_state['values']['chado_type_use_linker']) {
                  $default_use_linker = $form_state['values']['chado_type_use_linker'];
                }
                $form['storage'][$store_type]['chado_type_use_linker'] = array(
                  '#type' => 'radios',
                  '#title' => 'Do you want to use the "' . $linker_table . '" table
                     to distinguish between content types?',
                  '#options' => array(
                    'Yes' => 'Yes',
                    'No' => 'No'
                  ),
                  '#description' => t('Sometimes records can be distringuish
                      using a linker table, especially if there is no column
                      in the specified Chado table to identify the
                      record types. In these cases the linker table is
                      sometimes used.'),
                  '#default_value' => $default_use_linker,
                  '#ajax' => array(
                    'callback' => "tripal_admin_add_type_form_ajax_callback",
                    'wrapper' => "tripal-vocab-select-form",
                    'effect' => 'fade',
                    'method' => 'replace'
                  ),
                );

                if($default_use_linker == 'No') {
                  if (count(array_keys($column_options)) == 1) {
                    $form['storage'][$store_type]['type_column'] = array(
                      '#type' => 'item',
                      '#title' => 'Type Column',
                      '#description' => 'The  "' . $default_table .
                        '" table does not have a column
                        that stores data types. You cannot map the "' .
                        $term_name . '" type to this table.',
                    );
                  }
                  else {
                    $form['storage'][$store_type]['type_column'] = array(
                      '#type' => 'select',
                      '#title' => 'Type Column',
                      '#options' => $column_options,
                      '#description' => 'Please select the column in the "' .
                        $default_table . '" table that will identify a record
                        as being of type "' . $term_name . '".',
                      '#default_value' => $default_type_column,
                      '#ajax' => array(
                        'callback' => "tripal_admin_add_type_form_ajax_callback",
                        'wrapper' => "tripal-vocab-select-form",
                        'effect' => 'fade',
                        'method' => 'replace'
                      ),
                    );
                    $submit_disabled = FALSE;
                  }
                } // end if(!$default_use_linker) {
                else {
                  $submit_disabled = FALSE;
                }
              } // end if ($default_has_type == 'No' and ...
            } // end else if ($default_type_column) {
            else {
              // If the type_id column is not wanted to differentiate between
              // different records of this type then find out if the user wants
              // to use a linker table, but only if one exists
              $linker_table = $default_table . '_cvterm';
              if (chado_table_exists($linker_table)) {
                $default_use_linker = 'Yes';
                if (array_key_exists('chado_type_use_linker', $form_state['values'])
                    and $form_state['values']['chado_type_use_linker']) {
                  $default_use_linker = $form_state['values']['chado_type_use_linker'];
                }
                $form['storage'][$store_type]['chado_type_use_linker'] = array(
                  '#type' => 'radios',
                  '#title' => 'Do you want to use the "' . $linker_table . '" table
                     to distinguish between content types?',
                  '#options' => array(
                    'Yes' => 'Yes',
                    'No' => 'No'
                  ),
                  '#description' => t('Sometimes records can be distringuish
                      using a linker, especially if there is no column
                      in the specified Chado table to identify the
                      record types. In these cases the linker table is
                      sometimes used.'),
                  '#default_value' => $default_use_linker,
                  '#ajax' => array(
                    'callback' => "tripal_admin_add_type_form_ajax_callback",
                    'wrapper' => "tripal-vocab-select-form",
                    'effect' => 'fade',
                    'method' => 'replace'
                  ),
                );

                if($default_use_linker == 'No') {
                  if (count(array_keys($column_options)) == 1) {
                    $form['storage'][$store_type]['type_column'] = array(
                      '#type' => 'item',
                      '#title' => 'Type Column',
                      '#description' => 'The  "' . $default_table .
                        '" table does not have a column
                        that stores data types. You cannot map the "' .
                        $term_name . '" type to this table.',
                    );
                  }
                  else {
                    $form['storage'][$store_type]['type_column'] = array(
                      '#type' => 'select',
                      '#title' => 'Type Column',
                      '#options' => $column_options,
                      '#description' => 'Please select the column in the "' .
                        $default_table . '" table that will identify a record
                        as being of type "' . $term_name . '".',
                      '#default_value' => $default_type_column,
                      '#ajax' => array(
                        'callback' => "tripal_admin_add_type_form_ajax_callback",
                        'wrapper' => "tripal-vocab-select-form",
                        'effect' => 'fade',
                        'method' => 'replace'
                      ),
                    );
                    $submit_disabled = FALSE;
                  }
                } // end if(!$default_use_linker) {
                else {
                  $submit_disabled = FALSE;
                }
              } // end if ($default_has_type == 'No' and ...
            } // end else
          } //  end if ($default_has_all == 'No') {
          else {
            $submit_disabled = FALSE;
          }
        } // end if ($default_table) {
      } // end if ($default_store) {
      // Add in the button for the cases of no terms or too many.
      $form['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Create content type'),
        '#name' => 'use_cvterm',
        '#disabled' => $submit_disabled,
      );
    }
  }

  $form['#prefix'] = '<div id = "tripal-vocab-select-form">';
  $form['#suffix'] = '</div>';


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
  $keys = array_keys($stores);
  $module = $stores[$keys[0]]['module'];
  $function = $module . '_vocab_select_term_form_validate';
  if (function_exists($function)) {
    $function($form, $form_state);
  }

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
      $form_state['term']['vocabulary'] = $term->dbxref_id->db_id->name;
      $form_state['term']['accession'] = $term->dbxref_id->accession;
      $form_state['term']['term_name'] = $term->name;

      // Make sure a default table is selected
      // TODO: move this Chado specific code out.
      $default_table = $form_state['values']['base_chado_table'];
      if (!$default_table) {
        form_set_error('base_chado_table', 'Please select a default table.');
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
    $vocabulary = array_key_exists('vocabulary', $selected_term) ? $selected_term['vocabulary'] : '';
    $accession = array_key_exists('accession', $selected_term) ? $selected_term['accession'] : '';
    $term_name = array_key_exists('term_name', $selected_term) ? $selected_term['term_name'] : '';

    // Before we try to add this type, check to see if it already exists
    // as a bundle.
    $term = tripal_load_term_entity(array('vocabulary' => $vocabulary, 'accession' => $accession));
    if (!$term) {
      $error = '';
      // TODO: the storage args are Chado specific. This should be made
      // more generic to support other back-ends.
      $storage_args = array();
      if (array_key_exists('base_chado_table', $form_state['values'])) {
        $storage_args['data_table'] = $form_state['values']['base_chado_table'];
      }
      if (array_key_exists('chado_type_use_linker', $form_state['values']) and
          $form_state['values']['chado_type_use_linker'] == 'Yes') {
        $storage_args['type_linker_table'] = $form_state['values']['base_chado_table'] . '_cvterm';
        $storage_args['type_column'] = 'cvterm_id';
      }
      if (array_key_exists('chado_table_has_type', $form_state['values']) and
          $form_state['values']['chado_table_has_type'] == 'Yes') {
        $storage_args['type_column'] = 'type_id';
      }
      if (array_key_exists('type_column', $form_state['values']) and
          $form_state['values']['type_column']) {
        $storage_args['type_column'] = $form_state['values']['type_column'];
      }

      $storage_args['type_id'] = $form_state['values']['selected_cvterm_id'];
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
        drupal_set_message('New data type created.');
        tripal_admin_access($bundle);
        $form_state['redirect'] = "admin/structure/bio_data";
      }

      // TODO: move these Chado specific settings out.


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
