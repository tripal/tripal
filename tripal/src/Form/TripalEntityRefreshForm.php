<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\EntityConfirmFormBase;
use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a form for refreshing Tripal Content entities.
 *
 * This is equilvalent to clearing the Drupal cache for this entity, and
 * then syncying/publishing unpublished field values.
 *
 * @ingroup tripal
 */
class TripalEntityRefreshForm extends EntityConfirmFormBase {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to unpublish %name?',
      ['%name' => $this->entity->label()]);
  }
  
  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
      return $this->getEntity()->toUrl();
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Refresh');
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->t('A refresh will clear the Drupal cache and look for missing field '.
        'values that may be present in the database but not yet publisehd to this page.');
  }

  /**
   * Similar to the parent::getDeleteMessage() but custom for unpublishing.
   */
  protected function getRefreshMessage() {
    $entity = $this->getEntity();
    return $this->t('The @entity-type %label has been refreshed.', [
      '@entity-type' => $entity->getEntityType()->getSingularLabel(),
      '%label' => $entity->label() ?? $entity->id(),
    ]);
  }

  /**
   * Similar to the parent::logUnpublishMessage() but custom for unpublishing.
   */
  protected function logRefreshMessage() {
    /** @var \Drupal\Core\Entity\ContentEntityInterface $entity */
    $entity = $this->getEntity();
    $this->logger($entity->getEntityType()->getProvider())->info('The @entity-type %label has been refreshed.', [
      '@entity-type' => $entity->getEntityType()->getSingularLabel(),
      '%label' => $entity->label(),
    ]);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    /** @var \Drupal\tripal\Entity\TripalEntity $entity */
    $entity = $this->getEntity();
    $message = $this->getRefreshMessage();

    $entity->refresh();
    $form_state->setRedirectUrl($this->getRedirectUrl());

    $this->messenger()->addStatus($message);
    $this->logRefreshMessage();
  }



}
