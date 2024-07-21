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
   *     encountered then an exception will be thrown.
   */
  public function getCv(array $identifiers, array $options = []) {
    $query = $this->connection->select('1:cv', 'cv');
    foreach ($identifiers as $key => $value) {
      $query->condition('cv.'.$key, $value, '=');
    }
    $query->fields('cv', ['cv_id', 'name', 'definition']);
    $results = $query->execute();
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
//      Not public variables so this won't work:
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
   *     - cv_id
   *     - cv_name
   *     - name
   *     - definition
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
   *     encountered then an exception will be thrown.
   */
  public function getCvterm(array $identifiers, array $options = []) {

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

    try {
      $query = $this->connection->insert('1:cv');
      $query->fields($values);
      $success = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy error '.$e->getMessage());
    }

    $existing_record = $this->getCv($values, $options);

    // These are unlikely cases, but you never know.
    if (!$existing_record) {
      throw new ChadoBuddyException("ChadoBuddy error, did not retrieve the record just added\n".print_r($values, TRUE));
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy error, more than one record matched the record just added\n".print_r($values, TRUE));
    }

    return $existing_record;
  }

  /**
   * Add a controlled vocabulary term.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - cv_id
   *     - cv_name
   *     - name
   *     - definition
   *     - term_accession
   *     - term_idspace
   *     - is_obsolete
   *     - is_relationshiptype
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown... if this is not the desired
   *   behaviour then use the upsert version of this method.
   */
  public function insertCvterm(array $values, array $options = []) {

  }

  /**
   * Updates an existing controlled vocabulary.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - cv_id
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
   *   returned if no record was found to update and an exception will be thrown
   *   if an error is encountered.
   */
  public function updateCv(array $values, array $conditions, array $options = []) {

  }

  /**
   * Updates an existing controlled vocabulary term.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - cv_id
   *     - cv_name
   *     - name
   *     - definition
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
   *   returned if no record was found to update and an exception will be thrown
   *   if an error is encountered.
   */
  public function updateCvterm() {

  }

  /**
   * Insert a controlled vocabulary if it doesn't yet exist OR update it if does.
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
   *   an exception will be thrown if an error is encountered.
   */
  public function upsertCv(array $values, array $options = []) {

  }

  /**
   * Insert a controlled vocabulary term if it doesn't yet exist OR update it if does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - cv_id
   *     - cv_name
   *     - name
   *     - definition
   *     - term_accession
   *     - term_idspace
   *     - is_obsolete
   *     - is_relationshiptype
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success, and
   *   an exception will be thrown if an error is encountered.
   */
  public function upsertCvterm(array $values, array $options = []) {

  }

  /**
   * Add a record to a controlled vocabulary term linking table (ie: feature_cvterm).
   *
   * @param string $basetable
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
   *   Returns true if successful, and throws an exception if an error is
   *   encountered. Both the cvterm and the chado record indicated by $record_id
   *   MUST ALREADY EXIST.
   */
  public function associateCvterm(string $basetable, int $record_id, ChadoBuddyRecord $cvterm, array $options = []) {

  }
}
