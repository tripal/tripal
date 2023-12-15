<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\Entity\TripalEntity;
use \Drupal\tripal\Entity\TripalEntityType;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;


class TripalEntityLookup {

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * A logger object.
   *
   * @var TripalLogger $logger
   */
  protected $logger;

  /**
   * Constructor
   */
  public function __construct(
//TripalIdSpaceManager $idSpaceManager,
//      TripalVocabularyManager $vocabularyManager, 
//TripalLogger $logger
//    $this->idSpaceManager = $idSpaceManager;
//    $this->vocabularyManager = $vocabularyManager;
) {
//    $this->logger = $logger;
  }

  /**
   * Retrieve the entity corresponding to a record in a table.
   *
   * @param string $table
   *   The table name
   * @param int $pkey_id
   *   The primary key value in this table
   */
  public function getEntity($table, $pkey_id) {
    $entities = [];
dpm("CP1 getEntity table=\"$table\" pkey_id=\"$pkey_id\""); //@@@

    // Get the name of the primary key for this table
    $pkey = 'project_id';  //@@@

    $chado = \Drupal::service('tripal_chado.database');
    $query = $chado->select($table, 't');
    $query->fields('t', [$pkey]);

//    $query->leftJoin('cvterm', 'cvt', 'c.type_id = cvt.cvterm_id');
//    $query->addField('cvt', 'name', 'contact_type');

    $query->condition('t.' . $pkey, $pkey_id, '=');
    $results = $query->execute();
    while ($result = $results->fetchObject()) {
      $x = $result->$pkey;
dpm($x, "pkey value");
    }




    return $entities;
  }

}
