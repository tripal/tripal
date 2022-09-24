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
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'field_ui'];

  /**
   * Tests the ChadoIdSpace Class
   *
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testChadoStorage() {
    $storage_manager = \Drupal::service('tripal.storage');
    $chado_storage = $storage_manager->createInstance('chado_storage');

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // All Chado storage testing requires an entity.
    $content_entity = $this->createTripalContent();
    $content_entity_id = $content_entity->id();
    $content_type = $content_entity->getType();
    $content_type_obj = \Drupal\tripal\Entity\TripalEntityType::load($content_type);

    // Specifically we are mocking our example based on a "Gene" entity type (bundle).
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
    $field_name  = 'schema__name';
    $field_type = 'tripal_string_type';
    $field_term_string = 'schema:name';
    $chado_table = 'feature';
    $chado_column = 'name';
    $cardinality = 1;
    $is_required = TRUE;
    $term = $this->createTripalTerm([
      'vocab_name' => 'Schema',
      'id_space_name' => 'schema',
      'term' => [
        'accession' => 'name',
        'name' => 'name',
      ],
    ]);

    // Testing the Property Type + Value class creation
    // + prepping for future tests.
    // NOTE: You need to set the value = feature_id when creating the record_id StoragePropertyValue.
    $recordId_propertyType = new ChadoIntStoragePropertyType($content_type, $field_name, 'record_id');
    $recordId_propertyValue = new StoragePropertyValue($content_type, $field_name, 'record_id', $content_entity_id, $feature_id);
    $value_propertyType = new ChadoVarCharStoragePropertyType($content_type, $field_name, 'value');
    $value_propertyValue = new StoragePropertyValue($content_type, $field_name, 'value', $content_entity_id);
    $this->assertIsObject($recordId_propertyType, "Unable to create record_id ChadoIntStoragePropertyType: $field_name, record_id");
    $this->assertIsObject($recordId_propertyValue, "Unable to create record_id StoragePropertyValue: $field_name, record_id, $content_entity_id");
    $this->assertIsObject($value_propertyType, "Unable to create value ChadoIntStoragePropertyType: $field_name, value");
    $this->assertIsObject($value_propertyValue, "Unable to create value StoragePropertyValue: $field_name, value, $content_entity_id");

    // Make sure the values start empty.
    $this->assertEquals($feature_id, $recordId_propertyValue->getValue(), "The $field_name record_id property should be the feature_id.");
    $this->assertTrue(empty($value_propertyValue->getValue()), "The $field_name value property should not have a value.");

    // Now test ChadoStorage->addTypes()
    // param array $types = Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
    $chado_storage->addTypes([$recordId_propertyType, $value_propertyType]);
    $retrieved_types = $chado_storage->getTypes();
    $this->assertIsArray($retrieved_types, "Unable to retrieve the PropertyTypes after adding $field_name.");
    $this->assertCount(2, $retrieved_types, "Did not revieve the expected number of PropertyTypes after adding $field_name.");

    // We also need FieldConfig classes for loading values.
    // We're going to create a TripalField and see if that works.
    $field_values = [];
    $field_values['field_name'] = $field_name;
    $field_values['field_type'] = $field_type;
    $field_values['is_required'] = $is_required;
    $field_values['term'] = $term;
    $field_values['storage_settings'] = [
      'storage_plugin_id' => 'chado_storage',
      'storage_plugin_settings' => [
        'base_table' => $chado_table,
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => $chado_table,
            'chado_column' => $chado_column,
          ]
        ],
      ],
    ];
    $field = $this->createTripalField($content_type, $field_values);
    // $fieldconfig = new FieldConfigMock(['field_name' => $field_name, 'entity_type' => $content_type]);
    // $fieldconfig->setMockChadoMapping($chado_table, $chado_column);
    $fieldconfig = $field;

    // Next we actually load the values.
    /**
    $values[$field_name] = [
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
    $this->assertTrue($success, "Loading values after adding $field_name was not success (i.e. did not return TRUE).");

    // Then we test that the values are now in the types that we passed in.
    $this->assertEquals('test_gene_name', $values['schema__name'][0]['value']['value']->getValue(), 'The gene name value was not loaded properly.');
    */

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // -- Multi-value single property field
    // Stored in featureprop.value; Term: local:note.
    // Values:
    //   - type: note (local:note), value: "Note 1", rank: 0
    //   - type: note (local:note), value: "Note 2", rank: 2
    //   - type: note (local:note), value: "Note 3", rank: 1
    $field_name = 'local__note';
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
    $field_name = 'obi__organism';
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
