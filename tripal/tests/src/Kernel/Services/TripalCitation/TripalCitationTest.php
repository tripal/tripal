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
    $pub1 = [
      'Title' => 'Some impressive publication',
      'Journal Name' => 'Journal of Science',
      'Series Name' => 'The Journal of Science',
      'Year' => '2024',
      'Volume' => '123',
      'Issue' => '4',
      'Pages' => '10-20',
      'Authors' => 'Chado, A.B.; Drupal, B.C.; Tripal, C.D.',
    ];
    // An unknown publication type should return the default template
    $format1 = $citation_service->getCitationTemplate('undefined_type');
    $this->assertEquals('{{Authors}.}{ {Title}.}{ {Publication Date|Year}.}{ {Journal Name|Journal Abbreviation|Series Name|Series Abbreviation}}{ {Volume}}{({Issue})}{:{Pages}.}',
      $format1, 'Format 1 is not the default citation template');

    // Test a journal article
    $format2 = $citation_service->getCitationTemplate('Journal Article');
    $c2 = $citation_service->generateCitation($pub1, $format2);
    $this->assertEquals('Chado, A.B.; Drupal, B.C.; Tripal, C.D. Some impressive publication. 2024. Journal of Science 123(4):10-20.',
      $c2, 'Citation 2 is not the expected value');

    // Test a review, with missing issue value, and missing Journal Name, use fallback Series Name
    $pub3 = $pub1;
    unset($pub3['Issue']);
    unset($pub3['Journal Name']);
    $format3 = $citation_service->getCitationTemplate('Review');
    $c3 = $citation_service->generateCitation($pub3, $format3);
    $this->assertEquals('Chado, A.B.; Drupal, B.C.; Tripal, C.D. Some impressive publication. The Journal of Science 2024. 123:10-20.',
      $c3, 'Citation 3 is not the expected value');
  }

}
