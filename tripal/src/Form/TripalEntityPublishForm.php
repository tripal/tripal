<?php

namespace Drupal\tripal\Form;

//use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\user\Entity\User;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\Core;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\file\Entity\File;


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
    $publish_form_defaults = \Drupal::state()->get('tripal_publish_form_defaults', []);

    // Get a list of TripalStorage plugins
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

    // If there is only one datastore available, set it as the default.
    if (count($datastores) == 1) {
      $datastore = array_key_first($datastores);
      $form_state->setValue('datastore', $datastore);
      $form['datastore']['#default_value'] = $datastore;
    }

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

    $form['republish'] = [
      '#type' => 'checkbox',
      '#title' => 'Republish Existing Content',
      '#description' => 'Check this if the title format has been changed, if'
         . ' new fields have been added to the content type, or if records have'
         . ' been changed in Chado. The entity ID number will not be changed.',
      '#default_value' => $publish_form_defaults['republish'] ?? 0,
    ];

    $form['batch_size'] = [
      '#type' => 'textfield',
      '#title' => 'Batch Size',
      '#description' => t('Divide the publish job into separate batches of the specified size to minimize memory usage'),
      '#default_value' => $publish_form_defaults['batch_size'] ?? 1000,
    ];

    $form['submit_button'] = [
      '#type' => 'submit',
      '#value' => t('Publish'),
    ];

    // Add Tripal 3 Migration Settings to the form
    // in a closed-by-default fieldset
    $form = $this->migration_form($form, $publish_form_defaults);

    return $form;
  }

  /**
   * Tripal 3 migration section of the publish form
   *
   * @param array $form
   *   The form array
   * @param array $publish_form_defaults
   *   Default values from the previous invocation
   *
   * @return array
   *   The updated form array
   */
  protected function migration_form(array $form, array $publish_form_defaults): array {
    $allowed_types = ['txt', 'tsv'];

    // If there was a migration file used on the previous invocation,
    // have this section open, otherwise it is closed by default.
    $open_state = FALSE;
    if ($publish_form_defaults['migration_file'] ?? FALSE) {
      $open_state = TRUE;
    }
    $form['migration_options'] = [
      '#title' => t('Tripal 3 Migration Options'),
      '#type' => 'details',
      '#open' => $open_state,
    ];

    $url = 'https://tripaldoc.readthedocs.io/en/latest/upgrade_guide/site/migrating_chado.html';
    $url = Link::fromTextAndUrl('Migrating Chado', Url::fromUri($url))->toString();
    $form['migration_options']['text'] = [
      '#type' => 'item',
      '#markup' => '<p>' . t('The file specified here is used for migration of'
         . ' Tripal 3 entity ID values to this Tripal 4 site, and will have'
         . ' been generated by the ":prog" utility on your Tripal 3 site.'
         . ' For details on this procedure, see the Tripal 4 documentation for ',
         [':prog' => 'export_tripal3_entity_mapping.php']) . $url . '</p>',
    ];
    $user = \Drupal::currentUser();
    $existing_files = $this->getUploadedFiles($user->id(), '34-tripal_migration', $allowed_types, 'tripal');
    if (count($existing_files) > 0) {
      $fids = [0 => '--Select a file--'];
      foreach ($existing_files as $fid => $file) {
        $fids[$fid] = $file->getFilename() . ' (' . tripal_format_bytes($file->getSize()) . ') ';
      }
      $form['migration_options']['migration_file_upload_existing'] = [
        '#type' => 'select',
        '#title' => t('Existing Files'),
        '#description' => t('You may select a migration data file that is already uploaded.'),
        '#options' => $fids,
      ];
    }
    $form['migration_options']['migration_file_upload'] = [
      '#type' => 'html5_file',
      '#title' => $this->t('Upload a migration data file'),
      '#description' => 'Specify the name of the migration data file here if you wish to upload it.',
      '#usage_type' => 'tripal_migration',
      '#usage_id' => 34,
      '#allowed_types' => $allowed_types,
      '#cardinality' => 1,
    ];
    $form['migration_options']['migration_file_local'] = [
      '#title' => t('Server path'),
      '#type' => 'textfield',
      '#maxlength' => 5120,
      '#description' => t('If the migration data file is local to the Tripal server,'
                          . ' please provide the full path here.'),
      '#default_value' => $publish_form_defaults['migration_file'] ?? '',
    ];
    $form['migration_options']['lenient_migration'] = [
      '#type' => 'checkbox',
      '#title' => t('Lenient Migration'),
      '#description' => t('This allows skipping over missing records in the migration data file.'
                          . ' This can happen if there were records on the Tripal 3 site that'
                          . ' had not been published.'),
      '#default_value' => $publish_form_defaults['lenient_migration'] ?? 0,
    ];

    return $form;
  }

  /**
   * Get a list of uploaded files
   *
   * This is planned to be added to a new class to be shared with the
   * importer base class to handle file upload form elements. This
   * function would replace the api function tripal_get_user_uploads().
   *
   * @param int $uid
   *   The file owner
   * @param string $type
   *   The usage id and type, e.g. "34-tripal_migration"
   * @param array $allowed_types
   *   Optional filter for allowed file extensions
   * @param string $module
   *   Optional since this will always be 'tripal'
   *
   * @return array
   *   Associative array, key is fid, value is file object
   */
  protected function getUploadedFiles(int $uid, string $type, array $allowed_types = [], string $module = 'tripal'): array {
    $uploaded_files = [];

    $db = \Drupal::database();
    $query = $db->select('file_usage', 'F');
    $query->condition('F.type', $type);
    $query->condition('F.module', $module);
    $query->fields('F', ['fid']);
    $files = $query->execute();
    while ($fid = $files->fetchField()) {
      /** @var Drupal\file\Entity\File **/
      $file_obj = \Drupal\file\Entity\File::load($fid);
      if ($file_obj->getOwnerId() == $uid) {
        $valid = $allowed_types?FALSE:TRUE;
        foreach ($allowed_types as $type) {
          if (preg_match('/\.' . $type . '$/', $file_obj->getFilename())) {
            $valid = TRUE;
          }
        }
        if ($valid) {
          $uploaded_files[$fid] = $file_obj;
        }
      }
    }

    return $uploaded_files;
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

    $batch_size = $form_state->getValue('batch_size');
    if (!preg_match('/^[0-9]+$/', $batch_size) or ($batch_size < 1)) {
      $form_state->setErrorByName('batch_size', t('The batch size must be a positive integer.'));
    }

    if ($storage_manager->datastoreExists($datastore) !== TRUE) {
      $form_state->setErrorByName('datastore',t('The chosen datastore is not registered properly with TripalStorage.'));
    }
    // Only try to call the datastore custom validation if the datastore actually exists.
    else {
      $storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
      $storage->publishFormValidate($form, $form_state);
    }

    // Optional migration data file can be selected in one of three ways
    $migration_file = '';
    $migration_file_upload_existing = $form_state->getValue('migration_file_upload_existing');
    $migration_file_upload = $form_state->getValue('migration_file_upload');
    $migration_file_local = $form_state->getValue('migration_file_local');
    if ($migration_file_upload) {
      $file = File::load($migration_file_upload);
      $migration_file = \Drupal::service('file_system')->realpath($file->getFileUri());
    }
    elseif ($migration_file_upload_existing) {
      $file = File::load($migration_file_upload_existing);
      $migration_file = \Drupal::service('file_system')->realpath($file->getFileUri());
    }
    elseif ($migration_file_local) {
      $migration_file = $migration_file_local;
    }
    if ($migration_file and !file_exists($migration_file)) {
      $form_state->setErrorByName('migration_file_local', 'The specified migration file does not exist');
    }
    $form_state->setValue('migration_file', $migration_file);
  }


  /**
   * Submit the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $bundle = $form_state->getValue('bundle');
    $datastore = $form_state->getValue('datastore');

    // Other values will be passed in as options. These should be values
    // provided by the storage backend form elements. Only include
    // those items from the form values that publish will need.
    $values = $form_state->getValues();
    $options = [];
    $publish_options = ['republish', 'batch_size', 'migration_file', 'lenient_migration'];
    foreach ($publish_options as $key) {
      $options[$key] = $values[$key];
    }

    // Store the current form settings as the default for the next time publish is run
    \Drupal::state()->set('tripal_publish_form_defaults', $options);

    // Run the form submit for the storage backend.
    /** @var \Drupal\tripal\TripalStorage\PluginManager\TripalStorageManager $storage_manager **/
    $storage_manager = \Drupal::service('tripal.storage');
    $storage = $storage_manager->getInstance(['plugin_id' => $datastore]);
    $storage->publishFromSubmit($form, $form_state);

    // Add the publish job.
    $current_user = \Drupal::currentUser();
    $job_args = [$bundle, $datastore, $options];
    $job_name = 'Publish pages of type: ' . $bundle;
    \Drupal::service('tripal.job')->create([
      'job_name' => $job_name,
      'modulename' => 'tripal',
      'callback' => [\Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager::class, 'runTripalJob'],
      'arguments' => $job_args,
      'uid' => $current_user->id(),
    ]);
  }
}
