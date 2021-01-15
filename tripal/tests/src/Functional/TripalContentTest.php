<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\tripal\Entity\TripalVocab;
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
      'view controlled vocabulary entities'
    ]);

    // Anonymous User should not see this content type add page.
    $this->drupalGet('bio_data/add');
    $assert->pageTextContains('Access denied');

    // Perform a user login with the permissions specified above
    $this->drupalLogin($web_user);

    // First check that the link shows up to create new vocabulary
    // if the page contains no content types / bundles
    $this->drupalGet('bio_data/add');
    $assert->pageTextContains('There are currently no tripal content types');
    $assert->linkExists('creating a vocabulary');

    // Visit the link for creating a vocabulary and make sure it loads
    $this->clickLink('creating a vocabulary');
    $assert->pageTextContains('tripal vocabulary');

  }

}
