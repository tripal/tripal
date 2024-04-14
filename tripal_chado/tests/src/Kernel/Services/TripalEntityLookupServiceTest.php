<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\Services\TripalJob;

/**
 * Tests the entity lookup service for chado-based content types.
 *
 * @group TripalEntityLookup
 */
class TripalEntityLookupServiceTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'tripal', 'tripal_chado', 'views', 'field'];

  protected $connection;

  // The terms for the content types we will be testing here
  protected $project_termIdSpace = 'NCIT';
  protected $project_Accession = 'C47885';
  protected $analysis_termIdSpace = 'operation';
  protected $analysis_Accession = '2945';
  // Tests 2 links, 1 link, and 0 links.
  protected $testlinks = [[1,1], [1,2], [2,2]];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    $this->installConfig('system');
    // ... we need entity types to publish them.
    $this->installEntitySchema('tripal_entity_type');
    $this->installEntitySchema('tripal_entity');
    // ... we need the config for tripal_chado since it defines the content types we will install.
    $this->installConfig('tripal_chado');
    // ... we need the tripal term tables
    $this->installSchema('tripal', ['tripal_id_space_collection', 'tripal_terms_idspaces', 'tripal_vocabulary_collection', 'tripal_terms_vocabs', 'tripal_terms']);

    // Get Chado in place
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Create three projects in chado.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:project')
        ->fields([
          'name' => 'Project No. ' . $i,
        ])->execute();
    }

    // Create three analyses in chado.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:analysis')
        ->fields([
          'name' => 'Analysis No. ' . $i,
          'program' => 'PHP',
          'programversion' => 'Version ' . $i,
        ])->execute();
    }

    // Create several links between these content types.
    foreach ($this->testlinks as $testlink) {
      $this->connection->insert('1:project_analysis')
        ->fields([
          'project_id' => $testlink[0],
          'analysis_id' => $testlink[1],
        ])->execute();
    }

    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach(['local', 'SIO', 'schema', 'data', 'NCIT', 'operation', 'OBCS', 'SWO', 'IAO', 'TPUB', 'SBO', 'ERO'] as $termIdSpace) {
      $idsmanager->createCollection($termIdSpace, "chado_id_space");
    }
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    foreach(['local', 'SIO', 'schema', 'EDAM', 'ncit', 'OBCS', 'swo', 'IAO', 'tripal_pub', 'sbo', 'ero'] as $termVocab) {
      $vmanager->createCollection($termVocab, "chado_vocabulary");
    }

    // Create the content types + fields that we need.
    $this->createContentTypeFromConfig('general_chado', 'project', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'analysis', TRUE);
  }

  /**
   * We publish some Chado records that have entries in linker tables,
   * and verify that the entity record is populated with the expected entity_id.
   *
   */
  public function testTripalEntityLookupService() {
    $drupal = \Drupal::service('database');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
    $current_user = \Drupal::currentUser();
    $values = ["schema_name" => $this->testSchemaName];
    $datastore = 'chado_storage';

    // Publish the test content entities and confirm that they have been created.
    // Submit the Tripal jobs by calling the callback directly.
    $bundle = 'project';
    tripal_publish($bundle, $datastore, $values);
    $project_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => 'project']);
    $this->assertCount(3, $project_entities,
      "We expected there to be the same number of project entities as we inserted.");

    $bundle = 'analysis';
    tripal_publish($bundle, $datastore, $values);
    $analysis_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => 'analysis']);
    $this->assertCount(3, $analysis_entities,
      "We expected there to be the same number of analysis entities as we inserted.");

    // Tests the entity lookup service directly.
    // Because this is a test environment, we know that the entity IDs
    // that we just published will start with 1.
    for ($project_id=1; $project_id <= 3; $project_id++) {
      $expected_entity_id = $project_id + 0;
      $entity_id = $lookup_manager->getEntityId(
        $project_id,
        $this->project_termIdSpace,
        $this->project_Accession,
      );
      $this->assertEquals($expected_entity_id, $entity_id, "We did not retrieve the expected entity_id for project $project_id");
    }
    for ($analysis_id=1; $analysis_id <= 3; $analysis_id++) {
      $expected_entity_id = $analysis_id + 3;
      $entity_id = $lookup_manager->getEntityId(
        $analysis_id,
        $this->analysis_termIdSpace,
        $this->analysis_Accession,
      );
      $this->assertEquals($expected_entity_id, $entity_id, "We did not retrieve the expected entity_id for analysis $analysis_id");
    }

    // An invalid record should return NULL. Mimics a record that has not been published.
    $analysis_id = 987654321;
    $entity_id = $lookup_manager->getEntityId(
      $analysis_id,
      $this->analysis_termIdSpace,
      $this->analysis_Accession,
    );
    $this->assertNull($entity_id, 'We retrieved an entity_id for a nonexistant record');

    // A valid record but invalid term should return NULL, mimics if this is not a content type.
    $analysis_id = 1;
    $entity_id = $lookup_manager->getEntityId(
      $analysis_id,
      $this->analysis_termIdSpace,
      'not-a-real-accession',
    );
    $this->assertNull($entity_id, 'We retrieved an entity_id for an invalid CV accession');

    // Test the getRenderableItem() function without an entity_id.
    $displayed_string = '<i>test string</i>';
    $renderable_item = $lookup_manager->getRenderableItem($displayed_string, NULL);
    $this->assertIsArray($renderable_item, 'getRenderableItem should always return an array');
    $this->assertArrayHasKey('#markup', $renderable_item, 'getRenderableItem should return plain markup if no entity_id');
    $this->assertEquals($displayed_string, $renderable_item['#markup'], 'getRenderableItem markup is not the value we supplied');
    $this->assertArrayNotHasKey('#url', $renderable_item, 'getRenderableItem should not return a url if no entity_id');

    // Test the getRenderableItem() function with a valid entity_id.
    $renderable_item = $lookup_manager->getRenderableItem($displayed_string, 1);
    $this->assertIsArray($renderable_item, 'getRenderableItem should always return an array');
    $this->assertArrayHasKey('#url', $renderable_item, 'getRenderableItem should return a url if entity_id provided');
    $this->assertIsObject($renderable_item['#url'], 'getRenderableItem should return an object for the url');
    $this->assertArrayNotHasKey('#markup', $renderable_item, 'getRenderableItem should not return plain markup if entity_id provided');
    $this->assertEquals($displayed_string, $renderable_item['#title'], 'getRenderableItem title is not the value we supplied');
    
  }
}
