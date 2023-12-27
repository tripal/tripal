<?php declare(strict_types = 1);

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a Tripal form.
 */
final class ImportTripalEntityTypeCollection extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId(): string {
    return 'tripal_import_tripal_entity_type_collection';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state): array {

    $form['msg'] = [
      '#type' => 'markup',
      '#markup' => '
      <p>Choose 1+ of the collections listed below to <strong>create multiple content types
      and their associated fields</strong>. These collections were registered
      with Tripal by various modules who are listed in brackets after the collection name.</p>
      <p><strong>Note:</strong> If a collection contains a content type which already exists,
      then it will simply update it with the fields associated with your chosen collection.</p>',
    ];

    $manager = \Drupal::service('tripal.tripalentitytype_collection');
    $collections = $manager->getTypeCollections();
    $collections = $this->orderCollections($collections);

    $form['collection_id'] = [
      '#type' => 'checkboxes',
      '#title' => 'Tripal Entity Type Collection',
      '#options' => [],
      '#required' => TRUE,
    ];
    foreach ($collections as $id => $details) {
      $form['collection_id']['#options'][$id] = $details['label'];
      $form['collection_id'][$id]['#description'] = $details['description'];
    }

    $form['actions'] = [
      '#type' => 'actions',
      'submit' => [
        '#type' => 'submit',
        '#value' => $this->t('Import'),
      ],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state): void {
    // No validation needed.
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state): void {

    $args = [[
      'collection_ids' => $form_state->getValue('collection_id')
    ]];

    // Get the current user for the job.
    $current_user = \Drupal::currentUser();

    // Now create the job.
    $job_submitted = \Drupal::service('tripal.job')->create([
      'job_name' => "Import Tripal Entity Type Collection",
      'modulename' => 'tripal',
      'uid' => $current_user->id(),
      'callback' => 'import_tripalentitytype_collection',
      'arguments' => $args,
    ]);
    if ($job_submitted) {
      $this->messenger()->addStatus('Successfully submitted a Tripal Job to load these collections.');
    }

    // Redirect back to the Tripal Entity Type listing.
    $form_state->setRedirect('entity.tripal_entity_type.collection');
  }

  /**
   * Make sure that the list of collections is in dependency order.
   *
   * Some collections may require that other collections are
   * already present when the job to create the collection is run,
   * so ensure that the array is appropriately ordered.
   *
   * @param array $collections
   *   Array from \Drupal::service('tripal.tripalentitytype_collection')->getTypeCollections()
   *
   * @return array
   *   The same array, but in dependency order.
   */
  private function orderCollections(array $collections) {
    // Note that in the case of a yaml dependency typo, the collection
    // will never be added.
    $ordered_collections = [];
    $added = 1;
    while ($added) {
      $added = 0;
      foreach ($collections as $id => $details) {
        if (!array_key_exists($id, $ordered_collections)) {
          $ready = TRUE;
          if (array_key_exists('dependency', $details)) {
            foreach ($details['dependency'] as $dependency) {
              if (!array_key_exists($dependency, $ordered_collections)) {
                $ready = FALSE;
              }
            }
          }
          if ($ready) {
            $ordered_collections[$id] = $details;
            $added++;
          }
        }
      }
    }
    return $ordered_collections;
  }
}
