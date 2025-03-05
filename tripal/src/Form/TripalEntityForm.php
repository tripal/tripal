<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\Component\Serialization\Json;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\Core\Entity\EntityRepositoryInterface;
use Drupal\Core\Entity\EntityTypeBundleInfoInterface;
use Drupal\Component\Datetime\TimeInterface;
use Drupal\Core\Datetime\DateFormatterInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Form controller for Tripal Content edit forms.
 *
 * @ingroup tripal
 */
class TripalEntityForm extends ContentEntityForm {
  use MessengerTrait;

  /**
   * The date formatter service.
   *
   * @var \Drupal\Core\Datetime\DateFormatterInterface
   */
  protected $dateFormatter;

  /**
   * Constructs a ContentEntityForm object.
   *
   * @param \Drupal\Core\Entity\EntityRepositoryInterface $entity_repository
   *   The entity repository service.
   * @param \Drupal\Core\Entity\EntityTypeBundleInfoInterface $entity_type_bundle_info
   *   The entity type bundle service.
   * @param \Drupal\Component\Datetime\TimeInterface $time
   *   The time service.
   * @param \Drupal\Core\Datetime\DateFormatterInterface $date_formatter
   *   The date formatter service.
   */
  public function __construct(EntityRepositoryInterface $entity_repository, EntityTypeBundleInfoInterface $entity_type_bundle_info, TimeInterface $time, DateFormatterInterface $date_formatter) {
    parent::__construct($entity_repository, $entity_type_bundle_info, $time);

    $this->dateFormatter = $date_formatter;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('entity.repository'),
      $container->get('entity_type.bundle.info'),
      $container->get('datetime.time'),
      $container->get('date.formatter')
    );
  }

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

    // -- If the theme being used is claro we want to add it's specific brand
    // of styling. This is what it does in claro_form_node_form_alter
    $theme = \Drupal::service('theme.manager')->getActiveTheme()->getName();
    if ($theme == 'claro') {
      $form['#attached']['library'][] = 'claro/form-two-columns';
      $form['advanced']['#type'] = 'container';
      $form['advanced']['#accordion'] = TRUE;
      $form['meta']['#type'] = 'container';
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);
    $entity = $this->entity;

    // Display an error message if the title format is not valid.
    if (!$this->validateTitleFormat()) {
      // @todo I would like to disable the save button here, but it's not in the form yet.
    }

    // -- Setup advanced sidebar.
    // Additional collapsed regions can be added to this group by creating
    // a field group of type "Details Siderbar" and adding fields to it.
    if (!isset($form['advanced'])) {
      $form['advanced'] = [
        '#type' => 'vertical_tabs',
        '#weight' => 99,
      ];
    }
    $form['advanced']['#attributes']['class'][] = 'entity-meta';

    // -- Metadata details in sidebar.
    $form['meta'] = [
      '#type' => 'details',
      '#group' => 'advanced',
      '#weight' => -10,
      '#title' => $this->t('Status'),
      '#attributes' => ['class' => ['entity-meta__header']],
      '#tree' => TRUE,
    ];
    $form['meta']['changed'] = [
      '#type' => 'item',
      '#title' => $this->t('Last saved'),
      '#markup' => !$entity->isNew() ? $this->dateFormatter->format($entity->getChangedTime(), 'short') : $this->t('Not saved yet'),
      '#wrapper_attributes' => ['class' => ['entity-meta__last-saved']],
      '#weight' => -5,
    ];
    $owner = $entity->getOwner();
    $author = $owner?$owner->getAccountName():'';
    $form['meta']['author'] = [
      '#type' => 'item',
      '#title' => $this->t('Author'),
      '#markup' => $author,
      '#wrapper_attributes' => ['class' => ['entity-meta__author']],
      '#weight' => -4,
    ];

    // -- Title disabled and added to metadata
    if ($entity->isNew()) {
      $form['title']['#weight'] = -2;
      $form['title']['#disabled'] = TRUE;
      $form['title']['#group'] = 'meta';
    }
    else {
      $form['meta']['title'] = [
        '#type' => 'item',
        '#title' => t('Title'),
        '#title_display' => 'before',
        '#description' => $form['title']['widget'][0]['#description'],
        '#field_prefix' => '<h3>',
        '#markup' => $entity->label(),
        '#field_suffix' => '</h3>',
        '#weight' => -2,
      ];
      $form['title']['#type'] = 'hidden';
    }

    // -- URL Alias
    $form['url_paths'] = [
      '#type' => 'details',
      '#title' => $this->t('URL Paths'),
      '#group' => 'advanced',
      '#attributes' => [
        'class' => ['path-form'],
      ],
      '#attached' => [
        'library' => ['path/drupal.path'],
      ],
      '#weight' => 90,
      '#optional' => TRUE,
      '#description' => '<p>All Tripal Content will be given a canonical URL of the form "bio_data/[entity_id]" and additionally, they will all be given a URL alias. The URL alias is user facing and will be shown in the address bar when viewing content.</p>
      <p>By default the URL alias will be assigned when a page after a page is created based on a pattern configured by administrators for this content type. If you would like to change it for this specific page, you can do so using the URL Alias field below.</p>
      <p>There should only be a single URL alias but if you would like the page to be accessed from multiple URLs, you can add URL redirects.</p>'
    ];
    $form['path']['#group'] = 'url_paths';

    // -- Author information for administrators.
    $form['author'] = [
      '#type' => 'details',
      '#title' => $this->t('Authoring information'),
      '#group' => 'advanced',
      '#attributes' => [
        'class' => ['node-form-author'],
      ],
      '#weight' => 90,
      '#optional' => TRUE,
    ];

    if (isset($form['uid'])) {
      $form['uid']['#group'] = 'author';
    }

    if (isset($form['created'])) {
      $form['created']['#group'] = 'author';
    }

    return $form;
  }

  /**
   * Check the title format for this content type for validity.
   *
   * @return bool
   *   TRUE if title_format is valid, FALSE if not valid.
   */
  private function validateTitleFormat() {
    $bundle_id = $this->entity->getType();
    $bundle_entity = \Drupal\tripal\Entity\TripalEntityType::load($bundle_id);
    $title_format = $bundle_entity->getTitleFormat();
    $message = '';
    if (!preg_match('/\[.*\]/', $title_format)) {
      $message = 'The Page Title Format for this content type does not contain any tokens.';
    }
    elseif ($title_format == 'Entity [TripalEntity__entity_id]') {
      $message = 'The Page Title Format for this content type is the default generic format.';
    }
    if ($message) {
      $url = '/admin/structure/bio_data/manage/' . $bundle_id;
      $message .= ' You must update the title format before creating any new content.'
        . ' <a href=":url">Click here</a> to update the title format.';
      \Drupal::messenger()->addError($this->t($message, [':url' => $url]));
      return FALSE;
    }
    else {
      return TRUE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $values = $form_state->getValues();
    $bundle = $this->entity->getType();
    $bundle_entity = \Drupal\tripal\Entity\TripalEntityType::load($bundle);
    $this->entity->setOwnerId($values['uid'][0]['target_id']);

    $msg = '';
    try {
      $status = parent::save($form, $form_state);
    }
    catch (\Exception $e) {
      $status = 'exception';
      $msg = $e->getMessage();
    }

    switch ($status) {
      case SAVED_NEW:
        $this->messenger()->addMessage($this->t('Created the %label.', [
          '%label' => $bundle_entity->label(),
        ]));
        break;

      case SAVED_UPDATED:
        $this->messenger()->addMessage($this->t('Updated the %label.', [
          '%label' => $bundle_entity->label(),
        ]));
        break;

      case 'exception':
        $this->messenger()->addError($this->t('Error, we were unable to save this page. %msg', [
          '%msg' => $msg,
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label.', [
          '%label' => $bundle_entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.tripal_entity.canonical', ['tripal_entity' => $this->entity->id()]);
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
