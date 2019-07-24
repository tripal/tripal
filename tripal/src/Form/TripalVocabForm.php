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

    // ID field: handled internally.

    // Vocabulary Field.
    $form['vocabulary'] = [
      '#title' => $this->t('Controlled Vocabulary Name'),
      '#description' => 'The short name for the vocabulary (e.g. SO, PATO).',
      '#type' => 'textfield',
      '#default_value' => $entity->getLabel(),
    ];



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
          '%label' => $entity->getLabel(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Controlled Vocabulary.', [
          '%label' => $entity->getLabel(),
        ]));
    }

    $form_state->setRedirect('entity.tripal_vocab.canonical', ['tripal_vocab' => $entity->id()]);
  }

}
