<?php
/**
 * Implements hook_field_group_formatter_info().
 */
function tripal_ds_field_group_formatter_info() {
  return array(
    'form' => array(
      'tripalpane' => array(
        'label' => t('Tripal Pane'),
        'description' => t('This fieldgroup renders the inner content in a Tripal Pane with the title as legend.'),
        'format_types' => array('open', 'collapsible', 'collapsed'),
        'instance_settings' => array('description' => '', 'classes' => '', 'id' => ''),
        'default_formatter' => 'collapsible',
      ),
    ),
     'display' => array(
        'tripalpane' => array(
          'label' => t('Tripal Pane'),
          'description' => t('This fieldgroup renders the inner content in a Tripal Pane with the title as legend.'),
          'format_types' => array('open', 'collapsible', 'collapsed'),
          'default_formatter' => 'collapsible',
          'instance_settings' => array('description' => '', 'classes' => '', 'id' => ''),
        ),
      ),
  );
}

/**
 * Implements hook_field_group_format_settings().
 * If the group has no format settings, default ones will be added.
 * @params Object $group The group object.
 * @return Array $form The form element for the format settings.
 */
function tripal_ds_field_group_format_settings($group) {
  $form = array();

  // Add instance_settings.
  switch ($group->format_type) {
    case 'tripalpane':
      $form['label']['#description'] = t('Please enter a label for collapsible elements');
      break;
  }

  return $form;
}
/*
 * Implements field_group_pre_render_<format-type>.
 * Format type: Tripalpane.
 *
 * @param $element The field group form element.
 * @param $group The Field group object prepared for pre_render.
 * @param $form The root element or form.

function field_group_pre_render_tripalpane(&$element, $group, &$form) {

  $element += array(
    '#type' => 'tripalpane',
    '#title' => check_plain(t($group->label)),
    '#collapsible' => $group->collapsible,
    '#collapsed' => $group->collapsed,
    '#pre_render' => array(),
    '#attributes' => array('class' => explode(' ', $group->classes)),
    '#description' => $group->description,
  );

  if ($group->collapsible || $group->collapsed) {
    $element['#attached']['library'][] = array('system', 'drupal.collapse');
  }
}
 */
/*
 * Implements hook_field_group_build_pre_re2 nder_alter().
 * @param Array $elements by address.

function tripal_ds_field_group_build_pre_render_alter(&$element) {
  // While custom fields and groups are present at the root level of the form
  // e.g. $element['field_custom_field'] they will later be moved into
  // $element['additional_settings']['group']['#groups']['additional_settings']['field_custom_field']
  // which is where we need to alter them.

  // Use the states API to set the visibility of a fieldset within a vertical
  // tab based on the value of a custom checkbox field.
  if (isset($element['group_parent_vertical_tab'])) {
    $element['additional_settings']['group']['#groups']['additional_settings']['group_parent_vertical_tab']['group_child_fieldset']['#states'] = array(
      'visible' => array(
        ':input[name="field_custom_checkbox[' . LANGUAGE_NONE . ']"]' => array('checked' => TRUE),
      ),
    );
  }
}

**
 * Implements hook_field_group_format_summary().

function tripal_ds_field_group_format_summary($group) {
}
 */