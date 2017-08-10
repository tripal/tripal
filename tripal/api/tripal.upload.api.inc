<?php
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
 */
function hook_file_upload_check($action, $details, &$message){
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