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

    // Create a new test schema for us to use.
    $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);

    // Get plugin managers we need for our testing.
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
    $fprop_id_0 = $this->addFeaturePropRecords($gene, $note_term, "Note 1", 0);
    $fprop_id_2 = $this->addFeaturePropRecords($gene, $note_term, "Note 2", 2);
    $fprop_id_1 = $this->addFeaturePropRecords($gene, $note_term, "Note 3", 1);

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
    $chado_table = 'feature';
    $chado_column = 'name';
    $storage_settings = [
      'storage_plugin_id' => 'chado_storage',
      'storage_plugin_settings' => [
        'base_table' => $chado_table,
      ],
    ];


    // Testing the Property Type + Value class creation
    // + prepping for future tests.
    $this->addFieldPropertyCVterms();
    $propertyTypes = [
      'feature_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'feature_id', 'SIO:000729', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $chado_table,
        'chado_column' => $chado_column,
      ]),
      'name' => new ChadoVarCharStoragePropertyType($content_type, $field_name, 'name', 'schema:name', 255, [
        'action' => 'store',
        'chado_table' => $chado_table,
        'chado_column' => $chado_column,
      ]),
    ];
    $propertyValues = [
      'feature_id' => new StoragePropertyValue(
        $content_type,
        $field_name,
        'feature_id',
        'SIO:000729',
        $content_entity_id,
        $feature_id
      ),
      'name' => new StoragePropertyValue(
        $content_type,
        $field_name,
        'name',
        'schema:name',
        $content_entity_id,
      ),
    ];
    $this->assertIsObject($propertyTypes['feature_id'], "Unable to create feature_id ChadoIntStoragePropertyType: $field_name, record_id");
    $this->assertIsObject($propertyValues['feature_id'], "Unable to create feature_id StoragePropertyValue: $field_name, record_id, $content_entity_id");
    $this->assertIsObject($propertyTypes['name'], "Unable to create feature.name ChadoIntStoragePropertyType: $field_name, value");
    $this->assertIsObject($propertyValues['name'], "Unable to create feature.name StoragePropertyValue: $field_name, value, $content_entity_id");

    // Make sure the values start empty.
    $this->assertEquals($feature_id, $propertyValues['feature_id']->getValue(), "The $field_name feature_id property should already be set.");
    $this->assertTrue(empty($propertyValues['name']->getValue()), "The $field_name feature.name property should not have a value.");

    // Now test ChadoStorage->addTypes()
    // param array $types = Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
    $chado_storage->addTypes($propertyTypes);
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
        'name'=> [
          'value' => $propertyValues['name'],
          'type' => $propertyTypes['name'],
          'definition' => $fieldconfig,
        ],
        'feature_id' => [
          'value' => $propertyValues['feature_id'],
          'type' => $propertyTypes['feature_id'],
          'definition' => $fieldconfig,
        ],
      ],
    ];
    $success = $chado_storage->loadValues($values);
    $this->assertTrue($success, "Loading values after adding $field_name was not success (i.e. did not return TRUE).");

    // Then we test that the values are now in the types that we passed in.
    //print_r($values['schema__name'][0]);
    $this->assertEquals('test_gene_name', $values['schema__name'][0]['name']['value']->getValue(), 'The gene name value was not loaded properly.');

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
    $chado_table = 'featureprop';
    $base_table = 'feature';
    $chado_column = 'organism_id';
    $storage_settings = [
      'storage_plugin_id' => 'chado_storage',
      'storage_plugin_settings' => [
        'base_table' => $chado_table,
      ],
    ];

    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $fk_term = $mapping->getColumnTermId('featureprop', 'feature_id');
    $type_id_term = $mapping->getColumnTermId('featureprop', 'type_id');
    $value_term = $mapping->getColumnTermId('featureprop', 'value');
    $rank_term = $mapping->getColumnTermId('featureprop', 'rank');

    $propertyTypes = [
      'feature_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'feature_id', 'SIO:000729', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => 'feature',
        'chado_column' => 'feature_id',
      ]),
      'featureprop_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'featureprop_id', 'SIO:000729', [
        'action' => 'store_pkey',
        'chado_table' => 'featureprop',
        'chado_column' => 'featureprop_id',
      ]),
      'fk_feature_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'fk_feature_id', $fk_term, [
        'action' => 'store_link',
        'chado_table' => 'featureprop',
        'chado_column' => 'feature_id',
      ]),
      'type_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'type_id', $type_id_term, [
        'action' => 'store',
        'chado_table' => 'featureprop',
        'chado_column' => 'type_id',
      ]),
      'value' => new ChadoIntStoragePropertyType($content_type, $field_name, 'value', $value_term, [
        'action' => 'store',
        'chado_table' => 'featureprop',
        'chado_column' => 'value',
      ]),
      'rank' => new ChadoIntStoragePropertyType($content_type, $field_name, 'rank', $rank_term, [
        'action' => 'store',
        'chado_table' => 'featureprop',
        'chado_column' => 'rank',
      ]),
    ];
    foreach ($propertyTypes as $key => $propType) {
      $this->assertIsObject($propType, "Unable to create the *StoragePropertyType: $field_name, $key");
    }

    // Testing the Property Value class creation.
    $propertyValues = [
      'feature_id' => new StoragePropertyValue($content_type, $field_name, 'feature_id', 'SIO:000729', $content_entity_id, $feature_id),
      'featureprop_id' => new StoragePropertyValue($content_type, $field_name, 'featureprop_id', 'SIO:000729', $content_entity_id),
      'fk_feature_id' => new StoragePropertyValue($content_type, $field_name, 'fk_feature_id', $fk_term, $content_entity_id),
      'type_id' => new StoragePropertyValue($content_type, $field_name, 'type_id', $type_id_term, $content_entity_id),
      'value' => new StoragePropertyValue($content_type, $field_name, 'value', $value_term, $content_entity_id),
      'rank' => new StoragePropertyValue($content_type, $field_name, 'rank', $rank_term, $content_entity_id),
    ];
    foreach ($propertyValues as $key => $propVal) {
      $this->assertIsObject($propVal, "Unable to create the StoragePropertyValue: $field_name, $key");
    }

    // Make sure the values start empty.
    $this->assertEquals($feature_id, $propertyValues['feature_id']->getValue(), "The $field_name feature_id property should be the feature_id.");
    $this->assertTrue(empty($propertyValues['featureprop_id']->getValue()), "The $field_name feature property pkey should not have a value.");
    $this->assertTrue(empty($propertyValues['fk_feature_id']->getValue()), "The $field_name feature property feature_id property should not have a value.");
    $this->assertTrue(empty($propertyValues['type_id']->getValue()), "The $field_name type_id property should not have a value.");
    $this->assertTrue(empty($propertyValues['value']->getValue()), "The $field_name value property should not have a value.");
    $this->assertTrue(empty($propertyValues['rank']->getValue()), "The $field_name rank property should not have a value.");

    // Now test ChadoStorage->addTypes()
    // param array $types = Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
    $chado_storage->addTypes($propertyTypes);
    $retrieved_types = $chado_storage->getTypes();
    $this->assertIsArray($retrieved_types, "Unable to retrieve the PropertyTypes after adding $field_name.");
    $this->assertCount(8, $retrieved_types, "Did not revieve the expected number of PropertyTypes after adding $field_name.");

    // We also need FieldConfig classes for loading values.
    // We're going to create a TripalField and see if that works.
    $fieldconfig = new FieldConfigMock(['field_name' => $field_name, 'entity_type' => $content_type]);
    $fieldconfig->setMock(['label' => $field_label, 'settings' => $storage_settings]);

    // Next we actually load the values.
    $values[$field_name] = [ 0 => [], 1 => [], 2 => [] ];
    foreach ($propertyTypes as $key => $propType) {
      $values[$field_name][0][$key] = [
        'type' => $propType,
        'value' => clone $propertyValues[$key],
        'definition' => $fieldconfig
      ];
      $values[$field_name][1][$key] = [
        'type' => $propType,
        'value' => clone $propertyValues[$key],
        'definition' => $fieldconfig
      ];
      $values[$field_name][2][$key] = [
        'type' => $propType,
        'value' => clone $propertyValues[$key],
        'definition' => $fieldconfig
      ];
    }
    // We also need to set the featureprop_id for each.
    $values[$field_name][0]['featureprop_id']['value']->setValue($fprop_id_0);
    $values[$field_name][1]['featureprop_id']['value']->setValue($fprop_id_1);
    $values[$field_name][2]['featureprop_id']['value']->setValue($fprop_id_2);
    // Now we can try to load the rest of the property.
    $success = $chado_storage->loadValues($values);
    $this->assertTrue($success, "Loading values after adding $field_name was not success (i.e. did not return TRUE).");

    // Then we test that the values are now in the types that we passed in.
    // All fields should have been loaded, not just our organism one.
    $this->assertEquals('test_gene_name', $values['schema__name'][0]['name']['value']->getValue(), 'The gene name value was not loaded properly.');
    // Now test the feature properties were loaded as expected.
    // Values:
    //   - type: note (local:note), value: "Note 1", rank: 0
    //   - type: note (local:note), value: "Note 2", rank: 2
    //   - type: note (local:note), value: "Note 3", rank: 1
    $this->assertEquals(
      "Note 1", $values['local__note'][0]['value']['value']->getValue(),
      'The delta 0 featureprop.value was not loaded properly.'
    );
    $this->assertEquals(
      "Note 3", $values['local__note'][1]['value']['value']->getValue(),
      'The delta 1 featureprop.value was not loaded properly.'
    );
    $this->assertEquals(
      "Note 2", $values['local__note'][2]['value']['value']->getValue(),
      'The delta 2 featureprop.value was not loaded properly.'
    );
    foreach([0,1,2] as $delta) {
      $this->assertEquals(
        $note_term->getInternalId(), $values['local__note'][$delta]['type_id']['value']->getValue(),
        "The type_id of the delta $delta note was not loaded properly."
      );
      $this->assertEquals(
        $feature_id, $values['local__note'][$delta]['fk_feature_id']['value']->getValue(),
        "The featureprop.feature_id of the delta $delta note was not loaded properly."
      );
      $this->assertEquals(
        $delta, $values['local__note'][$delta]['rank']['value']->getValue(),
        "The featureprop.rank of the delta $delta note was not loaded properly."
      );
    }


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
    $storage_settings = [
      'storage_plugin_id' => 'chado_storage',
      'storage_plugin_settings' => [
        'base_table' => $chado_table,
      ],
    ];

    // Testing the Property Type class creation.
    $base_table = $chado_table;
    $org_id_term = $mapping->getColumnTermId($base_table, 'organism_id');
    $genus_term = $mapping->getColumnTermId('organism', 'genus');
    $species_term = $mapping->getColumnTermId('organism', 'species');
    $iftype_term = $mapping->getColumnTermId('organism', 'type_id');
    $ifname_term = $mapping->getColumnTermId('organism', 'infraspecific_name');
    $propertyTypes = [
      'feature_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'feature_id', 'SIO:000729', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => 'feature_id'
      ]),
      'organism_id' => new ChadoIntStoragePropertyType($content_type, $field_name, 'organism_id', $org_id_term, [
        'action' => 'store',
        'chado_table' => $base_table,
        'chado_column' => 'organism_id',
      ]),
      'label' => new ChadoVarCharStoragePropertyType($content_type, $field_name, 'label', 'rdfs:label', 255, [
        'action' => 'replace',
        'template' => "<i>[genus] [species]</i> [infraspecific_type] [infraspecific_name]",
      ]),
      'genus' => new ChadoVarCharStoragePropertyType($content_type, $field_name, 'genus', $genus_term, 255, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id',
        'chado_column' => 'genus'
      ]),
      'species' => new ChadoVarCharStoragePropertyType($content_type, $field_name, 'species', $species_term, 255, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id',
        'chado_column' => 'species'
      ]),
      'infraspecific_name' => new ChadoVarCharStoragePropertyType($content_type, $field_name, 'infraspecific_name', $ifname_term, 255, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id',
        'chado_column' => 'infraspecific_name',
      ]),
      'infraspecific_type'=> new ChadoIntStoragePropertyType($content_type, $field_name, 'infraspecific_type', $iftype_term, [
        'action' => 'join',
        'path' => $base_table . '.organism_id>organism.organism_id;organism.type_id>cvterm.cvterm_id',
        'chado_column' => 'name',
        'as' => 'infraspecific_type_name'
      ])
    ];
    foreach ($propertyTypes as $key => $propType) {
      $this->assertIsObject($propType, "Unable to create the *StoragePropertyType: $field_name, $key");
    }

    // Testing the Property Value class creation.
    $propertyValues = [
      'feature_id' => new StoragePropertyValue($content_type, $field_name, 'feature_id', 'SIO:000729', $content_entity_id, $feature_id),
      'organism_id' => new StoragePropertyValue($content_type, $field_name, 'organism_id', $org_id_term, $content_entity_id),
      'label' => new StoragePropertyValue($content_type, $field_name, 'label', 'rdfs:label', $content_entity_id),
      'genus' => new StoragePropertyValue($content_type, $field_name, 'genus', $genus_term, $content_entity_id),
      'species' => new StoragePropertyValue($content_type, $field_name, 'species', $species_term, $content_entity_id),
      'infraspecific_name' => new StoragePropertyValue($content_type, $field_name, 'infraspecific_name', $ifname_term, $content_entity_id),
      'infraspecific_type'=> new StoragePropertyValue($content_type, $field_name, 'infraspecific_type', $iftype_term, $content_entity_id)
    ];
    foreach ($propertyValues as $key => $propVal) {
      $this->assertIsObject($propVal, "Unable to create the StoragePropertyValue: $field_name, $key");
    }

    // Make sure the values start empty.
    $this->assertEquals($feature_id, $propertyValues['feature_id']->getValue(), "The $field_name feature_id property should be the feature_id.");
    $this->assertTrue(empty($propertyValues['organism_id']->getValue()), "The $field_name value property should not have a value.");
    $this->assertTrue(empty($propertyValues['label']->getValue()), "The $field_name label property should not have a value.");
    $this->assertTrue(empty($propertyValues['genus']->getValue()), "The $field_name genus property should not have a value.");
    $this->assertTrue(empty($propertyValues['species']->getValue()), "The $field_name species property should not have a value.");
    $this->assertTrue(empty($propertyValues['infraspecific_name']->getValue()), "The $field_name infraspecific_name property should not have a value.");
    $this->assertTrue(empty($propertyValues['infraspecific_type']->getValue()), "The $field_name infraspecific_type property should not have a value.");

    // Now test ChadoStorage->addTypes()
    // param array $types = Array of \Drupal\tripal\TripalStorage\StoragePropertyTypeBase objects.
    $chado_storage->addTypes($propertyTypes);
    $retrieved_types = $chado_storage->getTypes();
    $this->assertIsArray($retrieved_types, "Unable to retrieve the PropertyTypes after adding $field_name.");
    $this->assertCount(15, $retrieved_types, "Did not revieve the expected number of PropertyTypes after adding $field_name.");

    // We also need FieldConfig classes for loading values.
    // We're going to create a TripalField and see if that works.
    $fieldconfig = new FieldConfigMock(['field_name' => $field_name, 'entity_type' => $content_type]);
    $fieldconfig->setMock(['label' => $field_label, 'settings' => $storage_settings]);

    // Next we actually load the values.
    $values[$field_name] = [ 0 => [] ];
    foreach ($propertyTypes as $key => $propType) {
      $values[$field_name][0][$key] = [
        'type' => $propType,
        'value' => $propertyValues[$key],
        'definition' => $fieldconfig
      ];
    }
    $success = $chado_storage->loadValues($values);
    $this->assertTrue($success, "Loading values after adding $field_name was not success (i.e. did not return TRUE).");

    // Then we test that the values are now in the types that we passed in.
    // All fields should have been loaded, not just our organism one.
    $this->assertEquals(
      'test_gene_name', $values['schema__name'][0]['name']['value']->getValue(),
      'The gene name value was not loaded properly.'
    );
    $this->assertEquals(
      "Note 1", $values['local__note'][0]['value']['value']->getValue(),
      'The delta 0 featureprop.value was not loaded properly.'
    );
    $this->assertEquals(
      "Note 3", $values['local__note'][1]['value']['value']->getValue(),
      'The delta 1 featureprop.value was not loaded properly.'
    );
    $this->assertEquals(
      "Note 2", $values['local__note'][2]['value']['value']->getValue(),
      'The delta 2 featureprop.value was not loaded properly.'
    );
    // Now test the organism values were loaded as expected.
    // Value: genus: Oryza, species: sativa, common_name: rice,
    //   abbreviation: O.sativa, infraspecific_name: Japonica,
    //   type: species_group (TAXRANK:0000010), comment: 'This is rice'
    $this->assertEquals($organism_id, $values['obi__organism'][0]['organism_id']['value']->getValue(), 'The organism value was not loaded properly.');
    $this->assertEquals('Oryza', $values['obi__organism'][0]['genus']['value']->getValue(), 'The organism genus was not loaded properly.');
    $this->assertEquals('sativa', $values['obi__organism'][0]['species']['value']->getValue(), 'The organism species was not loaded properly.');
    $this->assertEquals('Japonica', $values['obi__organism'][0]['infraspecific_name']['value']->getValue(), 'The organism infraspecific name was not loaded properly.');
    $this->assertEquals('species_group', $values['obi__organism'][0]['infraspecific_type']['value']->getValue(), 'The organism infraspecific type was not loaded properly.');
    $this->assertEquals("<i>Oryza sativa</i> species_group Japonica", $values['obi__organism'][0]['label']['value']->getValue(), 'The organism label was not loaded properly.');

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

    // Retrieve the test schema created in testChadoStorage().
    $chado = $this->getTestSchema();

    $chado->insert('1:organism')
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

    return $chado->select('1:organism', 'O')
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
   * A helper function for adding the CVterms needed for field properties.
   */
  protected function addFieldPropertyCVterms() {

    // Create the terms that are needed for this field.
    $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'genus',
        'definition' => '',
        'accession' =>'0000005',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'species',
        'definition' => '',
        'accession' =>'0000006',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'SIO',
      'id_space_name' => 'SIO',
      'term' => [
        'name' => 'record identifier',
        'definition' => 'A record identifier is an identifier for a database entry.',
        'accession' =>'000729',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'name' => 'name',
        'definition' => 'The name of the item.',
        'accession' =>'name',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'name' => 'additionalType',
        'definition' => 'An additional type for the item, typically used for adding more specific types from external vocabularies in microdata syntax. This is a relationship between something and a class that the thing is in.	',
        'accession' =>'additionalType',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'sequence',
      'id_space_name' => 'SO',
      'term' => [
        'name' => 'sequence_feature',
        'definition' => 'Any extent of continuous biological sequence. [LAMHDI:mb, SO:ke]',
        'accession' =>'0000110',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'NCIT',
      'id_space_name' => 'NCIT',
      'term' => [
        'name' => 'Value',
        'definition' => 'A numerical quantity measured or assigned or computed.',
        'accession' =>'C25712',
      ],
    ]);
    $this->createTripalTerm([
      'vocab_name' => 'OBCS',
      'id_space_name' => 'OBCS',
      'term' => [
        'name' => 'rank order',
        'definition' => 'A data item that represents an arrangement according to a rank, i.e., the position of a particular case relative to other cases on a defined scale.',
        'accession' =>'0000117',
      ],
    ]);


    return $gene;
  }

  /**
   * A helper function for adding a gene recrod to the feature table.
   */
  protected function addFeatureRecord($name, $uniquename, $type, $organism) {

    // Retrieve the test schema created in testChadoStorage().
    $chado = $this->getTestSchema();

    $chado->insert('1:feature')
      ->fields([
        'name' => $name,
        'uniquename' => $uniquename,
        'type_id' => $type->getInternalId(),
        'organism_id' => $organism->organism_id,
      ])
      ->execute();

    return $chado->select('1:feature', 'F')
      ->fields('F')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();
  }

  /**
   * A helper function for adding notes values to the featureprop table.
   */
  protected function addFeaturePropRecords($feature, $term, $value, $rank) {

    // Retrieve the test schema created in testChadoStorage().
    $chado = $this->getTestSchema();

    return $chado->insert('1:featureprop')
      ->fields([
        'feature_id' => $feature->feature_id,
        'type_id' => $term->getInternalId(),
        'value' => $value,
        'rank' => $rank,
      ])
      ->execute();
  }
}
