<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Url;

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

    $good = [
      'label' => 'Organism',
      'term' => 'OBI:0100026',
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'name' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1']
    ];

    /** @var \Drupal\tripal\Services\TripalContentTypes $content_type_setup **/
    $content_type_setup = \Drupal::service('tripal.content_types');

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


    //$content_type = $content_type_setup->createContentType($good, $logger);

  }
}