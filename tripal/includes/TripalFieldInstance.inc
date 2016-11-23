<?php

class TripalFieldInstance {
  /**
   * Provides a form for the 'Field Settings' of an instance of this field.
   *
   * This function corresponds to the hook_field_instance_settings_form()
   * function of the Drupal Field API.
   *
   * Validation of the instance settings form is not supported by Drupal, but
   * the TripalField class does provide a mechanism for supporting validation.
   * To allow for validation of your setting form you must call the parent
   * in your child class:
   *
   * @code
   *   $element = parent::instanceSettingsForm();
   * @endcode
   *
   * Please note, the form generated with this function does not easily
   * support AJAX calls in the same way that other Drupal forms do.  If you
   * need to use AJAX you must manually alter the $form in your ajax call.
   * The typical way to handle updating the form via an AJAX call is to make
   * the changes in the form function itself but that doesn't work here.
   */
  public function instanceSettingsForm() {
    $settings = $this->instance['settings'];
    $element = array();

    //     $element['semantic_web'] = array(
    //       '#type' => 'textfield',
    //       '#title' => 'Semantic Web',
    //       '#description' => t('Each field must be associated with a term
    //           from a controlled vocabulary.  This allows computer programs to understand
    //           the data provided on this site.  Please be cautions changing these
    //           values.  Defaults are set by Tripal and sites that use the same
    //           terms can exchange information.'),
    //       '#collapsed' => TRUE,
    //       '#collapsible' => TRUE,
    //       '#tree' => TRUE,
    //     );
    $element['#field'] = $this->field;
    $element['#instance'] = $this->instance;
    $element['#element_validate'][] = 'tripal_field_instance_settings_form_validate';

    return $element;
  }
  /**
  * Provides validation of the instance settings form.
  *
  * There is no equivalent function in the Drupal Field API. Validation
  * of instance settings forms in Drupal is not supported. However, the
  * TripalField provides this function to fill the gap.  See the
  * documentation for the instanceSettingsForm() function for instructions
  * to support use of this function.
  *
  * @param $form
  * @param $form_state
  */
  public function instanceSettingsFormValidate($form, &$form_state) {

  }
}