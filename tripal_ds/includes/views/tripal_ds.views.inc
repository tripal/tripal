<?php


/**
 * Implements hook_views_data().
 */
function tripal_ds_views_data() {

  $data = [];

  $data['tripal_ds']['table']['group'] = t('Tripal Panes');

  $data['tripal_ds']['table']['base'] = [
    'title' => t('Tripal Panes'),
    'help' => t('Contains Tripal Pane records we want exposed to Views.'),
  ];

  $data['tripal_ds']['bundle'] = [
    'title' => t('Bundle ID'),
    'help' => t('This the unque bundle name, eg bio_data_1.'),
    'relationship' => [
      'base' => 'tripal_entity', // The name of the table to join with.
      'base field' => 'bundle', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Bundle relationship with tripal_entity table'),
      'title' => t('Bundle and tripal_entity relationship'),
      'help' => t('This relationship links the tripal_ds table with the tripal_entity table via the bundle.'),
    ],
    'filter' => [
      'handler' => 'views_handler_filter_bundle',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
    'field' => [
      'handler' => 'views_handler_field',
    ],
  ];

  $data['tripal_ds']['tripal_ds_field_name'] = [
    'title' => t('Field Name'),
    'help' => t('This the field name, it is not necessarily unique, like "summary" which might feature across several bundles.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  $data['tripal_ds']['tripal_ds_field_label'] = [
    'title' => t('Field Label'),
    'help' => t('The human readable name of the field, like "Sequences".'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];
  $data['tripal_ds']['weight'] = [
    'title' => t('Weight'),
    'help' => t('The weight as determined by the order in the Manage Display tab of the content type.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];
  return $data;
}

