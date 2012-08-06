<?php

/**
 * @file
 * Purpose: This Handler provides a file upload field by extending the
 * views_handler_filter object.
 *
 * @ingroup tripal_views_integration
 */
class tripal_views_handler_filter_textarea extends views_handler_filter {

  /**
   * Defines the value field in both the views filter options form
   *   and the exposed form
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
   * Ensures the upload field gets rendered when the filter is exposed. It also
   * changes the form type from a GET to a POST so that file uploads will work.
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
   *  Validates the input form
   */
  function exposed_validate(&$form, &$form_state) {
     //dpm($form_state);
  }

  /**
   *
   */
  function query() {
    $this->ensure_my_table();
    $field = "$this->table.$this->real_field";

    // get the form element value
    $value = $this->value[0];

    // the form element value has sub values, so get those
    $items =  $value['items'];
    $file_path = $value['file_path'];

    $holders = array();
    $values = array();

    // get the file upload content if one has been provided
    if ($file_path) {
      $fh = fopen($file_path, 'r');
      while ($line = fgets($fh)) {
        $items = trim($line);

        // remove extra spaces and new lines
        $items = preg_replace("/\s+,/", ",", $items);
        $items = preg_replace("/\s+\n/", "\n", $items);
        $items = preg_replace("/,\n/", "\n", $items);

        // add the values from this line to the values[] array
        $vals = preg_split("/[\n,]+/", $items);
        $values = array_merge($values, $vals);
      }
    }

    // if a file upload has not been provided then use the value in the textarea
    if ($items) {

      // remove extra spaces and new lines
      $items = preg_replace("/\s+,/", ",", $items);
      $items = preg_replace("/\s+\n/", "\n", $items);
      $items = preg_replace("/,\n/", "\n", $items);

      // in the event the user uploaded a file and provided items in the
      // textbox then we need to merge these two lists
      $vals = preg_split("/[\n,]+/", $items);
      $values = array_merge($values, $vals);
    }

    // iterate through all of the values and generate the corresponding
    // sprintf style holders
    for ($i = 0 ; $i < count($values); $i++) {
      $values[$i] = trim($values[$i]);
      $holders[] = "'%s'";
    }

    // if we have any values supplied then update the where clause for
    // the views query
    if (count($holders) > 0) {
      $where = "$field IN (" . implode(", ", $holders) . ")";
      $this->query->add_where($this->options['group'], $where, $values);
    }
  }
}
