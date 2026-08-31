<?php

namespace Drupal\tripal_ws\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Configure Tripal Webservices settings for this site.
 */
class TripalWsSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_ws_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['tripal_ws.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['example'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Example'),
      '#description' => $this->t('Please type the word "example".'),
      '#default_value' => $this->config('tripal_ws.settings')->get('example'),
    ];
    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    if ($form_state->getValue('example') != 'example') {
      $form_state->setErrorByName('example', $this->t('The value is not correct. Instead enter "example" to get validation to pass.'));
    }
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $value = $form_state->getValue('example');
    $this->config('tripal_ws.settings')
      ->set('example', $value)
      ->save();
    $this->messenger()->addStatus("Setting the value of tripal_ws.settings.example to $value.");

    parent::submitForm($form, $form_state);
  }

}
