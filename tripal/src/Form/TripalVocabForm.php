<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Messenger\MessengerInterface;
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

    $form['vocabulary'] = [
      '#title' => $this->t('Short Name'),
      '#description' => 'The short name for the vocabulary (e.g. SO, PATO).',
      '#type' => 'textfield',
      '#default_value' => $entity->getLabel(),
    ];

    $form['name'] = [
      '#title' => $this->t('Full Name'),
      '#description' => 'The full name for the vocabulary (e.g. sequence).',
      '#type' => 'textfield',
      '#default_value' => $entity->getName(),
    ];

    $form['description'] = [
      '#title' => $this->t('Description'),
      '#description' => 'The definition for the current vocabulary.',
      '#type' => 'textarea',
      '#default_value' => $entity->getDescription(),
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
        $this->messenger()->addMessage($this->t('Created the %label Controlled Vocabulary.', [
          '%label' => $entity->getLabel(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Controlled Vocabulary.', [
          '%label' => $entity->getLabel(),
        ]));
    }

    $form_state->setRedirect('entity.tripal_vocab.collection');
  }

}
