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
       '#type' => 'checkbox',
       '#title' => t('Provide fields for upstream and downstream'),
       '#default_value' => $this->value,
       '#multiple' => FALSE,
       '#description' => 
         t('Check here to provide form fields to allow the user to retrieve ' .
           'sequences beyond the feature residues.  This requires that the ' .
           'feature is aligned to a parent sequence and the optin to derive ' .
           'the sequence from the parent is turned on in the field settings.'),
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
    $form[$value] = $form['value'];
    unset($form[$value]['#title']);

    // add the upstream and downstream boxes
    $form['upstream'] = array(
       '#type' => 'textfield',
       '#title' => t('Get Upstream Bases'),
       '#description' => t('Specify the number of upstream bases to include in the sequnce'),
    );
    $form['downstream'] = array(
       '#type' => 'textfield',
       '#title' => t('Get Downstream Bases'),
       '#description' => t('Specify the number of upstream bases to include in the sequnce'),
    );
  }

  /**
   *  Validates the input form
   */
  function exposed_validate(&$form, &$form_state) {
     $upstream = $form_state['values']['upstream'];
     $downstream = $form_state['values']['downstream'];
     
     if($upstream < 0){
        form_set_error($form_state['values']['upstream'], 
           'Please provide a positive number for upstream bases');
     }
     if($upstream and !preg_match('/^\d+$/',$upstream)){
        form_set_error($form_state['values']['upstream'], 
           'Please provide a decimal number for upstream bases');
     }

     if($downstream < 0){
        form_set_error($form_state['values']['upstream'], 
           'Please provide a positive number for downstream bases');
     }
     if($downstream and !preg_match('/^\d+$/',$downstream)){
        form_set_error($form_state['values']['upstream'], 
           'Please provide a decimal number for downstream bases');
     }
     
     // we need the values provided by the user so that the field
     // handler can generate the results properly.  These values
     // will get stored in the view object.  This may not be the best place
     $this->view->sequence_q['upstream'] = $upstream;
     $this->view->sequence_q['downstream'] = $downstream;
  }
}
