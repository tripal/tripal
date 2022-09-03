<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;


/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class ChadoStorageTest extends ChadoTestBrowserBase {

  /**
   * Tests the ChadoIdSpace Class
   *
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testChadoStorage() {
    $storage_manager = \Drupal::service('tripal.storage');
    $chado_storage = $storage_manager->createInstance('chado_storage');

    // All Chado storage testing requires an entity.
    // Luckily we do not need to fully mock one.
    // Specifically we are using a "Gene" entity type (bundle).
    // Stored in the feature Chado table:
    //    name: test_gene_name, uniquename: test_gene_uname, type: gene (SO:0000704)
    //    organism) genus: Oryza, species: sativa, common_name: rice,
    //      abbreviation: O.sativa, infraspecific_name: Japonica,
    //      type: species_group (TAXRANK:0000010), comment: 'This is rice'
    //    Feature Properties)
    //      - type: note (local:note), value: "Note 1", rank: 0
    //      - type: note (local:note), value: "Note 2", rank: 2
    //      - type: note (local:note), value: "Note 3", rank: 1
    // The Gene entity type has 3 fields: Gene Name, Notes, Organism.
    $entity_id = 1;
    $entity_type = 'bio_data_8';
    // Add the organism record.
    $type_term = $this->addTaxRankSubGroupCVTerm();
    $organism = $this->addOryzaSativaRecord($type_term);
    $organism_id = $organism->organism_id;
    // Add the gene record.
    $gene_term = $this->addSOGeneCVterm();
    $gene = $this->addFeatureRecord('test_gene_name', 'test_gene_uname', $gene_term, $organism);
    $feature_id = $gene->feature_id;
    // Add featureprop notes:
    $note_term = $this->addLocalNoteCVTerm();
    $this->addFeaturePropRecords($gene, $note_term, "Note 1", 0);
    $this->addFeaturePropRecords($gene, $note_term, "Note 2", 2);
    $this->addFeaturePropRecords($gene, $note_term, "Note 3", 1);

    // For the ChadoStorage->addTypes() and ChadoStorage->loadValues()
    // We are going to progressively test these methods with more + more fields.
    // Hence, I'm starting the values variable here to be added to as we go.
    // The types will be stored in the $chado_storage service object.
    $values = [];

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // -- Single value + single property field
    // Stored in feature.name; Term: schema:name.
    // Value: test_gene_name
    $field_type  = 'schema__name';
    $field_term_string = 'schema:name';
    $chado_table = 'feature';
    $chado_column = 'name';

    // Testing the Property Type + Value class creation
    // + prepping for future tests.
    // NOTE: You need to set the value = feature_id when creating the record_id StoragePropertyValue.
    $recordId_propertyType = new ChadoIntStoragePropertyType($entity_type, $field_type, 'record_id');
    $recordId_propertyValue = new StoragePropertyValue($entity_type, $field_type, 'record_id', $entity_id, $feature_id);
    $value_propertyType = new ChadoVarCharStoragePropertyType($entity_type, $field_type, 'value');
    $value_propertyValue = new StoragePropertyValue($entity_type, $field_type, 'value', $entity_id);
    $this->assertIsObject($recordId_propertyType, "Unable to create record_id ChadoIntStoragePropertyType: $field_type, record_id");
    $this->assertIsObject($recordId_propertyValue, "Unable to create record_id StoragePropertyValue: $field_type, record_id, $entity_id");
    $this->assertIsObject($value_propertyType, "Unable to create value ChadoIntStoragePropertyType: $field_type, value");
    $this->assertIsObject($value_propertyValue, "Unable to create value StoragePropertyValue: $field_type, value, $entity_id");

    // Make sure the values start empty.
    $this->assertEquals($feature_id, $recordId_propertyValue->getValue(), "The $field_type record_id property should be the feature_id.");
    $this->assertTrue(empty($value_propertyValue->getValue()), "The $field_type value property should not have a value.");

    // Now test ChadoStorage->addTypes()
    // param array $types = Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
    $chado_storage->addTypes([$recordId_propertyType, $value_propertyType]);
    $retrieved_types = $chado_storage->getTypes();
    $this->assertIsArray($retrieved_types, "Unable to retrieve the PropertyTypes after adding $field_type.");
    $this->assertCount(2, $retrieved_types, "Did not revieve the expected number of PropertyTypes after adding $field_type.");

    // We also need FieldConfig classes for loading values.
    // We'll use a mock class here + set the chado mapping manually.
    $fieldconfig = new FieldConfigMock(['field_name' => $field_type, 'entity_type' => $entity_type]);
    $fieldconfig->setMockChadoMapping($chado_table, $chado_column);

    // Next we actually load the values.
    $values[$field_type] = [
      0 => [
        'value'=> [
          'value' => $value_propertyValue,
          'type' => $value_propertyType,
          'definition' => $fieldconfig,
        ],
        'record_id' => [
          'value' => $recordId_propertyValue,
          'type' => $recordId_propertyType,
          'definition' => $fieldconfig,
        ],
      ],
    ];
    $success = $chado_storage->loadValues($values);
    $this->assertTrue($success, "Loading values after adding $field_type was not success (i.e. did not return TRUE).");

    // Then we test that the values are now in the types that we passed in.
    $this->assertEquals('test_gene_name', $values['schema__name'][0]['value']['value']->getValue(), 'The gene name value was not loaded properly.');

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // -- Multi-value single property field
    // Stored in featureprop.value; Term: local:note.
    // Values:
    //   - type: note (local:note), value: "Note 1", rank: 0
    //   - type: note (local:note), value: "Note 2", rank: 2
    //   - type: note (local:note), value: "Note 3", rank: 1
    $field_type = 'local__note';
    $field_term_string = 'local:note';
    $chado_table = 'featureprop';
    $chado_column = 'value';

    // @todo We're not actually ready to test this yet.

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // -- Single value, multi-property field
    // Stored in feature.organism_id; Term: obi:organism.
    // Value: genus: Oryza, species: sativa, common_name: rice,
    //   abbreviation: O.sativa, infraspecific_name: Japonica,
    //   type: species_group (TAXRANK:0000010), comment: 'This is rice'
    $field_type = 'obi__organism';
    $field_term_string = 'obi:organism';
    $chado_table = 'feature';
    $chado_column = 'organism_id';

    // @todo We're not actually ready to test this yet.

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
  protected function addOryzaSativaRecord($type_term) {

    $this->chado->insert('1:organism')
      ->fields([
        'genus' => 'Oryza',
        'species' => 'sativa',
        'common_name' => 'rice',
        'abbreviation' => 'O.sativa',
        'infraspecific_name' => 'Japonica',
        'type_id' => $type_term->getInternalId(),
        'comment' => 'This is rice'
      ])
      ->execute();

    return $this->chado->select('1:organism', 'O')
      ->fields('O')
      ->condition('species', 'sativa')
      ->execute()
      ->fetchObject();
  }

  /**
   * A helper function to add the SO:0000704 (gene) term.
   * @return unknown
   */
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
    return $gene;
  }

  /**
   * A helper function for adding a gene recrod to the feature table.
   */
  protected function addFeatureRecord($name, $uniquename, $type, $organism) {

    $this->chado->insert('1:feature')
      ->fields([
        'name' => $name,
        'uniquename' => $uniquename,
        'type_id' => $type->getInternalId(),
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
        'type_id' => $term->getInternalId(),
        'value' => $value,
        'rank' => $rank,
      ])
      ->execute();
  }
}
