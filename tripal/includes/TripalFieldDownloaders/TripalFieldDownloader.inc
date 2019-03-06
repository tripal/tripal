<?php


abstract class TripalFieldDownloader {

  /**
   * Sets the label shown to the user describing this formatter.  It
   * should be a short identifier. Use the $full_label for a more
   * descriptive label.
   */
  static public $label = 'Generic';

  /**
   * A more verbose label that better describes the formatter.
   */
  static public $full_label = 'Generic File format';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'txt';

  /**
   * The data collection assigned to this downloader.
   */
  protected $collection = NULL;

  /**
   * The collection ID
   */
  protected $collection_id = NULL;

  /**
   * An array of collection_bundle records for the content types that
   * belong to this data collection.
   */
  protected $collection_bundles = NULL;

  /**
   * The output file URI.
   */
  protected $outfile = '';

  /**
   * An array of printable fields.  Because fields can come from multiple
   * bundles and those bundles can be from multiple sites, it is possible that
   * 1) two bundles use the same field and we want to conslidate to a
   * single printable field; and 2) that a remote site may use the same term
   * for a field as a bundle on the local site.  The only way to sort out this
   * mess is to use the term accession numbers.  Therefore, the array contains
   * a unique list of printable fields using their accession numbers as keys
   * and a field label as the value.
   *
   */
  protected $printable_fields = [];

  /**
   * The remote site json data returned for the entity
   */
  protected $remote_entity = '';

  /**
   * An array that associates a field ID with a term.
   *
   * The first-level key is the site ID. For the local site this will be
   * the word 'local' for all others it will be the numeric id.  The second
   * level key is the bundle bundle name.  For local bundles this will
   * always be bio_data_xxxx.  Third, are two subkeys: by_field and
   * by_accession.  To lookup a field's term you use the 'by_field' subkey
   * with the field_id as the next level.  To lookup the field ID for a term
   * use the 'by_accession' subkey with the accession as the next level.  Below
   * is an example of the structure of this array.
   *
   * @code
   * Array (
   * [local] => Array(
   * [bio_data_7] => Array(
   * [by_field] => Array(
   * [56] => data:2091,
   * [57] => OBI:0100026,
   * [17] => schema:name,
   * [58] => data:2044,
   * [34] => data:0842,
   * [67] => schema:alternateName,
   * ),
   * [by_accession] => Array (
   * [data:2091] => 56,
   * [OBI:0100026] => 57,
   * [schema:name] => 17,
   * [data:2044] => 58,
   * [data:0842] => 34,
   * [schema:alternateName] => 67,
   * ),
   * ),
   * ),
   * )
   * @endcode
   */
  protected $fields2terms = [];


  /**
   * A list of field and instance items, indexed first by site_id with 'local'
   * being the key for local fields and the numeric site_id for remote
   * fields.  The second-levle key is the bundle_name and the the field_id.
   * Below the field_id are the keys 'field' or 'instance' where the
   * value of each is the field or instance details respectively.
   */
  protected $fields = [];


  /**
   * The file handle for an opeend file using during writing.
   */
  protected $fh;

  /**
   * Constructs a new instance of the TripalFieldDownloader class.
   *
   * @param $collection_id
   *   The ID for the collection.
   * @param $outfile_name
   *   The name of the output file to create. The name should not include
   *   a path.
   */
  public function __construct($collection_id, $outfile_name) {

    if (!$outfile_name) {
      throw new Exception("Please provide an outputfilename");
    }

    // Get the collection record and store it.
    $collection = db_select('tripal_collection', 'tc')
      ->fields('tc')
      ->condition('collection_id', $collection_id, '=')
      ->execute()
      ->fetchObject();

    if (!$collection) {
      throw new Exception(t("Cannot find a collection that matches the provided id: !id", ['!id' => $collection_id]));
    }
    $this->collection = $collection;

    // Make sure the user directory exists
    $user = user_load($this->collection->uid);
    $user_dir = tripal_get_user_files_dir($user);
    if (!tripal_is_user_files_dir_writeable($user)) {
      throw new Exception(t("The user's data directory is not writeable: !user_dir.", ['!user_dir' => $user_dir]));
    }

    // Set the collection ID of the collection that this downloader will use.
    $this->collection_id = $collection_id;

    // Make sure the data_collections directory exists.
    $collections_dir = $user_dir . '/data_collections';
    if (!file_exists($collections_dir)) {
      if (!file_prepare_directory($collections_dir, [FILE_CREATE_DIRECTORY])) {
        throw new Exception(t("The data_collection directory cannot be created: !collections_dir.", ['!collections_dir' => $collections_dir]));
      }
    }
    $this->outfile = $collections_dir . '/' . $outfile_name;

    // A data collection may have multiple bundles.  We'll need to get
    // them all and store them.
    $collection_bundles = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle')
      ->condition('collection_id', $collection_id, '=')
      ->execute();
    while ($collection_bundle = $collection_bundles->fetchObject()) {
      $collection_bundle->ids = unserialize($collection_bundle->ids);
      $collection_bundle->fields = unserialize($collection_bundle->fields);
      $this->collection_bundles[] = $collection_bundle;
    }

    // Map the fields to their term accessions.
    $this->setFields();
    $this->setFields2Terms();
    $this->setPrintableFields();
  }

  /**
   * Inidcates if a given field is supported by this Downloader class.
   *
   * @param $field
   *   A field info array.
   */
  public function isFieldSupported($field, $instance) {
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $this_formatter = get_class($this);

    // If a field is a TripalField then check its supported downloaders.
    if (tripal_load_include_field_class($field_type)) {
      $formatters = $field_type::$download_formatters;
      if (in_array($this_formatter, $formatters)) {
        return TRUE;
      }
    }

    // If this is a remote field then check that differently.
    if ($field['storage']['type'] == 'tripal_remote_field') {
      if (in_array($this_formatter, $instance['formatters'])) {
        return TRUE;
      }
    }
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
      ->fields('fm', ['fid'])
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
   *
   * @param TripalJob $job
   */
  public function writeInit(TripalJob $job = NULL) {

    $user = user_load($this->collection->uid);

    $this->fh = fopen(drupal_realpath($this->outfile), "w");
    if (!$this->fh) {
      throw new Exception("Cannout open collection file: " . $this->outfile);
    }

    // Add the headers to the file.
    $headers = $this->getHeader();
    if ($headers) {
      foreach ($headers as $line) {
        fwrite($this->fh, $line . "\r\n");
      }
    }
  }

  /**
   * Write a single entity to the file.
   *
   * Before calling this function call the initWrite() function to
   * establish the file and write headers.
   *
   * @param $entity
   *   The Entity to write.
   * @param TripalJob $job
   */
  public function writeEntity($entity, TripalJob $job = NULL) {
    $lines = $this->formatEntity($entity);
    foreach ($lines as $line) {
      fwrite($this->fh, $line . "\r\n");
    }
  }

  /**
   * Closes the output file once writing of all entities is completed.
   *
   * @param TripalJob $job
   */
  public function writeDone(TripalJob $job = NULL) {
    fclose($this->fh);

    $user = user_load($this->collection->uid);

    $file = new stdClass();
    $file->uri = $this->outfile;
    $file->filename = basename($this->outfile);
    $file->filemime = file_get_mimetype($this->outfile);
    $file->uid = $user->uid;
    $file->status = FILE_STATUS_PERMANENT;

    // Check if this file already exists. If it does then just update
    // the stats.
    $fid = db_select('file_managed', 'fm')
      ->fields('fm', ['fid'])
      ->condition('uri', $this->outfile)
      ->execute()
      ->fetchField();
    if ($fid) {
      $file->fid = $fid;
      $file = file_save($file);
    }
    else {
      $file = file_save($file);
      $fid = $file->fid;
      $file = file_load($fid);
    }

    // Associate this file with data collections if it isn't already
    $usage = file_usage_list($file);
    if (!array_key_exists('tripal', $usage) or
      !array_key_exists('data_collection', $usage['tripal'])) {
      file_usage_add($file, 'tripal', 'data_collection', $this->collection_id);
    }
  }


  /**
   * Creates the downloadable file.
   *
   * @param $job
   *    If this function is run as a Tripal Job then this argument can be
   *    set to the Tripaljob object for keeping track of progress.
   */
  public function write(TripalJob $job = NULL) {

    $this->initWrite($job);

    $num_handled = 0;
    foreach ($this->collection_bundles as $bundle_collection) {
      $collection_bundle_id = $bundle_collection->collection_bundle_id;
      $bundle_name = $bundle_collection->bundle_name;
      $entity_ids = $bundle_collection->ids;
      $fields = $bundle_collection->fields;
      $site_id = $bundle_collection->site_id;

      foreach ($entity_ids as $entity_id) {
        $num_handled++;
        if ($job) {
          $job->setItemsHandled($num_handled);
        }

        // if we have a site_id then we need to get the entity from the
        // remote service. Otherwise create the entity from the local system.
        if ($site_id) {
          $entity = $this->loadRemoteEntity($entity_id, $site_id, $bundle_name);
          if (!$entity) {
            continue;
          }
        }
        else {
          $result = tripal_load_entity('TripalEntity', [$entity_id], FALSE, $fields, FALSE);
          $entity = $result[$entity_id];
        }

        if (!$entity) {
          continue;
        }

        $lines = $this->formatEntity($entity);
        foreach ($lines as $line) {
          fwrite($this->fh, $line . "\r\n");
        }
      }
    }

    $this->finishWrite($job);
  }

  /**
   * Setups a download stream for the file.
   */
  public function download() {

  }


  /**
   * A helper function for the setFields() function.
   *
   * Adds local fields to the list of fields.
   */
  private function setLocalFields() {
    foreach ($this->collection_bundles as $collection_bundle) {
      $bundle_name = $collection_bundle->bundle_name;
      if ($collection_bundle->site_id) {
        continue;
      }
      foreach ($collection_bundle->fields as $field_id) {
        $field = field_info_field_by_id($field_id);
        $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
        $this->fields['local'][$bundle_name][$field_id]['field'] = $field;
        $this->fields['local'][$bundle_name][$field_id]['instance'] = $instance;
      }
    }
  }

  /**
   * A helper function for the setFields() function.
   *
   * Adds remote fields to the list of fields.
   */
  private function setRemoteFields() {
    // We can't use the Tripal ws API extensions if the
    // tripal_ws module is not enabled.
    if (!module_exists('tripal_ws')) {
      return;
    }

    foreach ($this->collection_bundles as $collection_bundle) {
      $bundle_name = $collection_bundle->bundle_name;
      $site_id = $collection_bundle->site_id;
      // Skip local fields.
      if (!$site_id) {
        continue;
      }

      // Iterate through the fields of this collection and get the
      // info for each one from the class.  We will create "fake" field and
      // instance info arrays.
      foreach ($collection_bundle->fields as $field_id) {
        $field = tripal_get_remote_field_info($site_id, $bundle_name, $field_id);
        $instance = tripal_get_remote_field_instance_info($site_id, $bundle_name, $field_id);
        $this->fields[$site_id][$bundle_name][$field_id]['field'] = $field;
        $this->fields[$site_id][$bundle_name][$field_id]['instance'] = $instance;
      }
    }
  }

  /**
   * Sets the fields array
   */
  protected function setFields() {
    $this->setLocalFields();
    $this->setRemoteFields();
  }

  /**
   * Sets the fields2term array.
   *
   * The fields2term array provides an easy lookup for mapping a term
   * to it's accession number.
   **/
  protected function setFields2Terms() {

    foreach ($this->fields as $site => $bundles) {
      foreach ($bundles as $bundle_name => $bundle_fields) {
        foreach ($bundle_fields as $field_id => $info) {
          $instance = $info['instance'];
          $accession = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];
          $this->fields2terms[$site][$bundle_name]['by_field'][$field_id] = $accession;
          $this->fields2terms[$site][$bundle_name]['by_accession'][$accession] = $field_id;
        }
      }
    }
  }

  /**
   * Conslidates all the fields into a single list of accession numbers.
   *
   * The array of printable fields will contain an array containing the
   * accession number and the label.  The title used is from the first
   * occurance of an accession.
   */
  protected function setPrintableFields() {

    foreach ($this->fields as $site => $bundles) {
      foreach ($bundles as $bundle_name => $bundle_fields) {
        foreach ($bundle_fields as $field_id => $info) {
          $field = $info['field'];
          $instance = $info['instance'];
          $accession = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];
          if (!array_key_exists($accession, $this->printable_fields)) {
            // Only include fields that support this downloader type in
            // or list of printable fields.
            if ($this->isFieldSupported($field, $instance)) {
              $this->printable_fields[$accession] = $instance['label'];
            }
          }
        }
      }
    }
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