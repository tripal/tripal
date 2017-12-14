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
            $outfile, $this->getUserID());
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
    foreach ($this->fields as $field_group) {
      foreach ($field_group as $field_id) {
        // Check is $field_groups is an array because if it is that means we
        // nested arrays we need to deal with.
        if (is_array($field_id)) {
          foreach ($field_id as $field) {
            $field_info = field_info_field_by_id($field);
            if (!$field_info) {
              continue;
            }
            $field_name = $field_info['field_name'];
            $field_type = $field_info['type'];
            $field_module = $field_info['module'];
            $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);

            $downloaders = array();

            // TODO: replace this code with the new tripal_get_field_field_formatters()
            // API function
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
        else {
          $field = field_info_field_by_id($field_id);
          if (!$field) {
            continue;
          }
          $field_name = $field['field_name'];
          $field_type = $field['type'];
          $field_module = $field['module'];
          $instance = field_info_instance('TripalEntity', $field_name, $bundle_name);

          $downloaders = array();

          // TODO: replace this code with the new tripal_get_field_field_formatters()
          // API function

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
    }
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
      throw new Exception("Must provide a 'bundle_name' key to TripalEntityCollection::add().");
    }
    if (!$details['ids']) {
      throw new Exception("Must provide a 'ids' key to TripalEntityCollection::add().");
    }
    if (!$details['fields']) {
      throw new Exception("Must provide a 'fields' key to TripalEntityCollection::add().");
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
    // Return the bundles from the collection_bundle table.
    $result = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle', array('fields'))
      ->condition('collection_id', $collection_id, '=')
      ->condition('bundle_name', $bundle_name, '=')
      ->execute()
      ->fetchAll();

     // Unserialize the array of standard class objects.
    $unserialized_result = [];
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
   * Retreives the name of the collection.
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
   * Retreives the collection description
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

    // Filter out fields that aren't supported by the formatter.
    $supported_fields = array();
    foreach ($this->fields as $field_group) {
      foreach ($field_group as $field_id) {
        // Check is $field_id is an array because if it is that means we
        // nested arrays we need to deal with.
        if (is_array($field_id)) {
          foreach ($field_id as $field) {
            // If the formatter is TripalTabDownloader or TripalCSVDownloader then
            // we always want to support the field.
            if ($formatter == 'TripalTabDownloader' or $formatter == 'TripalCSVDownloader') {
              if (!in_array($field, $supported_fields)) {
                $supported_fields[] = $field;
              }
              continue;
            }

            // Otherwise, find out if the formatter specified is supporte by the
            // field and if so then add it to our list of supported fields.
            $field_info = field_info_field_by_id($field);
            $field_name = $field_info['field_name'];
            $field_type = $field_info['type'];
            if (tripal_load_include_field_class($field_type)) {
              $settings = $field_type::$default_instance_settings;
              if (array_key_exists('download_formatters', $settings)) {
                if (in_array($formatter, $settings['download_formatters'])) {
                  $supported_fields[] = $field;
                }
              }
            }
          }
        }
        else {
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
      }
    }

    $downloader = new $formatter($this->bundles, $this->ids, $supported_fields, $outfile, $this->user->uid);
    $downloader->write();

  }

}
