<?php
/**
 * @file
 * Contants tripal_views_handler_filter_textarea Filter Handler
 */

/**
 * This Handler provides a file upload field by extending the
 * views_handler_filter object.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_textarea extends views_handler_filter {

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    $this->value_form = array(
       '#type' => 'textarea',
       '#title' => t('%label', array('%label' => $this->options['expose']['label'])),
       '#default_value' => $this->value,
       '#multiple' => FALSE,
       '#description' => t('Provide search values for %label. Please place each search
        item on a separate line or separated by commas.', array('%label' => $this->options['expose']['label'])),
    );
    $form['value'] = &$this->value_form;
  }

  /**
   * {@inheritdoc}
   */
  function exposed_form(&$form, &$form_state) {

    // don't do anything if the form isn't exposed.
    if (empty($this->options['exposed'])) {
      return;
    }
    // rebuild the form elements
    $value = $this->options['expose']['identifier'];
    $this->value_form($form, $form_state);

    $form[$value] = $form['value'];
    unset($form[$value]['#title']);


    $this->exposed_translate($form[$value], 'value');

    if ($value != 'value') {
      unset($form['value']);
    }

  }

  /**
   * {@inheritdoc}
   */
  function query() {

    // make optional
    // if it is not set or empty then don't restrict the query
    if (!$this->value) {
      return;
    }

    $this->ensure_my_table();
    $field = "$this->table.$this->real_field";

    // get the form element value
    if (is_array($this->value)) {
      $value = $this->value[0];
    }
    else {
      $value = $this->value;
    }

    // remove extra spaces and new lines
    $value = preg_replace("/\s+,/", ",", $value);
    $value = preg_replace("/\s+\n/", "\n", $value);
    $value = preg_replace("/,\n/", "\n", $value);
    $values = preg_split("/[\n,]+/", $value);
    $values = array_filter($values);

    if (sizeof($values)) {
      $operator = 'IN';
    }
    else {
      $operator = '=';
    }

    if (!empty($values)) {
      $this->query->add_where($this->options['group'], $field, $values, $operator);
    }
  }
}
