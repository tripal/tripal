<?php

/**
 * @file
 * Purpose: This Handler provides a file upload field by extending the
 * views_handler_filter object.
 *
 * @ingroup tripal_views_integration
 */
class tripal_views_handler_filter_sequence extends views_handler_filter {
 
 /**
   * Defines the value field in both the views filter options form
   *   and the exposed form
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    $this->value_form = array(
       '#type' => 'sequence_combo',
       '#title' => t('%label', array('%label' => $this->options['expose']['label'])),
       '#default_value' => $this->value,
       '#multiple' => FALSE,       
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
    $upstream = $form_state['values'][$this->field]['upstream'];
    $downstream = $form_state['values'][$this->field]['downstream'];       
     
    // we need the values provided by the user so that the field
    // handler can generate the results properly.  These values
    // will get stored in the view object.  This may not be the best place
    $this->view->sequence_q['upstream'] = $upstream;
    $this->view->sequence_q['downstream'] = $downstream;
  }
}
