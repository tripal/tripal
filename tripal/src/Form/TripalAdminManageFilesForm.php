<?php

namespace Drupal\tripal\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal;

class TripalAdminManageFilesForm implements FormInterface{

  use MessengerTrait;

  /**
   * Form ID.
   *
   * @return string
   */
  public function getFormId() {
    return 'tripal_admin_manage_files_form';
  }

  /**
   * Allow users to specify a max file size.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return array
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $settings = Drupal::config('tripal.settings');
    $default_max_size = $settings->get('files.max_size');

    $form['php_defaults'] = [
      '#type' => 'item',
      '#title' => 'PHP Maximum Upload Size',
      '#description' => t('Your php.ini file is currently configured with this size as the maximum size allowed for a single file during upload. However, Tripal uses an HTML5 uploader that supports much larger file sizes.  It works by breaking the file into chunks and uploading each chunk separately. Therefore this becomes the maximum allowed size of a chunk.'),
      '#markup' => ini_get("upload_max_filesize"),
    ];

    $form_upload_max = $form_state->has('upload_max') ? $form_state->getValue('upload_max') : $default_max_size;
    $upload_max = tripal_format_bytes($form_upload_max, $default_max_size);
    $form['upload_max'] = [
      '#type' => 'textfield',
      '#title' => 'Maximum file size',
      '#description' => t('Set the maximum size that a file can have for upload. The number must be followed by the suffix "MB" (megabytes) or "GB" (gigabytes) with no space between the number and the suffix (e.g.: 200MB).  No user will be allowed to upload a file larger than this when Tripal\'s file upload tool is used.'),
      '#default_value' => $upload_max,
    ];

    $form['update_defaults'] = [
      '#type' => 'submit',
      '#value' => t('Save'),
    ];

    return $form;
  }

  /**
   * Validate form.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $upload_max = $form_state->getValue('upload_max');
    // Validate the quota string.
    if (!preg_match("/^\d+(\.\d+)*(MB|GB|TB)$/", $upload_max)) {
      $form_state->setErrorByName('upload_max',
        t('Please provide a maximum size in the format indicated.'));
    }
  }

  /**
   * Save settings.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $upload_max = $form_state->getValue('upload_max');

    // if the 2nd element of the quota string occupied by a valid suffix we need to check to see
    // what we have to multiply the value by (1024 for GB 1024^2 for TB because
    // we assume that the initial number is already in MB)
    $matches = [];
    $multiplier = 'MB';
    $size = $upload_max;
    if (preg_match("/^(\d+(?:\.\d+)*)(MB|GB|TB)$/", $upload_max, $matches)) {
      $multiplier = $matches[2];
      $size = $matches[1];
    }
    switch ($multiplier) {
      case 'GB':
        $size = (int) ($size * pow(10, 9));
        break;
      case 'TB':
        $size = (int) ($size * pow(10, 12));
        break;
      default:
        $size = (int) ($size * pow(10, 6));
        break;
    }

    // Update configuration
    Drupal::configFactory()
      ->getEditable('tripal.settings')
      ->set('files.max_size', $size)
      ->save();

    $this->messenger()->addStatus('Default settings have been set.');
  }

}
