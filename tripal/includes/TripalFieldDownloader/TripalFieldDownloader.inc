<?php


class TripalFieldDownloader {

  /**
   * The bundle name.
   * @var string
   */
  protected $bundle_name = '';

  /**
   * A set of entity IDs. The entities must all be of the same bundle type.
   */
  protected $entity_ids = array();

  /**
   * The set of fields
   */
  protected $fields = array();

  /**
   * The output file URI.
   */
  protected $outfile = '';

  /**
   * Constructs a new instance of the TripalFieldDownloader class.
   * @param $bundle_name
   *   The name of the bundle to which the IDs in the $id argument belong.
   * @param $ids
   *   An array of entity IDs
   * @param $fields
   *   An array of numeric field IDs to use when constructing the download. If
   *   no fields are provided then all fields that are appropriate for the
   *   given type will be used.
   * @param $outfile_name
   *   The name of the output file to create (minus any extension).
   * @param $extension
   *   The extension to add to the end of the output file.
   */
  public function __construct($bundle_name, $ids, $fields = array(),
      $outfile_name = '', $extension = 'txt') {
    global $user;

    $this->entity_ids = $ids;
    $this->fields = $fields;

    // Make sure the user directory exists
    $user_dir = 'public://tripal/users/' . $user->uid;
    if (!file_prepare_directory($user_dir, FILE_CREATE_DIRECTORY)) {
      $message = 'Could not access the directory on the server for storing this file.';
      watchdog('tripal', $message, array(), WATCHDOG_ERROR);
      drupal_json_output(array(
        'status'  => 'failed',
        'message' => $message,
        'file_id' => '',
      ));
      return;
    }

    if (!$outfile_name) {
      $outfile_name = unqiueid();
    }

    $this->outfile = $user_dir. '/' . $outfile_name . '.' . $outfile_ext;
  }

  /**
   * Retrieves the URL for the downloadable file.
   */
  public function getURL() {
     return $this0>outfile;
  }

  /**
   * Creates the download able file.
   */
  public function create() {
    $fh = fopen($this->outfile, "w");
    foreach ($this->entity_ids as $entity_id) {
      $entity = tripal_load_entity('TripalEntity', array($entity_id), FALSE, $this->fields);
      $content = $this->format($entity, $this->fields);
      fwrite($fh, $content);
    }
    fclose($fh);
  }

  /**
   * Setups a download stream for the file.
   */
  public function download() {

  }

  /**
   * Formats the output for a given entity.
   *
   * This function should be implemented by a child class. It should iterate
   * over the fields for the entity and return the appropriate format.
   *
   * @param $entity
   *   The entity object.  The fields that should be formatted are already
   *   loaded.
   * @param $fields
   *   A list of field names that should be formatted.
   *
   * @return
   *   A string containing the formatted output.
   */
  protected function format($entity, $fields) {

  }
}