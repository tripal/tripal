<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the base functionality for chado-focused importers.
 *
 * Functionality for specific importers will be tested in their own test classes.
 */
class TripalImporterTest extends ChadoTestBrowserBase {

	/**
   * Tests focusing on the Tripal Importer plugin system and chado importers.
   *
   * @group tripal_importer
	 * @group chado_importer
   */
  public function testTripalImporterManager() {
		$this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
	}

  /**
   * Tests focusing on the Chado importer base class.
   *
   * @group tripal_importer
	 * @group chado_importer
   */
  public function testChadoImporterBase() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
	}
}
