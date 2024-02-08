<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\file\Entity\File;
use Drupal\user\Entity\Role;
use Drupal\Core\Url;

/**
 * Tests the basic functions of the TripalTerm Entity Type.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Permissions
 */
class TripalChadoRoutePermissionsTest extends BrowserTestBase {

  protected $defaultTheme = 'stark';

  protected static $modules = ['tripal', 'tripal_chado', 'file', 'field_ui'];

  /**
   * Test all the base Tripal Chado admin paths.
   *
   */
  public function testTripalChadoAdminPages() {
    $session = $this->getSession();

    // The URLs to check with the key being the label expected in the
    // Tripal admin menu listing.
    $urls = [
      'Data Loaders' => 'admin/tripal/loaders',
      'Data Storage' => 'admin/tripal/storage',
      // Under Drupal ~10.2, if there are no extensions present, and there aren't, then
      // we won't be able to access the 'admin/tripal/extension' menu, even as admin.
      // To test, we would have to create an extension first.
      // 'Extensions' => 'admin/tripal/extension',
    ];

    $userAuthenticatedOnly = $this->drupalCreateUser();
    // Drupal 10.2 tightens permissions, second permission is needed to access importers
    $userTripalAdmin = $this->drupalCreateUser(['administer tripal', 'allow tripal import']);

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unprivileged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission('administer tripal'), "The unprivileged user should not have the 'administer tripal' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, privileged user.
    // This checks privileged users can access these pages.
    $this->drupalLogin($userTripalAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalAdmin), "The privileged user should be logged in.");
    $this->assertTrue($userTripalAdmin->hasPermission('administer tripal'), "The privileged user should have the 'administer tripal' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(200, $status_code, "The privileged user should be able to access this admin page: $title which should be at '$path'.");
    }

    // Test that the Tripal admin menu includes the above links.
    // We use try/catch here because WebAssert throws exceptions which are not very readable.
    $assert = $this->assertSession();
    $html = $this->drupalGet('admin/tripal');
    unset($urls['Tripal']);
    foreach ($urls as $label => $path) {
      // -- Find links with the label.
      try {
        $assert->linkExists($label, 0);
      }
      catch (Exception $e) {
        $this->assertTrue(FALSE, "The '$label' link should exist in the Tripal admin listing.");
      }

      // -- Find links with the URL/path.
      try {
        $assert->linkByHrefExists($path, 0);
      }
      catch (Exception $e) {
        $this->assertTrue(FALSE, "The '$path' link should exist in the Tripal admin listing.");
      }
    }
  }

}
