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
    $label = db_query('SELECT label FROM tripal_bundle LIMIT 1')->fetchField();

    // Call /web-services/content/v0.1/[label]
    $response = $this->get("web-services/content/v0.1/$label");

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
    $this->assertEquals($json['label'], "$label Collection");
  }
}
