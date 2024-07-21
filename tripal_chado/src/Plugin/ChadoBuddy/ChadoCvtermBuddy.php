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
   * Keys are the column aliases for each table alias and column
   * for the cvterm buddy
   * @var array
   *
   */
  protected array $cvterm_mapping = [
    'cvterm_id' => 't.cvterm_id',
    'name' => 't.name',
    'definition' => 't.definition',
    'is_obsolete' => 't.is_obsolete',
    'is_relationshiptype' => 't.is_relationshiptype',
    'cv_id' => 'cv.cv_id',
    'cv_name' => 'cv.name',
    'dbxref_id' => 'x.dbxref_id',
    'term_accession' => 'x.accession',
    'term_idspace' => 'db.name',
  ];
  /**
   * Whether a column value is required for the cvterm table.
   * For performance reasons this is pre-populated.
   * @var array
   *
   */
  protected array $cvterm_required = [
    'name' => TRUE,
    'cv_id' => TRUE,
    'dbxref_id' => TRUE,
    'definition' => FALSE,
    'is_obsolete' => FALSE,
    'is_relationshiptype' => FALSE,
  ];

  /**
   * Retrieves a controlled vocabulary.
   *
   * @param array $identifiers
   *   An array where the key is a column in chado and the value describes the
   *   cv you want to select. Valid keys include:
   *     - cv_id
   *     - name
   *     - definition
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
    if (!$identifiers) {
      throw new ChadoBuddyException("ChadoBuddy getCv error, no select values were specified\n");
    }
    $query = $this->connection->select('1:cv', 'cv');
    foreach ($identifiers as $key => $value) {
      $query->condition('cv.'.$key, $value, '=');
    }
    $query->fields('cv', ['cv_id', 'name', 'definition']);
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
   * @return bool|array|ChadoBuddyRecord
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE and if an error is
   *     encountered then a ChadoBuddyException will be thrown.
   */
  public function getCvterm(array $identifiers, array $options = []) {
    if (!$identifiers) {
      throw new ChadoBuddyException("ChadoBuddy getCvterm error, no select values were specified\n");
    }
    $query = $this->connection->select('1:cvterm', 't');
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($this->cvterm_mapping as $key => $mapping) {
      $parts = explode('.', $mapping);
      $query->addField($parts[0], $parts[1], $key);
    }
    $query->leftJoin('1:cv', 'cv', 't.cv_id = cv.cv_id');
    $query->leftJoin('1:dbxref', 'x', 't.dbxref_id = x.dbxref_id');
    $query->leftJoin('1:db', 'db', 'x.db_id = db.db_id');
    foreach ($identifiers as $key => $value) {
      if (array_key_exists($key, $this->cvterm_mapping)) {
        $query->condition($this->cvterm_mapping[$key], $value, '=');
      }
      else {
        throw new ChadoBuddyException("ChadoBuddy getCvterm error, invalid key \"$key\"\n");
      }
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
    if (!$values) {
      throw new ChadoBuddyException("ChadoBuddy insertCv error, no values were specified\n");
    }

    try {
      $query = $this->connection->insert('1:cv');
      $query->fields($values);
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
    if (!$values) {
      throw new ChadoBuddyException("ChadoBuddy insertCvterm error, no values were specified\n");
    }

    // If cv_name specified but not cv_id, lookup cv_id
    if (!array_key_exists('cv_id', $values) or !$values['cv_id']) {
      if (!array_key_exists('cv_name', $values) or !$values['cv_name']) {
        throw new ChadoBuddyException("ChadoBuddy insertCvterm error, neither cv_id nor cv_name were specified\n");
      }
      $existing_record = $this->getCv(['name' => $values['cv_name']]);
      if (!$existing_record or is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy insertCvterm error, invalid cv_name \"$cv_name\" was specified\n");
      }
      $values['cv_id'] = $existing_record->getValue('cv_id');
    }
    unset($values['cv_name']);

    // Insert dbxref if one was not specified.
    if (!array_key_exists('dbxref_id', $values) or !$values['dbxref_id']) {
// @@@ Need to write dbxref buddy! To allow testing, generate a bogus dbxref
$n = rand(1,1000000000);
$query = $this->connection->insert('1:dbxref');
$query->fields(['db_id' => 1, 'accession' => $n]);
$query->execute();
$query = $this->connection->select('1:dbxref', 'x');
$query->condition('x.accession', $n, '=');
$query->fields('x', ['dbxref_id']);
$dbxref_id = $query->execute()->fetchField();
      $values['dbxref_id'] = $dbxref_id;
    }

    // Insert cvterm
    try {
      $query = $this->connection->insert('1:cvterm');
      // Create a subset of the passed $values for just the cvterm table.
      $term_values = [];
      foreach ($this->cvterm_required as $key => $required) {
        if ($required and !array_key_exists($key, $values)) {
          throw new ChadoBuddyException("ChadoBuddy insertCvterm error, required column \"$key\" was not specified");
        }
        if (array_key_exists($key, $values)) {
          $term_values[$key] = $values[$key];
        }
      }
      $query->fields($term_values);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCvterm error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getCvterm($term_values, $options);

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
    if (!$values) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, no values were specified\n");
    }
    if (!$conditions) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, no conditions were specified\n");
    }
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
    if (array_key_exists('cv_id', $values)) {
      unset($values['cv_id']);
    }
    $query = $this->connection->update('1:cv');
    $query->condition('cv_id', $cv_id, '=');
    $query->fields($values);
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
    if (!$values) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, no values were specified\n");
    }
    if (!$conditions) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, no conditions were specified\n");
    }
    $existing_record = $this->getCvterm($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // If the dbxref is being changed, then we will optionally delete
    // the old one, then create a new one.
    $existing_values = $existing_record->getValues();
    $update_dbxref = FALSE;
    if (array_key_exists('term_idspace', $values) and ($values['term_idspace'] != $existing_values['term_idspace'])) {
      $update_dbxref = TRUE;
    }
    if (array_key_exists('term_accession', $values) and ($values['term_accession'] != $existing_values['term_accession'])) {
      $update_dbxref = TRUE;
    }
    if ($update_dbxref) {
      // @@@ update here once dbxref buddy is done
    }

    // Update query will only be based on the cvterm_id, which we get from the retrieved record.
    $cvtern_id = $existing_record->getValue('cvterm_id');
    // We do not support changing the cvterm_id.
    if (array_key_exists('cvterm_id', $values)) {
      unset($values['cvterm_id']);
    }
    // Create a subset of the passed $values for just the cvterm table.
    $term_values = [];
    foreach ($this->cvterm_required as $key => $required) {
      if ($required and !array_key_exists($key, $values)) {
        throw new ChadoBuddyException("ChadoBuddy updateCvterm error, required column \"$key\" was not specified");
      }
      if (array_key_exists($key, $values)) {
        $term_values[$key] = $values[$key];
      }
    }
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
    if (!$values) {
      throw new ChadoBuddyException("ChadoBuddy upsertCv error, no values were specified\n");
    }
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
    if (!$values) {
      throw new ChadoBuddyException("ChadoBuddy upsertCvterm error, no values were specified\n");
    }
    $existing_record = $this->getCvtern($values, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertCvterm error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $conditions = ['cvterm_id' => $existing_record->getValue('cvterm_id')];
      $new_record = $this->updateCv($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertCvterm($values, $options);
    }
    return $new_record;
  }

  /**
   * Add a record to a controlled vocabulary term linking table (ie: feature_cvterm).
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
   *   None supported yet. Here for consistency.
   *
   * @return bool
   *   Returns TRUE if successful, and throws a ChadoBuddyException if an error is
   *   encountered. Both the cvterm and the chado record indicated by $record_id
   *   MUST ALREADY EXIST.
   */
  public function associateCvterm(string $base_table, int $record_id, ChadoBuddyRecord $cvterm, array $options = []) {
    // Get primary key of the base table
$t1 = microtime(TRUE);
    $schema = $this->connection->schema(); //@@@ wrong
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_table_def['primary key'];
$t2 = microtime(TRUE);
print "CPX1 time=".($t2-$t1)."\n";

    try {
      $query = $this->connection->insert('1:'.$basetable);
      $query->fields(['cvterm_id' => $cvterm->getValue('cvterm_id'),
                      $base_pkey_col => $record_id]);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy associateCvterm error '.$e->getMessage());
    }
    return TRUE;
  }
}