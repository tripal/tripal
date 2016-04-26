<?php

class chado_base__organism_id extends TripalField {

  /**
   * @see TripalField::info()
   */
  public function field_info() {
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
   * @see TripalField::attach_info()
   */
  public function attach_info($entity_type, $bundle, $settings) {

    $field_info = array();

    $table_name = $settings['data_table'];
    $type_table = $settings['type_table'];
    $type_field = $settings['field'];
    $cv_id      = $settings['cv_id'];
    $cvterm_id  = $settings['cvterm_id'];

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
   * @see TripalField::widget_info()
   */
  public function widget_info() {
    return array(
      'label' => t('Organism Select'),
      'field types' => array('chado_base__organism_id')
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  public function formatter_info() {
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
   * @see TripalField::formatter_settings_summary()
   */
  public function formatter_settings_summary($field, $instance, $view_mode) {
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
   * @see TripalField::formatter_settings_form()
   */
  public function formatter_settings_form($field, $instance,
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
          For example: [organism.genus] [organism.species].  When displayed,
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

    // Here we use the tripal_get_chado_tokens rather than the
    // tripal_get_entity_tokens because we can't gurantee that all organisms
    // have entities.
    $tokens = tripal_get_chado_tokens('organism');
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

  /**
   * @see TripalField::formatter_view()
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];
    $record = $entity->chado_record;

    foreach ($items as $delta => $item) {
      $organism = $record->organism_id;

      if ($settings['field_display_teaser']) {
      }
      else {
        $field_name = $field['field_name'];
        $string = $settings['field_display_string'];
        $field_data = $entity->$field_name;
        $content = tripal_replace_chado_tokens($string, $organism);
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
      }
    }
  }

  /**
   * @see TripalField::widget()
   */
  public function widget_form(&$widget, $form, $form_state, $field, $instance,
      $langcode, $items, $delta, $element) {
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
   * @see TripalField::ws_formatter()
   */
  public function ws_formatter($entity_type, $entity, $field, $instance, $items) {

    $organism = (array) $entity->chado_record->organism_id;
    unset($organism->tablename);

    return $organism;
  }
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