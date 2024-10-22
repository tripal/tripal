<?php

namespace Drupal\tripal_chado\Database;

use Symfony\Component\Yaml\Yaml;
use Drupal\tripal\TripalDBX\TripalDbxSchema;
use Drupal\tripal\TripalDBX\Exceptions\SchemaException;

/**
 * Chado schema class.
 */
class ChadoSchema extends TripalDbxSchema {

  /**
   * {@inheritdoc}
   */
  public function getSchemaDef(array $parameters) :array {
    static $schema_structure = [];

    $source = $parameters['source'] ?? 'file';
    $format = strtolower($parameters['format'] ?? '');

    if (array_key_exists('version', $parameters) and !empty($parameters['version'])) {
      $version = $parameters['version'];
    }
    else {
      $version = $this->connection->getVersion();
    }

    if (!empty($parameters['clear'])) {
      $schema_structure = [];
    }
    if ('none' == $format) {
      return [];
    }

    // Adjust cache key.
    if ('database' == $source) {
      // Make sure we got a schema to work on.
      if (empty($this->defaultSchema)) {
        throw new SchemaException("No schema to work on.");
      }
      $version = $this->defaultSchema;
    }
    $cache_key = $source . '-' . $format . '-' . $version;

    // Check cache and compute if needed.
    if (empty($schema_structure[$cache_key])) {
      if ('file' == $source) {
        $filename =
          \Drupal::service('extension.list.module')->getPath('tripal_chado')
          . '/chado_schema/chado_schema-'
          . $version
          . '.yml'
        ;

        // Make sure we got a valid version format.
        if (!preg_match('/^\\d\\.\\d$/', $version)
            || !file_exists($filename)
        ) {
          throw new SchemaException("Invalid or unsupported Chado schema version '$version'.");
        }
        $schema_structure[$cache_key] =
          Yaml::parse(file_get_contents($filename));
      }
      elseif ('database' == $source) {
        // Use Schema object to fetch each table structures from database.
        $schema_def = [];
        $tables = $this->getTables();
        foreach (array_keys($tables) as $table) {
          $schema_def[$table] =
            $this->getTableDef($table, $parameters);
        }
        $schema_structure[$cache_key] = $schema_def;
      }
      else {
        throw new SchemaException("Invalid schema definition source '$source'.");
      }
    }
    return $schema_structure[$cache_key];
  }

  /**
   * Returns all chado base tables.
   *
   * Base tables are those that contain the primary record for a data type.
   * For example, feature, organism, stock, are all base tables.  Other tables
   * include linker tables (which link two or more base tables), property
   * tables, and relationship tables.  These provide additional information
   * about primary data records and are therefore not base tables.  This
   * function retrieves only the list of tables that are considered 'base'
   * tables.
   *
   * @return
   *   An array of base table names.
   */
  public function getMainTables() {

    // Initialize the base tables with those tables that are missing a type.
    // Ideally they should have a type, but that's for a future version of Chado.
    $base_tables = [
      'organism',
      'project',
      'analysis',
      'biomaterial',
      'eimage',
      'assay',
    ];

    // We'll use the cvterm table to guide which tables are base tables. Typically
    // base tables (with a few exceptions) all have a type.  Iterate through the
    // referring tables.
    $schema = $this->getTableSchema('cvterm');
    if (isset($schema['referring_tables'])) {
      foreach ($schema['referring_tables'] as $tablename) {

        $is_base_table = TRUE;

        // Ignore the cvterm tables + chadoprop tables.
        if (in_array($tablename, ['cvterm_dbxref', 'cvterm_relationship', 'cvtermpath', 'cvtermprop', 'chadoprop', 'cvtermsynonym'])) {
          $is_base_table = FALSE;
        }
        // Ignore relationship linked tables.
        elseif (preg_match('/_relationship$/', $tablename)) {
          $is_base_table = FALSE;
        }
        // Ignore cvterm annotation linking tables.
        elseif (preg_match('/_cvterm$/', $tablename)) {
          $is_base_table = FALSE;
        }
        // Ignore property tables.
        elseif (preg_match('/prop$/', $tablename) || preg_match('/prop_.+$/', $tablename)) {
          $is_base_table = FALSE;
        }
        // Ignore natural diversity tables.
        elseif (preg_match('/^nd_/', $tablename)) {
          $is_base_table = FALSE;
        }

        // If it's not any of the above then add it to the list.
        if ($is_base_table === TRUE) {
          array_push($base_tables, $tablename);
        }
      }
    }

    // Remove any linker tables that have snuck in.  Linker tables are those
    // whose foreign key constraints link to two or more base table.
    $final_list = [];
    foreach ($base_tables as $i => $tablename) {
      // A few tables break our rule and seems to look
      // like a linking table, but we want to keep it as a base table.
      if ($tablename == 'biomaterial' or $tablename == 'assay' or $tablename == 'arraydesign') {
        $final_list[] = $tablename;
        continue;
      }

      // Remove the phenotype table. It really shouldn't be a base table as
      // it is meant to store individual phenotype measurements.
      if ($tablename == 'phenotype') {
        continue;
      }
      $num_links = 0;
      $schema = $this->getTableSchema($tablename);
      $fkeys = $schema['foreign keys'];
      foreach ($fkeys as $fkid => $details) {
        $fktable = $details['table'];
        if (in_array($fktable, $base_tables)) {
          $num_links++;
        }
      }
      if ($num_links < 2) {
        $final_list[] = $tablename;
      }
    }

    // Now add in the cvterm table to the list.
    $final_list[] = 'cvterm';

    // Sort the tables and return the list.
    sort($final_list);
    return $final_list;
  }

  /**
   * Get information about which Chado base table a cvterm is mapped to.
   *
   * Vocabulary terms that represent content types in Tripal must be mapped to
   * Chado tables.  A cvterm can only be mapped to one base table in Chado.
   * This function will return an object that contains the chado table and
   * foreign key field to which the cvterm is mapped.  The 'chado_table'
   * property of the returned object contains the name of the table, and the
   * 'chado_field' property contains the name of the foreign key field (e.g.
   * type_id), and the
   * 'cvterm' property contains a cvterm object.
   *
   * @params
   *   An associative array that contains the following keys:
   *     - cvterm_id:  the cvterm ID value for the term.
   *     - vocabulary: the short name for the vocabulary (e.g. SO, GO, PATO)
   *     - accession:  the accession for the term.
   *     - bundle_id:  the ID for the bundle to which a term is associated.
   *   The 'vocabulary' and 'accession' must be used together, the 'cvterm_id'
   *   can be used on its own.
   *
   * @return
   *   An object containing the chado_table and chado_field properties or NULL
   *   if if no mapping was found for the term.
   *
  public function getCvtermMapping($params) {
    return chado_get_cvterm_mapping($params);
  }*/

   /***
   * Retrieve the default chado schema.
   *
   * This method ensures that we support multiple chado instances
   * and do not make any assumptions about the name of the chado schema.
   *
   * Note: The admin can change the default chado instance via the UI
   * by going to Admin > Tripal > Data Storage > Chado > Chado Schemas 
   * (admin/tripal/storage/chado/manager) and clicking "Set default".
   * We DO NOT recommend setting this programmaticly as it is confusing
   * to the admin.
   *
   * @return string
   *   The name of the schema with Chado installed that is to be considered
   *   the default.
   */
  public function getDefault() {
    return \Drupal::config('tripal_chado.settings')->get('default_schema');
  }

}
