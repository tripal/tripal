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
   * @see TripalField::can_attach()
   */
  protected function can_attach($entity_type, $bundle, $details) {

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    $schema = chado_get_schema($table_name);
    if (!$schema) {
      return FALSE;
    }

    // If this is the organism table then do not attach as the organism_id
    // field is the primary key and we don't want a field for that.
    if ($table_name == 'organism') {
      return FALSE;
    }

    // Check the schema for the data table if it has
    // an 'organism_id' column then we want to attach this field.
    if (array_key_exists('organism_id', $schema['fields'])) {
      return TRUE;
    }

    return FALSE;
  }

  /**
   * @see TripalField::create_info()
   */
  public function create_info($entity_type, $bundle, $details) {

    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    return array(
      'field_name' => $table_name . '__organism_id',
      'type' => 'chado_base__organism_id',
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
      'settings' => array(
        'chado_table' => $table_name,
        'chado_column' => 'organism_id',
        'semantic_web' => 'local:organism',
      ),
    );
  }
  /**
   * @see TripalField::create_instance_info()
   */
  public function create_instance_info($entity_type, $bundle, $details) {

    if (!$this->can_attach($entity_type, $bundle, $details)) {
      return;
    }

    $table_name = $details['chado_table'];
    $type_table = $details['chado_type_table'];
    $type_field = $details['chado_type_column'];
    $cv_id      = $details['chado_cv_id'];
    $cvterm_id  = $details['chado_cvterm_id'];

    $is_required = FALSE;
    $schema = chado_get_schema($table_name);
    if (array_key_exists('not null', $schema['fields']['organism_id']) and
        $schema['fields']['organism_id']['not null']) {
      $is_required = TRUE;
    }

    return array(
      'field_name' => $table_name . '__organism_id',
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Organism',
      'description' => 'Select an organism.',
      'required' => $is_required,
      'settings' => array(
        'auto_attach' => TRUE,
      ),
      'widget' => array(
        'type' => 'chado_base__organism_id_widget',
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'deafult' => array(
          'label' => 'above',
          'type' => 'chado_base__organism_id_formatter',
          'settings' => array(),
        ),
      ),
    );
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

    if (count($items) > 0) {
      $content =  $items[0]['value'];
      if (array_key_exists('entity_id', $items[0])) {
        $content = l(strip_tags($items[0]['value']), 'bio_data/' . $items[0]['entity_id']);
      }

      // The cardinality of this field is 1 so we don't have to
      // iterate through the items array, as there will never be more than 1.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }

  /**
   * @see TripalField::widget()
   */
  public function widget_form(&$widget, &$form, &$form_state, $field, $instance,
      $langcode, $items, $delta, $element) {

    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];

    $organism_id = 0;
    if (count($items) > 0 and array_key_exists($field_table . '__organism_id', $items[0])) {
      $organism_id = $items[0][$field_table . '__organism_id'];
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $options = tripal_get_organism_select_options(FALSE);
    $widget[$field_table . '__organism_id'] = array(
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $organism_id,
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
      '#element_validate' => array('chado_base__organism_id_widget_validate'),
    );
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

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
    );

    if ($record) {
      $organism = $record->organism_id;
      $string = $settings['field_display_string'];
      $value = tripal_replace_chado_tokens($string, $organism);
      $entity->{$field_name}['und'][0]['value'] = $value;
      $entity->{$field_name}['und'][0][$field_table . '__organism_id'] = $organism->organism_id;

      // Is there a published entity for this organism?
      if (property_exists($entity->chado_record->$field_column, 'entity_id')) {
        $fk_entity_id = $entity->chado_record->$field_column->entity_id;
        $entity->{$field_name}['und'][0]['entity_id'] = $fk_entity_id;
        $entity->{$field_name}['und'][0]['entity_type'] = 'TripalEntity';
      }
    }
  }

  /**
   * @see TripalField::settings_form()
   */
  public function settings_form($field, $instance, $has_data) {

    $settings = $field['settings'];

    $element = array();
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

    // Add in the semantic web fields.
    $parent_elements = parent::settings_form($field, $instance, $has_data);
    $element = array_merge($element, $parent_elements);

    return $element;
  }

}


/**
 * Callback function for validating the chado_base__organism_id_widget.
 */
function chado_base__organism_id_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];
  $field = $form_state['field'][$field_name]['und']['field'];
  $settings = $field['settings'];
  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }
  $organism_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_table . '__organism_id');

  if (!$organism_id) {
    form_error($element, t("Please specify an organism."));
  }
}