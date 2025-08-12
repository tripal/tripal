<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\tripal\Services\TripalJob;
use Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager;
use Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a form for unpublishing Tripal content.
 *
 * @package Drupal\tripal\Form
 *
 * @ingroup tripal
 */
class TripalEntityUnpublishMultipleForm extends FormBase {

  /**
   * The state of the drupal site providing key => value storage.
   *
   * @var \Drupal\Core\State\StateInterface
   */
  protected StateInterface $state;

  /**
   * The Tripal storage manager.
   *
   * @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager
   */
  protected TripalStorageManager $tripal_storage_manager;

  /**
   * The Drupal Entity Type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected EntityTypeManagerInterface $entity_type_manager;

  /**
   * The Tripal Job service.
   *
   * @var \Drupal\tripal\Services\TripalJob
   */
  protected TripalJob $tripal_job_service;

  /**
   * The current user account.
   *
   * @var \Drupal\Core\Session\AccountInterface
   */
  protected AccountInterface $current_user;

  /**
   * The constructor.
   *
   * @param \Drupal\Core\State\StateInterface $state
   *   The state of the drupal site providing key => value storage.
   * @param \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $tripal_storage_manager
   *   The Tripal storage manager.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The Drupal Entity Type manager.
   * @param \Drupal\tripal\Services\TripalJob $tripal_job_service
   *   The Tripal Job service.
   * @param \Drupal\Core\Session\AccountInterface $current_user
   *   The current user account.
   */
  public function __construct(
    StateInterface $state,
    TripalStorageManager $tripal_storage_manager,
    EntityTypeManagerInterface $entity_type_manager,
    TripalJob $tripal_job_service,
    AccountInterface $current_user,
  ) {

    $this->state = $state;
    $this->tripal_storage_manager = $tripal_storage_manager;
    $this->entity_type_manager = $entity_type_manager;
    $this->tripal_job_service = $tripal_job_service;
    $this->current_user = $current_user;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('state'),
      $container->get('tripal.storage'),
      $container->get('entity_type.manager'),
      $container->get('tripal.job'),
      $container->get('current_user'),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId(): string {
    return 'tripal_content_unpublish_multiple_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state): array {
    $bundles = [];
    $datastores = [];
    $unpublish_form_defaults = $this->state->get('tripal_unpublish_form_defaults', []);

    // Get a list of TripalStorage plugins.
    $storage_defs = $this->tripal_storage_manager->getDefinitions();
    foreach ($storage_defs as $plugin_id => $storage_def) {
      // Don't use the Tripal default 'drupal_sql_storage' plugin
      // as a source for publishing records.
      if ($plugin_id == 'drupal_sql_storage') {
        continue;
      }
      $datastores[$plugin_id] = $storage_def['label']->__toString();
    }

    // Get the available content types (bundles)
    $entity_types = $this->entity_type_manager
      ->getStorage('tripal_entity_type')
      ->loadByProperties([]);
    foreach ($entity_types as $entity_type) {
      $bundles[$entity_type->id()] = $entity_type->getLabel();
    }

    $form['msg'] = [
      '#type' => 'markup',
      '#markup' => '
      <p>Unpublishing content prevents it from being displayed on this site, however the underlying data
         remains in the data store. If necessary these records can be published again later, but note
         that the entity numeric identifier will not be the same if the content is republished.</p>',
    ];

    $form['orphaned'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Only unpublish orphaned content'),
      '#description' => $this->t('Sometimes published content can become orphaned.
         This can occur if records are removed directly from the underlying data store,
         yet Tripal is not aware of it. When this is selected, only this orphaned content
         is unpublished, so that stored data and displayed data are synchronized.'),
      '#default_value' => $unpublish_form_defaults['orphaned'] ?? 1,
    ];

    $form['datastore'] = [
      '#title' => 'Storage Backend',
      '#description' => $this->t('Please select the data storage backend that should be used for unpublishing content.'),
      '#type' => 'select',
      '#options' => $datastores,
      '#sort_options' => TRUE,
      '#required' => TRUE,
      '#ajax' => [
        'callback' => '::storageAjaxCallback',
        'wrapper' => 'storage-options',
      ],
    ];

    // If there is only one datastore available, set it as the default.
    if (count($datastores) == 1) {
      $datastore = array_key_first($datastores);
      $form_state->setValue('datastore', $datastore);
      $form['datastore']['#default_value'] = $datastore;
    }

    $form['storage-options'] = [
      '#type' => 'details',
      '#description' => $this->t('Please select a storage backend for additional options.'),
      '#title' => 'Storage Options',
      '#prefix' => '<div id="storage-options">',
      '#suffix' => '</div>',
      '#open' => TRUE,
    ];

    // If the user has selected the data storage backend then add any
    // form options to it that the storage backend needs.
    if ($datastore = $form_state->getValue('datastore') and $this->tripal_storage_manager->datastoreExists($datastore)) {
      $storage = $this->tripal_storage_manager->getInstance(['plugin_id' => $datastore]);
      $datastore_form = $storage->publishForm($form, $form_state);
      if (!empty($datastore_form)) {
        $form['storage-options'] = array_merge_recursive($form['storage-options'], $datastore_form);
      }
      else {
        $form['storage-options']['#description'] = $this->t('The storage backend did not provide any additional options.');
      }
    }

    $form['bundle'] = [
      '#title' => 'Content Type',
      '#description' => $this->t('Please select a content type to unpublish.'),
      '#type' => 'select',
      '#options' => $bundles,
      '#sort_options' => TRUE,
      '#required' => TRUE,
    ];

    $form['actions'] = [
      '#type' => 'actions',
      'submit' => [
        '#type' => 'submit',
        '#value' => $this->t('Unpublish'),
      ],
    ];

    return $form;
  }

  /**
   * AJAX callback for storage backend updates.
   *
   * @param array $form
   *   The form array.
   * @param Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return array
   *   Associative array of storage options.
   */
  public function storageAjaxCallback(array &$form, FormStateInterface $form_state): array {
    return $form['storage-options'];
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state): void {
    $datastore = $form_state->getValue('datastore');

    // Run the form validate for the storage backend.
    if ($this->tripal_storage_manager->datastoreExists($datastore) !== TRUE) {
      $form_state->setErrorByName('datastore', $this->t('The chosen datastore is not registered properly with TripalStorage.'));
    }
    // Only try to call the datastore custom validation if the
    // datastore actually exists.
    else {
      $storage = $this->tripal_storage_manager->getInstance(['plugin_id' => $datastore]);
      // Validation here is the same as for publishing.
      $storage->publishFormValidate($form, $form_state);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state): void {

    $bundle = $form_state->getValue('bundle');
    $datastore = $form_state->getValue('datastore');
    $orphaned = $form_state->getValue('orphaned');
    $options = [
      'unpublish' => TRUE,
      'orphaned' => $orphaned,
    ];

    // Store the current form settings as the default for the next
    // time unpublish is run.
    $this->state->set('tripal_unpublish_form_defaults', $options);

    // Run the form submit for the storage backend.
    $storage = $this->tripal_storage_manager->getInstance(['plugin_id' => $datastore]);
    $storage->publishFromSubmit($form, $form_state);

    // Add the unpublish job.
    $job_args = [$bundle, $datastore, $options];
    $job_name = 'Unpublish ' . ($orphaned ? 'orphaned' : 'all') . ' pages of type: ' . $bundle;
    $this->tripal_job_service->create([
      'job_name' => $job_name,
      'modulename' => 'tripal',
      'callback' => [TripalBackendPublishManager::class, 'runTripalJob'],
      'arguments' => $job_args,
      'uid' => $this->current_user->id(),
    ]);
  }

}
