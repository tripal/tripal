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
class TripalEntityTypeCollectionInstallTest extends TripalTestKernelBase {


  /**
   * {@inheritdoc}
   */
  protected static $modules = ['user', 'tripal'];

  /**
   * A made-up set of details for some collection types to be used in testing
   * getTypeCollections. These will be written to storage in the setUp().
   */
  protected array $config_array = [
    'tripal.tripalentitytype_collection.monsters' => [
      'id' => 'monsters',
      'label' => 'Monsters',
      'description' => 'Types of monsters including those who live in the water, on land, and under beds.',
      'content_types' => [
        [
          'id' => 'selkies',
          'label' => 'Selkies',
          'term' => 'MONSTER:001',
          'category' => 'Monsters',
          'help_text' => 'Transformative creatures, changing from a seal to a human who often ensnare the heart of fishermen.',
          'title_format' => '[name]',
          'url_format' => 'selkie/[name]',
        ],
        [
          'id' => 'scottish_vampire',
          'label' => 'Baobhan Sith',
          'term' => 'MONSTER:002',
          'category' => 'Monsters',
          'help_text' => 'Beautiful young women with flowing green dresses and deer hooves instead of feet who dance with foolish hunters until they are exhausted before feasting on their blood.',
          'title_format' => '[name]',
          'url_format' => 'baobhan-sith/[name]',
        ],
      ],
    ],
    'tripal.tripalentitytype_collection.fairies' => [
      'id' => 'fairies',
      'label' => 'Fairies',
      'description' => 'Types of fairies including those from both the Seelie and Unseelie Courts.',
      'content_types' => [
        [
          'id' => 'banshees',
          'label' => 'Banshees',
          'term' => 'FAIRIES:001',
          'category' => 'Fairies',
          'help_text' => 'Small fairies, clad in white, with long flowing silver brushed hair who forewarn of death as their piercing cries of sorrow shake ye to the bone.',
          'title_format' => '[name]',
          'url_format' => 'banshees/[name]',
        ],
        [
          'id' => 'ghillie_dhu',
          'label' => 'Ghillie Dhu',
          'term' => 'FAIRIES:002',
          'category' => 'Fairies',
          'help_text' => 'Ancient, shy and solitary brown-harid lads, the Ghillie Dhu gaurd the ancient forests of Scotland.',
          'title_format' => '[name]',
          'url_format' => 'ghillie-dhu/[name]',
        ],
      ],
    ],
  ];


    /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Define some types to get when testing.
    $active_storage = \Drupal::service('config.storage');
    foreach ($this->config_array as $config_item => $config) {
      $active_storage->write($config_item, $config);
    }

    // Grab the container.
    $container = \Drupal::getContainer();

    // Create a mock ID space to return our mock term when asked.

    // Create a mock Tripal ID Space service to return our mock idspace when asked.
    $mock_idspace_service = $this->createMock(\Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager::class);
    $mock_idspace_service->method('loadCollection')
      ->willReturnCallback(function($id_space) {

        $mock_idspace = $this->createMock(\Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface::class);
        $mock_idspace->method('getTerm')
          ->willReturnCallback(function($accession) {

            $mock_term = $this->createMock(\Drupal\tripal\TripalVocabTerms\TripalTerm::class);
            $mock_term->method('getName')
              ->willReturn('Generic Fairy');
            $mock_term->method('getIdSpace')
              ->willReturn('FAIRIES');
            $mock_term->method('getAccession')
              ->willReturn($accession);
            $mock_term->method('getVocabulary')
              ->willReturn('FAIRIES');
            $mock_term->method('isValid')
              ->willReturn(TRUE);
            return $mock_term;
        });
      return $mock_idspace;
    });
    $container->set('tripal.collection_plugin_manager.idspace', $mock_idspace_service);
  }

  /**
   * Tests the TripalEntityTypeCollection::install() method.
   */
  public function testTripalEntityTypeCollection_install() {

    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');

    $content_type_service->install(['fairies']);

    // @todo assert that we created the content types we wanted to.
    // @todo assert that the monster content types were not created.
  }

  /**
   * Tests the TripalEntityTypeCollection::install() method.
   */
  public function testTripalEntityTypeCollection_installException() {

    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');

    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );

    // We expect an exception since this config does not exist.
    $this->expectException(\Exception::class);
    $content_type_service->install(['random']);

    // This is not giving the expected exception.

  }
}
