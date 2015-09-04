<?php

/**
 * Implements hook_field_info().
 */
function tripal_entities_field_info() {
  $fields = array(
    'organism_id' => array(
      'label' => t('Organism'),
      'description' => t('A field for specifying an organism.'),
      'default_widget' => 'tripal_entities_organism_select_widget',
      'default_formatter' => 'tripal_entities_organism_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'tripal_entities_storage',
        'module' => 'tripal_entities',
        'active' => TRUE
      ),
    ),
  );
  return $fields;
}

/**
 * Implements hook_field_widget_info().
 */
function tripal_entities_field_widget_info() {
  return array(
    'tripal_entities_organism_select_widget' => array(
      'label' => t('Organism Select'),
      'field types' => array('organism_id')
    ),
  );
}
/**
 * Implements hook_field_formatter_info().
 */
function tripal_entities_field_formatter_info() {
  return array(
    'tripal_entities_organism_formatter' => array(
      'label' => t('Organism'),
      'field types' => array('organism_id')
    ),
  );
}
/**
 * Implements hook_field_formatter_view().
 *
 * Two formatters are implemented.
 * - field_example_simple_text just outputs markup indicating the color that
 *   was entered and uses an inline style to set the text color to that value.
 * - field_example_color_background does the same but also changes the
 *   background color of div.region-content.
 *
 * @see field_example_field_formatter_info()
 */
function tripal_entities_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  switch ($display['type']) {
    // This formatter simply outputs the field as text and with a color.
    case 'tripal_entities_organism_formatter':
      foreach ($items as $delta => $item) {
        $organism = chado_select_record('organism', array('genus', 'species'), array('organism_id' => $item['value']));
        $element[$delta] = array(
          // We create a render array to produce the desired markup,
          // "<p>Genus Species</p>".
          // See theme_html_tag().
          '#type' => 'markup',
          '#markup' => '<i>' . $organism[0]->genus .' ' . $organism[0]->species . '</i>',
        );
      }
      break;
  }
  return $element;
}

/**
 * Implements hook_field_widget_form().
 */
function tripal_entities_field_widget_form(&$form, &$form_state, $field,
  $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  $widget['#delta'] = $delta;

  switch ($instance['widget']['type']) {
    case 'tripal_entities_organism_select_widget':
      $options = tripal_get_organism_select_options();
      $widget += array(
        '#type' => 'select',
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#options' => $options,
        '#default_value' => count($items) > 0 ? $items[0]['value'] : 0,
        '#required' => $element['#required'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#delta' => $delta,
        '#element_validate' => array('tripal_entities_organism_select_widget_validate'),
      );
      $element['value'] = $widget;
      break;
  }
  return $element;
}


/**
 * Implements hook_field_is_empty().
 */
function tripal_entities_field_is_empty($item, $field) {
  if (empty($item['value']) && (string) $item['value'] !== '0') {
    return TRUE;
  }
  return FALSE;
}
/**
 * Callback function for validating the tripal_entities_organism_select_widget.
 */
function tripal_entities_organism_select_widget_validate($element, &$form_state) {

  $field_name = $element['#field_name'];
  // Make sure we have a valid organism
  foreach ($form_state['values'][$field_name] as $langcode => $items) {
    foreach ($items as $delta => $value) {
      $organism_id = chado_select_record('organism', array('organism_id'),
        array('organism_id' => $value['value']), array('has_record' => TRUE));
      if (!$organism_id) {
        form_error($element, t("Please specify an organism that already exists in the database."));
      }
    }
  }
}