<?php

namespace Drupal\Tests\tripal_chado\Functional\ChadoImporter;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the base functionality for chado-focused importers.
 *
 * Functionality for specific importers will be tested in their own test classes.
 */
#[Group('tripal-importer')]
#[Group('chado-importer')]
#[RunTestsInSeparateProcesses]
class ChadoImporterTest extends ChadoTestBrowserBase {

  /**
   * Tests focusing on the Tripal Importer plugin system and chado importers.
   */
  public function testTripalImporterManager() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

  /**
   * Tests focusing on the Chado importer base class.
   */
  public function testChadoImporterBase() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

}
