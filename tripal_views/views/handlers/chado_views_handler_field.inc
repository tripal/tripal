<?php

/**
 * @file
 * A chado wrapper for the views_handler_field.
 *
 * Handles fields which may be aggregated during the chado join process. This field
 * will render an aggregated field as a pre_rendered list and will dynamically detect
 * whether the field is aggregated or not.
 */
class chado_views_handler_field extends views_handler_field {

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

    // Add a link to node checkbox
    // but only if this base table is linked to a node and this field is from the base_table
    if (tripal_core_is_tripal_node_type($this->table) && $this->table == $this->view->base_table) {
      // If there is a Node: NID field then show a link to node checkbox
      if (isset($this->view->display['default']->display_options['fields']['nid'])) {
        $form['link_to_node'] = array(
          '#type' => 'checkbox',
          '#title' => t('Link to Node'),
          '#description' => t('If a given row is associated with a drupal node then '
            .'this field will appear as a link, linking the user to that node. Otherwise,'
            .' no link will be displayed.'),
          '#default_value' => $this->options['link_to_node'],
        );
      }
      // Otherwise inform the user that they need to add a Node:Nid field
      // to get this functionality
      else {
        $form['link_to_node'] = array(
          '#type' => 'item',
          '#value' => "This field has the ability to link to it's corresponding node. "
            . "However, you first need to add the NID field associated with the node. "
            . "Simple set the NID field to hidden when adding it to ensure it's not "
            . "shown in the resulting view."
        );
      }
    }

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
  function pre_render($values) {

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
    if ($this->options['link_to_node']) {
      $link_text = chado_wrapper_render_items($this, $values);
      return $this->render_node_link($link_text, $values);
    }
    else {
      return chado_wrapper_render_items($this, $values);
    }
  }

  /**
   * Will render the supplied text as a link to the node
   *
   * @param $link_text
   *  The text to render as a link (ie; the text that would normally become underlined
   *
   * @return
   *  A rendered link to the node based on the nid field
   */
  function render_node_link($link_text, $values) {

    $node_field = $this->view->field['nid'];
    if (isset($node_field->aliases['nid'])) {
      $nid = $values->{$node_field->aliases['nid']};
    }

    if ($nid) {
      return l($link_text, 'node/' . $nid);
    }
    else {
      return $link_text;
    }
  }

  function parent_render($val) {
    return parent::render($val);
  }

}