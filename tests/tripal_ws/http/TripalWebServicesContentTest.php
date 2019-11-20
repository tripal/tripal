<?php

namespace Tests\tripal_ws\http;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalWebServicesContentTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /** @test */
  public function testGettingMainContentList() {
    // Grant user permission to all content.
    $role_id = (user_is_anonymous()) ? DRUPAL_ANONYMOUS_RID : DRUPAL_AUTHENTICATED_RID;
    $bundles = db_query('SELECT name FROM tripal_bundle');
    foreach($bundles as $bundle) {
      $bundle_name = 'view ' . $bundle->name;
      user_role_grant_permissions($role_id, array($bundle_name));
    }

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

    // Grant user permission to this content.
    $role_id = (user_is_anonymous()) ? DRUPAL_ANONYMOUS_RID : DRUPAL_AUTHENTICATED_RID;
    user_role_grant_permissions($role_id, array('view ' . $label->name));

    // Call /web-services/content/v0.1/[label]
    $ctype = preg_replace('/[^\w]/', '_', $label->label);
    $response = $this->get("web-services/content/v0.1/" . $ctype);

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
