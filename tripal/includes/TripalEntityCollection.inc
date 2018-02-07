<?php

class TripalEntityCollection {

  /**
   * The name of the bundle (i.e. content type) to which the entities belong.
   */
  protected $bundle_name = '';

  /**
   * The collection ID
   */
  protected $collection_id = NULL;

  /**
   * The name of this collection.
   */
  protected $collection_name = '';
  /**
   * An array of numeric entities IDs.
   */
  protected $ids = array();
  /**
   * An array of field IDs.
   */
  protected $fields = array();
  /**
   * The user object of the user that owns the collection.
   */
  protected $user = array();
  /**
   * The date that the collection was created.
   */
  protected $create_date = '';

  /**
   * The list of downloaders available for this bundle.
   */
  protected $downloaders = array();

  /**
   * The description for this collection.
   */
  protected $description = '';

  /**
   * Constructs a new instance of the TripalEntityCollection class.
   */
  public function __construct() {

  }

  /**
   * Deletes the current collection
   */
  public function delete() {

    if (!$this->collection_id) {
      throw new Exception('This data collection object has not yet been loaded. Cannot delete.');
    }

    try {
      db_delete('tripal_collection')
        ->condition('collection_id', $this->collection_id)
        ->execute();

      // Remove any files that may have been created
      foreach ($this->downloaders as $class_name => $label) {
        tripal_load_include_downloader_class($class_name);
        $outfile = $this->getOutfile($class_name);
        $downloader = new $class_name($this->bundle_name, $this->ids, $this->fields,
            $outfile, $this->getUserID());
        $downloader->delete();
      }

      // Reset the class to defaults.
      $this->collection_id = NULL;
      $this->bundle_name = '';
      $this->collection_name = '';
      $this->create_date = '';
      $this->description = '';
      $this->fields = array();
      $this->ids = array();

    }
    catch (Exception $e) {
      throw new Exception('Cannot delete collection: ' .  $e->getMessage());
    }
  }

  /**
   * Loads an existing collection using a collection ID.
   *
   * @param $collection_id
   *   The ID of the collection to load.
   *
   * @throws Exception
   */
  public function load($collection_id) {

    // Make sure we have a numeric job_id.
    if (!$collection_id or !is_numeric($collection_id)) {
      throw new Exception("You must provide the collection_id to load the collection.");
    }

    $collection = db_select('tripal_collection', 'tc')
      ->fields('tc')
      ->condition('collection_id', $collection_id)
      ->execute()
      ->fetchObject();

    if (!$collection) {
      throw new Exception("Cannot find a collection with the ID provided.");
    }

    // Fix the date/time fields.
    $this->bundle_name = $collection->bundle_name;
    $this->collection_name = $collection->collection_name;
    $this->create_date = $collection->create_date;
    $this->user = user_load($collection->uid);
    $this->ids = unserialize($collection->ids);
    $this->fields = unserialize($collection->fields);
    $this->description = $collection->description;
    $this->collection_id = $collection->collection_id;

    // Iterate through the fields and find out what download formats are
    // supported for this basket.
    foreach ($this->fields as $field_id) {
      $field = field_info_field_by_id($field_id);
      if (!$field) {
        continue;
      }
      $field_name = $field['field_name'];
      $field_type = $field['type'];
      $field_module = $field['module'];
      $instance = field_info_instance('TripalEntity', $field_name, $this->bundle_name);
      $downloaders = array();

      // All fields should support the Tab and CSV downloaders.
      tripal_load_include_downloader_class('TripalTabDownloader');
      $this->downloaders['TripalTabDownloader'] = TripalTabDownloader::$label;
      tripal_load_include_downloader_class('TripalCSVDownloader');
      $this->downloaders['TripalCSVDownloader'] = TripalCSVDownloader::$label;

      if (tripal_load_include_field_class($field_type)) {
        $settings = $field_type::$default_instance_settings;
        if (array_key_exists('download_formatters', $settings)) {
          foreach ($settings['download_formatters'] as $class_name) {
            if (!array_key_exists($class_name, $this->downloaders)) {
              tripal_load_include_downloader_class($class_name);
              $this->downloaders[$class_name] = $class_name::$label;
            }
          }
        }
      }
    }
  }

  /**
   * Creates a new collection.
   *
   * @param  $details
   *   An association array containing the details for a collection. The
   *   details must include the following key/value pairs:
   *   - uid:  The ID of the user that owns the collection
   *   - collection_name:  The name of the collection
   *   - bundle_name:  The name of the TripalEntity content type.
   *   - ids:  An array of the entity IDs that form the collection.
   *   - fields: An array of the field IDs that the collection is limited to.
   *   - description:  A user supplied description for the collection.
   *
   * @throws Exception
   */
  public function create($details) {
    if (!$details['uid']) {
      throw new Exception("Must provide a 'uid' key to TripalEntityCollection::create().");
    }
    if (!$details['collection_name']) {
      throw new Exception("Must provide a 'collection_name' key to TripalEntityCollection::create().");
    }
    if (!$details['bundle_name']) {
      throw new Exception("Must provide a 'bundle_name' key to TripalEntityCollection::create().");
    }
    if (!$details['ids']) {
      throw new Exception("Must provide a 'ids' key to TripalEntityCollection::create().");
    }
    if (!$details['fields']) {
      throw new Exception("Must provide a 'fields' key to TripalEntityCollection::create().");
    }

    if (!is_array($details['ids'])) {
      throw new Exception("The 'ids' key must be an array key to TripalEntityCollection::create().");
    }
    if (!is_array($details['fields'])) {
      throw new Exception("The 'ids' key must be an array key to TripalEntityCollection::create().");
    }

    // Before inserting the new collection make sure we don't violote the unique
    // constraint that a user can only have one collection of the give name.
    $has_match = db_select('tripal_collection', 'tc')
      ->fields('tc', array('collection_id'))
      ->condition('uid', $details['uid'])
      ->condition('collection_name', $details['collection_name'])
      ->execute()
      ->fetchField();
    if ($has_match) {
      throw new Exception('Cannot create the collection. One with this name already exists');
    }

    try {
      $collection_id = db_insert('tripal_collection')
        ->fields(array(
          'collection_name' => $details['collection_name'],
          'bundle_name' => $details['bundle_name'],
          'ids' => serialize($details['ids']),
          'fields' => serialize($details['fields']),
          'create_date' => time(),
          'uid' => $details['uid'],
          'description' => array_key_exists('description', $details) ? $details['description'] : '',
        ))
        ->execute();
      // Now load the job into this object.
      $this->load($collection_id);
    }
    catch (Exception $e) {
      throw new Exception('Cannot create collection: ' .  $e->getMessage());
    }
  }

  /**
   * Retrieves the list of appropriate download formatters for the basket.
   *
   * @return
   *   An associative array where the key is the TripalFieldDownloader class
   *   name and the value is the human-readable lable for the formatter.
   */
  public function getDownloadFormatters() {
     return $this->downloaders;
  }

  /**
   * Retrieves the list of entity IDs.
   *
   * @return
   *   An array of numeric entity IDs.
   */
  public function getEntityIDs(){
    return $this->ids;
  }

  /**
   * Retrieves the list of fields in the basket.
   *
   * @return
   *   An array of numeric field IDs.
   */
  public function getFields() {
    return $this->fields;
  }

  /**
   * Retrieves the date that the basket was created.
   *
   * @param $formatted
   *   If TRUE then the date time will be formatted for human readability.
   * @return
   *   A UNIX time stamp string containing the date or a human-readable
   *   string if $formatted = TRUE.
   */
  public function getCreateDate($formatted = TRUE) {
    if ($formatted) {
      return format_date($this->create_date);
    }
    return $this->create_date;
  }

  /**
   * Retrieves the name of the collection.
   *
   * @return
   *   A string containing the name of the collection.
   */
  public function getName() {
    return $this->collection_name;
  }

  /**
   * Retrieves the collection ID.
   *
   * @return
   *   A numeric ID for this collection.
   */
  public function getCollectionID(){
    return $this->collection_id;
  }

  /**
   * Retrieves the collection description
   *
   * @return
   *   A string containing the description of the collection.
   */
  public function getDescription() {
    return $this->description;
  }

  /**
   * Retrieves the user object of the user that owns the collection
   *
   * @return
   *   A Drupal user object.
   */
  public function getUser() {
    return $this->user;
  }

  /**
   * Retrieves the ID of the user that owns the collection
   *
   * @return
   *   The numeric User ID.
   */
  public function getUserID() {
    if ($this->user) {
      return $this->user->uid;
    }
    return NULL;
  }

  /**
   * Retrieves the output filename for the desired formatter.
   *
   * @param $formatter
   *   The class name of the formatter to use.  The formatter must
   *   be compatible with the data collection.
   *
   * @throws Exception
   */
  public function getOutfile($formatter) {
    if(!$this->isFormatterCompatible($formatter)) {
      throw new Exception(t('The formatter, "%formatter", is not compatible with this data collection.', array('%formatter' => $formatter)));
    }

    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', array('@formatter', $formatter)));
    }

    $extension = $formatter::$default_extension;
    $create_date = $this->getCreateDate(FALSE);
    $outfile = preg_replace('/[^\w]/', '_', ucwords($this->collection_name)) . '_collection' . '_' . $create_date . '.' . $extension;

    return $outfile;
  }

  /**
   * Indicates if the given formatter is compatible with the data collection.
   *
   * @param $formatter
   *   The class name of the formatter to check.
   * @return boolean
   *   TRUE if the formatter is compatible, FALSE otherwise.
   */
  public function isFormatterCompatible($formatter) {
    foreach ($this->downloaders as $class_name => $label) {
      if ($class_name == $formatter) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   * Writes the collection to all file downloadable formats.
   *
   * @throws Exception
   */
  public function writeAll() {
    foreach ($this->downloaders as $class_name => $lable) {
      $this->write($class_name);
    }
  }

  /**
   * Retrieves the URL for the downloadable file.
   *
   * @param $formatter
   *   The name of the class
   */
  public function getOutfileURL($formatter) {
    $outfile = $this->getOutfilePath($formatter);
    return file_create_url($outfile);
  }

  /**
   * Retrieves the path for the downloadable file.
   *
   * The path is in the Drupal URI format.
   *
   * @param $formatter
   *   The name of the class
   */
  public function getOutfilePath($formatter) {
    if(!$this->isFormatterCompatible($formatter)) {
      throw new Exception(t('The formatter, "@formatter", is not compatible with this data collection.', array('@formatter' => $formatter)));

    }

    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', array('@formatter', $formatter)));
    }

    $outfile = $this->getOutfile($formatter);

    $downloader = new $formatter($this->bundle_name, $this->ids, $this->fields, $outfile, $this->user->uid);

    return $downloader->getURL();
  }

  /**
   * Writes the collection to a file.
   *
   * @param formatter
   *   The name of the formatter class to use (e.g. TripalTabDownloader). The
   *   formatter must be compatible with the data collection.
   *
   * @throws Exception
   */
  public function write($formatter) {

    if(!$this->isFormatterCompatible($formatter)) {
      throw new Exception(t('The formatter, "@formatter", is not compatible with this data collection.', array('@formatter' => $formatter)));

    }

    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', array('@formatter', $formatter)));
    }

    $outfile = $this->getOutfile($formatter);

    // Filter out fields that aren't supported by the formatter.
    $supported_fields = array();
    foreach ($this->fields as $field_id) {
      // If the formatter is TripalTabDownloader or TripalCSVDownloader then
      // we always want to support the field.
      if ($formatter == 'TripalTabDownloader' or $formatter == 'TripalCSVDownloader') {
        if (!in_array($field_id, $supported_fields)) {
          $supported_fields[] = $field_id;
        }
        continue;
      }

      // Otherwise, find out if the formatter specified is supporte by the
      // field and if so then add it to our list of supported fields.
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $field_type = $field['type'];
      if (tripal_load_include_field_class($field_type)) {
        $settings = $field_type::$default_instance_settings;
        if (array_key_exists('download_formatters', $settings)) {
          if (in_array($formatter, $settings['download_formatters'])) {
            $supported_fields[] = $field_id;
          }
        }
      }
    }

    $downloader = new $formatter($this->bundle_name, $this->ids, $supported_fields, $outfile, $this->user->uid);
    $downloader->write();

  }
}