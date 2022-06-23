<?php 

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;



/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class ChadoStorageTest extends ChadoTestBrowserBase {
  
  /**
   * A helper function to retrieve a Chado cvterm record.
   *
   * @param string $cvname
   * @param string $cvterm_name
   */
  protected function getCVterm($cvname, $cvterm_name) {
    $query = $this->chado->select('1:cvterm', 'CVT');
    $query->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
    $query->fields('CVT', ['cv_id', 'name', 'cvterm_id', 'definition', 'is_obsolete', 'is_relationshiptype'])
      ->condition('CVT.name', $cvterm_name, '=')
      ->condition('CV.name', $cvname, '=');
    $result = $query->execute();
    if (!$result) {
      return [];
    }
    return $result->fetchObject();
  }
  
  /**
   * A helper function to create the `bio_data_x` table.
   * 
   * @param string $entity_type
   */
  protected function createEntityTypeTable($entity_type) {
    $table_def = [
      'description' => 'The linker table that associates TripalEntities with Chado records for entities of type ' . $entity_type . '.',
      'fields' => [
        'mapping_id' => [
          'type' => 'serial',
          'not null' => TRUE,
        ],
        'entity_id' => [
          'description' => 'The unique entity id.',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'record_id' => [
          'description' => 'The unique numerical identifier for the record that this entity is associated with (e.g. feature_id, stock_id, library_id, etc.).',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'nid' => [
          'description' => 'Optional. For linking nid to the entity when migrating Tripal v2 content',
          'type' => 'int',
        ],
      ],
      'primary key' => [
        'mapping_id',
      ],
      'indexes' => [
        'record_id' => ['record_id'],
        'entity_id' => ['entity_id'],
        'nid' => ['nid'],
      ],
      'unique keys' => [
        'table_record' => ['record_id'],
        'entity_id' => ['entity_id'],
      ],
    ];
    $public = \Drupal::database();
    $public->schema()->createTable('chado_' . $entity_type, $table_def);   
  }
  
  /**
   * A helper function to add a recrod to the `tripal_bundle` table.
   * 
   * A bundle_id is needed for the `chado_bundle` table that maps
   * entity types to chado tables.
   * 
   * @param string $entity_type
   */
  protected function addTripalBundleRecord($entity_type) {
    $public = \Drupal::database();
    $public->insert('tripal_bundle')
      ->fields([
        'type' => 'TripalEntity',
        'term_id' => 1,
        'name' => $entity_type,
        'label' => 'Organism'
      ])
      ->execute();
    
    return $public->select('tripal_bundle', 'TB')
      ->fields('TB', ['id'])
      ->condition('TB.name', $entity_type)
      ->execute()
      ->fetchField();
  }
 
  
  /**
   * a helper function to add a record to the `chado_bundle` table. 
   */
  protected function addChadoBundleRecord($bundle_id, $data_table, 
      $type_linker_table = NULL, $type_column = NULL, $type_id = NULL, 
      $type_value = NULL, $base_type_id = NULL) {
    
    $public = \Drupal::database();
    $public->insert('chado_bundle')
      ->fields([
        'bundle_id' => $bundle_id,
        'data_table' => $data_table,
        'type_linker_table' => $type_linker_table,
        'type_column' => $type_column,
        'type_id' => $type_id,
        'type_value' => $type_value,
        'base_type_id' => $base_type_id,
      ])
      ->execute();
    
    return $public->select('chado_bundle', 'CB')
      ->fields('CB')
      ->condition('CB.bundle_id', $bundle_id, '')
      ->execute()
      ->fetchObject();
    
  }
  
  /**
   * A helper function to add the TAXRANK:species_subgroup term to Chado.
   */
  protected function addTaxRankSubGroupCVTerm() {
    
    // First add the vocabulary term for the organism.type_id column.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $taxrank = $idsmanager->createCollection('TAXRANK', 'chado_id_space');
    $vmanager->createCollection('taxonomic_rank', 'chado_vocabulary');
    $species_group = new TripalTerm([
      'name' => 'species_group',
      'idSpace' => 'TAXRANK',
      'vocabulary' => 'taxonomic_rank',
      'accession' => '0000010',
    ]);
    $taxrank->saveTerm($species_group);
    return $species_group;
  }
  
  
  /**
   * A helper function to add an organism record to Chado.
   */
  protected function addOryzaSativaRecord() {
    
    // @todo Ask Josh about adding an internalID to the TripalTerm class so I
    // don't have to do a TripalDBX to get the cvterm_id from it.
    $cvterm = $this->getCVterm('taxonomic_rank', 'species_group');
    
    $this->chado->insert('1:organism')
      ->fields([
        'genus' => 'Oryza',
        'species' => 'sativa',
        'common_name' => 'rice',
        'abbreviation' => 'O.sativa',
        'infraspecific_name' => 'Japonica',
        'type_id' => $cvterm->cvterm_id,
      ])
      ->execute();
    
    return $this->chado->select('1:organism', 'O')
      ->fields('O', ['organism_id'])
      ->condition('species', 'sativa')
      ->execute()
      ->fetchField();
  }
  
  /**
   * A helper function to add a record to the `chado_bio_data_x` table.
   * 
   * @param string $field_type
   * @param int $organism_id
   * @param int $entity_id
   */
  protected function addChadoBioDataRecord($entity_type, $organism_id, $entity_id) {
    $public = \Drupal::database();
    $public->insert('chado_' . $entity_type)
      ->fields([
        'entity_id' => $entity_id,
        'record_id' => $organism_id,
      ])
      ->execute();      
  }
  
  /**
   * Tests the ChadoIdSpace Class
   *
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testChadoStorage() {
    
    $storage_manager = \Drupal::service('tripal.storage');
    $chado_storage = $storage_manager->createInstance('chado_storage');
           
    $entity_id = 1;
    $entity_type = 'bio_data_1';
    $field_type = 'obi__organism';
    
    // Similate the existence of a Tripal Bundle with the needed Chado records.
    // @todo: we can should renove the calls to the first four functions below
    // when the prepare step of the installation is complete so that we have a
    // test environment with a properly prepared Chado. For now we fake it.
    $this->createEntityTypeTable($entity_type);
    $bundle_id = $this->addTripalBundleRecord($entity_type);
    $chado_bundle = $this->addChadoBundleRecord($bundle_id, 'organism');
    $species_group = $this->addTaxRankSubGroupCVTerm();
    $organism_id = $this->addOryzaSativaRecord();
    $this->addChadoBioDataRecord($entity_type, $organism_id, $entity_id);
    
    
    //
    // Test Property Type and Value Creation.
    //
    
    // Create some properties that will correspond to an entry in the
    // organism table of Chado.    
    $organism_id_type = new IntStoragePropertyType($entity_type, $field_type, 'organism_id');
    $abbreviation_type = new VarCharStoragePropertyType($entity_type, $field_type, 'abbreviation', 255);
    $genus_type = new VarCharStoragePropertyType($entity_type, $field_type, 'genus', 255);
    $species_type = new VarCharStoragePropertyType($entity_type, $field_type, 'species', 255);    
    $common_name_type = new VarCharStoragePropertyType($entity_type, $field_type, 'common_name', 255);
    $infra_name_type = new VarCharStoragePropertyType($entity_type, $field_type, 'infraspecific_name', 1024);
    $type_id_type = new IntStoragePropertyType($entity_type, $field_type, 'type_id');    
    //$comment_type = new TextStoragePropertyType($entity_type, $field_type, 'comment');
        
    // Add the properties types.
    $types = [$organism_id_type, $abbreviation_type, $genus_type,
      $species_type, $common_name_type, $infra_name_type, $type_id_type      
    ]; # $comment_type   
    
    $chado_storage->addTypes($types); 
    
   
   
    // 
    // Testing Loading of Data.
    //
    $organism_id_value = new StoragePropertyValue($entity_type, $field_type, 'organism_id', $entity_id);
    $abbreviation_value = new StoragePropertyValue($entity_type, $field_type, 'abbreviation', $entity_id);
    $genus_value = new StoragePropertyValue($entity_type, $field_type, 'genus', $entity_id);
    $species_value = new StoragePropertyValue($entity_type, $field_type, 'species', $entity_id);
    $common_name_value = new StoragePropertyValue($entity_type, $field_type, 'common_name', $entity_id);
    $infra_name_value = new StoragePropertyValue($entity_type, $field_type, 'infraspecific_name', $entity_id);
    $type_id_value = new StoragePropertyValue($entity_type, $field_type, 'type_id', $entity_id);
    //$comment_value = new StoragePropertyValue($entity_type, $field_type, 'comment', $entity_id);
    
    $values = [$organism_id_value, $abbreviation_value, $genus_value, $species_value, 
      $common_name_value, $infra_name_value, $type_id_value
    ]; #$comment_value
    
    $chado_storage->loadValues($values);
    
     
  }
}


