<?php

namespace Tests\tripal_ws\http;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalWebServicesContentTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /** @test */
  public function testGettingMainContentList() {
    $response = $this->get('web-services/content/v0.1');

    // Make sure it returned valid json
    $response->assertSuccessful();
    $response->assertJsonStructure([
      '@context',
      '@id',
      '@type',
      'label',
      'totalItems',
      'member' => [
        [
          '@id',
          '@type',
          'label',
          'description',
        ],
      ],
    ]);
  }

  /** @test
   * @group ws
   */
  public function testGettingListOfEntitiesInABundle() {
    // Get bundle label
    $label = db_query('SELECT label, name FROM tripal_bundle LIMIT 1')->fetchObject();

    // Grant user permission to view bundle.
    $bundle_name = 'view ' . $label->name;
    if (!user_access($bundle_name)) {
      // Either the user is anonymous or registered.
      $role_id = (user_is_anonymous()) ? DRUPAL_ANONYMOUS_RID : DRUPAL_AUTHENTICATED_RID;
      user_role_grant_permissions($role_id, array($bundle_name));
    }

    // Call /web-services/content/v0.1/[label]
    $response = $this->get("web-services/content/v0.1/$label->label");

    // Verify the returned JSON matches the structure
    $response->assertSuccessful();
    $response->assertJsonStructure([
      '@context',
      '@id',
      '@type',
      'label',
      'totalItems',
      'member',
    ]);

    // Verify the collection is of the correct type
    $json = $response->json();
    $this->assertEquals($json['label'], "$label->label Collection");
  }
}
