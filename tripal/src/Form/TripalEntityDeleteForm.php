<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\Core\Form\FormStateInterface;


/**
 * Provides a form for deleting Tripal Content entities.
 *
 * @ingroup tripal
 */
class TripalEntityDeleteForm extends ContentEntityDeleteForm {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to unpublish and permanenty '
      . 'delete %name?', ['%name' => $this->entity->label()]);
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
  public function getDescription() {
    return $this->t('This action is not fully implemented in Tripal v4. It currently functions as an"unpublish".');
  }

}
