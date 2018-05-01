<?php

/**
 * This theme function is meant to override the data_combo theme.
 *
 * @param $variables
 */
function theme_tripal_chado_date_combo($variables) {
  $element = $variables['element'];
  $field = field_info_field($element['#field_name']);
  $instance = field_info_instance($element['#entity_type'], $element['#field_name'], $element['#bundle']);

  // Group start/end items together in fieldset.
  $fieldset = [
    '#title' => t($element['#title']) . ' ' . ($element['#delta'] > 0 ? intval($element['#delta'] + 1) : ''),
    '#value' => '',
    '#description' => !empty($element['#fieldset_description']) ? $element['#fieldset_description'] : '',
    '#attributes' => [],
    '#children' => $element['#children'],
    '#attributes' => ['class' => ['collapsible', 'collapsed']],
  ];
  return theme('fieldset', ['element' => $fieldset]);
}
