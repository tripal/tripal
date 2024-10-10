<?php
namespace Drupal\tripal_layout\Form;

use Drupal\Core\Entity\EntityConfirmFormBase;
use Drupal\Core\Url;
use Drupal\Core\Form\FormStateInterface;

/**
 * Builds the form to delete an TripalLayoutDefaultForm Config Entity.
 */
class TripalLayoutDefaultFormDeleteForm extends EntityConfirmFormBase {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete the Form Layout %name?', ['%name' => $this->entity->label()]);
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    $content = '<p>'
    . $this->t('You are deleting an entire configuration that specifies how '
    . 'tripal content edit forms should be organized by default.')
      . '</p>';
    $content .= '<p>' . $this->t('Rebuild the cache to re-import the '
    . 'default settings of this configuration.') . '</p>';
    return $content;
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
    return new Url('entity.tripal_layout_default_form.layouts');
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->entity->delete();
    $this->messenger()->addMessage($this->t('The %label Tripal Default Form Layout has been deleted.', ['%label' => $this->entity->label()]));

    $form_state->setRedirectUrl($this->getCancelUrl());
  }

}
