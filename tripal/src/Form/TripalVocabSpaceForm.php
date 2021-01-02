<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Form controller for Tripal Vocabulary IDSpace edit forms.
 *
 * @ingroup tripal
 */
class TripalVocabSpaceForm extends ContentEntityForm {

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    // Instantiates this form class.
    $instance = parent::create($container);
    return $instance;
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var \Drupal\tripal\Entity\TripalVocabSpace $entity */
    $form = parent::buildForm($form, $form_state);
    $entity = $this->entity;

    $form['IDSpace'] = [
      '#title' => $this->t('IDSpace'),
      '#description' => 'The IDSpace of the vocabulary (e.g. SO).',
      '#type' => 'textfield',
      '#default_value' => $entity->getIDSpace(),
    ];

    $form['vocab_id'] = [
      '#type' => 'entity_autocomplete',
      '#title' => 'Tripal Vocabulary',
      '#description' => 'The name (e.g. sequence) of the vocabulary this IDSpace belongs to.',
      '#target_type' => 'tripal_vocab',
      '#default_value' => $entity->getVocab(),
    ];

    $form['URLprefix'] = [
      '#title' => $this->t('URL Prefix'),
      '#description' => 'A URL to access the term of this IDSpace. It can include the {{IDSpace}} and {{accession}} tokens.',
      '#type' => 'textfield',
      '#default_value' => $entity->getURLPrefix(),
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
        $this->messenger()->addMessage($this->t('Created the %label Tripal Vocabulary IDSpace.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Tripal Vocabulary IDSpace.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.tripal_vocab_space.canonical', ['tripal_vocab_space' => $entity->id()]);
  }

}
