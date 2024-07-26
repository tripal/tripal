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
   *     - pkey_id - (required) integer value for the base table
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

    if (!isset($this->dbxref_instance)) {
      $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
      $this->cvterm_instance = $buddy_service->createInstance('chado_cvterm_buddy', []);
    }

    // Convert generic pkey and pkey_id to actual names for this property table.
    list($property_table, $pkey, $fkey) = $this->translatePkey($mapping, $conditions);

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
    list($property_table, $pkey, $fkey) = $this->translatePkey($mapping, $values);

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

  }

  /**
   * Add a record to a database reference linking table (ie: feature_Property).
   *
   * @param string $base_table
   *   The base table for which the Property should be associated. Thus to associate
   *   a Property with a feature the basetable=feature and Property_id is added to the
   *   feature_Property table.
   * @param int $record_id
   *   The primary key of the basetable to associate the Property with.
   * @param ChadoBuddyRecord $Property
   *   A Property object returned by any of the *Property() in this service.
   * @param $options
   *   'pkey': Looking up the primary key for the base table is costly. If it is
   *           known, then pass it in as this option for better performance.
   *   Also pass in any other columns used in the linking table, some of which may
   *   have a NOT NULL constraint.
   *
   * @return bool
   *   Returns true if successful, and throws a ChadoBuddyException if an error is
   *   encountered. Both the Property and the chado record indicated by $record_id
   *   MUST ALREADY EXIST.
   */
  public function associateProperty(string $base_table, int $record_id, ChadoBuddyRecord $Property, array $options = []) {
  }

}
