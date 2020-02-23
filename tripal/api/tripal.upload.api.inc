<?php
/**
 * @file
 * Provides an application programming interface (API) for working with
 * file uploads.
 */

/**
 * @defgroup tripal_upload_api File Upload
 * @ingroup tripal_api
 * @{
 * Tripal provides a convenient HTML5 Javascript uploader. It is automatically
 * embedded into the TripalImporter class.  This application programing
 * interface (API) provides support for working with uploaded files.
 *
 * If you want to use the TripalUploader JavaScript in your own form the
 * following must be performed:
 *
 * 1) Add a Drupal form to your code that contains the following:
 *   * A Drupal-style table with 4 or 8 columns.  See the initialize
 *     function in this class for a description of the columns.
 *   * A button for submitting a file for upload.
 *
 * @code
 * $headers = array(
 *    array('data' => 'Sequence File'),
 *    array('data' => 'Size', 'width' => '10%'),
 *    array('data' => 'Upload Progress', 'width' => '20%'),
 *    array('data' => 'Action', 'width' => '10%')
 *  );
 *  $rows = array();
 *  $table_vars = array(
 *    'header'      => $headers,
 *    'rows'        => $rows,
 *    'attributes'  => array('id' => 'sequence-file-upload-table'),
 *    'sticky'      => TRUE,
 *    'colgroups'   => array(),
 *    'empty'       => t('There are currently no files added.'),
 *  );
 *  $form['upload']['sequence_file'] = array(
 *    '#markup' => theme('table', $table_vars)
 *  );
 *  $form['upload']['sequence_fid'] = array(
 *    '#type' => 'hidden',
 *    '#value' => 0,
 *    '#attributes' => array('id' => 'sequence-fid')
 *  );
 *  $form['upload']['sequence_file_submit'] = array(
 *    '#type'     => 'submit',
 *    '#value'    => 'Upload Sequence File',
 *    '#name' => 'sequence_file_submit',
 *    // We don't want this button to submit as the file upload
 *    // is handled by the JavaScript code.
 *    '#attributes' => array('onclick' => 'return (false);')
 *  );
 * @endcode
 *
 *
 * 2)  Edit the theme/js/[module_name].js and in the "Drupal.behaviors.[module]"
 * section add a JQuery show function to the form that converts the table
 * created in the Drupal form to a TripalUploader table.  The 'table_id' must be
 * the same as the 'id' attribute set for the table in the Drupal code above.
 * The 'submit_id' must be the id of the upload button added in the Drupal
 * code above.  The 'category' for the files.  This is the category that
 * will be saved in Tripal for the file.  See the addUploadTable function
 * for additional options.  Include a 'cardinality' setting to indicate
 * the number of allowed files per upload, and set the 'target_id' to the
 * name of the field that will contain the file ID (fid) after uploading.
 *
 * @code
 *  // The TripalUploader object used for uploading of files using the
 *  // HTML5 File API. Large files are uploaded as chunks and a progress
 *  // bar is provided.
 *  var uploader = new TripalUploader();
 *
 *  $('#tripal-sequences-panel-form').show(function() {
 *    uploader.addUploadTable('sequence_file', {
 *      'table_id' : '#sequence-file-upload-table',
 *      'submit_id': '#edit-sequence-file-submit',
 *      'category' : ['sequence_file'],
 *      'cardinality' : 1,
 *      'target_id' : 'sequence-fid',
 *    });
 *  });
 * @endcode
 *
 *
 * 3) Files are uploaded automatically to Tripal.  Files are saved in the
 * Tripal user's directory.  You can retreive information about the
 * file by querying for the file category for the current project.
 *
 * @code
 *   $seq_files = TripalFeature::getFilesByTypes($user->uid,
 *                   array('sequence_file'), $project_id);
 * @endcode
 *
 * 4) If the 'target_id' was used in array for step #2 above, then the
 * file ID can be retrieved in the hook_validate() and hook_submit() functions
 * via the $form_state['input'] array (not the $form_state['values'] array.
 *
 * @}
 */

/**
 * Allows a module to interact with the Tripal file uploader during upload.
 *
 * This function is called prior to an 'action' aoccuring and allows the
 * module that is responsible for the file upload to halt an upload if
 * needed.
 *
 * @param $action
 *   The current action that is being peformed during the upload process. The
 *   actions are:  'save', 'check' and 'merge'.
 * @param $details
 *   An associative array containing details about the upload process. Valid
 *   keys include:
 *     - filename:  The name of the file.
 *     - file_size:  The total size of the file.
 *     - chunk_size:  The size of the chunk.
 *     - fid:  The file ID of the file.
 * @param $message
 *   An error message to report to the user if the function returns FALSE.
 *
 * @return
 *   TRUE if the upload should continue. FALSE if a problem occurs and the
 *   upload should be terminated.
 *
 * @ingroup tripal_upload_api
 */
function hook_file_upload_check($action, $details, &$message) {
  switch ($action) {
    case 'save':
      // Place code here when chunks are being saved.
      break;
    case 'check':
      // Place code here when a chunck is being checked if the upload
      // completed successfully.
      break;
    case 'merge':
      // Place code here when all chunks will be merged.
      break;
  }
  return TRUE;
}
