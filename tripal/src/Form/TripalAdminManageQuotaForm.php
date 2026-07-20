<?php

namespace Drupal\tripal\Form;

use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Provides the form to manage file quotas for users.
 */
class TripalAdminManageQuotaForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('entity_type.manager'),
    );
  }

  /**
   * Class constructor.
   */
  public function __construct(
    protected readonly EntityTypeManagerInterface $entityTypeManager,
  ) {
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tripal_admin_manage_quota_form';
  }

  /**
   * Allow users to specify a max file size.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return array
   *   Render array containing elements for the form.
   */
  public function buildForm(array $form, FormStateInterface $form_state): array {
    $settings = $this->config('tripal.settings');
    $username = $form_state->getValue('username', '');
    $default_quota = $form_state->getValue('default_quota',
      $settings->get('files.quota'));
    $default_expiration = $form_state->getValue('default_expiration_date',
      $settings->get('files.expiration'));

    // Textfield (ajax call based off of existing users) for users on the site.
    $form['username'] = [
      '#type' => 'textfield',
      '#title' => 'User',
      '#autocomplete_path' => 'admin/tripal/files/quota/user/autocomplete',
      '#autocomplete_route_name' => 'tripal.files_quota_user_autocomplete',
      '#default_value' => $username,
    ];

    // Custom quota textfield (prepopulated with default value)
    $form['quota'] = [
      '#type' => 'textfield',
      '#title' => 'Custom User Quota',
      '#description' => $this->t('Set the number of megabytes that a user can consume. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).'),
      '#default_value' => tripal_format_bytes($default_quota),
    ];

    // Custom exp date textfield (prepopulated with default value)
    $form['expiration'] = [
      '#type' => 'textfield',
      '#title' => 'Days to Expire',
      '#description' => $this->t('The number of days that a user uploaded file can remain on the server before it is automatically removed.'),
      '#default_value' => $default_expiration,
    ];

    // Submit button.
    $form['button'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];

    $form['cancel'] = [
      '#markup' => Link::fromTextAndUrl('Cancel',
        Url::fromRoute('tripal.files_quota'))->toString(),
    ];

    return $form;
  }

  /**
   * Validate form.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $username = $form_state->getValue('username');
    $quota = $form_state->getValue('quota');
    $expiration = $form_state->getValue('expiration');

    // Make sure the username is a valid user.
    $users = $this->entityTypeManager->getStorage('user')->loadByProperties(['name' => $username]);
    $user = reset($users);
    if (!$user) {
      $form_state->setErrorByName('username', 'Cannot find this username');
      return;
    }

    // Validate the quota string.
    if (!preg_match("/^\d+(MB|GB|TB)$/", $quota)) {
      $form_state->setErrorByName('quota',
        $this->t('Please provide a quota size in the format indicated.'));
    }

    // Validate the expiration time.
    if (!preg_match("/^\d+$/", $expiration)) {
      $form_state->setErrorByName('expiration',
        $this->t('Please providate a positive non-decimal numeric value for the days to expire'));
    }
  }

  /**
   * Save settings.
   *
   * @param array $form
   *   The form array definition.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return \Symfony\Component\HttpFoundation\RedirectResponse|void
   *   A redirection to the parent quota form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $username = $form_state->getValue('username');
    $quota = $form_state->getValue('quota');
    $expiration = $form_state->getValue('expiration');

    // If the 2nd element of the quota string is occupied by a valid suffix
    // we need to check to see what we have to multiply the value by (1024
    // for GB 1024^2 for TB because we assume that the initial number is
    // already in MB).
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
    $users = $this->entityTypeManager->getStorage('user')->loadByProperties(['name' => $username]);
    $user = reset($users);
    try {
      // Set user quota.
      tripal_set_user_quota($user->id(), $size, $expiration);
    }
    catch (\Exception $exception) {
      $this->messenger()->addError($exception->getMessage());
      return;
    }

    $this->messenger()->addStatus($this->t('Custom quota set for the user: @username',
      ['@username' => $username]));

    $form_state->setRedirect('tripal.files_quota');
  }

}
