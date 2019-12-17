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

  /**
   * Tests the sanitizeFieldKeys() method.
   * @group tripal_ws
   * @group tripalWS-ServiceResource
   */
  public function testSanitizeFieldKeys() {

    // We need a bundle in order to determine a valid base path.
    $label = db_query('SELECT label FROM tripal_bundle LIMIT 1')->fetchField();
    $bundle = tripal_load_bundle_entity(['label' => $label]);
    $this->assertNotNull($bundle,
      "Unable to load the associated bundle object.");

    // We need a resource to add context to.
    $base_path = "web-services/content/v0.1/$label";
    $resource = new \TripalWebServiceResource($base_path);
    $this->assertNotNull($resource,
      "Unable to create a Tripal Web Service Resource for testing.");

    // We need a ContentService object to call sanitizeFieldKeys().
    module_load_include('inc', 'tripal_ws', 'includes/TripalWebService/TripalContentService_v0_1');
    $web_service = new \TripalContentService_v0_1($base_path);
    $this->assertNotNull($web_service,
      "Unable to create a TripalContentService_v0_1 object for testing.");

    // Now finally, we try to test it!
    // - Associative array where keys are valid terms.
    $value = [
      'rdfs:type' => 'fake',
    ];
    $sanitized_value = $this->invokeMethod($web_service, 'sanitizeFieldKeys', [
      $resource,
      $value,
      $bundle,
      $base_path
    ]);
    $this->assertNotNull($sanitized_value,
      "You should be able to sanitize a term-indexed array if terms are valid.");

    // - Numeric keys to direct values.
    $value = [
      'fake',
      'none',
      5
    ];
    $sanitized_value = $this->invokeMethod($web_service, 'sanitizeFieldKeys', [
      $resource,
      $value,
      $bundle,
      $base_path
    ]);
    $this->assertNotNull($sanitized_value,
      "You should be able to sanitize a numeric-indexed array if sub-elements are direct values.");

    // - Numeric keys where value is an array with term keys.
    $value = [
      ['rdfs:type' => 'fake'],
      ['rdfs:type' => 'none'],
    ];
    $sanitized_value = $this->invokeMethod($web_service, 'sanitizeFieldKeys', [
      $resource,
      $value,
      $bundle,
      $base_path
    ]);
    $this->assertNotNull($sanitized_value,
      "You should be able to sanitize a numeric-indexed array if sub-elements are also arrays.");

    // - Numeric keys where value is an array with random keys.
    //   (random keys should be removed.)
    $value = [
      ['randomnotterm' => 'fake'],
    ];
    $sanitized_value = $this->invokeMethod($web_service, 'sanitizeFieldKeys', [
      $resource,
      $value,
      $bundle,
      $base_path
    ]);
    $this->assertEmpty($sanitized_value[0],
      "You should be able to sanitize a numeric-indexed array if sub-elements arrays are not keyed with valid terms.");

  }

  /**
   * Call protected/private method of a class.
   *
   * @param object &$object    Instantiated object that we will run method on.
   * @param string $methodName Method name to call
   * @param array  $parameters Array of parameters to pass into method.
   *
   * @return mixed Method return.
   */
  public function invokeMethod(&$object, $methodName, array $parameters = array()) {
      $reflection = new \ReflectionClass(get_class($object));
      $method = $reflection->getMethod($methodName);
      $method->setAccessible(true);

      return $method->invokeArgs($object, $parameters);
  }
}
