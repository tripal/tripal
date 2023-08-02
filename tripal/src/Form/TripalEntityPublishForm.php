<?php

namespace Drupal\tripal\Form;

//use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\user\Entity\User;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\Core;

//class TripalEntityPublishForm extends ContentEntityForm {

/**
 * Form that asks the user (admin) which content type they want to publish.
 */
class TripalEntityPublishForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'content_bio_data_publish_form';
  }

  /**
   * Build the form.
   */
  function buildForm(array $form, FormStateInterface $form_state) {
    // Get a list of all types.
    $bundle_entities = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties([]);

    // Get the available content types (bundles) and sort them
    // for user convenience. They exist in this form:
    // "bio_data_1" => "Organism"
    //$bundles = ['select'] = 'Select';
    foreach ($bundle_entities as $entity) {
      $bundles[$entity->getName()] = $entity->getLabel();
    }
    // arsort($bundles);
    // $bundles['select'] = 'Select';
    // $bundles = array_reverse($bundles);

    $form['bundle'] = [
      '#title' => 'Content Type',
      '#description' => 'Please select a content type to publish.',
      '#type' => 'select',
      '#options' => $bundles,
      '#sort_options' => TRUE,
      '#default_value' => 'bio_data_1', // Pumpkin.
      '#required' => TRUE,
    ];

    $form['submit_button'] = [
      '#type' => 'submit',
      '#value' => t('Pubish'),
    ];

    return $form;
  }

  // /**
  //  * Validate the form.
  //  *
  //  * @ todo this.
  //  */
  // function validateForm() {
  //   // Do something.
  // }

  /**
   * Submit the form.
   */
  function submitForm(array &$form, FormStateInterface $form_state) {

    // Get the uid of the current user.
    //$user = User::load(\Drupal::currentUser()->id());
    $current_user = \Drupal::currentUser();
    $bundle = $form_state->getValue('bundle');
    $job_name = 'Publish ' . $bundle;

    // Invoke the job, which should call the publish job?
    tripal_add_job($job_name, 'tripal', 'tripal_publish', [$bundle], $current_user->id());
  }

}
