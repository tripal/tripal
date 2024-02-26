<?php

namespace Drupal\tripal\Services;

use Drupal\field\Entity\FieldStorageConfig;
use \Drupal\tripal\Services\TripalEntityTitle;


class TripalEntityLookup {

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * Stores the bundle (entity type) object.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType $entity_type
   **/
  protected $entity_type = NULL;

  /**
   * The TripalStorage object.
   *
   * @var \Drupal\tripal\TripalStorage\TripalStorageBase $storage
   **/
  protected $storage = NULL;



  /**
   * Used by fields to get a ready-to-use url to link to an entity.
   *
   * @param string $displayed_string
   *   The text that will be displayed as a url link
   * @param integer $record_id
   *   The primary key value for the requested record
   * @param array $item_settings
   *   Contains the following key-value pairs:
   *   'storage_plugin_id' => The id of the TripalStorage plugin, e.g. "chado_storage"
   *   'termIdSpace' => The bundle's CV term namespace e.g. "NCIT"
   *   'termAccession' => The bundle's CV term accession e.g. "C47954"
   *
   * @return string
   *   The rendered url, or if no match was found, the original $displayed_string.
   */
  public function getFieldUrl($displayed_string, $record_id, $item_settings) {
    $bundle_id = $this->getBundleFromCvTerm($item_settings['termIdSpace'], $item_settings['termAccession']);
    if ($bundle_id) {
      $base_table = $this->getTableFromCvTerm($item_settings['termIdSpace'], $item_settings['termAccession']);
      if ($base_table) {
        $uri = $this->getEntityURI($item_settings['storage_plugin_id'], $base_table, $record_id, $bundle_id);
        if ($uri) {
          // Url::fromUri($uri) takes 0.75 seconds!
          //$displayed_string = Link::fromTextAndUrl($displayed_string, Url::fromUri($uri))->toString();
          // we can just bypass that and save tons of time -- @to-do is that okay?
          $displayed_string = '<a href="' . $uri . '">' . $displayed_string . '</a>';
        }
      }
    }
    return $displayed_string;
  }

  /**
   * Retrieve the base chado table for a given bundle given the bundle's CV term
   *
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. "NCIT"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. "C47954"
   *
   * @return string
   *   The chado table name, or null if no match found.
   */
  public function getTableFromCvTerm($termIdSpace, $termAccession) {
    $chado_table = NULL;
    $entity_type = 'tripal_entity';
    $bundle = $this->getBundleFromCvTerm($termIdSpace, $termAccession);
    if ($bundle) {
      $entityFieldManager = \Drupal::service('entity_field.manager');
      $fields = $entityFieldManager->getFieldDefinitions($entity_type, $bundle);
      $field_list = array_keys($fields);
      $type_name = NULL;
      $base_table = NULL;
      foreach ($field_list as $field_name) {
        // Skip drupal fields. Look for the first tripal field that has a base_column
        // set. Fields from linker tables do not have a base_column.
        if (preg_match('/^'.$bundle.'/', $field_name)) {
          $field_storage = FieldStorageConfig::loadByName('tripal_entity', $field_name);
          if ($field_storage) {
            $storage_plugin_settings = $field_storage->getSettings()['storage_plugin_settings'];
            $base_table = $storage_plugin_settings['base_table'];
            $base_column = $storage_plugin_settings['base_column'] ?? '';
            if ($base_table and $base_column) {
              $chado_table = $base_table;
              break;
            }
          }
        }
      }
    }
    return $chado_table;
  }

  /**
   * Returns a list of columns with a not null constraint in the
   * indicated chado table. The primary key is excluded.
   * It takes significant time to retrieve $chado_schema, so we cache the results.
   *
   * @param string $chado_table
   *   The name of the chado table.
   * @param string $chado_schema
   *   The chado schema name.
   * @param string $chado_version
   *   The chado version.
   *
   * @return array
   *   The chado table name, or null if no match found.
   */
  public function getNotNullColumns($chado_table, $chado_schema = NULL, $chado_version = NULL) {
$t1 = microtime(true); //@@@
    // Retrieve the default name of the chado schema if it's not provided.
    if ($chado_schema === NULL) {
      $chado_schema = chado_get_schema_name('chado');
    }
    if ($chado_version === NULL) {
      $chado_version = chado_get_version(FALSE, FALSE, $chado_schema);
    }
    $cache_id = 'tripalentitylookup:' . $chado_schema . '.' . $chado_table;
    if ($cache = \Drupal::cache()->get($cache_id)) {
      $cache_values = $cache->data;
$t2 = microtime(true); dpm($t2 - $t1, "Elapsed time for cached lookup="); //@@@
    }
    else {
      $cache_values = [];
      $chado_schema = new \Drupal\tripal_chado\api\ChadoSchema($chado_version, $chado_schema);
      $table_schema = $chado_schema->getTableSchema($chado_table);
      $cache_values['primary_keys'] = $table_schema['primary key'];
      $not_null_columns = [];
      foreach ($table_schema['fields'] as $column => $config) {
        if (!in_array($column, $cache_values['primary_keys'])) {
          if (array_key_exists('not null', $config) and $config['not null']) {
            $cache_values['not_null_columns'][] = $column;
          }
        }
      }
      \Drupal::cache()->set($cache_id, $cache_values);
$t2 = microtime(true); dpm($t2 - $t1, "Elapsed time for UNcached lookup="); //@@@
    }
    return $cache_values['not_null_columns'];
  }

  /**
   * Retrieve a Tripal bundle id based on its CV term
   *
   * @param string $termIdSpace
   *   The bundle's CV Term namespace e.g. "NCIT"
   * @param string $termAccession
   *   The bundle's CV term accession e.g. "C47954"
   *
   * @return string
   *   The bundle id, or null if no match found.
   */
  public function getBundleFromCvTerm($termIdSpace, $termAccession) {
    $bundle_id = NULL;
    $bundle_manager = \Drupal::service('entity_type.bundle.info');
    $bundle_list = $bundle_manager->getBundleInfo('tripal_entity');
    foreach ($bundle_list as $id => $properties) {
      // Get each bundle's CV term
      $bundle_info = \Drupal::entityTypeManager()->getStorage('tripal_entity_type')->load($id);
      $bundleIdSpace = $bundle_info->getTermIdSpace();
      $bundleAccession = $bundle_info->getTermAccession();
      // If this is the desired bundle, the values will match
      if (($termIdSpace == $bundleIdSpace) and ($termAccession == $bundleAccession)) {
        $bundle_id = $id;
        break;
      }
    }
    return $bundle_id;
  }

  /**
   * Retrieve a uri for an entity corresponding to a record in a table.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage"
   * @param string $base_table
   *   The name of the chado table
   * @param integer $record_id
   *   The primary key value for the requested record in the $base_table
   * @param string $bundle_id
   *   The name of the drupal bundle, e.g. for base table 'arraydesign' it is 'array_design'
   * @param string $chado_schema
   *   The chado schema name, usually 'chado'. Pass NULL for the default chado schema.
   * @param string $chado_version
   *   The chado version, usually '1.3'. Pass NULL for the default version of the given schema.
   *
   * @return string
   *   The local uri string for the requested entity.
   *   Will be null if either zero or multiple hits.
   */
  public function getEntityURI($datastore, $base_table, $record_id, $bundle_id) {
    $uri = NULL;
    $id = $this->getEntityIdFromRecordId($datastore, $base_table, $record_id, $bundle_id);
    if ($id) {
      $uri = "internal:/bio_data/$id";
    }

    return $uri;
  }

  /**
   * Retrieve the pkey for an entity corresponding to a record in a table.
   *
   * @param string $datastore
   *   The id of the TripalStorage plugin, e.g. "chado_storage"
   * @param string $base_table
   *   The name of the chado table
   * @param integer $record_id
   *   The primary key value for the requested record in the $base_table
   * @param string $bundle_id
   *   The name of the drupal bundle, e.g. for base table 'arraydesign' it is 'array_design'
   * @param string $chado_schema
   *   The chado schema name, usually 'chado'. Pass NULL for the default chado schema.
   * @param string $chado_version
   *   The chado version, usually '1.3'. Pass NULL for the default version of the given schema.
   *
   * @return integer
   *   The id for the requested entity in the tripal_entity table.
   *   Will be null if zero or if multiple hits.
   */
  public function getEntityIdFromRecordId($datastore, $base_table, $record_id, $bundle_id, $chado_schema = NULL, $chado_version = NULL) {
    $id = NULL;
    $not_null_columns = $this->getNotNullColumns($base_table, $chado_schema, $chado_version);
    if (!$not_null_columns) {
      dpm("Temporary warning that there are no not-null columns for table \"$base_table\"");
      return NULL;
    }
    // @@@to-do this is a temporary hack for the Manufacturer on the Array Design content type
    $not_null_columns[0] = preg_replace('/_id$/', '', $not_null_columns[0]);

    $entity_table_name = 'tripal_entity__' . $bundle_id . '_' . $not_null_columns[0];
    $entity_column_name = $bundle_id . '_' . $not_null_columns[0] . '_record_id';

    // Query the appropriate field table for this record_id
    $conn = \Drupal::service('database');
    $query = $conn->select($entity_table_name, 'e');
    $query->addField('e', 'entity_id');
    $query->condition('e.' . $entity_column_name, $record_id, '=');

    // There should only be one hit, but this is here to check for
    // multiple hits just in case. If this happens, we return null.
    $num_hits = $query->countQuery()->execute()->fetchField();
    if ($num_hits == 1) {
      $id = $query->execute()->fetchField();
    }
    else {
      dpm("Temporary warning that there were $num_hits hits to record_id $record_id in table $entity_table_name column $entity_column_name"); //@@@
    }
    return $id;
  }

}
