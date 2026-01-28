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
 * Plugin implementation of the chado stock buddy.
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
   * Used to store the valid tables for obtaining/inserting an organism record.
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
   *     - an organism ChadoBuddyRecord can be used in place of or in addition
   *       to other keys.
   *   The following terms are valid where it pertains to stock.type_id only:
   *     - cvterm.name
   *     - cvterm.is_obsolete
   *     - cv.name
   *     - a cvterm ChadoBuddyRecord can be used in place of or in addition to
   *       other keys.
   *   The following terms are valid where it pertains to stock.dbxref_id only:
   *     - dbxref.accession
   *     - db.name
   *     - a dbxref ChadoBuddyRecord can be used in place of or in addition to
   *       other keys.
   * @param array $options
   *   (Optional) Associative array of options with these supported keys:
   *     - 'case_insensitive' - a single key, or an array of keys
   *       to query case insensitively.
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

    // @todo How do we handle input of multiple dbxref identifiers using the
    // stock_dbxref table?
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

}
