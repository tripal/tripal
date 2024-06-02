<?php

namespace Drupal\tripal\Form;

use Drupal;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class TripalEntitySettingsForm.
 *
 * @package Drupal\tripal\Form
 *
 * @ingroup tripal
 */
class TripalEntitySettingsForm extends FormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'tripal_entity_settings';
  }

  /**
   * Defines the settings form for Tripal Content entities.
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
    $settings = Drupal::config('tripal.settings');

    $form['tripal_entity_settings']['#markup'] = 'Settings form for Tripal Content entities.';

    // Provide overall server consumption (and space remaining)
    $allowed_title_tags = $form_state->getValue('allowed_title_tags',
      $settings->get('tripal_entity_type.allowed_title_tags'));

    $form['allowed_title_tags'] = [
      '#type' => 'textfield',
      '#title' => t('HTML tags allowed in page titles'),
      '#description' => t('A list of HTML tags that can be used in page titles.'
                        . ' Enter one or more tags separated by spaces, or leave blank to disable HTML tag rendering.'
                        . ' Any tag not in this list will be escaped if present in a page title.'),
      '#default_value' => $allowed_title_tags,
      '#required' => FALSE,
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Save'),
    ];

    return $form;
  }

  /**
   * Validate the list of tags, only letters and spaces allowed.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $allowed_title_tags = $form_state->getValue('allowed_title_tags');

    if (!preg_match("/^[A-Za-z ]*$/", $allowed_title_tags)) {
      $form_state->setErrorByName('allowed_title_tags',
        t('Only letters and spaces can be used.'));
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
    $allowed_title_tags = $form_state->getValue('allowed_title_tags');

    # trim, convert to lower case, and collapse multiple spaces for consistency
    $allowed_title_tags = strtolower(trim($allowed_title_tags));
    $allowed_title_tags = preg_replace('/ +/', ' ', $allowed_title_tags);

    // Update configuration
    Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('tripal_entity_type.allowed_title_tags', $allowed_title_tags)
      ->save();

    $this->messenger()->addStatus('Settings have been saved.');
  }

}
