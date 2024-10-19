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
      'Year' => '2024',
      'Volume' => '123',
      'Issue' => '4',
      'Pages' => '10-20',
      'Authors' => 'Chado, A.B.; Drupal, B.C.; Tripal, C.D.',
    ];
    $format1 = '{{Authors}.}{ {Title}.}{ {Year}.}{ {Journal}}{ {Volume}}{({Issue})}{: {Pages}.}{^{bogus}$}';
    $c1 = $citation_service->generateCitation($pub1, $format1);
    $this->assertEquals('Chado, A.B.; Drupal, B.C.; Tripal, C.D. Some impressive publication. 2024. 123(4): 10-20.', $c1, 'Citation 1 is not as expected'); //@@@

    $pub2 = $pub1;
    unset($pub2['Issue']);
    $c2 = $citation_service->generateCitation($pub2, $format1);
    $this->assertEquals('Chado, A.B.; Drupal, B.C.; Tripal, C.D. Some impressive publication. 2024. 123: 10-20.', $c2, 'Citation 2 is not as expected'); //@@@
  }

}
