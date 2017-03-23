<?php

/**
 * @file
 * Contains the basic 'entity' field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a entity.
 * Definition terms:
 * - link_to_entity default: Should this field have the checkbox "link to entity" enabled by default.
 *
 * @ingroup views_field_handlers
 */
class tripal_views_handler_field_entity extends tripal_views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    // Don't add the additional fields to groupby
    if (!empty($this->options['link_to_entity'])) {
      $this->additional_fields['id'] = array('table' => 'tripal_entity', 'field' => 'id');
      if (module_exists('translation')) {
        $this->additional_fields['language'] = array('table' => 'entity', 'field' => 'language');
      }
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_entity'] = array('default' => isset($this->definition['link_to_entity default']) ? $this->definition['link_to_entity default'] : FALSE, 'bool' => TRUE);
    return $options;
  }

  /**
   * Provide link to entity option
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_entity'] = array(
      '#title' => t('Link this field to the original piece of content'),
      '#description' => t("Enable to override this field's links."),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_entity']),
    );

    parent::options_form($form, $form_state);
  }

  /**
   * Render whatever the data is as a link to the entity.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_entity']) && !empty($this->additional_fields['id'])) {
      if ($data !== NULL && $data !== '') {
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
