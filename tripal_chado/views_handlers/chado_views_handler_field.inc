<?php

// We need to include the views_handler_field_field file since it 
// includes _field_view_formatter_options() which we need.
require_once drupal_get_path('module', 'views') . '/modules/field/views_handler_field_field.inc';

/**
 * Views Field Handler for chado fields.
 * Uses the same approach as the field api views_handler_field_field.
 */
class chado_views_handler_field extends views_handler_field {

  /**
   * Alter the views query to provide information for this field.
   *
   * We are going to take the same approach as the field api and simply load
   * the entities in order to get the values of the chado fields. The hope is
   * that a small number of cached simple queries will be more efficient than
   * the crazy joins that occur when using chado. *fingers crossed*
   */
  function query($use_groupby = FALSE) {

    $this->base_table = $this->definition['entity_table'];
    $this->base_table_alias = $this->base_table;
    $this->ensure_my_table();

    // Because we are just loading entities, we need the entity id and type only.
    $this->entity = $entity_info = entity_get_info($this->definition['entity_type']);
    $alias_stub = 'chado_field_' . $this->definition['chado_table'] . '_' . $this->definition['chado_field'];

    $this->id_alias = $this->field_alias = $this->query->add_field($this->base_table_alias, $entity_info['entity keys']['id'], $alias_stub . '_entity_id');
    $this->type_alias = $this->query->add_field(NULL, "'" . $this->definition['entity_type'] . "'", $alias_stub . '_entity_type');

  }

  /**
   * Load the entities for all fields that are about to be displayed.
   *
   * Notice that, although we load the entities for each chado field,
   * Drupal caches entities to ensure we don't get a performance hit per field,
   * just per row.
   */
  function post_execute(&$values) {
    if (!empty($values) AND isset($this->id_alias) AND isset($this->type_alias)) {

      // Foreach row in the view we want to grab the appropriate entity_id/type.
      $entity_ids = [];
      $keys = [];
      foreach ($values as $key => $object) {
        // Only load the entity if we can access the entity_id.
        if (isset($this->id_alias) AND isset($object->{$this->id_alias})) {
          $entity_ids[$object->{$this->type_alias}][] = $object->{$this->id_alias};
          $keys[$key] = $object->{$this->id_alias};
        }
      }

      // Now load the entities.
      foreach ($entity_ids as $type => $ids) {
        $entities[$type] = entity_load($type, $ids);
      }

      // Finally add the loaded entities and values back into the resultset for easy access.
      foreach ($keys as $row_id => $entity_id) {

        // First set the entities.
        foreach ($entities as $type => $objects) {
          $values[$row_id]->_chado_field_data[$type] = [
            'entity_type' => $type,
            'entity' => $objects[$entity_id],
          ];
        }

        // Then set the value of this field.
        $values[$row_id]->{$this->field_alias} = $this->render_field($objects[$entity_id], $this->definition['field_name'], $row_id);
      }
    }
  }

  /**
   * Render the field for display in the view.
   *
   * @param TripalEntity $entity
   *   The entity containing the field to be rendered.
   * @param string $field_name
   *   The name of the field to render.
   * @param integer $row_id
   *   The id of the row this field will be displayed in.
   *
   * @return string
   *   The rendered field.
   */
  function render_field($entity, $field_name, $row_id) {

    $display = [
      'type' => $this->options['type'],
      'settings' => (isset($this->options['settings'])) ? $this->options['settings'] : [],
      'label' => 'hidden',
      // Pass the View object in the display so that fields can act on it.
      'views_view' => $this->view,
      'views_field' => $this,
      'views_row_id' => $row_id,
    ];

    $langcode = LANGUAGE_NONE;
    $items = field_get_items($entity->type, $entity, $field_name);
    if (count($items) == 1) {
      $render_array = field_view_value($entity->type, $entity, $field_name, $items[0], $display, $langcode);
    }
    // @todo: handle fields with multiple values.
    else {
      $render_array = field_view_value($entity->type, $entity, $field_name, $items[0], $display, $langcode);
      drupal_set_message('Tripal Chado currently only supports views integration for single value fields. The first value has been shown.', 'warning');
    }

    return $render_array;
  }

  /**
   * @{inheritdoc}
   */
  function render($values) {

    $value = $this->get_value($values);

    if (is_array($value)) {
      return drupal_render($value);
    }
    else {
      return $this->sanitize_value($value);
    }
  }

  /**
   * Provide options for views ui admin specific to fields.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field = field_info_field($this->definition['field_name']);
    $formatters = _field_view_formatter_options($field['type']);

    $form['type'] = [
      '#type' => 'select',
      '#title' => t('Formatter'),
      '#options' => $formatters,
      '#default_value' => $this->options['type'],
      '#ajax' => [
        'path' => views_ui_build_form_url($form_state),
      ],
      '#submit' => ['views_ui_config_item_form_submit_temporary'],
      '#executes_submit_callback' => TRUE,
    ];

    // Get the currently selected formatter.
    $format = $this->options['type'];

    $formatter = field_info_formatter_types($format);
    if (!isset($this->options['settings'])) {
      $this->options['settings'] = [];
    }
    $settings = $this->options['settings'] + field_info_formatter_settings($format);

    // Provide an instance array for hook_field_formatter_settings_form().
    ctools_include('fields');
    $this->instance = ctools_fields_fake_field_instance($this->definition['field_name'], '_custom', $formatter, $settings);

    // Store the settings in a '_custom' view mode.
    $this->instance['display']['_custom'] = [
      'type' => $format,
      'settings' => $settings,
    ];

    // Get the settings form.
    $settings_form = ['#value' => []];
    $function = $formatter['module'] . '_field_formatter_settings_form';
    if (function_exists($function)) {
      $settings_form = $function($field, $this->instance, '_custom', $form, $form_state);
    }
    $form['settings'] = $settings_form;
  }

  /**
   * Define the options we are going to provide.
   */
  function option_definition() {
    $options = parent::option_definition();

    $field = field_info_field($this->definition['field_name']);
    $field_type = field_info_field_types($field['type']);

    $options['type'] = [
      'default' => $field_type['default_formatter'],
    ];
    $options['settings'] = [
      'default' => [],
    ];

    return $options;
  }
}
