<?php 

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBase;
use Drupal\tripal_chado\ChadoVocabTerms\ChadoVocabulary;
use Drupal\tripal_chado\ChadoVocabTerms\ChadoIdSpace;
use Drupal\tripal_chado\ChadoVocabTerms\ChadoTerm;

/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoVocabTerms
 */
class ChadoVocabTermsTest extends ChadoTestBase {
   
  /**
   * Tests task.
   * 
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testVocabulary() {
    // Create a temporary schema.
    $biodb = $this->getTestSchema(ChadoTestBase::INIT_DUMMY);
    
  }
}


