<?php


abstract class TripalFieldDownloader {

  /**
   * Sets the label shown to the user describing this formatter.
   */
  static public $label = 'Generic';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'txt';

  /**
   * The bundle name.
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
   *   An array of entity IDs.  The order of the IDs will be the order that
   *   output is generated.
   * @param $fields
   *   An array of numeric field IDs to use when constructing the download. If
   *   no fields are provided then all fields that are appropriate for the
   *   given type will be used.
   * @param $outfile_name
   *   The name of the output file to create. The name should not include
   *   a path.
   */
  public function __construct($bundle_name, $ids, $fields = array(),
      $outfile_name, $uid) {

    $user = user_load($uid);
    if (!$user) {
      throw new Exception(t("The provided user ID does not reference a real user: '@uid'.", array('@uid' => $uid)));
    }
    if (!$outfile_name) {
      throw new Exception("Please provide an outputfilename");
    }

    $this->bundle_name = $bundle_name;
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

    $this->outfile = $user_dir. '/' . $outfile_name;
  }

  /**
   * Retrieves the URL for the downloadable file.
   */
  public function getURL() {
     return $this->outfile;
  }

  /**
   * Removes the downloadable file.
   */
  public function delete() {
    $fid = db_select('file_managed', 'fm')
     ->fields('fm', array('fid'))
     ->condition('uri', $this->outfile)
     ->execute()
     ->fetchField();
     if ($fid) {
       $file = file_load($fid);
       file_usage_delete($file, 'tripal', 'data-collection');
       file_delete($file, TRUE);
     }
  }

  /**
   * Creates the downloadable file.
   */
  public function write() {
    global $user;

    $fh = fopen(drupal_realpath($this->outfile), "w");
    if (!$fh) {
      throw new Exception("Cannout open collection file: " . $this->outfile);
    }

    $headers = $this->getHeader();
    if ($headers) {
      foreach ($headers as $line) {
        fwrite($fh, $line . "\r\n");
      }
    }

    foreach ($this->entity_ids as $entity_id) {
      $result = tripal_load_entity('TripalEntity', array($entity_id), FALSE, $this->fields);
      reset($result);
      $entity = $result[$entity_id];
      $lines = $this->formatEntity($entity);
      foreach ($lines as $line) {
        fwrite($fh, $line . "\r\n");
      }
    }
    fclose($fh);

    $file = new stdClass();
    $file->uri = $this->outfile;
    $file->filename = basename($this->outfile);
    $file->filemime = file_get_mimetype($this->outfile);
    $file->uid = $user->uid;
    $file->status = FILE_STATUS_PERMANENT;
    $file = file_save($file);
    $fid = $file->fid;
    $file = file_load($fid);
    // We use the fid for the last argument because these files
    // aren't really associated with any entity, but we need a value.
    file_usage_add($file, 'tripal', 'data-collection', $fid);
  }

  /**
   * Setups a download stream for the file.
   */
  public function download() {

  }

  /**
   * Formats the entity and the specified fields for output.
   *
   * This function should be implemented by a child class. It should iterate
   * over the fields for the entity and return the appropriate format. It may
   * return multiple lines of output if necessary.
   *
   * @param $entity
   *   The entity object.  The fields that should be formatted are already
   *   loaded.
   *
   * @return
   *   An array of strings (one per line of output.
   */
  abstract protected function formatEntity($entity);

  /**
   *  Retrieves header lines
   *
   *  This function should be implemented by a child class.  It should return
   *  the header lines for an output file.
   */
  abstract protected function getHeader();
}