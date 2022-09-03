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
class TripalRoutePermissionsTest extends BrowserTestBase {

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

  /**
   * Test permissions around Job management pages.
   *
   * @group Tripal Permissions
	 * @group Tripal Jobs
   */
  public function testTripalJobPages() {

    // The job to use for testing.
    $job = new \Drupal\tripal\Services\TripalJob();
    $values = [];
    $values['job_name'] = 'Job ' . uniqid();
    $values['modulename'] = 'tripal';
    $values['callback'] = 'tripal_help';
    $values['ignore_duplicate'] = TRUE;
    $values['uid'] = 1;
    $values['arguments'] = [];
    $job->create($values);
    $job_id = $job->getJobID();

    // The URLs to check.
    $urls = [
      'Listing' => 'admin/tripal/tripal_jobs',
      'Cancel' => 'admin/tripal/tripal_jobs/cancel/' . $job_id,
      'Re-Run' => 'admin/tripal/tripal_jobs/rerun/' . $job_id,
      'View' => 'admin/tripal/tripal_jobs/view/' . $job_id,
    ];

    $permission = 'manage tripal jobs';

    // The users for testing.
    $userAuthenticatedOnly = $this->drupalCreateUser();
		$userTripalJobAdmin = $this->drupalCreateUser([$permission]);

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unpriviledged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unpriviledged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(403, $status_code, "The unpriviledged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, priviledged user.
    // This checks priviledged users can access these pages.
    $this->drupalLogin($userTripalJobAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalJobAdmin), "The priviledged user should be logged in.");
    $this->assertTrue($userTripalJobAdmin->hasPermission($permission), "The priviledged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(200, $status_code, "The priviledged user should be able to access this admin page: $title which should be at '$path'.");
    }
  }

  /**
   * Test permissions around JTripal Dashboard pages.
   *
   * @group Tripal Permissions
   * @group Tripal Dashboard
   */
  public function testTripalDashboardPages() {

    // The URLs to check.
    $urls = [
      'Listing' => 'admin/dashboard',
    ];

    $permission = 'administer tripal';

    // The users for testing.
    $userAuthenticatedOnly = $this->drupalCreateUser();
		$userTripalJobAdmin = $this->drupalCreateUser([$permission]);

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unpriviledged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unpriviledged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(403, $status_code, "The unpriviledged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, priviledged user.
    // This checks priviledged users can access these pages.
    $this->drupalLogin($userTripalJobAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalJobAdmin), "The priviledged user should be logged in.");
    $this->assertTrue($userTripalJobAdmin->hasPermission($permission), "The priviledged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $this->getSession()->getStatusCode();
      $this->assertEquals(200, $status_code, "The priviledged user should be able to access this admin page: $title which should be at '$path'.");
    }
  }

  /**
   * Tests permissions around Tripal content pages.
	 *
	 * Permissions to test:
	 *  - administer tripal content: Allows users to access the Tripal Content listing and add, edit, delete Tripal content of any type.
	 *  - access tripal content overview: Allows the user to access the Tripal content listing.
	 *  - publish tripal content: Allows the user to publish Tripal content of all Tripal Content Types for online access.
	 *  - add tripal content entities: Create new Tripal Content
	 *  - edit tripal content entities: Edit Tripal Content
	 *  - delete tripal content entities: Delete Tripal Content
	 *  - view tripal content entities: View Tripal Content
   *
   * @group Tripal Permissions
	 * @group Tripal Content
   */
  public function testTripalContentPages() {
    $assert = $this->assertSession();

		// Create a Content Type + Entity for this test.
		// -- Content Type.
		$values = [];
	  $values['id'] = random_int(1,500);
		$values['name'] = 'bio_data_' . $values['id'];
		$values['label'] = 'Freddyopolis-' . uniqid();
		$values['category'] = 'Testing';
		$content_type_obj = \Drupal\tripal\Entity\TripalEntityType::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test content type.");
		$content_type_obj->save();
		$content_type = $values['name'];
		// -- Content Entity.
		$values = [];
		$values['title'] = 'Mini Fredicity ' . uniqid();
		$values['type'] = $content_type;
		$entity = \Drupal\tripal\Entity\TripalEntity::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test entity.");
		$entity->save();
		$entity_id = $entity->id();

		// The URLs to check.
    $urls = [
			'canonical' => 'bio_data/' . $entity_id,
		  'add-page' => 'bio_data/add',
		  'add-form' => 'bio_data/add/' . $content_type,
		  'edit-form' => 'bio_data/' . $entity_id . '/edit',
		  'delete-form' => 'bio_data/' . $entity_id . '/delete',
		  'collection' => 'admin/content/bio_data',
			//'publish-content' => '',
			'unpublish-content' => 'admin/content/bio_data/unpublish',
    ];

		// Keys in the array are pages which that permission SHOULD be able to access.
		// It's assumed url keys not in the array should return 403 access denied
		// for that permission.
		$permissions_mapping = [
			'access tripal content overview' => ['collection'],
			'publish tripal content' => ['publish-content', 'unpublish-content'],
			'add tripal content entities' => ['add-page', 'add-form'],
			'edit tripal content entities' => ['edit-form'],
			'delete tripal content entities' => ['delete-form'],
			'view tripal content entities' => ['canonical'],
  		'administer tripal content' => ['canonical', 'add-page', 'add-form', 'edit-form', 'delete-form', 'collection', 'publish-content', 'unpublish-content'],
		];

		// Create users for the tests.
		// -- Create a user that has no extra permissions.
		$userAuthenticatedOnly = $this->drupalCreateUser();
		// -- Create a user with only the specified permission.
		$userPriviledged = [];
		foreach ($permissions_mapping as $permission => $pages) {
			$userPriviledged[$permission] = $this->drupalCreateUser([$permission]);
			$this->assertTrue($userPriviledged[$permission]->hasPermission($permission), "The priviledged user should have the '$permission' permission assigned to it.");
		}

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $assert->statusCodeEquals(403, "The anonymous user should not be able to access any content pages including: $title ($path).");
    }

    // Next check all the URLs with the authenticated, unpriviledged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $assert->statusCodeEquals(403, "The unpriviledged user should not be able to access any content pages including: $title ($path).");
    }

    // Finally use the permissions mapping to check each permission.
    // Keys in the array are pages which that permission SHOULD be able to access.
    // It's assumed url keys not in the array should return 403 access denied
    // for that permission.
    foreach ($permissions_mapping as $permission => $pages_200) {
      $this->drupalLogin($userPriviledged[$permission]);
      foreach ($urls as $title => $path) {
        $html = $this->drupalGet($path);
        $expected_code = (array_search($title, $pages_200) === FALSE) ? 403 : 200;
        $msg_part = ($expected_code === 200) ? 'should have permission to' : 'should be denied access to';

        $status_code = $this->getSession()->getStatusCode();
        $this->assertEquals($expected_code, $status_code, "The user with only '$permission' permission $msg_part $title ($path).");
      }
    }
  }

  /**
   * Test permissions around Tripal Controlled Vocabulary pages.
   *
   * @group Tripal Permissions
   * @group Tripal Controlled Vocabularies
   */
  public function testTripalControlledVocabPages() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

  /**
   * Test permissions around Tripal Data Loaders pages.
   *
   * @group Tripal Permissions
   * @group Tripal Data Loaders
   */
  public function testTripalDataLoadersPages() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

  /**
   * Test permissions around Data Collections pages.
   *
   * @group Tripal Permissions
   * @group Tripal Data Collections
   */
  public function testTripalDataCollectionPages() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

  /**
   * Test permissions around Administering Tripal File Usage pages.
   *
   * @group Tripal Permissions
   * @group Tripal Data Files
   */
  public function testAdminTripalDataFilesPages() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

  /**
   * Test permissions around user file management pages.
   *
   * @group Tripal Permissions
   * @group Tripal Data Files
   */
  public function testTripalDataFilesPages() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }

  /**
   * Test permissions around Term Configuration plugin pages.
   *
   * @group Tripal Permissions
   * @group Tripal Term Configuration
   */
  public function testTripalTermConfigPages() {
    $this->markTestIncomplete(
      'This test has not been implemented yet.'
    );
  }
}
