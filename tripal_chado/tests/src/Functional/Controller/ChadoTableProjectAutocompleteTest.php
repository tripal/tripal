<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Controller\ChadoProjectAutocompleteController;
use Symfony\Component\HttpFoundation\Request;

/**
 * Test autocomplete project name.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Autocomplete
 */
class ChadoTableProjectAutocompleteTest extends ChadoTestBrowserBase {
  /**
   * Registered user with access content privileges.
   *
   * @var \Drupal\user\Entity\User
   */
  private $registered_user;


  /**
   * Test autocomplete project name.
   */
  public function testAutocompleteProject() {
    // Setup registered user.
    $this->registered_user = $this->drupalCreateUser(
      ['access content'],
    );

    $this->drupalLogin($this->registered_user);

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Create a new test schema for us to use.
    $connection = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    $projects = [
      'Project Okay',
      'Project Good',
      'Project Best',
      'Project Winner',
      'Project Super',
      'Project Great',
      'Project Green',
      'Awesome Project',
      'Wow Project',
      'Yes Project',
    ];

    // Create test project records and projects with type_id set.
    $insert_project = "
      INSERT INTO {1:project} (name, description)
      VALUES
        ('$projects[0]', '$projects[0] description'),
        ('$projects[1]', '$projects[1] description'),
        ('$projects[2]', '$projects[2] description'),
        ('$projects[3]', '$projects[3] description'),
        ('$projects[4]', '$projects[4] description'),
        ('$projects[5]', '$projects[5] description'),
        ('$projects[6]', '$projects[6] description'),
        ('$projects[7]', '$projects[7] description'),
        ('$projects[8]', '$projects[8] description'),
        ('$projects[9]', '$projects[9] description')
    ";
    $connection->query($insert_project);

    // Set type_id of Null on projects: Good, Winner and Great.
    $ids = $connection->query("
      SELECT project_id FROM {1:project} WHERE name
      IN ('Project Good', 'Project Winner', 'Project Great')
    ")->fetchAllKeyed(0, 0);

    $ids = array_values($ids);

    $insert_type = "
      INSERT INTO {1:projectprop} (project_id, type_id)
      VALUES
        ($ids[0], 1),
        ($ids[1], 1),
        ($ids[2], 1)
    ";
    $connection->query($insert_type);

    $m = $connection->query("select * from {1:projectprop}")
      ->fetchAllKeyed(0, 1);

    $autocomplete = new ChadoProjectAutocompleteController();
    $this->assertNotNull($autocomplete, 'Failed to create the ChadoProjectAutocompleteController');

    // Any project regardless of type.
    $request = Request::create(
      'chado/project/autocomplete/10/0',
      'GET',
      ['q' => 'project']
    );

    // Test Limit/count.
    // Seven projects begin with the word 'Project'.
    // Request will only return these projects (7 rows) so suggest 1 - 7:
    // Error on 0 count.
    $suggest_count = range(0, 7);
    foreach($suggest_count as $count) {
      $suggest = $autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();
      if ($count > 0) {

        $this->assertEquals(count(json_decode($suggest)), $count, 'Number of suggestions does not match requested count limit when not specifying projectprop');

        // Each suggestion matches the projects that were inserted.
        foreach(json_decode($suggest) as $item) {
          $is_found = (in_array($item->value, $projects)) ? TRUE : FALSE;
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

    $suggest = $autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();

    // Test Limit/count while specifying projectprop.
    // Request will return 3 projects but suggest 1 - 3:
    $suggest_count = range(1, 3);
    foreach($suggest_count as $count) {
      $suggest = $autocomplete->handleAutocomplete($request, $count, 0)
        ->getContent();

      $this->assertEquals(count(json_decode($suggest)), $count, 'Number of suggestions does not match requested count limit when projectprop specified');

      // Each suggestion matches the projects that were inserted.
      foreach(json_decode($suggest) as $item) {
        $is_found = (in_array($item->value, $projects)) ? TRUE : FALSE;
        $this->assertTrue($is_found, "Returned a project not in the list of projects");
      }
    }

    // Test getProjectName().
    // Ids of project with type_id set. see insert query above.
    $id_name = [];
    foreach($ids as $project) {
      $name = ChadoProjectAutocompleteController::getProjectName($project);
      $id_name[] = $name;
      $match = (in_array($name, ['Project Good', 'Project Winner', 'Project Great'])) ? TRUE : FALSE;
      $this->assertTrue($match, 'Did not match to "Project Good", "Project Winner", or "Project Great"');
    }

    // Test getProjectId().
    // Ids of project with type_id set. see insert query above.
    foreach($id_name as $project) {
      $id = ChadoProjectAutocompleteController::getProjectId($project);
      $match = (in_array($id, $ids)) ? TRUE : FALSE;
      $this->assertTrue($match, 'Returned an ID not in set of valid IDs');
    }

    // Not found.
    $not_found = ChadoProjectAutocompleteController::getProjectName(11111111111111111);
    $this->assertEquals($not_found, '', 'Returned a project name for a non-existent name');

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

    $suggest = $autocomplete->handleAutocomplete($request, 5, 0)
      ->getContent();

    foreach(json_decode($suggest) as $item) {
      $is_found = (in_array($item->value, ['Project Great', 'Project Green'])) ? TRUE : FALSE;
      $this->assertTrue($is_found, '"%gre" matched to "'.$item->value.'" not to "Project Great" or "Project Green"');
    }
  }
}
