<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;

/**
 * @ChadoBuddy(
 *   id = "chado_dbxref_buddy",
 *   label = @Translation("Chado Database Reference Buddy"),
 *   description = @Translation("Provides helper methods for managing chado dbs and dbxrefs.")
 * )
 */
class ChadoDbxrefBuddy extends ChadoBuddyPluginBase {

  /**
   * Retrieves a chado database.
   *
   * @param array $identifiers
   *   An array where the key is a column in the chado.db table and the value
   *   describes the db you want to select. Valid keys include:
   *     - db_id
   *     - name
   *     - description
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE and if an error is
   *     encountered then an exception will be thrown.
   */
  public function getDb(array $identifiers, array $options = []) {

  }

  /**
   * Retrieves a chado database reference.
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
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE and if an error is
   *     encountered then an exception will be thrown.
   */
  public function getDbxref(array $identifiers, array $options = []) {

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
   * @param ChadoBuddyRecord $dbxref
   *   A dbxref object retrieved by getDbxref().
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return string
   *   A string containing the URL. If this database doesn't have a URL prefix,
   *   then the built in version for your Tripal site will be used. An exception
   *   is thrown if an error is encountered.
   */
  public function getDbxrefUrl(ChadoBuddyRecord $dbxref, array $options = []) {

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
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown... if this is not the desired
   *   behaviour then use the upsert version of this method.
   */
  public function insertDb(array $values, array $options = []) {

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
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown... if this is not the desired
   *   behaviour then use the upsert version of this method.
   */
  public function insertDbxref(array $values, array $options = []) {

  }

  /**
   * Updates an existing database.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *   - name: The name of the database. This name is usually used as the prefix
   *     for CV term accessions.
   *   - description: (Optional) A description of the database.  By default no
   *     description is required.
   *   - url: (Optional) The URL for the database.
   *   - urlprefix: (Optional) The URL that is to be used as a prefix when
   *     constructing a link to a database term.
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for the $values.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update and an exception will be thrown
   *   if an error is encountered.
   */
  public function updateDb(array $values, array $conditions, array $options = []) {

  }

  /**
   * Updates an existing database reference.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *    - db_id: the database_id of the database the reference is from.
   *    - accession: the accession.
   *    - version: (Optional) The version of the database reference.
   *    - description: (Optional) A description of the database reference.
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for the $values.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update and an exception will be thrown
   *   if an error is encountered.
   */
  public function updateDbxref() {

  }

  /**
   * Insert a database if it doesn't yet exist OR update it if does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *   - name: The name of the database. This name is usually used as the prefix
   *     for CV term accessions.
   *   - description: (Optional) A description of the database.  By default no
   *     description is required.
   *   - url: (Optional) The URL for the database.
   *   - urlprefix: (Optional) The URL that is to be used as a prefix when
   *     constructing a link to a database term.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success, and
   *   an exception will be thrown if an error is encountered.
   */
  public function upsertDb(array $values, array $options = []) {

  }

  /**
   * Insert a database reference if it doesn't yet exist OR update it if does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *    - db_id: the database_id of the database the reference is from.
   *    - accession: the accession.
   *    - version: (Optional) The version of the database reference.
   *    - description: (Optional) A description of the database reference.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success, and
   *   an exception will be thrown if an error is encountered.
   */
  public function upsertDbxref(array $values, array $options = []) {

  }

  /**
   * Add a record to a database reference linking table (ie: feature_dbxref).
   *
   * @param string $basetable
   *   The base table for which the dbxref should be associated. Thus to associate
   *   a dbxref with a feature the basetable=feature and dbxref_id is added to the
   *   feature_dbxref table.
   * @param int $record_id
   *   The primary key of the basetable to associate the dbxref with.
   * @param ChadoBuddyRecord $dbxref
   *   A dbxref object returned by any of the *Dbxref() in this service.
   * @param $options
   *   None supported yet. Here for consistency.
   * @return bool
   *   Returns true if successful, and throws an exception if an error is
   *   encountered. Both the dbxref and the chado record indicated by $record_id
   *   MUST ALREADY EXIST.
   */
  public function associateDbxref(string $basetable, int $record_id, ChadoBuddyRecord $dbxref, array $options = []) {

  }
}
