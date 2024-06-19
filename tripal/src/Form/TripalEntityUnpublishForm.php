<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a form for unpublishing Tripal Content entities.
 *
 * This is equilvalent to deleting an Drupal entity, but in the base of
 * Tripal entities, it does not try to delete the item in the database
 * back-end.
 *
 * @ingroup tripal
 */
class TripalEntityUnpublishForm extends ContentEntityDeleteForm {

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
  public function getConfirmText() {
    return $this->t('Unpublish');
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->t('Unpublishing will remove this page from the site but the data '
      . 'will be retained in the database and you can republish it again later if desired.');
  }

  /**
   * Similar to the parent::getDeleteMessage() but custom for unpublishing.
   */
  protected function getUnpublishMessage() {
    $entity = $this->getEntity();
    return $this->t('The @entity-type %label has been unpublished.', [
      '@entity-type' => $entity->getEntityType()->getSingularLabel(),
      '%label' => $entity->label() ?? $entity->id(),
    ]);
  }

  /**
   * Similar to the parent::logUnpublishMessage() but custom for unpublishing.
   */
  protected function logUnpublishMessage() {
    /** @var \Drupal\Core\Entity\ContentEntityInterface $entity */
    $entity = $this->getEntity();
    $this->logger($entity->getEntityType()->getProvider())->info('The @entity-type %label has been unpublished.', [
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
    $message = $this->getUnpublishMessage();

    $entity->unpublish();
    $form_state->setRedirectUrl($this->getRedirectUrl());

    $this->messenger()->addStatus($message);
    $this->logUnpublishMessage();
  }



}
