<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;


/**
 * Tests the basic functions of the TripalContentTypes Service..
 *
 * @group Tripal
 * @group Tripal Content
 */
class TripalContentTypesTest extends TripalTestBrowserBase {

  /**
   * Tests the TripalContentTypes class public functions.
   */
  public function testTripalContentTypes() {
    $logger = \Drupal::service('tripal.logger');

    // Create the vocabulary term needed for testing the content type.
    // We'll use the default Tripal plugins.
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

    // Createa good content type array.
    $good = [
      'label' => 'Organism',
      'term' => $term,
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'name' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1']
    ];

    /** @var \Drupal\tripal\Services\TripalContentTypes $content_type_setup **/
    $content_type_setup = \Drupal::service('tripal.content_types');
    $content_type_setup->setIdSpacePlugin('tripal_default_id_space');
    $content_type_setup->setVocabPlugin('tripal_default_vocabulary');

    // Test the public validate routine to make sure it fails when it should.
    $is_valid = $content_type_setup->validate($good, $logger);
    $this->assertTrue($is_valid, "A good content type definition failed validation check.");

    // Test that when a value is missing it fails validation.
    $bad = $good;
    unset($bad['term']);
    $is_valid = $content_type_setup->validate($bad, $logger);
    $this->assertFalse($is_valid, "A content type definition missing the 'term' should fail the validation check but it passed.");

    $bad = $good;
    unset($bad['name']);
    $is_valid = $content_type_setup->validate($bad, $logger);
    $this->assertFalse($is_valid, "A content type definition missing the 'name' should fail the validation check but it passed.");

    $bad = $good;
    unset($bad['label']);
    $is_valid = $content_type_setup->validate($bad, $logger);
    $this->assertFalse($is_valid, "A content type definition missing the 'label' should fail the validation check but it passed.");

    $bad = $good;
    unset($bad['category']);
    $is_valid = $content_type_setup->validate($bad, $logger);
    $this->assertFalse($is_valid, "A content type definition missing the 'category' should fail the validation check but it passed.");

    $bad = $good;
    unset($bad['help_text']);
    $is_valid = $content_type_setup->validate($bad, $logger);
    $this->assertFalse($is_valid, "A content type definition missing the 'help_text' should fail the validation check but it passed.");

    $bad = $good;
    $bad['synonyms'] = 'xyz';
    $is_valid = $content_type_setup->validate($bad, $logger);
    $this->assertFalse($is_valid, "A content type definition with a malformed synonyms list should fail the validation check but it passed.");


    $content_type = $content_type_setup->createContentType($good);
    $content_typ
  }
}