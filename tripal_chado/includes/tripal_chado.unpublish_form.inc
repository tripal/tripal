<?php

/**
 * Unpublish orphaned entities form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_unpublish_form($form, &$form_state) {
  $bundles = tripal_chado_get_orphaned_bundles();

  drupal_set_title('Unpublish Orphaned Content');
  
  $form['description'] = [
    '#type' => 'markup',
    '#markup' => t('Sometimes published content can become orphaned. This can
      occur if someone deletes records directly from the underlying data store
      yet Tripal is not aware of it.  Here, you can unpublish orphaned content
      '),
  ];

  if (empty($bundles)) {
    $form['message'] = [
      '#type' => 'markup',
      '#markup' => t("No orphaned content detected."),
    ];

    return $form;
  }

  $form['bundles'] = [
    '#title' => t('Content type'),
    '#type' => 'select',
    '#options' => $bundles,
    '#empty_option' => t('-- Select a Content Type --'),
    '#description' => t('Select a Tripal content type to find orphaned content.'),
    '#ajax' => [
      'callback' => 'tripal_chado_unpublish_form_callback',
      'wrapper' => 'bundle_info_fieldset_wrapper',
    ],
  ];

  $form['bundle_info_fieldset'] = [
    '#type' => 'fieldset',
    '#title' => 'Search Results',
    '#states' => [
      'invisible' => [
        'select[name="bundles"]' => ['value' => ''],
      ],
    ],
    '#collapsible' => FALSE,
    '#prefix' => '<div id="bundle_info_fieldset_wrapper">',
    '#suffix' => '</div>',
  ];

  $selected_bundle_id = isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : NULL;
  if ($selected_bundle_id) {
    $count = tripal_chado_get_orphaned_counts_for_bundle($selected_bundle_id);
    $name = $bundles[$selected_bundle_id];
    $form['bundle_info_fieldset']['message'] = [
      '#type' => 'markup',
      '#markup' => t('<p><strong>There are ' . $count . ' orphaned entities in the ' . $name . ' bundle.</strong></p>'),
    ];

    if ($count > 0) {
      $form['bundle_info_fieldset']['example_table'] = [
        '#type' => 'markup',
        '#prefix' => t('The following is a subset of the records that will be unpublished. Only a maximum of 10 records are shown.'),
        '#markup' => tripal_chado_missing_records_table($selected_bundle_id),
      ];

      $form['bundle_info_fieldset']['submit'] = [
        '#type' => 'submit',
        '#value' => 'Unpublish Orphaned Entities',
      ];
    }
  }

  return $form;
}

/**
 * Validate form entries.
 *
 * @param array $form
 * @param array $form_state
 */
function tripal_chado_unpublish_form_validate($form, &$form_state) {
  $bundle_id = isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : NULL;

  if (empty($bundle_id) || !is_numeric($bundle_id)) {
    form_set_error('bundles', t('Please select a valid bundle.'));
  }
}

/**
 * Process unpublish form.
 *
 * @param array $form
 * @param array $form_state
 */
function tripal_chado_unpublish_form_submit($form, &$form_state) {
  global $user;
  $bundle_id = isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : NULL;
  tripal_add_job('Delete Orphaned Entities', 'tripal_chado',
    'tripal_chado_delete_orphaned_entities', [$bundle_id], $user->uid, 10);

  drupal_set_message('Job submitted successfully!');
//   drupal_set_message('WARNING: It is expected to see a few tripal_chado errors
//    while the job is executing. The errors can be safely ignored.', 'warning');
}

/**
 * Get a list of bundles keyed by bundle id.
 *
 * @return array
 */
function tripal_chado_get_orphaned_bundles() {
  $bundles = db_query('SELECT bundle_id, data_table, label 
             FROM {chado_bundle} CB
             INNER JOIN {tripal_bundle} TB ON TB.id = CB.bundle_id
             ORDER BY label')->fetchAll();

  $mapped = [];

  foreach ($bundles as $bundle) {
    $mapped[$bundle->bundle_id] = "{$bundle->label}";
  }

  return $mapped;
}

/**
 * Get the count of orphaned entities per bundle.
 *
 * @param int $bundle
 *
 * @return int
 */
function tripal_chado_get_orphaned_counts_for_bundle($bundle_id) {
  // Get the bundle
  $bundle = db_query('SELECT bundle_id, data_table, label 
             FROM {chado_bundle} CB
             INNER JOIN {tripal_bundle} TB ON TB.id = CB.bundle_id
             WHERE bundle_id = :id', [':id' => $bundle_id])->fetchObject();

  $bundle_table = db_escape_table("chado_bio_data_{$bundle->bundle_id}");
  $chado_table = db_escape_table($bundle->data_table);
  $schema = chado_get_schema($chado_table);
  $primary_key = is_array($schema) ? $schema['primary key'][0] : NULL;
  $count = 0;

  // Get the count
  if ($primary_key) {
    $count = db_query('SELECT count(*) FROM {' . $bundle_table . '} BT
                        LEFT JOIN {chado.' . $chado_table . '} CT ON BT.record_id = CT.' . $primary_key . '
                        WHERE CT.' . $primary_key . ' IS NULL')->fetchField();
  }

  return (int) $count;
}

/**
 * Ajax callback for this form.
 *
 * @param array $form
 *
 * @return array
 */
function tripal_chado_unpublish_form_callback($form) {
  return $form['bundle_info_fieldset'];
}

/**
 * Create a table populated with examples of records that would get deleted.
 *
 * @param $bundle_id
 *
 * @return string
 * @throws \Exception
 */
function tripal_chado_missing_records_table($bundle_id) {
  // Get the bundle
  $bundle = db_query('SELECT bundle_id, data_table, label 
             FROM {chado_bundle} CB
             INNER JOIN {tripal_bundle} TB ON TB.id = CB.bundle_id
             WHERE bundle_id = :id', [':id' => $bundle_id])->fetchObject();

  $bundle_table = db_escape_table("chado_bio_data_{$bundle->bundle_id}");
  $chado_table = db_escape_table($bundle->data_table);
  $schema = chado_get_schema($chado_table);
  $primary_key = is_array($schema) ? $schema['primary key'][0] : NULL;

  $entities = db_query('SELECT TE.title, TE.id, BT.record_id FROM {' . $bundle_table . '} BT
                        LEFT JOIN {chado.' . $chado_table . '} CT ON BT.record_id = CT.' . $primary_key . '
                        LEFT JOIN {tripal_entity} TE ON TE.id = BT.entity_id
                        WHERE CT.' . $primary_key . ' IS NULL
                        ORDER BY BT.entity_id ASC
                        LIMIT 10')->fetchAll();

  return theme('table', [
    'header' => [
      'Entity ID',
      'Title',
      'Chado Table',
      str_replace('_', ' ', $primary_key),
    ],
    'rows' => array_map(function ($entity) use ($chado_table) {
      return [
        $entity->id,
        l($entity->title, 'bio_data/' . $entity->id),
        $chado_table,
        $entity->record_id,
      ];
    }, $entities),
  ]);
}
