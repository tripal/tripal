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
 */
function tripal_ds_field_group_pre_render(&$element, $group, &$form) {
  $group_name = $group->group_name;
  switch ($group->format_type) {
    case 'tripalpane':
      $element['#prefix'] = '<div class="tripal_pane-fieldset-'.$group_name.' '.$group_name.'">';
      $element['#suffix'] = '</div>';
      break;
  }
}