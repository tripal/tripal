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
      // Don't use the Tripal default 'drupal_sql_storage' plugin
      // as a source for publishing records.
      if ($plugin_id == 'drupal_sql_storage') {
        continue;
      }
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
      '#ajax' => [
        'callback' => '::storageAjaxCallback',
        'wrapper' => 'storage-options'
      ],
    ];
    $form['storage-options'] = [
      '#type' => 'details',
      '#description' => 'Please select a storage backend for additional options.',
      '#title' => 'Storage Options',
      '#prefix' => '<div id="storage-options">',
      '#suffix' => '</div>',
      '#open' => TRUE,
    ];

    // If the user has selected the data storage backend then add any
    // form options to it that the storage backend needs.
    if ($datastore = $form_state->getValue('datastore') AND $storage_manager->datastoreExists($datastore)) {
      $storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
      $datastore_form = $storage->publishForm($form, $form_state);
      if (!empty($datastore_form)) {
        $form['storage-options'] = array_merge_recursive($form['storage-options'], $datastore_form);
      }
      else {
        $form['storage-options']['#description'] = t('The storage backend did not provide any additional options.');
      }
    }

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
      '#value' => t('Publish'),
    ];

    return $form;
  }

  /**
   * AJAX callback for storage backend updates.
   *
   * @param array $form
   *   The form array
   * @param FormStateInterface $form_state
   *   The form state object.
   */
  public function storageAjaxCallback(array &$form, FormStateInterface $form_state) {
    return $form['storage-options'];
  }

  /**
   *
   * {@inheritDoc}
   * @see \Drupal\Core\Form\FormBase::validateForm()
   */
  public function validateForm(array &$form, $form_state) {
    $bundle = $form_state->getValue('bundle');
    $datastore = $form_state->getValue('datastore');

    // Run the form validate for the storage backend.
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');

    if ($storage_manager->datastoreExists($datastore) !== TRUE) {
      $form_state->setErrorByName('datastore',t('The chosen datastore is not registered properly with TripalStorage.'));
    }
    // Only try to call the datastore custom validation if the datastore actually exists.
    else {
      $storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
      $storage->publishFormValidate($form, $form_state);
    }
  }


  /**
   * Submit the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $bundle = $form_state->getValue('bundle');
    $datastore = $form_state->getValue('datastore');

    // All otehr values will be passed in as options. These should be
    // values provided by the storage backend form elements.  Take out
    // those items we don't need.
    $values = $form_state->getValues();
    unset($values['submit_button']);
    unset($values['form_build_id']);
    unset($values['form_token']);
    unset($values['form_id']);
    unset($values['bundle']);
    unset($values['datastore']);
    unset($values['op']);

    // Run the form submit for the storage backend.
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');
    $storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
    $storage->publishFromSubmit($form, $form_state);

    // Add the publish job.
    $current_user = \Drupal::currentUser();
    $job_args = [$bundle, $datastore, $values];
    $job_name = 'Publish pages of type: ' . $bundle;
    tripal_add_job($job_name, 'tripal', 'tripal_publish', $job_args, $current_user->id());
  }
}
