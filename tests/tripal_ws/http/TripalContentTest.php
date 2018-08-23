<?php

namespace Tests\tripal_ws\http;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalContentTest extends TripalTestCase{

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /** @test */
  public function testGettingMainContentList() {
    //call /web-services/content/v0.1
    $response = $this->get('/web-services/content/v0.1');

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

  /** @test */
  public function testGettingListOfEntitiesInABundle() {
    // Get bundle label
    $label = db_query('SELECT label FROM tripal_bundle LIMIT 1')->fetchField();

    // Call /web-services/content/v0.1/[label]
    $response = $this->get("/web-services/content/v0.1/$label");

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

  /** @test */
  public function testGettingAFeatureResource() {
    // Create an mRNA feature
    $mRNA_term = db_query('SELECT * FROM chado.cvterm WHERE name=:name',
      [':name' => 'mRNA'])->fetchObject();
    $this->assertNotEmpty($mRNA_term);

    $feature = factory('chado.feature')->create([
      'type_id' => $mRNA_term->cvterm_id,
    ]);
    $this->publish('feature', [$feature->feature_id]);

    // Get the entity to retrieve the ID
    $entity_id = chado_get_record_entity_by_table('feature', $feature->feature_id);
    $this->assertNotEmpty($entity_id);

    // Call the web services url
    $response = $this->get("/web-services/content/v0.1/mRNA/$entity_id");
    $response->assertSuccessful();

    $response->assertJsonStructure([
      '@context',
      '@id',
      '@type',
      'label',
      'ItemPage',
      'type',
    ]);

    // Check that the feature name is what we have expected
    $data = $response->json();
    $this->assertEquals($feature->name, $data['name']);
  }
}
