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
   * Keys are the column aliases, and values are the
   * table aliases and columns for the dbxref buddy.
   * @var array
   *
   */
  protected array $db_mapping = [
    'db_id' => 'db.db_id',
    'db_name' => 'db.name',
    'db_description' => 'db.description',
    'urlprefix' => 'db.urlprefix',
    'url' => 'db.url',
  ];

  /**
   * Keys are the column aliases, and values are the
   * table aliases and columns for the dbxref buddy.
   * @var array
   *
   */
  protected array $dbxref_mapping = [
    'dbxref_id' => 'x.dbxref_id',
    'accession' => 'x.accession',
    'version' => 'x.version',
    'dbxref_description' => 'x.description',
    'db_id' => 'db.db_id',
    'db_name' => 'db.name',
    'db_description' => 'db.description',
    'urlprefix' => 'db.urlprefix',
    'url' => 'db.url',
  ];

  /**
   * Whether a column value is required for the dbxref table.
   * For performance reasons this is pre-populated.
   * @var array
   *
   */
  protected array $dbxref_required = [
    'db_id' => TRUE,
    'accession' => TRUE,
    'version' => FALSE,
    'description' => FALSE,
  ];

  /**
   * Retrieves a chado database.
   *
   * @param array $identifiers
   *   An array where the key is a column in the chado.db table and the value
   *   describes the db you want to select. Valid keys include:
   *     - db_id
   *     - db_name
   *     - db_description
   *     - urlprefix
   *     - url
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE and if an error is
   *     encountered then a ChadoBuddyException will be thrown.
   */
  public function getDb(array $identifiers, array $options = []) {
    $this->validateInput($identifiers, $this->db_mapping, 'getDb');

    $query = $this->connection->select('1:db', 'db');
    foreach ($identifiers as $key => $value) {
      $mapping = $this->db_mapping[$key];
      $parts = explode('.', $mapping);
      $query->condition($parts[0].'.'.$parts[1], $value, '=');
    }
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($this->db_mapping as $key => $mapping) {
      $parts = explode('.', $mapping);
      $query->addField($parts[0], $parts[1], $key);
    }
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy GetDb error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setValues($values);
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
   * @param array $identifiers
   *   An array where the key is a column in chado and the value describes the
   *   dbxref you want to select. Valid keys include:
   *     - dbxref_id
   *     - accession
   *     - version
   *     - dbxref_description
   *     - db_id
   *     - db_name
   *     - db_description
   *     - urlprefix
   *     - url
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE and if an error is
   *     encountered then a ChadoBuddyException will be thrown.
   */
  public function getDbxref(array $identifiers, array $options = []) {
    $this->validateInput($identifiers, $this->dbxref_mapping, 'getDbxref');

    $query = $this->connection->select('1:dbxref', 'x');
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($this->dbxref_mapping as $key => $mapping) {
      $parts = explode('.', $mapping);
      $query->addField($parts[0], $parts[1], $key);
    }
    $query->leftJoin('1:db', 'db', 'x.db_id = db.db_id');
    foreach ($identifiers as $key => $value) {
      $query->condition($this->dbxref_mapping[$key], $value, '=');
    }
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getDbxref error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setValues($values);
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
   *   A ChadoBuddyException is thrown if an error is encountered.
   *
   *   @to-do the built in page for cv/lookup is not yet implemented for Tripal 4
   */
  public function getDbxrefUrl(ChadoBuddyRecord $dbxref, array $options = []) {
// almost the same as getUrl() in tripal/src/TripalVocabTerms/TripalTerm.php
    $db = $dbxref->getValue('db_name');
    $accession = $dbxref->getValue('accession');
    $urlprefix = $dbxref->getValue('urlprefix');
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
    $this->validateInput($values, $this->db_mapping, 'insertDb');

    try {
      $query = $this->connection->insert('1:db');
      foreach ($values as $key => $value) {
        $mapping = $this->db_mapping[$key];
        $parts = explode('.', $mapping);
        $query->addField($parts[0], $parts[1], $value);
      }
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertDb error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getDb($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy insertDb error, did not retrieve the record just added\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy insertDb error, more than one record matched the record just added\n".print_r($values, TRUE));
    }

    return $existing_record;
  }

  /**
   * Add a database reference.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *    - db_id: the database_id of the database the reference is from.
   *    - db_name: may be used in place of db_id if that is not available.
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
    $this->validateInput($values, $this->dbxref_mapping, 'insertDbxref');

    // If db_name specified, but not db_id, lookup db_id
    if (!array_key_exists('db_id', $values) or !$values['db_id']) {
      if (!array_key_exists('db_name', $values) or !$values['db_name']) {
        throw new ChadoBuddyException("ChadoBuddy insertDbxref error, neither db_id nor db_name were specified\n");
      }
      $existing_record = $this->getDb(['db_name' => $values['db_name']]);
      if (!$existing_record or is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy insertDbxref error, invalid db_name \"$db_name\" was specified\n");
      }
      $values['db_id'] = $existing_record->getValue('db_id');
      unset($values['db_name']);
    }

    try {
      $query = $this->connection->insert('1:dbxref');
      // Create a subset of the passed $values for just the dbxref table.
      $dbxref_values = $this->validateInput($values, $this->db_mapping, 'insertDbxref', TRUE);
      $query->fields($dbxref_values);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertDbxref error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getDbxref($dbxref_values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy insertDbxref error, did not retrieve the record just added\n".print_r($term_values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy insertDbxref error, more than one record matched the record just added\n".print_r($term_values, TRUE));
    }

    return $existing_record;
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
   *   returned if no record was found to update and a ChadoBuddyException will be thrown
   *   if an error is encountered.
   */
  public function updateDb(array $values, array $conditions, array $options = []) {
    $this->validateInput($values, $this->db_mapping, 'updateDb');
    $this->validateInput($conditions, $this->db_mapping, 'updateDb');
    $existing_record = $this->getDb($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateDb error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Update query will only be based on the db_id, which we get from the retrieved record.
    $db_id = $existing_record->getValue('db_id');
    // We do not support changing the db_id.
    if (array_key_exists('db_id', $values)) {
      unset($values['db_id']);
    }
    $query = $this->connection->update('1:db');
    $query->condition('db_id', $db_id, '=');
    $query->fields($values);
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateDb error '.$e->getMessage());
    }
    $existing_record = $this->getDb($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy updateDb error, did not retrieve the record just updated\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateDb error, more than one record matched the record just updated\n".print_r($values, TRUE));
    }

    return $existing_record;
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
   *   returned if no record was found to update and a ChadoBuddyException will be thrown
   *   if an error is encountered.
   */
  public function updateDbxref(array $values, array $conditions, array $options = []) {
    $this->validateInput($values, $this->dbxref_mapping, 'updateDbxref');
    $this->validateInput($conditions, $this->dbxref_mapping, 'updateDbxref');

    $existing_record = $this->getDbxref($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateDbxref error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }

    // Update query will only be based on the cvterm_id, which we get from the retrieved record.
    $dbxref_id = $existing_record->getValue('dbxref_id');
    // We do not support changing the dbxref_id.
    if (array_key_exists('dbxref_id', $values)) {
      unset($values['dbxref_id']);
    }
    // Create a subset of the passed $values for just the dbxref table.
    $term_values = $this->validateInput($values, $this->db_mapping, 'updateDbxref', TRUE);
    $query = $this->connection->update('1:dbxref');
    $query->condition('dbxref_id', $dbxref_id, '=');
    $query->fields($term_values);
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateDbxref error '.$e->getMessage());
    }
    $existing_record = $this->getDbxref($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy updateDbxref error, did not retrieve the record just updated\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateDbxref error, more than one record matched the record just updated\n".print_r($values, TRUE));
    }

    return $existing_record;
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
   *   a ChadoBuddyException will be thrown if an error is encountered.
   */
  public function upsertDb(array $values, array $options = []) {
    $this->validateInput($values, $this->db_mapping, 'upsertDb');
    $existing_record = $this->getDb($values, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertDb error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $conditions = ['db_id' => $existing_record->getValue('db_id')];
      $new_record = $this->updateDb($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertDb($values, $options);
    }
    return $new_record;
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
   *   a ChadoBuddyException will be thrown if an error is encountered.
   */
  public function upsertDbxref(array $values, array $options = []) {
    $this->validateInput($values, $this->dbxref_mapping, 'upsertDbxref');
    $existing_record = $this->getDbxref($values, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertDbxref error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $conditions = ['dbxref_id' => $existing_record->getValue('dbxref_id')];
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
   *   Returns true if successful, and throws a ChadoBuddyException if an error is
   *   encountered. Both the dbxref and the chado record indicated by $record_id
   *   MUST ALREADY EXIST.
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
      'dbxref_id' => $dbxref->getValue('dbxref_id'),
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
      $query->fields($fields);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy associateDbxref error '.$e->getMessage());
    }

    return TRUE;
  }

  protected function validateInput(array $uservalues, array $validvalues, string $function_name, bool $return = FALSE) {
//    if (!$uservalues) {
//      throw new ChadoBuddyException("ChadoBuddy $function_name error, no $mode values were specified\n");
//    }
    $subset = [];
    foreach ($uservalues as $key => $value) {
      if (!array_key_exists($key, $validvalues)) {
        if (!$return) {
          throw new ChadoBuddyException("ChadoBuddy $function_name error, value \"$key\" is not valid for for this function\n");
        }
      }
      else {
        $subset[] = [$key => $value];
      }
    }
    if (!$subset) {
      throw new ChadoBuddyException("ChadoBuddy $function_name error, no valid values were specified\n");
    }
    return $subset;
  }
}
