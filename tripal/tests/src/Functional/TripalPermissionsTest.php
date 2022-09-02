<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\user\Entity\Role;
use Drupal\Core\Url;

/**
 * Tests the basic functions of the TripalTerm Entity Type.
 *
 * @group Tripal
 * @group Tripal Term
 * @group Tripal Entities
 */
class TripalPermissionsTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;
  protected $defaultTheme = 'stable';

  protected static $modules = ['tripal', 'block', 'field_ui'];

  /**
   * Test all the base Tripal admin paths.
   *
   * @group Tripal Permissions
   */
  public function testTripalAdminPages() {
    $assert = $this->assertSession();

    // The URLs to check with the key being the label expected in the
    // Tripal admin menu listing.
    $urls = [
      'Tripal' => 'admin/tripal',
      'Registration' => 'admin/tripal/register',
      'Jobs' => 'admin/tripal/tripal_jobs',
      'Data Loaders' => 'admin/tripal/loaders',
      'Data Storage' => 'admin/tripal/storage',
      'Extensions' => 'admin/tripal/extension',
      'User File Management' => 'admin/tripal/files',
    ];

		$userAuthenticatedOnly = $this->drupalCreateUser();
		$userTripalAdmin = $this->drupalCreateUser(['administer tripal']);

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $assert->statusCodeEquals(403, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unpriviledged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission('administer tripal'), "The unpriviledged user should not have the 'administer tripal' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $assert->statusCodeEquals(403, "The unpriviledged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, priviledged user.
    // This checks priviledged users can access these pages.
    $this->drupalLogin($userTripalAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalAdmin), "The priviledged user should be logged in.");
    $this->assertTrue($userTripalAdmin->hasPermission('administer tripal'), "The priviledged user should have the 'administer tripal' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(200, $status_code, "The priviledged user should be able to access this admin page: $title which should be at '$path'.");
    }

    // Test that the Tripal admin menu includes the above links.
		$html = $this->drupalGet('admin/tripal');
		foreach ($urls as $title => $path) {
			$assert->linkExists($title, 0, "The '$title' link should exist in the Tripal admin listing.");
			$assert->linkByHrefExists($path, 0, "The '$path' link should exist in the Tripal admin listing.");
		}
  }
}
