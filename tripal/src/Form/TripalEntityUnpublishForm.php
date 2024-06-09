<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;

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
    return $this->t('Are you sure you want to unpublish %name? The data '
      . 'will be unpublished but retained in the database.',
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
  protected function getDeletionMessage() {
    $entity = $this->getEntity();
    return $this->t('The @entity-type %label has been unpublished.', [
      '@entity-type' => $entity->getEntityType()->getSingularLabel(),
      '%label' => $entity->label() ?? $entity->id(),
    ]);
  }

}
