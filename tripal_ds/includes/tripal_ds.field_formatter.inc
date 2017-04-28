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
        'instance_settings' => array('description' => '', 'classes' => '', 'id' => ''),
      ),
    ),
     'display' => array(
        'tripalpane' => array(
          'label' => t('Tripal Pane'),
          'description' => t('This fieldgroup renders the inner content in a Tripal Pane with the title as legend.'),
          'instance_settings' => array('description' => '', 'classes' => '', 'id' => ''),
        ),
      ),
  );
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
  switch ($group->format_type) {
    case 'tripalpane':
      $group_name = $group->group_name;
      $description = $group->format_settings['instance_settings']['description'];
      $classes = $group->format_settings['instance_settings']['classes'];
      $element['#prefix'] = '<div class="tripal_pane-fieldset-'.$group_name.' '.$group_name.' tripal_pane '.$classes.'"> <span class="field-group-format-title">' . check_plain(t($group->label)) . '</span>';
      if (!empty($description)) {
        $element['#prefix'] .= '<div class="description">' . $description . '</div>';
      }
      $element['#suffix'] = '</div>';
      break;
  }
}