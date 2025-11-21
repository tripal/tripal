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
   * Used to store the manager so we can create a buddy.
   */
  protected object $buddy_manager;

  /**
   * Provide the dbxref instance.
   */
  protected object $dbxref_instance;

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
      $container->get('tripal_chado.chado_buddy')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(
    array $configuration,
    $plugin_id,
    $plugin_definition,
    ChadoConnection $connection,
    ChadoBuddyPluginManager $buddy_manager,
  ) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $connection);
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
   *   (Optional)
   *   Associative array of options.
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
    $valid_tables = ['organism', 'cv', 'cvterm', 'db', 'dbxref'];
    $valid_columns = $this->getTableColumns($valid_tables);
    $conditions = $this->dereferenceBuddyRecord($conditions);
    $this->validateInput($conditions, $valid_columns);

    $query = $this->connection->select('1:organism', 'organism');

    // Return the joined fields aliased to the unique names
    // as listed in this function's header.
    foreach ($valid_columns as $key) {
      $parts = explode('.', $key);
      $query->addField($parts[0], $parts[1], $this->makeAlias($key));
    }
    $query->leftJoin('1:cv', 'cv', 'cvterm.cv_id = cv.cv_id');
    $query->leftJoin('1:dbxref', 'dbxref', 'cvterm.dbxref_id = dbxref.dbxref_id');
    $query->leftJoin('1:db', 'db', 'dbxref.db_id = db.db_id');
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
      $new_record->setSchemaName($this->connection->getSchemaName());
      $new_record->setBaseTable('organism');
      foreach ($values as $key => $value) {
        $new_record->setValue($this->unmakeAlias($key), $value);
      }
      $buddies[] = $new_record;
    }

    return $buddies;
  }

}
