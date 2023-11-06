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
    $form['collection_id'] = [
      '#type' => 'checkboxes',
      '#title' => 'Tripal Entity Type Collection',
      '#options' => [],
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
    // @todo Validate the form here.
    // Example:
    // @code
    //   if (mb_strlen($form_state->getValue('message')) < 10) {
    //     $form_state->setErrorByName(
    //       'message',
    //       $this->t('Message should be at least 10 characters.'),
    //     );
    //   }
    // @endcode
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state): void {

    // Let them know this isn't yet implemented.
    $this->messenger()->addStatus('This functionality has not yet been implemented.');

    // Redirect back to the Tripal Entity Type listing.
    $form_state->setRedirect('entity.tripal_entity_type.collection');
  }

}
