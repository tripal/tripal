<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Task\ChadoInstaller;

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

    $chado = new ChadoConnection();

    $form['msg-top'] = [
      '#type' => 'item',
      '#markup' => t('@chado is a relational database schema for storing molecular
        biological and ancillary data. It is used in many tools. Tripal
        uses Chado as a datastore for biological content.  If Chado is not installed
        then click the button below to install it.  You may also use the advanced
        options to use a specific PostgreSQL schema name for Chado or
        install multiple Chado instances if desired.', [
          '@chado' => Link::fromTextAndUrl('Chado', Url::fromUri('https://chado.readthedocs.io/en/rtd/'))->toString(),
        ]),
    ];

    // Now that we support multiple chado instances, we need to list all the
    // currently installed ones here since they may be different versions.
    $rows = [];
    $instances = $chado->getAvailableInstances();

    // Sort instances.
    uasort($instances, function ($a, $b) {
      $order = 0;
      // Sort by test/reservation status first.
      if ($a['is_test'] || $a['is_reserved']) {
        if ($b['is_test'] || $b['is_reserved']) {
          // Then sort by schema name.
          $order = strcasecmp($a['schema_name'], $b['schema_name']);
        }
        else {
          $order = 1;
        }
      }
      else {
        if ($b['is_test'] || $b['is_reserved']) {
          $order = -1;
        }
        else {
          // Then sort by Tripal integration.
          if ($a['integration']) {
            if ($b['integration']) {
              // Then sort by schema name.
              $order = strcasecmp($a['schema_name'], $b['schema_name']);
            }
            else {
              $order = -1;
            }
          }
          else {
            if ($b['integration']) {
              $order = 1;
            }
            else {
              // Then sort by schema name.
              $order = strcasecmp($a['schema_name'], $b['schema_name']);
            }
          }
        }
      }
      return $order;
    });

    // Get the default Chado instance.
    $default_chado = NULL;
    if ($chado->schema()->schemaExists()) {
      $default_chado =$chado->getSchemaName();
    }

    // Determine if the default Chado is integrated with Tripal.
    $default_integrated = False;
    $none_integrated = True;
    if ($default_chado && $instances[$default_chado]['integration']) {
      $default_integrated = True;
    }

    // Build the table of Chado installations.
    foreach ($instances as $schema_name => $details) {

      // Integrated schemas and non-integrated have different informations.
      if ($details['integration']) {
        $none_integrated = False;
        $rows[] = [
          $details['integration']['schema_name'] . ($default_chado == $schema_name ? ' (default)' : ''),
          $details['integration']['version'],
          $details['has_data'] ? $this->t('Yes') : $this->t('No'),
          $this->t('Yes'),
          \Drupal::service('date.formatter')->format($details['integration']['created']),
          \Drupal::service('date.formatter')->format($details['integration']['updated']),
        ];
      }
      else {
        // @todo: add a row style for test schemas (to "gray" them).
        $rows[] = [
          $details['schema_name'],
          $details['version'],
          $details['has_data'] ? $this->t('Yes') : $this->t('No'),
          $details['is_test'] ? $this->t('Test') : $this->t('No'),
          '',
          '',
        ];
      }
    }

    $form['existing_instances'] = [
      '#type' => 'table',
      '#header' => ['Schema Name', 'Chado Version', 'Has data', 'In Tripal', 'Created', 'Updated'],
      '#rows' => $rows,
      '#empty' => 'There are no instances of Chado available.  Please submit this form to install Chado.'
    ];

    if (count($rows) > 0 and empty($default_chado)) {
      \Drupal::messenger()->addWarning(t('Chado is installed but no default
        schema was set. Please @select or install a new default Chado instance.',
          ['@select' => Link::fromTextAndUrl('select a default Chado schema',
              Url::fromUri('internal:/admin/tripal/storage/chado/manager'))->toString()],
      ));
    }
    if (count($rows) > 0 and $none_integrated) {
      \Drupal::messenger()->addWarning(t('There is no Chado installation that
        has been integratred with Tripal. Please @integrate it into Tripal.',
          ['@integrate' => Link::fromTextAndUrl('add',
              Url::fromUri('internal:/admin/tripal/storage/chado/manager'))->toString()],
          ));
    }
    if (count($rows) > 0 and $default_chado and !$default_integrated) {
      \Drupal::messenger()->addWarning(t('The default Chado schema is not
        integrated with Tripal. Please @integrate it into Tripal.',
          ['@integrate' => Link::fromTextAndUrl('add',
              Url::fromUri('internal:/admin/tripal/storage/chado/manager'))->toString()],
      ));
    }

    if (count($rows) == 0) {
      \Drupal::messenger()->addWarning('Chado is not installed.');
    }

    $form['advanced'] = [
      '#type' => 'details',
      '#title' => 'Advanced Options',
    ];

    // Allow the admin to set the chado schema name.
    $form['advanced']['schema_name'] = [
      '#type' => 'textfield',
      '#title' => t('Chado Schema Name'),
      '#required' => TRUE,
      '#default_value' => 'chado',
      '#description' => t('By default, the PostgreSQL schema name for Chado
        is "chado".  Change this if you perfer a different name.
        Additionally, you may install multiple instances of Chado by submitting
        this form once for each Chado instance and providing a unique schema
        name each time. Examples cases when you may want multiple chado
        instances are for a separate testing version, if different chado
        instances are needed for different user groups (e.g., breeders of
        different crops) or both a public and private chado).'),
    ];

    $form['button'] = [
      '#type' => 'submit',
      '#value' => $this->t(
        'Install Chado @version',
        ['@version' => ChadoInstaller::DEFAULT_CHADO_VERSION]
      ),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $tripal_dbx = \Drupal::service('tripal.dbx');

    // We do not want to allow re-installation of Chado if other
    // Tripal modules are installed. This is because the install files
    // of those modules may add content to Chado and reinstalling Chado
    // removes that content which may break the modules.
    //
    // Cannot do this and still allow multiple chado installs...
    // @todo: add a hook for modules to add in to the prepare or install
    // processes.
    // It may be solved by a method added to ChadoConnection class that would
    // check "locked" Chado instances. It may rely on a flag stored into Drupal
    // config or state? And/or as proposed, by an event/hook to let modules
    // react.

    // Check for schema name issues.
    $issue = $tripal_dbx->isInvalidSchemaName($values['schema_name']);
    if ($issue) {
      $form_state->setErrorByName(
        'schema_name',
        $issue
      );
    }

    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $schema_name = trim($values['schema_name']);
    $current_user = \Drupal::currentUser();
    $args = [$schema_name, ChadoInstaller::DEFAULT_CHADO_VERSION];

    \Drupal::service('tripal.job')->create([
      'job_name' => t('Install Chado @version', ['@version' => ChadoInstaller::DEFAULT_CHADO_VERSION]),
      'modulename' => 'tripal_chado',
      'callback' => 'tripal_chado_install_chado',
      'arguments' => $args,
      'uid' => $current_user->id(),
    ]);
  }
}
