<?php

/**
 * Unpublish orphaned entities form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_unpublish_orphans_form($form, &$form_state) {

  // Get the list of bundles.
  $bundles = [];
  $query = '
    SELECT bundle_id, data_table, label
    FROM {chado_bundle} CB
      INNER JOIN {tripal_bundle} TB ON TB.id = CB.bundle_id
    ORDER BY label
  ';
  $results = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->orderBy('label')
    ->execute();
  while ($bundle = $results->fetchObject()) {
    $bundles[$bundle->id] = $bundle->label;
  }
  $bundles[0] = "Delete all orphaned content";
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
      'callback' => 'tripal_unpublish_orphans_form_callback',
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
    $bundlec = entity_get_controller('TripalBundle');
    $count = $bundlec->findOrphans($selected_bundle_id, TRUE);
    $name = $bundles[$selected_bundle_id];
    $form['bundle_info_fieldset']['message'] = [
      '#type' => 'markup',
      '#markup' => t('<p><strong>There are ' . $count . ' orphaned entities in the ' . $name . ' bundle.</strong></p>'),
    ];

    if ($count > 0) {
      $form['bundle_info_fieldset']['example_table'] = [
        '#type' => 'markup',
        '#prefix' => t('The following is a subset of the records that will be unpublished. Only a maximum of 10 records are shown.'),
        '#markup' => tripal_chado_missing_records_table($bundlec, $selected_bundle_id),
      ];

      $form['bundle_info_fieldset']['submit'] = [
        '#type' => 'submit',
        '#value' => 'Unpublish Orphaned Entities',
      ];
    }
  }

  if ($selected_bundle_id === '0') {
    $bundlec = entity_get_controller('TripalBundle');
    $count = $bundlec->findAllOrphans(TRUE);
    $form['bundle_info_fieldset']['message'] = [
      '#type' => 'markup',
      '#markup' => t('<p><strong>There are ' . $count . ' orphaned entities in all bundles.</strong></p>'),
    ];

    if ($count > 0) {
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
function tripal_unpublish_orphans_form_validate($form, &$form_state) {
  $bundle_id = isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : NULL;

  if (empty($bundle_id) || !is_numeric($bundle_id)) {
    if (!isset($bundle_id) || $bundle_id !== '0') {
      form_set_error('bundles', t('Please select a valid bundle.'));
    }
  }
}

/**
 * Process unpublish form.
 *
 * @param array $form
 * @param array $form_state
 */
function tripal_unpublish_orphans_form_submit($form, &$form_state) {
  global $user;

  $bundle_id = isset($form_state['values']['bundles']) ? $form_state['values']['bundles'] : NULL;

  tripal_add_job('Delete Orphaned Entities', 'tripal_chado',
    'tripal_unpublish_orphans', [$bundle_id], $user->uid, 10, []);

  drupal_set_message('Job submitted');
}


/**
 * Ajax callback for this form.
 *
 * @param array $form
 *
 * @return array
 */
function tripal_unpublish_orphans_form_callback($form) {
  return $form['bundle_info_fieldset'];
}

/**
 * Create a table populated with examples of records that would get deleted.
 *
 * @param TripalBundleController $bundlec
 * @param int $selected_bundle_id
 *
 * @return string
 */
function tripal_chado_missing_records_table(TripalBundleController $bundlec, int $selected_bundle_id) {
  $ids = $bundlec->findOrphans($selected_bundle_id, FALSE, 0, 10);
  $entities = tripal_load_entity('TripalEntity', $ids);

  return theme('table', [
    'header' => [
      'Entity ID',
      'Title',
    ],
    'rows' => array_map(function ($entity) {
      return [
        $entity->id,
        l($entity->title, 'bio_data/' . $entity->id),
      ];
    }, $entities),
  ]);
}
