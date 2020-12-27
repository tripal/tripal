<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\tripal\Entity\TripalTerm;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the basic functions of the TripalTerm Entity Type.
 *
 * @group Tripal
 * @group Tripal Term
 * @group Tripal Entities
 */
class TripalMenuPathsTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;
  protected $defaultTheme = 'stable';

  public static $modules = ['tripal', 'block', 'field_ui'];

  /**
   * Test all paths exposed by the module, by permission.
   *
   * @group tripal_admin_path
   */
  public function testPaths() {
    $assert = $this->assertSession();

    // Gather the test data.
    $data = $this->providerTestPaths();

    // All users with administer Tripal should be able to
    // access the Tripal administration menu.
    $admin_user = $this->drupalCreateUser([
      'access administration pages',
      'administer tripal'
    ]);
    $this->drupalLogin($admin_user);

    // Run the tests.
    foreach ($data as $datum) {
      $html = $this->drupalGet($datum[1]);
      $assert->statusCodeEquals($datum[0], 'Recieved an unexpected status code for '.$datum[1]);
    }
  }

  /**
   * Data provider for testPaths.
   *
   * @return array
   *   Nested array of testing data. Arranged like this:
   *   - Expected response code.
   *   - Path to request.
   */
  protected function providerTestPaths() {
    return [
      [200, 'admin/tripal'],

      [200, 'admin/tripal/register'],
      [200, 'admin/tripal/storage'],
      [200, 'admin/tripal/extension'],
      [200, 'admin/tripal/tripal_jobs'],
      [200, 'admin/tripal/loaders'],
      [200, 'admin/tripal/data-collections'],
      [200, 'admin/tripal/files'],
      [200, 'admin/content/bio_data/unpublish'],
    ];
  }

}
