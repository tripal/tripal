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


  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    // Call the parent class submit so that we can delete the Drupal entity.
    parent::submitForm($form, $form_state);

    // Now remove the record from Chado.

    /** @var \Drupal\tripal\TripalStorage\TripalStorageBase $storage **/
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');


  }
}
