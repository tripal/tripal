<?php

namespace Drupal\tripal_chado\Plugin\ChadoBuddy;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddy;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyPluginBase;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;

/**
 * Plugin implementation of the chado organism buddy.
 */
#[ChadoBuddy(
  id: 'chado_organism_buddy',
  label: new TranslatableMarkup('Chado Organism Buddy'),
  description: new TranslatableMarkup('Provides helper methods for managing organisms in Chado.'),
)]
class ChadoOrganismBuddy extends ChadoBuddyPluginBase implements ChadoBuddyInterface, ContainerFactoryPluginInterface {

  /**
   * A Database query interface for querying Chado using Tripal DBX.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  public ChadoConnection $chado_connection;

  /**
   * Used to store the manager so we can create a buddy.
   *
   * @var \Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager
   */
  public ChadoBuddyPluginManager $buddy_manager;

  /**
   * Used to store the cvterm ChadoBuddy instance.
   *
   * @var \Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy
   */
  protected ChadoCvtermBuddy $cvterm_buddy;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * We are injecting an additional dependency here, the
   * ChadoBuddyPluginManager, so that this buddy can have
   * access to the Cvterm/Dbxref buddies.
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
   * Retrieves an organism record.
   *
   * @param array $conditions
   *   An array where the key is a column in chado and the value describes the
   *   organism you want to select. Valid keys include:
   *   - organism.genus
   *   - organism.species
   *   - organism.infraspecific_name
   *   - organism.type_id
   *   - organism.abbreviation
   *   - organism.common_name
   *   - organism.comment
   *   - cvterm.name
   *   - cvterm.is_obsolete
   *   - cv.name
   *   - dbxref.accession
   *   - db.name
   *   - buddy_record = a ChadoBuddyRecord can be used
   *     in place of or in addition to other keys.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - 'case_insensitive' - a single key, or an array of keys
   *     to query case insensitively.
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
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function getOrganism(array $conditions, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'organism'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    if (!($options['skip_validate'] ?? FALSE)) {
      $this->validateInput($conditions, $valid_columns);
    }

    $query = $this->chado_connection->select('1:organism', 'organism');
    $query->leftJoin('1:cvterm', 'cvterm', 'cvterm.cvterm_id = organism.type_id');
    $query->leftJoin('1:cv', 'cv', 'cv.cv_id = cvterm.cv_id');
    $query->leftJoin('1:dbxref', 'dbxref', 'dbxref.dbxref_id = cvterm.dbxref_id');
    $query->leftJoin('1:db', 'db', 'db.db_id = dbxref.db_id');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header.
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    $this->addConditions($query, $conditions, $options);

    try {
      $results = $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy getOrganism database error ' . $e->getMessage());
    }
    $buddies = [];
    while ($values = $results->fetchAssoc()) {
      $new_record = new ChadoBuddyRecord();
      $new_record->setSchemaName($this->chado_connection->getSchemaName());
      $new_record->setBaseTable('organism');
      foreach ($values as $key => $value) {
        $new_record->setValue($this->unmakeAlias($key), $value);
      }
      $buddies[] = $new_record;
    }

    return $buddies;
  }

  /**
   * Insert a new organism.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted into the
   *   chado.organism table. Valid keys include:
   *   - organism.genus
   *   - organism.species
   *   - organism.infraspecific_name
   *   - organism.type_id
   *   - organism.abbreviation
   *   - organism.common_name
   *   - organism.comment
   *   - cvterm.name
   *   - cvterm.is_obsolete
   *   - cv.name
   *   - dbxref.accession
   *   - db.name
   *   - buddy_record = a ChadoBuddyRecord can be used
   *     in place of or in addition to other keys.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_cvterm - set to TRUE (default FALSE) if you specified the
   *     necessary fields and want to create the dbxref and cvterm for
   *     organism.type_id when creating this organism, if they do not exist.
   *     NOTE: This is NOT recommended. We suggest you import ontologies first.
   *   - validate_foreign_keys - set to FALSE (default TRUE) if you specified
   *     the necessary fields to insert a foreign key into the organism table,
   *     but do not want this method to peform a lookup to validate the key
   *     exists. This is ideal for performance if you already did an insert or
   *     lookup on this key and want to pass the information through.
   *
   * @return \Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown. If this is not the desired
   *   behaviour, then use the upsert version of this method.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function insertOrganism(array $values, array $options = []) {

    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'organism'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // Check if this organism already exists. This is needed because Chado does
    // not ensure a unique constraint on organism.
    // Skip the validate step since the values have already been checked.
    $options['skip_validate'] = TRUE;
    $existing_records = $this->getOrganism($values, $options);
    if (count($existing_records) > 0) {
      throw new ChadoBuddyException("ChadoBuddy insertOrganism error, an organism record already exists that matches the specified values:\n" . print_r($values, TRUE));
    }

    // Validate the organism rank.
    $values = $this->validateOrganismRankCvterm($values, $options);

    // Insert the organism record.
    try {
      $query = $this->chado_connection->insert('1:organism');
      // Create a subset of the passed $values for just the organism table.
      $organism_values = $this->subsetInput($values, ['organism']);
      $query->fields($this->removeTablePrefix($organism_values));
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy insertOrganism database error ' . $e->getMessage());
    }

    // Retrieve the newly inserted record.
    $existing_records = $this->getOrganism($organism_values, $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($existing_records, $values);

    return $existing_records[0];
  }

  /**
   * Updates an existing organism record.
   *
   * @param array $values
   *   An associative array that describes the values to be updated for a
   *   record in the chado.organism table. Valid keys include:
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.type_id
   *     - organism.abbreviation
   *     - organism.common_name
   *     - organism.comment
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys.
   * @param array $conditions
   *   An associative array of the conditions to find the record to update.
   *   The same keys are supported as those indicated for $values.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_cvterm - set to TRUE (default FALSE) if you specified the
   *     necessary fields and want to create the dbxref and cvterm for
   *     organism.type_id when updating this organism, if they do not exist.
   *     NOTE: This is NOT recommended. We suggest you import ontologies first.
   *   - validate_foreign_keys - set to FALSE (default TRUE) if you specified
   *     the necessary fields to insert a foreign key into the organism table,
   *     but do not want this method to peform a lookup to validate the key
   *     exists. This is ideal for performance if you already did an insert or
   *     lookup on this key and want to pass the information through.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function updateOrganism(array $values, array $conditions, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'organism'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($values, $valid_columns);
    $this->validateInput($conditions, $valid_columns);

    $existing_records = $this->getOrganism($conditions, $options);
    if (count($existing_records) < 1) {
      return FALSE;
    }
    if (count($existing_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy updateOrganism error, more than one record matched the conditions specified:\n" . print_r($conditions, TRUE));
    }

    // Validate the organism rank.
    $values = $this->validateOrganismRankCvterm($values, $options);

    // Update query will only be based on the organism_id,
    // which we get from the retrieved record.
    $organism_id = $existing_records[0]->getValue('organism.organism_id');
    // We do not support changing the organism_id.
    if (array_key_exists('organism.organism_id', $values)) {
      unset($values['organism.organism_id']);
    }

    $query = $this->chado_connection->update('1:organism');
    $query->condition('organism_id', $organism_id, '=');
    // Create a subset of the passed $values for just the organism table.
    $organism_values = $this->subsetInput($values, ['organism']);
    $query->fields($this->removeTablePrefix($organism_values));
    try {
      $query->execute();
    }
    catch (\Exception $e) {
      throw new ChadoBuddyException('ChadoBuddy updateOrganism database error ' . $e->getMessage());
    }
    $updated_records = $this->getOrganism(['organism.organism_id' => $organism_id], $options);

    // Validate that exactly one record was obtained.
    $this->validateOutput($updated_records, $values);

    return $updated_records[0];
  }

  /**
   * Insert an organism if it doesn't yet exist OR update it if it does.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted/updated in
   *   the chado.organism table. Valid keys include:
   *     - organism.genus
   *     - organism.species
   *     - organism.infraspecific_name
   *     - organism.type_id
   *     - organism.abbreviation
   *     - organism.common_name
   *     - organism.comment
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - dbxref.accession
   *     - db.name
   *     - buddy_record = a ChadoBuddyRecord can be used
   *       in place of or in addition to other keys.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_cvterm - set to TRUE (default FALSE) if you specified the
   *     necessary fields and want to create the dbxref and cvterm for
   *     organism.type_id when creating this organism, if they do not exist.
   *     NOTE: This is NOT recommended. We suggest you import ontologies first.
   *
   * @return \Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddyRecord
   *   The inserted/updated ChadoBuddyRecord will be returned on success.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function upsertOrganism(array $values, array $options = []) {
    $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'organism'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // For upsert, the query conditions are a subset consisting of
    // only the columns that are part of a unique constraint:
    // genus + species + type_id + infraspecific_name.
    $key_columns = $this->getTableColumns($valid_tables, 'unique');
    $conditions = $this->makeUpsertConditions($values, $key_columns);

    $existing_records = $this->getOrganism($conditions, $options);
    if (count($existing_records) > 0) {
      if (count($existing_records) > 1) {
        throw new ChadoBuddyException("ChadoBuddy upsertOrganism error, more than one record matched the specified values:\n" . print_r($values, TRUE));
      }
      $new_record = $this->updateOrganism($values, $conditions, $options);
    }
    else {
      $new_record = $this->insertOrganism($values, $options);
    }
    return $new_record;
  }

  /**
   * Retrieves the scientific name for a specific organism.
   *
   * @param array $conditions
   *   An array where the key is a column in chado and the value describes the
   *   organism you want to select. Supports all the same keys as getOrganism().
   *   @see ::getOrganism()
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - abbreviate_rank: If this organism has a rank, attempt to abbreviate it.
   *     For full list of valid abbreviations,
   *     @see ::abbreviateInfraspecificRank()
   *     Default is TRUE.
   *
   * @return string
   *   The fully formatted scientific name for the retrieved organism.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function getOrganismScientificName(array $conditions, array $options = []) {
    $organism_records = $this->getOrganism($conditions, $options);
    // Check if we have exactly one record.
    if (count($organism_records) < 1) {
      throw new ChadoBuddyException("ChadoBuddy getOrganismScientificName error, could not find an organism record that matches the specified conditions:\n" . print_r($conditions, TRUE));
    }
    elseif (count($organism_records) > 1) {
      throw new ChadoBuddyException("ChadoBuddy getOrganismScientificName error, more than one organism record matches the specified conditions:\n" . print_r($conditions, TRUE));
    }
    // Grab the genus and species.
    $organism_values = $organism_records[0]->getValues();
    $organism_name = $organism_values['organism.genus'] . ' ' . $organism_values['organism.species'];
    // If this organism has a rank and infraspecific name, we need to add it
    // to its scientific name.
    $rank = '';
    if ($organism_values['organism.type_id']) {
      $cvterm_id = $organism_values['organism.type_id'];
      // We need a cvterm buddy to grab the name of the rank.
      if (!isset($this->cvterm_buddy)) {
        $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
      }
      $cvterm_record = $this->cvterm_buddy->getCvterm(['cvterm.cvterm_id' => $cvterm_id]);
      $rank = $cvterm_record[0]->getValue('cvterm.name');
    }
    // If we successfully grabbed the name of the rank, check options if we
    // should find its abbreviation before adding it to our scientific name.
    if ($rank) {
      if ($options['abbreviate_rank'] ?? TRUE) {
        $rank = $this->abbreviateInfraspecificRank($rank);
      }
      $organism_name .= ' ' . $rank . ' ' . $organism_values['organism.infraspecific_name'];
    }
    // If we're missing a rank but have an infraspecific name, tag that onto the
    // end.
    elseif ($organism_values['organism.infraspecific_name']) {
      $organism_name .= ' ' . $organism_values['organism.infraspecific_name'];
    }

    return $organism_name;
  }

  /**
   * Retrieves an organism record from its scientific name.
   *
   * @param string $scientific_name
   *   The scientific name of the desired organism record. At minimum, this
   *   includes the organism genus and species concatenated together. If the
   *   organism has an infraspecific type, it is to be included after species
   *   and before the infraspecific name and can be abbreviated or not.
   *   Note: There is the option to specify an organism's abbreviation or common
   *   name here instead - please see $options.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - check_abbreviation: If TRUE and $scientific_name did not match the
   *     scientific name, then check organism.abbreviation. Default is FALSE.
   *   - check_common_name: If TRUE and $scientific_name did not match the
   *     scientific name, then check organism.common_name. Default is FALSE.
   *   - case_sensitive: If TRUE then all searches should be case sensitive.
   *     Default is FALSE.
   *   If no options are specified, search is for a match of $scientific_name to
   *   the combination of genus + species + rank + infraspecific name only, case
   *   insensitive.
   *
   * @return array
   *   An array of ChadoBuddyRecord objects. More specifically,
   *   (1) if the select values return a single record then we return an
   *     array containing a single ChadoBuddyRecord describing the record.
   *   (2) if the select values return multiple records, then we return an
   *     array of ChadoBuddyRecords describing the results.
   *   (3) if there are no results then we return an empty array.
   */
  public function getOrganismFromScientificName(string $scientific_name, array $options = []) {
    $buddies = [];
    // Handle empty $scientific_name by returning an empty array.
    if (!$scientific_name) {
      return $buddies;
    }

    // Check scientific name first, and if a match is found, nothing
    // else specified by $options will be checked.
    // Split our string up into a maximum of 4 substrings or parts.
    $parts = preg_split('/\s+/', $scientific_name, 4);
    // $scientific_name could be a single word, so make sure this is defined.
    $parts[1] = $parts[1] ?? '';

    // Setup our conditions for lookup.
    $conditions = [
      'organism.genus' => $parts[0],
      'organism.species' => $parts[1],
    ];
    // Remove the abbreviation from rank if it exists.
    if (array_key_exists(2, $parts)) {
      $conditions['cvterm.name'] = $this->unabbreviateInfraspecificRank($parts[2]);
    }
    if (array_key_exists(3, $parts)) {
      $conditions['organism.infraspecific_name'] = $parts[3];
    }

    // Check 'case_sensitive' option and pass it through to our getter.
    $lookup_options = [];
    if (!($options['case_sensitive'] ?? FALSE)) {
      foreach ($conditions as $key => $value) {
        $lookup_options['case_insensitive'][] = $key;
      }
    }

    $buddies = $this->getOrganism($conditions, $lookup_options);

    // Check other search modes only when no match was found.
    if (empty($buddies)) {
      // Try to find $scientific_name in the abbreviation column. This does not
      // have a unique constraint, so there may be more than one match.
      if ($options['check_abbreviation'] ?? FALSE) {
        $abbrev_conditions = ['organism.abbreviation' => $scientific_name];
        $buddies = $this->getOrganism($abbrev_conditions, $lookup_options);
      }

      // Try to find $scientific_name in the common_name column. This does not
      // have a unique constraint, so there may be more than one match.
      if ($options['check_common_name'] ?? FALSE) {
        $common_conditions = ['organism.common_name' => $scientific_name];
        // Avoid adding duplicates if identical records were found via
        // abbreviation.
        $temp_buddies = [];
        $temp_buddies = $this->getOrganism($common_conditions, $lookup_options);
        $buddy_comparator = function (ChadoBuddyRecord $x, ChadoBuddyRecord $y): int {
          return ChadoBuddyRecord::compareTo($x, $y, 'organism.organism_id');
        };
        $buddies = array_merge($buddies, array_udiff($temp_buddies, $buddies, $buddy_comparator));
      }
    }

    return $buddies;
  }

  /**
   * A helper method to validate cvterm when inserting an organism with rank.
   *
   * @param array $values
   *   An associative array that describes the values to be inserted/updated in
   *   the chado.organism table. Supports all the same keys as insertOrganism()
   *   and updateOrganism().
   *   @see ::insertOrganism()
   *   @see ::updateOrganism()
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *   - create_cvterm - set to TRUE (default FALSE) if you specified the
   *     necessary fields and want to create the dbxref and cvterm for
   *     organism.type_id when creating/updating this organism, if they do not
   *     exist. This option is passed internally from insertOrganism() or
   *     updateOrganism().
   *   - validate_foreign_keys - set to FALSE (default TRUE) if you specified
   *     the necessary fields to insert a foreign key into the organism table,
   *     but do not want this method to peform a lookup to validate the key
   *     exists. This is ideal for performance if you already did an insert or
   *     lookup on this key and want to pass the information through. This
   *     option is passed internally from insertOrganism() or updateOrganism().
   *
   * @return array
   *   If a cvterm was found or created for organism.type_id, then the $values
   *   parameter will be returned with the organism.type_id key and its value
   *   added. Otherwise, the $values parameter is returned unchanged.
   *
   * @throws \Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   Throws an exception in the following scenarios:
   *   - organism.type_id & cvterm.cvterm_id were both provided but don't match.
   *   - More than one cvterm matched the provided cvterm values.
   *   - Could not find or create a cvterm for organism.type_id but cvterm
   *     values were provided.
   */
  protected function validateOrganismRankCvterm(array $values, array $options = []) {
    // Check if we already have an organism.type_id AND a cvterm.cvterm_id, and
    // ensure they match.
    if (array_key_exists('organism.type_id', $values)) {
      if (array_key_exists('cvterm.cvterm_id', $values)) {
        if ($values['organism.type_id'] != $values['cvterm.cvterm_id']) {
          throw new ChadoBuddyException("ChadoBuddy validateOrganismRankCvterm error, organism.type_id and cvterm.cvterm_id values were both provided but do not match:\n" . print_r($values, TRUE));
        }
      }
      elseif ($options['validate_foreign_keys'] ?? TRUE) {
        // If only organism.type_id was provided and we want to validate it,
        // set cvterm.cvterm_id to match and unset organism.type_id.
        $values['cvterm.cvterm_id'] = $values['organism.type_id'];
        unset($values['organism.type_id']);
      }
    }

    if ($options['validate_foreign_keys'] ?? TRUE) {
      // Check for cvterm identifiers and use ChadoCvtermBuddy to try and
      // validate or retrieve the cvterm_id.
      $cvterm_values = $this->subsetInput($values, ['db', 'dbxref', 'cv', 'cvterm'], ['strict' => FALSE]);
      if ($cvterm_values) {
        // Use the buddy manager to create a Cvterm buddy instance.
        if (!isset($this->cvterm_buddy)) {
          $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
        }
        $cvterm_records = $this->cvterm_buddy->getCvterm($cvterm_values, $options);
        // If a cvterm was retrieved, set organism.type_id to the cvterm_id.
        if ($cvterm_records) {
          // Ensure that we didn't retrieve multiple possible cvterms.
          if (count($cvterm_records) > 1) {
            throw new ChadoBuddyException("ChadoBuddy validateOrganismRankCvterm error, more than one record matched the values specified:\n" . print_r($cvterm_values, TRUE));
          }
          $values['organism.type_id'] = $cvterm_records[0]->getValue('cvterm.cvterm_id', ['strict' => FALSE]);
        }
        // If a cvterm could not be found, try to create it if the required
        // fields were included. For safety, this is an opt-in setting.
        elseif ($options['create_cvterm'] ?? FALSE) {
          $new_cvterm_record = $this->cvterm_buddy->upsertCvterm($cvterm_values, $options);
          $values['organism.type_id'] = $new_cvterm_record->getValue('cvterm.cvterm_id');
        }
        // If we could not find or create a cvterm for organism.type_id, yet
        // cvterm values were provided, throw an exception.
        // Note: if the user didn't want to provide a type_id, they wouldn't
        // have provided cvterm values in the first place.
        else {
          throw new ChadoBuddyException("ChadoBuddy validateOrganismRankCvterm error, could not find or create a cvterm for organism.type_id but cvterm values were provided:\n" . print_r($cvterm_values, TRUE));
        }
      }
    }
    elseif (array_key_exists('cvterm.cvterm_id', $values)) {
      // If only cvterm.cvterm_id was provided and we don't want to validate it,
      // set it as the organism.type_id.
      $values['organism.type_id'] = $values['cvterm.cvterm_id'];
    }

    return $values;
  }

  /**
   * A helper method to abbreviate the infraspecific rank of an organism.
   *
   * @param string $rank
   *   The rank below species.
   *
   * @return string
   *   The proper abbreviation for the rank.
   */
  public function abbreviateInfraspecificRank(string $rank) {
    $abb = '';
    $rank = strtolower($rank);
    $abb = match ($rank) {
      'no_rank' => '',
      'subspecies' => 'subsp.',
      'varietas' => 'var.',
      'variety' => 'var.',
      'subvarietas' => 'subvar.',
      'subvariety' => 'subvar.',
      'convariety' => 'convar.',
      'cultivar' => 'cv.',
      'cultivar group' => 'Group',
      'forma' => 'f.',
      'subforma' => 'subf.',
      default => $rank,
    };
    return $abb;
  }

  /**
   * A helper method to expand the infraspecific rank of an abbreviated one.
   *
   * @param string $rank
   *   The rank below species or its abbreviation.
   *   A period at the end of the abbreviation is optional.
   *
   * @return string
   *   The proper unabbreviated form for the rank.
   */
  public function unabbreviateInfraspecificRank(string $rank) {
    if (preg_match('/^subsp\.?$/i', $rank)) {
      $rank = 'subspecies';
    }
    elseif (preg_match('/^ssp\.?$/i', $rank)) {
      $rank = 'subspecies';
    }
    elseif (preg_match('/^var\.?$/i', $rank)) {
      $rank = 'varietas';
    }
    elseif (preg_match('/^subvar\.?$/i', $rank)) {
      $rank = 'subvarietas';
    }
    elseif (preg_match('/^convar\.?$/i', $rank)) {
      $rank = 'convariety';
    }
    elseif (preg_match('/^cv\.?$/i', $rank)) {
      $rank = 'cultivar';
    }
    elseif (preg_match('/^group$/i', $rank)) {
      $rank = 'cultivar group';
    }
    elseif (preg_match('/^f\.?$/i', $rank)) {
      $rank = 'forma';
    }
    elseif (preg_match('/^subf\.?$/i', $rank)) {
      $rank = 'subforma';
    }
    // If none of the above matched, rank is returned unchanged.
    return $rank;
  }

}
