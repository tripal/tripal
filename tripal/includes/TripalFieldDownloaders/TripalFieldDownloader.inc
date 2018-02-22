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
   * The bundle name.
   */
  protected $bundle_name = '';

  /**
   * The collection ID
   */
  protected $collection_id = NULL;

  /**
   * The collection bundle IDs
   */
  protected $collection_bundles = '';

    /**
   * The collection bundle IDs
   */
  protected $collection_bundle_ids = array();

  /**
   * A set of entity IDs.
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
   * An array of the header.
   */
  protected $header_array = array();

  /**
   * The remote site json data returned for the entity
   */
  protected $remote_entity = '';

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
      $outfile_name, $uid, $collection_id) {

    $user = user_load($uid);
    if (!$user) {
      throw new Exception(t("The provided user ID does not reference a real user: '@uid'.", array('@uid' => $uid)));
    }
    if (!$outfile_name) {
      throw new Exception("Please provide an outputfilename");
    }

    $this->collection_bundles = db_select('tripal_collection_bundle')
      ->fields('tripal_collection_bundle')
      ->condition('collection_id', $collection_id, '=')
      ->execute()
      ->fetchAll();

    foreach ($this->collection_bundles as $collection_bundle) {
      $collection_id = $collection_bundle->collection_bundle_id;
      $this->collection_bundle_ids[] = $collection_id;
      $this->bundle_name_ . $collection_id = $collection_bundle->bundle_name;
      $this->entity_ids_ . $collection_id = unserialize($collection_bundle->ids);
      $this->fields_ . $collection_id = unserialize($collection_bundle->fields);
      $this->site_id_ . $collection_id = $collection_bundle->site_id;
    }
    $this->collection_id = $collection_id;

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

    $this->outfile = $user_dir . '/' . $outfile_name;
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

    $bundle_collections = $this->collection_bundles;
    foreach ($bundle_collections as $bundle_collection) {
      $collection_bundle_id = $bundle_collection->collection_bundle_id;
      $bundle_name = $bundle_collection->bundle_name;
      $fields = unserialize($bundle_collection->fields);
      $entity_ids = unserialize($bundle_collection->ids);
      $site_id = $bundle_collection->site_id;

      // Determine if the entity is remote or local.
      if (strpos($bundle_name, 'bio_data_') !== 0) {
        $external = TRUE;
      }
      else {
        $external = FALSE;
      }

      // Now handle the external bundles.
      if ($external) {
        // Now we have the remote site info we need to check against the
        // passed field and entity ids.
        foreach ($entity_ids as $remote_id) {
          $fake_tripal_entity = $this->getRemoteEntity($remote_id, $site_id, $fields, $bundle_name);
          $lines = $this->formatEntity($fake_tripal_entity);
          foreach ($lines as $line) {
            fwrite($fh, $line . "\r\n");
          }
        }
      }
      else {
        // Now handle the local bundles.
        foreach ($entity_ids as $entity) {
          // If the field is from a remote entity then we need to load the info through web services.
          $result = tripal_load_entity('TripalEntity', array($entity), FALSE, $fields);
          $entity_info = $result[$entity];
          $lines = $this->formatEntity($entity_info);
          foreach ($lines as $line) {
            fwrite($fh, $line . "\r\n");
          }
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
   * Recursive function to walk down the array looking for the passed value.
   *
   * @param $haystack
   *   The array of vocab returned from the remote site.
   * @param $needle
   *   The field which is trying to be matched to.
   * @param $current_array
   *   Holder for the last array identified.
   *
   * @return
   *   The array of the field with title included.
   */
  public function vocabSearch($haystack, $needle, &$current_array) {
    static $_return_array = array();
    // If the $current_array has a value and the $item is not an array
    // then we are at the single item section of the nested vocab array
    // and we can start to check for the $needle.
    if ($current_array['property'] == $needle) {
      $_return_array = $current_array;
      return $_return_array;
    }
    elseif (is_array($haystack) && count($haystack) > 0) {
      foreach ($haystack as $item) {
        if (is_array($item)) {
          // Clear out the previous array.
          $current_array = [];
          $current_array = $item;
          $this->vocabSearch($item, $needle, $current_array);
        }
      }
    }
    return $_return_array;
  }

  /**
   * Find the header info for the remote field
   *
   * @param $field
   *   The accession which is stored as the field id.
   *
   * @return
   *   .
   */
  public function getRemoteEntity($remote_ids, $site_id, $remote_fields, $bundle_name) {
    // Before a request can be made we need to get the entity type
    // '@id' contains the bundle_name, so look for that.
    if (!empty($site_id)) {
      $cache_name = 'tripal_web_services_vocab_' . $site_id;
      if ($cache = cache_get($cache_name)) {
        $site_vocab = $cache->data;
      }
      else {
        $site_vocab = tripal_web_services_vocab_request($site_id);
        if (!empty($site_vocab)) {
          cache_set('tripal_web_services_vocab', $site_vocab);
        }
      }
      // Now we have the vocab we can look for the bundle_name in the @id field.
      foreach ($site_vocab as $item) {
        if (is_array($item)) {
          foreach ($item as $vocab_term) {
            if (!empty($vocab_term['@id'])) {
              if (strpos($vocab_term['@id'], $bundle_name) !== FALSE) {
                $entity_type = $vocab_term['hydra:title'];
              }
            }
          }
        }
      }
      if (is_array($remote_ids)) {
        foreach ($remote_ids as $remote_id) {
          // This entity needs to be pulled down and data grabbed.
          $query = $entity_type . '/' . $remote_id;
          $this->remote_entity = tripal_web_services_remote_request($site_id, $query);
        }
      }
      else {
        $query = $entity_type . '/' . $remote_ids;
        $this->remote_entity = tripal_web_services_remote_request($site_id, $query);
      }
      /**
       * remote_entity looks like this:
       *  [@context] => http://demo.tripal.info/3.x/sites/default/files/tripal/ws/context/content.v0_1.gene.296.json
       *  [@id] => http://demo.tripal.info/3.x/web-services/content/v0.1/Gene/296
       *  [@type] => gene
       *  [label] => td01_000348m.g
       *  [ItemPage] => http://demo.tripal.info/3.x/bio_data/296
       *  [type] => Gene
       *  [organism] => Array
       *     (
       *         [label] => <i>Tripalus databasica</i>
       *          [genus] => Tripalus
       *          [species] => databasica
       *      )
       *
       *  [name] => td01_000348m.g
       *  [identifier] => td01_000348m.g
       *  [sequence_checksum] => d41d8cd98f00b204e9800998ecf8427e
       *  [time_accessioned] => 2011-06-30 17:00:58.050856
       *  [time_last_modified] => 2011-06-30 17:00:58.050856
       *  [sequence_coordinates] => http://demo.tripal.info/3.x/web-services/content/v0.1/Gene/296/Sequence+coordinates
       *  [relationship] => http://demo.tripal.info/3.x/web-services/content/v0.1/Gene/296/relationship
       */

      // Build the fields for the fake entity.
      /**
       * $site_vocab will look like this:
       * [3] => Array(
       *  [property] => data:0842
       *  [hydra:title] => Identifier
       *  [hydra:description] =>
       *  [required] => 1
       *  [readable] =>
       *  [writeable] => 1
       *  [tripal_formatters] => Array
       *    (
       *      [0] => TripalTabDownloader
       *      [1] => TripalCSVDownloader
       *    )
       *  )
       */
      // If only one field was passed then it needs to be handled differently.
      if (!is_array($remote_fields)) {
        $field_entity = array();
        $returned_array = $this->vocabSearch($site_vocab, $remote_fields, $field_entity);
        if ($returned_array['hydra:title']) {
          $fields[$remote_fields]['field_name'] = $returned_array['hydra:title'];
          // Turn the hydra:title into the machine name to get the label.
          $machine_name = (str_replace(' ', '-', strtolower($fields[$remote_fields]['field_name'])));
          $fields[$remote_fields]['label'] = $machine_name;
          // Get the value from the previously created entity.
          $fields[$remote_fields]['value'] = $this->remote_entity[$machine_name];
          // Add the accession information.
          $fields[$remote_fields]['accession'] = $remote_fields;
        }
      }
      else {
        foreach ($remote_fields as $field) {
          $field_entity = array();
          $returned_array = $this->vocabSearch($site_vocab, $field, $field_entity);
          if (!empty($returned_array)) {
            if ($returned_array['hydra:title']) {
              $fields[$field]['field_name'] = $returned_array['hydra:title'];
              // Turn the hydra:title into the machine name to get the label.
              $machine_name = (str_replace(' ', '-', strtolower($fields[$field]['field_name'])));
              $fields[$field]['label'] = $machine_name;
              // Get the value from the previously created entity.
              $fields[$field]['value'] = $this->remote_entity[$machine_name];
              // Add the accession information.
              $fields[$field]['accession'] = $field;
            }
          }
        }
      }
      // Because this is a remote field we need to construct a fake entity.
      $fake_tripal_entity = new stdClass();
      $fake_tripal_entity->entityType = 'TripalEntity';
      $fake_tripal_entity->entityInfo = [];
      $fake_tripal_entity->id = $entity_id;
      $fake_tripal_entity->type = 'TripalEntity';
      $fake_tripal_entity->bundle = $bundle_name;
      if (is_array($fields)) {
        foreach ($fields as $name => $field) {
          $fake_tripal_entity->$name = [
            'und' => [
              '0' => [
                'field_name' => $field['field_name'],
                'label' => $field['label'],
                'value' => $field['value'],
                'accession' => $field['accession'],
              ],
            ],
          ];
        };
      }
      else {
        $fake_tripal_entity->$name = [
          'und' => [
            '0' => [
              'field_name' => $fields[$remote_fields]['field_name'],
              'label' => $fields[$remote_fields]['label'],
              'value' => $fields[$remote_fields]['value'],
              'accession' => $fields[$remote_fields]['accession'],
            ],
          ],
        ];
      }
    }
    return $fake_tripal_entity;
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