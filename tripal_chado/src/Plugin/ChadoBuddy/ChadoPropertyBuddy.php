<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;

/**
 * @ChadoBuddy(
 *   id = "chado_property_buddy",
 *   label = @Translation("Chado Property Buddy"),
 *   description = @Translation("Provides helper methods for managing property tables.")
 * )
 */
class ChadoPropertyBuddy extends ChadoBuddyPluginBase {

  /**
   * Keys are the column aliases, and values are the
   * table aliases and columns for the Property buddy.
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
   * table aliases and columns for the Property buddy.
   * @var array
   *
   */
  protected array $Property_mapping = [
    'Property_id' => 'x.Property_id',
    'db_id' => 'x.db_id',
    'accession' => 'x.accession',
    'version' => 'x.version',
    'Property_description' => 'x.description',
  ];

  /**
   * Keys are the column aliases, and values are the
   * table aliases and columns for the Property buddy.
   * @var array
   *
   */
  protected array $property_mapping = [
    'base_table' => 'x.for_validation_only',
    'pkey' => 'x.for_validation_only',
    'fkey' => 'x.for_validation_only',
    'property_table' => 'x.for_validation_only',
    'cvterm' => 'x.for_validation_only',
    'pkey_id' => 'p.pkey',
    'fkey_id' => 'p.fkey',
    'type_id' => 'p.type_id',
    'value' => 'p.value',
    'rank' => 'p.rank',
  ];

  /**
   * Cache the cvterm instance here
   */
  protected object $cvterm_instance;


  /**
   * Retrieves a chado property.
   *
   * @param array $conditions
   *   An array where the key is a column in the chado.db table and the value
   *   describes the db you want to select. Valid keys include:
   *     - base_table - (required) chado base table, e.g. 'feature'
   *     - pkey - (optional) property table primary key column name, this will vary for
   *              different base tables, e.g. 'featureprop_id'.
   *              If omitted, then the standard default is generated
   *     - fkey - (optional) base table primary key column name,
   *              e.g. feature_id. If omitted, then '_id' is appended
   *              to the base table name
   *     - fkey_id - (required) integer value for the base table
   *                 primary key e.g. feature_id
   *     - type_id - foreign key to cvterm_id
   *     - value - the value of the property
   *     - rank - optional rank of the property
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
  public function getProperty(array $conditions, array $options = []) {
    $mapping = $this->property_mapping;
    $this->validateInput($conditions, $mapping);

    if (!isset($this->cvterm_instance)) {
      $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
      $this->cvterm_instance = $buddy_service->createInstance('chado_cvterm_buddy', []);
    }

    // Convert generic pkey and pkey_id to actual names for this property table.
    list($base_table, $property_table, $pkey, $fkey) = $this->translatePkey($mapping, $conditions);

    $query = $this->connection->select('1:'.$property_table, 'p');

    foreach ($conditions as $key => $value) {
      $map = $mapping[$key];
      $parts = explode('.', $map);
      $query->condition($parts[0].'.'.$parts[1], $value, '=');
    }
    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($mapping as $key => $map) {
      $parts = explode('.', $map);
      $query->addField($parts[0], $parts[1], $key);
    }
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getProperty database error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      // convert the type_id to a Cvterm buddy so we get all linked columns
      $record = $this->cvterm_instance->getCvterm(['cvterm_id' => $values['type_id']], $options);
      $values['cvterm'] = $record;
      $values['pkey'] = $pkey;
      $values['fkey'] = $fkey;
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable($base_table);
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
   * Deletes a chado property or multiple properties.
   *
   * @param array $conditions
   *   An array where the key is a column in the chado.db table and the value
   *   describes the db you want to select. Valid keys include:
   *     - base_table - (required) chado base table, e.g. 'feature'
   *     - pkey - (optional) property table primary key column name, this will vary for
   *              different base tables, e.g. 'featureprop_id'.
   *              If omitted, then the standard default is generated
   *     - fkey - (optional) base table primary key column name,
   *              e.g. feature_id. If omitted, then '_id' is appended
   *              to the base table name
   *     - fkey_id - (required) integer value for the base table
   *                 primary key e.g. feature_id
   *     - type_id - foreign key to cvterm_id
   *     - value - the value of the property
   *     - rank - optional rank of the property
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return int
   *   If the select values return a single record then we return the
   *     ChadoBuddyRecord describing the chado record.
   *   If the select values return multiple records, then we return an array
   *     of ChadoBuddyRecords describing the results.
   *   If there are no results then we return FALSE and if an error is
   *     encountered then a ChadoBuddyException will be thrown.
   */
  public function deleteProperty(array $conditions, array $options = []) {
    $mapping = $this->property_mapping;
    $this->validateInput($conditions, $mapping);

    // Convert generic pkey and pkey_id to actual names for this property table.
    $original_conditions = $conditions;
    list($base_table, $property_table, $pkey, $fkey) = $this->translatePkey($mapping, $conditions);

    $existing_records = $this->getProperty($original_conditions, $options);
    $pkey_ids = [];
    if ($existing_records) {
      if (is_array($existing_records)) {
        foreach ($existing_records as $record) {
          $pkey_ids[] = $record->getValue('pkey_id');
        }
      }
      else {
        $pkey_ids[] = $existing_records->getValue('pkey_id');
      }

      $query = $this->connection->delete('1:'.$property_table);
      $pkey_column = preg_replace('/^.*\./', '', $mapping['pkey_id']);
      $query->condition($pkey_column, $pkey_ids, 'IN');
      try {
        $results = $query->execute();
      }
      catch (\Exception $e) {
        throw new ChadoBuddyException('ChadoBuddy deleteProperty database error '.$e->getMessage());
      }
    }

    return count($pkey_ids);
  }

  /**
   * Adds a new property linked to the specified base table and record
   *
   * @param $values
   *   An associative array of the values of the db (those to be inserted):
   *     - base_table - e.g. 'feature', this is always required
   *     - pkey - (optional) property table primary key column name, this will vary for
   *              different base tables, e.g. 'featureprop_id'.
   *              If omitted, then the standard default is generated
   *     - pkey_id - (required) integer value for the base table
   *                 primary key e.g. feature_id
   *     - cvterm - (required) a chado Cvterm buddy specifying the term
   *     - type_id - integer, can be used in place of cvterm if you have it
   *     - value - the value of the property
   *     - rank - optional rank of the property
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown... if this is not the desired
   *   behaviour then use the upsert version of this method.
   */
  public function insertProperty(array $values, array $options = []) {
    $mapping = $this->property_mapping;
    $fields = $this->validateInput($values, $mapping);

    // Convert generic pkey and pkey_id to actual names for this property table.
    $original_values = $values;
    list($base_table, $property_table, $pkey, $fkey) = $this->translatePkey($mapping, $values);

    if (array_key_exists('type_id', $values)) {
      $type_id = $values['type_id'];
    }
    elseif (array_key_exists('cvterm', $values)) {
      $type_id = $values['cvterm']->getValue('cvterm_id');
      unset($values['cvterm']);
      $values['type_id'] = $type_id;
    }
    else {
      throw new ChadoBuddyException('ChadoBuddy insertProperty error, neither cvterm nor type_id were specified');
    }

    // Convert the pkey_id and fkey_id to actual column name
    if (array_key_exists('pkey_id', $values)) {
      $values[$pkey] = $values['pkey_id'];
      unset($values['pkey_id']);
    }
    if (array_key_exists('fkey_id', $values)) {
      $values[$fkey] = $values['fkey_id'];
      unset($values['fkey_id']);
    }

    // Insert the property record
    try {
      // Create a subset of the passed $values for just the property table.
//      $cvterm_values = $this->validateInput($values, $mapping, TRUE);
      $query = $this->connection->insert('1:'.$property_table);
      $query->fields($values);
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCvterm database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getProperty($original_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Updates an existing database reference.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - Property_id
   *     - db_id: the database_id of the database the reference is from.
   *     - accession: the accession.
   *     - version: (Optional) The version of the database reference.
   *     - Property_description
   *     - db_name: may be used in place of db_id if that is not available.
   *     - db_description: (Optional) A description of the database reference.
   *     - urlprefix
   *     - url
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
  public function updateProperty(array $values, array $conditions, array $options = []) {
    $existing_record = $this->getProperty($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateProperty error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }

    // copy base table and pkey stuff from conditions so that they
    // don't need to be included twice
    $this->copyConditions($conditions, $values);
    $mapping = $this->property_mapping;
    $original_values = $values;
    $unaliased_values = $this->validateInput($values, $mapping);

    // Convert generic pkey and pkey_id to actual names for this property table.
    list($base_table, $property_table, $pkey, $fkey) = $this->translatePkey($mapping, $conditions);
    $pkey_id = $existing_record->getValue('pkey_id');

    // Convert the pkey_id and fkey_id to actual column name
    if (array_key_exists('pkey', $unaliased_values)) {
      $unaliased_values[$pkey] = $unaliased_values['pkey'];
      unset($unaliased_values['pkey']);
    }
    if (array_key_exists('fkey', $unaliased_values)) {
      $unaliased_values[$fkey] = $unaliased_values['fkey'];
      unset($unaliased_values['fkey']);
    }

    $query = $this->connection->update('1:' . $property_table);
    $query->condition($pkey, $pkey_id, '=');
    $query->fields($unaliased_values);
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateProperty database error '.$e->getMessage());
    }
    $existing_record = $this->getProperty(['pkey_id' => $pkey_id, 'base_table' => $base_table, 'pkey' => $pkey, 'fkey' => $fkey], $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;

  }

  /**
   * Insert a database reference if it doesn't yet exist OR update it if does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - Property_id
   *     - db_id: the database_id of the database the reference is from.
   *     - accession: the accession.
   *     - version: (Optional) The version of the database reference.
   *     - Property_description
   *     - db_name: may be used in place of db_id if that is not available.
   *     - db_description: (Optional) A description of the database reference.
   *     - urlprefix
   *     - url
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success, and
   *   a ChadoBuddyException will be thrown if an error is encountered.
   */
  public function upsertProperty(array $values, array $options = []) {
    $mapping = $this->property_mapping;
    $this->validateInput($values, $mapping);
    $original_values = $values;
    $existing_record = $this->getProperty($values, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertProperty error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      // Convert generic pkey and pkey_id to actual names for this property table.
      list($base_table, $property_table, $pkey, $fkey) = $this->translatePkey($mapping, $values);
      $existing_values = $existing_record->getValues();
      $conditions = ['pkey_id' => $existing_values['pkey_id'], 'pkey' => $existing_values['pkey'], 'base_table' => $existing_record->getBaseTable()];
      // copy base table and pkey stuff from conditions so that they
      // don't need to be included twice
      $this->copyConditions($existing_values, $original_values);
      $new_record = $this->updateProperty($original_values, $conditions, $options);
    }
    else {
      $new_record = $this->insertProperty($original_values, $options);
    }
    return $new_record;
  }

  /**
   * Helper function to copy base table and pkey stuff from conditions to values,
   * so that they don't need to be included twice for update functions.
   *
   * @param array $conditions
   *   Copy from this array.
   * @param array $values
   *   Copy to this array.
   **/
  protected function copyConditions(array &$conditions, array &$values) {
    // copy base table and pkey stuff from conditions to values,
    // so that they don't need to be included twice
    foreach (['base_table', 'pkey', 'fkey', 'fkey_id'] as $key) {
      if (array_key_exists($key, $conditions)) {
        if (array_key_exists($key, $values) and ($conditions[$key] != $values[$key])) {
          $calling_function = debug_backtrace()[1]['function'];
          throw new ChadoBuddyException("ChadoBuddy $calling_function error, the new value for \"$key\" differs from the existing value");
        }
        $values[$key] = $conditions[$key];
      }
    }
  }

}
