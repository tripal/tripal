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

  // A couple of users to test permissions with.
  // On set-up each user has the corresponding empty role.
  // Note that the user always has the default permissions
  // derived from the "authenticated users" role.
  public $user1;
  public $user2;

  // A couple of roles to add permissions to.
  // On set-up each user has the corresponding empty role.
  public $role1;
  public $role2;

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

    // User 1: Authenticated user WITHOUT admin tripal permission.
    $this->role1->grantPermission('access administration pages');
    $this->role1->save();
    // User 2: Authenticated user WITH admin tripal permission.
    $this->role2->grantPermission('access administration pages');
    $this->role2->grantPermission('administer tripal');
    $this->role2->save();

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $assert->statusCodeEquals(403, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unpriviledged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($this->user1);
    $this->assertFalse($this->user1->hasPermission('administer tripal'), "The unpriviledged user should not have the 'administer tripal' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $assert->statusCodeEquals(403, "The unpriviledged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, priviledged user.
    // This checks priviledged users can access these pages.
    $this->drupalLogin($this->user2);
    $this->assertTrue($this->drupalUserIsLoggedIn($this->user2), "The priviledged user should be logged in.");
    $this->assertTrue($this->user2->hasPermission('administer tripal'), "The priviledged user should have the 'administer tripal' permission.");
    foreach ($urls as $title => $path) {
      $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(200, $status_code, "The priviledged user should be able to access this admin page: $title.");
    }

    // Test that the Tripal admin menu includes the above links.

    $html = $this->drupalGet($path);
    print_r($html);
  }

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $this->user1 = $this->drupalCreateUser();
    $this->user2 = $this->drupalCreateUser();

    $rid1 = $this->drupalCreateRole([]);
    $this->role1 = Role::load($rid1);
    $rid2 = $this->drupalCreateRole([]);
    $this->role2 = Role::load($rid2);

    $this->user1->addRole( $rid1 );
    $this->user2->addRole( $rid2 );
  }
}
