<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a form for deleting Tripal Content entities.
 *
 * @ingroup tripal
 */
class TripalAdminAddCustomForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'tripal_admin.settings',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_admin_add_custom_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('tripal_admin.settings');
    $username = '';
    $default_quota =  $config->get('tripal_default_file_quota') ?: pow(20, 6);
    $default_expiration = $config->get('tripal_default_file_expiration') ?: '60';

    if (array_key_exists('values', $form_state)) {
      $username = $form_state['values']['username'];
      $default_quota = $form_state['values']['default_quota'];
      $default_expiration = $form_state['values']['default_expiration_date'];
    }


    // Textfield (ajax call based off of existing users) for users on the site
    $form['username'] = [
      '#type' => 'textfield',
      '#title' => $this->t('User'),
      '#autocomplete_path' => 'admin/tripal/files/quota/user/autocomplete',
      '#default_value' => $username,
    ];

    // Custom quota textfield (prepopulated with defualt value)
    $form['quota'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Custom User Quota'),
      '#description' => $this->t('Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).'),
      '#default_value' => tripal_format_bytes($default_quota),
    ];

    // Custom exp date textfield (prepopulated with defualt value)
    $form['expiration'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Days to Expire'),
      '#description' => $this->t('The number of days that a user uploaded file can remain on the server before it is automatically removed.'),
      '#default_value' => $default_expiration,
    ];

    // Submit button
    $form['button'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];
    $form['cancel'] = [
      '#markup' => l('Cancel', 'admin/tripal/files/quota'),
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $username = $form_state->getValue('username');
    $quota = $form_state->getValue('quota');
    $expiration = $form_state->getValue('expiration');

    // Make sure the username is a valid user.
    $sql = "SELECT uid FROM {users} WHERE name = :name";
    $uid = Drupal::database()->query($sql, [':name' => $username])->fetchField();
    if (!$uid) {
      $form_state->setErrorByName('username', $this->t('Cannot find this username'));
    }

    // Does a quota already exist for this user? If so, then don't add it again
    $check = Drupal::database()->select('tripal_custom_quota', 'tgcq')
      ->fields('tgcq', ['uid'])
      ->condition('uid', $uid)
      ->execute()
      ->fetchField();
    if ($check) {
      $form_state->setErrorByName('username', $this->t('The user "' . $username . '" already has a custom quota set.'));
    }

    // Validate the quota string.
    if (!preg_match("/^\d+(MB|GB|TB)$/", $quota)) {
      $form_state->setErrorByName('quota', $this->t('Please provide a quota size in the format indicated.'));
    }

    // Validate the expiration time.
    if (!preg_match("/^\d+$/", $expiration)) {
      $form_state->setErrorByName('expiration', $this->t('Please providate a positive non-decimal numeric value for the days to expire'));
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $username = $form_state->getValue('username');
    $quota = $form_state->getValue('quota');
    $expiration = $form_state->getValue('expiration');

    // if the 2nd element of the quota string occupied by a valid suffix we need to check to see
    // what we have to multiply the value by (1024 for GB 1024^2 for TB because
    // we assume that the initial number is already in MB)
    $matches = [];
    $multiplier = 'MB';
    $size = $quota;
    if (preg_match("/^(\d+)(MB|GB|TB)$/", $quota, $matches)) {
      $multiplier = $matches[2];
      $size = $matches[1];
    }

    switch ($multiplier) {
      case 'GB':
        $size = (int) $quota * pow(10, 9);
        break;
      case 'TB':
        $size = (int) $quota * pow(10, 12);
        break;
      default:
        $size = (int) $quota * pow(10, 6);
        break;
    }

    // Get the UID of the given user.
    $sql = "SELECT uid FROM {users} WHERE name = :name";
    $uid = Drupal::database()->query($sql, [':name' => $username])->fetchField();

    // Stripaluota.
    tripal_set_user_quota($uid, $size, $expiration);

    // TODO: check to make sure that the quota was actually set, can we assume
    // it will always work?

    $this->messenger()->addStatus($this->t('Custom quota set for the user: %username', ['%username' => $username]));
    $form_state->setRedirectUrl('admin/tripal/files/quota');
  }


}
