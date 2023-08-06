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
    $bundles = [];
    $datastores = [];

    // Get a list of TripalSTorage plugins
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');
    $storage_defs = $storage_manager->getDefinitions();
    foreach ($storage_defs as $plugin_id => $storage_def) {
      $datastores[$plugin_id] = $storage_def['label']->__toString();
    }

    // Get the available content types (bundles)
    $entity_types = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadByProperties([]);
    foreach ($entity_types as $entity_type) {
        $bundles[$entity_type->id()] = $entity_type->getLabel();
    }

    $form['datastore'] = [
      '#title' => 'Storage Backend',
      '#description' => 'Please select the data storage backend that should be used for publishing content.',
      '#type' => 'select',
      '#options' => $datastores,
      '#sort_options' => TRUE,
      '#required' => TRUE,
    ];

    /** @todo: what about different Chado instances? How do we let the user select those?**/

    $form['bundle'] = [
      '#title' => 'Content Type',
      '#description' => 'Please select a content type to publish.',
      '#type' => 'select',
      '#options' => $bundles,
      '#sort_options' => TRUE,
      '#required' => TRUE,
    ];

    $form['submit_button'] = [
      '#type' => 'submit',
      '#value' => t('Pubish'),
    ];

    return $form;
  }


  /**
   * Submit the form.
   */
  function submitForm(array &$form, FormStateInterface $form_state) {

    // Get the uid of the current user.
    //$user = User::load(\Drupal::currentUser()->id());
    $current_user = \Drupal::currentUser();
    $bundle = $form_state->getValue('bundle');
    $datastore = $form_state->getValue('datastore');
    $job_name = 'Publish ' . $bundle;

    // Invoke the job, which should call the publish job?
    tripal_add_job($job_name, 'tripal', 'tripal_publish', [$bundle, $datastore], $current_user->id());
  }

}
