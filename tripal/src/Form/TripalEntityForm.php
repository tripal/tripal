<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\Component\Serialization\Json;
use Drupal\tripal\Entity\TripalEntity;

/**
 * Form controller for Tripal Content edit forms.
 *
 * @ingroup tripal
 */
class TripalEntityForm extends ContentEntityForm {
  use MessengerTrait;

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var $entity \Drupal\tripal\Entity\TripalEntity */
    $form = parent::buildForm($form, $form_state);
    $entity = $this->entity;

    // We want to theme our form just like the node form.
    $form['#theme'] = 'node_edit_form';
    $form['#attached']['library'] = ['node/drupal.node'];
    // But also add room for our own styles.
    $form['#attached']['library'][] = 'tripal/tripal-entity-form';

    // Drupal only adds this to content forms if they support revisions but we want it in general.
    if (!isset($form['advanced'])) {
      $form['advanced'] = [
        '#type' => 'vertical_tabs',
        '#weight' => 99,
      ];
    }
    // -- Additional collapsed regions can be added to this group by creating
    //    a field group of type "Details Siderbar" and adding fields to it.
    // -- Add a metadata section to provide basic context info.
    $form['advanced']['#attributes']['class'][] = 'entity-meta';
    $form['meta'] = $this->buildMetadataFormElement($entity);
    // -- If the theme being used is claro we want to add it's specific brand
    // of styling. This is what it does in claro_form_node_form_alter
    $theme = \Drupal::service('theme.manager')->getActiveTheme()->getName();
    if ($theme == 'claro') {
      $form['#attached']['library'][] = 'claro/form-two-columns';
      $form['advanced']['#type'] = 'container';
      $form['advanced']['#accordion'] = TRUE;
      $form['meta']['#type'] = 'container';
    }
    // -- Add a "Authoring information" panel.
    // @todo make this match the one for nodes.
    $form['author'] = $this->buildAuthorInfoFormElement($entity, $form);

    // We also want to disable the title to ensure it uses the token system.
    $form['title']['#weight'] = 5;
    $form['title']['#disabled'] = TRUE;
    $form['title']['widget'][0]['value']['#description'] .= ' The title will be automatically updated based on the title format defined by administrators.';
    $form['meta']['title'] = $form['title'];
    unset($form['title']);

    return $form;
  }

  /**
   * Adds a meta form element to the main form.
   *
   * @param TripalEntity $entity
   *   The entity this form is for.
   * @return array
   *   The form elements to add to the existing entity add/edit form.
   */
  protected function buildMetadataFormElement(TripalEntity $entity) {
    $element = [
      '#type' => 'details',
      '#group' => 'advanced',
      '#weight' => -10,
      '#title' => $this->t('Status'),
      '#attributes' => [
        'class' => [
          'entity-meta__header',
        ],
      ],
      '#tree' => TRUE,
    ];
    $element['published'] = [
      '#type' => 'item',
      '#markup' => $entity->isPublished() ? $this->t('Published') : $this->t('Not published'),
      '#access' => !$entity->isNew(),
      '#wrapper_attributes' => [
        'class' => [
          'entity-meta__title',
        ],
      ],
    ];
    $element['changed'] = [
      '#type' => 'item',
      '#title' => $this->t('Last saved'),
      '#markup' => !$entity->isNew() ? $this->dateFormatter
        ->format($entity->getChangedTime(), 'short') : $this->t('Not saved yet'),
      '#wrapper_attributes' => [
        'class' => [
          'entity-meta__last-saved',
        ],
      ],
    ];
    $element['author'] = [
      '#type' => 'item',
      '#title' => $this->t('Author'),
      '#markup' => $entity->getOwner()
        ->getAccountName(),
      '#wrapper_attributes' => [
        'class' => [
          'entity-meta__author',
        ],
      ],
    ];

    return $element;
  }

  /**
   * Adds a meta form element to the main form.
   *
   * @param TripalEntity $entity
   *   The entity this form is for.
   * @return array
   *   The form elements to add to the existing entity add/edit form.
   */
  protected function buildAuthorInfoFormElement(TripalEntity $entity, array &$form) {
    $element = [
      '#type' => 'details',
      '#group' => 'advanced',
      '#weight' => -8,
      '#title' => $this->t('Authoring Information'),
    ];

    $element['uid'] = $form['uid'];
    unset($form['uid']);

    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $entity = $this->entity;
    $bundle = $entity->getType();
    $bundle_entity = \Drupal\tripal\Entity\TripalEntityType::load($bundle);

    $entity->setTitle($values['title'][0]['value']);
    $entity->setOwnerId($values['uid'][0]['target_id']);
    $status = parent::save($form, $form_state);
    $entity->setAlias();

    switch ($status) {
      case SAVED_NEW:
        $this->messenger()->addMessage($this->t('Created the %label.', [
          '%label' => $bundle_entity->label(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label.', [
          '%label' => $bundle_entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.tripal_entity.canonical', ['tripal_entity' => $entity->id()]);
  }

  /**
   *
   * {@inheritDoc}
   * @see \Drupal\Core\Entity\EntityForm::actions()
   */
  protected function actions(array $form, FormStateInterface $form_state) {
    $actions = parent::actions($form, $form_state);
    if (!$this->entity->isNew() && $this->entity->hasLinkTemplate('unpublish-form')) {
      $route_info = $this->entity->toUrl('canonical');
      $actions['cancel'] = [
        '#type' => 'link',
        '#title' => $this->t('Cancel'),
        '#access' => $this->entity->access('administer tripal content'),
        '#attributes' => [
          'class' => ['button'],
        ],
        '#url' => $route_info,
      ];
      $route_info = $this->entity->toUrl('unpublish-form');
      $actions['unpublish'] = [
        '#type' => 'link',
        '#title' => $this->t('Unpublish'),
        '#access' => $this->entity->access('administer tripal content'),
        '#attributes' => [
          'class' => ['button', 'button--danger', 'use-ajax'],
          'data-dialog-type' => 'modal',
          'data-dialog-options' => Json::encode([
            'width' => 880,
          ]),
        ],
        '#url' => $route_info,
        '#attached' => [
          'library' => ['core/drupal.dialog.ajax'],
        ],
      ];
    }
    return $actions;
  }
}
