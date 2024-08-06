<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
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
   * Retrieves a chado database record.
   *
   * @param array $conditions
   *   An array where the key is a chado table name+dot+column name.
   *   Valid keys include:
   *     - db.db_id
   *     - db.name
   *     - db.description
   *     - db.urlprefix
   *     - db.url
   * @param array $options (Optional)
   *   Associative array of options.
   *     - 'case_insensitive' - a single key, or an array of keys
   *                            to query case insensitively.
   *
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function getDb(array $conditions, array $options = []) {
    $valid_tables = ['db'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->connection->select('1:db', 'db');
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    $this->addConditions($query, $conditions, $options);

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy GetDb database error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable('db');
      foreach ($values as $key => $value) {
        $new_record->setValue($this->unmakeAlias($key), $value);
      }
      $buddies[] = $new_record;
    }

    if (count($buddies) > 1) {
      return $buddies;
    }
    elseif (count($buddies) == 1) {
      return $buddies[0];
    }
    else {
      return FALSE;
    }
  }

  /**
   * Retrieves a chado database reference.
   *
   * @param array $conditions
   *   An array where the key is a column in chado and the value describes the
   *   dbxref you want to select. Valid keys include:
   *     - dbxref.dbxref_id
   *     - dbxref.db_id
   *     - dbxref.description
   *     - dbxref.accession
   *     - dbxref.version
   *     - db.db_id
   *     - db.name
   *     - db.description
   *     - db.urlprefix
   *     - db.url
   * @param array $options (Optional)
   *   Associative array of options.
   *     - 'case_insensitive' - a single key, or an array of keys
   *                            to query case insensitively.
   *
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function getDbxref(array $conditions, array $options = []) {
    $valid_tables = ['db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->connection->select('1:dbxref', 'dbxref');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }

    $query->leftJoin('1:db', 'db', 'dbxref.db_id = db.db_id');
    $this->addConditions($query, $conditions, $options);

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getDbxref database error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable('dbxref');
      foreach ($values as $key => $value) {
        $new_record->setValue($this->unmakeAlias($key), $value);
      }
      $buddies[] = $new_record;
    }

    if (count($buddies) > 1) {
      return $buddies;
    }
    elseif (count($buddies) == 1) {
      return $buddies[0];
    }
    else {
      return FALSE;
    }
  }

  /**
   * Generates a URL for a database reference (e.g. the reference for a cvterm).
   *
   * If the URL prefix is provided for the database record of a cvterm,
   * then a URL can be created for the term. By default, the db name and
   * dbxref accession are concatenated and appended to the end of the
   * urlprefix. But Tripal supports the use of {db} and {accession} tokens
   * in the db.urlprefix string. If present, they will be replaced with the
   * db name and dbxref accession, respectively.
   *
   * @param ChadoBuddyRecord $dbxref
   *   A dbxref object retrieved by getDbxref().
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return string
   *   A string containing the URL. If this database doesn't have a URL prefix,
   *   then the built in version for your Tripal site will be used.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   *
   * @todo the built in page for cv/lookup is not yet implemented for Tripal 4
   */
  public function getDbxrefUrl(ChadoBuddyRecord $dbxref, array $options = []) {
// almost the same as getUrl() in tripal/src/TripalVocabTerms/TripalTerm.php
    $db = $dbxref->getValue('db.name');
    $accession = $dbxref->getValue('dbxref.accession');
    $urlprefix = $dbxref->getValue('db.urlprefix');
    if (!$urlprefix) {
      $urlprefix = 'cv/lookup/{db}/{accession}';
    }

    $url = $urlprefix;
    $substituted = FALSE;
    if (preg_match('/\{db\}/', $url)) {
      $url = preg_replace('/\{db\}/', $db, $url);
      $substituted = TRUE;
    }
    if (preg_match('/\{accession\}/', $url)) {
      $url = preg_replace('/\{accession\}/', $accession, $url);
      $substituted = TRUE;
    }
    if (!$substituted) {
      $url .= $db . ':' . $accession;
    }
    return $url;
  }

  /**
   * Adds a new database to the Chado DB table and returns the DB object.
   *
   * @param $values
   *   An associative array of the values of the db (those to be inserted):
   *     - db.db_id: not valid for an insert.
   *     - db.name: The name of the database. This name is usually used as the prefix
   *       for CV term accessions.
   *     - db.description: (Optional) A description of the database. By default no
   *       description is required.
   *     - db.url: (Optional) The URL for the database.
   *     - db.urlprefix: (Optional) The URL that is to be used as a prefix when
   *       constructing a link to a database term.
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown... if this is not the desired
   *   behaviour then use the upsert version of this method.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function insertDb(array $values, array $options = []) {
    $valid_tables = ['db'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    try {
      $query = $this->connection->insert('1:db');
      $query->fields($this->removeTablePrefix($values));
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertDb database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getDb($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Add a database cross reference.
   * The database must already exist. If necessary first create it with insertDb().
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - dbxref.dbxref_id: not valid for an insert.
   *     - dbxref.db_id or db.db_id: (Required) the database_id of the database the reference is to.
   *     - dbxref.description: (Optional) description.
   *     - dbxref.accession: (Requried) the accession.
   *     - dbxref.version: (Optional) The version of the database reference.
   *     - db.name: may be used in place of db.db_id or dbxref.db_id if that is not available.
   *     - db.description: valid, but has no effect for this function.
   *     - db.urlprefix: valid, but has no effect for this function.
   *     - db.url: valid, but has no effect for this function.
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown... if this is not the desired
   *   behaviour then use the upsert version of this method.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function insertDbxref(array $values, array $options = []) {
    $valid_tables = ['db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // Can use db.db_id in place of dbxref.db_id
    if (array_key_exists('db.db_id', $values) and !array_key_exists('dbxref.db_id', $values)) {
      $values['dbxref.db_id'] = $values['db.db_id'];
      unset($values['db.db_id']);
    }

    // If db.name specified, but not db.db_id or dbxref.db_id, then lookup db.db_id
    if (!array_key_exists('dbxref.db_id', $values) or !$values['dbxref.db_id']) {
      if (!array_key_exists('db.name', $values) or !$values['db.name']) {
        throw new ChadoBuddyException("ChadoBuddy insertDbxref error, neither db.db_id, dbxref.db_id, nor db.name were specified\n");
      }
      $existing_record = $this->getDb(['db.name' => $values['db.name']], $options);
      if (!$existing_record or is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy insertDbxref error, invalid db.name \"".$values['db.name']."\" was specified\n");
      }
      $values['dbxref.db_id'] = $existing_record->getValue('db.db_id');
      unset($values['db.name']);
    }

    try {
      $query = $this->connection->insert('1:dbxref');
      // Create a subset of the passed $values for just the dbxref table.
      $dbxref_values = $this->subsetInput($values, ['dbxref']);
      $query->fields($this->removeTablePrefix($dbxref_values));
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertDbxref database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getDbxref($dbxref_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Updates an existing database.
   * Generally use either the db.db_id or db.name in the $conditions array
   * to select the existing record. db.db_id in the $values array will be ignored.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - db.db_id: primary key for db table.
   *     - db.name: The name of the database. This name is usually used as the prefix
   *       for CV term accessions.
   *     - db.description: (Optional) A description of the database. By default no
   *       description is required.
   *     - db.url: (Optional) The URL for the database.
   *     - db.urlprefix: (Optional) The URL that is to be used as a prefix when
   *       constructing a link to a database term.
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for the $values.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function updateDb(array $values, array $conditions, array $options = []) {
    $valid_tables = ['db'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);
    $this->validateInput($values, $valid_columns);

    $existing_record = $this->getDb($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateDb error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Update query will only be based on the db.db_id, which we
    // can get from the retrieved record.
    $db_id = $existing_record->getValue('db.db_id');
    // We do not support changing the db_id.
    if (array_key_exists('db.db_id', $values)) {
      unset($values['db.db_id']);
    }
    $query = $this->connection->update('1:db');
    $query->condition('db_id', $db_id, '=');
    $query->fields($this->removeTablePrefix($values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateDb database error '.$e->getMessage());
    }
    $existing_record = $this->getDb($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Updates an existing database reference.
   *
   * Generally use either dbxref.dbxref_id or a combination of dbxref.db_id and
   * dbxref.accession in the $conditions to select the existing record.
   * dbxref.dbxref_id in the $values array will be ignored.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - dbxref.dbxref_id: the primary key of the dbxref table.
   *     - dbxref.db_id or db.db_id: The database_id of the database the reference is to.
   *     - dbxref.description: Description.
   *     - dbxref.accession: The accession.
   *     - dbxref.version: The version of the database reference.
   *     - db.name: may be used in place of db.db_id or dbxref.db_id if that is not available.
   *     - db.description: valid, but has no effect for this function.
   *     - db.urlprefix: valid, but has no effect for this function.
   *     - db.url: valid, but has no effect for this function.
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for the $values.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function updateDbxref(array $values, array $conditions, array $options = []) {
    $valid_tables = ['db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_record = $this->getDbxref($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateDbxref error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }

    // Update query will only be based on the dbxref_id, which we
    // can get from the retrieved record.
    $dbxref_id = $existing_record->getValue('dbxref.dbxref_id');
    // We do not support changing the dbxref_id.
    if (array_key_exists('dbxref.dbxref_id', $values)) {
      unset($values['dbxref.dbxref_id']);
    }

    $query = $this->connection->update('1:dbxref');
    $query->condition('dbxref_id', $dbxref_id, '=');
    // Create a subset of the passed $values for just the dbxref table.
    $dbxref_values = $this->subsetInput($values, ['dbxref']);
    $query->fields($this->removeTablePrefix($dbxref_values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateDbxref database error '.$e->getMessage());
    }
    $existing_record = $this->getDbxref($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Insert a database if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - db.db_id: primary key for db table.
   *     - db.name: The name of the database. This name is usually used as the prefix
   *       for CV term accessions.
   *     - db.description: (Optional) A description of the database. By default no
   *       description is required.
   *     - db.url: (Optional) The URL for the database.
   *     - db.urlprefix: (Optional) The URL that is to be used as a prefix when
   *       constructing a link to a database term.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function upsertDb(array $values, array $options = []) {
    $valid_tables = ['db'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_record = $this->getDb($conditions, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertDb error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $new_record = $this->updateDb($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertDb($values, $options);
    }
    return $new_record;
  }

  /**
   * Insert a database reference if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - dbxref.dbxref_id: the primary key of the dbxref table.
   *     - dbxref.db_id or db.db_id: The database_id of the database the reference is to.
   *     - dbxref.description: Description.
   *     - dbxref.accession: The accession.
   *     - dbxref.version: The version of the database reference.
   *     - db.name: may be used in place of db.db_id or dbxref.db_id if that is not available.
   *     - db.description: valid, but has no effect for this function.
   *     - db.urlprefix: valid, but has no effect for this function.
   *     - db.url: valid, but has no effect for this function.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function upsertDbxref(array $values, array $options = []) {
    $valid_tables = ['db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_record = $this->getDbxref($conditions, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertDbxref error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $new_record = $this->updateDbxref($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertDbxref($values, $options);
    }
    return $new_record;
  }

  /**
   * Add a record to a database reference linking table (ie: feature_dbxref).
   *
   * @param string $base_table
   *   The base table for which the dbxref should be associated. Thus to associate
   *   a dbxref with a feature the basetable=feature and dbxref_id is added to the
   *   feature_dbxref table.
   * @param int $record_id
   *   The primary key of the basetable to associate the dbxref with.
   * @param ChadoBuddyRecord $dbxref
   *   A dbxref object returned by any of the *Dbxref() in this service.
   * @param $options
   *   'pkey': Looking up the primary key for the base table is costly. If it is
   *           known, then pass it in as this option for better performance.
   *   Also pass in any other columns used in the linking table, some of which may
   *   have a NOT NULL constraint.
   *
   * @return bool
   *   Returns true if successful.
   *   Both the dbxref and the chado record indicated by $record_id MUST ALREADY EXIST.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function associateDbxref(string $base_table, int $record_id, ChadoBuddyRecord $dbxref, array $options = []) {
    $linking_table = $base_table . '_dbxref';

    // Get the primary key of the base table
    $base_pkey_col = $options['pkey'] ?? NULL;
    if (!$base_pkey_col) {
      $schema = $this->connection->schema();
      $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
      $base_pkey_col = $base_table_def['primary key'];
    }

    $fields = [
      'dbxref_id' => $dbxref->getValue('dbxref.dbxref_id'),
      $base_pkey_col => $record_id,
    ];
    // Add in any of the other columns for the linking table.
    foreach ($options as $key => $value) {
      if ($key != 'pkey') {
        $fields[$key] = $value;
      }
    }
    try {
      $query = $this->connection->insert('1:'.$linking_table);
      $query->fields($this->removeTablePrefix($fields));
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy associateDbxref database error '.$e->getMessage());
    }

    return TRUE;
  }

}
