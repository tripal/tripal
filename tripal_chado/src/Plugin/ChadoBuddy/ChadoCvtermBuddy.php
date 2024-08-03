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
 * Plugin implementation of the chado_buddy.
 *
 * @ChadoBuddy(
 *   id = "chado_cvterm_buddy",
 *   label = @Translation("Chado Controlled Vocabulary Term Buddy"),
 *   description = @Translation("Provides helper methods for managing chado cvs and cvterms.")
 * )
 */
class ChadoCvtermBuddy extends ChadoBuddyPluginBase implements ChadoBuddyInterface, ContainerFactoryPluginInterface {

  /**
   * Used to store the manager so we can create a buddy
   */
  protected object $buddy_manager;

  /**
   * Provide the dbxref instance
   */
  protected object $dbxref_instance;

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
   * Retrieves a controlled vocabulary.
   *
   * @param array $conditions
   *   An array where the key is a column in chado and the value describes the
   *   cv you want to select. Valid keys include:
   *     - cv.cv_id
   *     - cv.name
   *     - cv.definition
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
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
  public function getCv(array $conditions, array $options = []) {
    $valid_tables = ['cv'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->connection->select('1:cv', 'cv');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    // Conditions are not aliased
    foreach ($conditions as $key => $value) {
      $query->condition($key, $value, '=');
    }

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getCv database error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable('cv');
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
   * Retrieves a controlled vocabulary term.
   *
   * @param array $conditions
   *   An array where the key is a column in chado and the value describes the
   *   cvterm you want to select. Valid keys include:
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
   *   None supported yet. Here for consistency.
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
  public function getCvterm(array $conditions, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->connection->select('1:cvterm', 'cvterm');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    $query->leftJoin('1:cv', 'cv', 'cvterm.cv_id = cv.cv_id');
    $query->leftJoin('1:dbxref', 'dbxref', 'cvterm.dbxref_id = dbxref.dbxref_id');
    $query->leftJoin('1:db', 'db', 'dbxref.db_id = db.db_id');
    // Conditions are not aliased
    foreach ($conditions as $key => $value) {
      $query->condition($key, $value, '=');
    }

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getCvterm database error '.$e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable('cvterm');
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
   * Add a controlled vocabulary.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - cv.cv_id
   *     - cv.name
   *     - cv.definition
   * @param $options (Optional)
   *   None supported yet. Here for consistency.
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
  public function insertCv(array $values, array $options = []) {
    $valid_tables = ['cv'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    try {
      $query = $this->connection->insert('1:cv');
      $query->fields($this->removeTablePrefix($values));
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCv database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getCv($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Add a controlled vocabulary term, including creating a
   * dbxref entry if necessary.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - cv.cv_id (either cv_id, cvterm.cv_id, or cv_name required)
   *     - cv.name
   *     - cv.definition
   *     - cvterm.cvterm_id: Not valid for an insert.
   *     - cvterm.cv_id
   *     - cvterm.name: Required.
   *     - cvterm.definition
   *     - cvterm.is_obsolete
   *     - cvterm.is_relationshiptype
   *     - dbxref.dbxref_id
   *     - dbxref.db_id: Required if generating a dbxref record.
   *     - dbxref.description: Optional
   *     - dbxref.accession: Required if generating a dbxref record.
   *     - dbxref.version: Optional
   *     - db.db_id: Can be used in place of dbxref.db_id
   *     - db.name: valid, but has no effect for this function.
   *     - db.description: valid, but has no effect for this function.
   *     - db.urlprefix: valid, but has no effect for this function.
   *     - db.url: valid, but has no effect for this function.
   * @param $options (Optional)
   *     - create_dbxref - set to FALSE (default TRUE) if you do not
   *         want to automatically create a dbxref if one does not
   *         already exist.
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
  public function insertCvterm(array $values, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // There should be values sufficient to retrieve a cvterm.cv_id
    if (!array_key_exists('cvterm.cv_id', $values) or !$values['cvterm.cv_id']) {
      if (!array_key_exists('cv.cv_id', $values) or !$values['cv.cv_id']) {
        $cv_values = $this->subsetInput($values, ['cv']);
        $cv_record = $this->getCv($cv_values);
        $this->validateOutput($cv_record, $values);
        $values['cvterm.cv_id'] = $cv_record->getValue('cv.cv_id');
      }
      else {
        $values['cvterm.cv_id'] = $values['cv.cv_id'];
      }
    }

    // Insert a new dbxref if an existing one was not specified, unless not desired.
    if (!array_key_exists('cvterm.dbxref_id', $values) or !$values['cvterm.dbxref_id']) {
      if (!array_key_exists('dbxref.dbxref_id', $values) or !$values['dbxref.dbxref_id']) {
        if (array_key_exists('create_dbxref', $options) and !$options['create_dbxref']) {
          throw new ChadoBuddyException('ChadoBuddy insertCvterm error, dbxref.dbxref_id was'
                                       . ' not specified and create_dbxref is set to FALSE');
        }
        else {
          $dbxref_record = $this->upsertDbxref($values, $values, $options);
          $values['cvterm.dbxref_id'] = $dbxref_record->getValue('dbxref.dbxref_id');
        }
      }
      else {
        $values['cvterm.dbxref_id'] = $values['dbxref.dbxref_id'];
      }
    }

    // Insert cvterm
    $query = $this->connection->insert('1:cvterm');

    // Create a subset of the passed $values for just the cvterm table.
    $cvterm_values = $this->subsetInput($values, ['cvterm']);
    $query->fields($this->removeTablePrefix($cvterm_values));
    try {
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCvterm database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_record = $this->getCvterm($cvterm_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Updates an existing controlled vocabulary.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - cv.cv_id (only used for $conditions)
   *     - cv.name
   *     - cv.definition
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
  public function updateCv(array $values, array $conditions, array $options = []) {
    $valid_tables = ['cv'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($conditions, $valid_columns);
    $this->validateInput($values, $valid_columns);

    $existing_record = $this->getCv($conditions, $options);
    if (!$existing_record) {
      return FALSE;
    }
    if (is_array($existing_record)) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Update query will only be based on the cv.cv_id, which we get from the retrieved record.
    $cv_id = $existing_record->getValue('cv.cv_id');
    // We do not support changing the cv_id.
    if (array_key_exists('cv.cv_id', $values)) {
      unset($values['cv.cv_id']);
    }
    $query = $this->connection->update('1:cv');
    $query->condition('cv_id', $cv_id, '=');
    $query->fields($this->removeTablePrefix($values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateCv database error '.$e->getMessage());
    }
    $existing_record = $this->getCv($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Updates an existing controlled vocabulary term.
   *
   * @param array $values
   *   An associative array of the values for the final record (i.e what you
   *   want to update the record to be) including:
   *     - cv.cv_id (either cv_id, cvterm.cv_id, or cv_name required)
   *     - cv.name
   *     - cv.definition
   *     - cvterm.cvterm_id: Not valid for an insert.
   *     - cvterm.cv_id
   *     - cvterm.name: Required.
   *     - cvterm.definition
   *     - cvterm.is_obsolete
   *     - cvterm.is_relationshiptype
   *     - dbxref.dbxref_id
   *     - dbxref.db_id: Required if generating a dbxref record.
   *     - dbxref.description: Optional
   *     - dbxref.accession: Required if generating a dbxref record.
   *     - dbxref.version: Optional
   *     - db.db_id: Can be used in place of dbxref.db_id
   *     - db.name: valid, but has no effect for this function.
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
  public function updateCvterm(array $values, array $conditions, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

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
    $check_fields = ['db.db_name', 'db.db_id', 'dbxref.db_id', 'dbxref.accession', 'dbxref.version', 'dbxref.description'];
    foreach ($check_fields as $field) {
      if (array_key_exists($field, $values) and ($values[$field] != $existing_values[$field])) {
        $update_dbxref = TRUE;
      }
    }
    if ($update_dbxref) {
      $dbxref_record = $this->upsertDbxref($this->subsetInput($values, ['db', 'dbxref']),
                                           $this->subsetInput($conditions, ['db', 'dbxref']), $options);
    }

    // Update query will only be based on the cvterm_id, which we get from the retrieved record.
    $cvterm_id = $existing_values['cvterm.cvterm_id'];
    // We do not support changing the cvterm_id.
    if (array_key_exists('cvterm.cvterm_id', $values)) {
      unset($values['cvterm.cvterm_id']);
    }
    $query = $this->connection->update('1:cvterm');
    $query->condition('cvterm_id', $cvterm_id, '=');
    // Create a subset of the passed $values for just the cvterm table.
    $term_values = $this->subsetInput($values, ['cvterm']);
    $query->fields($this->removeTablePrefix($term_values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateCvterm database error '.$e->getMessage());
    }
    $existing_record = $this->getCvterm($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_record, $values);

    return $existing_record;
  }

  /**
   * Insert a controlled vocabulary if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - cv.cv_id
   *     - cv.name
   *     - cv.definition
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function upsertCv(array $values, array $options = []) {
    $valid_tables = ['cv'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_record = $this->getCv($conditions, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertCv error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
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
   *     - cv.cv_id (either cv_id, cvterm.cv_id, or cv_name required)
   *     - cv.name
   *     - cv.definition
   *     - cvterm.cvterm_id: Not valid for an insert.
   *     - cvterm.cv_id
   *     - cvterm.name: Required.
   *     - cvterm.definition
   *     - cvterm.is_obsolete
   *     - cvterm.is_relationshiptype
   *     - dbxref.dbxref_id
   *     - dbxref.db_id: Required if generating a dbxref record.
   *     - dbxref.description: Optional
   *     - dbxref.accession: Required if generating a dbxref record.
   *     - dbxref.version: Optional
   *     - db.db_id: Can be used in place of dbxref.db_id
   *     - db.name: valid, but has no effect for this function.
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
  public function upsertCvterm(array $values, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_record = $this->getCvterm($conditions, $options);
    if ($existing_record) {
      if (is_array($existing_record)) {
        throw new ChadoBuddyException("ChadoBuddy upsertCvterm error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
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
   *   Returns TRUE if successful.
   *   Both the cvterm and the chado record indicated by $record_id MUST ALREADY EXIST.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
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
      'cvterm_id' => $cvterm->getValue('cvterm.cvterm_id'),
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
      throw new ChadoBuddyException('ChadoBuddy associateCvterm database error '.$e->getMessage());
    }

    return TRUE;
  }

  /**
   * A helper function to add or update a dbxref for a cvterm, this will filter
   * out extra non-applicable fields that the Cvterm function here may have.
   */
  protected function upsertDbxref(array $values, array $conditions, array $options = []) {
    // Use the buddy manager dependency to create a Dbxref buddy instance
    if (!isset($this->dbxref_instance)) {
      $this->dbxref_instance = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
    }

    // Remove fields not valid for the dbxref table
    $dbxref_values = $this->subsetInput($values, ['db', 'dbxref']);
    $dbxref_conditions = $this->subsetInput($conditions, ['db', 'dbxref']);

    // Call the Dbxref buddy to perform the upsert
    $record = $this->dbxref_instance->upsertDbxref($dbxref_values, $dbxref_conditions, $options);
    return $record;
  }
}
