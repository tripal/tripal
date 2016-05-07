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
      'settings' => array(
        'field_display_string' => '<i>[organism.genus] [organism.species]</i>',
      ),
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
      'label' => 'Organism',
      'is_required' => 0,
      'storage' => 'field_chado_storage',
      'widget_settings' => array(
        'display_label' => 1
      ),
      'field_settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'organism_id',
        'semantic_web' => array(
          'name' => 'organism',
          'accession' => 'organism',
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
      ),
    );
  }

  /**
   * @see TripalField::formatter_view()
   */
  public function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    // The cardinality of this field is 1 so we don't have to
    // iterate through the items array, as there will never be more than 1.
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => $items[0]['value'],
    );
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
      '#default_value' => count($items) > 0 ? $items[0]['organism_id'] : 0,
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

  /**
   * @see TripalField::load()
   */
  public function load($field, $entity, $details) {

    $record = $details['record'];
    $settings = $field['settings'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    $organism = $record->organism_id;
    $string = $settings['field_display_string'];
    $value = tripal_replace_chado_tokens($string, $organism);

    $entity->{$field_name}['und'][0]['value'] = $value;
    $entity->{$field_name}['und'][0]['organism_id'] = $organism->organism_id;

    // Is there a published entity for this organism?
    if (property_exists($entity->chado_record->$field_column, 'entity_id')) {
      $fk_entity_id = $entity->chado_record->$field_column->entity_id;
      $entity->{$field_name}['und'][0]['entity_id'] = $fk_entity_id;
      $entity->{$field_name}['und'][0]['entity_type'] = 'TripalEntity';
    }
  }

  /**
   * @see TripalField::settings_form()
   */
  public function settings_form($field, $instance, $view_mode) {

    $settings = $field['settings'];

    $element = array();
    $term = NULL;
    $entity = NULL;

    // Check to see if the organism bundle exists
    $term = tripal_load_term_entity(array(
      'namespace' => $field['settings']['semantic_web']['ns'],
      'accession' => $field['settings']['semantic_web']['accession']
    ));
    if ($term) {
      $entity = tripal_load_bundle_entity(array('term_id' => $term->id));
    }

    $element['instructions'] = array(
      '#type' => 'item',
      '#markup' => 'You may rewrite the way this field is presented to the end-user.
        The Rewrite Value field allows you to use tokens to indicate how the
        value should be displayed.  Tokens will be substituted with appriorate
        data from the database.  See the Available tokens list for the
        tokens you may use.'
    );

    $element['field_display_string'] = array(
      '#type' => 'textfield',
      '#title' => 'Rewrite Value',
      '#description' => t('Provide a mixture of text and/or tokens for the format.
          For example: [organism.genus] [organism.species].  When displayed,
          the tokens will be replaced with the actual value.'),
      '#default_value' => $settings['field_display_string'],
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
   * @see TripalField::views_data()
   */
  public function views_data_alter(&$data, $field, $entity_info) {
    dpm($data);
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