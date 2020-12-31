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

    $form['name'] = [
      '#type' => 'textfield',
      '#title' => 'Term Name',
      '#description' => 'The human readable name for this term.',
      '#default_value' => $entity->getName(),
    ];

    $default_idspace = $entity->getIDSpace();
    $form['idspace_id'] = [
      '#type' => 'entity_autocomplete',
      '#title' => 'IDSpace',
      '#description' => 'The IDSpace (e.g. SO) of the default vocabulary this term belongs to.',
      '#target_type' => 'tripal_vocab_space',
      '#default_value' => $default_idspace,
    ];

    $form['vocab_id'] = [
      '#type' => 'entity_autocomplete',
      '#title' => 'Vocabulary',
      '#description' => 'The default vocabulary this IDSpace belongs to.',
      '#target_type' => 'tripal_vocab',
      '#tags' => TRUE,
      '#default_value' => $entity->getVocab(),
      //'#disabled' => TRUE,
    ];

    $form['accession'] = [
      '#type' => 'textfield',
      '#title' => 'Accession',
      '#description' => 'The unique ID (or accession) of this term not including the IDSpace.',
      '#default_value' => $entity->getAccession(),
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
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);
    // @todo make sure the default vocabulary from the IDSpace
    // is in the vocabulary values.
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $entity = $this->entity;

    // Now save the entity base on the form state.
    $status = parent::save($form, $form_state);

    // Finally tell the user how it went.
    switch ($status) {
      case SAVED_NEW:
        $this->messenger()->addMessage($this->t('Created the %label Tripal Term.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Tripal Term.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.tripal_term.collection');
  }

}
