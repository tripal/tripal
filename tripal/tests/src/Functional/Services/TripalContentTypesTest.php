<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;


/**
 * Tests the basic functions of the TripalContentTypes Service.
 *
 * @group Tripal
 * @group Tripal Content
 * @group Tripal Content Types
 */
class TripalContentTypesTest extends TripalTestBrowserBase {

  /**
   * Tests the TripalContentTypes class public functions.
   */
  public function testTripalContentTypes() {

    // Create the vocabulary term needed for testing the content type.
    // We'll use the default Tripal IdSpace and Vocabulary plugins.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $idspace = $idsmanager->createCollection('OBI', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('OBI', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'organism',
      'idSpace' => 'OBI',
      'vocabulary' => 'OBI',
      'accession' => '0100026',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);

    // Create a good content type array.
    $good = [
      'label' => 'Organism',
      'term' => $term,
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1']
    ];

    /** @var \Drupal\tripal\Services\TripalContentTypes $content_type_service **/
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');

    // Test creating a good content type.
    $is_valid = $content_type_service->validate($good);
    $this->assertTrue($is_valid, "A good content type definition failed validation check.");
    $content_type = $content_type_service->createContentType($good);
    $this->assertTrue(!is_null($content_type), "Failed to create a content type with a valid definition.");


    // Test that when a value is missing it fails validation.
    $bad = $good;
    unset($bad['term']);
    $is_valid = $content_type_service->validate($bad);
    $this->assertFalse($is_valid, "A content type definition missing the 'term' should fail the validation check but it passed.");
    $content_type = $content_type_service->createContentType($bad);
    $this->assertTrue(is_null($content_type), "Created a content type when the term is incorrect.");


    $bad = $good;
    unset($bad['id']);
    $is_valid = $content_type_service->validate($bad);
    $this->assertFalse($is_valid, "A content type definition missing the 'id' should fail the validation check but it passed.");
    $content_type = $content_type_service->createContentType($bad);
    $this->assertTrue(is_null($content_type), "Created a content type when the id is incorrect.");


    $bad = $good;
    unset($bad['label']);
    $is_valid = $content_type_service->validate($bad);
    $this->assertFalse($is_valid, "A content type definition missing the 'label' should fail the validation check but it passed.");
    $content_type = $content_type_service->createContentType($bad);
    $this->assertTrue(is_null($content_type), "Created a content type when the label is incorrect.");


    $bad = $good;
    unset($bad['category']);
    $is_valid = $content_type_service->validate($bad);
    $this->assertFalse($is_valid, "A content type definition missing the 'category' should fail the validation check but it passed.");
    $content_type = $content_type_service->createContentType($bad);
    $this->assertTrue(is_null($content_type), "Created a content type when the category is incorrect.");


    $bad = $good;
    unset($bad['help_text']);
    $is_valid = $content_type_service->validate($bad);
    $this->assertFalse($is_valid, "A content type definition missing the 'help_text' should fail the validation check but it passed.");
    $content_type = $content_type_service->createContentType($bad);
    $this->assertTrue(is_null($content_type), "Created a content type when the help_text is incorrect.");


    $bad = $good;
    $bad['synonyms'] = 'xyz';
    $is_valid = $content_type_service->validate($bad);
    $this->assertFalse($is_valid, "A content type definition with a malformed synonyms list should fail the validation check but it passed.");
    $content_type = $content_type_service->createContentType($bad);
    $this->assertTrue(is_null($content_type), "Created a content type when the synonyms are incorrect.");

  }
}
