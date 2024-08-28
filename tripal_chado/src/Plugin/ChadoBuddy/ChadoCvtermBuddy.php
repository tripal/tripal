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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   * @param array $options (Optional)
   *   Associative array of options.
   *     - 'case_insensitive' - a single key, or an array of keys
   *                            to query case insensitively.
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
  public function getCv(array $conditions, array $options = []) {
    $valid_tables = ['cv'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->connection->select('1:cv', 'cv');

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

    return $buddies;
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   * @param array $options (Optional)
   *   Associative array of options.
   *     - 'case_insensitive' - a single key, or an array of keys
   *                            to query case insensitively.
   *     - 'synonyms' - set to true to enable synonym query,
   *                    used internally be getCvtermSynonym()
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
  public function getCvterm(array $conditions, array $options = []) {
    $synonyms_enabled = $options['synonyms'] ?? FALSE;
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref'];
    if ($synonyms_enabled) {
      $valid_tables[] = 'cvtermsynonym';
    }
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
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
    if ($synonyms_enabled) {
      $query->leftJoin('1:cvtermsynonym', 'cvtermsynonym', 'cvterm.cvterm_id = cvtermsynonym.cvterm_id');
    }
    $this->addConditions($query, $conditions, $options);

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

    return $buddies;  }

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
   *     - cvtermsynonym.cvtermsynonym_id
   *     - cvtermsynonym.cvterm_id
   *     - cvtermsynonym.synonym
   *     - cvtermsynonym.type_id
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
   * @param array $options (Optional)
   *   Associative array of options.
   *     - 'case_insensitive' - a single key, or an array of keys
   *                            to query case insensitively.
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
  public function getCvtermSynonym(array $conditions, array $options = []) {
    $options['synonyms'] = TRUE;
    return $this->getCvterm($conditions, $options);
  }

  /**
   * Add a controlled vocabulary.
   *
   * @param $values
   *   An associative array of the values to be inserted including:
   *     - cv.cv_id
   *     - cv.name
   *     - cv.definition
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
    $values = $this->dereferenceBuddyRecord($values);
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
    $existing_records = $this->getCv($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // There should be values sufficient to retrieve a cvterm.cv_id
    if (!array_key_exists('cvterm.cv_id', $values) or !$values['cvterm.cv_id']) {
      if (!array_key_exists('cv.cv_id', $values) or !$values['cv.cv_id']) {
        $cv_values = $this->subsetInput($values, ['cv']);
        $cv_record = $this->getCv($cv_values);
        $this->validateOutput($cv_record, $values);
        $values['cvterm.cv_id'] = $cv_record[0]->getValue('cv.cv_id');
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
    $existing_records = $this->getCvterm($cvterm_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
  }

  /**
   * Add a controlled vocabulary term synonym. The existing Cvterm
   * must already exist.
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
   *     - cvtermsynonym.cvtermsynonym_id
   *     - cvtermsynonym.cvterm_id
   *     - cvtermsynonym.synonym
   *     - cvtermsynonym.type_id
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
  public function insertCvtermSynonym(array $values, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'cvtermsynonym'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // There should be values sufficient to retrieve a cvterm.cvterm_id
    if (!array_key_exists('cvtermsynonym.cvterm_id', $values) or !$values['cvtermsynonym.cvterm_id']) {
      if (!array_key_exists('cvterm.cvterm_id', $values) or !$values['cvterm.cvterm_id']) {
        $cvterm_values = $this->subsetInput($values, ['cv', 'cvterm']);
        $cvterm_record = $this->getCvterm($cvterm_values);
        $this->validateOutput($cvterm_record, $values);
        $values['cvtermsynonym.cvterm_id'] = $cvterm_record[0]->getValue('cvterm.cvterm_id');
      }
      else {
        $values['cvtermsynonym.cvterm_id'] = $values['cvterm.cvterm_id'];
      }
    }

    // Insert synonym
    $query = $this->connection->insert('1:cvtermsynonym');

    // Create a subset of the passed $values for just the cvterm table.
    $cvtermsynonym_values = $this->subsetInput($values, ['cvtermsynonym']);
    $query->fields($this->removeTablePrefix($cvtermsynonym_values));
    try {
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertCvtermSynonym database error '.$e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_records = $this->getCvtermSynonym($cvtermsynonym_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
    $values = $this->dereferenceBuddyRecord($values);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getCv($conditions, $options);
    if (count($existing_records) < 1) {
      return FALSE;
    }
    if (count($existing_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy updateCv error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Update query will only be based on the cv.cv_id, which we get from the retrieved record.
    $cv_id = $existing_records[0]->getValue('cv.cv_id');
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
    $existing_records = $this->getCv($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
    $values = $this->dereferenceBuddyRecord($values);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getCvterm($conditions, $options);
    if (count($existing_records) < 1) {
      return FALSE;
    }
    if (count($existing_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // Only update the dbxref if it is being changed.
    $existing_values = $existing_records[0]->getValues();
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
    $existing_records = $this->getCvterm($values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
  }

  /**
   * Updates an existing controlled vocabulary term synonym.
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
   *     - cvtermsynonym.cvtermsynonym_id
   *     - cvtermsynonym.cvterm_id
   *     - cvtermsynonym.synonym
   *     - cvtermsynonym.type_id
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
  public function updateCvtermSynonym(array $values, array $conditions, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'cvtermsynonym'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getCvtermSynonym($conditions, $options);
    if (count($existing_records) < 1) {
      return FALSE;
    }
    if (count($existing_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy updateCvterm error, more than one record matched the conditions specified\n".print_r($conditions, TRUE));
    }
    // This function will only update the cvtermsynonym table
    // Update query will only be based on the cvtermsynonym_id, which we get from the retrieved record.
    $cvtermsynonym_id = $existing_records[0]->getValue('cvtermsynonym.cvtermsynonym_id');
    // We do not support changing the cvtermsynonym_id.
    if (array_key_exists('cvterm.cvtermsynonym_id', $values)) {
      unset($values['cvterm.cvtermsynonym_id']);
    }
    $query = $this->connection->update('1:cvtermsynonym');
    $query->condition('cvtermsynonym_id', $cvtermsynonym_id, '=');
    // Create a subset of the passed $values for just the cvtermsynonym table.
    $synonym_values = $this->subsetInput($values, ['cvtermsynonym']);
    $query->fields($this->removeTablePrefix($synonym_values));
    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateCvtermSynonym database error '.$e->getMessage());
    }
    $existing_records = $this->getCvtermSynonym($synonym_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
  }

  /**
   * Insert a controlled vocabulary if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array of the values for the final record including:
   *     - cv.cv_id
   *     - cv.name
   *     - cv.definition
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_records = $this->getCv($conditions, $options);
    if (count($existing_records) > 0) {
      if (count($existing_records) > 1) {
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
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
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_records = $this->getCvterm($conditions, $options);
    if (count($existing_records) > 0) {
      if (count($existing_records) > 1) {
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
   * Insert a controlled vocabulary term synonym if it doesn't yet exist
   * OR update it if it does.
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
   *     - cvtermsynonym.cvtermsynonym_id
   *     - cvtermsynonym.cvterm_id
   *     - cvtermsynonym.synonym
   *     - cvtermsynonym.type_id
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
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys
   * @param array $options (Optional)
   *   None supported yet. Here for consistency.
   *
   * @return ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function upsertCvtermSynonym(array $values, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'cvtermsynonym'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_records = $this->getCvtermSynonym($conditions, $options);
    if (count($existing_records) > 0) {
      if (count($existing_records) > 1) {
        throw new ChadoBuddyException("ChadoBuddy upsertCvtermSynonym error, more than one record matched the specified values\n".print_r($values, TRUE));
      }
      $new_record = $this->updateCvtermSynonym($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertCvtermSynonym($values, $options);
    }
    return $new_record;
  }

  /**
   * Add a record to a controlled vocabulary term linking table (e.g. feature_cvterm).
   *
   * @param string $base_table
   *   The base table for which the cvterm should be associated. Thus to associate
   *   a cvterm with a feature the base_table=feature and cvterm_id is added to the
   *   feature_cvterm table.
   * @param int $record_id
   *   The primary key of the base_table to associate the cvterm with.
   * @param ChadoBuddyRecord $cvterm
   *   A cvterm object returned by any of the *Cvterm() in this service.
   * @param $options
   *   'pkey': Looking up the primary key for the base table is costly. If it is
   *           known, then pass it in as this option for better performance.
   *
   *   Also pass in any other columns used in the linking table, some of which may
   *   have a NOT NULL constraint. See the table below for a list of which of
   *   the following may be required: 'pub_id', 'is_not', 'rank', 'cvterm_type_id'.
   *   If not specified, then they will be looked up automatically, but this will
   *   be a slight performance hit. Disable this by specifying at least one additional
   *   column, or by setting the option 'lookup_columns' to FALSE.
   *
   *   Chado 1.3 defines these columns in the various linking tables:
   *   | table                       | pub_id   | is_not      | rank        | cvterm_type_id |
   *   +-----------------------------+----------+-------------+-------------+----------------+
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
    $options = $this->addLinkingColumns($linking_table, $options);
    foreach ($options as $key => $value) {
      if (($key != 'pkey') and ($key != 'lookup_columns')) {
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
   * If there are additional not NULL columns in the linking table then add them to the options.
   *
   * @param string $linking_table
   *   The name of the linking table, e.g. featureprop.
   * @param array $options
   *   The options passed to the Chado Buddy.
   *
   * @return array
   *   The passed options with not-NULL columns added.
   */
  private function addLinkingColumns(string $linking_table, array $options): array {
    $lookup_columns = $options['lookup_columns'] ?? TRUE;
    if ($lookup_columns) {
      // For Chado 1.3, these are the only possible additional columns.
      // Defaults are null pub, FALSE (encoded as zero), rank zero, null cvterm
      $defaults = ['pub_id' => 1, 'is_not' => 0, 'rank' => 0, 'cvterm_type_id' => 1];
      // If any of these were specified, we disable the automatic lookup.
      foreach (array_keys($options) as $key) {
        if (in_array($key, array_keys($defaults))) {
          $lookup_columns = FALSE;
          break;
        }
      }
      if ($lookup_columns) {
        // Automatic lookup is enabled.
        // Determine actual columns for this linking table.
        $schema = $this->connection->schema();
        $linking_table_def = $schema->getTableDef($linking_table, ['format' => 'Drupal']);
        foreach ($linking_table_def['fields'] as $field_id => $def) {
          if (array_key_exists($field_id, $defaults)) {
            // Only include if a NOT NULL constraint exists,
            // and there is not some type of default value
            if ($def['not null'] and ($def['type'] != 'serial') and !($def['default'] ?? FALSE)) {
              $options[$field_id] = $defaults[$field_id];
            }
          }
        }
      }
    }
    return $options;
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
