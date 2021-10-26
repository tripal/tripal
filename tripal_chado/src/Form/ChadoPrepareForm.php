<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

class ChadoPrepareForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_prepare_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    if (\Drupal::state()->get('tripal_chado_is_prepared') == TRUE) {
      \Drupal::messenger()->addMessage('Your site is prepared.');
    }

    $form['instructions'] = [
      '#type' => 'item',
      '#title' => 'Prepare Drupal for Chado.',
      '#description' => t("Before a Drupal site can use Chado (via Tripal), both
        Chado and Drupal must be prepared a bit more.  Tripal will add some new
        materialized views, custom tables and controlled vocabularies to Chado.
        It will also add some management tables to Drupal and add some default
        content types for biological and ancillary data."),
    ];

    $form['advanced']['schema_name'] = [
      '#type' => 'textfield',
      '#title' => 'Chado Schema Name',
      '#required' => TRUE,
      '#description' => 'The name of the schema to install chado in.',
      '#default_value' => 'chado',
    ];

    $form['prepare-button'] = [
      '#type' => 'submit',
      '#value' => t('Prepare this site'),
      '#name' => 'prepare-chado',
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    if ($form_state->getTriggeringElement()['#name'] == 'prepare-chado') {
      $schema_name = trim($form_state->getValues()['schema_name']);
      $current_user = \Drupal::currentUser();
      $args = [$schema_name];

      tripal_add_job('Prepare Chado', 'tripal_chado',
        'tripal_chado_prepare_chado', $args, $current_user->id(), 10);
    }
  }
}
