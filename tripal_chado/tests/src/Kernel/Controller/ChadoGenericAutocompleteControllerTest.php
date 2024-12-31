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

  protected array $types;

  protected array $project_ids;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Open connection to a test Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Look up a few cvterms
    $terms = ['null', 'Genome Project', 'Book', 'Proceedings Article'];
    foreach ($terms as $term) {
      $this->types[$term] = $this->connection
        ->select('1:cvterm', 'T')
        ->fields('T', ['cvterm_id'])
        ->condition('T.name', $term, '=')
        ->execute()
        ->fetchField();
    }

    // Create a small number of projects with some various type_id values
    // Set type_id of null on projects: Good, Winner and Great.
    $projects = [
      'Project Okay' => ['type_id' => $this->types['Genome Project']],
      'Project Good' => ['type_id' => $this->types['null']],
      'Project Best' => ['type_id' => $this->types['Genome Project']],
      'Project Winner' => ['type_id' => $this->types['null']],
      'Project Super' => ['type_id' => $this->types['Genome Project']],
      'Project Great' => ['type_id' => $this->types['null']],
      'Project Green' => ['type_id' => $this->types['Genome Project']],
      'Awesome Project' => ['type_id' => $this->types['Genome Project']],
      'Wow Project' => ['type_id' => $this->types['Genome Project']],
      'Yes Project' => ['type_id' => NULL],
    ];

    // Create test project records with type_id set for all but the last one
    foreach ($projects as $name => $options) {
      $description = $name . ' description';
      $project_id = $this->connection
        ->insert('1:project')
        ->fields(['name' => $name, 'description' => $description])
        ->execute();
      $this->project_ids[$name] = $project_id;

      if (!is_null($options['type_id'])) {
        $select = $this->connection
          ->insert('1:projectprop')
          ->fields(['project_id' => $project_id, 'type_id' => $options['type_id'], 'value' => 'test type_id'])
          ->execute();
        // Add another random property to make sure having multiple properties is okay
        $select = $this->connection
          ->insert('1:projectprop')
          ->fields(['project_id' => $project_id, 'type_id' => 2, 'value' => 'random property'])
          ->execute();
      }
    }

    // Create a large number of pubs of two types
    for ($i=1; $i <= 101; $i++) {
      $this->connection->insert('1:pub')
        ->fields([
          'title' => 'Publication No. ' . $i,
          'uniquename' => 'ID' . $i,
          'type_id' => (($i % 2) == 0) ? $this->types['Book'] : $this->types['Proceedings Article'],
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

    // Test for an internal string, matches "Great" and "Green"
    $request = Request::create(
      'chado/generic/autocomplete/project/name/project/10/0',
      'GET',
      ['q' => 'gre']
    );
    $results = $autocomplete->handleAutocomplete($request, 'project', 'name', 'project', 10, 0)->getContent();
    $this->assertCount(2, json_decode($results), 'Expected exactly two results from "gre" project generic autocomplete');

    // Make sure both of these have the expected pkey id embedded
    foreach (json_decode($results) as $result) {
      $value = $result->value;
      $title = preg_replace('/ \(\d+\)$/', '', $value);
      $expected_id = $this->project_ids[$title] ?? -1;
      $id = $autocomplete->getPkeyId($value);
      $this->assertGreaterThan(0, $expected_id, 'Failed determining expected project_id from ' . $value);
      $this->assertIsInt($id, 'Expected integer from getPkeyId()');
      $this->assertGreaterThan(0, $id, 'Expected a positive integer from getPkeyId() operating on ' . $value);
      $this->assertEquals($expected_id, $id, 'Retrieved id did not match expected value for ' . $value);
    }

    // Test getPkeyId on a string without embedded pkey id
    $id = $autocomplete->getPkeyId('This is missing an embedded value');
    $this->assertEquals(0, $id, 'getPkeyId() should return zero when there is no embedded id in parentheses');
    $id = $autocomplete->getPkeyId('');
    $this->assertEquals(0, $id, 'getPkeyId() should return zero for an empty string');

    // Test count limit
    $request = Request::create(
      'chado/generic/autocomplete/pub/title/pub/10/0',
      'GET',
      ['q' => 'p']
    );
    $results = $autocomplete->handleAutocomplete($request, 'pub', 'title', 'pub', 10, 0)->getContent();
    $this->assertCount(10, json_decode($results), 'Expected exactly 10 results from pub generic autocomplete');

    // Test limiting by type_id for a table with a type_id column
    $type_id = $this->types['Proceedings Article'];
    $request = Request::create(
      'chado/generic/autocomplete/pub/title/pub/1000/' . $type_id,
      'GET',
      ['q' => 'p']
    );
    $results = $autocomplete->handleAutocomplete($request, 'pub', 'title', 'pub', 1000, $type_id)->getContent();
    $this->assertCount(51, json_decode($results), 'Expected exactly 51 results from pub generic autocomplete for Proceedings Article containing "p"');

    // Test limiting by type_id for a table with a type_id in a property table
    $type_id = $this->types['Genome Project'];
    $request = Request::create(
      'chado/generic/autocomplete/project/name/projectprop/1000/' . $type_id,
      'GET',
      ['q' => 'w']
    );
    $results = $autocomplete->handleAutocomplete($request, 'project', 'name', 'projectprop', 1000, $type_id)->getContent();
    $this->assertCount(2, json_decode($results), 'Expected exactly 2 results from project generic autocomplete for Genome Project containing "w"');

  }
}
