<?php

namespace Drupal\tripal_chado\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Form handler for the Chado Term Mapping Configuration entity add and edit forms.
 */
class ChadoTermMappingForm extends EntityForm {

  /**
   * Constructs an ChadoTermMappingForm object.
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
    $chado_table = $form_state->hasValue('chado_table') ? $form_state->getValue('chado_table') : 0;
    $table_name = $chado_tables[$chado_table];

    $form['id'] = [
      '#type' => 'item',
      '#title' => 'Term Mapping Configuration',
      '#markup' => $this->entity->label(),
    ];
    $form['instructions'] = [
      '#type' => 'item',
      '#title' => 'Instructions',
      '#markup' => $this->t('The following drop down contains the list of tables ' .
          'that are configured using this mapping configuration. By default the ' .
          '"Core Chado Term Mapping" should provide configurations for all Chado tables ' .
          'and custom tables provided by Tripal. Extension modules will contain mappings ' .
          'for their own additions.'),
    ];
    $form['chado_table'] = [
      '#type' => 'select',
      '#title' => 'Chado Table',
      '#description' => t('Select a chado table to view colum to term mappings.'),
      '#options' => $chado_tables,
      '#default_value' => $chado_table,
      '#ajax' => [
        'callback' =>  [$this, 'formAjaxCallback'],
        'wrapper' => 'chado-terms-table',
        'effect' => 'fade'
      ],
    ];


    $chado = \Drupal::service('tripal_chado.database');
    $idSpace_manager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $schema = $chado->schema();
    $schema_def = $schema->getTableDef($table_name, ['format' => 'Drupal']);
    $pk = $schema_def['primary key'][0];
    $columns = $schema_def['fields'];
    $header = [
      'Chado Column',
      'Term',
      [
        'data' => 'Name',
        'nowrap' => TRUE,
      ],
      'Term Definition',
      'Action',
    ];
    $rows = [];
    foreach ($columns AS $column => $detail) {
      // Do not show column if it's the primary key or default cv
      if ($column == $pk) {
        continue;
      }


      $term_name = '';
      $term_desc = '';
      $term_desc = '';
      $buttons = '';

      $term_id = $this->entity->getColumnTermId($table_name, $column);
      if (!empty($term_id)) {
        list($idSpace_name, $accession) = explode(':', $term_id);
        $idSpace = $idSpace_manager->loadCollection($idSpace_name);
        if ($idSpace) {
          $term = $idSpace->getTerm($accession);
          if ($term) {
            $term_name = $term->getName();
            $term_desc = $term->getDefinition();
          }
          else {
            $messenger = \Drupal::messenger();
            $messenger->addWarning($this->t('The term, @term, has been assigned to column, @column, but has not been added to Tripal.',
                ['@term' => $term_id, '@column' => $column]));
          }
        }
      }
      $rows[] = [
        $column,
        $term_id,
        $term_name,
        $term_desc,
        $buttons
      ];
    }

    $form['term_table'] = [
      '#type' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#prefix' => '<div id="chado-terms-table">',
      '#suffix' => '</div>',
    ];

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
    return $form['term_table'];
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {

    $this->messenger()->addMessage($this->t('There are no changes to save.'));

    /*
    $entity = $this->entity;
    $status = $entity->save();

    if ($status === SAVED_NEW) {
      $this->messenger()->addMessage($this->t('The %label mapping created.', [
        '%label' => $entity->label(),
      ]));
    }
    else {
      $this->messenger()->addMessage($this->t('The %label mapping updated.', [
        '%label' => $entity->label(),
      ]));
    }
    */

    $form_state->setRedirect('entity.chado_term_mapping.collection');
  }

  /**
   * Helper function to check whether a Chado Term Mapping configuration entity exists.
   */
  public function exist($id) {
    $entity = $this->entityTypeManager->getStorage('chado_term_mapping')->getQuery()
      ->condition('id', $id)
      ->execute();
    return (bool) $entity;
  }

}
