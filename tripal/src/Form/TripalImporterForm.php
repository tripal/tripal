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

    if (array_key_exists('cardinality', $importer_def) and $importer_def['cardinality'] != 1) {
      \Drupal::messenger()->addError('Error in the definition of this importer. Tripal Importers'
        . ' currently only support cardinality of 1, see Tripal issue #1635');
    }

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
        '#weight' => -15,
      ];

      $form['file']['upload_description'] = [
        '#markup' => $importer->describeUploadFileFormat(),
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
        '#description' => 'Remember to click the "Upload File" button below to send ' .
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
dpm($form['file']['file_upload'], "CP31 form['file']['file_upload']");//@@@
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

    // We should only add a submit button if this importer uses a button.
    // Examples of importers who don't use this button are multi-page forms.
    if (array_key_exists('use_button', $importer_def) AND $importer_def['use_button'] !== FALSE) {

      // We will allow specific importers to disable this button based on the state of the form.
      // By default it is enabled.
      $disabled = FALSE;
      // Unless the annotation says it should be disabled by default..
      if (array_key_exists('submit_disabled', $importer_def) AND $importer_def['submit_disabled'] === TRUE) {
        $disabled = TRUE;
      }
      // But if they set the storage to indicate we should disable/enable it
      // then we will do whatever they say ;-).
      $storage = $form_state->getStorage();
      if (array_key_exists('disable_TripalImporter_submit', $storage)) {
        $disabled = $storage['disable_TripalImporter_submit'];
      }
      $form['button'] = [
        '#type' => 'submit',
        '#value' => $importer_def['button_text'],
        '#weight' => 10,
        '#disabled' => $disabled,
      ];
    }

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

    // Now allow the loader to do its own submit if needed.
    try {
      $importer->formSubmit($form, $form_state);
      // Ensure any modifications made by the importer are used.
      $form_values = $form_state->getValues();
      $run_args = $form_values;
    }
    catch (\Exception $e) {
        \Drupal::messenger()->addMessage('Cannot submit import: ' . $e->getMessage(), 'error');
    }

    // If the importer wants to rebuild the form for some reason then let's
    // not add a job.
    if ($form_state->isRebuilding() == TRUE) {
      return;
    }

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
      $importer->createImportJob($run_args, $file_details);
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
    $file_remote = NULL;
    $file_upload = NULL;
    $file_existing = NULL;

    // Determine which file source was specified.
    if (array_key_exists('file_local', $importer_def) and $importer_def['file_local'] == TRUE) {
      $file_local = trim($form_values['file_local']);
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

    // How many methods were specified for the source of the file?
    $n_methods = ($file_local?1:0) + ($file_remote?1:0) + (($file_upload or $file_existing)?1:0);
    // If a file is required, the user must provide at least one file source method.
    if (array_key_exists('file_required', $importer_def) and ($importer_def['file_required'] == TRUE) and ($n_methods == 0)) {
      $form_state->setErrorByName('file_local', t('You must provide a file location or upload a file.'));
    }
    // No more than one method can be specified.
    elseif ($n_methods > 1) {
      $field = $file_remote?'file_remote':'file_local';
      $form_state->setErrorByName($field, t('You have specified more than one source option for'
                                          . ' the file, only one may be used at a time.'));
    }
    // A single file source has been provided. If local or remote, check that it is valid.
    else {
      // If the file is local make sure it exists on the local filesystem.
      if ($file_local) {
        // check to see if the file is located local to Drupal
        $file_local = trim($file_local);
        $dfile = \Drupal::root() . '/' . $file_local;
        if (!file_exists($dfile)) {
          // if not local to Drupal, the file must be someplace else, just use
          // the full path provided
          $dfile = $file_local;
        }
        if (!file_exists($dfile)) {
          $form_state->setErrorByName('file_local', t('Cannot find the file on the system.'
            . ' Check that the file exists or that the web server has permission to read the file.'));
        }
      }
      elseif ($file_remote) {
        // Validate that the remote URI is of the correct format.
        if (filter_var($file_remote, FILTER_VALIDATE_URL) === false) {
          $form_state->setErrorByName('file_remote', t('The Remote Path provided is not a valid URI.'));
        }

        // Validate a correct format remote URI to make sure it can be
        // accessed, with successful HTTP response code 200.
        else {
          $headers = @get_headers($file_remote);
          if (($headers === false) or (!is_array($headers)) or (!strpos($headers[0], '200'))) {
            $form_state->setErrorByName('file_remote', t('The Remote Path provided cannot be accessed.'
              . ' Check that it is correct.'));
          }
        }
      }
    }

    // Now allow the loader to do validation of its form additions.
    $importer->formValidate($form, $form_state);
  }
}
