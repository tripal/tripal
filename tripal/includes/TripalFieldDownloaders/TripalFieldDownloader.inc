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
   * The collection ID
   */
  protected $collection_id = NULL;  
  

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

    $this->bundle_name = $bundle_name;
    $this->entity_ids = $ids;
    $this->fields = $fields;
    $this->collection_id = $collection_id;
    
    // Make sure the user directory exists
    $user_dir = 'public://tripal/users/' . $user->uid;
    if (!file_prepare_directory($user_dir, FILE_CREATE_DIRECTORY)) {
      $message = 'Could not access the directory on the server for storing this file.';
      watchdog('tripal', $message, array(), WATCHDOG_ERROR);
      /*drupal_json_output(array(
        'status'  => 'failed',
        'message' => $message,
        'file_id' => '',
      ));*/
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
   // print_r("public function write \n \n ");
    global $user;
    $fh = fopen(drupal_realpath($this->outfile), "w");

    if (!$fh) {
      throw new Exception("Cannout open collection file: " . $this->outfile);
    }
 
    // If more than one bundle is supplied we need to break the headers and entities
    // apart so that headers and content correspond.
    if (count($this->bundle_name) > 1) {
      foreach ($this->bundle_name as $bundle) {
        // Set the single bundle name for getting the Header.
        $this->bundle_name = $bundle->bundle_name;
        /*$headers = $this->getHeader();
        if ($headers) {
          foreach ($headers as $line) {
            fwrite($fh, $line . "\r\n");
          }
        }*/
        //print_r($this->bundle_name);
        // Determine if the entity is remote or local.
        if (strpos($this->bundle_name, 'bio_data_') !== 0) {
          // Get all fields for that remote site/bundle name pairing, put them all in a fake entity
          // Need Field_id, have bundle_name, need collection_id
          // Return the bundles from the collection_bundle table.
          // Get the site id to build the web services call
          $remote_site_data = db_select('tripal_collection_bundle')
            ->fields('tripal_collection_bundle', array('site_id', 'fields', 'ids'))
            ->condition('collection_id', $this->collection_id, '=')
            ->condition('bundle_name', $this->bundle_name, '=')
            ->execute()
            ->fetchAssoc();
 
          $site_id = $remote_site_data['site_id'];
          $remote_fields = unserialize($remote_site_data['fields']);
          $remote_ids = unserialize($remote_site_data['ids']);
          $remote_entities = [];

          //Now we have the remote site info we need to check against the passed field and entity ids.
          foreach ($remote_ids as $remote_id) {
           // print_r("remote_id \n");
           // print_r($remote_id);
            foreach ($this->entity_ids as $entity_id) {
              if (is_array($entity_id)) {
                foreach ($entity_id as $entity) {
                  if (is_array($entity)) {
                    foreach ($entity as $single_id) {
                      if ($remote_id == $single_id) {
                        // Before a request can be made we need to get the entity type
                        // '@id' contains the bundle_name, so look for that.
                        if (!empty($remote_id)) {
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
                          // Now we have the vocab we can look for the bundle_name in the @id field
                          foreach ($site_vocab as $item) {
                            if (is_array($item)) {
                              foreach ($item as $vocab_term) {
                                if (!empty($vocab_term['@id'])) {
                                  if (strpos($vocab_term['@id'], $this->bundle_name) !== FALSE) {
                                    $entity_type = $vocab_term['hydra:title'];
                                  }
                                }
                              }
                            }
                          }
                          //print_r($entity_info);
                          //print_r("\n \n");
                          // This entity needs to be pulled down and data grabbed.
                          $query = $entity_type . '/' . $single_id;
                          $this->remote_entity = tripal_web_services_remote_request($site_id, $query);
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
                          print_r($this->remote_entity);
                        }
                      }
                      
                    }
                  }
                }
              }
              else {
                if ($remote_id == $entity_id) {
                  // Before a request can be made we need to get the entity type
                  // '@id' contains the bundle_name, so look for that.
                  if (!empty($remote_id)) {
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
                    //print_r($site_vocab);
                    // Now we have the vocab we can look for the bundle_name in the @id field
                    foreach ($site_vocab as $item) {
                      if (is_array($item)) {
                        foreach ($item as $vocab_term) {
                          if (!empty($vocab_term['@id'])) {
                            if (strpos($vocab_term['@id'], $this->bundle_name) !== FALSE) {
                              $entity_type = $vocab_term['hydra:title'];
                            }
                          }
                        }
                      }
                    }
                    //print_r($entity_info);
                    // This entity needs to be pulled down and data grabbed.
                    $query = $entity_type . '/' . $entity_id;
                    $this->remote_entity = tripal_web_services_remote_request($site_id, $query);
                  }
                }
              }
            }

            // Build the fields for the fake entity.
            // Need the label of the field so we can pull the data from the json.
            $fields = [];
            $fields = $this->fields;
            print_r($fields);
            print_r($site_vocab);
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
            foreach ($fields as $field) {
              $returned_array = $this->vocabSearch($site_vocab, $field, array());
              if (in_array('hydra:title', $returned_array)) {
                print_r($returned_array['hydra:title']);
                $fields[] = $returned_array['hydra:title'];
              }
            }

            // Because this is a remote field we need to construct a fake entity.
            $fake_tripal_entity = new stdClass();
            $fake_tripal_entity->entityType = 'TripalEntity';
            $fake_tripal_entity->entityInfo = [];
            $fake_tripal_entity->id = $entity_id;
            $fake_tripal_entity->type = 'TripalEntity';
            $fake_tripal_entity->bundle = $this->bundle_name;
            foreach ($fields as $name => $field) {
              $fake_tripal_entity->$name = [
                'und' => [
                  '0' => [
                    'value' => $field,
                  ],
                ],
              ];
            };
            $lines = $this->formatEntity($fake_tripal_entity);
            foreach ($lines as $line) {
              fwrite($fh, $line . "\r\n");
            }
          }
        }
        else {
          foreach ($this->entity_ids[$bundle_id] as $entity_id) {
            if (is_array($entity_id)) {
              foreach ($entity_id as $single_entity) {
                if (is_array($single_entity)) {
                  foreach ($single_entity as $entity) {
                    // If the field is from a remote entity then we need to load the info through web services. 

                    $result = tripal_load_entity('TripalEntity', array($entity), FALSE, $this->fields);
                    $entity_info = $result[$entity];
                    $lines = $this->formatEntity($entity_info);
                    foreach ($lines as $line) {
                      fwrite($fh, $line . "\r\n");
                    }
                  }
                }
                else {
                  $result = tripal_load_entity('TripalEntity', array($single_entity), FALSE, $this->fields);
                  $entity = $result[$single_entity];
                  $lines = $this->formatEntity($entity);
                  foreach ($lines as $line) {
                    fwrite($fh, $line . "\r\n");
                  }
                }
              }
            }
            else {
              $result = tripal_load_entity('TripalEntity', array($entity_id), FALSE, $this->fields);
              $entity = $result[$entity_id];
              $lines = $this->formatEntity($entity);
              foreach ($lines as $line) {
                fwrite($fh, $line . "\r\n");
              }
            }
          }
        }
      }
    }
    else {
      $bundle_id = $this->bundle_name[0]->bundle_name;
      $this->bundle_name = $bundle_id;
      $headers = $this->getHeader();
      if ($headers) {
        foreach ($headers as $line) {
          fwrite($fh, $line . "\r\n");
        }
      }
      foreach ($this->entity_ids as $entity_id) {
        if (is_array($entity_id)) {
          foreach ($entity_id as $single_entity) {
            if (is_array($single_entity)) {
              foreach ($single_entity as $entity) {
                $result = tripal_load_entity('TripalEntity', array($entity), FALSE, $this->fields);
                $entity_info = $result[$entity];
                $lines = $this->formatEntity($entity_info);
                foreach ($lines as $line) {
                  fwrite($fh, $line . "\r\n");
                }
              }
            }
            else {
              $result = tripal_load_entity('TripalEntity', array($single_entity), FALSE, $this->fields);
              $entity = $result[$single_entity];
              $lines = $this->formatEntity($entity);
              foreach ($lines as $line) {
                fwrite($fh, $line . "\r\n");
              }
            }
          }
        }
        else {
          $result = tripal_load_entity('TripalEntity', array($entity_id), FALSE, $this->fields);
          $entity = $result[$entity_id];
          $lines = $this->formatEntity($entity);
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
   * Returns the array of the field with title included.
   */
  public function vocabSearch($haystack, $needle, &$current_array) {
    if (is_array($haystack)) {
      foreach ($array as $item) {
        if (is_array($item)) {
          $current_array = $item;
          vocabSearch($item, $needle, $current_array);
        }
        else {
          print($item);
          if (strtolower($item) == strtolower($needle)) {
            return $current_array;
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
   *  Retreives header lines
   *
   *  This function should be implemented by a child class.  It should return
   *  the header lines for an output file.
   */
  abstract protected function getHeader();

}