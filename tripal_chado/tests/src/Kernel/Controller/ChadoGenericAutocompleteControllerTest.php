<?php

namespace Drupal\Tests\tripal_chado\Kernel;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use Symfony\Component\HttpFoundation\Request;
use Drupal\tripal_chado\Controller\ChadoGenericAutocompleteController;

/**
 * Tests the Generic Autocomplete.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Autocomplete
 */
class ChadoGenericAutocompleteControllerTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected $connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Open connection to a test Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Create a small number of projects.
    for ($i=1; $i <= 3; $i++) {
      $this->connection->insert('1:project')
        ->fields([
          'name' => 'Project No. ' . $i,
          'description' => 'Description No. ' . $i,
        ])->execute();
    }

    // Create a large number of pubs.
    for ($i=1; $i <= 51; $i++) {
      $this->connection->insert('1:pub')
        ->fields([
          'title' => 'Publication No. ' . $i,
          'uniquename' => 'ID' . $i,
          'type_id' => (($i % 20) + 1),
        ])->execute();
    }
  }

  /**
   * Tests the Chado Generic Autocomplete using project and pub tables.
   */
  public function testChadoGenericAutocompleteController() {

    $autocomplete = new ChadoGenericAutocompleteController();
    $this->assertIsObject($autocomplete, 'Failed to create the ChadoGenericAutocompleteController');

    // Test empty string
    $request = Request::create(
      'chado/generic/autocomplete/project/name/project/10/0',
      'GET',
      ['q' => '']
    );
    $results = $autocomplete->handleAutocomplete($request, 'project', 'name', 'project', 10, 0)->getContent();
    $this->assertCount(0, json_decode($results), 'Expected no results from empty string project generic autocomplete');

    // Test for string with no matches
    $request = Request::create(
      'chado/generic/autocomplete/project/name/project/10/0',
      'GET',
      ['q' => 'xxx']
    );
    $results = $autocomplete->handleAutocomplete($request, 'project', 'name', 'project', 10, 0)->getContent();
    $this->assertCount(0, json_decode($results), 'Expected no results from "xxx" project generic autocomplete');

    // Test for an internal string
    $request = Request::create(
      'chado/generic/autocomplete/project/name/project/10/0',
      'GET',
      ['q' => 'rojec']
    );
    $results = $autocomplete->handleAutocomplete($request, 'project', 'name', 'project', 10, 0)->getContent();
    $this->assertCount(3, json_decode($results), 'Expected exactly three results from "rojec" project generic autocomplete');

    // Test count limit
    $request = Request::create(
      'chado/generic/autocomplete/analysis/name/pub/10/0',
      'GET',
      ['q' => 'p']
    );
    $results = $autocomplete->handleAutocomplete($request, 'pub', 'title', 'pub', 10, 0)->getContent();
    $this->assertCount(10, json_decode($results), 'Expected exactly 10 results from pub generic autocomplete');

    // Test limiting by type_id=2
    $request = Request::create(
      'chado/generic/autocomplete/analysis/name/pub/10/2',
      'GET',
      ['q' => 'p']
    );
    $results = $autocomplete->handleAutocomplete($request, 'pub', 'title', 'pub', 10, 2)->getContent();
    $this->assertCount(3, json_decode($results), 'Expected exactly 3 results from pub generic autocomplete type_id=2');

  }
}
