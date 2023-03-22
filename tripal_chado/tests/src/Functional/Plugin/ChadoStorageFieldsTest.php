<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

/**
 * Tests for the ChadoStorage class which are focused on field-specific test cases.
 * Specifically, each test method will focus on the properties used for
 * a specific type of field.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal ChadoStorage
 */
class ChadoStorageFieldsTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'field_ui'];

  protected $content_entity_id;
  protected $content_type;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Create a new test schema for us to use.
    $connection = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // All Chado storage testing requires an entity.
    $content_entity = $this->createTripalContent();
    $content_entity_id = $content_entity->id();
    $content_type = $content_entity->getType();
    $content_type_obj = \Drupal\tripal\Entity\TripalEntityType::load($content_type);

    $this->content_entity_id = $content_entity_id;
    $this->content_type = $content_type;
  }

  /**
   * Tests double join situations such as with linker tables.
   * The specific example used will be feature > analysisfeature > analysis.
   *
   * @group current-focus
   */
  public function testLinkerTableDoubleHop() {

    // Setup
    // --------------------------------

    $connection = $this->getTestSchema();

    // Get plugin managers we need for our testing.
    $storage_manager = \Drupal::service('tripal.storage');
    $chado_storage = $storage_manager->createInstance('chado_storage');

    // This is just a fake field name for our test.
    // The field does not actually need to exist for this test.
    $field_name = 'testlinkerfield';

    // The term does not need to be unique for this test
    // as such we will use the same term for all properties for ease
    // and performance of this test.
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vocabulary = $vmanager->createCollection('rdfs', 'chado_vocabulary');
    $idSpace = $idsmanager->createCollection('rdfs', 'chado_id_space');
    $idSpace->setDefaultVocabulary($vocabulary->getName());
    $test_term_string = 'rdfs:type';

    // We also need FieldConfig classes for loading values.
    // We're going to use Drupal's FieldConfigMock to simulate it for us.
    // by using setMock we can set the specific settings we will need to use for our tests.
    $fieldconfig = new FieldConfigMock([
      'field_name' => $field_name,
      'entity_type' => $this->content_type
    ]);
    $fieldconfig->setMock([
      'label' => $field_name,
      'settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings' => [
          'base_table' => 'feature',
        ],
      ],
    ]);

    // We will want a feature record to use in testing.
    // Add the organism record.
    $infra_type_id = $this->getCvtermID('TAXRANK', '0000010');
    $query = $connection->insert('1:organism');
    $query->fields([
      'genus' => 'Oryza',
      'species' => 'sativa',
      'common_name' => 'rice',
      'abbreviation' => 'O.sativa',
      'infraspecific_name' => 'Japonica',
      'type_id' => $infra_type_id,
      'comment' => 'This is rice'
    ]);
    $organism_id = $query->execute();

    // Add the gene record.
    $gene_type_id = $this->getCvtermID('SO', '0000704');
    $query = $connection->insert('1:feature');
    $query->fields([
      'name' => 'test_gene_name',
      'uniquename' => 'test_gene_uname',
      'type_id' => $gene_type_id,
      'organism_id' => $organism_id,
    ]);
    $feature_id = $query->execute();

    // Creating the Types + Values.
    // --------------------------------
    // Here we define all the properties that you would normally define in the
    // ChadoFieldItemBase::tripalTypes() method.
    // This example is specific to a field whose base_table was set to feature
    // and that wants to save an analysis (program + programversion) associated
    // with the feature for a given Tripal content page.
    $propertyTypes = [
      // Keeps track of the feature record our hypothetical field cares about.
      'base_id' => new ChadoIntStoragePropertyType($this->content_type, $field_name, 'base_id', $test_term_string, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => 'feature',
        'chado_column' => 'feature_id'
      ]),
      // Generate `JOIN {analysisfeature} ON feature.feature_id = analysisfeature.feature_id`
      // Will also store the feature.feature_id so no need for drupal_store => TRUE.
      'first_hop' => new ChadoIntStoragePropertyType($this->content_type, $field_name, 'first_hop', $test_term_string, [
        'action' => 'store_link',
        'left_table' => 'feature',
        'left_table_id' => 'feature_id',
        'right_table' => 'analysisfeature',
        'right_table_id' => 'feature_id'
      ]),
      // Store the primary key for our linker table.
      'linker_id' => new ChadoIntStoragePropertyType($this->content_type, $field_name, 'linker_id',  $test_term_string, [
        'action' => 'store_pkey',
        'chado_table' => 'analysisfeature',
        'chado_column' => 'analysisfeature_id',
      ]),
      // Generate `JOIN {analysis} ON analysisfeature.analysis_id = analysis.analysis_id`
      // Will also store the analysisfeature.analysis_id so no need for drupal_store => TRUE.
      'second_hop' => new ChadoIntStoragePropertyType($this->content_type, $field_name, 'second_hop', $test_term_string, [
        'action' => 'store_link',
        'left_table' => 'analysisfeature',
        'left_table_id' => 'analysis_id',
        'right_table' => 'analysis',
        'right_table_id' => 'analysis_id'
      ]),
      // Keeps track of the analysis_id and also indicates that the analysis table is a base table
      // and should be handled before records in analysisfeature.
      'analysis_id' => new ChadoIntStoragePropertyType($this->content_type, $field_name, 'analysis_id', $test_term_string, [
        'action' => 'store_id',
        'chado_table' => 'analysis',
        'chado_column' => 'analysis_id'
      ]),
      // Now we are going to store all the core columns of the analysis table to
      // ensure we can meet the unique and not null requirements of the table.
      'program' => new ChadoVarCharStoragePropertyType($this->content_type, $field_name, 'program', $test_term_string, 255, [
        'action' => 'store',
        'chado_table' => 'analysis',
        'chado_column' => 'program'
      ]),
      'programversion' => new ChadoVarCharStoragePropertyType($this->content_type, $field_name, 'programversion', $test_term_string, 255, [
        'action' => 'store',
        'chado_table' => 'analysis',
        'chado_column' => 'programversion'
      ]),
    ];

    // Now confirm they were created.
    foreach ($propertyTypes as $name => $type) {
      $this->assertIsObject(
        $type,
        "Unable to create $name property type: not an object."
      );
      $this->assertInstanceOf(
        StoragePropertyTypeBase::class,
        $type,
        "Unable to create $name property type: does not inherit from StoragePropertyTypeBase."
      );
    }

    // Now add them to Chado storage.
    $chado_storage->addTypes($propertyTypes);
    $retrieved_types = $chado_storage->getTypes();
    $this->assertIsArray(
      $retrieved_types,
      "Unable to retrieve the PropertyTypes after adding $field_name."
    );
    $this->assertCount(
      7,
      $retrieved_types,
      "Did not revieve the expected number of PropertyTypes after adding $field_name."
    );

    // Now we ned to create the value objects.
    $propertyValues = [];
    foreach ($propertyTypes as $name => $type) {
      $propertyValues[$name] = new StoragePropertyValue(
        $this->content_type,
        $field_name,
        $type->getKey(),
        $test_term_string,
        $this->content_entity_id,
      );

      // Test that we were able to create them properly.
      $this->assertIsObject(
        $propertyValues[$name],
        "Unable to create $name property type: not an object."
      );
      $this->assertInstanceOf(
        StoragePropertyValue::class,
        $propertyValues[$name],
        "Unable to create $name property type: does not inherit from StoragePropertyValue."
      );
      $this->assertTrue(
        empty($propertyValues[$name]->getValue()),
        "The $field_name $name property should not have a value."
      );
    }

    // Test that we can
    // create the linkage using this field
    // -------------------------------------
    // Set the values in our propertyValue classes.
    // NOTE: The feature was added in the prep for this test
    // but there are no analysis records.
    $propertyValues['base_id']->setValue($feature_id);
    $propertyValues['program']->setValue('NCBI Blast');
    $propertyValues['programversion']->setValue('v2.13.0');
    // Setup the right structure for insertValues (see TripalStorageInterface::insertValues)
    $values = [$field_name => [0 => []]];
    foreach ($propertyValues as $keyname => $propval) {
      $values[$field_name][0][$keyname] = [
        'definition' => $fieldconfig,
        'type' => $propertyTypes[$keyname],
        'value' => $propval,
      ];
    }
    $success = $chado_storage->insertValues($values);

    $this->assertTrue($success, 'We were not able to insert the data.');
  }
}
