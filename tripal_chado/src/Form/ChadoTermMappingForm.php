<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Form handler for the Example add and edit forms.
 */
class ChadoTermMappingForm extends EntityForm {

  /**
   * Constructs an ExampleForm object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entityTypeManager
   *   The entityTypeManager.
   */
  public function __construct(EntityTypeManagerInterface $entityTypeManager) {
    $this->entityTypeManager = $entityTypeManager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('entity_type.manager'));
  }

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);


    $chado_tables = $this->entity->getTableNames();
    $chado_table = $form_state->hasValue('chado_table') ? $form_state->getValue('chado_table') : $chado_tables[0];

    $form['chado_table'] = [
      '#type' => 'select',
      '#title' => 'Chado Table',
      '#description' => t('Select a chado table to view colum to term mappings.'),
      '#options' => $chado_tables,
      '#default_value' => $chado_table,
      '#ajax' => [
        'callback' =>  [$this, 'formAjaxCallback'],
        'wrapper' => 'obo-existing-fieldset',
      ],
    ];

    if ($chado_table) {
      $chado = \Drupal::service('tripal_chado.database');
      $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
      $schema = $chado->schema();
      $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);
      $pk = $schema_def['primary key'][0];
      $columns = $schema_def['fields'];
      $header = [
        'Column',
        'Term',
        [
          'data' => 'Name',
          'nowrap' => TRUE,
        ],
        'Term Definition',
        'Vocabulary',
        'Action',
      ];
      $rows = [];
      foreach ($columns AS $column => $detail) {
        // Do not show column if it's the primary key or default cv
        if ($column == $pk) {
          continue;
        }

        $term_id = $this->entity->getColumnTermId($chado_table, $column);

        $term_name = '';
        $term_desc = '';
        $term_desc = '';
        $vocab_name = '';
        if (!empty($term_id)) {
          list($idSpace_name, $accession) = explode(':', $term_id);
          /**
           * @var \Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface $idspace
           */
          $idSpace = $idSpace_manager->loadCollection($idSpace_name);
          if ($idSpace) {
            $term = $idSpace->getTerm($accession);
            $term_name = $term->getName();
            $term_desc = $term->getDefinition();
            $vocab  = $term->getVocabularyObject();
            $vocab_name = $vocab->getName();
          }
        }
        $rows[] = [
          $column,
          $term_id,
          $term_name,
          $term_desc,
          $vocab_name,
        ];
      }
      $form['term_table'] = [
        '#type' => 'table',
        '#header' => $header,
        '#rows' => $rows,
      ];
    }

    // You will need additional form elements for your custom properties.
    return $form;
  }

  /**
   * Ajax callback for the form() function.
   *
   * @param array $form
   *   The form array.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function formAjaxCallback($form, &$form_state) {

//     $uobo_name = $form['obo_existing']['uobo_name']['#default_value'];
//     $uobo_url = $form['obo_existing']['uobo_url']['#default_value'];
//     $uobo_file = $form['obo_existing']['uobo_file']['#default_value'];

//     $response = new AjaxResponse();
//     $response->addCommand(new ReplaceCommand('#obo-existing-fieldset', $form['obo_existing']));
//     $response->addCommand(new InvokeCommand('#edit-uobo-name', 'val', [$uobo_name]));
//     $response->addCommand(new InvokeCommand('#edit-uobo-url', 'val', [$uobo_url]));
//     $response->addCommand(new InvokeCommand('#edit-uobo-file', 'val', [$uobo_file]));

//     return $response;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $example = $this->entity;
    $status = $example->save();

    if ($status === SAVED_NEW) {
      $this->messenger()->addMessage($this->t('The %label mapping created.', [
        '%label' => $example->label(),
      ]));
    }
    else {
      $this->messenger()->addMessage($this->t('The %label mapping updated.', [
        '%label' => $example->label(),
      ]));
    }

    $form_state->setRedirect('entity.chado_term_mapping.collection');
  }

  /**
   * Helper function to check whether an Example configuration entity exists.
   */
  public function exist($id) {
    $entity = $this->entityTypeManager->getStorage('chado_term_mapping')->getQuery()
      ->condition('id', $id)
      ->execute();
    return (bool) $entity;
  }

}