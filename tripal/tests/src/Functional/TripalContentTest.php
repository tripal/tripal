<?php

namespace Drupal\Tests\tripal\Functional;

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

    protected static $modules = ['tripal', 'block', 'field_ui'];

  /**
   * Basic tests for Tripal Content Types.
   *
   * @group tripal_content
   */
  public function testTripalEmptyContentTypes() {
    $assert = $this->assertSession();

    $web_user = $this->drupalCreateUser([
      'administer tripal',
    ]);

    // Anonymous User should not see this content type add page.
    $this->drupalGet('bio_data/add');
    $assert->pageTextContains('Access denied');

    // Perform a user login with the permissions specified above
    $this->drupalLogin($web_user);

    // First check that the link shows up to create new content type.
    // if the page contains no content types / bundles
    $this->drupalGet('bio_data/add');
    $assert->elementExists('css', 'td.empty.message');

  }

}
