<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Messenger\MessengerInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form controller for Controlled Vocabulary Term edit forms.
 *
 * @ingroup tripal
 */
class TripalTermForm extends ContentEntityForm {

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var $entity \Drupal\tripal\Entity\TripalTerm */
    $form = parent::buildForm($form, $form_state);
    $entity = $this->entity;

    $form['vocab_id'] = [
      '#type' => 'entity_autocomplete',
      '#title' => 'Tripal Controlled Vocabulary',
      '#description' => 'The short name (e.g. SO, PATO) of the vocabulary this term belongs to.',
      '#target_type' => 'tripal_vocab',
      '#default_value' => $entity->getVocab(),
    ];

    $form['accession'] = [
      '#type' => 'textfield',
      '#title' => 'Accession',
      '#description' => 'The unique ID (or accession) of this term in the vocabulary.',
      '#default_value' => $entity->getAccession(),
    ];

    $form['name'] = [
      '#type' => 'textfield',
      '#title' => 'Term Name',
      '#description' => 'The human readable name for this term.',
      '#default_value' => $entity->getName(),
    ];

    $form['definition'] = [
      '#type' => 'textarea',
      '#title' => 'Definition',
      '#description' => 'The definition of this term.',
      '#default_value' => $entity->getDefinition(),
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
        $this->messenger()->addMessage($this->t('Created the %label Controlled Vocabulary Term.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Controlled Vocabulary Term.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.tripal_term.collection');
  }

}
