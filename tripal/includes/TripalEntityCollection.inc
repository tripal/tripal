<?php

class TripalEntityCollection {

  /**
   * The name of the bundles (i.e. content type) to which the entities belong.
   */
  protected $bundles = array();

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
      // Delete from the tripal collection table.
      db_delete('tripal_collection')
        ->condition('collection_id', $this->collection_id)
        ->execute();

      // Delete the field groups from the tripal_bundle_collection table.
      db_delete('tripal_collection_bundle')
        ->condition('collection_id', $this->collection_id)
        ->execute();

      // Remove any files that may have been created
      foreach ($this->downloaders as $class_name => $label) {
        tripal_load_include_downloader_class($class_name);
        $outfile = $this->getOutfile($class_name);
        $downloader = new $class_name($this->bundles, $this->ids, $this->fields,
            $outfile, $this->getUserID(), $this->collection_id);
        $downloader->delete();
      }

      // Reset the class to defaults.
      $this->collection_id = NULL;
      $this->collection_name = '';
      $this->create_date = '';
      $this->description = '';

    }
    catch (Exception $e) {
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

    /* Add the IDs, Fields, Bundles for this collection from the
     * collection_bundle table.
     */
    $this->bundles = $this->getBundles();
    // If more than one bundle plop into associative array.
    $bundle_name = "";
    if (count($this->bundles) > 1) {
      foreach ($this->bundles as $bundle) {
        // If bundle name is not bio_data_# then it's an accession name from
        // a remote site, so we need to handle it differently.
        $bundle_name = $bundle->bundle_name;
        if (strpos($bundle->bundle_name, 'bio_data_') !== 0) {
          $ids[$bundle_name] = $this->getEntityIDs($bundle_name);
          $fields[$bundle_name] = $this->getFields($bundle_name);
        }
        $bundle_name = $bundle->bundle_name;
        $ids[$bundle_name] = $this->getEntityIDs($bundle_name);
        $fields[$bundle_name] = $this->getFields($bundle_name);
      }
      $this->ids = $ids;
      $this->fields = $fields;
    }
    else {
      if (!empty($this->bundles)) {
        $bundle_name = $this->bundles[0]->bundle_name;
        $this->ids = $this->getEntityIDs($bundle_name);
        $this->fields = $this->getFields($bundle_name);
      }
    }
    // Iterate through the fields and find out what download formats are
    // supported for this basket.
    $this->downloaders = $this->getDownloadFormattersList($this->fields);
  }

  /**
   * Creates a new unique collection ID used as a look up against the
   * tripal_collection_bundle to find fields, ids, and bundles.
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
          'create_date' => time(),
          'uid' => $details['uid'],
          'description' => array_key_exists('description', $details) ? $details['description'] : '',
        ))
        ->execute();
      // Now add the second table with bundle info.
      $this->addFields($details, $collection_id);
      // Now load the job into this object.
      $this->load($collection_id);
    }
    catch (Exception $e) {
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
  public function addFields($details, $collection_id) {
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
        ->fields(array(
          'bundle_name' => $details['bundle_name'],
          'ids' => serialize($details['ids']),
          'fields' => serialize($details['fields']),
          'collection_id' => $collection_id,
        ))
        ->execute();
      // Now load the job into this object.
      //$this->load($collection_bundle_id);
    }
    catch (Exception $e) {
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
    $collection_id = $this->collection_id;
    // Return the bundles from the collection_bundle table.
    $result = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle', array('bundle_name'))
      ->condition('collection_id', $collection_id, '=')
      ->execute()
      ->fetchAll();

    return $result;
  }

  /**
   * Retrieves the site id for this specific bundle fo the collection.
   *
   * @return
   *   A single site id.
   */
  public function getSiteId($bundle_name) {
    $collection_id = $this->collection_id;
    // Return the bundles from the collection_bundle table.
    $result = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle', array('site_id'))
      ->condition('collection_id', $collection_id, '=')
      ->condition('bundle_name', $bundle_name, '=')
      ->execute()
      ->fetchAssoc();

    return $result;
  }

  /**
   * Retrieves the vocabulary for a remote Tripal web service.
   *
   * @return
   *   The vocabulary of a remote Tripal web service.
   */
  protected function retrieveRemoteAPIDoc($site_id) {
    $cache_name = 'tripal_web_services_doc_' . $site_id;
    if ($cache = cache_get($cache_name)) {
      $site_doc = $cache->data;
    }
    else {
      $site_doc = tripal_get_remote_site_doc($site_id);
      if (!$site_doc) {
        cache_set('tripal_web_services_doc_'  . $site_id, $site_doc);
      }
    }
    return $site_doc;
  }

  /**
   * Retrieves the list of appropriate download formatters for the basket.
   *
   * @param $fields
   *   An array of field_ids, organized by bundles. They key is a bundle
   *   name and the value is the list of fields for that bundle.
   *
   * @return
   *   An associative array where the key is the TripalFieldDownloader class
   *   name and the value is the human-readable lable for the formatter.
   */
  public function getDownloadFormattersList($fields) {

    $downloaders = array();
    // Iterate through the fields and find out what download formats are
    // supported for this basket.
    foreach ($fields as $bundle_name => $field_group) {
      foreach ($field_group as $field_id) {
        // Check is $field_groups is an array because if it is that means we
        // nested arrays we need to deal with.
        if (is_array($field_id)) {
          foreach ($field_id as $fid) {
            // If the $field is numeric it's a field id from the local site but
            // if it is not then it's a remote field that needs to be handled differently.
            if (!is_numeric($fid)) {
              // Need the site ID from the tripal_collection_bundle table.
              $site_id = $this->getSiteId($bundle_name);
              $site_doc = $this->retrieveRemoteAPIDoc($site_id);
              // Now find the tripal formatters in the json data returned.
              $this->getRemoteFieldDownloadFormats($site_doc, $bundle_name, $fid, $site_id['site_id']);
            }
            else {
              $field_info = field_info_field_by_id($fid);
              if (!$field_info) {
                continue;
              }
              $field_name = $field_info['field_name'];
              $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
              // API function
              // All fields should support the Tab and CSV downloaders.
              $downloaders = array();
              $this->downloaders += tripal_get_field_field_formatters($field_info, $instance);
            }
          }
        }
        else {
          $field_info = field_info_field_by_id($field_id);
          if (!$field_info) {
            continue;
          }
          $field_name = $field_info['field_name'];
          $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);
          // API function
          $downloaders = array();
          $this->downloaders += tripal_get_field_field_formatters($field_info, $instance);
        }
      }
    }
    return $this->downloaders;
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
   * Retrieves the list of remote download formatters for the basket and
   * assigns them to $this->downloader.
   *
   */
  public function getRemoteFieldDownloadFormats($site_doc, $bundle_name, $field, $site_id) {
    // Now find the tripal formatters in the json data returned.
    foreach ($site_doc as $item) {
      if (is_array($item)) {
        foreach ($item as $vocab_term) {
          /* The returned $vocab item should look like this:
          "supportedProperty": [
          {
          "property": "OBI:0100026",
          "hydra:title": "Organism",
          "hydra:description
          http://www.w3.org/ns/hydra/core#description
          ": "The full scientific name for a species..",
          "required": false,
          "readable": false,
          "writeable": true.
          "tripal_formatter": "
          (
          [0] => TripalTabDownloader
          [1] => TripalCSVDownloader
          )"
          }, */
          if (!empty($vocab_term['supportedProperty'])) {
            $vocab_supported_properties = $vocab_term['supportedProperty'];
            if (is_array($vocab_supported_properties)) {
              foreach ($vocab_supported_properties as $property) {
                if ($property['property'] === $field) {
                  if (array_key_exists('tripal_formatters', $property)) {
                    $download_types = $property['tripal_formatters'];
                    if (is_array($download_types)) {
                      foreach ($download_types as $download_type) {
                        $this->downloaders[$download_type] = $download_type;
                        return $this->downloaders;
                      }
                    }
                    else {
                      $this->downloaders[$download_types] = $download_types;
                      return $this->downloaders;
                    }
                  }
                  else {
                    $result = db_select('tripal_sites')
                      ->fields('tripal_sites', array('name'))
                      ->condition('id', $site_id, '=')
                      ->execute()
                      ->fetchAssoc();
                    $site_name = $result['name'];
                    drupal_set_message(t("Cannot find tripal_formatters array elements in the remote site: $site_name"), 'warning');
                  }
                }
              }
            }
            else {
              if (in_array('tripal_formatters', $vocab_supported_properties)) {
                $download_types = $vocab_supported_properties['tripal_formatters'];
                foreach ($download_types as $download_type) {
                  $this->downloaders[$download_type] = $download_type;
                  return $this->downloaders;
                }
              }
            }
          }
        }
      }
    }
  }

  /**
   * Retrieves the list of entity IDs.
   *
   * @return
   *   An array of numeric entity IDs.
   */
  public function getEntityIDs($bundle_name) {
    $collection_id = $this->collection_id;
    // Return the bundles from the collection_bundle table.
    $result = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle', array('ids'))
      ->condition('collection_id', $collection_id, '=')
      ->condition('bundle_name', $bundle_name, '=')
      ->execute()
      ->fetchAll();

    // Unserialize the array of standard class objects.
    $unserialized_result = [];
    foreach ($result as $id_list) {
      $unserialized_id_list = unserialize($id_list->ids);

      foreach ($id_list as $item) {
        $unserialized_result[] = $unserialized_id_list;
      }
    }

    return $unserialized_result;
  }

  /**
   * Retrieves the list of fields in the basket.
   *
   * @return
   *   An array of numeric field IDs.
   */
  public function getFields($bundle_name) {
    $collection_id = $this->collection_id;

    // Get the fields from the collection_bundle table.
    $result = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle', array('fields'))
      ->condition('collection_id', $collection_id, '=')
      ->condition('bundle_name', $bundle_name, '=')
      ->execute()
      ->fetchAll();

    // Unserialize the array of standard class objects.
    $unserialized_result = array();
    foreach ($result as $field_list) {
      $unserialized_field_list = unserialize($field_list->fields);
      foreach ($field_list as $item) {
        $unserialized_result[] = $unserialized_field_list;
      }
    }

    return $unserialized_result;
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
    foreach ($this->downloaders as $class_name => $label) {
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
    // Make sure the user directory exists
    $user_dir = 'public://tripal/users/' . $this->user->uid;
    $outfilePath = $user_dir. '/' . $outfile;
    return $outfilePath;
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
    if (!$this->isFormatterCompatible($formatter)) {
      throw new Exception(t('The formatter, "@formatter", is not compatible with this data collection.', array('@formatter' => $formatter)));

    }

    if (!tripal_load_include_downloader_class($formatter)) {
      throw new Exception(t('Cannot find the formatter named "@formatter".', array('@formatter', $formatter)));
    }

    $outfile = $this->getOutfile($formatter);

    $downloader = new $formatter($this->collection_id, $outfile);
    $downloader->write();

  }

}
