<?php

class TripalEntityCollection {

  /**
   * The name of the bundle (i.e. content type) to which the entities belong.
   */
  protected $bundle_name = '';
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
   * Constructs a new instance of the TripalEntityCollection class.
   */
  public function __construct() {

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

    // Iterate through the fields and find out what download formats are
    // supported for this basket.
    foreach ($this->fields as $field_id) {
      $field = field_info_field_by_id($field_id);
      $field_name = $field['field_name'];
      $field_type = $field['type'];
      $field_module = $field['module'];
      $instance = field_info_instance('TripalEntity', $field_name, $this->bundle_name);
      $settings = $instance['settings'];
      $downloaders = array();
      if (array_key_exists('download_formatters', $settings)) {
        foreach ($settings['download_formatters'] as $class_name) {
          if (!in_array($class_name, $settings['download_formatters'])) {
            tripal_load_include_downloader_class($class_name);
            $this->downloaders[$class_name] = $class_name::$label;
          }
          // For backwards compatibility for fields that don't have
          // the download_formatters we'll set tab-delimeted and CSV
          // downloaders.
          else {
            tripal_load_include_downloader_class('TripalTabDownloader');
            $this->downloaders[$class_name] = $class_name::$label;
            tripal_load_include_downloader_class('TripalCSVDownloader');
            $this->downloaders[$class_name] = $class_name::$label;
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
   *   An array of numeric enity IDs.
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
   * Retreives the name of the collection.
   *
   * @return
   *   A string containing the name of the collection.
   */
  public function getName() {
    return $this->collection_name;
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
   * Writes the collection to a file.
   *
   * @param formatter
   *   The name of the formatter class to use (e.g. TripalTabDownloader).
   */
  public function write($formatter) {

    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', array('@formatter', $formatter)));
    }

    $extension = $formatter::$default_extension;
    $create_date = $this->getCreateDate(FALSE);
    $outfile = preg_replace('/[^\w]/', '_', ucwords($this->collection_name)) . '_collection' . '_' . $create_date . '.' . $extension;

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
      $instance = field_info_instance('TripalEntity', $field_name, $this->bundle_name);
      if (array_key_exists('download_formatters', $instance['settings'])) {
        if (in_array($formatter, $instance['settings']['download_formatters'])) {
          $supported_fields[] = $field_id;
        }
      }
    }

    $downloader = new $formatter($this->bundle_name, $this->ids, $supported_fields, $outfile, $this->user->uid);
    $downloader->write();

  }
}