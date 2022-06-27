<?php
/**
 * @file
 * Contains \Drupal\tripal\Form\TripalImporterForm.
 */
namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerInterface;

/**
 * Provides a test form object.
 */
class TripalImporterForm implements FormInterface {
  /**
   * {@inheritdoc}
   */
  public function getFormID() {
    return 'tripal_admin_form_tripalimporter';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $plugin_id = NULL) {

    if (!$plugin_id) {
      return $form;
    }

    $user = \Drupal::currentUser();


    // Load the specific importer from the plugin_id
    $importer_manager = \Drupal::service('tripal.importer');
    $importer = $importer_manager->createInstance($plugin_id);
    $importer_def = $importer_manager->getDefinitions()[$plugin_id];

    $form['#title'] = $importer_def['label'];

    $form['importer_plugin_id'] = [
      '#type' => 'value',
      '#value' => $plugin_id,
    ];

    if ((array_key_exists('file_upload', $importer_def) and $importer_def['file_upload'] == TRUE) or
        (array_key_exists('file_local', $importer_def) and $importer_def['file_local'] == TRUE) or
        (array_key_exists('file_remote', $importer_def) and $importer_def['file_remote'] == TRUE)) {
      $form['file'] = [
        '#type' => 'fieldset',
        '#title' => $importer_def['upload_title'],
        '#description' => $importer_def['upload_description'],
        '#weight' => -15,
      ];
    }

    if (array_key_exists('file_upload', $importer_def) and $importer_def['file_upload'] == TRUE) {
      $existing_files = tripal_get_user_uploads($user->id(), $importer_def['file_types']);
      if (count($existing_files) > 0) {
        $fids = [0 => '--Select a file--'];
        foreach ($existing_files as $fid => $file) {
          $fids[$fid] = $file->getFilename() . ' (' . tripal_format_bytes($file->getSize()) . ') ';
        }
        $form['file']['file_upload_existing'] = [
          '#type' => 'select',
          '#title' => t('Existing Files'),
          '#description' => t('You may select a file that is already uploaded.'),
          '#options' => $fids,
        ];
      }
      $form['file']['file_upload'] = [
        '#type' => 'html5_file',
        '#title' => '',
        '#description' => 'Remember to click the "Upload" button below to send ' .
            'your file to the server.  This interface is capable of uploading very ' .
            'large files.  If you are disconnected you can return, reload the file and it ' .
            'will resume where it left off.  Once the file is uploaded the "Upload ' .
            'Progress" will indicate "Complete".  If the file is already present on the server ' .
            'then the status will quickly update to "Complete".',
        '#usage_type' => 'tripal_importer',
        '#usage_id' => 0,
        '#allowed_types' => $importer_def['file_types'],
        '#cardinality' => $importer_def['cardinality'],
      ];
    }

    if (array_key_exists('file_local', $importer_def) and $importer_def['file_local'] == TRUE) {
      $form['file']['file_local'] = [
        '#title' => t('Server path'),
        '#type' => 'textfield',
        '#maxlength' => 5120,
        '#description' => t('If the file is local to the Tripal server please provide the full path here.'),
      ];
    }

    if (array_key_exists('file_remote', $importer_def) and $importer_def['file_remote'] == TRUE) {
        $form['file']['file_remote'] = [
          '#title' => t('Remote path'),
          '#type' => 'textfield',
          '#maxlength' => 5102,
          '#description' => t('If the file is available via a remote URL please provide the full URL here.  The file will be downloaded when the importer job is executed.'),
        ];
    }

    // Add the analysis form element if needed. Each importer should add this
    // appropriate for the data store it uses.
    if ($importer_def['use_analysis']) {
      $analysis_form = $importer->addAnalysis($form, $form_state);
      if (is_array($analysis_form)) {
        $form = array_merge($form, $analysis_form);
      }
    }

    // Add the importer custom form elements
    $importer_form = $importer->form($form, $form_state);
    if (is_array($importer_form)) {
      $form = array_merge($form, $importer_form);
    }


    $form['button'] = [
      '#type' => 'submit',
      '#value' => $importer_def['button_text'],
      '#weight' => 10,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $user = \Drupal::currentUser();

    //$run_args = $form_state['values'];
    $form_values = $form_state->getValues();
    $run_args = $form_values;
    $plugin_id = $form_values['importer_plugin_id'];

    $importer_manager = \Drupal::service('tripal.importer');
    $importer = $importer_manager->createInstance($plugin_id);
    $importer_def = $importer_manager->getDefinitions()[$plugin_id];

    // Remove the file_local and file_upload args. We'll add in a new
    // full file path and the fid instead.
    unset($run_args['file_local']);
    unset($run_args['file_upload']);
    unset($run_args['file_upload_existing']);
    unset($run_args['form_build_id']);
    unset($run_args['form_token']);
    unset($run_args['form_id']);
    unset($run_args['op']);
    unset($run_args['button']);

    $file_local = NULL;
    $file_upload = NULL;
    $file_remote = NULL;
    $file_existing = NULL;

    // Get the form values for the file.
    if (array_key_exists('file_local', $importer_def) and $importer_def['file_local'] == TRUE) {
      $file_local = trim($form_values['file_local']);
    }
    if (array_key_exists('file_upload', $importer_def) and $importer_def['file_upload'] == TRUE) {
      $file_upload = trim($form_values['file_upload']);
      if (array_key_exists('file_upload_existing', $form_values) and $form_values['file_upload_existing']) {
        $file_existing = trim($form_values['file_upload_existing']);
      }
    }
    if (array_key_exists('file_remote', $importer_def) and $importer_def['file_remote'] == TRUE) {
      $file_remote = trim($form_values['file_remote']);
    }

    // Sumbit a job for this loader.
    $fname = '';
    $fid = NULL;
    $file_details = [];
    if ($file_existing) {
        $file_details['fid'] = $file_existing;
    }
    elseif ($file_local) {
      $fname = preg_replace("/.*\/(.*)/", "$1", $file_local);
      $file_details['file_local'] = $file_local;
    }
    elseif ($file_upload) {
      $file_details['fid'] = $file_upload;
    }
    elseif ($file_remote) {
      $file_details['file_remote'] = $file_remote;
    }
    try {
      // Now allow the loader to do its own submit if needed.
      $importer->formSubmit($form, $form_state);
      // If the formSubmit made changes to the $form_state we need to update the
      // $run_args info.
      if ($run_args !== $form_values) {
        $run_args = $form_values;
      }

      // If the importer wants to rebuild the form for some reason then let's
      // not add a job.
      if ($form_state->isRebuilding() == TRUE) {
        return;
      }

      $importer->create($run_args, $file_details);
      $importer->submitJob();

    }
    catch (\Exception $e) {
        \Drupal::messenger()->addMessage('Cannot submit import: ' . $e->getMessage(), 'error');
    }
  }

    /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {

    // Convert the validation code into the D8/9 equivalent
    $form_values = $form_state->getValues();
    $plugin_id = $form_values['importer_plugin_id'];
    $importer_manager = \Drupal::service('tripal.importer');
    $importer = $importer_manager->createInstance($plugin_id);
    $importer_def = $importer_manager->getDefinitions()[$plugin_id];

    $file_local = NULL;
    $file_upload = NULL;
    $file_remote = NULL;
    $file_existing = NULL;

    // Get the form values for the file.
    if (array_key_exists('file_local', $importer_def) and $importer_def['file_local'] == TRUE) {
      $file_local = trim($form_values['file_local']);
      // If the file is local make sure it exists on the local filesystem.
      if ($file_local) {
        // check to see if the file is located local to Drupal
        $file_local = trim($file_local);
        $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $file_local;
        if (!file_exists($dfile)) {
          // if not local to Drupal, the file must be someplace else, just use
          // the full path provided
          $dfile = $file_local;
        }
        if (!file_exists($dfile)) {
          // form_set_error('file_local', t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
          $form_state->setErrorByName('file_local', t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
        }
      }
    }
    if (array_key_exists('file_upload', $importer_def) and $importer_def['file_upload'] == TRUE) {
      $file_upload = trim($form_values['file_upload']);
      if (array_key_exists('file_upload_existing', $form_values) and $form_values['file_upload_existing']) {
        $file_existing = $form_values['file_upload_existing'];
      }
    }
    if (array_key_exists('file_remote', $importer_def) and $importer_def['file_remote'] == TRUE) {
      $file_remote = trim($form_values['file_remote']);
    }

    // The user must provide at least an uploaded file or a local file path.
    if ($importer_def['file_required'] == TRUE and !$file_upload and !$file_local and !$file_remote and !$file_existing) {
      $form_state->setErrorByName('file_local', t("You must provide a file."));
    }

    // Now allow the loader to do validation of it's form additions.
    $importer->formValidate($form, $form_state);
  }
}
