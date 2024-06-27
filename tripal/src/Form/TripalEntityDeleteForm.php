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
    return $this->t('This action will unpublish this page from the site and '
        . 'remove it from the underlying data storage system. To keep '
        . 'this item in the data store but remove it from the site, '
        . 'please choose the "unpublish" option instead.');
  }

}
