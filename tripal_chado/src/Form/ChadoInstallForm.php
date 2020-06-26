<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

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

    // Retrieve the name of the current schema.
    $schema_name = chado_get_schema_name('chado');
    // We want to force the version of Chado to be set properly.
    $real_version = chado_get_version(TRUE, FALSE, $schema_name);
    // get the effective version.  Pass true as second argument
    // to warn the user if the current version is not compatible.
    $version = chado_get_version(FALSE, TRUE, $schema_name);

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
    }

    // Add some information to admin regarding chado installation.
    $info[] = 'Tripal Chado Integration now supports <strong>setting the schema name</strong> local Chado instances are installed in. In Tripal v3 and lower, the recommended name for your chado schema was <code>chado</code> and that is still the default. Note: Schema name cannot be changed once set.';
    $info[] = 'Additionally, you can now install <strong>multiple chado instances</strong>, although this is only recommended as needed. Examples where you may need multiple chado instances: (1) separate testing version of chado, (2) different chado instances for specific user groups (i.e. breeders of different crops), (3) both a public and private chado where Drupal permissions are not sufficient.';
    $info[] = 'To install multiple chado instances, submit this form once for each chado instance indicating a different schema name each time. <strong>Each chado instance must have a unique name and only one instance can be used at a time.</strong>';
    $form['info'] = [
      '#type' => 'markup',
      '#markup' => '<p>' . implode ('</p><p>', $info) . '</p>',
    ];

    // Now that we support multiple chado instances, we need to list all the
    // currently installed ones here since they may be different versions.
    // @upgrade currently we have no way to pull out all chado installs.
    $form['current_version'] = [
      '#type' => 'table',
      '#caption' => 'Installed version(s) of Chado',
      '#header' => ['Schema Name', 'Chado Version'],
      '#rows' => [
        [$schema_name, $real_version],
      ],
    ];

    // Add a sub-header.
    $form['subheader'] = [
      '#type' => 'markup',
      '#markup' => '<br /><h2>Chado Installation</h2>',
    ];

    // Allow the admin to set the chado schema name.
    $form['schema_name'] = [
      '#type' => 'textfield',
      '#title' => 'Chado Schema Name',
      '#required' => TRUE,
      '#description' => 'The name of the schema to install chado in.',
      '#default_value' => $schema_name,
    ];

    $form['action_to_do'] = [
      '#type' => 'select',
      '#title' => 'Installation/Upgrade Action',
      '#options' => [
        'Install Chado v1.3' => t('New Install of Chado v1.3 (erases all existing Chado data if Chado already exists with the same schema name).'),
      ],
      '#description' => t('Select an action to perform.'),
      '#required' => TRUE,
      '#default_value' => 0,
      '#ajax' => [
        'callback' => '::ajaxFormVersionUpdate',
        'wrapper' => 'tripal_chado_load_form',
        'effect' => 'fade',
        'method' => 'replace',
        'disable-refocus' => FALSE,
      ],
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
    if ($values['action_to_do'] == "Install Chado v1.3") {
        $modules = \Drupal::service('extension.list.module')->getAllAvailableInfo();
        $list = [];
        foreach ($modules as $mname => $module) {
          if (array_key_exists('dependencies', $module) and in_array('tripal:tripal_chado', $module['dependencies'])) {
            $list[] = $module['name'] . " ($mname)";
          }
        }
        if (count($list) > 0) {
          $message = [
            '#theme' => 'item_list',
            '#title' => 'Chado cannot be installed while other Tripal modules
              are enabled.  You must fully uninstall the following modules if
              you would like to install or re-install chado.',
            '#list_type' => 'ul',
            '#items' => $list,
            '#wrapper_attributes' => ['class' => 'container'],
          ];
          $form_state->setErrorByName("action_to_do", $message);
        }
    }
    /*
    if ($values['action_to_do'] == "Upgrade Chado v1.2 to v1.3") {
      // Make sure we are already not at v1.3
      $real_version = chado_get_version(TRUE);
      if ($real_version == "1.3") {
        $form_state->setErrorByName("action_to_do", "You are already at v1.3.  There is no need to upgrade.");
      }
    }
    */

    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    global $user;
    $action_to_do = trim($form_state->getValues()['action_to_do']);
    $schema_name = trim($form_state->getValues()['schema_name']);
    $args = [$action_to_do];

    $command = "drush php-eval \""
      . "\Drupal::service('tripal_chado.chadoInstaller')"
      . "->install(1.3, '".$schema_name."');\"";
    $message = [
      '#markup' => '<strong>Must upgrade Tripal Jobs system first. In the meantime,
        execute the following drush command: </strong><pre>'.$command.'</pre>',
    ];
    \Drupal::messenger()->addMessage($message, 'warning');
    // @upgrade $includes = [module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.install')];
    // @upgrade tripal_add_job($action_to_do, 'tripal_chado',
    //  'tripal_chado_install_chado', $args, $user->uid, 10, $includes);
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
