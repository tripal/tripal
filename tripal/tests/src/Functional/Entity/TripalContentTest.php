<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the basic functions of Tripal Content.
 *
 * @group Tripal
 * @group Tripal Content
 */
class TripalContentTest extends BrowserTestBase {
    protected $defaultTheme = 'stable';

    protected static $modules = ['tripal'];

  /**
   * Basic tests for Tripal Content Types.
   */
  public function testTripalEmptyContentTypes() {
    $assert = $this->assertSession();

    $web_user = $this->drupalCreateUser([
      'administer tripal',
      'manage tripal content types',
      'administer tripal content',
    ]);

    $urls = [
      'Tripal Content Listing' => 'admin/content/bio_data',
      'Tripal Content Type Listing' => 'admin/structure/bio_data',
      'Add Tripal Content Listing/Form' => 'bio_data/add',
    ];

    // Anonymous User should not be able to see any of these urls.
    foreach ($urls as $msg => $url) {

      $this->drupalGet($url);
      $assert->statusCodeEquals(403);
      $assert->pageTextContains('Access denied');
    }

    // Perform a user login with the permissions specified above
    $this->drupalLogin($web_user);

    // Then check that we can load each page with the correct permissions.
    foreach ($urls as $msg => $url) {
      $this->drupalGet($url);
      $assert->statusCodeEquals(200);
    }
  }

}
