<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal;
use Drupal\Core\Link;
use Drupal\Core\Messenger\MessengerTrait;

class TripalFileQuota implements FormInterface{

  use MessengerTrait;

  /**
   * Form ID.
   *
   * @return string
   */
  public function getFormId() {
    return 'tripal_admin_manage_quota_form';
  }

  /**
   * Build a form to allow the user to set the default quota and control user
   * specific quota.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return array
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $settings = Drupal::config('tripal.settings');
    // Provide overall server consumption (and space remaining)
    $default_quota = $form_state->getValue('default_quota',
      $settings->get('files.quota'));
    $default_expiration = $form_state->getValue('default_expiration_date',
      $settings->get('files.expiration'));

    // Optimized query to compute total size of storage used
    // by tripal users
    $sql = "SELECT SUM(filesize) FROM {file_managed}
                WHERE fid IN (
                    SELECT DISTINCT fid FROM {file_usage}
                        WHERE module = 'tripal'
                )";
    $total_size = (float) Drupal::database()->query($sql)->fetchField();

    $form['total_size'] = [
      '#type' => 'item',
      '#title' => t('Total Current Usage'),
      '#description' => t('The total amount of space consumed by user file uploads.'),
      '#markup' => tripal_format_bytes($total_size),
    ];

    // TODO: add a D3 chart showing the amount of storage used by each user.
    $form['default_quota'] = [
      '#type' => 'textfield',
      '#title' => 'Default System-Wide User Quota',
      '#description' => t('Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).'),
      '#default_value' => tripal_format_bytes($default_quota),
    ];

    $form['default_expiration_date'] = [
      '#type' => 'textfield',
      '#title' => 'Default System-Wide Expiration Date',
      '#description' => t('The number of days that a user uploaded file can remain on the server before it is automatically removed'),
      '#default_value' => $default_expiration,
    ];

    // Populate the table from the custom quota db table (users, quota, exp date).
    $header = [
      'uid' => t('UID'),
      'user' => t('Users'),
      'custom_quota' => t('Custom Quota'),
      'exp_date' => t('Expiration Date'),
      'actions' => t('Actions'),
    ];

    // API call to the gather the users that have a custom quota
    $rows = [];
    $query = "SELECT * FROM {tripal_custom_quota}";
    $data = Drupal::database()->query($query);
    while ($entry = $data->fetchObject()) {
      $user = Drupal\user\Entity\User::load($entry->uid);

      $uid = $user->id();

      // Use render arrays for action links.
      // This was done since concatenating the links showed the markup.
      $actions_renderable = [];
      $actions_renderable['edit'] = Link::fromTextAndUrl('Edit',
        Drupal\Core\Url::fromRoute('tripal.files_quota_user_edit',
          ['uid' => $uid]))->toRenderable();
      $actions_renderable['divider'] = [
        '#markup' => ' | ',
      ];
      $actions_renderable['remove'] = Link::fromTextAndUrl('Remove',
        Drupal\Core\Url::fromRoute('tripal.files_quota_remove',
          ['uid' => $uid]))->toRenderable();

      $rows[] = [
        'uid' => $uid,
        'user' => $user->getAccountName(),
        'custom_quota' => tripal_format_bytes($entry->custom_quota),
        'exp_date' => $entry->custom_expiration,
        'actions' => Drupal::service('renderer')->render($actions_renderable),
      ];
    }

    $form['custom'] = [
      '#type' => 'fieldset',
      '#title' => 'Custom Settings',
      '#description' => t('The settings above apply to all users.  The following allows for custom user settings that override the defaults set above.'),
      '#collapsed' => TRUE,
      '#collapsible' => FALSE,
    ];

    $form['custom']['links'] = [
      '#markup' => '<br>' . Link::fromTextAndUrl('Add Custom User Quota',
          Drupal\Core\Url::fromUri('internal:/admin/tripal/files/quota/add'))
          ->toString(),
    ];


    $form['custom']['custom_quotas'] = [
      '#title' => t('Custom User Quotas'),
      '#type' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#attributes' => [],
      '#caption' => '',
      '#sticky' => TRUE,
      '#empty' => 'There are no custom user quotas.',
      '#colgroups' => [],
    ];

    $form['update_defaults'] = [
      '#type' => 'submit',
      '#value' => t('Save'),
    ];

    return $form;
  }

  /**
   * Validate the form's values: proper numbers and/or MB, GB, TB for quota
   * field.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $quota = $form_state->getValue('default_quota');
    $expiration = $form_state->getValue('default_expiration_date');

    // Validate the quota string.
    if (!preg_match("/^\d+(\.\d+)*(MB|GB|TB)$/", $quota)) {
      $form_state->setErrorByName('default_quota',
        t('Please provide a quota size in the format indicated.'));
    }

    // Validate the expiration time.
    if (!preg_match("/^\d+$/", $expiration)) {
      $form_state->setErrorByName('default_expiration',
        t('Please providate a positive non-decimal numeric value for the days to expire'));
    }
  }

  /**
   * Write to the two drupal state the site wide default quota and exp date.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $quota = $form_state->getValue('default_quota');
    $expiration = $form_state->getValue('default_expiration_date');

    // if the 2nd element of the quota string occupied by a valid suffix we need to check to see
    // what we have to multiply the value by (1024 for GB 1024^2 for TB because
    // we assume that the initial number is already in MB)
    $matches = [];
    $multiplier = 'MB';
    $size = $quota;
    if (preg_match("/^(\d+(?:\.\d+)*)(MB|GB|TB)$/", $quota, $matches)) {
      $multiplier = $matches[2];
      $size = $matches[1];
    }
    switch ($multiplier) {
      case 'GB':
        $size = (int) ($size * pow(10, 9));
        break;
      case 'TB':
        $size = (int) ($size * pow(10, 12));
        break;
      default:
        $size = (int) ($size * pow(10, 6));
        break;
    }

    // Update configuration
    Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('files.quota', $size)
      ->set('files.expiration', $expiration)
      ->save();

    $this->messenger()->addStatus('Default quota settings have been set.');
  }

}
