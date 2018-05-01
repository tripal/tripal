<?php
/**
 * Implements hook_field_group_formatter_info().
 */
function tripal_ds_field_group_formatter_info() {
  return [
    'form' => [
      'tripalpane' => [
        'label' => t('Tripal Pane'),
        'description' => t('This fieldgroup renders the inner content in a Tripal Pane with the title as legend.'),
        'instance_settings' => [
          'description' => '',
          'classes' => '',
          'id' => '',
          'hide' => 1,
        ],
      ],
    ],
    'display' => [
      'tripalpane' => [
        'label' => t('Tripal Pane'),
        'description' => t('This fieldgroup renders the inner content in a Tripal Pane with the title as legend.'),
        'instance_settings' => [
          'description' => '',
          'classes' => '',
          'id' => '',
          'hide' => 1,
        ],
      ],
    ],
  ];
}

/**
 * Implements hook_field_group_format_settings().
 *
 * @params Object $group The group object.
 *
 * @return Array $form The form element for the format settings.
 */
function tripal_ds_field_group_format_settings($group) {
  // Add a wrapper for extra settings to use by others.
  $form = [
    'instance_settings' => [
      '#tree' => TRUE,
      '#weight' => 2,
    ],
  ];

  $field_group_types = field_group_formatter_info();
  $mode = $group->mode == 'form' ? 'form' : 'display';
  $formatter = $field_group_types [$mode][$group->format_type];

  // Add optional instance_settings.
  switch ($group->format_type) {
    case 'tripalpane':
      $form['instance_settings']['hide'] = [
        '#title' => t('Hide panel on page load'),
        '#type' => 'checkbox',
        '#default_value' => isset($group->format_settings['instance_settings']['hide']) ? $group->format_settings['instance_settings']['hide'] : $formatter['instance_settings']['hide'],
        '#weight' => 2,
      ];
      break;
  }

  return $form;
}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Tripalpane.
 *
 * @param $element The field group form element.
 * @param $group The Field group object prepared for pre_render.
 * @param $form The root element or form.
 */
function tripal_ds_field_group_pre_render(&$element, $group, &$form) {
  switch ($group->format_type) {
    case 'tripalpane':
      $group_name = $group->group_name;
      //Hide the tripal panes here if there are no children.
      $description = $group->format_settings['instance_settings']['description'];
      $hide = isset($group->format_settings['instance_settings']['hide']) ? $group->format_settings['instance_settings']['hide'] : 'none';
      $classes = $group->format_settings['instance_settings']['classes'];
      if ($hide == '1') {
        $element['#prefix'] = '<div class="tripal_pane-fieldset-' . $group_name . ' ' . $group_name . ' tripal_pane ' . $classes . ' hideTripalPane"> <span class="field-group-format-title">' . check_plain(t($group->label)) . '</span>';
      }
      else {
        $element['#prefix'] = '<div class="tripal_pane-fieldset-' . $group_name . ' ' . $group_name . ' tripal_pane ' . $classes . '"> <span class="field-group-format-title">' . check_plain(t($group->label)) . '</span>';

      }
      if (!empty($description)) {
        $element['#prefix'] .= '<div class="description">' . $description . '</div>';
      }
      $element['#suffix'] = '</div>';
      break;
  }
}

/**
 * Updated the weight of field groups to reflect their order in the layout
 * array.
 *
 * @param $field_name
 *  Machine readable name of the field.
 * @param $bundle
 *  Machine name of bundle, example bio_data_1
 * @param $weight
 */
function tripal_ds_field_group_update_weight($field_name, $bundle, $weight) {
  // Pull the group data from field_group table.
  $result = db_select('field_group', 'fg')
    ->fields('fg')
    ->condition('bundle', $bundle, '=')
    ->condition('group_name', $field_name, '=')
    ->execute()
    ->fetchAssoc();
  // If result found update the weight.
  if (!empty($result)) {
    $data = unserialize($result['data']);
    $data['weight'] = $weight;
    $data = serialize($data);

    // Write the new weight to the field_group entry.
    db_update('field_group')
      ->fields([
        'data' => $data,
      ])
      ->condition('bundle', $bundle, '=')
      ->condition('group_name', $field_name, '=')
      ->execute();
  }
}
