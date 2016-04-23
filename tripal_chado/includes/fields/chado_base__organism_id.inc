<?php
/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_base__organism_id_info() {
  return array(
    'label' => t('Organism'),
    'description' => t('A field for specifying an organism.'),
    'default_widget' => 'chado_base__organism_id_widget',
    'default_formatter' => 'chado_base__organism_id_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_attach_info().
 *
 * This is a hook provided by the tripal_Chado module. It allows the field
 * to specify which bundles it will attach to and to specify thee settings.
 *
 * @param $entity_type
 * @param $entity
 * @param $term
 *
 * @return
 *   A field array
 */
function chado_base__organism_id_attach_info($entity_type, $bundle, $target) {

  $field_info = array();

  $table_name = $target['data_table'];
  $type_table = $target['type_table'];
  $type_field = $target['field'];
  $cv_id      = $target['cv_id'];
  $cvterm_id  = $target['cvterm_id'];


  // Check the schema for the data table if it does not have
  // an 'organism_id' column then we don't want to attach this field.
  $schema = chado_get_schema($table_name);
  if (!array_key_exists('organism_id', $schema['fields'])) {
    return $field_info;
  }

  // There is an organism_id column so attach the field!
  $field_info = array(
    'field_name' => $table_name . '__organism_id',
    'field_type' => 'chado_base__organism_id',
    'widget_type' => 'chado_base__organism_id_widget',
    'description' => 'Select an organism.',
    'label' => 'Oraganism',
    'is_required' => 0,
    'storage' => 'field_chado_storage',
    'widget_settings' => array(
      'display_label' => 1
    ),
    'field_settings' => array(
      'chado_table' => $table_name,
      'chado_column' => 'organism_id',
      'semantic_web' => array(
        'type' => 'organism',
        'ns' => 'local',
        'nsurl' => '',
      ),
    ),
  );

  return $field_info;
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_base__organism_id_widget_info() {
  return array(
    'label' => t('Organism Select'),
    'field types' => array('chado_base__organism_id')
  );
}
/**
 * Implements hook_formatter_info.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_info() for each field
 * to specify.
 *
 */
function chado_base__organism_id_formatter_info() {
  return array(
    'label' => t('Organism'),
    'field types' => array('chado_base__organism_id'),
    'settings' => array(
      'field_display_teaser' => 0,
      'field_display_string' => '<i>[organism__genus] [organism__species]</i>',
    ),
  );
}
/**
 * Implements hook_formatter_settings_summary.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_summary() for each field
 * to specify.
 *
 */
function chado_base__organism_id_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = 'Settings summary: ';
  if (array_key_exists('field_display_teaser', $settings) and
      $settings['field_display_teaser'] == 1) {
        $summary .= 'displays the organism teaser if published.';
      }
      else if (array_key_exists('field_display_string', $settings)) {
        $summary .= 'uses the token string "' . $settings['field_display_string'] . '"';
      }
      else {
        $summary .= "uses display defaults.";
      }

      return $summary;
}

/**
 * Provides a settings form for the formatter.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_settings_form() for each field
 * to specify.
 */
function chado_base__organism_id_formatter_settings_form($field, $instance,
    $view_mode, $form, &$form_state) {

  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];


  $element = array();
  $term = NULL;
  $entity = NULL;

  // Check to see if the organism bundle exists
  $term = tripal_load_term_entity(array(
    'namespace' => $field['settings']['semantic_web']['ns'],
    'accession' => $field['settings']['semantic_web']['type']
  ));
  if ($term) {
    $entity = tripal_load_bundle_entity(array('term_id' => $term->id));
  }

  $element['instructions'] = array(
    '#type' => 'item',
    '#markup' => 'Please provide the format for viewing the organism. You
      can specify the format using tokens that correspond to each field'
  );

  $element['field_display_string'] = array(
    '#type' => 'textfield',
    '#title' => 'Display Format',
    '#description' => t('Provide a mixture of text and/or tokens for the format.
        For example: [organism__genus] [organism__species].  When displayed,
        the tokens will be replaced with the actual value.'),
    '#default_value' => $settings['field_display_string'],
  );

  $element['field_display_teaser'] = array(
    '#type' => 'checkbox',
    '#title' => 'Display teaser if available',
    '#description' => t('If the organism that this field is associated with is
        a published page then display the teaser rather use the display
        format above.'),
    '#default_value' => $settings['field_display_teaser'],
  );

  $element['tokens'] = array(
    '#type' => 'fieldset',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#title' => 'Available Tokens'
  );
  $headers = array('Token', 'Description');
  $rows = array();
  $tokens = tripal_get_tokens($entity);
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

  $element['#element_validate'] = array('chado_base__organism_id_formatter_settings_form_validate');

  return $element;

}

/**
 * Validation function for the chado_base__organism_id_formatter_settings_form.
 */
function chado_base__organism_id_formatter_settings_form_validate(&$form, &$form_state) {

  // Place here as an example for validating the settings form.
}

/**
 *
 */
function chado_base__organism_id_formatter(&$element, $entity_type, $entity,
    $field, $instance, $langcode, $items, $display) {

  // Get the settings
  $settings = $display['settings'];

  foreach ($items as $delta => $item) {
    $organism = chado_select_record('organism', array('genus', 'species'), array('organism_id' => $item['value']));

    if ($settings['field_display_teaser'] == 0) {
      $field_name = $field['field_name'];
      $string = $settings['field_display_string'];
      $field_data = $entity->$field_name;
      // TODO: add a way to get tokens from Chado records. this is
      // needed in the case that a record is not published as an entity.
      $content = tripal_replace_tokens($string, $field_data['und'][0]['entity']);
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
    else {

    }
  }
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 *
 * @param $field
 * @param $entity
 * @param $base_table
 * @param $record
 */
function chado_base__organism_id_load($field, $entity, $base_table, $record) {
  $field_name = $field['field_name'];

  // The organism_id is already set as the value. We need to get it and
  // see if there is a published entity for this organism. If there is
  // then we want to add the organism object generated using chado_generate_var
  // as well as the entity to the record.
  $organism_id = $entity->{$field_name}['und'][0]['value'];
  $organism = chado_generate_var('organism', array('organism_id' => $organism_id));
  $entity->{$field_name}['und'][0]['organism'] = $organism;
  $org_entity = tripal_load_chado_entity('organism', $organism_id);
  $entity->{$field_name}['und'][0]['entity'] = $org_entity;
}
/**
 * Implements hook_ws_formatter().
 */
function chado_base__organism_id_ws_formatter(&$element, $entity_type, $entity,
    $field, $instance, $items) {

  foreach ($items as $delta => $item) {
    $organism = $item['organism'];
    $entity = $item['entity'];
    if ($entity) {
      $element[$delta]['#entity'] = $entity;
    }
    $element[$delta]['genus'] = $organism->genus;
    $element[$delta]['species'] = $organism->species;
    $element[$delta]['common_name'] = $organism->common_name;
  }
}

/**
 *  Implements hook_widget().
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

