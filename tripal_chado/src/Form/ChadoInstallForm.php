<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

/**
 * Class ChadoInstallForm.
 */
class ChadoInstallForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_install_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    // Add warnings to the admin based on their choice (as needed).
    $values = $form_state->getValues();
    if (array_key_exists('action_to_do', $values)) {
      if ($values['action_to_do'] == "Install Chado v1.3") {
        \Drupal::messenger()->addMessage(
            'Please note: if Chado is already installed it will
            be removed and recreated and all data will be lost. If this is
            desired or if this is the first time Chado has been installed
            you can ignore this issue.', 'warning');
      }
      elseif ($values['action_to_do'] == "Drop Chado Schema") {
        \Drupal::messenger()->addMessage(
            'Please note: all data will be lost in the schema you choose to
            remove. This is not reversible.', 'warning');
      }
    }

    $form['msg-top'] = [
      '#type' => 'item',
      '#markup' => 'Chado is a relational database schema that underlies many
      GMOD installations. It is capable of representing many of the general
      classes of data frequently encountered in modern biology such as sequence,
      sequence comparisons, phenotypes, genotypes, ontologies, publications,
      and phylogeny. It has been designed to handle complex representations of
      biological knowledge and should be considered one of the most
      sophisticated relational schemas currently available in molecular
      biology.',
      '#prefix' => '<blockquote>',
      '#suffix' => t('- <a href="@url">GMOD Chado Documentation</a></blockquote>',
        ['@url' => Url::fromUri('https://chado.readthedocs.io/en/rtd/')->toString()]),
    ];

    // Now that we support multiple chado instances, we need to list all the
    // currently installed ones here since they may be different versions.
    // @upgrade currently we have no way to pull out all chado installs.
    $rows = [];
    $installs = chado_get_installed_schemas();
    foreach($installs as $i) {
      $rows[] = [
        $i->schema_name,
        $i->version,
        \Drupal::service('date.formatter')->format($i->created),
        \Drupal::service('date.formatter')->format($i->updated)
      ];
    }
    if (!empty($rows)) {
      $form['current_version'] = [
        '#type' => 'table',
        '#caption' => 'Installed version(s) of Chado',
        '#header' => ['Schema Name', 'Chado Version', 'Created', 'Updated'],
        '#rows' => $rows,
      ];
    }
    else {
      $form['current_version'] = [
        '#type' => 'item',
        '#markup' => '<div class="messages messages--warning">
            <h2>Chado Not Installed</h2>
            <p>Please select an Install action below and click "Install/Upgrade Chado". We recommend you choose the most recent version of Chado.</p>
          </div>',
      ];
    }

    $form['msg-middle'] = [
      '#type' => 'item',
      '#markup' => t('<br /><p>Use the following drop-down to choose whether you want
      to install or upgrade Chado. You can use the advanced options to change
      the schema name for multi-chado install.</p>'),
    ];

    $form['action_to_do'] = [
      '#type' => 'select',
      '#title' => 'Installation/Upgrade Action',
      '#options' => [
        'Install Chado v1.3' => t('New Install of Chado v1.3 (erases all
          existing Chado data if this chado schema already exists).'),
        'Drop Chado Schema' => t('Remove Existing Chado (erases all existing
          chado data)'),
      ],
      '#required' => TRUE,
      "#empty_option" => t('- Select an action to perform -'),
      '#ajax' => [
        'callback' => '::ajaxFormVersionUpdate',
        'wrapper' => 'tripal_chado_load_form',
        'effect' => 'fade',
        'method' => 'replace',
        'disable-refocus' => FALSE,
      ],
    ];



    // Add some information to admin regarding chado installation.
    $info[] = 'Tripal Chado Integration now supports <strong>setting the schema
      name</strong> local Chado instances are installed in. In Tripal v3 and
      lower, the recommended name for your chado schema was <code>chado</code>
      and that is still the default. Note: Schema name cannot be changed once
      set.';
    $info[] = 'Additionally, you can now install <strong>multiple chado
    instances</strong>, although this is only recommended as needed. Examples
    where you may need multiple chado instances: (1) separate testing version of
    chado, (2) different chado instances for specific user groups (i.e. breeders
    of different crops), (3) both a public and private chado where Drupal
    permissions are not sufficient.';
    $info[] = 'To install multiple chado instances, submit this form once for
    each chado instance indicating a different schema name each time.
    <strong>Each chado instance must have a unique name.</strong>';
    $form['advanced'] = [
      '#type' => 'details',
      '#title' => 'Advanced Options',
      '#description' => '<p>' . implode ('</p><p>', $info) . '</p>',
    ];

    // Allow the admin to set the chado schema name.
    $form['advanced']['schema_name'] = [
      '#type' => 'textfield',
      '#title' => 'Chado Schema Name',
      '#required' => TRUE,
      '#description' => 'The name of the schema to install chado in.',
      '#default_value' => 'chado',
    ];

    $form['button'] = [
      '#type' => 'submit',
      '#value' => t('Install/Upgrade Chado'),
    ];

    $form['#prefix'] = '<div id="tripal_chado_load_form">';
    $form['#suffix'] = '</div>';

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();

    // We do not want to allow re-installation of Chado if other
    // Tripal modules are installed.  This is because the install files
    // of those modules may add content to Chado and reinstalling Chado
    // removes that content which may break the modules.
    //
    // Cannot do this and still allow multiple chado installs...
    // @todo add a hook for modules to add in to the prepare or install processes.

    // Schema name must be all lowercase with no special characters.
    // It should also be a single word.
    if (preg_match('/^[a-z][a-z0-9]+$/', $values['schema_name']) === 0) {
      $form_state->setErrorByName('schema_name',
        t('The schema name must be a single word containing only lower case letters or numbers and cannot begin with a number.'));
    }

    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $action_to_do = trim($form_state->getValues()['action_to_do']);
    $schema_name = trim($form_state->getValues()['schema_name']);
    $args = [$action_to_do];

    switch ($action_to_do) {
      case 'Install Chado v1.3':
        $args = ['install', 1.3, $schema_name];
        break;
      case 'Drop Chado Schema':
        $args = ['drop', $schema_name];
        break;
    }

    $current_user = \Drupal::currentUser();
    tripal_add_job($action_to_do, 'tripal_chado',
        'tripal_chado.chadoInstaller', $args, $current_user->id(), 10);
  }

  /**
   * Ajax callback: triggered when version is selected
   * to provide additional feedback and help text.
   *
   * @param array $form
   * @param array $form_state
   * @return array
   *   Portion of the form to re-render.
   */
  public function ajaxFormVersionUpdate($form, $form_state) {
    return $form;
  }

}
