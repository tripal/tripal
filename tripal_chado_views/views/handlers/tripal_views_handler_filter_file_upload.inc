<?php
/**
 * @file
 * Contains tripal_views_handler_filter_file_upload filter handler
 */

/**
 * This Handler provides a file upload field by extending the
 * views_handler_filter object.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_file_upload extends views_handler_filter {

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    $this->value_form = array(
       '#type' => 'file_upload_combo',
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

    // since this is an exposed form we want to enable file uploads by
    // setting the 'enctype' attribute and the method to POST
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['#method'] = 'POST';
    $this->exposed_translate($form[$value], 'value');

    if ($value != 'value') {
      unset($form['value']);
    }

  }

  /**
   * {@inheritdoc}
   */
  function query() {
    $this->ensure_my_table();
    $field = "$this->table.$this->real_field";

    // get the form element values
    $value = $this->value[0];
    $values = $value['items_array'];

    // trim all values supplied
    foreach ($values as $k => $val) {
      $values[$k] = trim($val);
    }

    // Determine the operator to use depending on # of items
    if (sizeof($values)) {
      $operator = 'IN';
    }
    else {
      $operator = '=';
    }

    // if we have any items then add the where
    if (!empty($values)) {
      $this->query->add_where($this->options['group'], $field, $values, $operator);
    }
  }
}
