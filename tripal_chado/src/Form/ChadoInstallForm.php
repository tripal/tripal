<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\tripal\TripalDBX\TripalDbxl;
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
      '#markup' => 'Chado is a relational database schema that underlies many
        GMOD installations. It is capable of representing many of the general
        classes of data frequently encountered in modern biology such as sequence,
        sequence comparisons, phenotypes, genotypes, ontologies, publications,
        and phylogeny. It has been designed to handle complex representations of
        biological knowledge and should be considered one of the most
        sophisticated relational schemas currently available in molecular
        biology.',
      '#prefix' => '<blockquote>',
      '#suffix' => $this->t('- <a href="@url">GMOD Chado Documentation</a></blockquote>',
        ['@url' => Url::fromUri('https://chado.readthedocs.io/en/rtd/')->toString()]),
    ];

    $form['msg-warning'] = [
      '#type' => 'item',
      '#markup' => 'Please note: if Chado is already installed it will
            be removed and recreated and all data will be lost. If this is
            desired or if this is the first time Chado has been installed
            you can ignore this issue.',
      // @todo:use CSS class "color-warning".
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
    foreach ($instances as $schema_name => $details) {
      // Integrated schemas and non-integrated have different informations.
      if ($details['integration']) {
        // @todo: row style should highlight it is integrated.
        // @todo: add a "check" sign in front of default Chado schema.
        $rows[] = [
          $details['integration']['schema_name'],
          $details['integration']['version'],
          // @todo: use CSS class ".system-status-counter__status-icon--checked"
          $details['has_data'] ? $this->t('Yes') : $this->t('No'),
          // @todo: use CSS class ".system-status-counter__status-icon--checked"
          $this->t('Yes'),
          \Drupal::service('date.formatter')->format(
            $details['integration']['created']
          ),
          \Drupal::service('date.formatter')->format(
            $details['integration']['updated']
          ),
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
    if (!empty($rows)) {
      $form['existing_instances'] = [
        '#type' => 'table',
        '#caption' => 'Existing Chado instances',
        '#header' => ['Schema Name', 'Chado Version', 'Has data', 'In Tripal', 'Created', 'Updated'],
        '#rows' => $rows,
      ];
      // Check if a default Chado instance is integrated.
      if ($chado->schema()->schemaExists()
          && $instances[$chado->getSchemaName()]['integration']) {
        $form['current_version'] = [
          '#type' => 'item',
          '#markup' => '<div class="messages messages--status">
              <p>Chado is installed in "'
              . $chado->getSchemaName()
              . '" (default Chado schema).</p>
            </div>',
        ];
      }
      // Check if a schema is integrated but not set as default.
      elseif ($instances[$rows[0][0]]['integration']) {
        $form['current_version'] = [
          '#type' => 'item',
          '#markup' => '<div class="messages messages--warning">
              <h2>Chado is installed but no default shcema was set</h2>
              <p>Please select a default Chado schema using the Chado management menu.</p>
            </div>',
        ];
      }
      else {
        $form['current_version'] = [
          '#type' => 'item',
          '#markup' => '<div class="messages messages--warning">
              <h2>Chado not integrated with Tripal</h2>
              <p>Please integrate an existing Chado schema into Tripal using the Chado management menu or install a new default Chado instance. We recommend you use or upgrade to the most recent version of Chado.</p>
            </div>',
        ];
      }
    }
    else {
      $form['current_version'] = [
        '#type' => 'item',
        '#markup' => '<div class="messages messages--warning">
            <h2>Chado Not Installed</h2>
            <p>Please install a new Chado schema. We recommend you choose the most recent version of Chado.</p>
          </div>',
      ];
    }

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
    tripal_add_job(
      t('Install Chado ' . ChadoInstaller::DEFAULT_CHADO_VERSION),
      'tripal_chado',
      'tripal_chado_install_chado',
      $args,
      $current_user->id(),
      10
    );
  }
}
