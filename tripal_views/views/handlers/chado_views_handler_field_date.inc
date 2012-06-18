<?php

/**
 * @file
 * A chado wrapper for the views_handler_field_date.
 *
 * Handles fields which may be aggregated during the chado join process. This field
 * will render an aggregated field as a pre_rendered list and will dynamically detect
 * whether the field is aggregated or not.
 */
class chado_views_handler_field_date extends views_handler_field_date {

  function init(&$view, $options) {
    include_once('chado_wrapper_functions.inc');
    parent::init($view, $options);
  }

  /**
   * Defines the defaults for the options form
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['type'] = array('default' => 'separator');
    $options['separator'] = array('default' => ', ');

    return $options;
  }

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['type'] = array(
      '#type' => 'radios',
      '#title' => t('Display type'),
      '#options' => array(
        'ul' => t('Unordered list'),
        'ol' => t('Ordered list'),
        'separator' => t('Simple separator'),
      ),
      '#default_value' => $this->options['type'],
    );

    $form['separator'] = array(
      '#type' => 'textfield',
      '#title' => t('Separator'),
      '#default_value' => $this->options['separator'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('radio:options[type]' => array('separator')),
    );
  }

  /**
   * Determines whether the current field is aggregated or not
   * Note: The parent::query() takes care of adding the field to the query, etc.
   */
  function query() {
    parent::query();
    $this->aggregated = chado_wrapper_is_aggregated_by_join($this);
  }

  /**
   * Splits the aggregated values up for use in rendering
   */
  function pre_render(&$values) {

    // further check the results to see if this field is a postgresql array
    $this->aggregated = chado_wrapper_is_aggregated_by_result($this, $values);

    // Split Aggregated Results
    chado_wrapper_split_array_agg_results($this, $values);

  }

  /**
   * Render the field.
   *
   * Note: Checks to see if we have an array or simple field. If we have an array, then
   *   split it up and render each part using the parent render functionality.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  function render($values) {
    return chado_wrapper_render_items($this, $values);
  }

  function parent_render($val) {
    return parent::render($val);
  }

}