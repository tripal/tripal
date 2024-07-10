<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\user\Entity\User;
use Drupal\Core\Messenger\MessengerTrait;

class TripalAdminRemoveQuota extends ConfirmFormBase{

  use MessengerTrait;

  /**
   * User ID.
   *
   * @var int
   */
  protected $uid;

  /**
   * Form ID.
   *
   * @return string
   */
  public function getFormId() {
    return 'tripal_admin_remove_quota';
  }

  /**
   * Get cancel URL.
   *
   * @return \Drupal\Core\Url
   */
  public function getCancelUrl() {
    return Url::fromRoute('tripal.files_quota');
  }

  /**
   * Build form.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @param null $uid
   *
   * @return array
   */
  public function buildForm(array $form, FormStateInterface $form_state, $uid = NULL) {
    $this->uid = $uid;

    return parent::buildForm($form, $form_state);
  }

  /**
   * Ask the user to confirm;
   *
   * @return \Drupal\Core\StringTranslation\TranslatableMarkup
   */
  public function getQuestion() {
    $user = User::load($this->uid);
    return t('Confirm deleting quota for @user', [
      '@user' => $user->getAccountName(),
    ]);
  }

  /**
   * Remove quota for a user.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return \Zend\Diactoros\Response\RedirectResponse
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    tripal_remove_user_quota($this->uid);
    $user = User::load($this->uid);
    $this->messenger()
      ->addStatus('The custom quota for user, "' . $user->getAccountName() . '", has been removed.');
    $form_state->setRedirect('tripal.files_quota');
  }

}
