<?php

class chado_base__organism_id extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Organism';

  // The default description for this field.
  public static $default_description = 'A field for specifying an organism.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'field_display_string' => '<i>[organism.genus] [organism.species]</i>',
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';


  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

    if (count($items) > 0) {
      $content = $items[0]['value']['label'];
      if (array_key_exists('entity', $items[0]['value'])) {
        list($entity_type, $entity_id) = explode(':', $items[0]['value']['entity']);
        $content = l(strip_tags($items[0]['value']['label']), 'bio_data/' . $entity_id);
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
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    $organism_id = 0;
    if (count($items) > 0 and array_key_exists('chado-' . $field_table . '__organism_id', $items[0])) {
      $organism_id = $items[0]['chado-' . $field_table . '__organism_id'];
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $options = tripal_get_organism_select_options(FALSE);
    $widget['chado-' . $field_table . '__organism_id'] = array(
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $organism_id,
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    );
  }

  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $organism_id = $values['chado-' . $field_table . '__organism_id'];
      if (!$organism_id or $organism_id == 0) {
        $errors[$field_name]['und'][0][] = array(
          'message' =>  t("Please specify an organism."),
          'error' => 'chado_base__organism_id'
        );
      }
    }
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $record = $details['record'];
    $settings = $this->field['settings'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        'chado-' . $field_table . '__organism_id' => '',
        'label' => '',
        'genus' => '',
        'species' => '',
      ),
      'semantic_web' => array(
        'label' => 'rdfs:label',
        'genus' => tripal_get_chado_semweb_term('organism', 'genus'),
        'species' => tripal_get_chado_semweb_term('organism', 'species'),
        'infraspecific_name' => tripal_get_chado_semweb_term('organism', 'infraspecific_name'),
        'infraspecific_type' => tripal_get_chado_semweb_term('organism', 'type_id'),
      ),
      'chado_mapping' => array(
        'genus' => 'organism_id,genus',
        'species' => 'organism_id,genus',
        'infraspecific_name' => 'organism_id,infraspecific_name',
        'infraspecific_type' => 'organism_id,infraspecific_type',
      )
    );

    if ($record) {
      $organism = $record->organism_id;
      $string = $settings['field_display_string'];
      $label = tripal_replace_chado_tokens($string, $organism);
      $entity->{$field_name}['und'][0]['value'] = array(
        'label' => $label,
        'genus' => $organism->genus,
        'species' => $organism->species,
      );
      // The infraspecific fiels were introdcued in Chado v1.3.
      if (property_exists($organism, 'infraspecific_name')) {
        $entity->{$field_name}['und'][0]['value']['infraspecific_type'] = NULL;
        $entity->{$field_name}['und'][0]['value']['infraspecific_name'] = $organism->infraspecific_name;
        if ($organism->type_id) {
          $entity->{$field_name}['und'][0]['value']['infraspecific_type'] =  $organism->type_id->name;
        }
      }
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__organism_id'] = $organism->organism_id;

      // Is there a published entity for this organism?
      if (property_exists($entity->chado_record->$field_column, 'entity_id')) {
        $fk_entity_id = $entity->chado_record->$field_column->entity_id;
        $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $fk_entity_id;
      }
    }
  }

  /**
   * @see TripalField::settings_form()
   */
  public function globalSettingsForm($has_data) {
    $element = parent::globalSettingsForm($has_data);

    $settings = $this->field['settings'];

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