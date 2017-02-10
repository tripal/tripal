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

/*
 * Implements hook_field_group_create_field_group
 * 
 */
function tripal_ds_field_group_create_field_group($group) {

  if($group->format_type == 'tripalpane'){
    //write to the tripal_ds table to record the new tripal pane
    $field_for_table = new stdClass();
    $field_for_table->tripal_ds_field_name = $group->group_name;
    $field_for_table->tripal_ds_field_label = $group->label;
    $field_for_table->entity_type = 'TripalEntity';
    $field_for_table->bundle = $group->bundle;

    drupal_write_record('tripal_ds', $field_for_table);

  }
}

/*
 * Implements hook_field_group_delete_field_group
 * 
 */
function tripal_ds_field_group_delete_field_group($group) {

  if($group->format_type == 'tripalpane'){
    db_delete('tripal_ds')
      ->condition('bundle', $group->bundle, '=')
      ->condition('tripal_ds_field_name', $group->group_name, '=')
      ->execute();
    }
}

/*
 * Implements hook_field_group_update_field_group
 * checks for changes from tripalpane to another type
 */
function tripal_ds_field_group_update_field_group($group) {
  //change from tripal pane to another type
  if(!empty($group->bundle)){
    $result = db_select('tripal_ds', 't')
      ->fields('t')
      ->condition('bundle', $group->bundle,'=')
      ->condition('tripal_ds_field_name', $group->group_name,'=')
      ->execute()
      ->fetchAssoc();

    //change from tripal pane to another type
    if($group->format_type !== 'tripalpane' && !empty($result)){
       db_delete('tripal_ds')
      ->condition('bundle', $result['bundle'], '=')
      ->condition('tripal_ds_field_name', $result['tripal_ds_field_name'], '=')
      ->execute();
    }

    //change from other type to tripalpane
    if($group->format_type == 'tripalpane' && empty($result)){

      if(strpos($group->group_name, 'Tripal Pane')){
        $group_name = str_replace('Tripal Pane', "", $group->group_name);
      } 
      $field_for_table = new stdClass();
      $field_for_table->tripal_ds_field_name = $group_name;
      $field_for_table->tripal_ds_field_label = $group->label;
      $field_for_table->entity_type = 'TripalEntity';
      $field_for_table->bundle = $group->bundle;

      drupal_write_record('tripal_ds', $field_for_table);
    }
  }
}

