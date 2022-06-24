<?php 

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
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
  protected function addTripalBundleRecord($entity_type, $cvterm, $label) {
    $public = \Drupal::database();
    $public->insert('tripal_bundle')
      ->fields([
        'type' => 'TripalEntity',
        'term_id' => $cvterm->cvterm_id,
        'name' => $entity_type,
        'label' => $label
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
   * A helper function to add the local::note term to Chado.
   */
  protected function addLocalNoteCVTerm() {
    
    // First add the vocabulary term for the organism.type_id column.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $local = $idsmanager->createCollection('local', 'chado_id_space');
    $vmanager->createCollection('local', 'chado_vocabulary');
    $note = new TripalTerm([
      'name' => 'note',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'accession' => 'note',
    ]);
    $local->saveTerm($note);
    return $note;
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
      ->fields('O')
      ->condition('species', 'sativa')
      ->execute()
      ->fetchObject();
  }
  
  protected function addSOGeneCVterm() {
    // First add the vocabulary term for the organism.type_id column.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $sequence = $idsmanager->createCollection('SO', 'chado_id_space');
    $vmanager->createCollection('sequence', 'chado_vocabulary');
    $gene = new TripalTerm([
      'name' => 'gene',
      'idSpace' => 'SO',
      'vocabulary' => 'sequence',
      'accession' => '0000704)',
    ]);
    $sequence->saveTerm($gene);
    return $sequence;
  }
  
  /**
   * A helper function for adding a gene recrod to the feature table.
   */
  protected function addFeatureRecord($name, $uniquename, $type, $organism) {
    
    $this->chado->insert('1:feature')
      ->fields([
        'name' => $name,
        'uniquename' => $uniquename,
        'type_id' => $type->cvterm_id,
        'organism_id' => $organism->organism_id,
      ])
      ->execute();
    
    return $this->chado->select('1:feature', 'F')
      ->fields('F')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();
  }
  
  /**
   * A helper function for adding notes values to the featureprop table.
   */
  protected function addFeaturePropRecords($feature, $term, $value, $rank) {
    $this->chado->insert('1:featureprop')
      ->fields([
        'feature_id' => $feature->feature_id,
        'type_id' => $term->cvterm_id,
        'value' => $value,
        'rank' => $rank,
      ])
      ->execute();    
  }
  
  /**
   * A helper function to add a record to the `chado_bio_data_x` table.
   * 
   * @param string $field_type
   * @param int $organism_id
   * @param int $entity_id
   */
  protected function addChadoBioDataRecord($entity_type, $entity_id, $record_id) {
    $public = \Drupal::database();
    $public->insert('chado_' . $entity_type)
      ->fields([
        'entity_id' => $entity_id,
        'record_id' => $record_id,
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

    // We'll simulare a "Gene" entity type (bundle).
    $entity_id = 1;
    $entity_type = 'bio_data_8';
    
    // Simularte A field with one property value.
    $name_field_type  = 'schema__name';
    
    // Simulate A field with multiple values for one property.
    $prop_field_type = 'local__note';
    
    // Simluate Acomplex field with multiple properties.
    $organism_field_type = 'obi__organism';

     
    // 
    // Populate Chado with Data for Testing.
    // 
        
    // Add the organism record.
    $this->addTaxRankSubGroupCVTerm();
    $organism = $this->addOryzaSativaRecord();
    
    // Add the gene record.
    $this->addSOGeneCVterm();
    $gene_term = $this->getCVterm('sequence', 'gene');
    $gene = $this->addFeatureRecord('test_gene_name', 'test_gene_uname', $gene_term, $organism);
    
    // Add featureprop notes:
    $this->addLocalNoteCVTerm();
    $note_term = $this->getCVterm('local', 'note');    
    $this->addFeaturePropRecords($gene, $note_term, "Note 1", 0);
    $this->addFeaturePropRecords($gene, $note_term, "Note 2", 2);
    $this->addFeaturePropRecords($gene, $note_term, "Note 3", 1);
    
    // Create entries in the `tripal_bundle`, `chado_bundle` and
    // `chado_bio_data_x` tables.
    // @todo the createEntityTypeTable() creates the chado_bio_data_x table
    // and it can be removed from this test once those tables are created
    // by the prepare step. At the time of writing this test that part
    // isn't yet working.
    $this->createEntityTypeTable($entity_type);
    $bundle_id = $this->addTripalBundleRecord($entity_type, $gene_term, 'Gene');
    $this->addChadoBundleRecord($bundle_id, 'feature', NULL, 'type_id', $gene_term->cvterm_id);
    $this->addChadoBioDataRecord($entity_type, $entity_id, $gene->feature_id);
    
    
    //
    // Create Properties
    // 
    
    // Note: we won't do any assertions of these constructors because they
    // should be tested in the Tripal module and the Chado implementations
    // just stores Chado table info and if that's broken all the tests
    // below will fail.
    
    // The gene name field single property.
    $name_type = new ChadoIntStoragePropertyType($entity_type, $name_field_type, 'schema:name', 
      ['chado_table' => 'feature', 
       'chado_column' => 'name']);
    $name_value = new StoragePropertyValue($entity_type, $name_field_type, 'schema:name', $entity_id);
    
    // The organism complex field properties.
    $abbreviation_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'local:abbreviation', 255, 
      ['chado_table' => 'organism', 
       'chado_column' => 'abbreviation', 
       'base_table_fk_column' => 'organism_id']);
    $genus_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000005', 255, 
      ['chado_table' => 'organism', 
       'chado_column' => 'genus', 
       'base_table_fk_column' => 'organism_id']);
    $species_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000006', 255, 
      ['chado_table' => 'organism', 
       'chado_column' => 'species', 
       'base_table_fk_column' => 'organism_id']);
    $common_name_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'NCBITaxon:common_name', 255, 
      ['chado_table' => 'organism', 
       'chado_column' => 'common_name', 
       'base_table_fk_column' => 'organism_id']);
    $infra_name_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000045', 1024, 
      ['chado_table' => 'organism', 
       'chado_column' => 'infraspecific_name', 
       'base_table_fk_column' => 'organism_id']);
    $type_id_type = new ChadoIntStoragePropertyType($entity_type, $organism_field_type, 'local:infraspecific_type', 
      ['chado_table' => 'organism', 
       'chado_column' => 'type_id', 
       'base_table_fk_column' => 'organism_id']);    
    $abbreviation_value = new StoragePropertyValue($entity_type, $organism_field_type, 'local:abbreviation', $entity_id);
    $genus_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000005', $entity_id);
    $species_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000006', $entity_id);
    $common_name_value = new StoragePropertyValue($entity_type, $organism_field_type, 'NCBITaxon:common_name', $entity_id);
    $infra_name_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000045', $entity_id);
    $type_id_value = new StoragePropertyValue($entity_type, $organism_field_type, 'local:infraspecific_type', $entity_id);    
    // @todo add the comment field once the Text property type is impelmented
    
    // The note field single property with multiple values.
    $note_type = new ChadoIntStoragePropertyType($entity_type, $prop_field_type, 'local:note', 
      ['chado_table' => 'featureprop', 
       'chado_column' => 'value', 
       'type_column' => 'type_id', 
       'type_id' => $note_term->cvterm_id,
     'chado_table_fk_column' => 'feature_id']);
    $note_value = new StoragePropertyValue($entity_type, $prop_field_type, 'local:note', $entity_id);
    
    // Bad properties.
    $bad_type = new ChadoIntStoragePropertyType($entity_type, $organism_field_type, 'organism_id', 
      ['chado_table' => 'feature2', 
       'chado_column' => 'organism_id']);
    $bad_value = new StoragePropertyValue($entity_type, $organism_field_type, 'organism_id', $entity_id);
    
    
    //
    // Test Property Type and Value Creation.
    //
    
    // Test validity of fields.
    $this->assertFalse($bad_type->isValid(), 'The bad property type should be invalid.');
    $this->assertTrue($abbreviation_type->isValid(), 'The abbreviation property type should be valid.');
    $this->assertTrue($genus_type->isValid(), 'The genus property type should be valid.');
    $this->assertTrue($species_type->isValid(), 'The species property type should be valid.');
    $this->assertTrue($common_name_type->isValid(), 'The common name property type should be valid.');
    $this->assertTrue($infra_name_type->isValid(), 'The infraspecific name property type should be valid.');
    $this->assertTrue($type_id_type->isValid(), 'The type_id property type should be valid.');
    $this->assertTrue($name_type->isValid(), 'The name property type should be valid.');
    $this->assertTrue($note_type->isValid(), 'The note property type should be valid.');
    
   
    // 
    // Testing Loading of Property Values from Chado.
    //
    
    // Make sure the values start as empty.
    $this->assertTrue(empty($abbreviation_value->getValue()), 'The abbreviation property should not have a value.');
    $this->assertTrue(empty($genus_value->getValue()), 'The genus property should not have a value.');
    $this->assertTrue(empty($species_value->getValue()), 'The species property should not have a value.');
    $this->assertTrue(empty($common_name_value->getValue()), 'The species property should not have a value.');
    $this->assertTrue(empty($infra_name_value->getValue()), 'The infraspecific name property should not have a value.');
    $this->assertTrue(empty($type_id_value->getValue()), 'The infraspecific type_id property should not have a value.');
    $this->assertTrue(empty($name_value->getValue()), 'The name property should not have a value.');
    $this->assertTrue(empty($note_value->getValue()), 'The note property should not have a value.');
    $this->assertTrue(empty($bad_value->getValue()), 'The bad property should not have a value.');
    
    // Add the types and load the values.
    $types = [
      $abbreviation_type, $genus_type, $species_type, $common_name_type, 
      $infra_name_type, $type_id_type, $bad_type, $name_type, $note_type
    ];    
    $values = [
      $abbreviation_value, $genus_value, $species_value, $common_name_value, 
      $infra_name_value, $type_id_value, $bad_value, $name_value, $note_value 
    ];
    $chado_storage->addTypes($types);
    $chado_storage->loadValues($values);
    
    // Tests loading a property from a base table with a single value.
    $this->assertTrue($name_value->getValue() == 'test_gene_name', 'The name value was not loaded properly.');
    
    // Tests loading properites each with a single value from a linking table
    // where the foreign key is in the base table.
    $this->assertTrue($abbreviation_value->getValue() == 'O.sativa', 'The abbreviation value was not loaded properly.');
    $this->assertTrue($genus_value->getValue() == 'Oryza', 'The genus value was not loaded properly.');
    $this->assertTrue($species_value->getValue() == 'sativa', 'The species value was not loaded properly.');
    $this->assertTrue($common_name_value->getValue() == 'rice', 'The species common name value was not loaded properly.');
    $this->assertTrue($infra_name_value->getValue() == 'Japonica', 'The infraspecific name value was not loaded properly.');
    $this->assertTrue($type_id_value->getValue() == '2', 'The infraspecific type_id value was not loaded properly.');
    
    // Tests loading a property from a linking table where the forkeign key
    // is in the linking table and the property has mutiple values.
    $this->assertTrue(is_array($note_value->getValue()), 'The note value should be an array.');
    $this->assertTrue(count($note_value->getValue()) == 3, 'The note value had the wrong number of elements.');
    $this->assertTrue($note_value->getValue()[0] == 'Note 1', 'The note first element is incorrect.');
    $this->assertTrue($note_value->getValue()[1] == 'Note 3', 'The note second element is incorrect.');
    $this->assertTrue($note_value->getValue()[2] == 'Note 2', 'The note third element is incorrect.');
    $this->assertTrue(empty($bad_value->getValue()), 'The bad property should have no value');   
  }
}


