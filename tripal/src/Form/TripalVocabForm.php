<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form controller for Controlled Vocabulary edit forms.
 *
 * @ingroup tripal
 */
class TripalVocabForm extends ContentEntityForm {

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var $entity \Drupal\tripal\Entity\TripalVocab */
    $form = parent::buildForm($form, $form_state);
    $entity = $this->entity;

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $entity = $this->entity;
    $status = parent::save($form, $form_state);

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Controlled Vocabulary.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Controlled Vocabulary.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.tripal_vocab.canonical', ['tripal_vocab' => $entity->id()]);
  }

}
