<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalCitation;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
//use Drupal\tripal\Services\TripalCitationManager;


/**
 * Focused on testing the citation generation methods.
 *
 * @group Tripal
 * @group Tripal Citation
 */
class TripalCitationTest extends TripalTestKernelBase {


  /**
   * {@inheritdoc}
   */
  protected static $modules = ['user', 'tripal'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
  }

  /**
   * Tests the TripalEntityTypeCollection::generateCitation() method.
   */
  public function testTripalCitation_generateCitation() {
    $citation_service = \Drupal::service('tripal.citation');
    $c1 = $citation_service->generateCitation(1, []);
    $this->assertEquals('xxx', $c1, 'Test 1'); //@@@
  }

}
