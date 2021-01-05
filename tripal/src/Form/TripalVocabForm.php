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

    $form['name'] = [
      '#title' => $this->t('Full Name'),
      '#description' => 'The full human-readable name for the ontology (e.g. The Sequence Ontology).',
      '#type' => 'textfield',
      '#default_value' => $entity->getName(),
    ];

    $form['namespace'] = [
      '#title' => $this->t('Namespace'),
      '#description' => 'The default namespace for the ontology (e.g. sequence).',
      '#type' => 'textfield',
      '#default_value' => $entity->getNamespace(),
    ];

    $form['url'] = [
      '#title' => $this->t('Reference URL'),
      '#description' => 'The URL referencing the original source for the ontology. This is used to provide attribution.',
      '#type' => 'textfield',
      '#default_value' => $entity->getURL(),
    ];

    $form['description'] = [
      '#title' => $this->t('Description'),
      '#description' => 'A description for the current ontology.',
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
        $this->messenger()->addMessage($this->t('Created the %label Tripal Vocabulary.', [
          '%label' => $entity->getLabel(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Tripal Vocabulary.', [
          '%label' => $entity->getLabel(),
        ]));
    }

    $form_state->setRedirect('entity.tripal_vocab.collection');
  }

}
