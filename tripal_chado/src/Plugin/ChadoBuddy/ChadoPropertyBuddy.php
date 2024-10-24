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
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
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
   *   a property with a feature, the basetable=feature and a record is added to the
   *   featureprop table.
   * @param int $record_id
   *   The primary key of the basetable to that the property is associated with.
   * @param array $conditions
   *   An array where the key is a table+dot+column to describe the
   *   name of the property table and the column desired. Examples
   *   here are for the project table:
   *     - projectprop.projectprop_id - (optional) property table primary key value
   *     - projectprop.project_id - (optional) base table primary key value
   *     - projectprop.type_id - a foreign key to cvterm_id
   *     - projectprop.value - the value of the property
   *     - projectprop.rank - optional rank of the property
   *     - and possibly other columns for some property tables
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   *
   * @param array $options (Optional)
   *     - property_table - if the default of $base_table . 'prop' needs to be changed
   *     - fkey - if the default of $base_table . '_id' needs to be changed
   *     - pkey - if the default of $property_table . '_id' needs to be changed
   *
   * @return bool|array|ChadoBuddyRecord
   *   An array of ChadoBuddyRecord objects. More specifically,
   *   (1) if the select values return a single record then we return an
   *     array containing a single ChadoBuddyRecord describing the record.
   *   (2) if the select values return multiple records, then we return an
   *     array of ChadoBuddyRecords describing the results.
   *   (3) if there are no results then we return an empty array.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function getProperty(string $base_table, int $record_id, array $conditions, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($conditions, $valid_columns);

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
    $this->addConditions($query, $conditions, $options);

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

    return $buddies;
  }

  /**
   * Adds a new property linked to the specified base table and record
   *
   * @param string $base_table
   *   The base table for which the property should be associated. Thus to associate
   *   a property with a feature, the basetable=feature and a record is added to the
   *   featureprop table.
   * @param int $record_id
   *   The primary key of the basetable to that the property is associated with.
   * @param $values
   *   An array where the key is a table+dot+column to describe the
   *   name of the property table and the column desired. Examples
   *   here are for the project table:
   *     - projectprop.projectprop_id - (optional) property table primary key value
   *     - projectprop.project_id - (optional) base table primary key value
   *     - projectprop.type_id - a foreign key to cvterm_id
   *     - projectprop.value - the value of the property
   *     - projectprop.rank - optional rank of the property
   *     - and possibly other columns for some property tables
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   *
   * @param array $options (Optional)
   *     - property_table - if the default of $base_table . 'prop' needs to be changed
   *     - fkey - if the default of $base_table . '_id' needs to be changed
   *     - pkey - if the default of $property_table . '_id' needs to be changed
   *     - create_cvterm - set to TRUE (default FALSE) if you specified the necessary
   *         fields and want to create the dbxref and cvterm when creating this
   *         property, if they do not already exist.
   *
   * @return ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown. If this is not the desired
   *   behaviour then use the upsert version of this method.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function insertProperty(string $base_table, int $record_id, array $values, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    if (!array_key_exists("$property_table.type_id", $values)) {
      if (array_key_exists('cvterm.cvterm_id', $values)) {
        $values["$property_table.type_id"] = $values['cvterm.cvterm_id'];
      }
      elseif ($options['create_cvterm'] ?? FALSE) {
        // If a term was not passed, we can create it if the required fields were included.
        // For safety, this is an opt-in setting.
        // Use the buddy manager dependency to create a Cvterm buddy instance
        if (!isset($this->cvterm_instance)) {
          $this->cvterm_instance = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
        }
        // Call the Cvterm buddy to perform the insert.
        $cvterm_values = $this->subsetInput($values, ['db', 'dbxref', 'cv', 'cvterm']);
        $cvterm_record = $this->cvterm_instance->upsertCvterm($cvterm_values, $options);
        $type_id = $cvterm_record->getValue('cvterm.cvterm_id');
        $values["$property_table.type_id"] = $type_id;
      }
      else {
        throw new ChadoBuddyException("ChadoBuddy insertProperty error, neither cvterm.cvterm_id nor $property_table.type_id"
                                     . " were specified and create_cvterm option is not enabled");
      }
    }

    // Insert the property record
    $query = $this->connection->insert('1:' . $property_table);
    $property_values = $this->subsetInput($values, [$property_table]);
    $fields = $this->removeTablePrefix($property_values);
    // The $record_id parameter is required for insert
    $fields[$fkey] = $record_id;
    $query->fields($fields);
    try {
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertProperty database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $inserted_records = $this->getProperty($base_table, $record_id, $property_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($inserted_records, $values);

    return $inserted_records[0];
  }

  /**
   * Updates an existing property.
   *
   * @param string $base_table
   *   The base table for which the property should be associated. Thus to associate
   *   a property with a feature, the basetable=feature and a record is added to the
   *   featureprop table.
   * @param int $record_id
   *   The primary key of the basetable to that the property is associated with.
   * @param array $values
   *   Values for what you want the updated propert to contaion.
   *   An array where the key is a table+dot+column to describe the
   *   name of the property table and the column desired. Examples
   *   here are for the project table:
   *     - projectprop.projectprop_id - (optional) property table primary key value
   *     - projectprop.project_id - (optional) base table primary key value
   *     - projectprop.type_id - a foreign key to cvterm_id
   *     - projectprop.value - the value of the property
   *     - projectprop.rank - optional rank of the property
   *     - and possibly other columns for some property tables
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   *
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   Although the same keys are supported as those indicated for the $values,
   *   only columns that are part of a unique constraint will be used for the
   *   database query. e.g. you can't query on the property value.
   *
   * @param array $options (Optional)
   *     - property_table - if the default of $base_table . 'prop' needs to be changed
   *     - fkey - if the default of $base_table . '_id' needs to be changed
   *     - pkey - if the default of $property_table . '_id' needs to be changed
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function updateProperty(string $base_table, int $record_id, array $values, array $conditions, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';
    $pkey = $options['pkey'] ?? $property_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getProperty($base_table, $record_id, $conditions, $options);
    if (count($existing_records) == 0) {
      return FALSE;
    }
    if (count($existing_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy updateProperty error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }

    $query = $this->connection->update('1:' . $property_table);
    // We can now reduce conditions to just the property table primary key
    $query->condition("$property_table.$pkey", $existing_records[0]->getValue("$property_table.$pkey"), '=');
    $property_values = $this->subsetInput($values, [$property_table]);
    $query->fields($this->removeTablePrefix($property_values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateProperty database error '.$e->getMessage());
    }
    $pkey_conditions = ["$property_table.$pkey" => $existing_records[0]->getValue("$property_table.$pkey")];
    $updated_records = $this->getProperty($base_table, $record_id, $pkey_conditions, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($updated_records, $values);

    return $updated_records[0];

  }

  /**
   * Insert a property if it doesn't yet exist OR update it if does.
   *
   * @param string $base_table
   *   The base table for which the property should be associated. Thus to associate
   *   a property with a feature, the basetable=feature and a record is added to the
   *   featureprop table.
   * @param int $record_id
   *   The primary key of the basetable to that the property is associated with.
   * @param array $values
   *   An array where the key is a table+dot+column to describe the
   *   name of the property table and the column desired. Examples
   *   here are for the project table:
   *     - projectprop.projectprop_id - (optional) property table primary key value
   *     - projectprop.project_id - (optional) base table primary key value
   *     - projectprop.type_id - a foreign key to cvterm_id
   *     - projectprop.value - the value of the property
   *     - projectprop.rank - optional rank of the property
   *     - and possibly other columns for some property tables
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   *
   * @param array $options (Optional)
   *     - property_table - if the default of $base_table . 'prop' needs to be changed
   *     - fkey - if the default of $base_table . '_id' needs to be changed
   *     - pkey - if the default of $property_table . '_id' needs to be changed
   *     - create_cvterm - set to TRUE (default FALSE) if you have the necessary fields
   *         and want to create the dbxref and cvterm when creating this property,
   *         if they do not already exist.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function upsertProperty(string $base_table, int $record_id, array $values, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';
    $pkey = $options['pkey'] ?? $property_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    // If cvterm.cvterm_id was supplied instead of $property_table.type_id,
    // it needs to also be included in the conditions
    $key_columns[] = 'cvterm.cvterm_id';
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_records = $this->getProperty($base_table, $record_id, $conditions, $options);
    if (count($existing_records) > 0) {
      if (count($existing_records) > 1) {
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
   * Deletes a chado property or multiple properties.
   *
   * @param string $base_table
   *   The base table for which the property should be associated. Thus to associate
   *   a property with a feature, the basetable=feature and a record is added to the
   *   featureprop table.
   * @param int $record_id
   *   The primary key of the basetable to that the property is associated with.
   * @param array $conditions
   *   An array where the key is a table+dot+column to describe the
   *   name of the property table and the column desired. Examples
   *   here are for the project table:
   *     - projectprop.projectprop_id - (optional) property table primary key value
   *     - projectprop.project_id - (optional) base table primary key value
   *     - projectprop.type_id - a foreign key to cvterm_id
   *     - projectprop.value - the value of the property
   *     - projectprop.rank - optional rank of the property
   *     - and possibly other columns for some property tables
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   *
   * @param array $options (Optional)
   *     - property_table - if the default of $base_table . 'prop' needs to be changed
   *     - fkey - if the default of $base_table . '_id' needs to be changed
   *     - pkey - if the default of $property_table . '_id' needs to be changed
   *     - max_delete - specifies the maximum number of properties that can be deleted.
   *       Default is 1. Set to -1 for unlimited.
   *       If the limit is exceeded, a ChadoBuddyException is thrown.
   *
   * @return int
   *   Returns a count of the number of records that were deleted.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function deleteProperty(string $base_table, int $record_id, array $conditions, array $options = []) {
    $property_table = $options['property_table'] ?? $base_table . 'prop';
    $fkey = $options['fkey'] ?? $base_table . '_id';
    $pkey = $options['pkey'] ?? $property_table . '_id';

    $valid_tables = ['cvterm', 'cv', 'dbxref', 'db', $base_table, $property_table];
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getProperty($base_table, $record_id, $conditions, $options);
    $pkey_ids = [];
    if (count($existing_records) > 0) {
      foreach ($existing_records as $record) {
        $pkey_ids[] = $record->getValue("$property_table.$pkey");
      }

      $max_delete = $options['max_delete'] ?? 1;
      if ((count($pkey_ids) > $max_delete) and ($max_delete != -1)) {
        throw new ChadoBuddyException('ChadoBuddy deleteProperty cannot delete '
          . count($pkey_ids) . ' records, max_delete is set to ' . $max_delete);
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

}
