<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\tripal\TripalDBX;

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

    $form['advanced'] = [
      '#type' => 'details',
      '#title' => 'Advanced Options',
    ];


    $chado_schemas = [];
    $chado = \Drupal::service('tripal_chado.database');
    foreach ($chado->getAvailableInstances() as $schema_name => $details) {
      $chado_schemas[$schema_name] = $schema_name;
    }
    $default_chado = $chado->getSchemaName();

    $form['advanced']['schema_name'] = [
      '#type' => 'select',
      '#title' => 'Chado Schema Name',
      '#required' => TRUE,
      '#description' => 'Select one of the installed Chado schemas to prepare..',
      '#options' => $chado_schemas,
      '#default_value' => $default_chado,
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

      \Drupal::service('tripal.job')->create([
        'job_name' => t('Prepare Chado'),
        'modulename' => 'tripal_chado',
        'callback' => 'tripal_chado_prepare_chado',
        'arguments' => $args,
        'uid' => $current_user->id()
      ]);
    }
  }
}
