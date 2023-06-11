<?php
namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

/**
 * Provides functions and member variables to be used
 * when testing Chado Storage. This allows for less
 * duplication of setup and more focus on the particular
 * use cases within the test classes themselves.
 */
trait ChadoStorageTestTrait {

  protected $content_entity_id;
  protected $content_type;
  protected $organism_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Create a new test schema for us to use.
    $connection = $this->createTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    /*
    // All Chado storage testing requires an entity.
    $content_entity = $this->createTripalContent();
    $content_entity_id = $content_entity->id();
    $content_type = $content_entity->getType();
    $content_type_obj = \Drupal\tripal\Entity\TripalEntityType::load($content_type);

    $this->content_entity_id = $content_entity_id;
    $this->content_type = $content_type;

    // And a term for properties.
    // This code ensures the vocab + ID Space are in the test drupal tables.
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vocabulary = $vmanager->createCollection('rdfs', 'chado_vocabulary');
    $idSpace = $idsmanager->createCollection('rdfs', 'chado_id_space');
    $idSpace->setDefaultVocabulary($vocabulary->getName());

    // Many need an organism page.
    $infra_type_id = $this->getCvtermID('TAXRANK', '0000010');
    $query = $connection->insert('1:organism');
    $query->fields([
      'genus' => 'Tripalus',
      'species' => 'databasica',
      'common_name' => 'Tripal',
      'abbreviation' => 'T. databasica',
      'infraspecific_name' => 'postgresql',
      'type_id' => $infra_type_id,
      'comment' => 'This is fake organism specifically for testing purposes.'
    ]);
    $this->organism_id = $query->execute();
    */
  }

}
