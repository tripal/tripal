<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
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
   * Used to store the manager so we can access the Cvterm buddy
   */
  protected object $buddy_manager;

  /**
   * Cache the cvterm instance here
   */
  protected object $cvterm_instance;


 /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * We are injecting an additional dependency here, the
   * ChadoBuddyPluginManager, so that this buddy can have
   * access to the Dbxref buddy.
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal_chado.database'),
      $container->get('tripal_chado.chado_buddy')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition,
                              ChadoConnection $connection, ChadoBuddyPluginManager $buddy_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $connection);
    $this->buddy_manager = $buddy_manager;
  }

  /**
   * Retrieves a chado property.
   *
   * @param string $base_table
   *   The base table for which the property should be associated. Thus to associate
   *   a property with a feature the basetable=feature and a record is added to the
   *   featureprop table.
   * @param int $record_id
   *   The primary key of the basetable to associate the property with.
   * @param array $conditions
   *   An array where the key is a table+dot+column to describe the
   *   name of the property table and the column desired. Examples
   *   here are for the project table:
   *     - base_table - (required) chado base table, e.g. 'feature'
   *     - projectprop.projectprop_id - (optional) property table primary key column name, this will vary for
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

   *     - cv.cv_id
   *     - cv.name
   *     - cv.definition
   *     - cvterm.cvterm_id
   *     - cvterm.cv_id
   *     - cvterm.name
   *     - cvterm.definition
   *     - cvterm.is_obsolete
   *     - cvterm.is_relationshiptype
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
   *     - property_table - if the default of $base_table . 'prop' needs to be changed
   *     - fkey - if the default of $base_table . '_id' needs to be changed
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
  public function getProperty(string $base_table, int $record_id, array $conditions, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);

    if (!isset($this->cvterm_instance)) {
      $this->cvterm_instance = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
    }

    $query = $this->connection->select('1:' . $property_table, $property_table);
    $query->leftJoin('1:' . $base_table, $base_table, $base_table . '.' . $fkey . ' = ' . $property_table . '.' . $fkey);
    $query->leftJoin('1:cvterm', 'cvterm', 'cvterm.cvterm_id = ' . $property_table . '.type_id');
    $query->leftJoin('1:cv', 'cv', 'cv.cv_id = cvterm.cv_id');
    $query->leftJoin('1:dbxref', 'dbxref', 'dbxref.dbxref_id = cvterm.dbxref_id');
    $query->leftJoin('1:db', 'db', 'db.db_id = dbxref.db_id');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    // Conditions are not aliased
    $query->condition($property_table . '.' . $fkey, $record_id, '=');
    foreach ($conditions as $key => $value) {
      $query->condition($key, $value, '=');
    }

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getProperty database error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable($base_table);
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
  public function deleteProperty(string $base_table, int $record_id, array $conditions, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';
    $pkey = $options['pkey'] ?? $property_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getProperty($base_table, $record_id, $conditions, $options);
    $pkey_ids = [];
    if ($existing_records) {
      if (is_array($existing_records)) {
        foreach ($existing_records as $record) {
          $pkey_ids[] = $record->getValue("$property_table.$pkey");
        }
      }
      else {
        $pkey_ids[] = $existing_records->getValue("$property_table.$pkey");
      }

      $query = $this->connection->delete('1:' . $property_table);
      $query->condition($pkey, $pkey_ids, 'IN');
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
  public function insertProperty(string $base_table, int $record_id, array $values, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    if (array_key_exists("$property_table.type_id", $values)) {
      $type_id = $values["$property_table.type_id"];
    }
    elseif (array_key_exists('cvterm', $values)) {
      $type_id = $values['cvterm']->getValue('cvterm_id');
      unset($values['cvterm']);
      $values["$property_table.type_id"] = $type_id;
    }
    else {
      throw new ChadoBuddyException('ChadoBuddy insertProperty error, neither cvterm nor type_id were specified');
    }

    // Insert the property record
    $query = $this->connection->insert('1:' . $property_table);
    $property_values = $this->subsetInput($values, [$property_table]);
    if (!array_key_exists($fkey, $property_values)) {
      $property_values[$fkey] = $record_id;
    }
    $query->fields($this->removeTablePrefix($property_values));
    try {
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertProperty database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getProperty($base_table, $record_id, $values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Updates an existing property.
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
  public function updateProperty(string $base_table, int $record_id, array $values, array $conditions, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';
    $pkey = $options['pkey'] ?? $property_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_record = $this->getProperty($base_table, $record_id, $conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateProperty error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }

    $query = $this->connection->update('1:' . $property_table);
    // We can now reduce conditions to just the property table primary key
    $query->condition("$property_table.$pkey", $existing_record->getValue("$property_table.$pkey"), '=');
    $property_values = $this->subsetInput($values, [$property_table]);
    $query->fields($this->removeTablePrefix($property_values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateProperty database error '.$e->getMessage());
    }
    $pkey_conditions = ["$property_table.$pkey" => $existing_record->getValue("$property_table.$pkey")];
    $existing_record = $this->getProperty($base_table, $record_id, $pkey_conditions, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;

  }

  /**
   * Insert a property if it doesn't yet exist OR update it if does.
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
  public function upsertProperty(string $base_table, int $record_id, array $values, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';
    $pkey = $options['pkey'] ?? $property_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_record = $this->getProperty($base_table, $record_id, $conditions, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertProperty error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $new_record = $this->updateProperty($base_table, $record_id, $values, $conditions, $options);
    }
    else {
      $new_record = $this->insertProperty($base_table, $record_id, $values, $options);
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
