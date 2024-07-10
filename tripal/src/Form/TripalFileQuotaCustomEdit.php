<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal;
use Drupal\Core\Link;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\user\Entity\User;

class TripalFileQuotaCustomEdit implements FormInterface{

  use MessengerTrait;

  /**
   * Form ID.
   *
   * @return string
   */
  public function getFormId() {
    return 'tripal_file_custom_quota_edit_form';
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
  public function buildForm(array $form, FormStateInterface $form_state, $uid = NULL) {
    $quota = tripal_get_user_quota($uid);
    $default_quota = $form_state->getValue('default_quota',
      $quota->custom_quota);
    $default_expiration = $form_state->getValue('default_expiration_date',
      $quota->custom_expiration);

    $user = User::load($uid);
    $form['uid'] = [
      '#type' => 'value',
      '#value' => $uid,
    ];
    // Textfield (ajax call based off of existing users) for users on the site
    $form['username'] = [
      '#type' => 'item',
      '#title' => 'User',
      '#markup' => $user->getAccountName(),
    ];

    // Custom quota textfield (prepopulated with defualt value)
    $form['quota'] = [
      '#type' => 'textfield',
      '#title' => 'Custom User Quota',
      '#description' => 'Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).',
      '#default_value' => tripal_format_bytes($default_quota),
    ];

    // Custom exp date textfield (prepopulated with defualt value)
    $form['expiration'] = [
      '#type' => 'textfield',
      '#title' => 'Days to Expire',
      '#description' => 'The number of days that a user uploaded file can remain on the server before it is automatically removed.',
      '#default_value' => $default_expiration,
    ];

    // Submit button
    $form['button'] = [
      '#type' => 'submit',
      '#value' => t('Submit'),
    ];

    $link = Link::fromTextAndUrl('Cancel',
      Drupal\Core\Url::fromRoute('tripal.files_quota'));
    $form['cancel'] = [
      '#markup' => $link->toString(),
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
    $uid = $form_state->getValue('uid');
    $quota = $form_state->getValue('quota');
    $expiration = $form_state->getValue('expiration');

    // Validate the quota string.
    if (!preg_match("/^\d+(MB|GB|TB)$/", $quota)) {
      $form_state->setErrorByName('quota',
        t('Please provide a quota size in the format indicated.'));
    }
    // Validate the expiration time.
    if (!preg_match("/^\d+$/", $expiration)) {
      $form_state->setErrorByName('expiration',
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
    $uid = $form_state->getValue('uid');
    $quota = $form_state->getValue('quota');
    $expiration = $form_state->getValue('expiration');

    // if the 2nd element of the quota string occupied by a valid suffix we need to check to see
    // what we have to multiply the value by (1024 for GB 1024^2 for TB because
    // we assume that the initial number is already in MB)
    $matches = [];
    $multiplier = 'MB';
    if (preg_match("/^\d+(\.\d+)*(MB|GB|TB)$/", $quota, $matches)) {
      $multiplier = $matches[2];
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

    // Set the user quota.
    tripal_remove_user_quota($uid);
    tripal_set_user_quota($uid, $size, $expiration);
    $user = User::load($uid);
    $this->messenger()->addStatus(t('Custom quota set for the user: @username',
      ['@username' => $user->getAccountName()]));
    $form_state->setRedirect('tripal.files_quota');
  }

}
