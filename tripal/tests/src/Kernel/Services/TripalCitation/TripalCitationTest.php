<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalCitation;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Focused on testing the citation generation methods.
 *
 * @group Tripal
 * @group Tripal Citation
 */
#[Group('service-citation')]
#[RunTestsInSeparateProcesses]
class TripalCitationTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['user', 'tripal'];

  /**
   * Tests the Tripal Citation service.
   */
  public function testGenerateCitation() {
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
    // An unknown publication type should return the default template.
    $format1 = $citation_service->getDefaultCitationTemplate('undefined_type');
    $expected1 = '[[Authors].][ [Title].][ [Journal Name|Journal Abbreviation|Series Name|Series Abbreviation].][ [Publication Date|Year];][ [Volume]][([Issue])][:[Pages]].';
    $this->assertEquals($expected1,
      $format1, 'Format 1 is not the expected default citation template');

    // Test a journal article.
    $format2 = $citation_service->getDefaultCitationTemplate('Journal Article');
    $c2 = $citation_service->generateCitation($format2, $pub1);
    $expected2 = 'Chado, A.B.; Drupal, B.C.; Tripal, C.D. Some impressive publication. Journal of Science. 2024; 123(4):10-20.';
    $this->assertEquals($expected2,
      $c2, 'Citation 2 is not the expected value');

    // Test a review, with missing issue value, and missing Journal Name,
    // use fallback Series Name.
    $pub3 = $pub1;
    unset($pub3['Issue']);
    unset($pub3['Journal Name']);
    $format3 = $citation_service->getDefaultCitationTemplate('Review');
    $c3 = $citation_service->generateCitation($format3, $pub3);
    $expected3 = 'Chado, A.B.; Drupal, B.C.; Tripal, C.D. Some impressive publication. The Journal of Science. 2024; 123:10-20.';
    $this->assertEquals($expected3,
      $c3, 'Citation 3 is not the expected value');
  }

}
