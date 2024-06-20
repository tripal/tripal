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
 * Class ChadoManagerForm.
 */
class ChadoManagerForm extends FormBase {

  /**
   * @defgroup chado_manager_form_tasks Form task names.
   * @{
   * Names used to identify form tasks in Chado manager form.
   */

  /**
   * Rename Chado task identifier.
   */
  public const RENAME_TASK = 'rename';

  /**
   * Set default Chado schema task identifier.
   */
  public const SET_DEFAULT_TASK = 'set_default';

  /**
   * Integrate Chado task identifier.
   */
  public const INTEGRATE_TASK = 'integrate';

  /**
   * Clone Chado task identifier.
   */
  public const CLONE_TASK = 'clone';

  /**
   * Drop Chado task identifier.
   */
  public const DROP_TASK = 'remove';

  /**
   * @} End of "defgroup chado_manager_form_tasks".
   */

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'chado_manager_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    // Get values from state.
    $schema_name = $form_state->getValue('chado_schema');
    $task = $form_state->getValue('task');

    $confirm = $form_state->getValue('confirm');
    if ($schema_name && $task) {
      switch ($task) {
        case static::RENAME_TASK:
          $form = $this->buildRenameForm($form, $form_state);
          break;

        case static::CLONE_TASK:
          $form = $this->buildCloneForm($form, $form_state);
          break;

        case static::DROP_TASK:
          $form_state->set(
            'confirm_message',
            $this->t(
              "Are you sure you want to remove schema '@schema_name'? This operation cannot be undone. We recommand you backup your data first.",
              [
                '@schema_name' => $schema_name,
              ]
            )
          );
          $form = $this->buildConfirmForm($form, $form_state);
          break;

        case static::INTEGRATE_TASK:
        case static::SET_DEFAULT_TASK:
        default:
          $form = $this->buildMainForm($form, $form_state);
      }
    }
    else {
      $form = $this->buildMainForm($form, $form_state);
    }
    return $form;
  }

  /**
   * Builds default form with Chado instance table.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function buildMainForm(array $form, FormStateInterface $form_state) {

    $tripal_dbx = \Drupal::service('tripal.dbx');
    $chado = new ChadoConnection();

    // Now that we support multiple chado instances, we need to list all the
    // currently installed ones here since they may be different versions.
    $rows = [];
    $instances = $chado->getAvailableInstances();

    $form['#attached']['library'][] = 'tripal_chado/tripal_chado.chado_table';
    $form['chado_schema'] = [
      '#type' => 'hidden',
      '#name' => 'chado_schema',
      '#default_value' => '',
    ];
    $form['task'] = [
      '#type' => 'hidden',
      '#name' => 'task',
      '#default_value' => '',
    ];

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

    $default_chado = '';
    foreach ($instances as $schema_name => $details) {
      // Schema name.
      $schema_name = $details['schema_name'];
      if ($details['is_default']) {
        $default_chado = $schema_name;
      }
      // Version.
      $version = $details['version'];
      // Integration.
      $integrated = $details['integration']
        ? $this->t('Yes')
        : (
          $details['is_test']
          ? $this->t('Test')
          : $this->t('No')
        )
      ;
      // Adds integration data.
      $created = '';
      $updated = '';
      if ($details['integration']) {
        // @todo: row style should highlight it is integrated.
        // @todo: add a "check" sign in front of default Chado schema.
        $created =
          \Drupal::service('date.formatter')->format(
            $details['integration']['created']
          )
        ;
        $updated =
          \Drupal::service('date.formatter')->format(
            $details['integration']['updated']
          )
        ;
      }

      // Set available operations.
      $operations = [];
      // Check if renamable, dropable.
      if (!$tripal_dbx->isSchemaReserved($schema_name)) {
        // Rename.
        $operations['rename_button'] = [
          '#type' => 'button',
          '#value' => $this->t('Rename'),
          '#attributes' => [
            'class' => ['chadoTableButton'],
            'data-chado-task' => static::RENAME_TASK,
            'data-chado-schema' => $schema_name,
          ],
        ];
        if ($details['integration']) {
          if (!$details['is_default']) {
            // Default instance.
            $operations['set_default_button'] = [
              '#type' => 'button',
              '#value' => $this->t('Set default'),
              '#attributes' => [
                'class' => ['chadoTableButton'],
                'data-chado-task' => static::SET_DEFAULT_TASK,
                'data-chado-schema' => $schema_name,
              ],
            ];
          }
        }
        else {
          // Integrate.
          $operations['integrate_button'] = [
            '#type' => 'button',
            '#value' => $this->t('Add to Tripal'),
            '#attributes' => [
              'class' => ['chadoTableButton'],
              'data-chado-task' => static::INTEGRATE_TASK,
              'data-chado-schema' => $schema_name,
            ],
          ];
        }
      }
      // Clone.
      $operations['clone_button'] = [
        '#type' => 'button',
        '#value' => $this->t('Clone'),
        '#attributes' => [
          'class' => ['chadoTableButton'],
          'data-chado-task' => static::CLONE_TASK,
          'data-chado-schema' => $schema_name,
        ],
      ];
      // Drop.
      $operations['drop_button'] = [
        '#type' => 'button',
        '#value' => $this->t('Drop'),
        '#attributes' => [
          'class' => ['chadoTableButton'],
          'data-chado-task' => static::DROP_TASK,
          'data-chado-schema' => $schema_name,
        ],
      ];

      $rows[$schema_name] = [
        $schema_name . ($default_chado == $schema_name ? $this->t(' (default)') : ''),
        $version,
        $details['has_data'] ? $this->t('Yes') : $this->t('No'),
        $integrated,
        $created,
        $updated,
        ['data' => [$operations]],
      ];
    }

    if (!empty($rows)) {
      $form['existing_instances'] = [
        '#type' => 'table',
        '#multiple' => FALSE,
        '#header' => ['Schema Name', 'Chado Version', 'Has data', 'In Tripal', 'Created', 'Updated', 'Operations'],
        '#rows' => $rows,
      ];
    }
    else {
      \Drupal::messenger()->addError(t('No Chado installations are found. @install.', [
          '@install' => Link::fromTextAndUrl('Please install a new Chado schema', Url::fromUri('internal:/admin/tripal/storage/chado/install'))->toString(),
      ]));
    }

    // Table buttons cannot submit the form without a submit button.
    // We add a submit button in order to fix that.
    $form['submit'] = [
      '#type' => 'submit',
      '#name' => 'action',
      '#value' => t('Refresh'),
    ];

    $form['#prefix'] = '<div id="tripal_chado_manage_form">';
    $form['#suffix'] = '</div>';

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // Get values from form first and then from state.
    $schema_name =
      $form_state->getValue('chado_schema')
      ?: $form_state->get('chado_schema')
    ;
    $form_state->set('chado_schema', $schema_name);

    $task =
      $form_state->getValue('task')
      ?: $form_state->get('task')
    ;
    $form_state->set('task', $task);

    $confirm = $form_state->getValue('confirm');

    switch ($task) {
      case static::SET_DEFAULT_TASK:
        $this->setDefaultSchema($schema_name);
        break;

      case static::INTEGRATE_TASK:
        $this->integrateSchema($schema_name);
        break;

      case static::DROP_TASK:
        if ($confirm) {
          $this->dropSchema($schema_name);
          $this->goBackForm($form, $form_state);
          break;
        }
      case static::RENAME_TASK:
      case static::CLONE_TASK:
      default:
        // A second form page should be provided.
        $form_state->setRebuild(TRUE);
    }
  }

  /**
   * Go back submission.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function goBackForm(array &$form, FormStateInterface $form_state) {
    $form_state
      ->set('task', '')
      ->set('schema_name', '')
      ->set('confirm_message', '')
      ->setValues([])
      ->setRebuild(TRUE);
  }

  /**
   * Validates forms that create a new schema.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function validateNewSchemaForm(array &$form, FormStateInterface $form_state) {
    $old_schema_name = $form_state->getValue('chado_schema');
    $new_schema_name = $form_state->getValue('new_schema_name');
    $tripal_dbx = \Drupal::service('tripal.dbx');

    // Check new schema name is valid.
    $issue = $tripal_dbx->isInvalidSchemaName($new_schema_name);
    if ($issue) {
      $form_state->setErrorByName(
        'new_schema_name',
        $issue
      );
    }

    // Make sure new schema does not exist.
    if ($tripal_dbx->schemaExists($new_schema_name)) {
      $form_state->setErrorByName(
        'new_schema_name',
        $this->t(
          'New schema name is already in use. Please choose a different name.'
        )
      );
    }

    parent::validateForm($form, $form_state);
  }

  /**
   * Sets default Chado schema.
   *
   * @param string $schema_name
   *   The schema name that will be used as default Chado schema. It should
   *   correspond to an existing Chado instance integrated in Tripal.
   */
  protected function setDefaultSchema(string $schema_name) {
    // Edit current config.
    $config = \Drupal::service('config.factory')
      ->getEditable('tripal_chado.settings')
    ;
    $config->set('default_schema', $schema_name)->save();

    \Drupal::messenger()->addStatus(
      $this->t(
        'Default Chado schema set to @schema_name.',
        ['@schema_name' => $schema_name, ]
      )
    );
  }

  /**
   * Live-runs a task on a Chado schema.
   *
   * @param string $task_service
   *   The name of the task service to use.
   * @param array $parameters
   *   Task parameters.
   */
  protected function runTask(string $task_service, array $parameters, $success_message) {
    $task = \Drupal::service($task_service);
    $task->setParameters($parameters);
    try {
      $success = $task->performTask();
    }
    catch (\Drupal\tripal\TripalDBX\Exceptions\TripalDbxException $e) {
      $success = FALSE;
    }
    if (!$success) {
      $error = "Failed to perform task."
        . ($e ? $e->getMessage() : " See previous log messages for details.")
      ;
      if ($e) {
        \Drupal::logger('tripal_chado')->error($e);
      }
      else {
        \Drupal::logger('tripal_chado')->error($error);
      }
      \Drupal::messenger()->addError($error);
    }
    else {
      \Drupal::messenger()->addStatus($success_message);
    }
  }

  /**
   * Integrates a Chado schema into Tripal.
   *
   * @param string $schema_name
   *   The schema name to integrate to Tripal.
   */
  protected function integrateSchema(string $schema_name) {
    $this->runTask(
      'tripal_chado.integrator',
      ['input_schemas' => [$schema_name], ],
      $this->t(
        'Chado schema @schema_name has been integrated into Tripal.',
        ['@schema_name' => $schema_name, ]
      )
    );
  }

  /**
   * Drops the given Chado schema.
   *
   * @param string $schema_name
   *   The schema name to drop.
   */
  protected function dropSchema(string $schema_name) {
    $this->runTask(
      'tripal_chado.remover',
      ['output_schemas' => [$schema_name], ],
      $this->t(
        'Chado schema @schema_name has been dropped.',
        ['@schema_name' => $schema_name, ]
      )
    );
  }

  /**
   * Builds a confirmation form.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function buildConfirmForm(array $form, FormStateInterface $form_state) {
    $schema_name = $form_state->get('chado_schema');
    $message = $form_state->get('confirm_message');
    if (empty($message)) {
      $message = $this->t('Are you sure you want to continue?');
    }

    $form['confirm_message'] = [
      '#type' => 'item',
      '#markup' => $message,
    ];

    $form['confirm'] = [
      '#type' => 'hidden',
      '#name' => 'confirm',
      '#value' => '1',
    ];

    $form['cancel'] = [
      '#type' => 'submit',
      '#name' => 'back',
      '#value' => t('Cancel'),
      '#submit' => ['::goBackForm'],
      '#limit_validation_errors' => [],
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#name' => 'action',
      '#value' => t('Confirm'),
      '#submit' => ['::submitForm'],
    ];

    return $form;
  }

  /**
   * Builds rename form.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function buildRenameForm(array $form, FormStateInterface $form_state) {
    $schema_name = $form_state->getValue('chado_schema');

    $form['task'] = [
      '#type' => 'item',
      '#markup' => t(
        '<h2>Rename "@schema_name" schema</h2>',
        ['@schema_name' => $schema_name, ]
      ),
    ];

    $form['chado_schema'] = [
      '#type' => 'hidden',
      '#name' => 'chado_schema',
      '#value' => $schema_name,
    ];

    $form['new_schema_name'] = [
      '#type' => 'textfield',
      '#title' => t('New Schema Name'),
      '#required' => TRUE,
      '#description' => t('Enter the new schema name to use.'),
      '#default_value' => '',
      '#attributes' => ['autocomplete' => 'off'],
    ];

    $form['cancel'] = [
      '#type' => 'submit',
      '#name' => 'back',
      '#value' => t('Cancel'),
      '#submit' => ['::goBackForm'],
      '#limit_validation_errors' => [],
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#name' => 'action',
      '#value' => t('Rename'),
      '#submit' => ['::submitRenameForm'],
      '#validate' => ['::validateNewSchemaForm'],
    ];

    return $form;
  }

  /**
   * Submit rename form.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitRenameForm(array &$form, FormStateInterface $form_state) {

    $old_schema_name = $form_state->getValue('chado_schema');
    $new_schema_name = $form_state->getValue('new_schema_name');

    $this->runTask(
      'tripal_chado.renamer',
      [
        'output_schemas' => [
          $old_schema_name,
          $new_schema_name,
        ],
      ],
      $this->t(
        'Chado schema @old_schema_name has been renamed into @new_schema_name.',
        [
          '@old_schema_name' => $old_schema_name,
          '@new_schema_name' => $new_schema_name,
        ]
      )
    );

    // Go back.
    $this->goBackForm($form, $form_state);
  }

  /**
   * Builds clone schema form.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function buildCloneForm(array $form, FormStateInterface $form_state) {
    $schema_name = $form_state->getValue('chado_schema');

    $form['task'] = [
      '#type' => 'item',
      '#markup' => t(
        '<h2>Clone "@schema_name" schema</h2>',
        ['@schema_name' => $schema_name, ]
      ),
    ];

    $form['chado_schema'] = [
      '#type' => 'hidden',
      '#name' => 'chado_schema',
      '#value' => $schema_name,
    ];

    $form['new_schema_name'] = [
      '#type' => 'textfield',
      '#title' => t('Clone Name'),
      '#required' => TRUE,
      '#description' => t('Enter the new schema name to use for the clone.'),
      '#default_value' => '',
      '#attributes' => ['autocomplete' => 'off'],
    ];

    $form['cancel'] = [
      '#type' => 'submit',
      '#name' => 'back',
      '#value' => t('Cancel'),
      '#submit' => ['::goBackForm'],
      '#limit_validation_errors' => [],
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#name' => 'action',
      '#value' => t('Clone'),
      '#submit' => ['::submitCloneForm'],
      '#validate' => ['::validateNewSchemaForm'],
    ];

    return $form;
  }

  /**
   * Submit clone form.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitCloneForm(array &$form, FormStateInterface $form_state) {

    $source_schema_name = $form_state->getValue('chado_schema');
    $new_schema_name = $form_state->getValue('new_schema_name');

    $current_user = \Drupal::currentUser();
    $args = [$source_schema_name, $new_schema_name];

    \Drupal::service('tripal.job')->create([
      'job_name' => t('Clone Chado schema'),
      'modulename' => 'tripal_chado',
      'callback' => 'tripal_chado_clone_schema',
      'arguments' => $args,
      'uid' => $current_user->id()
    ]);

    // Go back.
    $this->goBackForm($form, $form_state);
  }
}
