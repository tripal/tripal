<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;

/**
 * @ChadoBuddy(
 *   id = "chado_dbxref_buddy",
 *   label = @Translation("Chado Dbxref Buddy"),
 *   description = @Translation("Provides helper methods for managing chado dbs and dbxrefs.")
 * )
 */
class ChadoDbxrefBuddy extends ChadoBuddyPluginBase {

  /**
   * Get chado.db record.
   *
   * @param array $identifiers
   *   An array where the key is a column in the chado.db table and the value
   *   describes the db you want to select. Valid keys include:
   *     - db_id
   *     - name
   *     - description
   * @param $options
   *   None supported yet. Here for consistency.
   * @return array|object
   *   If the identifiers match a single record then we return an object
   *     describing that record.
   *   If the identifiers match multiple records, then we return an array
   *     of objects describing the results.
   */
  public function getDb(array $identifiers, array $options) {

  }

  /**
   * Retrieves a chado database reference variable.
   *
   * @param array $identifiers
   *   An array where the key is a column in chado and the value describes the
   *   dbxref you want to select. Valid keys include:
   *     - db_id
   *     - db_name
   *     - accession
   *     - idspace
   *     - version
   *     - dbxref_id
   * @param $options
   *   None supported yet. Here for consistency.
   * @return array|object
   *   If the select values return a single record then we return an object
   *     describing that record.
   *   If the select values return multiple records, then we return an array
   *     of objects describing the results.
   */
  public function getDbxref(array $identifiers, array $options) {

  }

  /**
   * Generates a URL for a database reference (e.g. the reference for a cvterm).
   *
   * If the URL and URL prefix are provided for the database record of a cvterm
   * then a URL can be created for the term.  By default, the db.name and
   * dbxref.accession are concatenated and appended to the end of the
   * db.urlprefix. But Tripal supports the use of {db} and {accession} tokens
   * when if present in the db.urlprefix string will be replaced with the db.name
   * and dbxref.accession respectively.
   *
   * @param $dbxref
   *   A dbxref object retrieved by getDbxref().
   * @param $options
   *   None supported yet. Here for consistency.
   * @return
   *   A string containing the URL.
   */
  public function getDbxrefUrl(object $dbxref, array $options) {

  }

  /**
   * Adds a new database to the Chado DB table and returns the DB object.
   *
   * @param $values
   *   An associative array of the values of the db (those to be inserted):
   *   - name: The name of the database. This name is usually used as the prefix
   *     for CV term accessions.
   *   - description: (Optional) A description of the database.  By default no
   *     description is required.
   *   - url: (Optional) The URL for the database.
   *   - urlprefix: (Optional) The URL that is to be used as a prefix when
   *     constructing a link to a database term.
   * @param $options
   *   None supported yet. Here for consistency.
   * @return
   *   An object populated with fields from the newly added database.  If the
   *   database already exists it returns the values in the current entry.
   */
  public function insertDb(array $values, array $options) {

  }

  /**
   * Add a database reference.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *    - db_id: the database_id of the database the reference is from.
   *    - accession: the accession.
   *    - version: (Optional) The version of the database reference.
   *    - description: (Optional) A description of the database reference.
   * @param $options
   *   Currently no options are supported.
   *   This is here for consistency throughout the API.
   *
   * @return
   *   The newly inserted dbxref as an object, similar to that returned by
   *   the chado_select_record() function.
   */
  public function insertDbxref(array $values, array $options) {

  }

  /**
   * Update chado.db record.
   */
  public function updateDb() {

  }

  /**
   * Update chado.dbxref record.
   */
  public function updateDbxref() {

  }

  /**
   * Insert chado.db record if it doesn't yet exist OR update if does.
   */
  public function upsertDb() {

  }

  /**
   * Insert chado.dbxref record if it doesn't yet exist OR update if does.
   */
  public function upsertDbxref() {

  }

  /**
   * Add a record to a database reference linking table (ie: feature_dbxref).
   *
   * @param $basetable
   *   The base table for which the dbxref should be associated. Thus to associate
   *   a dbxref with a feature the basetable=feature and dbxref_id is added to the
   *   feature_dbxref table.
   * @param $record_id
   *   The primary key of the basetable to associate the dbxref with. This should
   *   be in integer.
   * @param $dbxref
   *   An associative array describing the dbxref. Valid keys include:
   *   'accession' => the accession for the dbxref, 'db_name' => the name of the
   *    database the dbxref belongs to.
   *   'db_id' => the primary key of the database the dbxref belongs to.
   * @param $options
   *   An associative array of options. Valid keys include:
   *    - insert_dbxref: Insert the dbxref if it doesn't already exist. TRUE is
   *      the default.
   */
  public function chado_associate_dbxref($basetable, $record_id, $dbxref, $options = []) {

  }
}
