<?php


/**
 *
 */
function tripal_cv_admin_set_defaults_form($form, &$form_state) {

  $form['instructions'] = [
    '#markup' => t('Much of the data housed in Chado is typed, meaning that a ' .
      'controlled vocabulary describes what type of data the record is. For example, ' .
      'a feature must have a "type" which is typically a term from ' .
      'the Sequence Ontology. Record properties typically have a type as well. ' .
      'Tripal allows the administrator to set a default type for each table in ' .
      'Chado that requires a type from a vocabulary. By default, autocomplete fields, ' .
      'type select boxes and type validation occur using the default vocabularies set below. '),
  ];

  // get the list of all tables that use the cvterm table as an FK
  $cvterm_schema = chado_get_schema('cvterm');
  $referring_tables = $cvterm_schema['referring_tables'];

  // get the list of tables that already have default set
  $cv_defaults = db_select('tripal_cv_defaults', 'TCD')
    ->fields('TCD', ['cv_default_id', 'table_name', 'field_name', 'cv_id'])
    ->orderBy('table_name', 'ASC')
    ->execute();

  // get the list of vocabularies
  $cvs = tripal_get_cv_select_options();

  $form['settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Configured Defaults'),
    '#description' => t('The following tables have a default vocabulary'),
    '#tree' => TRUE,
  ];
  foreach ($cv_defaults as $cv_default) {
    $cv_default_id = $cv_default->cv_default_id;
    $cv = tripal_get_cv(['cv_id' => $cv_default->cv_id]);

    $form['settings']['existing'][$cv_default_id]["id"] = [
      '#type' => 'hidden',
      '#value' => $cv_default_id,
    ];

    // Display
    $form['settings']['existing'][$cv_default_id]["table_name-display"] = [
      '#type' => 'markup',
      '#markup' => $cv_default->table_name,
    ];
    $form['settings']['existing'][$cv_default_id]["field_name-display"] = [
      '#type' => 'markup',
      '#markup' => $cv_default->field_name,
    ];

    // Save for use in submit
    $form['settings']['existing'][$cv_default_id]["table_name"] = [
      '#type' => 'hidden',
      '#value' => $cv_default->table_name,
    ];
    $form['settings']['existing'][$cv_default_id]["field_name"] = [
      '#type' => 'hidden',
      '#value' => $cv_default->field_name,
    ];

    // Selectbox to set the vocabulary
    if (!empty($cv)) {
      $default_cv = $cv_default->cv_id;
    }
    else {
      $cvs[0] = 'NONE SET';
      $default_cv = 0;
    }
    $form['settings']['existing'][$cv_default_id]["vocabulary"] = [
      '#type' => 'select',
      '#options' => $cvs,
      '#default_value' => $default_cv,
    ];

    // Actions
    $view_terms = l('New Vocabulary', 'admin/tripal/loaders/chado_vocabs/chado_cv/add');
    $add_term = '';
    if (!empty($cv)) {
      $view_terms = l(
        'View Terms',
        'admin/tripal/loaders/chado_vocabs/chado_cvterms',
        ['query' => ['cv' => $cv->name]]
      );

      $add_term = l(
        'Add Term',
        'admin/tripal/loaders/chado_vocabs/chado_cv/' . $cv->cv_id . '/cvterm/add'
      );
    }
    $form['settings']['existing'][$cv_default_id]["view-terms"] = [
      '#type' => 'markup',
      '#markup' => $view_terms,
    ];
    $form['settings']['existing'][$cv_default_id]["add-new-term"] = [
      '#type' => 'markup',
      '#markup' => $add_term,
    ];

  }

  $form['settings']['submit'] = [
    '#type' => 'submit',
    '#value' => 'Update Defaults',
  ];

  // Adding new CV Defaults
  $form['new'] = [
    '#type' => 'fieldset',
    '#title' => 'Add New Defaults',
    '#description' => 'You can use the form below to add a default controlled vocabulary
      for a table/field combination not available in the "Configured Defaults" section above.',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#prefix' => '<div id="new-default">',
    '#suffix' => '</div>',
  ];

  $tripal_msg = tripal_set_message(
    'If you are developing a custom module and would like to use the Default Controlled
      Vocabulary API to flexibly set the controlled vocabulary, then it is better to set
      the default programatically rather than through this interface. To do this <ol>
        <li>Tell Tripal about the table/field you would like to set the default for. This
          is done by implementing hook_install() in your modules .install file and adding
          a call to <code>tripal_set_default_cv([table name], [field name], [cv name])</code> which
          will set the default for <code>[table name].[field name]</code> to the <code>[cv name]</code> controlled
          vocabulary. This vocabulary must already exist.</li>
        <li>Then everywhere in your module that you need to know the controlled vocabulary,
          you call <code>tripal_get_default_cv([table name], [field name])</code> which will return
          an object describing the set default controlled vocabulary or call
          <code>tripal_get_cvterm_default_select_options([table name], [field name], [field friendly name])</code>
          if you would like an array of options for use in a select or autocomplete form element.</li></ol>
      ',
    TRIPAL_NOTICE,
    ['return_html' => TRUE]
  );

  $form['new']['instructions'] = [
    '#type' => 'markup',
    '#markup' => $tripal_msg,
  ];

  $chado_tables = chado_get_table_names(TRUE);
  $chado_tables[0] = 'Select a Table';
  $form['new']['table'] = [
    '#type' => 'select',
    '#title' => 'Table Name',
    '#description' => 'The name of the table you would like to set a controlled vocabulary default for.',
    '#options' => $chado_tables,
    '#default_value' => 0,
    '#ajax' => [
      'callback' => 'tripal_cv_admin_ajax_new_default_field_callback',
      'wrapper' => 'new-default',
    ],
  ];

  $table = (isset($form_state['values']['new']['table'])) ? $form_state['values']['new']['table'] : FALSE;
  $columns = ['Select a Field'];
  if ($table) {
    // get the table description
    $table_desc = chado_get_schema($table);
    if (isset($table_desc['foreign keys']['cvterm'])) {
      foreach ($table_desc['foreign keys']['cvterm']['columns'] as $left_column => $right_column) {
        $columns[$left_column] = $left_column;
      }
    }
  }
  $form['new']['field'] = [
    '#type' => 'select',
    '#title' => 'Field Name',
    '#description' => 'The name of the field you would like to set a controlled vocabulary default for.',
    '#options' => $columns,
    '#default_value' => 0,
  ];

  $cvs[0] = 'Select a Vocabulary';
  $form['new']['vocabulary'] = [
    '#type' => 'select',
    '#title' => 'Vocabulary',
    '#description' => 'The default controlled vocabulary you would like to set for this field.',
    '#options' => $cvs,
    '#default_value' => 0,
  ];

  $form['new']['add_new'] = [
    '#type' => 'submit',
    '#value' => 'Set New Default',
  ];

  return $form;
}

function tripal_cv_admin_set_defaults_form_submit($form, $form_state) {

  if ($form_state['triggering_element']['#value'] == 'Update Defaults') {
    foreach ($form_state['values']['settings']['existing'] as $default_cv) {
      if (!empty($default_cv['vocabulary'])) {
        tripal_set_default_cv(
          $default_cv['table_name'],
          $default_cv['field_name'],
          '', // We are passing in the cv_id so we don't need the name
          $default_cv['vocabulary']
        );
      }
    }
  }

  if ($form_state['triggering_element']['#value'] == 'Set New Default') {
    if (!empty($form_state['values']['new']['vocabulary'])) {
      tripal_set_default_cv(
        $form_state['values']['new']['table'],
        $form_state['values']['new']['field'],
        '', // We are passing in the cv_id so we don't need the name
        $form_state['values']['new']['vocabulary']
      );
    }
  }
}

function tripal_cv_admin_ajax_new_default_field_callback($form, $form_state) {
  return $form['new'];
}

/**
 *
 * @param unknown $variables
 */
function theme_tripal_cv_admin_set_defaults_form($variables) {
  $element = $variables['element'];

  $header = [
    'table_name' => ['data' => t('Table Name'), 'width' => '20%'],
    'field_name' => ['data' => t('Field Name'), 'width' => '20%'],
    'vocabulary' => ['data' => t('Default Vocabulary'), 'width' => '30%'],
    'actions' => ['data' => t('Actions'), 'width' => '30%'],
  ];

  $rows = [];

  foreach ($element['settings']['existing'] as $key => $value) {
    if (is_numeric($key)) {

      //       $action_links = '<ul class="links inline">';
      //       if (!empty($value['view-terms'])) {
      //         $action_links .= '<li>' . drupal_render($value['view-terms']) . '</li>';
      //       }
      //       if (!empty($value['add-new-term'])) {
      //         $action_links .= '<li>' . drupal_render($value['add-new-term']) . '</li>';
      //       }
      //       $action_links .= '</li></ul>';

      $rows[] = [
        drupal_render($value['table_name-display']),
        drupal_render($value['field_name-display']),
        drupal_render($value['vocabulary']),
        $action_links,
      ];
    }
  }
  $settings_table = theme('table', [
    'header' => $header,
    'rows' => $rows,
  ]);
  $element['settings']['existing'] = [
    '#type' => 'markup',
    '#markup' => $settings_table,
  ];

  // TODO: I believe rendering of the form should not happen here. But rather
  // the form should be returned as is. This way other modules can have access
  // to the form elements via the hook_form_alter.  Rather, there should
  // be a theme function for the form where the rendering in the table
  // should occur.  See the tripal_pub_importer_setup_form() for an exmaple.
  return drupal_render_children($element);

}