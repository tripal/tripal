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
  protected $printable_fields = array();

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
    Array (
      [local] => Array(
        [bio_data_7] => Array(
          [by_field] => Array(
            [56] => data:2091,
            [57] => OBI:0100026,
            [17] => schema:name,
            [58] => data:2044,
            [34] => data:0842,
            [67] => schema:alternateName,
          ),
          [by_accession] => Array (
            [data:2091] => 56,
            [OBI:0100026] => 57,
            [schema:name] => 17,
            [data:2044] => 58,
            [data:0842] => 34,
            [schema:alternateName] => 67,
          ),
        ),
      ),
    )
   * @endcode
   */
  protected $fields2terms = array();


  /**
   * A list of field and instance items, indexed first by site_id with 'local'
   * being the key for local fields and the numeric site_id for remote
   * fields.  The second-levle key is the bundle_name and the the field_id.
   * Below the field_id are the keys 'field' or 'instance' where the
   * value of each is the field or instance details respectively.
   */
  protected $fields = array();


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
    $this->collection = db_select('tripal_collection', 'tc')
      ->fields('tc')
      ->condition('collection_id', $collection_id, '=')
      ->execute()
      ->fetchObject();

    // Make sure the user directory exists
    $user = user_load($this->collection->uid);
    $user_dir = 'public://tripal/users/' . $user->uid;

    // Set the collection ID of the collection that this downloader will use.
    $this->collection_id = $collection_id;
    $this->outfile = $user_dir . '/' . $outfile_name;
    $this->selected_fields = $selected_fields;

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

    // If a field is a TripalField then check its supported downloaders.
    if (tripal_load_include_field_class($field_type)) {
      $formatters = $field_type::$download_formatters;
      if (in_array($formatter, $settings['download_formatters'])) {
        return TRUE;
      }
    }

    $is_remote = $field['field']['storage']['type'] == 'tripal_remote_field' ? TRUE : FALSE;
    if ($is_remote) {
      if (in_array($formatter, $instance['formatters'])) {
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
    $user = user_load($this->collection->uid);

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

    $bundle_collections = $this->collection_bundles;
    foreach ($bundle_collections as $bundle_collection) {
      $collection_bundle_id = $bundle_collection->collection_bundle_id;
      $bundle_name = $bundle_collection->bundle_name;
      $entity_ids = $bundle_collection->ids;
      $fields = $bundle_collection->fields;
      $site_id = $bundle_collection->site_id;

      foreach ($entity_ids as $entity_id) {

        // if we have a site_id then we need to get the entity from the
        // remote service. Otherwise create the entity from the local system.
        if ($site_id) {
          $entity = $this->loadRemoteEntity($entity_id, $site_id, $bundle_name);
          if (!$entity) {
            continue;
          }
        }
        else {
          $result = tripal_load_entity('TripalEntity', array($entity_id), FALSE, $fields);
          $entity = $result[$entity_id];
        }

        if (!$entity) {
          continue;
        }

        $lines = $this->formatEntity($entity);
        foreach ($lines as $line) {
          fwrite($fh, $line . "\r\n");
        }
      }
    }
    fclose($fh);

    $file = new stdClass();
    $file->uri = $this->outfile;
    $file->filename = basename($this->outfile);
    $file->filemime = file_get_mimetype($this->outfile);
    $file->uid = $user->uid;
    $file->status = FILE_STATUS_PERMANENT;

    // Check if this file already exists. If it does then just update
    // the stats.
    $fid = db_select('file_managed', 'fm')
      ->fields('fm', array('fid'))
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


    // We use the fid for the last argument because these files
    // aren't really associated with any entity, but we need a value./
    // But, only add the usage if it doens't already exists.
    $usage = file_usage_list($file);
    if (array_key_exists('tripal', $usage)) {
      if (!array_key_exists('data-collection', $usage['tripal'])) {
        file_usage_add($file, 'tripal', 'data-collection', $fid);
      }
    }
  }

  /**
   * Setups a download stream for the file.
   */
  public function download() {

  }

  /**
   * Build and return a fake entity from a remote site using
   * tripal web services calls.
   *
   * @param $remote_ids
   *   Array of the remote ids.
   *
   * @param $site_id
   *   Unique site id assigned in the tripal_sites table when
   *   a new site is created via the web services interface.
   *
   * @param $remote_fields
   *   Array of the remote fields.
   *
   * @param $bundle_name
   *   Bundle name of the remote field, in this instance it will be
   *   the accession of the field.
   *
   * @return $fake_tripal_entity
   *    This is a fake entity structured to allow the format
   *    entity function to process and return the info.
   */
  protected function loadRemoteEntity($remote_id, $site_id, $bundle_name) {

    // Get the site documentation
    $site = empty($site_id) ? 'local' : $site_id;
    $site_doc = $this->retrieveRemoteAPIDoc($site_id);

    // Get the remote entity and create the fake entity.
    $query = $bundle_name . '/' . $remote_id;
    $remote_entity = tripal_query_remote_site($site_id, $query);
    if (!$remote_entity) {
      return FALSE;
    }

    // Start building the fake id.
    $entity = new stdClass();
    $entity->entityType = 'TripalEntity';
    $entity->entityInfo = [];
    $entity->id = $remote_id;
    $entity->type = 'TripalEntity';
    $entity->bundle = $bundle_name;
    $entity->site_id = $site_id;

    // Get the context JSON for this remote entity, we'll use it to map
    // the properties to the correct fields.
    $context = drupal_http_request($remote_entity['@context']);
    $context = drupal_json_decode($context->data);
    $context = $context['@context'];

    // Iterate through the fields that are printable and get those values
    // from the results.
    foreach ($this->printable_fields as $accession => $label) {
      $field_id = $this->fields2terms[$site][$bundle_name]['by_accession'][$accession];

      // If the field isn't part of this bundle then skip it.
      if (!$field_id) {
        continue;
      }

      $field = $this->fields[$site][$bundle_name][$field_id]['field'];
      $instance = $this->fields[$site][$bundle_name][$field_id]['instance'];
      $field_name = $field['field_name'];
      $accession = $instance['settings']['term_vocabulary'] . ':' . $instance['settings']['term_accession'];

      // Get the key for this field from the context.
      $field_key = $accession;
      foreach ($context as $k => $v) {
        if (!is_array($v)) {
        }
        if (!is_array($v) and $v == $accession) {
          $field_key = $k;
        }
      }

      // If the field is not in this remote bundle then add an empty value for
      // it.
      if (!$field_key) {
        $entity->{$field_name}['und'][0]['value'] = '';
        continue;
      }

      // If the key is for a field that is not "auto attached' then we need
      // to get that field through a separate call.
      $needs_query = FALSE;
      if (array_key_exists($field_name, $context) and is_array($context[$field_name]) and
          array_key_exists('@type', $context[$field_name]) and $context[$field_name]['@type'] == '@id'){
        $needs_query = TRUE;
      }

      $value = '';
      if (!$needs_query) {
        $value = $remote_entity[$field_key];
      }
      $entity->{$field_name}['und'][0]['value'] = $value;
    }

    return $entity;
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
   * A helper function for the setFields() function.
   *
   * Adds local fields to the list of fields.
   */
  private function setLocalFields() {
    foreach ($this->collection_bundles as $collection_bundle) {
      $bundle_name = $collection_bundle->bundle_name;
      $site = empty($collection_bundle->site_id) ? 'local' : $collection_bundle->site_id;
      // Skip remote fields.
      if ($collection_bundle->site_id) {
        continue;
      }
      foreach ($collection_bundle->fields as $field_id) {
        $field = field_info_field_by_id($field_id);
        $instance = field_info_instance('TripalEntity', $field['field_name'], $bundle_name);
        $this->fields[$site][$bundle_name][$field_id]['field'] = $field;
        $this->fields[$site][$bundle_name][$field_id]['instance'] = $instance;
      }
    }
  }

  /**
   * A helper function for the setFields() function.
   *
   * Adds remote fields to the list of fields.
   */
  private function setRemoteFields() {
    foreach ($this->collection_bundles as $collection_bundle) {
      $bundle_name = $collection_bundle->bundle_name;
      // Skip local fields.
      if (!$collection_bundle->site_id) {
        continue;
      }

      $site = empty($site_id) ? 'local' : $site_id;
      $site_doc = $this->retrieveRemoteAPIDoc($collection_bundle->site_id);

      // Get the class that matches this bundle.
      $class = $this->getRemoteClass($bundle_name, $site_doc);

      // Iterate through the fields of this collection and get the
      // info for each one from the class.  We will create "fake" field and
      // instance info arrays.
      foreach ($collection_bundle->fields as $field_id) {

        // Get the property from the document for this field.
        $property = $this->getRemoteClassProperty($class, $field_id, $site_doc);

        // Now create the fake field and instance.
        list($vocab, $accession) = explode(':', $field_id);
        $field_name = 'tripal_remote_site_' . $collection_bundle->site_id . '_' . $field_id;
        $field = array(
          'field_name' => $field_name,
          'type' => $field_name,
          'storage' => array(
            'type' => 'tripal_remote_site'
          ),
        );
        $instance = array(
          'label' => $property['hydra:title'],
          'description' => $property['hydra:description'],
          'formatters' => $property['tripal_formatters'],
          'settings' => array(
            'term_vocabulary' => $vocab,
            'term_accession' => $accession
          ),
          'field_name' => $field_name,
          'entity_type' => 'TripalEntity',
          'bundle_name' => $bundle_name,
        );
        $this->fields[$site][$bundle_name][$field_id]['field'] = $field;
        $this->fields[$site][$bundle_name][$field_id]['instance'] = $instance;
      }
    }
  }

  private function getRemoteClass($class_id, $site_doc){
    // Get the class that matches this bundle.
    $classes = $site_doc['supportedClass'];
    $class = NULL;
    foreach ($classes as $item) {
      if ($item['@id'] == $class_id) {
        $class = $item;
      }
    }
    return $class;
  }

  private function getRemoteClassProperty($class, $prop_id, $site_doc){
    $properties = $class['supportedProperty'];
    foreach ($properties as $item) {
      if ($item['property'] == $prop_id) {
         return $item;
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