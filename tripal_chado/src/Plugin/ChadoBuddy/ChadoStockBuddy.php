<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;
use Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddy;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;
use Drupal\tripal_chado\Database\ChadoConnection;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Plugin implementation of the Chado stock buddy.
 */
#[ChadoBuddy(
  id: 'chado_stock_buddy',
  label: new TranslatableMarkup('Chado Stock Buddy'),
  description: new TranslatableMarkup('Provides helper methods for managing stocks in Chado.'),
)]
class ChadoStockBuddy extends ChadoBuddyPluginBase implements ChadoBuddyInterface, ContainerFactoryPluginInterface {

  /**
   * A Database query interface for querying Chado using Tripal DBX.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * Used to store the manager so we can create a buddy.
   *
   * @var \Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager
   */
  protected ChadoBuddyPluginManager $buddy_manager;

  /**
   * Used to store the dbxref ChadoBuddy instance.
   *
   * @var \Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy
   */
  protected ChadoDbxrefBuddy $dbxref_buddy;

  /**
   * Used to store the organism ChadoBuddy instance.
   *
   * @var \Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoOrganismBuddy
   */
  protected ChadoOrganismBuddy $organism_buddy;

  /**
   * Used to store the cvterm ChadoBuddy instance.
   *
   * @var \Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy
   */
  protected ChadoCvtermBuddy $cvterm_buddy;

  /**
   * Used to store the valid tables for obtaining/inserting a stock record.
   *
   * @var array
   */
  protected array $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'organism', 'stock'];

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * We are injecting an additional dependency here, the
   * ChadoBuddyPluginManager, so that this buddy can have
   * access to the Cvterm/Dbxref/Organism buddies.
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static
   * function will be called behind the scenes when a Plugin Manager uses
   * createInstance(). Specifically, this method is used to determine the
   * parameters to pass to the constructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The current container.
   * @param array $configuration
   *   A configuration array.
   * @param string $plugin_id
   *   The plugin identifier.
   * @param mixed $plugin_definition
   *   The definition of the plugin.
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal_chado.database'),
      $container->get('tripal_chado.chado_buddy'),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(
    array $configuration,
    $plugin_id,
    $plugin_definition,
    ChadoConnection $chado_connection,
    ChadoBuddyPluginManager $buddy_manager,
  ) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $chado_connection);
    $this->buddy_manager = $buddy_manager;
  }

  /**
   * Retrieves a stock record.
   *
   * @param array $conditions
   *   An array where the key is a column in chado and the value describes the
   *   stock you want to select. Valid keys include:
   *     - stock.dbxref_id
   *     - stock.organism_id
   *     - stock.name
   *     - stock.uniquename
   *     - stock.description
   *     - stock.type_id
   *     - stock.is_obsolete
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.common_name
   *     - buddy_record = an organism ChadoBuddyRecord can be used in place of
   *       or in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.type_id only:
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - buddy_record = a cvterm ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.dbxref_id only:
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a dbxref ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - 'case_insensitive' - a single key, or an array of keys
   *     to query case insensitively. Default is FALSE.
   *   - 'skip_validate' - if TRUE, skips the input validation step. This option
   *     is used internally by other methods that have already validated input.
   *     Default is FALSE.
   *
   * @return array
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
  public function getStock(array $conditions, array $options = []) {
    $valid_columns = $this->getTableColumns($this->valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);

    if (!($options['skip_validate'] ?? FALSE)) {
      $this->validateInput($conditions, $valid_columns);
    }

    $query = $this->chado_connection->select('1:stock', 'stock');
    $query->leftJoin('1:organism', 'organism', 'organism.organism_id = stock.organism_id');
    $query->leftJoin('1:cvterm', 'cvterm', 'cvterm.cvterm_id = stock.type_id');
    $query->leftJoin('1:cv', 'cv', 'cv.cv_id = cvterm.cv_id');
    $query->leftJoin('1:dbxref', 'dbxref', 'dbxref.dbxref_id = stock.dbxref_id');
    $query->leftJoin('1:db', 'db', 'db.db_id = dbxref.db_id');

    // Return the joined fields aliased to the unique names as listed in this
    // function's header.
    foreach ($valid_columns as $key) {
      [$table_name, $field_name] = explode('.', $key);
      $query->addField($table_name, $field_name, $this->makeAlias($key));
    }
    $this->addConditions($query, $conditions, $options);

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getStock database error ' . $e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->chado_connection->getSchemaName());
      $new_record->setBaseTable('stock');
      foreach ($values as $key => $value) {
        $new_record->setValue($this->unmakeAlias($key), $value);
      }
      $buddies[] = $new_record;
    }

    return $buddies;
  }

  /**
   * Inserts a stock record.
   *
   * NOTE: In addition to the stock record, only creation of a single dbxref
   *   record for stock.dbxref_id is supported using this method. See $options.
   *   Creation of an organism record or a cvterm record are NOT supported.
   *   Please use either the ChadoOrganismBuddy or ChadoCvtermBuddy to create an
   *   organism for your stock record or to create the cvterm for stock.type_id,
   *   respectively, if required.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted into the
   *   chado.stock table. Valid keys include:
   *     - stock.dbxref_id
   *     - stock.organism_id
   *     - stock.name
   *     - stock.uniquename
   *     - stock.description
   *     - stock.type_id
   *     - stock.is_obsolete
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.common_name
   *     - buddy_record = an organism ChadoBuddyRecord can be used in place of
   *       or in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.type_id only:
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - buddy_record = a cvterm ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.dbxref_id only:
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a dbxref ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_dbxref - when TRUE, the unique identifier for a stock will be
   *     created based on the necessary fields from the dbxref table and stored
   *     in the stock.dbxref_id. This is the default, pass in FALSE if you do
   *     NOT want the dbxref to be created for this stock.
   *     NOTE: If you want to annotate this stock with multiple dbxref
   *     associations stored in the stock_dbxref table, then you will need to
   *     create them using the ChadoDbxrefBuddy and associate them using
   *     ::associateStock().
   *   - validate_foreign_keys - specifies whether to validate foreign keys.
   *     Default is TRUE for all foreign keys. If you specify a boolean value,
   *     then that value is used for validating all potential foreign keys.
   *     You can skip validation for specific foreign keys by passing an array
   *     of foreign keys to skip validation for, and setting their values to
   *     FALSE. For insertStock(), valid keys are:
   *     - 'organism_id'
   *     - 'cvterm_id'
   *     - 'dbxref_id'
   *     This is ideal for performance if you already did an insert or lookup on
   *     the specified key(s) and just want to pass the information through.
   *
   * @return array
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown. If this is not the desired
   *   behaviour, then use the upsert version of this method.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   - If conditions are missing information to uniquely identify organism
   *     or cvterm for stock type.
   *   - If a stock record matching the values passed in already exists.
   *   - If the values fail validation.
   *   - If a buddy_record is passed in that is not valid.
   *   - If a database exception is encountered when inserting the stock.
   */
  public function insertStock(array $values, array $options = []) {
    $valid_columns = $this->getTableColumns($this->valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // Check if this stock already exists. This is needed because Chado does
    // not ensure a unique constraint when primary keys are nullable.
    // Skip the validate step since the values have already been checked.
    $options['skip_validate'] = TRUE;
    $existing_records = $this->getStock($values, $options);
    if (count($existing_records) > 0) {
      throw new ChadoBuddyException("ChadoBuddy insertStock error, a stock record already exists that matches the specified values:\n" . print_r($values, TRUE));
    }

    // Validate that organism exists.
    $values = $this->validateStockOrganism($values, $options);
    if (!array_key_exists('stock.organism_id', $values)) {
      throw new ChadoBuddyException("ChadoBuddy insertStock error, a unique organism record must be provided to insert a stock record.");
    }
    // Validate that stock type exists.
    $values = $this->validateStockType($values, $options);
    if (!array_key_exists('stock.type_id', $values)) {
      throw new ChadoBuddyException("ChadoBuddy insertStock error, a unique cvterm must be provided to insert a stock record.\nNOTE: You cannot specify the cvterm accession using db.name and dbxref.accession because they apply to the stock.dbxref_id. If you are having trouble uniquely selecting a cvterm, pass in a ChadoCvtermBuddy.");
    }
    // Validate that stock dbxref exists or create it if permitted.
    $values = $this->validateStockDbxref($values, $options);

    // Insert the stock record.
    try {
      $query = $this->chado_connection->insert('1:stock');
      // Create a subset of the passed $values for just the stock table.
      $stock_values = $this->subsetInput($values, ['stock']);
      $query->fields($this->removeTablePrefix($stock_values));
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertStock database error ' . $e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_records = $this->getStock($stock_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
  }

  /**
   * Updates an existing stock record.
   *
   * NOTE: When updating a stock record, only creation of a single dbxref record
   *   for stock.dbxref_id is supported using this method. See $options.
   *   Creation of an organism record or a cvterm record are NOT supported.
   *   Please use either the ChadoOrganismBuddy or ChadoCvtermBuddy to create an
   *   organism for your stock record or to create the cvterm for stock.type_id,
   *   respectively, if required.
   *
   * @param array $values
   *   An associative array that describes the values to be updated for a
   *   record in the chado.stock table. Valid keys include:
   *     - stock.dbxref_id
   *     - stock.organism_id
   *     - stock.name
   *     - stock.uniquename
   *     - stock.description
   *     - stock.type_id
   *     - stock.is_obsolete
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.common_name
   *     - buddy_record = an organism ChadoBuddyRecord can be used in place of
   *       or in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.type_id only:
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - buddy_record = a cvterm ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.dbxref_id only:
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a dbxref ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for $values.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_dbxref - when TRUE, the unique identifier for a stock will be
   *     created based on the necessary fields from the dbxref table and stored
   *     in the stock.dbxref_id. This is the default, pass in FALSE if you do
   *     NOT want the dbxref to be created for this stock.
   *     NOTE: If you want to annotate this stock with multiple dbxref
   *     associations stored in the stock_dbxref table, then you will need to
   *     create them using the ChadoDbxrefBuddy and associate them using
   *     ::associateStock().
   *   - validate_foreign_keys - specifies whether to validate foreign keys for
   *     values. Note: Foreign keys in the conditions are always validated.
   *     Default is TRUE for all foreign keys. If you specify a boolean value,
   *     then that value is used for validating all potential foreign keys.
   *     You can skip validation for specific foreign keys by passing an array
   *     of foreign keys to skip validation for, and setting their values to
   *     FALSE. For updateStock(), valid keys are:
   *     - 'organism_id'
   *     - 'cvterm_id'
   *     - 'dbxref_id'
   *     This is ideal for performance if you already did an insert or lookup on
   *     the specified key(s) and just want to pass the information through.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   - If conditions are missing information to uniquely identify organism
   *     or cvterm for stock type.
   *   - If more than one stock record matches the values passed in.
   *   - If the values fail validation.
   *   - If a buddy_record is passed in that is not valid.
   *   - If a database exception is encountered when updating the stock.
   */
  public function updateStock(array $values, array $conditions, array $options = []) {
    $valid_columns = $this->getTableColumns($this->valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    // @todo We may want to optimize the following in the future so that these
    // validate methods aren't always called twice during an update.
    // Here, options are not passed through because we want to ensure that
    // we can obtain the necessary columns for a stock's unique constraints.
    $conditions = $this->validateStockOrganism($conditions);
    if (!array_key_exists('stock.organism_id', $conditions)) {
      throw new ChadoBuddyException("ChadoBuddy updateStock error, a unique organism must be provided as a condition to update a stock record.");
    }
    // Validate that stock type exists.
    $conditions = $this->validateStockType($conditions);
    if (!array_key_exists('stock.type_id', $conditions)) {
      throw new ChadoBuddyException("ChadoBuddy updateStock error, a unique cvterm must be provided as a condition to update a stock record.\nNOTE: You cannot specify the cvterm accession using db.name and dbxref.accession because they apply to the stock.dbxref_id. If you are having trouble uniquely selecting a cvterm, pass in a ChadoCvtermBuddy.");
    }
    // Validate that stock dbxref exists or create it if permitted.
    $conditions = $this->validateStockDbxref($conditions);

    // Skip the validate step since the conditions have already been checked.
    $options['skip_validate'] = TRUE;
    $existing_records = $this->getStock($conditions, $options);
    if (count($existing_records) < 1) {
      return FALSE;
    }
    elseif (count($existing_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy updateStock error, more than one stock record matches the specified conditions:\n" . print_r($conditions, TRUE));
    }

    // Validate that organism exists.
    $values = $this->validateStockOrganism($values, $options);
    // Validate that stock type exists.
    $values = $this->validateStockType($values, $options);
    // Validate that stock dbxref exists or create it if permitted.
    $values = $this->validateStockDbxref($values, $options);

    // Update query will only be based on the stock_id, which we get from the
    // retrieved record.
    $stock_id = $existing_records[0]->getValue('stock.stock_id');
    // We do not support changing the stock_id.
    if (array_key_exists('stock.stock_id', $values)) {
      unset($values['stock.stock_id']);
    }

    $query = $this->chado_connection->update('1:stock');
    $query->condition('stock_id', $stock_id, '=');
    // Create a subset of the passed $values for just the stock table.
    $stock_values = $this->subsetInput($values, ['stock']);
    $query->fields($this->removeTablePrefix($stock_values));
    try {
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateStock database error ' . $e->getMessage());
    }
    $updated_records = $this->getStock(['stock.stock_id' => $stock_id], $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($updated_records, $values);

    return $updated_records[0];
  }

  /**
   * Insert a stock if it doesn't yet exist OR update it if it does.
   *
   * NOTE: In addition to the stock record, only creation of a single dbxref
   *   record for stock.dbxref_id is supported using this method. See $options.
   *   Creation of an organism record or a cvterm record are NOT supported.
   *   Please use either the ChadoOrganismBuddy or ChadoCvtermBuddy to create an
   *   organism for your stock record or to create the cvterm for stock.type_id,
   *   respectively, if required.
   *
   * @param array $values
   *   An associative array that describes the values to be updated for a
   *   record in the chado.stock table. Valid keys include:
   *     - stock.dbxref_id
   *     - stock.organism_id
   *     - stock.name
   *     - stock.uniquename
   *     - stock.description
   *     - stock.type_id
   *     - stock.is_obsolete
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.common_name
   *     - buddy_record = an organism ChadoBuddyRecord can be used in place of
   *       or in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.type_id only:
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - buddy_record = a cvterm ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   *   The following terms are valid where it pertains to stock.dbxref_id only:
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a dbxref ChadoBuddyRecord can be used in place of or
   *       in addition to other keys (but NOT additional buddy records).
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_dbxref - when TRUE, the unique identifier for a stock will be
   *     created based on the necessary fields from the dbxref table and stored
   *     in the stock.dbxref_id. This is the default, pass in FALSE if you do
   *     NOT want the dbxref to be created for this stock.
   *     NOTE: If you want to annotate this stock with multiple dbxref
   *     associations stored in the stock_dbxref table, then you will need to
   *     create them using the ChadoDbxrefBuddy and associate them using
   *     ::associateStock().
   *   - validate_foreign_keys - specifies whether to validate foreign keys.
   *     Default is TRUE for all foreign keys. If you specify a boolean value,
   *     then that value is used for validating all potential foreign keys.
   *     You can skip validation for specific foreign keys by passing an array
   *     of foreign keys to skip validation for, and setting their values to
   *     FALSE. For updateStock(), valid keys are:
   *     - 'organism_id'
   *     - 'cvterm_id'
   *     - 'dbxref_id'
   *     This is ideal for performance if you already did an insert or lookup on
   *     the specified key(s) and just want to pass the information through.
   *
   * @return \Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   - If more than one stock record matches the values passed in.
   *   - If the values fail validation.
   *   - If a buddy_record is passed in that is not valid.
   *   - If a database exception is encountered when inserting/updating.
   */
  public function upsertStock(array $values, array $options = []) {
    $valid_columns = $this->getTableColumns($this->valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of only the
    // columns that are part of a unique constraint
    // (stock.organism_id + stock.uniquename + stock.type_id) and their
    // referring tables (cv, cvterm, organism).
    // Note: The stock.dbxref_id is not part of the unique key and should be
    // available to update, thus, it is not part of the query conditions.
    $key_columns = $this->getTableColumns(['cv', 'cvterm', 'organism', 'stock'], 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);
    $existing_records = $this->getStock($conditions, $options);
    if (count($existing_records) > 0) {
      if (count($existing_records) > 1) {
        throw new ChadoBuddyException("ChadoBuddy upsertStock error, more than one stock record matches the specified values:\n" . print_r($values, TRUE));
      }
      $new_record = $this->updateStock($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertStock($values, $options);
    }
    return $new_record;
  }

  /**
   * A helper method to validate stock dbxref when inserting/updating a stock.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted/updated in
   *   the chado.stock table. Supports all the same keys as insertStock()
   *   and updateStock().
   *   @see ::insertStock()
   *   @see ::updateStock()
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_dbxref - This option is passed internally from insertStock() or
   *     updateStock(). When TRUE, the unique identifier for a stock will be
   *     created based on the necessary fields from the dbxref table and stored
   *     in the stock.dbxref_id. This is the default, pass in FALSE if you do
   *     NOT want the dbxref to be created for this stock.
   *   - validate_foreign_keys - This option is passed internally from
   *     insertStock() or updateStock(). This method will check for the key
   *     'dbxref_id' to determine whether to validate it.
   *     @see ::insertStock()
   *     @see ::updateStock()
   *
   * @return array
   *   If a dbxref_id was found or created for stock.dbxref_id, then the $values
   *   parameter will be returned with the stock.dbxref_id key and its value
   *   added. Otherwise, the $values parameter is returned unchanged.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   Throws an exception in the following scenarios:
   *   - stock.dbxref_id & dbxref.dbxref_id were both provided but don't match.
   *   - More than one dbxref matched the provided dbxref values.
   *   - Could not find or create a dbxref for stock.dbxref_id but dbxref
   *     values were provided.
   */
  protected function validateStockDbxref(array $values, array $options = []) {
    // Parse our 'validate_foreign_keys' option if provided, defaulting to TRUE
    // if it is not set for key 'dbxref_id'.
    $validate_dbxref = $this->parseValidateForeignKeysOption($options, 'dbxref_id');

    // Check if we already have a stock.dbxref_id AND a dbxref.dbxref_id, and
    // ensure they match.
    if (array_key_exists('stock.dbxref_id', $values)) {
      if (array_key_exists('dbxref.dbxref_id', $values)) {
        if ($values['stock.dbxref_id'] != $values['dbxref.dbxref_id']) {
          throw new ChadoBuddyException("ChadoBuddy validateStockDbxref error, stock.dbxref_id and dbxref.dbxref_id values were both provided but do not match:\n" . print_r($values, TRUE));
        }
      }
      elseif ($validate_dbxref) {
        // If only stock.dbxref_id was provided and we want to validate it,
        // set dbxref.dbxref_id to match and unset stock.dbxref_id.
        $values['dbxref.dbxref_id'] = $values['stock.dbxref_id'];
        unset($values['stock.dbxref_id']);
      }
    }

    if ($validate_dbxref) {
      // Check for dbxref identifiers and use ChadoDbxrefBuddy to try and
      // validate or retrieve the dbxref_id.
      $dbxref_values = $this->subsetInput($values, ['db', 'dbxref'], ['strict' => FALSE]);
      if ($dbxref_values) {
        // Use the buddy manager to create a Dbxref buddy instance.
        if (!isset($this->dbxref_buddy)) {
          $this->dbxref_buddy = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
        }
        $dbxref_records = $this->dbxref_buddy->getDbxref($dbxref_values, $options);
        // If a dbxref was retrieved, set stock.dbxref_id to the dbxref_id.
        if ($dbxref_records) {
          // Ensure that we didn't retrieve multiple possible dbxrefs.
          if (count($dbxref_records) > 1) {
            throw new ChadoBuddyException("ChadoBuddy validateStockDbxref error, more than one record matched the values specified:\n" . print_r($dbxref_values, TRUE));
          }
          $values['stock.dbxref_id'] = $dbxref_records[0]->getValue('dbxref.dbxref_id', ['strict' => FALSE]);
        }
        // If a dbxref could not be found, try to create it.
        elseif ($options['create_dbxref'] ?? TRUE) {
          $new_dbxref_record = $this->dbxref_buddy->upsertDbxref($dbxref_values, $options);
          $values['stock.dbxref_id'] = $new_dbxref_record->getValue('dbxref.dbxref_id');
        }
        // If we could not find or weren't allowed to create a dbxref for
        // stock.dbxref_id, yet dbxref values were provided, throw an exception.
        // Note: if the user didn't want to provide a dbxref_id, they wouldn't
        // have provided dbxref values in the first place.
        else {
          throw new ChadoBuddyException("ChadoBuddy validateStockDbxref error, could not find or create a dbxref, but dbxref values were provided from the db and/or dbxref tables:\n" . print_r($dbxref_values, TRUE));
        }
      }
    }
    elseif (array_key_exists('dbxref.dbxref_id', $values)) {
      // If only dbxref.dbxref_id was provided and we don't want to validate it,
      // set it as the stock.dbxref_id.
      $values['stock.dbxref_id'] = $values['dbxref.dbxref_id'];
    }

    return $values;
  }

  /**
   * A helper method to validate organism when inserting/updating a stock.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted/updated in
   *   the chado.stock table. Supports all the same keys as insertStock()
   *   and updateStock().
   *   @see ::insertStock()
   *   @see ::updateStock()
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - validate_foreign_keys - This option is passed internally from
   *     insertStock() or updateStock(). This method will check for the key
   *     'organism_id' to determine whether to validate it.
   *     @see ::insertStock()
   *     @see ::updateStock()
   *
   * @return array
   *   If an organism_id was found for stock.organism_id, then the $values
   *   parameter will be returned with the stock.organism_id key and its value
   *   added. Otherwise, the $values parameter is returned unchanged.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   Throws an exception in the following scenarios:
   *   - stock.organism_id & organism.organism_id were both provided but don't
   *     match.
   *   - More than one organism match the provided organism values.
   *   - Could not find an organism for stock.organism_id but organism values
   *     were provided.
   */
  protected function validateStockOrganism(array $values, array $options = []) {
    // Parse our 'validate_foreign_keys' option if provided, defaulting to TRUE
    // if it is not set for key 'organism_id'.
    $validate_organism = $this->parseValidateForeignKeysOption($options, 'organism_id');
    // Check if we already have an stock.organism_id AND a organism.organism_id,
    // and ensure they match.
    if (array_key_exists('stock.organism_id', $values)) {
      if (array_key_exists('organism.organism_id', $values)) {
        if ($values['stock.organism_id'] != $values['organism.organism_id']) {
          throw new ChadoBuddyException("ChadoBuddy validateStockOrganism error, stock.organism_id and organism.organism_id values were both provided but do not match:\n" . print_r($values, TRUE));
        }
      }
      elseif ($validate_organism) {
        // If only stock.organism_id was provided and we want to validate it,
        // set organism.organism_id to match and unset stock.organism_id.
        $values['organism.organism_id'] = $values['stock.organism_id'];
        unset($values['stock.organism_id']);
      }
    }

    if ($validate_organism) {
      // Check for organism identifiers and use ChadoOrganismBuddy to try and
      // validate or retrieve the organism_id.
      $organism_values = $this->subsetInput($values, ['organism'], ['strict' => FALSE]);
      if ($organism_values) {
        // Use the buddy manager to create an Organism buddy instance.
        if (!isset($this->organism_buddy)) {
          $this->organism_buddy = $this->buddy_manager->createInstance('chado_organism_buddy', []);
        }
        $organism_records = $this->organism_buddy->getOrganism($organism_values, $options);
        // If an organism was retrieved, set stock.organism_id.
        if ($organism_records) {
          // Ensure that we didn't retrieve multiple possible organisms.
          if (count($organism_records) > 1) {
            throw new ChadoBuddyException("ChadoBuddy validateStockOrganism error, more than one record matched the values specified:\n" . print_r($organism_values, TRUE) . "\nNOTE: You cannot specify the organism type using values from the cv, cvterm, db and dbxref tables because they apply to the stock.type_id or stock.dbxref_id. If you are having trouble uniquely selecting an organism, pass in a ChadoOrganismBuddy.");
          }
          $values['stock.organism_id'] = $organism_records[0]->getValue('organism.organism_id', ['strict' => FALSE]);
        }
        // If an organism could not be found, yet organism values were provided,
        // throw an exception.
        else {
          throw new ChadoBuddyException("ChadoBuddy validateStockOrganism error, could not find an organism, but organism values were provided from the organism table:\n" . print_r($organism_values, TRUE));
        }
      }
    }
    elseif (array_key_exists('organism.organism_id', $values)) {
      // If only organism.organism_id was provided and we don't want to validate
      // it, set it as the stock.organism_id.
      $values['stock.organism_id'] = $values['organism.organism_id'];
    }

    return $values;
  }

  /**
   * A helper method to validate stock type when inserting/updating a stock.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted/updated in
   *   the chado.stock table. Supports all the same keys as insertStock()
   *   and updateStock().
   *   @see ::insertStock()
   *   @see ::updateStock()
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - validate_foreign_keys - This option is passed internally from
   *     insertStock() or updateStock(). This method will check for the key
   *     'cvterm_id' to determine whether to validate it.
   *     @see ::insertStock()
   *     @see ::updateStock()
   *
   * @return array
   *   - If a type_id was found for stock.type_id, then the $values parameter
   *     will be returned with the stock.type_id key and its value added.
   *   - Otherwise, the $values parameter is returned unchanged.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   Throws an exception in the following scenarios:
   *   - stock.type_id & cvterm.cvterm_id were both provided but don't match.
   *   - More than one cvterm matched the provided cvterm values.
   *   - Could not find a cvterm for stock.type_id but cvterm values were
   *     provided.
   */
  protected function validateStockType(array $values, array $options = []) {
    // Parse our 'validate_foreign_keys' option if provided, defaulting to TRUE
    // if it is not set for key 'cvterm_id'.
    $validate_cvterm = $this->parseValidateForeignKeysOption($options, 'cvterm_id');

    // Check if we already have a stock.type_id AND a cvterm.cvterm_id, and
    // ensure they match.
    if (array_key_exists('stock.type_id', $values)) {
      if (array_key_exists('cvterm.cvterm_id', $values)) {
        if ($values['stock.type_id'] != $values['cvterm.cvterm_id']) {
          throw new ChadoBuddyException("ChadoBuddy validateStockType error, stock.type_id and cvterm.cvterm_id values were both provided but do not match:\n" . print_r($values, TRUE));
        }
      }
      elseif ($validate_cvterm) {
        // If only stock.type_id was provided and we want to validate it, set
        // cvterm.cvterm_id to match and unset stock.type_id.
        $values['cvterm.cvterm_id'] = $values['stock.type_id'];
        unset($values['stock.type_id']);
      }
    }

    if ($validate_cvterm) {
      // Check for cvterm identifiers and use ChadoCvtermBuddy to try and
      // validate or retrieve the type_id.
      $cvterm_values = $this->subsetInput($values, ['cvterm', 'cv'], ['strict' => FALSE]);
      if ($cvterm_values) {
        // Use the buddy manager to create a cvterm buddy instance.
        if (!isset($this->cvterm_buddy)) {
          $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
        }
        $cvterm_records = $this->cvterm_buddy->getCvterm($cvterm_values, $options);
        // If a cvterm was retrieved, set stock.type_id to the cvterm_id.
        if ($cvterm_records) {
          // Ensure that we didn't retrieve multiple possible cvterms.
          if (count($cvterm_records) > 1) {
            throw new ChadoBuddyException("ChadoBuddy validateStockType error, more than one cvterm record matched the values specified:\n" . print_r($cvterm_values, TRUE) . "\nNOTE: You cannot specify the cvterm accession using db.name and dbxref.accession because they apply to the stock.dbxref_id. If you are having trouble uniquely selecting a cvterm, pass in a ChadoCvtermBuddy.");
          }
          $values['stock.type_id'] = $cvterm_records[0]->getValue('cvterm.cvterm_id', ['strict' => FALSE]);
        }
        // If a cvterm could not be found, yet cvterm values were provided,
        // throw an exception.
        else {
          throw new ChadoBuddyException("ChadoBuddy validateStockType error, could not find a cvterm, but cvterm values were provided from the cv and/or cvterm tables:\n" . print_r($cvterm_values, TRUE));
        }
      }
    }
    elseif (array_key_exists('cvterm.cvterm_id', $values)) {
      // If only cvterm.cvterm_id was provided and we don't want to validate
      // it, set it as the stock.cvterm_id.
      $values['stock.type_id'] = $values['cvterm.cvterm_id'];
    }

    return $values;
  }

  /**
   * Add a record to a stock linking table.
   *
   * For example, to the project_stock or stockcollection_stock table.
   *
   * Both the stock record and the chado record indicated by $record_id
   * MUST ALREADY EXIST.
   *
   * @param string $base_table
   *   The base table for which the stock should be associated. For example, to
   *   associate a stock with a project, the base_table=project and stock_id is
   *   added to the project_stock table.
   * @param int $record_id
   *   The primary key of the base_table to associate the stock with.
   * @param \Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddyRecord $stock
   *   A stock object returned by any of the *Stock() methods in this service.
   * @param array $values
   *   (Optional) An associative array defining the values to be associated with
   *   the stock. These values will be inserted into the linking table along
   *   with the foreign keys to the stock and the base table. The keys should
   *   be the name of the column in this specific linking table and the value
   *   should be the value to insert for that column. If values are boolean,
   *   they should be encoded as 1 (TRUE) or 0 (FALSE).
   *   This is optional because most of these columns have default values
   *   defined in chado that can be looked up. See the table below for a list of
   *   columns used by various linking tables.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - pkey (string): The name of the primary key column in the base table.
   *     Looking up the primary key for the base table is costly. If it is
   *     known, then pass it in using this option for better performance.
   *   - lookup_columns (bool): Whether to look up any additional columns that
   *     are not specified in the options. FALSE will disable looking up any
   *     additional columns, which may cause the insert to fail if any NOT NULL
   *     columns are not specified. Default TRUE.
   *
   *   phpcs:disable
   *   Chado 1.3 defines these columns in the various linking tables:
   *   | table                 | pub_id    | is_current  | is_not      | rank        | cvterm_type_id |
   *   +-----------------------+-----------+-------------+-------------+-------------+----------------+
   *   | project_stock         | -absent   | -absent     | -absent     | -absent     | -absent        |
   *   | stockcollection_stock | -absent   | -absent     | -absent     | -absent     | -absent        |
   *   | nd_experiment_stock   | -absent   | -absent     | -absent     | -absent     | not null       |
   *   | stock_cvterm          | not null  | -absent     | has default | has default | -absent        |
   *   | stock_dbxref          | -absent   | has default | -absent     | -absent     | -absent        |
   *   | stock_feature         | -absent   | -absent     | -absent     | has default | not null       |
   *   | stock_featuremap      | -absent   | -absent     | -absent     | -absent     | not null       |
   *   | stock_genotype        | -absent   | -absent     | -absent     | -absent     | -absent        |
   *   | stock_library         | -absent   | -absent     | -absent     | -absent     | -absent        |
   *   | stock_pub             | -absent   | -absent     | -absent     | -absent     | -absent        |
   *   phpcs:enable
   *
   * @return int
   *   Indicates whether the association was
   *   - created (ChadoBuddyPluginBase::NEW = 1)
   *   - already existed (ChadoBuddyPluginBase::EXISTING = 2)
   *   If the association request was not successful, an exception is thrown.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   - If an invalid base_table is provided.
   *   - If an error is encountered during insert.
   */
  public function associateStock(string $base_table, int $record_id, ChadoBuddyRecord $stock, array $values = [], array $options = []): int {
    $possible_linking_tables = [
      'project' => 'project_stock',
      'stockcollection' => 'stockcollection_stock',
      'nd_experiment' => 'nd_experiment_stock',
      'cvterm' => 'stock_cvterm',
      'dbxref' => 'stock_dbxref',
      'feature' => 'stock_feature',
      'featuremap' => 'stock_featuremap',
      'genotype' => 'stock_genotype',
      'library' => 'stock_library',
      'pub' => 'stock_pub',
    ];

    $linking_table = $possible_linking_tables[$base_table] ?? NULL;
    if (!$linking_table) {
      throw new ChadoBuddyException("ChadoBuddy associateStock error, invalid base_table provided: $base_table. Valid options are: " . implode(', ', array_keys($possible_linking_tables)));
    }

    // Get the primary key of the base table.
    $base_pkey_col = $options['pkey'] ?? NULL;
    if (!$base_pkey_col) {
      $base_table_def = $this->getChadoTableDef($base_table);
      $base_pkey_col = $base_table_def['primary key'];
    }
    try {
      // Verify that this exact record does not already exist.
      $query = $this->chado_connection->select('1:' . $linking_table, 'L');
      $query->condition('L.stock_id', $stock->getValue('stock.stock_id'), '=');
      $query->condition('L.' . $base_pkey_col, $record_id, '=');
      $count = $query->countQuery()->execute()->fetchField();

      // If count is not zero, the record already exists, so skip insert.
      if (!$count) {
        $fields = $values;
        $fields['stock_id'] = $stock->getValue('stock.stock_id');
        $fields[$base_pkey_col] = $record_id;

        // Set the default values for any of the optional columns that apply to
        // any of the stock linking tables.
        // Defaults: 'is_not' FALSE (encoded as zero), rank zero, null cvterm,
        // 'is_current' TRUE (encoded as one).
        $defaults = [
          'is_not' => 0,
          'rank' => 0,
          'cvterm_type_id' => 1,
          'is_current' => 1,
        ];
        // Add in any of the other columns for the linking table.
        $fields = $this->addLinkingColumns($linking_table, $fields, $defaults, $options);
        $query = $this->chado_connection->insert('1:' . $linking_table);
        $query->fields($fields);
        $query->execute();

        // Return NEW to indicate the association was created.
        return self::NEW;
      }
      else {
        // Return EXISTING to indicate the association already existed.
        return self::EXISTING;
      }
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy associateStock database error ' . $e->getMessage());
    }
  }

  /**
   * Add a record to the stock_relationship table.
   *
   * @param array $subject_values
   *   An array where the key is a column in chado and the value describes the
   *   stock that you want to be the subject of the relationship.
   *   Valid keys include:
   *     - stock.dbxref_id
   *     - stock.organism_id
   *     - stock.name
   *     - stock.uniquename
   *     - stock.description
   *     - stock.type_id
   *     - stock.is_obsolete
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.common_name
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys.
   * @param array $object_values
   *   An array where the key is a column in chado and the value describes the
   *   stock that you want to be the object of the relationship.
   *   Valid keys include the same ones listed for $subject_values.
   * @param array $rel_type_values
   *   An array where the key is a column in chado and the value describes the
   *   cvterm that you want to use as the relationship type. Valid keys include:
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
   *     - buddy_record (object): a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys.
   * @param array $stock_rel_values
   *   (Optional) An array where the key is a non-required column in the
   *   stock_relationship table and the value is the value to insert for that
   *   column. Default values are set to null and 0, respectively. Valid keys
   *   are: stock_relationship.value and stock_relationship.rank.
   * @param array $options
   *   (Optional)
   *   Associative array of options.
   *     - 'case_insensitive' - a single key, or an array of keys
   *     to query for stocks and cvterms case insensitively. Default is FALSE.
   *     @todo Allow this option to be an array, where the keys refer to each of
   *     the 3 input parameters so that this option can be applied
   *     independently. If only TRUE or FALSE is provided, then it will be
   *     applied to all 3 input parameters.
   *
   * @return int
   *   Indicates whether the relationship was
   *   - created (ChadoBuddyPluginBase::NEW = 1)
   *   - already existed (ChadoBuddyPluginBase::EXISTING = 2)
   *   If the relationship request was not successful, an exception is thrown.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   - If none or more than one stock record matches the values passed in for
   *     either the subject or object stock.
   *   - If none or more than one cvterm record matches the values passed in for
   *     the relationship type.
   *   - If more than one stock_relationship record already exists.
   *   - If a database exception is encountered when inserting the new
   *     stock_relationship record.
   */
  public function relateStock(array $subject_values, array $object_values, array $rel_type_values, array $stock_rel_values = [], array $options = []) {

    // Get the subject stock record.
    $subject_stock = $this->getStock($subject_values, $options);
    if (count($subject_stock) < 1) {
      throw new ChadoBuddyException("ChadoBuddy relateStock error, could not find a stock which matched the specified subject values:\n" . print_r($subject_values, TRUE));
    }
    elseif (count($subject_stock) > 1) {
      throw new ChadoBuddyException("ChadoBuddy relateStock error, more than one stock record matched the specified subject values:\n" . print_r($subject_values, TRUE));
    }

    // Get the object stock record.
    $object_stock = $this->getStock($object_values, $options);
    if (count($object_stock) < 1) {
      throw new ChadoBuddyException("ChadoBuddy relateStock error, could not find a stock which matched the specified object values:\n" . print_r($object_values, TRUE));
    }
    elseif (count($object_stock) > 1) {
      throw new ChadoBuddyException("ChadoBuddy relateStock error, more than one stock record matched the specified object values:\n" . print_r($object_values, TRUE));
    }

    // Get the relationship type cvterm record.
    if (!isset($this->cvterm_buddy)) {
      $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
    }
    $rel_cvterm = $this->cvterm_buddy->getCvterm($rel_type_values, $options);
    if (count($rel_cvterm) < 1) {
      throw new ChadoBuddyException("ChadoBuddy relateStock error, could not find a cvterm which matched the specified values:\n" . print_r($rel_type_values, TRUE));
    }
    elseif (count($rel_cvterm) > 1) {
      throw new ChadoBuddyException("ChadoBuddy relateStock error, more than one cvterm record matched the specified values:\n" . print_r($rel_type_values, TRUE));
    }

    try {
      // Check if this relationship already exists between the two stocks.
      // We are checking using the primary key:
      // subject_id + object_id + type_id + rank (if provided).
      // NOTE: If a relationship exists with this primary key but the user
      // specified a different value for stock_relationship.value, the user
      // will only be told that the relationship already exists, and the value
      // will not be updated.
      $query = $this->chado_connection->select('1:stock_relationship', 'SR');
      $query->condition('SR.subject_id', $subject_stock[0]->getValue('stock.stock_id'), '=');
      $query->condition('SR.object_id', $object_stock[0]->getValue('stock.stock_id'), '=');
      $query->condition('SR.type_id', $rel_cvterm[0]->getValue('cvterm.cvterm_id'), '=');
      if (array_key_exists('stock_relationship.rank', $stock_rel_values)) {
        $query->condition('SR.rank', $stock_rel_values['stock_relationship.rank'], '=');
      }
      $count = $query->countQuery()->execute()->fetchField();

      if ($count < 1) {
        // Set defaults for optional columns in stock_relationship table if not
        // provided.
        $defaults = [
          'stock_relationship.value' => NULL,
          'stock_relationship.rank' => 0,
        ];
        $stock_rel_values = array_merge($defaults, $stock_rel_values);
        $this->validateInput($stock_rel_values, array_keys($defaults));
        $stock_rel_values = $this->removeTablePrefix($stock_rel_values);

        // Add only the information needed for the stock_relationship table.
        $fields = [
          'subject_id' => $subject_stock[0]->getValue('stock.stock_id'),
          'object_id' => $object_stock[0]->getValue('stock.stock_id'),
          'type_id' => $rel_cvterm[0]->getValue('cvterm.cvterm_id'),
        ];

        $fields = array_merge($fields, $stock_rel_values);

        // Insert the new relationship.
        $query = $this->chado_connection->insert('1:stock_relationship');
        $query->fields($fields);
        $query->execute();
        // Return NEW to indicate the relationship was created.
        return self::NEW;
      }
      elseif ($count == 1) {
        // Return EXISTING to indicate the relationship already exists.
        return self::EXISTING;
      }
      else {
        throw new ChadoBuddyException("ChadoBuddy relateStock error, more than one stock_relationship record already exists between the two stocks with the specified relationship type.\nSUBJECT: " . print_r($subject_stock[0]->getValues(), TRUE) . "\nOBJECT:" . print_r($object_stock[0]->getValues(), TRUE) . "\n RELATIONSHIP TYPE: " . print_r($rel_cvterm[0]->getValues(), TRUE) . "\nRANK: " . print_r($stock_rel_values['stock_relationship.rank'] ?? 'not specified', TRUE));
      }
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy relateStock database error ' . $e->getMessage());
    }
  }

}
