<?php

/**
 * @file
 * Contains the basic 'entity' field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a entity.
 * Definition terms:
 * - link_to_entity default: Should this field have the checkbox "link to
 * entity" enabled by default.
 *
 * @ingroup views_field_handlers
 */
class tripal_views_handler_field_entity extends tripal_views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    // Don't add the additional fields to groupby
    if (!empty($this->options['link_to_entity'])) {
      $this->additional_fields['id'] = [
        'table' => 'tripal_entity',
        'field' => 'id',
      ];
      if (module_exists('translation')) {
        $this->additional_fields['language'] = [
          'table' => 'entity',
          'field' => 'language',
        ];
      }
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_entity'] = [
      'default' => isset($this->definition['link_to_entity default']) ? $this->definition['link_to_entity default'] : FALSE,
      'bool' => TRUE,
    ];
    return $options;
  }

  /**
   * Provide link to entity option
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_entity'] = [
      '#title' => t('Link this field to the original piece of content'),
      '#description' => t("Enable to override this field's links."),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_entity']),
    ];

    parent::options_form($form, $form_state);
  }

  /**
   * Get the value that's supposed to be rendered.
   *
   * This api exists so that other modules can easy set the values of the field
   * without having the need to change the render method as well.
   *
   * @param $values
   *   An object containing all retrieved values.
   * @param $field
   *   Optional name of the field where the value is stored.
   */
  function get_value($values, $field = NULL) {

    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;

    // For some reason the alias isn't being added to the $this->aliases
    // variable when the user clicks the checkbox 'Link this field to the
    // original piece of content'.  That may need to be investigated and is
    // probably a side effect of using the  views_handler_field as a parent
    // class that is expecting something more for SQL. We don't use aliases
    // for fields so the following if statement will fix the problem.
    if (!$alias) {
      $alias = $this->field_alias;
    }
    if (isset($values->{$alias})) {
      return $values->{$alias}->id;
    }
  }

  /**
   * Render whatever the data is as a link to the entity.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_entity']) && !empty($this->additional_fields['id'])) {
      if ($data !== NULL && $data !== '') {
        $alias = $this->field_alias;
        if (entity_access('view', 'TripalEntity', $values->{$alias})) {
          $this->options['alter']['make_link'] = TRUE;
          $this->options['alter']['path'] = "bio_data/" . $this->get_value($values, 'id');
          if (isset($this->aliases['language'])) {
            $languages = language_list();
            $language = $this->get_value($values, 'language');
            if (isset($languages[$language])) {
              $this->options['alter']['language'] = $languages[$language];
            }
            else {
              unset($this->options['alter']['language']);
            }
          }
        }
      }
      else {
        $this->options['alter']['make_link'] = FALSE;
      }
    }
    return $data;
  }

  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
