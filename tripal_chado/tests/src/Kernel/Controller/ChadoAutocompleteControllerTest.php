<?php

namespace Drupal\Tests\tripal_chado\Kernel;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Symfony\Component\HttpFoundation\Request;
use Drupal\tripal_chado\Controller\ChadoGenericAutocompleteController;
use Drupal\tripal_chado\Controller\ChadoProjectAutocompleteController;
use Drupal\tripal_chado\Controller\ChadoOrganismAutocompleteController;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\IgnoreDeprecations;

/**
 * Tests the Generic Autocomplete.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Autocomplete
 */
#[Group('chado')]
#[Group('autocomplete')]
#[IgnoreDeprecations]
class ChadoAutocompleteControllerTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected $connection;

  protected array $types;

  protected array $projects;

  protected array $project_ids;

  protected array $organisms;

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
    $this->projects = [
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
    foreach ($this->projects as $name => $options) {
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

    // Create some test organisms
    $subspecies_id = $this->connection->select('1:cvterm', 'T')
      ->fields('T', ['cvterm_id'])
      ->condition('T.name', 'subspecies', '=')
      ->execute()
      ->fetchField();
    $this->organisms = [
      1 => ['genus' => 'Tripalus', 'species' => 'bogusii', 'type_id' => $subspecies_id, 'infraspecific_name' => 'fakus', 'abbreviation' => 'T. bogusii subsp. fakus'],
      2 => ['genus' => 'Tripalus', 'species' => 'databasica', 'type_id' => NULL, 'infraspecific_name' => NULL, 'abbreviation' => NULL],
      3 => ['genus' => 'Drupalus', 'species' => 'fictus', 'type_id' => NULL, 'infraspecific_name' => NULL, 'abbreviation' => 'Drupalus fictus'],
    ];
    foreach ($this->organisms as $details) {
      $insert = $this->connection->insert('1:organism');
      $insert->fields([
        'genus' => $details['genus'],
        'species' => $details['species'],
        'type_id' => $details['type_id'] ?? NULL,
        'infraspecific_name' => $details['infraspecific_name'] ?? NULL,
        'abbreviation' => $details['abbreviation'] ?? NULL,
        'common_name' => $details['common_name'] ?? NULL,
        'comment' => $details['comment'] ?? NULL,
      ]);
      $insert->execute();
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

    $generic_autocomplete = new ChadoGenericAutocompleteController();
    $this->assertIsObject($generic_autocomplete, 'Failed to create the ChadoGenericAutocompleteController');

    $project_autocomplete = new ChadoProjectAutocompleteController();
    $this->assertIsObject($project_autocomplete, 'Failed to create the ChadoProjectAutocompleteController');

    $organism_autocomplete = new ChadoOrganismAutocompleteController();
    $this->assertIsObject($organism_autocomplete, 'Failed to create the ChadoOrganismAutocompleteController');

    // Test empty string
    $request = Request::create(
      'chado/generic/autocomplete/project/name/x/project/10/0',
      'GET',
      ['q' => '']
    );
    $results = $generic_autocomplete->handleGenericAutocomplete($request, 'project', 'name', 'x', 'project', 10, 0)->getContent();
    $this->assertCount(0, json_decode($results, FALSE), 'Expected no results from empty string project generic autocomplete');

    // Test for string with no matches
    $request = Request::create(
      'chado/generic/autocomplete/project/name/x/project/10/0',
      'GET',
      ['q' => 'xxx']
    );
    $results = $generic_autocomplete->handleGenericAutocomplete($request, 'project', 'name', 'x', 'project', 10, 0)->getContent();
    $this->assertCount(0, json_decode($results, FALSE), 'Expected no results from "xxx" project generic autocomplete');

    // Test for an internal string, matches "Great" and "Green"
    $request = Request::create(
      'chado/generic/autocomplete/project/name/x/project/10/0',
      'GET',
      ['q' => 'gre']
    );
    $results = $generic_autocomplete->handleGenericAutocomplete($request, 'project', 'name', 'x', 'project', 10, 0)->getContent();
    $this->assertCount(2, json_decode($results, FALSE), 'Expected exactly two results from "gre" project generic autocomplete');

    // Make sure both of these have the expected pkey id embedded
    foreach (json_decode($results, FALSE) as $result) {
      $value = $result->value;
      $title = preg_replace('/ \(\d+\)$/', '', $value);
      $expected_id = $this->project_ids[$title] ?? -1;
      $id = $generic_autocomplete->getPkeyId($value);
      $this->assertGreaterThan(0, $expected_id, 'Failed determining expected project_id from ' . $value);
      $this->assertIsInt($id, 'Expected integer from getPkeyId()');
      $this->assertGreaterThan(0, $id, 'Expected a positive integer from getPkeyId() operating on ' . $value);
      $this->assertEquals($expected_id, $id, 'Retrieved id did not match expected value for ' . $value);
    }

    // Test getPkeyId on a string without embedded pkey id
    $id = $generic_autocomplete->getPkeyId('This is missing an embedded value');
    $this->assertEquals(0, $id, 'getPkeyId() should return zero when there is no embedded id in parentheses');
    $id = $generic_autocomplete->getPkeyId('');
    $this->assertEquals(0, $id, 'getPkeyId() should return zero for an empty string');

    // Test count limit
    $request = Request::create(
      'chado/generic/autocomplete/pub/title/type_id/pub/10/0',
      'GET',
      ['q' => 'p']
    );
    $results = $generic_autocomplete->handleGenericAutocomplete($request, 'pub', 'title', 'type_id', 'pub', 10, 0)->getContent();
    $decoded_results = json_decode($results, FALSE);
    $this->assertCount(10, $decoded_results, 'Expected exactly 10 results from pub generic autocomplete');
    $this->assertMatchesRegularExpression('/\[.*\]/', $decoded_results[0]->value, 'The publication type in brackets should be present when a type is not specified: ' . $decoded_results[0]->value);

    // Test limiting by type_id for a table with a type_id column
    $type_id = $this->types['Proceedings Article'];
    $request = Request::create(
      'chado/generic/autocomplete/pub/title/type_id/pub/1000/' . $type_id,
      'GET',
      ['q' => 'p']
    );
    $results = $generic_autocomplete->handleGenericAutocomplete($request, 'pub', 'title', 'type_id', 'pub', 1000, $type_id)->getContent();
    $decoded_results = json_decode($results, FALSE);
    $this->assertCount(51, $decoded_results, 'Expected exactly 51 results from pub generic autocomplete for Proceedings Article containing "p"');
    $this->assertDoesNotMatchRegularExpression('/\[.*\]/', $decoded_results[0]->value, 'The publication type in brackets should be absent when a type is specified: ' . $decoded_results[0]->value);

    // Test limiting by type_id for a table with a type_id in a property table
    $type_id = $this->types['Genome Project'];
    $request = Request::create(
      'chado/generic/autocomplete/project/name/x/projectprop/1000/' . $type_id,
      'GET',
      ['q' => 'w']
    );
    $results = $generic_autocomplete->handleGenericAutocomplete($request, 'project', 'name', 'x', 'projectprop', 1000, $type_id)->getContent();
    $this->assertCount(2, json_decode($results, FALSE), 'Expected exactly 2 results from project generic autocomplete for Genome Project containing "w"');

    /**
     * Tests the project autocomplete which is derived from the generic controller
     */

    // Any project regardless of type.
    $request = Request::create(
      'chado/project/autocomplete/10/0',
      'GET',
      ['q' => 'project']
    );

    // Seven projects begin with the word 'Project'.
    // Request will only return these projects (7 rows) so suggest 1 - 7:
    // Error on 0 count.
    $suggest_count = range(0, 7);
    foreach($suggest_count as $count) {
      $suggest = $project_autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();
      if ($count > 0) {

        $this->assertEquals(count(json_decode($suggest, FALSE)), $count, 'Number of suggestions does not match requested count limit when not specifying projectprop');

        // Each suggestion matches the projects that were inserted.
        foreach(json_decode($suggest, FALSE) as $item) {
          $is_found = (in_array($item->value, array_keys($this->projects))) ? TRUE : FALSE;
          $this->assertTrue($is_found, "Did not return the $count-th project");
        }
      }
      else {
        $this->assertEquals($suggest, '[]', 'Passing count of zero did not return an empty array');
      }
    }

    // Restrict to project with projectprop.type_id set to null (id: 1).
    // Will return - 'Project Good', 'Project Winner', 'Project Great'.
    $request = Request::create(
      'chado/project/autocomplete/10/1',
      'GET',
      ['q' => 'project']
    );

    $suggest = $project_autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();

    // Test Limit/count while specifying projectprop.
    // Request will return 3 projects but suggest 1 - 3:
    $suggest_count = range(1, 3);
    foreach($suggest_count as $count) {
      $suggest = $project_autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();

      $this->assertEquals(count(json_decode($suggest, FALSE)), $count, 'Number of suggestions does not match requested count limit when projectprop specified');

      // Each suggestion matches the projects that were inserted.
      foreach(json_decode($suggest, FALSE) as $item) {
        $is_found = (in_array($item->value, array_keys($this->projects))) ? TRUE : FALSE;
        $this->assertTrue($is_found, "Returned a project not in the list of projects");
      }
    }

    // Test getProjectName().
    foreach($this->project_ids as $name => $id) {
      $retrieved_name = ChadoProjectAutocompleteController::getProjectName($id);
      $this->assertEquals($name, $retrieved_name, 'Did not retrieve the expected project name from id ' . $id);
    }

    // Test getProjectId().
    foreach($this->project_ids as $name => $id) {
      $retrieved_id = ChadoProjectAutocompleteController::getProjectId($name);
      $this->assertEquals($id, $retrieved_id, 'Did not retrieve the expected project ID from name ' . $name);
    }

    // Not found.
    $not_found = ChadoProjectAutocompleteController::getProjectName(65536);
    $this->assertEquals($not_found, '', 'Returned a project name for a non-existent ID');

    // Not found.
    $not_found = ChadoProjectAutocompleteController::getProjectId('Project Not Found');
    $this->assertEquals($not_found, 0, 'Returned a project ID for a non-existent name');

    // Test partial keyword by prefixing with wildcard '%'
    // Will return Project Great and Project Green.
    $request = Request::create(
      'chado/project/autocomplete/10/0',
      'GET',
      ['q' => '%gre']
    );

    $suggest = $project_autocomplete->handleAutocomplete($request, 5, 0)
      ->getContent();

    foreach(json_decode($suggest, FALSE) as $item) {
      $is_found = (in_array($item->value, ['Project Great', 'Project Green'])) ? TRUE : FALSE;
      $this->assertTrue($is_found, '"%gre" matched to "'.$item->value.'" not to "Project Great" or "Project Green"');
    }

    /**
     * Tests the organism autocomplete which uses multiple table columns
     */

    // Any organism containing a 't', all three do have a 't' somewhere
    $request = Request::create(
      'chado/organism/autocomplete/10',
      'GET',
      ['q' => 't']
    );
    $suggest = $organism_autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();
    $this->assertEquals(3, count(json_decode($suggest, FALSE)), 'Number of suggestions incorrect for organisms "t" autocomplete');
    foreach(json_decode($suggest, FALSE) as $item) {
      $organism_id = ChadoGenericAutocompleteController::getPkeyId($item->value);
      $this->assertIsInt($organism_id, 'getPkeyId did not return an integer');
      $this->assertArrayHasKey($organism_id, $this->organisms, 'Invalid organism_id returned');
    }

    // Any organism containing a 'k', only one does
    $request = Request::create(
      'chado/organism/autocomplete/10',
      'GET',
      ['q' => 'k']
    );
    $suggest = $organism_autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();
    $this->assertEquals(1, count(json_decode($suggest, FALSE)), 'Number of suggestions incorrect for organisms "k" autocomplete');
  }
}
