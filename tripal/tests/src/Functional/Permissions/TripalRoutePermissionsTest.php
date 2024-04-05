<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\file\Entity\File;
use Drupal\user\Entity\Role;
use Drupal\Core\Session;
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
  protected $defaultTheme = 'stark';

  protected static $modules = ['tripal', 'file', 'field_ui'];

  /**
   * Test all the base Tripal admin paths.
   *
   * @group Tripal Permissions
   */
  public function testTripalAdminPages() {
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

    // The URLs to check with the key being the label expected in the
    // Tripal admin menu listing.
    $urls = [
      'Tripal' => 'admin/tripal',
      'Registration' => 'admin/tripal/register',
      'Jobs' => 'admin/tripal/tripal_jobs',
      'Data Collections' => 'admin/tripal/data-collections',
      'Tripal Managed Files' => 'admin/tripal/files',
      'Tripal Content Terms' => 'admin/tripal/config/terms',
      // Under Drupal ~10.2, if there are no extensions present, and there aren't, then
      // we won't be able to access the 'admin/tripal/extension' menu, even as admin.
      // To test, we would have to create an extension first.
      // 'Extensions' => 'admin/tripal/extension',
    ];

    $userAuthenticatedOnly = $this->drupalCreateUser();
    // Drupal 10.2 tightens permissions, second permission is needed to access files path
    $userTripalAdmin = $this->drupalCreateUser(['administer tripal', 'admin tripal files']);

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

  /**
   * Test permissions around Job management pages.
   *
   * @group Tripal Permissions
   * @group Tripal Jobs
   */
  public function testTripalJobPages() {
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

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
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unprivileged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unprivileged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, privileged user.
    // This checks privileged users can access these pages.
    $this->drupalLogin($userTripalJobAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalJobAdmin), "The privileged user should be logged in.");
    $this->assertTrue($userTripalJobAdmin->hasPermission($permission), "The privileged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(200, $status_code, "The privileged user should be able to access this admin page: $title which should be at '$path'.");
    }
  }

  /**
   * Test permissions around JTripal Dashboard pages.
   *
   * @group Tripal Permissions
   * @group Tripal Dashboard
   */
  public function testTripalDashboardPages() {
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

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
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access this admin page: $title.");
    }

    // Next check all the URLs with the authenticated, unprivileged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unprivileged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, privileged user.
    // This checks privileged users can access these pages.
    $this->drupalLogin($userTripalJobAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalJobAdmin), "The privileged user should be logged in.");
    $this->assertTrue($userTripalJobAdmin->hasPermission($permission), "The privileged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(200, $status_code, "The privileged user should be able to access this admin page: $title which should be at '$path'.");
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
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

    // Create a Content Type + Entity for this test.
    // -- Content Type.
    $values = [];
    $values['label'] = 'Freddyopolis-' . uniqid();
    $values['id'] = 'freddy';
    $values['termIdSpace'] = 'FRED';
    $values['termAccession'] = '1g2h3j4k5';
    $values['help_text'] = 'This is just random text to meet the requirement of this field.';
    $values['category'] = 'Testing';
    $content_type_obj = \Drupal\tripal\Entity\TripalEntityType::create($values);
    $this->assertIsObject($content_type_obj, "Unable to create a test content type.");
    $content_type_obj->save();
    $content_type = $content_type_obj->id();
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
      'entity-canonical' => 'bio_data/' . $entity_id,
      'entity-add-page' => 'bio_data/add',
      'entity-add-form' => 'bio_data/add/' . $content_type,
      'entity-edit-form' => 'bio_data/' . $entity_id . '/edit',
      'entity-delete-form' => 'bio_data/' . $entity_id . '/delete',
      'entity-collection' => 'admin/content/bio_data',
      //'publish-content' => '',
      'unpublish-content' => 'admin/content/bio_data/unpublish',
      'entitytype-add-form' => 'admin/structure/bio_data/add',
      'entitytype-edit-form' => 'admin/structure/bio_data/manage/' . $content_type,
      'entitytype-delete-form' => 'admin/structure/bio_data/manage/' . $content_type . '/delete',
      'entitytype-manage-fields' => 'admin/structure/bio_data/manage/' . $content_type . '/fields',
      'entitytype-manage-form' => 'admin/structure/bio_data/manage/' . $content_type . '/form-display',
      'entitytype-manage-display' => 'admin/structure/bio_data/manage/' . $content_type . '/display',
      'entitytype-collection' => 'admin/structure/bio_data',
    ];

    // Keys in the array are pages which that permission SHOULD be able to access.
    // It's assumed url keys not in the array should return 403 access denied
    // for that permission.
    $permissions_mapping = [
      'access tripal content overview' => ['entity-collection'],
      'publish tripal content' => ['publish-content', 'unpublish-content'],
      'add tripal content entities' => ['entity-add-form'],
      'edit tripal content entities' => ['entity-edit-form'],
      'delete tripal content entities' => ['entity-delete-form'],
      'view tripal content entities' => ['entity-canonical'],
      'administer tripal content' => ['entity-canonical', 'entity-add-page', 'entity-add-form', 'entity-edit-form', 'entity-delete-form', 'entity-collection', 'publish-content', 'unpublish-content'],
      'manage tripal content types' => ['entitytype-add-form', 'entitytype-edit-form', 'entitytype-delete-form', 'entitytype-collection'],
      'administer tripal_entity fields' => ['entitytype-manage-fields'],
      'administer tripal_entity form display' => ['entitytype-manage-form'],
      'administer tripal_entity display' => ['entitytype-manage-display'],
    ];

    // Create users for the tests.
    // -- Create a user that has no extra permissions.
    $userAuthenticatedOnly = $this->drupalCreateUser();
    // -- Create a user with only the specified permission.
    $userprivileged = [];
    foreach ($permissions_mapping as $permission => $pages) {
      $userprivileged[$permission] = $this->drupalCreateUser([$permission]);
      $this->assertTrue($userprivileged[$permission]->hasPermission($permission), "The privileged user should have the '$permission' permission assigned to it.");
    }

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access any content pages including: $title ($path).");
    }

    // Next check all the URLs with the authenticated, unprivileged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access any content pages including: $title ($path).");
    }

    // Finally use the permissions mapping to check each permission.
    // Keys in the array are pages which that permission SHOULD be able to access.
    // It's assumed url keys not in the array should return 403 access denied
    // for that permission.
    foreach ($permissions_mapping as $permission => $pages_200) {
      $this->drupalLogin($userprivileged[$permission]);
      foreach ($urls as $title => $path) {
        $html = $this->drupalGet($path);
        $expected_code = (array_search($title, $pages_200) === FALSE) ? 403 : 200;
        $msg_part = ($expected_code === 200) ? 'should have permission to' : 'should be denied access to';

        $status_code = $session->getStatusCode();
        $this->assertEquals($expected_code, $status_code, "The user with only '$permission' permission $msg_part $title ($path).");
      }
    }
  }


  /**
   * Test permissions around Administering Tripal File Usage pages.
   *
   * @group Tripal Permissions
   * @group Tripal Data Files
   */
  public function testAdminTripalDataFilesPages() {
    $session = $this->getSession();
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');

    // The URLs to check.
    $urls = [
      'Tripal Managed Files' => 'admin/tripal/files',
      'Manage File Upload Size' => 'admin/tripal/files/manage',
      'Add Custom User Quota' => 'admin/tripal/files/quota/add',
      'File Usage Reports' => 'admin/tripal/files/usage',
      'Disk Usage Quotas' => 'admin/tripal/files/quota',
    ];

    $permission = 'admin tripal files';

    // The users for testing.
    $userAuthenticatedOnly = $this->drupalCreateUser();
    $userTripalAdmin = $this->drupalCreateUser([$permission]);

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
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unprivileged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, privileged user.
    // This checks privileged users can access these pages.
    $this->drupalLogin($userTripalAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalAdmin), "The privileged user should be logged in.");
    $this->assertTrue($userTripalAdmin->hasPermission($permission), "The privileged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(200, $status_code, "The privileged user should be able to access this admin page: $title which should be at '$path'.");
    }
  }

  /**
   * Test permissions around user file management pages.
   *
   * @group Tripal Permissions
   * @group Tripal Data Files
   */
  public function testTripalDataFilesPages() {
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

    $permission = 'manage tripal files';

    // The users for testing.
    $userAuthenticatedOnly = $this->drupalCreateUser();
    $userTripalwFile = $this->drupalCreateUser([$permission]);
    $uid = $userTripalwFile->id();
    $userTripalwithoutFile = $this->drupalCreateUser([$permission]);

    // The file for testing.
    $uri = 'public://Файл для тестирования' . uniqid() . '.testing.txt';
    $contents = "file_put_contents() doesn't seem to appreciate empty strings so let's put in some data.";
    file_put_contents($uri, $contents);
    $file = File::create([
      'uri' => $uri,
      'uid' => $uid,
    ]);
    $file->save();
    $file_id = $file->id();

    // The URLs to check.
    $urls = [
      'Files' => 'user/' . $uid . '/files',
      'File Details' => 'user/' . $uid . '/files/' . $file_id . '',
      'Renew File' => 'user/' . $uid . '/files/' . $file_id . '/renew',
      'Download File' => 'user/' . $uid . '/files/' . $file_id . '/download',
      'Delete File' => 'user/' . $uid . '/files/' . $file_id . '/delete',
    ];

    // First check all the URLs with no user logged in.
    // This checks the anonymous user cannot access these pages.
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The anonymous user should not be able to access this page: $title.");
    }

    // Next check all the URLs with the authenticated, unprivileged user.
    // This checks generic authenticated users cannot access these pages.
    $this->drupalLogin($userAuthenticatedOnly);
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unprivileged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access this page: $title.");
    }

    // Then check all the URLs with a privileged user different from the one in the URLs.
    // This checks only the user who owns the file/profile can access the pages.
    $this->drupalLogin($userTripalwithoutFile);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalwithoutFile), "The unrelated privileged user should be logged in.");
    $this->assertTrue($userTripalwithoutFile->hasPermission($permission), "The unrelated riviledged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unrelated privileged user should not be able to access this page: $title which should be at '$path'.");
    }

    // Finally check all URLs with the authenticated, privileged user.
    // This checks privileged users can access these pages.
    $this->drupalLogin($userTripalwFile);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalwFile), "The privileged user who owns the file should be logged in.");
    $this->assertTrue($userTripalwFile->hasPermission($permission), "The privileged user who owns the file should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(200, $status_code, "The privileged user who owns the file should be able to access this page: $title which should be at '$path'.");
    }
  }

  /**
   * Test permissions around Term Configuration plugin pages.
   *
   * @group Tripal Permissions
   * @group Tripal Term Configuration
   */
  public function testTripalTermConfigPages() {
    $this->assertTrue(\Drupal::request()->hasSession(),
      'This test depends on having a session but for some reason there is not one available.');
    $session = $this->getSession();

    // The URLs to check.
    $urls = [
      'Listing' => 'admin/tripal/config/terms',
      'add-form' => 'admin/tripal/config/terms/add',
    ];

    $permission = 'administer tripal';

    // The users for testing.
    $userAuthenticatedOnly = $this->drupalCreateUser();
    $userTripalAdmin = $this->drupalCreateUser([$permission]);

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
    $this->assertFalse($userAuthenticatedOnly->hasPermission($permission), "The unprivileged user should not have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(403, $status_code, "The unprivileged user should not be able to access this admin page: $title.");
    }

    // Finally check all URLs with the authenticated, privileged user.
    // This checks privileged users can access these pages.
    $this->drupalLogin($userTripalAdmin);
    $this->assertTrue($this->drupalUserIsLoggedIn($userTripalAdmin), "The privileged user should be logged in.");
    $this->assertTrue($userTripalAdmin->hasPermission($permission), "The privileged user should have the '$permission' permission.");
    foreach ($urls as $title => $path) {
      $html = $this->drupalGet($path);
      $status_code = $session->getStatusCode();
      $this->assertEquals(200, $status_code, "The privileged user should be able to access this admin page: $title which should be at '$path'.");
    }
  }
}
