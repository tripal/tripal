<?php

class TripalEntityCollection {

  /**
   * The name of the bundles (i.e. content type) to which the entities belong.
   */
  protected $bundles = [];

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
  protected $ids = [];

  /**
   * An array of field IDs.
   */
  protected $fields = [];

  /**
   * The user object of the user that owns the collection.
   */
  protected $user = [];

  /**
   * The date that the collection was created.
   */
  protected $create_date = '';

  /**
   * The list of downloaders available for this bundle.
   */
  protected $formatters = [];

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
      // Remove any files that may have been created
      foreach ($this->formatters as $class_name => $label) {
        tripal_load_include_downloader_class($class_name);
        $outfile = $this->getOutfile($class_name);
        $downloader = new $class_name($this->collection_id, $outfile);
        $downloader->delete();
      }

      // Delete from the tripal collection table.
      db_delete('tripal_collection')
        ->condition('collection_id', $this->collection_id)
        ->execute();

      // Delete the field groups from the tripal_bundle_collection table.
      db_delete('tripal_collection_bundle')
        ->condition('collection_id', $this->collection_id)
        ->execute();

      // Reset the class to defaults.
      $this->collection_id = NULL;
      $this->collection_name = '';
      $this->create_date = '';
      $this->description = '';

    } catch (Exception $e) {
      throw new Exception('Cannot delete collection: ' . $e->getMessage());
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
    $this->collection_name = $collection->collection_name;
    $this->create_date = $collection->create_date;
    $this->user = user_load($collection->uid);
    $this->description = $collection->description;
    $this->collection_id = $collection->collection_id;

    // Now get the bundles in this collection.
    $bundles = db_select('tripal_collection_bundle', 'tcb')
      ->fields('tcb')
      ->condition('collection_id', $collection->collection_id)
      ->execute();

    // If more than one bundle plop into associative array.
    while ($bundle = $bundles->fetchObject()) {
      $bundle_name = $bundle->bundle_name;
      $this->bundles[$bundle_name] = $bundle;
      $this->ids[$bundle_name] = unserialize($bundle->ids);
      $this->fields[$bundle_name] = unserialize($bundle->fields);
    }

    // Iterate through the fields and find out what download formats are
    // supported for this basket.
    $this->formatters = $this->setFormatters();
  }

  /**
   * Creates a new data collection.
   *
   * To add bundles with entities and fields to a collection, use the
   * addBundle() function after the collection is created.
   *
   * @param  $details
   *   An association array containing the details for a collection. The
   *   details must include the following key/value pairs:
   *   - uid:  The ID of the user that owns the collection
   *   - collection_name:  The name of the collection
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


    // Before inserting the new collection make sure we don't violate the unique
    // constraint that a user can only have one collection of the give name.
    $has_match = db_select('tripal_collection', 'tc')
      ->fields('tc', ['collection_id'])
      ->condition('uid', $details['uid'])
      ->condition('collection_name', $details['collection_name'])
      ->execute()
      ->fetchField();
    if ($has_match) {
      throw new Exception('Cannot create the collection. One with this name already exists');
    }

    try {
      $collection_id = db_insert('tripal_collection')
        ->fields([
          'collection_name' => $details['collection_name'],
          'create_date' => time(),
          'uid' => $details['uid'],
          'description' => array_key_exists('description', $details) ? $details['description'] : '',
        ])
        ->execute();

      // Now load the job into this object.
      $this->load($collection_id);
    } catch (Exception $e) {
      throw new Exception('Cannot create collection: ' . $e->getMessage());
    }
  }

  /**
   * Creates a new tripal_collection_bundle entry.
   *
   * @param  $details
   *   An association array containing the details for a collection. The
   *   details must include the following key/value pairs:
   *   - bundle_name:  The name of the TripalEntity content type.
   *   - ids:  An array of the entity IDs that form the collection.
   *   - fields: An array of the field IDs that the collection is limited to.
   *
   * @throws Exception
   */
  public function addBundle($details) {
    if (!$details['bundle_name']) {
      throw new Exception("Must provide a 'bundle_name' to TripalEntityCollection::addFields().");
    }
    if (!$details['ids']) {
      throw new Exception("Must provide a 'ids' to TripalEntityCollection::addFields().");
    }
    if (!$details['fields']) {
      throw new Exception("Must provide a 'fields' to TripalEntityCollection::addFields().");
    }

    try {
      $collection_bundle_id = db_insert('tripal_collection_bundle')
        ->fields([
          'bundle_name' => $details['bundle_name'],
          'ids' => serialize($details['ids']),
          'fields' => serialize($details['fields']),
          'collection_id' => $this->collection_id,
        ])
        ->execute();

      // Now load the job into this object.
      $this->load($this->collection_id);
    } catch (Exception $e) {
      throw new Exception('Cannot create collection: ' . $e->getMessage());
    }
  }

  /**
   * Retrieves the list of bundles associated with the collection.
   *
   * @return
   *   An array of bundles.
   */
  public function getBundles() {
    return $this->bundles;
  }

  /**
   * Retrieves the site id for this specific bundle fo the collection.
   *
   * @return
   *   A remote site ID, or an empty string if the bundle is local.
   */
  public function getBundleSiteId($bundle_name) {
    return $this->bundles[$bundle_name]->site_id;
  }

  /**
   * Retrieves the list of appropriate download formatters for the basket.
   *
   * @return
   *   An associative array where the key is the TripalFieldDownloader class
   *   name and the value is the human-readable label for the formatter.
   */
  private function setFormatters() {

    $downloaders = [];
    // Iterate through the fields and find out what download formats are
    // supported for this basket.
    foreach ($this->fields as $bundle_name => $field_ids) {

      // Need the site ID from the tripal_collection_bundle table.
      $site_id = $this->getBundleSiteId($bundle_name);

      foreach ($field_ids as $field_id) {
        // If this is a field from a remote site then get it's formatters
        if ($site_id and module_exists('tripal_ws')) {
          $formatters = tripal_get_remote_field_formatters($site_id, $bundle_name, $field_id);
          $this->formatters += $formatters;
        }
        else {
          $field_info = field_info_field_by_id($field_id);
          if (!$field_info) {
            continue;
          }
          $field_name = $field_info['field_name'];
          $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
          $formatters = tripal_get_field_field_formatters($field_info, $instance);
          $this->formatters += $formatters;
        }
      }
    }
    $this->formatters = array_unique($this->formatters);
    return $this->formatters;
  }

  /**
   * Retrieves the list of appropriate download formatters for the basket.
   *
   * @return
   *   An associative array where the key is the TripalFieldDownloader class
   *   name and the value is the human-readable label for the formatter.
   */
  public function getFormatters() {
    return $this->formatters;
  }

  /**
   * Retrieves the list of entity IDs.
   *
   * @return
   *   An array of numeric entity IDs.
   */
  public function getEntityIDs($bundle_name) {
    return $this->ids[$bundle_name];
  }

  /**
   * Retrieves the list of fields in the basket.
   *
   * @return
   *   An array of numeric field IDs.
   */
  public function getFieldIDs($bundle_name) {
    return $this->fields[$bundle_name];
  }

  /**
   * Retrieves the date that the basket was created.
   *
   * @param $formatted
   *   If TRUE then the date time will be formatted for human readability.
   *
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
  public function getCollectionID() {
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
    if (!$this->isFormatterCompatible($formatter)) {
      throw new Exception(t('The formatter, "%formatter", is not compatible with this data collection.', ['%formatter' => $formatter]));
    }

    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', [
        '@formatter',
        $formatter,
      ]));
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
   *
   * @return boolean
   *   TRUE if the formatter is compatible, FALSE otherwise.
   */
  protected function isFormatterCompatible($formatter) {
    foreach ($this->formatters as $class_name => $label) {
      if ($class_name == $formatter) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   * Retrieves the URL for the downloadable file.
   *
   * @param $formatter
   *   The name of the class
   */
  public function getOutfileURL($formatter) {
    $outfile = $this->getOutfilePath($formatter);
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
    if (!$this->isFormatterCompatible($formatter)) {
      throw new Exception(t('The formatter, "@formatter", is not compatible with this data collection.', ['@formatter' => $formatter]));
    }
    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', [
        '@formatter',
        $formatter,
      ]));
    }

    $outfile = $this->getOutfile($formatter);

    // Make sure the user directory exists
    $user_dir = 'public://tripal/users/' . $this->user->uid;
    $outfilePath = $user_dir . '/data_collections/' . $outfile;
    return $outfilePath;
  }

  /**
   * Writes the collection to a file using a given formatter.
   *
   * @param formatter
   *   The name of the formatter class to use (e.g. TripalTabDownloader). The
   *   formatter must be compatible with the data collection.  If no
   *   formatter is supplied then all file formats supported by this
   *   data collection will be created.
   * @param $job
   *    If this function is run as a Tripal Job then this argument can be
   *    set to the Tripaljob object for keeping track of progress.
   *
   * @throws Exception
   */
  public function write($formatter = NULL, TripalJob $job = NULL) {

    // Initialize the downloader classes and initialize the files for writing.
    $formatters = [];
    foreach ($this->formatters as $class => $label) {
      if (!$this->isFormatterCompatible($class)) {
        throw new Exception(t('The formatter, "@formatter", is not compatible with this data collection.', ['@formatter' => $formatter]));
      }
      if (!tripal_load_include_downloader_class($class)) {
        throw new Exception(t('Cannot find the formatter named "@formatter".', [
          '@formatter',
          $formatter,
        ]));
      }
      $outfile = $this->getOutfile($class);
      if (!$formatter or ($formatter == $class)) {
        $formatters[$class] = new $class($this->collection_id, $outfile);
        $formatters[$class]->writeInit($job);
        if ($job) {
          $job->logMessage("Writing " . lcfirst($class::$full_label) . " file.");
        }
      }
    }

    // Count the total number of entities
    $total_entities = 0;
    $bundle_collections = $this->collection_bundles;
    foreach ($this->bundles as $bundle) {
      $bundle_name = $bundle->bundle_name;
      $entity_ids = $this->getEntityIDs($bundle_name);
      $total_entities += count($entity_ids);
    }
    if ($job) {
      $job->setTotalItems($total_entities);
      $job->setInterval(1);
    }

    // Iterate through the bundles in this collection and get the entities.
    foreach ($this->bundles as $bundle) {
      $bundle_name = $bundle->bundle_name;
      $site_id = $bundle->site_id;
      $entity_ids = array_unique($this->getEntityIDs($bundle_name));
      $field_ids = array_unique($this->getFieldIDs($bundle_name));

      // Clear any cached @context or API docs.
      if ($site_id and module_exists('tripal_ws')) {
        tripal_clear_remote_cache($site_id);
      }

      // We want to load entities in batches to speed up performance.
      $num_eids = count($entity_ids);
      $bundle_eids_handled = 0;
      $slice_size = 100;
      while ($bundle_eids_handled < $num_eids) {
        // Get a bantch of $slice_size elements from the entities array.
        $slice = array_slice($entity_ids, $bundle_eids_handled, $slice_size);
        if ($job) {
          $job->logMessage('Getting entities for ids !start to !end of !total',
            [
              '!start' => $bundle_eids_handled,
              '!end' => $bundle_eids_handled + count($slice),
              '!total' => $num_eids,
            ]);
        }
        $bundle_eids_handled += count($slice);

        // If the bundle is from a remote site then call the appropriate
        // function, otherwise, call the local function.
        if ($site_id and module_exists('tripal_ws')) {
          $entities = tripal_load_remote_entities($slice, $site_id, $bundle_name, $field_ids);
        }
        else {
          $entities = tripal_load_entity('TripalEntity', $slice, FALSE, $field_ids, FALSE);
        }
        $job->logMessage('Got !count entities.', ['!count' => count($entities)]);

        // Now write each entity one at a time to the files.
        foreach ($entities as $entity_id => $entity) {

          // Write the same entity to all the formatters that are supported.
          foreach ($formatters as $class => $formatter) {
            //if ($class == 'TripalTabDownloader') {
            $formatter->writeEntity($entity, $job);
            //}
          }
        }
        if ($job) {
          $job->setItemsHandled($num_handled + count($slice));
        }
      }
    }

    // Now close up all the files
    foreach ($formatters as $class => $formatter) {
      $formatter->writeDone($job);
    }
  }


}
