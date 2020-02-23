<?php

function tripal_chado_publish_form($form, &$form_state) {

  $langcode = 'und';

  $bundle_name = '';
  if (array_key_exists('values', $form_state)) {
    $bundle_name = $form_state['values']['bundle_name'];
  }

  // Build the list of available TripalEntity content types.
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->orderBy('label', 'ASC')
    ->execute();
  $bundle_ids = [];
  $bundle_ids[] = 'Select a Content Type';
  while ($bundle = $bundles->fetchObject()) {
    $bundle_ids[$bundle->name] = $bundle->label;
  }

  $form['bundle_name'] = [
    '#type' => 'select',
    '#title' => 'Content Type',
    '#description' => t('Select a content type to publish.  Only data that
      is mapped to the selected vocabulary term will be published.'),
    '#options' => $bundle_ids,
    '#default_value' => $bundle_name,
    '#ajax' => [
      'callback' => "tripal_chado_publish_form_ajax_callback",
      'wrapper' => "tripal-chado-publish-form",
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];

  // If the user has selected a content type, then we need to
  // show some filters.
  if ($bundle_name) {
    $form['filters'] = [
      '#type' => 'fieldset',
      '#title' => 'Filters',
      '#description' => t('Please provide any filters for limiting
          the records. Only those that match the filters specified
          below will be published.  To publish all records of this
          type, leave all filters blank.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    ];

    // Get the list of fields and their widgets
    $fields = field_info_field_map();
    foreach ($fields as $field_name => $details) {
      if (array_key_exists('TripalEntity', $details['bundles']) and
        in_array($bundle_name, $details['bundles']['TripalEntity'])) {
        $field = field_info_field($field_name);
        $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);

        // Exclude data fields from the publish form.
        // We do this because the lack of default causes errors.
        if ($field['type'] == 'datetime'){
          continue;
        }

        // Get the Chado mapping information.
        $base_table = array_key_exists('base_table', $instance['settings']) ? $instance['settings']['base_table'] : '';
        $chado_table = array_key_exists('chado_table', $instance['settings']) ? $instance['settings']['chado_table'] : '';
        $chado_field = array_key_exists('chado_field', $instance['settings']) ? $instance['settings']['chado_field'] : '';

        // For now, only handle filtering by fields that are mapped directly
        // to the base table.
        if (!$base_table or $base_table != $chado_table) {
          continue;
        }

        // Create a default element for this field.  This code
        // is adapted from the field_multiple-value_form() function.
        $title = check_plain($instance['label']);
        $description = field_filter_xss($instance['description']);
        $parents = array_key_exists('#parents', $form) ? $form['#parents'] : '';
        $multiple = $field['cardinality'] > 1 || $field['cardinality'] == FIELD_CARDINALITY_UNLIMITED;
        $element = [
          '#entity_type' => $instance['entity_type'],
          '#entity' => NULL,
          '#bundle' => $instance['bundle'],
          '#field_name' => $field_name,
          '#language' => $langcode,
          '#field_parents' => $parents,
          '#columns' => array_keys($field['columns']),
          '#title' => $title,
          '#description' => $description,
          '#required' => FALSE,
          '#delta' => 0,
        ];

        // Now call the widget callback function to let the module adjust the
        // element as needed.
        if (!isset($instance['widget']['module'])) {
          tripal_report_error(
            'tripal_chado',
            TRIPAL_ERROR,
            "No module set for field :name widget.",
            [':name' => $instance['field_name']]
          );
          $instance['widget']['module'] = 'tripal';
        }
        $function = $instance['widget']['module'] . '_field_widget_form';
        $items = [];
        $delta = 0;
        $element = [];
        if (function_exists($function)) {
          $element = $function($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
        }

        // None of these fields are required, so turn off that setting.
        $element['#required'] = FALSE;

        $form['filters'][$field_name] = [
          '#type' => 'container',
          '#attributes' => [
            'class' => [
              'field-type-' . drupal_html_class($field['type']),
              'field-name-' . drupal_html_class($field_name),
              'field-widget-' . drupal_html_class($instance['widget']['type']),
            ],
          ],
          '#weight' => $instance['widget']['weight'],
          '#tree' => TRUE,
          $langcode => [
            0 => $element,
          ],
        ];
      }
    }
    $form['publish_btn'] = [
      '#type' => 'submit',
      '#name' => 'publish_btn',
      '#value' => 'Publish',
    ];
  }

  $form['#prefix'] = '<div id="tripal-chado-publish-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Validate handler for the tripal_chado_publish_form form.
 */
function tripal_chado_publish_form_validate($form, &$form_state) {

}

/**
 * Submit handler for the tripal_chado_publish_form form.
 */
function tripal_chado_publish_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'publish_btn') {
    global $user;

    $langcode = 'und';

    $bundle_name = $form_state['values']['bundle_name'];
    $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

    // Iterate through any filters and add those to the arguments
    $filters = [];
    $fields = field_info_field_map();
    foreach ($fields as $field_name => $details) {
      if (array_key_exists('TripalEntity', $details['bundles']) and
        in_array($bundle_name, $details['bundles']['TripalEntity']) and
        array_key_exists($field_name, $form_state['values']) and
        array_key_exists('value', $form_state['values'][$field_name][$langcode][0])) {
        $value = $form_state['values'][$field_name][$langcode][0]['value'];
        if ($value) {
          $filters[$field_name] = $value;
        }
      }
    }
    $args = [
      [
        'bundle_name' => $bundle_name,
        'filters' => $filters,
      ],
    ];
    $includes = [
      module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.publish'),
    ];
    return tripal_add_job("Publish " . $bundle->label . " records.",
      'tripal_chado', 'chado_publish_records', $args,
      $user->uid, 10, $includes);
  }
}

/**
 *
 */
function tripal_chado_publish_form_ajax_callback($form, $form_state) {
  return $form;
}
