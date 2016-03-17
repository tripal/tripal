<?php

/**
 *
 * @param $entity_type
 * @param $entity
 * @param $field
 * @param $instance
 * @param $langcode
 * @param $items
 * @param $display
 * @return string
 */
function chado_base__organism_id_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    $organism = chado_select_record('organism', array('genus', 'species'), array('organism_id' => $item['value']));
    $content = '<i>' . $organism[0]->genus .' ' . $organism[0]->species . '</i>';
    $element[$delta] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}
/**
 *
 * @param $field_name
 * @param $widget
 * @param $form
 * @param $form_state
 * @param $field
 * @param $instance
 * @param $langcode
 * @param $items
 * @param $delta
 * @param $element
 */
function chado_base__organism_id_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $options = tripal_get_organism_select_options(FALSE);
  $widget['value'] = array(
    '#type' => 'select',
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#options' => $options,
    '#default_value' => count($items) > 0 ? $items[0]['value'] : 0,
    '#required' => $element['#required'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#delta' => $delta,
    '#element_validate' => array('chado_base__organism_id_widget_validate'),
  );
  $widget['add_organism'] = array(
    '#type' => 'item',
    '#markup' => l('Add a new species', 'admin/content/bio_data/add/species', array('attributes' => array('target' => '_blank'))),
  );
}
/**
 * Callback function for validating the chado_base__organism_id_widget.
 */
function chado_base__organism_id_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }
  $organism_id = tripal_chado_get_field_form_values($field_name, $form_state);

  if (!$organism_id) {
    form_error($element, t("Please specify an organism."));
  }
}

/**
 * Provides a settings form for the formatter.
 *
 * This function is equiavlent to the hook_field_formatter_settings_form()
 * hook.
 *
 * @param $field
 *   The field structure being configured.
 * @param $instance
 *   The instance structure being configured.
 * @param $view_mode
 *   The view mode being configured.
 * @param $form
 *   The (entire) configuration form array, which will usually have no use here.
 * @param $form_state
 *   The form state of the (entire) configuration form.
 */
function chado_base__organism_id_formatter_form($field, $instance,
    $view_mode, $form, &$form_state) {

  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();
  $term = NULL;
  $bundle = NULL;

  // Check to see if the organism bundle exists
  $term = tripal_load_term_entity(array(
    'namespace' => $field['settings']['semantic_web']['ns'],
    'accession' => $field['settings']['semantic_web']['type']
  ));
  if ($term) {
    $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));
  }

  $element['instructions'] = array(
    '#type' => 'item',
    '#markup' => 'Please provide the format for viewing the organism. You
      can specify the format using tokens that correspond to each field'
  );

  $element['field_display'] = array(
    '#type' => 'textfield',
    '#title' => 'Display Format',
    '#description' => t('Provide a mixture of text and/or tokens for the format.
        For example: [organism__genus] [organism__species].  When displayed
        the tokens will be replaced with the actual value.'),
    '#default_value' => '[organism__genus] [organism__species]',
  );

  $element['field_display_entity'] = array(
    '#type' => 'checkbox',
    '#title' => 'Display teaser if available',
    '#description' => t('If the organism that this field is associated with is
        a published page then display the teaser rather use the display
        format above.'),
  );

  $element['tokens'] = array(
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#title' => 'Available Tokens'
  );
  $headers = array('Token', 'Description');
  $rows = array();
  $tokens = tripal_get_tokens($bundle);
  foreach ($tokens as $token) {
    $rows[] = array(
      $token['token'],
      $token['description'],
    );
  }


  $table_vars = array(
    'header'     => $headers,
    'rows'       => $rows,
    'attributes' => array(),
    'sticky'     => FALSE,
    'caption'    => '',
    'colgroups'  => array(),
    'empty'      => 'There are no tokens',
  );
  $project_details = theme('table', $table_vars);

  $element['tokens']['list'] = array(
    '#type' => 'item',
    '#markup' => theme_table($table_vars),
  );

  return $element;

}


