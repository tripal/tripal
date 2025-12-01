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
   * @var Drupal\tripal_chado\ChadoBuddy\PluginManagers\ChadoBuddyPluginManager
   */
  public ChadoBuddyPluginManager $buddy_manager;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * We are injecting an additional dependency here, the
   * ChadoBuddyPluginManager, so that this buddy can have
   * access to the Dbxref buddy.
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
  public function getOrganism(array $conditions, array $options = []) {
    // $valid_tables = ['cv', 'cvterm', 'db', 'dbxref', 'organism'];
    $valid_tables = ['organism'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->chado_connection->select('1:organism', 'organism');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header.
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    /*
    $query->leftJoin('1:cv', 'cv', 'cvterm.cv_id = cv.cv_id');
    $query->leftJoin('1:dbxref', 'dbxref', 'cvterm.dbxref_id = dbxref.dbxref_id');
    $query->leftJoin('1:db', 'db', 'dbxref.db_id = db.db_id');
     */
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
   *
   * @return \Drupal\tripal_chado\ChadoBuddy\Attribute\ChadoBuddyRecord
   *   The inserted ChadoBuddyRecord will be returned on success and an
   *   exception will be thrown if an error is encountered. If the record
   *   already exists then an error will be thrown. If this is not the desired
   *   behaviour, then use the upsert version of this method.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If an error is encountered.
   */
  public function insertOrganism(array $values, array $options = []) {

    $valid_tables = ['organism'];
    if (array_key_exists('create_cvterm', $options) and $options['create_cvterm']) {
      // @todo perhaps also check here if required keys are provided
      $valid_tables[] = ['cv', 'cvterm', 'db', 'dbxref'];
    }
    $valid_columns = $this->getTableColumns($valid_tables);
    $values = $this->dereferenceBuddyRecord($values);
    $this->validateInput($values, $valid_columns);

    // Check if provided an infraspecific_name for this organism.
    // If so, type_id is also required.
    if (array_key_exists('organism.infraspecific_name', $values)) {
      if (!array_key_exists('organism.type_id', $values)) {
        if (array_key_exists('cvterm.cvterm_id', $values)) {
          $values['organism.type_id'] = $values['cvterm.cvterm_id'];
        }
        elseif ($options['create_cvterm'] ?? FALSE) {
          // If a term was not passed, we can create it if the required
          // fields were included. For safety, this is an opt-in setting.
          // Use the buddy manager dependency to create a Cvterm buddy instance.
          if (!isset($this->cvterm_instance)) {
            $this->cvterm_instance = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
          }
          // Call the Cvterm buddy to perform the insert.
          $cvterm_values = $this->subsetInput($values, ['db', 'dbxref', 'cv', 'cvterm']);
          $cvterm_record = $this->cvterm_instance->upsertCvterm($cvterm_values, $options);
          $type_id = $cvterm_record->getValue('cvterm.cvterm_id');
          $values['organism.type_id'] = $type_id;
        }
        else {
          throw new ChadoBuddyException("ChadoBuddy insertOrganism error, neither cvterm.cvterm_id nor organism.type_id were specified and 'create_cvterm' option is not enabled");
        }
      }
    }

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
   *     organism.type_id when creating this organism, if they do not exist.
   *     NOTE: This is NOT recommended. We suggest you import ontologies first.
   *
   * @return bool|ChadoBuddyRecord
   *   The updated ChadoBuddyRecord will be returned on success, FALSE will be
   *   returned if no record was found to update.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
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
      throw new ChadoBuddyException("ChadoBuddy updateOrganism error, more than one record matched the conditions specified\n" . print_r($conditions, TRUE));
    }

    // Update query will only be based on the organism_id,
    // which we get from the retrieved record.
    $organism_id = $existing_values['organism.organism_id'];
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

}
