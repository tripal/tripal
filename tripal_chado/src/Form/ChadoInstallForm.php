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

    // @testing @REMOVE
    module_load_include('inc', 'tripal_chado', 'src/LegacyIncludes/tripal_chado.install');

    // we want to force the version of Chado to be set properly
    // @upgrade $real_version = chado_get_version(TRUE);
    $real_version = 'unknown';

    // get the effective version.  Pass true as second argument
    // to warn the user if the current version is not compatible
    // @upgrade $version = chado_get_version(FALSE, TRUE);
    $version = 'unknown';

    $values = $form_state->getValues();
    if (array_key_exists('action_to_do', $values)) {
      if ($values['action_to_do'] == "Upgrade Chado v1.2 to v1.3") {
        $tables_list = implode(', ', [
          'analysis_cvterm',
          'analysis_dbxref',
          'analysis_pub',
          'analysis_relationship',
          'contactprop',
          'dbprop',
          'feature_contact',
          'featuremap_contact',
          'featuremap_dbxref',
          'featuremap_organism',
          'featuremapprop',
          'featureposprop',
          'library_contact',
          'library_expression',
          'library_expressionprop',
          'library_featureprop',
          'library_relationship',
          'library_relationship_pub',
          'nd_experiment_analysis',
          'organism_cvterm',
          'organism_cvtermprop',
          'organism_pub',
          'organism_relationship',
          'organismprop_pub',
          'phenotypeprop',
          'phylotreeprop',
          'project_analysis',
          'project_dbxref',
          'project_feature',
          'project_stock',
          'pubauthor_contact',
          'stock_feature',
          'stock_featuremap',
          'stock_library',
          'stockcollection_db',
        ]);
        $items = [
          'PostgreSQL version 9.1 is required to perform this upgrade. If your Tripal
           site uses an older version please upgrade before proceeding.',
          'A major change between Chado v1.2 and v1.3 is that primary and foreign
           keys were upgraded from integers to big integers. If your site has custom
           materialized views that will hold data derived from fields changed to
           big integers then you may need to alter the views to change the fields
           from integers to big integers and repopulate those views. If you have not
           added any materialized views you can ignore this issue.',
          'Custom PL/pgSQL functions that expect primary and
           foreign key fields to be integers will not work after the upgrade.
           Those functions will need to be altered to accept big integers. If you
           do not have any custom PL/pgSQL functions you can ignore this issue.',
          'PostgreSQL Views that use fields that are converted to big
           integers will cause this upgrade to fail.  You must first remove
           those views, perform the upgrade and then recreate them with the
           appropriate fields change to big integers. If you do not have custom
           PostgreSQL Views you can ignore this issue.',
          'Several new tables were added to Chado v1.3.  However, some groups have
           added these tables to their Chado v1.2 installation.  The Tripal upgrader
           will alter the primary and foreign keys of those tables to be "bigints"
           if they already exist but will otherwise leave them the same.  You should
           verify that any tables with Chado v1.3 names correctly match the v1.3 schema.
           Otherwise you may have problems using Tripal. If you have not added any
           Chado v1.3 tables to your Chado v1.2 database you can ignore this issue.
           These are the newly added tables:  ' .
          $tables_list . '.',
        ];
        $list = [
          '#theme' => 'item_list',
          '#title' => 'Please note: the upgrade of Chado from v1.2 to v1.3 may
              require several fixes to your database. Please review the following
              list to ensure a safe upgrade. The Tripal upgrader is
              not able to fix these problems automatically:',
          '#list_type' => 'ul',
          '#items' => $items,
          '#wrapper_attributes' => ['class' => 'container'],
        ];
        \Drupal::messenger()->addMessage($list, 'warning');
      }
      if ($values['action_to_do'] == "Install Chado v1.3" or
        $values['action_to_do'] == "Install Chado v1.2" or
        $values['action_to_do'] == "Install Chado v1.11") {
        \Drupal::messenger()->addMessage('Please note: if Chado is already installed it will
            be removed and recreated and all data will be lost. If this is
            desired or if this is the first time Chado has been installed
            you can ignore this issue.', 'warning');
      }
    }

    $form['current_version'] = [
      '#type' => 'item',
      '#title' => t("Current installed version of Chado:"),
      '#description' => $real_version,
    ];

    $form['action_to_do'] = [
      '#type' => 'select',
      '#title' => 'Installation/Upgrade Action',
      '#options' => [
        'Install Chado v1.3' => t('New Install of Chado v1.3 (erases all existing Chado data if Chado already exists)'),
        'Upgrade Chado v1.2 to v1.3' => t('Upgrade existing Chado v1.2 to v1.3 (no data is lost)'),
        'Install Chado v1.2' => t('New Install of Chado v1.2 (erases all existing Chado data if Chado already exists)'),
        'Upgrade Chado v1.11 to v1.2' => t('Upgrade existing Chado v1.11 to v1.2 (no data is lost)'),
        'Install Chado v1.11' => t('New Install of Chado v1.11 (erases all existing Chado data if Chado already exists)'),
      ],
      '#description' => t('Select an action to perform.'),
      '#required' => TRUE,
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

    // we want to force the version of Chado to be set properly
    // @upgrade $real_version = chado_get_version(TRUE);
    $real_version = 'unknown';
    
    // We do not want to allow re-installation of Chado if other
    // Tripal modules are installed.  This is because the install files
    // of those modules may add content to Chado and reinstalling Chado
    // removes that content which may break the modules.
    if ($values['action_to_do'] == "Install Chado v1.3" or
      $values['action_to_do'] == "Install Chado v1.2" or
      $values['action_to_do'] == "Install Chado v1.11") {
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
              are enabled.  You must fully uninstall the following modules if you
              would like to install or re-install chado.',
            '#list_type' => 'ul',
            '#items' => $list,
            '#wrapper_attributes' => ['class' => 'container'],
          ];
          $form_state->setErrorByName("action_to_do", $message);
        }
    }
    if ($values['action_to_do'] == "Upgrade Chado v1.11 to v1.2") {
      // Make sure we are already not at v1.2
      // @upgrade $real_version = chado_get_version(TRUE);
      if ($real_version == "1.2") {
        $form_state->setErrorByName("action_to_do", "You are already at v1.2.  There is no need to upgrade.");
      }
    }
    if ($values['action_to_do'] == "Upgrade Chado v1.2 to v1.3") {
      // Make sure we are already not at v1.3
      // @upgrade $real_version = chado_get_version(TRUE);
      if ($real_version == "1.3") {
        $form_state->setErrorByName("action_to_do", "You are already at v1.3.  There is no need to upgrade.");
      }
    }

    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    global $user;
    $action_to_do = trim($form_state->getValues()['action_to_do']);
    $args = [$action_to_do];

    $command = "drush php-eval \"module_load_include('inc', 'tripal_chado', 'src/LegacyIncludes/tripal_chado.install');
tripal_chado_install_chado('".$action_to_do."');\"";
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
