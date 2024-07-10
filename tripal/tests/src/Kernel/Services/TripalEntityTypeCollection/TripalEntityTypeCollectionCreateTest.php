<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalEntityTypeCollection;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;


/**
 * Focused on testing the create() and createContentType() methods.
 *
 * @group Tripal
 * @group Tripal Content
 * @group TripalEntityTypeCollection
 */
class TripalEntityTypeCollectionCreateTest extends TripalTestKernelBase {


  /**
   * {@inheritdoc}
   */
  protected static $modules = ['user', 'tripal'];

  /**
   * A dummy Tripal Term.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var array of \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected array $mock_terms;

  /**
   * A dummy Tripal ID Space.
   * NOTE: This is a dummy object so any methods called on it will return NULL.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase
   */
  protected object $mock_idspace;

    /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Grab the container.
    $container = \Drupal::getContainer();

    // We need a term for property types so we will create a generic mocked one
    // here which will be pulled from the container any time a term is requested.
    // -- valid organism term.
    $mock_term = $this->createMock(\Drupal\tripal\TripalVocabTerms\TripalTerm::class);
    $mock_term->method('getName')
      ->willReturn('organism');
    $mock_term->method('getIdSpace')
      ->willReturn('OBI');
    $mock_term->method('getAccession')
      ->willReturn('0100026');
    $mock_term->method('getVocabulary')
      ->willReturn('OBI');
    $mock_term->method('isValid')
      ->willReturn(TRUE);
    $this->mock_terms['organism'] = $mock_term;

    // Create a mock ID space to return our mock term when asked.
    $this->mock_idspace = $this->createMock(\Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface::class);
    $this->mock_idspace->method('getTerm')
      ->willReturnCallback(function($accession) {
        if (array_key_exists($accession, $this->mock_terms)) {
          return $this->mock_terms[$accession];
        }
        else {
          return NULL;
        }
      });
    // Create a mock Tripal ID Space service to return our mock idspace when asked.
    $mock_idspace_service = $this->createMock(\Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager::class);
    $mock_idspace_service->method('loadCollection')
      ->willReturnCallback(function($id_space) {
        if ($id_space == 'mock') {
          return $this->mock_idspace;
        }
        else {
          return NULL;
        }
      });
    $container->set('tripal.collection_plugin_manager.idspace', $mock_idspace_service);

    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['error'])
      ->getMock();
    $mock_logger->method('error')
      ->willReturnCallback(function($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $container->set('tripal.logger', $mock_logger);
  }

  /**
   * Tests the TripalEntityTypeCollection::create() method.
   */
  public function testTripalEntityTypeCollection_create() {

    $container = \Drupal::getContainer();
    $content_type = \Drupal\tripal\Services\TripalEntityTypeCollection::create($container);
    $this->assertIsObject($content_type,
      "We should be able to instanciate a TripalEntityTypeCollection using late static binding and dependency injection.");
    $this->assertInstanceOf(\Drupal\tripal\Services\TripalEntityTypeCollection::class, $content_type,
      "We should have created a TripalEntityTypeCollection object specifically via late static binding.");
  }

  /**
   * Tests the TripalEntityTypeCollection::createContentType() method.
   */
  public function testTripalEntityTypeCollection_createContentType() {
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');

    $good = [
      'label' => 'Organism',
      'term' => $this->mock_terms['organism'],
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1']
    ];

    // Test creating a good content type.
    $is_valid = $content_type_service->validate($good);
    $this->assertTrue($is_valid, "A good content type definition failed validation check.");
    $content_type = $content_type_service->createContentType($good);
    $this->assertTrue(!is_null($content_type), "Failed to create a content type with a valid definition.");

    // Now create the same one again and make sure it is skipped.
    $is_valid = $content_type_service->validate($good);
    $this->assertTrue($is_valid, "Even when creating a duplicate, the array should still be considered valid.");
    $content_type_dup = $content_type_service->createContentType($good);

    // Confirm the two are the same.
    $this->assertEquals($content_type->uuid(), $content_type_dup->uuid(),
      "When submitting the same content type a second time, we should just have the first one returned.");

  }
}
