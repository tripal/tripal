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

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // -- Single value + single property field
    // Stored in feature.name; Term: schema:name.
    // Value: test_gene_name
    $field_name  = 'schema__name';
    $field_label = 'Gene Name';
    $field_type = 'tripal_string_type';
    $field_term_string = 'schema:name';
    $chado_table = 'feature';
    $chado_column = 'name';
    $cardinality = 1;
    $is_required = TRUE;
    $propsettings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => $chado_column,
    ];
    $storage_settings = [
      'storage_plugin_id' => 'chado_storage',
      'storage_plugin_settings' => [
        'base_table' => $chado_table,
        'property_settings' => [
          'value' => $propsettings,
        ],
      ],
    ];

    // Testing the Property Type + Value class creation
    // + prepping for future tests.
    // NOTE: You need to set the value = feature_id when creating the record_id StoragePropertyValue.
    $recordId_propertyType = new ChadoIntStoragePropertyType($content_type, $field_name, 'record_id', $propsettings);
    $recordId_propertyValue = new StoragePropertyValue($content_type, $field_name, 'record_id', $content_entity_id, $feature_id);
    $value_propertyType = new ChadoVarCharStoragePropertyType($content_type, $field_name, 'value', 255, $propsettings);
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
    $fieldconfig = new FieldConfigMock(['field_name' => $field_name, 'entity_type' => $content_type]);
    $fieldconfig->setMock(['label' => $field_label, 'settings' => $storage_settings]);

    // Next we actually load the values.
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

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // -- Single value, multi-property field
    // Stored in feature.organism_id; Term: obi:organism.
    // Value: genus: Oryza, species: sativa, common_name: rice,
    //   abbreviation: O.sativa, infraspecific_name: Japonica,
    //   type: species_group (TAXRANK:0000010), comment: 'This is rice'
    $field_name = 'obi__organism';
    $field_label = 'Organism';
    $field_type = 'obi__organism';
    $field_term_string = 'obi:organism';
    $chado_table = 'feature';
    $chado_column = 'organism_id';
    $cardinality = 1;
    $is_required = TRUE;
    $propsettings = [
      'action' => 'store',
      'chado_table' => $chado_table,
      'chado_column' => $chado_column,
    ];
    $storage_settings = [
      'storage_plugin_id' => 'chado_storage',
      'storage_plugin_settings' => [
        'base_table' => $chado_table,
        'property_settings' => [
          'value' => $propsettings,
        ],
      ],
    ];

    // Testing the Property Type class creation.
    $base_table = $chado_table;
    $value_settings = $propsettings;
    $label_settings = [
      'action' => 'replace',
      'template' => "<i>[TAXRANK:0000005] [TAXRANK:0000006]</i> [TAXRANK:0000046] [TAXRANK:0000047]",
    ];
    $genus_settings = [
      'action' => 'join',
      'path' => $base_table . '.organism_id>organism.organism_id',
      'chado_column' => 'genus'
    ];
    $species_settings = [
      'action' => 'join',
      'path' => $base_table . '.organism_id>organism.organism_id',
      'chado_column' => 'species'
    ];
    $iftype_settings = [
      'action' => 'join',
      'path' => $base_table . '.organism_id>organism.organism_id;organism.type_id>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'infraspecific_type_name'
    ];
    $ifname_settings = [
      'action' => 'join',
      'path' => $base_table . '.organism_id>organism.organism_id',
      'chado_column' => 'infraspecific_name',
    ];
    $recordId_propertyType = new ChadoIntStoragePropertyType($content_type, $field_name, 'record_id', $propsettings);
    $value_propertyType = new ChadoIntStoragePropertyType($content_type, $field_name, 'value', $value_settings);
    $label_propertyType = new ChadoVarCharStoragePropertyType($content_type, $field_name, 'NCBITaxon:common_name', 125, $label_settings);
    $genus_propertyType = new ChadoVarCharStoragePropertyType($content_type, $field_name, 'TAXRANK:0000005', 125, $genus_settings);
    $species_propertyType = new ChadoVarCharStoragePropertyType($content_type, $field_name, 'TAXRANK:0000006', 125, $species_settings);
    $ifname_propertyType = new ChadoVarCharStoragePropertyType($content_type, $field_name, 'TAXRANK:0000047', 125, $ifname_settings);
    $iftype_propertyType = new ChadoIntStoragePropertyType($content_type, $field_name, 'TAXRANK:0000046', $iftype_settings);
    $this->assertIsObject($recordId_propertyType, "Unable to create the ChadoIntStoragePropertyType: $field_name, record_id");
    $this->assertIsObject($value_propertyType, "Unable to create the ChadoIntStoragePropertyType: $field_name, value");
    $this->assertIsObject($label_propertyType, "Unable to create the ChadoVarCharStoragePropertyType: $field_name, label");
    $this->assertIsObject($genus_propertyType, "Unable to create the ChadoVarCharStoragePropertyType: $field_name, genus");
    $this->assertIsObject($species_propertyType, "Unable to create the ChadoVarCharStoragePropertyType: $field_name, species");
    $this->assertIsObject($ifname_propertyType, "Unable to create the ChadoVarCharStoragePropertyType: $field_name, ifname");
    $this->assertIsObject($iftype_propertyType, "Unable to create the ChadoIntStoragePropertyType: $field_name, iftype");

    // Testing the Property Value class creation.
    $recordId_propertyValue = new StoragePropertyValue($content_type, $field_name, 'record_id', $content_entity_id, $organism_id);
    $value_propertyValue = new StoragePropertyValue($content_type, $field_name, 'value', $content_entity_id);
    $label_propertyValue = new StoragePropertyValue($content_type, $field_name, 'NCBITaxon:common_name', $content_entity_id);
    $genus_propertyValue = new StoragePropertyValue($content_type, $field_name, 'TAXRANK:0000005', $content_entity_id);
    $species_propertyValue = new StoragePropertyValue($content_type, $field_name, 'TAXRANK:0000006', $content_entity_id);
    $ifname_propertyValue = new StoragePropertyValue($content_type, $field_name, 'TAXRANK:0000047', $content_entity_id);
    $iftype_propertyValue = new StoragePropertyValue($content_type, $field_name, 'TAXRANK:0000046', $content_entity_id);
    $this->assertIsObject($value_propertyValue, "Unable to create the StoragePropertyValue: $field_name, value");
    $this->assertIsObject($label_propertyValue, "Unable to create the StoragePropertyValue: $field_name, label");
    $this->assertIsObject($genus_propertyValue, "Unable to create the StoragePropertyValue: $field_name, genus");
    $this->assertIsObject($species_propertyValue, "Unable to create the StoragePropertyValue: $field_name, species");
    $this->assertIsObject($ifname_propertyValue, "Unable to create the StoragePropertyValue: $field_name, ifname");
    $this->assertIsObject($iftype_propertyValue, "Unable to create the StoragePropertyValue: $field_name, iftype");

    // Make sure the values start empty.
    $this->assertEquals($organism_id, $recordId_propertyValue->getValue(), "The $field_name record_id property should be the organism_id.");
    $this->assertTrue(empty($value_propertyValue->getValue()), "The $field_name value property should not have a value.");
    $this->assertTrue(empty($label_propertyValue->getValue()), "The $field_name label property should not have a value.");
    $this->assertTrue(empty($genus_propertyValue->getValue()), "The $field_name genus property should not have a value.");
    $this->assertTrue(empty($species_propertyValue->getValue()), "The $field_name species property should not have a value.");
    $this->assertTrue(empty($ifname_propertyValue->getValue()), "The $field_name ifname property should not have a value.");
    $this->assertTrue(empty($iftype_propertyValue->getValue()), "The $field_name iftype property should not have a value.");

    // Now test ChadoStorage->addTypes()
    // param array $types = Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
    $chado_storage->addTypes([$recordId_propertyType, $value_propertyType, $label_propertyType, $genus_propertyType, $species_propertyType, $ifname_propertyType, $iftype_propertyType]);
    $retrieved_types = $chado_storage->getTypes();
    $this->assertIsArray($retrieved_types, "Unable to retrieve the PropertyTypes after adding $field_name.");
    $this->assertCount(9, $retrieved_types, "Did not revieve the expected number of PropertyTypes after adding $field_name.");

    // We also need FieldConfig classes for loading values.
    // We're going to create a TripalField and see if that works.
    $fieldconfig = new FieldConfigMock(['field_name' => $field_name, 'entity_type' => $content_type]);
    $fieldconfig->setMock(['label' => $field_label, 'settings' => $storage_settings]);

    // Next we actually load the values.
    $values[$field_name] = [
      0 => [
        'record_id' => [
          'value' => $recordId_propertyValue,
          'type' => $recordId_propertyType,
          'definition' => $fieldconfig,
        ],
        'value' => [
          'value' => $value_propertyValue,
          'type' => $value_propertyType,
          'definition' => $fieldconfig,
        ],
        'NCBITaxon_common_name' => [
          'value' => $label_propertyValue,
          'type' => $label_propertyType,
          'definition' => $fieldconfig,
        ],
        'TAXRANK_0000005' => [
          'value' => $genus_propertyValue,
          'type' => $genus_propertyType,
          'definition' => $fieldconfig,
        ],
        'TAXRANK_0000006' => [
          'value' => $species_propertyValue,
          'type' => $species_propertyType,
          'definition' => $fieldconfig,
        ],
        'TAXRANK_0000047' => [
          'value' => $ifname_propertyValue,
          'type' => $ifname_propertyType,
          'definition' => $fieldconfig,
        ],
        'TAXRANK_0000046' => [
          'value' => $iftype_propertyValue,
          'type' => $iftype_propertyType,
          'definition' => $fieldconfig,
        ],
      ],
    ];
    $success = $chado_storage->loadValues($values);
    $this->assertTrue($success, "Loading values after adding $field_name was not success (i.e. did not return TRUE).");

    // Then we test that the values are now in the types that we passed in.
    // All fields should have been loaded, not just our organism one.
    $this->assertEquals('test_gene_name', $values['schema__name'][0]['value']['value']->getValue(), 'The gene name value was not loaded properly.');
    // Now test the organism values were loaded as expected.
    // Value: genus: Oryza, species: sativa, common_name: rice,
    //   abbreviation: O.sativa, infraspecific_name: Japonica,
    //   type: species_group (TAXRANK:0000010), comment: 'This is rice'
    $this->assertEquals($organism_id, $values['obi__organism'][0]['value']['value']->getValue(), 'The organism value was not loaded properly.');
    $this->assertEquals('Oryza', $values['obi__organism'][0]['TAXRANK_0000005']['value']->getValue(), 'The organism genus was not loaded properly.');
    $this->assertEquals('sativa', $values['obi__organism'][0]['TAXRANK_0000006']['value']->getValue(), 'The organism species was not loaded properly.');
    $this->assertEquals('Japonica', $values['obi__organism'][0]['TAXRANK_0000047']['value']->getValue(), 'The organism ifname was not loaded properly.');
    $this->assertEquals('species_group', $values['obi__organism'][0]['TAXRANK_0000046']['value']->getValue(), 'The organism iftype was not loaded properly.');
    $this->assertEquals("<i>Oryza sativa</i> species_group Japonica", $values['obi__organism'][0]['NCBITaxon_common_name']['value']->getValue(), 'The organism label was not loaded properly.');

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
