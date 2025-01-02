<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class TripalWidgetSettingsForm.
 *
 * @package Drupal\tripal\Form
 *
 * @ingroup tripal
 */
class TripalWidgetSettingsForm extends FormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'tripal_widget_settings_form';
  }

  /**
   * Defines the settings form for Tripal field widgets.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   *
   * @return array
   *   Form definition array.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $settings = \Drupal::config('tripal.settings');

    $form['tripal_entity_settings']['#markup'] = 'Settings form for Tripal widgets.';

    // Defines the limit of records for a select. Above this value,
    // the form element will change to an autocomplete.
    // Supplying zero or less means always use autocomplete.
    $widget_select_limit = $form_state->getValue('widget_select_limit',
      $settings->get('tripal_entity_type.widget_select_limit'));
    // If nothing is set, use the default value
    if (is_null($widget_select_limit) or (trim($widget_select_limit) === '')) {
      $widget_select_limit = 50;
    }

    $form['widget_select_limit'] = [
      '#type' => 'textfield',
      '#title' => t('Maximum records for a select'),
      '#description' => t('The value here controls whether a widget select element uses a'
                        . ' dropdown select list, or an autocomplete.'
                        . ' A dropdown can be difficult to use and is a performance problem'
                        . ' if the number of records is large.'
                        . ' When the number of records is larger than the value entered here,'
                        . ' use an autocomplete if the field supports one.'
                        . ' Enter zero to indicate that an autocomplete should always be used.'),
      '#default_value' => $widget_select_limit,
      '#required' => FALSE,
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Save'),
    ];

    return $form;
  }

  /**
   * Validate the entered values.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $widget_select_limit = trim($form_state->getValue('widget_select_limit'));

    // Non-negative integers or an empty string are valid
    if (!preg_match('/^\d*$/', $widget_select_limit)) {
      $form_state->setErrorByName('widget_select_limit',
        t('This field must contain an integer value.'));
    }
  }

  /**
   * Form submission handler. Saves the form values to tripal settings.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $widget_select_limit = trim($form_state->getValue('widget_select_limit'));

    // Update configuration
    \Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.widget_select_limit', $widget_select_limit)
      ->save();

    $this->messenger()->addStatus('Settings have been saved.');
  }

}
