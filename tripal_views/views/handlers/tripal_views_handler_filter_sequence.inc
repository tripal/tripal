<?php
/**
 * @file
 * Contains tripal_views_handler_filter_sequence Filter handler
 */

/**
 * This Handler provides a file upload field by extending the
 * views_handler_filter object.
 *
 * @ingroup tripal_views
 */
class tripal_views_handler_filter_sequence extends views_handler_filter {

  /**
   * {@inheritdoc}
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
   * {@inheritdoc}
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
   * {@inheritdoc}
   */
  function query() {
    $this->ensure_my_table();

    $upstream = $this->value[0]['upstream'];
    $downstream = $this->value[0]['downstream'];

    // we need the values provided by the user so that the field
    // handler can generate the results properly.  Saving these as session
    // variables may not be the best way but it works.
    if ($upstream) {
      $_SESSION['upstream'] = $upstream;
    }
    if ($downstream) {
      $_SESSION['downstream'] = $downstream;
    }
  }
}
