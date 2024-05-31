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
  protected $contact_termIdSpace = 'NCIT';
  protected $contact_Accession = 'C47954';
  protected $array_design_termIdSpace = 'EFO';
  protected $array_design_Accession = '0000269';
  protected $manufacturer_termIdSpace = 'EFO';
  protected $manufacturer_Accession = '0001728';

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

    // Create one contact in chado.
    $this->connection->insert('1:contact')
      ->fields([
        'name' => 'Contact No. 1',
      ])->execute();

    // Create one arraydesign in chado, to test the mismatched
    // foreign key names manufacturer_id -> contact_id
    $this->connection->insert('1:arraydesign')
      ->fields([
        'name' => 'ArrayDesign No. 1',
        'platformtype_id' => 1,  // not used, whatever the very first cvterm is
        'manufacturer_id' => 2,  // 1 is the null contact, defined by chado
      ])->execute();

    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach(['local', 'SIO', 'schema', 'data', 'NCIT', 'operation', 'OBCS', 'SWO', 'IAO', 'TPUB', 'SBO', 'sep', 'ERO', 'EFO'] as $termIdSpace) {
      $idsmanager->createCollection($termIdSpace, "chado_id_space");
    }
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    foreach(['local', 'SIO', 'schema', 'EDAM', 'ncit', 'OBCS', 'swo', 'IAO', 'tripal_pub', 'sbo', 'sep', 'ero', 'efo'] as $termVocab) {
      $vmanager->createCollection($termVocab, "chado_vocabulary");
    }

    // Create the content types + fields that we need.
    $this->createContentTypeFromConfig('general_chado', 'project', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'analysis', TRUE);
    $this->createContentTypeFromConfig('general_chado', 'contact', TRUE);
    $this->createContentTypeFromConfig('expression_chado', 'array_design', TRUE);
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
    $publish_options = ["schema_name" => $this->testSchemaName];
    $datastore = 'chado_storage';

    // Publish the test content entities and confirm that they have been created.
    // Submit the Tripal jobs by calling the callback directly.
    $bundle = 'project';
    tripal_publish($bundle, $datastore, $publish_options);
    $project_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => $bundle]);

    $this->assertCount(3, $project_entities,
      "We expected there to be the same number of project entities as we inserted.");

    $bundle = 'analysis';
    tripal_publish($bundle, $datastore, $publish_options);
    $analysis_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => $bundle]);
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
        NULL
      );
      $this->assertEquals($expected_entity_id, $entity_id, "We did not retrieve the expected entity_id for project $project_id");
    }
    for ($analysis_id=1; $analysis_id <= 3; $analysis_id++) {
      $expected_entity_id = $analysis_id + 3;
      $entity_id = $lookup_manager->getEntityId(
        $analysis_id,
        $this->analysis_termIdSpace,
        $this->analysis_Accession,
        NULL
      );
      $this->assertEquals($expected_entity_id, $entity_id, "We did not retrieve the expected entity_id for analysis $analysis_id");
    }

    // An invalid record should return NULL. Mimics a record that has not been published.
    $analysis_id = 987654321;
    $entity_id = $lookup_manager->getEntityId(
      $analysis_id,
      $this->analysis_termIdSpace,
      $this->analysis_Accession,
      NULL
    );
    $this->assertNull($entity_id, 'We retrieved an entity_id for a nonexistent record');

    // A valid record but invalid term should return NULL, mimics if this is not a content type.
    $analysis_id = 1;
    $entity_id = $lookup_manager->getEntityId(
      $analysis_id,
      $this->analysis_termIdSpace,
      'not-a-real-accession',
      NULL
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

    // Query a Drupal entity table to confirm that it stores the Chado record ID, and that it is correct.
    $entity_table_name = 'tripal_entity__analysis_name';
    $entity_column_name = 'analysis_name_record_id';
    for ($entity_id=4; $entity_id <= 6; $entity_id++) {
      $chado_analysis_id = $entity_id - 3;
      $query = \Drupal::entityQuery('tripal_entity')
        ->condition('type', 'analysis')
        ->condition('analysis_name.record_id', $chado_analysis_id, '=')
        ->accessCheck(TRUE);
      $ids = $query->execute();
      $this->assertEquals(1, count($ids), "Expected exactly one match from $entity_table_name query");
    }

    // Sometimes foreign key names are different than the object table
    // primary key, e.g. arraydesign table column manufacturer_id is
    // a foreign key to the contact table column contact_id.

    // Publish the (null and) test contact and arraydesign entities and confirm that they have been created.
    $bundle = 'contact';
    tripal_publish($bundle, $datastore, $publish_options);
    $contact_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => $bundle]);
    // Expect 2 here instead of 1 because the null contact will also be published - Issue #1809
    $this->assertCount(2, $contact_entities,
      "We expected there to be the same number of $bundle entities as we inserted plus one.");

    $bundle = 'array_design';  // not the same as the table name
    tripal_publish($bundle, $datastore, $publish_options);
    $arraydesign_entities = \Drupal::entityTypeManager()->getStorage('tripal_entity')->loadByProperties(['type' => $bundle]);
    $this->assertCount(1, $arraydesign_entities,
      "We expected there to be the same number of $bundle entities as we inserted.");

    // The entity lookup from arraydesign manufacturer_id should
    // retrieve the entity for the contact_id we published, internally
    // this uses the fallback entity lookup function getDefaultBundle(),
    // because the term for manufacturer_id is NOT a content type.
    // For the fallback lookup we need to also pass the base table.
    $base_table = 'contact';
    $chado_contact_id = 2;  // 1 is the null contact
    $expected_contact_entity_id = 8; // 1-3: project, 4-6: analysis, 7: null contact, 8: test contact, 9: array_design
    $entity_id = $lookup_manager->getEntityId(
      $chado_contact_id,
      $this->manufacturer_termIdSpace,
      $this->manufacturer_Accession,
      $base_table
    );
    $this->assertEquals($expected_contact_entity_id, $entity_id, "We did not retrieve the expected entity_id for manufacturer_id $chado_contact_id");

    // Also check that the contact_id (as manufacturer_id) is in the Drupal table
    $entity_table_name = 'tripal_entity__array_design_manufacturer';
    $entity_column_name = 'array_design_manufacturer_record_id';
    $query = \Drupal::entityQuery('tripal_entity')
      ->condition('type', 'array_design')
      ->condition('array_design_manufacturer.manufacturer_id', $chado_contact_id, '=')
      ->accessCheck(TRUE);
    $ids = $query->execute();
    $this->assertEquals(1, count($ids), "Expected exactly one match from array_design manufacturer query");
  }
}
