<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;

/**
 * Plugin implementation of the chado_buddy.
 *
 * @ChadoBuddy(
 *   id = "chado_cvterm_buddy",
 *   label = @Translation("Chado Controlled Vocabulary Term Buddy"),
 *   description = @Translation("Provides helper methods for managing chado cvs and cvterms.")
 * )
 */
class ChadoCvtermBuddy extends ChadoBuddyPluginBase {

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
    'db_id' => 'x.db_id',
    'accession' => 'x.accession',
    'version' => 'x.version',
    'dbxref_description' => 'x.description',
  ];

  /**
   * Keys are the column aliases, and values are the
   * table aliases and columns for the cvterm buddy.
   * @var array
   *
   */
  protected array $cv_mapping = [
    'cv_id' => 'cv.cv_id',
    'cv_name' => 'cv.name',
    'cv_definition' => 'cv.definition',
  ];

  /**
   * Keys are the column aliases, and values are the
   * table aliases and columns for the cvterm buddy.
   * @var array
   *
   */
  protected array $cvterm_mapping = [
    'cvterm_id' => 't.cvterm_id',
    'cv_id' => 't.cv_id',
    'dbxref_id' => 't.dbxref_id',
    'cvterm_name' => 't.name',
    'cvterm_definition' => 't.definition',
    'is_obsolete' => 't.is_obsolete',
    'is_relationshiptype' => 't.is_relationshiptype',
  ];

  /**
   * Whether a column value is required for the cvterm table.
   * For performance reasons this is pre-populated.
   * @var array
   *
   */
  protected array $cvterm_required = [
    'cv_id' => TRUE,
    'dbxref_id' => TRUE,
    'cvterm_name' => TRUE,
    'cvterm_definition' => FALSE,
    'is_obsolete' => FALSE,
    'is_relationshiptype' => FALSE,
  ];

  /**
   * Cache the dbxref instance here
   */
  protected object $dbxref_instance;

  /**
   * Retrieves a controlled vocabulary.
   *
   * @param array $identifiers
   *   An array where the key is a column in chado and the value describes the
   *   cv you want to select. Valid keys include:
   *     - cv_id
   *     - cv_name
   *     - cv_definition
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
  public function getCv(array $identifiers, array $options = []) {
    $this->validateInput($identifiers, $this->cv_mapping);

    $query = $this->connection->select('1:cv', 'cv');

    foreach ($identifiers as $key => $value) {
      $mapping = $this->cv_mapping[$key];
      $parts = explode('.', $mapping);
      $query->condition($parts[0].'.'.$parts[1], $value, '=');
    }
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($this->cv_mapping as $key => $map) {
      $parts = explode('.', $map);
      $query->addField($parts[0], $parts[1], $key);
    }
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getCv error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
//      Not public variables so this won't work currently:
//      $new_record->schema_name = $this->connection->getSchemaName();
//      $new_record->base_table = 'cv';
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
   * Retrieves a controlled vocabulary term.
   *
   * @param array $identifiers
   *   An array where the key is a column in chado and the value describes the
   *   cvterm you want to select. Valid keys include:
   *     - cvterm_id
   *     - cv_id
   *     - cv_name
   *     - cv_name
   *     - cvterm_name
   *     - cv_definition
   *     - cvterm_definition
   *     - dbxref_id
   *     - accession
   *     - db_name
   *     - is_obsolete
   *     - is_relationshiptype
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
  public function getCvterm(array $identifiers, array $options = []) {
    $mapping = array_merge($this->db_mapping, $this->dbxref_mapping, $this->cv_mapping, $this->cvterm_mapping);
    $this->validateInput($identifiers, $mapping);

    $query = $this->connection->select('1:cvterm', 't');
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($mapping as $key => $map) {
      $parts = explode('.', $map);
      $query->addField($parts[0], $parts[1], $key);
    }
    $query->leftJoin('1:cv', 'cv', 't.cv_id = cv.cv_id');
    $query->leftJoin('1:dbxref', 'x', 't.dbxref_id = x.dbxref_id');
    $query->leftJoin('1:db', 'db', 'x.db_id = db.db_id');
    foreach ($identifiers as $key => $value) {
      $query->condition($mapping[$key], $value, '=');
    }
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getCvterm error '.$e->getMessage());
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
   * Add a controlled vocabulary.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - cv_id
   *     - name
   *     - definition
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown. If this is not the desired
   *   behaviour then use the upsert version of this method.
   */
  public function insertCv(array $values, array $options = []) {
    $fields = $this->validateInput($values, $this->cv_mapping);

    try {
      $query = $this->connection->insert('1:cv');
      $query->fields($fields);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCv error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getCv($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy insertCv error, did not retrieve the record just added\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy insertCv error, more than one record matched the record just added\n".print_r($values, TRUE));
    }

    return $existing_record;
  }

  /**
   * Add a controlled vocabulary term, including creating a
   * dbxref entry if necessary.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - cv_id
   *     - cv_name (either cv_id or cv_name required)
   *     - name (required)
   *     - definition
   *     - dbxref_id
   *     - term_accession (required unless dbxref_id specified)
   *     - term_idspace (required unless dbxref_id specified)
   *     - db_id (can be used in place of term_idspace)
   *     - is_obsolete
   *     - is_relationshiptype
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown. If this is not the desired
   *   behaviour then use the upsert version of this method.
   */
  public function insertCvterm(array $values, array $options = []) {
    $mapping = array_merge($this->db_mapping, $this->dbxref_mapping, $this->cv_mapping, $this->cvterm_mapping);
    $this->validateInput($values, $mapping);

    // If cv_name specified but not cv_id, lookup cv_id
    if (!array_key_exists('cv_id', $values) or !$values['cv_id']) {
      if (!array_key_exists('cv_name', $values) or !$values['cv_name']) {
        throw new ChadoBuddyException("ChadoBuddy insertCvterm error, neither cv_id nor cv_name were specified\n");
      }
      $existing_record = $this->getCv(['cv_name' => $values['cv_name']]);
      if (!$existing_record or is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy insertCvterm error, invalid cv_name \"$cv_name\" was specified\n");
      }
      $values['cv_id'] = $existing_record->getValue('cv_id');
    }
    unset($values['cv_name']);

    // Insert a new dbxref if an existing one was not specified.
    if (!array_key_exists('dbxref_id', $values) or !$values['dbxref_id']) {
      $dbxref_record = $this->upsertDbxref($values, $values, $options);
      $values['dbxref_id'] = $dbxref_record->getValue('dbxref_id');
    }

    // Insert cvterm
    try {
      $query = $this->connection->insert('1:cvterm');
      // Create a subset of the passed $values for just the cvterm table.
      $cvterm_values = $this->validateInput($values, $this->cvterm_mapping, TRUE);
      $query->fields($cvterm_values);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCvterm error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getCvterm($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy insertCvterm error, did not retrieve the record just added\n".print_r($term_values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy insertCvterm error, more than one record matched the record just added\n".print_r($term_values, TRUE));
    }

    return $existing_record;
  }

  /**
   * Updates an existing controlled vocabulary.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - cv_id (only used for $conditions)
   *     - name
   *     - definition
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for the $values.
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update and a ChadoBuddyException will
   *   be thrown if an error is encountered.
   */
  public function updateCv(array $values, array $conditions, array $options = []) {
    $unaliased_values = $this->validateInput($values, $this->cv_mapping);
    $this->validateInput($conditions, $this->cv_mapping);
    $existing_record = $this->getCv($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Update query will only be based on the cv_id, which we get from the retrieved record.
    $cv_id = $existing_record->getValue('cv_id');
    // We do not support changing the cv_id.
    if (array_key_exists('cv_id', $unaliased_values)) {
      unset($unaliased_values['cv_id']);
    }
    $query = $this->connection->update('1:cv');
    $query->condition('cv_id', $cv_id, '=');
    $query->fields($unaliased_values);
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateCv error '.$e->getMessage());
    }
    $existing_record = $this->getCv($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, did not retrieve the record just updated\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, more than one record matched the record just updated\n".print_r($values, TRUE));
    }

    return $existing_record;
  }

  /**
   * Updates an existing controlled vocabulary term.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - cvterm_id
   *     - cv_id
   *     - cv_name
   *     - name
   *     - definition
   *     - dbxref_id
   *     - term_accession
   *     - term_idspace
   *     - is_obsolete
   *     - is_relationshiptype
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
  public function updateCvterm(array $values, array $conditions, array $options = []) {
    $mapping = array_merge($this->db_mapping, $this->dbxref_mapping, $this->cv_mapping, $this->cvterm_mapping);
    $this->validateInput($values, $mapping);
    $this->validateInput($conditions, $mapping);

    $existing_record = $this->getCvterm($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Only update the dbxref if it is being changed.
    $existing_values = $existing_record->getValues();
    $update_dbxref = FALSE;
    $check_fields = ['db_name', 'db_id', 'accession', 'version', 'dbxref_description'];
    foreach ($check_fields as $field) {
      if (array_key_exists($field, $values) and ($values[$field] != $existing_values[$field])) {
        $update_dbxref = TRUE;
      }
    }
    if ($update_dbxref) {
      $dbxref_record = $this->upsertDbxref($values, $conditions, $options);
    }

    // Update query will only be based on the cvterm_id, which we get from the retrieved record.
    $cvterm_id = $existing_record->getValue('cvterm_id');
    // We do not support changing the cvterm_id.
    if (array_key_exists('cvterm_id', $values)) {
      unset($values['cvterm_id']);
    }
    // Create a subset of the passed $values for just the cvterm table.
    $term_values = $this->validateInput($values, $this->cvterm_mapping, TRUE);
//    foreach ($this->cvterm_required as $key => $required) {
//      // We don't check required columns for an update, only for an insert.
//      if (array_key_exists($key, $values)) {
//        $term_values[$key] = $values[$key];
//      }
//    }
    $query = $this->connection->update('1:cvterm');
    $query->condition('cvterm_id', $cvterm_id, '=');
    $query->fields($term_values);
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateCvterm error '.$e->getMessage());
    }
    $existing_record = $this->getCvterm($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, did not retrieve the record just updated\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, more than one record matched the record just updated\n".print_r($values, TRUE));
    }

    return $existing_record;
  }

  /**
   * Insert a controlled vocabulary if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - cv_id
   *     - name
   *     - definition
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success, and
   *   a ChadoBuddyException will be thrown if an error is encountered.
   */
  public function upsertCv(array $values, array $options = []) {
    $this->validateInput($values, $this->cv_mapping);
    $existing_record = $this->getCv($values, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertCv error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $conditions = ['cv_id' => $existing_record->getValue('cv_id')];
      $new_record = $this->updateCv($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertCv($values, $options);
    }
    return $new_record;
  }

  /**
   * Insert a controlled vocabulary term if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - cvterm_id
   *     - cv_id
   *     - cv_name
   *     - name
   *     - definition
   *     - dbxref_id
   *     - term_accession
   *     - term_idspace
   *     - is_obsolete
   *     - is_relationshiptype
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success, and
   *   a ChadoBuddyException will be thrown if an error is encountered.
   */
  public function upsertCvterm(array $values, array $options = []) {
    $mapping = array_merge($this->db_mapping, $this->dbxref_mapping, $this->cv_mapping, $this->cvterm_mapping);
    $this->validateInput($values, $mapping);
    $existing_record = $this->getCvterm($values, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertCvterm error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $conditions = ['cvterm_id' => $existing_record->getValue('cvterm_id')];
      $new_record = $this->updateCvterm($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertCvterm($values, $options);
    }
    return $new_record;
  }

  /**
   * Add a record to a controlled vocabulary term linking table (e.g. feature_cvterm).
   *
   * @param string $base_table
   *   The base table for which the cvterm should be associated. Thus to associate
   *   a cvterm with a feature the basetable=feature and cvterm_id is added to the
   *   feature_cvterm table.
   * @param int $record_id
   *   The primary key of the basetable to associate the cvterm with.
   * @param ChadoBuddyRecord $cvterm
   *   A cvterm object returned by any of the *Cvterm() in this service.
   * @param $options
   *   'pkey': Looking up the primary key for the base table is costly. If it is
   *           known, then pass it in as this option for better performance.
   *   Also pass in any other columns used in the linking table, some of which may
   *   have a NOT NULL constraint. See the table below for a list of which of
   *   the following may be required: 'pub_id', 'is_not', 'rank', 'cvterm_type_id'.
   *
   *   Chado 1.3 defines these columns in the various linking tables:
   *   ^ table                       ^ pub_id   ^ is_not      ^ rank        ^ cvterm_type_id ^
   *   | analysis_cvterm             | -absent  | has default | has default | -absent        |
   *   | cell_line_cvterm            | not null | -absent     | has default | -absent        |
   *   | environment_cvterm          | -absent  | -absent     | -absent     | -absent        |
   *   | expression_cvterm           | -absent  | -absent     | has default | not null       |
   *   | feature_cvterm              | not null | has default | has default | -absent        |
   *   | library_cvterm              | not null | -absent     | -absent     | -absent        |
   *   | organism_cvterm             | not null | -absent     | has default | -absent        |
   *   | phenotype_comparison_cvterm | not null | -absent     | has default | -absent        |
   *   | phenotype_cvterm            | -absent  | -absent     | has default | -absent        |
   *   | stock_cvterm                | not null | has default | has default | -absent        |
   *   | stock_relationship_cvterm   | yes null | -absent     | -absent     | -absent        |
   *
   * @return bool
   *   Returns TRUE if successful, and throws a ChadoBuddyException if an error is
   *   encountered. Both the cvterm and the chado record indicated by $record_id
   *   MUST ALREADY EXIST.
   */
  public function associateCvterm(string $base_table, int $record_id, ChadoBuddyRecord $cvterm, array $options = []) {
    $linking_table = $base_table . '_cvterm';

    // Get the primary key of the base table
    $base_pkey_col = $options['pkey'] ?? NULL;
    if (!$base_pkey_col) {
      $schema = $this->connection->schema();
      $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
      $base_pkey_col = $base_table_def['primary key'];
    }
    $fields = [
      'cvterm_id' => $cvterm->getValue('cvterm_id'),
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
      throw new ChadoBuddyException('ChadoBuddy associateCvterm error '.$e->getMessage());
    }

    return TRUE;
  }

  /**
   * A helper function to add or update a dbxref for a cvterm, this will filter
   * out extra non-applicable fields that the Cvterm function here may have.
   */
  protected function upsertDbxref(array $values, array $conditions, array $options = []) {
    if (!isset($this->dbxref_instance)) {
      $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
      $this->dbxref_instance = $buddy_service->createInstance('chado_dbxref_buddy', []);
    }

    // Remove fields not valid for the dbxref table
    $mapping = array_merge($this->db_mapping, $this->dbxref_mapping);
    $dbxref_values = [];
    $dbxref_conditions = [];
    foreach ($mapping as $key => $map) {
      if (array_key_exists($key, $values)) {
        $dbxref_values[$key] = $values[$key];
      }
      if (array_key_exists($key, $conditions)) {
        $dbxref_conditions[$key] = $conditions[$key];
      }
    }
    $record = $this->dbxref_instance->upsertDbxref($dbxref_values, $dbxref_conditions, $options);
    return $record;
  }
}
