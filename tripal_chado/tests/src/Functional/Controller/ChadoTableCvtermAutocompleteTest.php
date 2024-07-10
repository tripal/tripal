<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Controller\ChadoCVTermAutocompleteController;
use Symfony\Component\HttpFoundation\Request;

/**
 * Test autocomplete cvterm name.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Autocomplete
 */
class ChadoTableCvtermAutocompleteTest extends ChadoTestBrowserBase {
  /**
   * Registered user with access content privileges.
   *
   * @var \Drupal\user\Entity\User
   */
  private $registered_user;


  /**
   * Test autocomplete cvterm name.
   */
  public function testAutocompleteCvterm() {
    // Setup registered user.
    $this->registered_user = $this->drupalCreateUser(
      ['access content'],
    );

    $this->drupalLogin($this->registered_user);

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Create a new test schema for us to use.
    $connection = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Test Handler:
    $autocomplete = new ChadoCVTermAutocompleteController();
    $this->assertNotNull($autocomplete, 'Failed to create the ChadoCVTermAutocompleteController');

    // Prepare a Request:$request entry. Search for null term
    // and suggest at least 5 items.
    $request = Request::create(
      'chado/cvterm/autocomplete/5/0',
      'GET',
      ['q' => 'null']
    );

    $suggest = $autocomplete->handleAutocomplete($request)
      ->getContent();

    // Find if null cvterm was suggested.
    $null_found = 0;
    foreach(json_decode($suggest) as $item) {
      if (str_contains($item->value, 'null')) {
        $null_found++;
      }
    }

    $this->assertTrue($null_found > 0, '"null" CV was not suggested');


    // Test Get Id.
    // Each item in the result for term null should have
    // an integer value which is the cvterm id number.
    foreach(json_decode($suggest) as $item) {
      // ChadoCVTermAutocompleteController::getCVtermId()
      $id = $autocomplete->getCVtermId($item->value);

      $this->assertNotNull($id, 'NULL returned for cvterm_id');
      $this->assertIsInt($id, 'non-integer cvterm_id returned as a suggestion');
    }


    // Test limit.
    $request = Request::create(
      'chado/cvterm/autocomplete/5/0',
      'GET',
      ['q' => 'pro']
    );
    // There are more than 6 terms starting with "pro" i.e. [pro]perty [Pro]tein sequence, [pro]moter, etc.
    // but should only suggest exactly 6 items.
    $suggest = $autocomplete->handleAutocomplete($request, 6, 0)
      ->getContent();

    $this->assertEquals(count(json_decode($suggest)), 6, 'Should have suggested 6 terms starting with "pro"');


    // Test exact term and 1 suggestion (exact match).
    $query = $connection->select('1:cvterm', 'c');
    $query->condition('c.name', 'null', '=');
    $query->fields('c', ['cvterm_id']);
    $null_cvterm_id = $query->execute()->fetchField();

    $request = Request::create(
      'chado/cvterm/autocomplete/5/0',
      'GET',
      ['q' => 'null']
    );

    $suggest = $autocomplete->handleAutocomplete($request, 1, 0)
      ->getContent();

    foreach(json_decode($suggest) as $item) {
      // ChadoCVTermAutocompleteController::getCVtermId()
      $id = $autocomplete->getCVtermId($item->value);

      $this->assertNotNull($id, 'NULL returned as a suggestion for "null"');
      $this->assertIsInt($id, 'non-integer cvterm_id returned as a suggestion');
      $this->assertEquals($id, $null_cvterm_id, 'cvterm_id suggested does not match the "null" cvterm_id');
    }


    // Compare with and without using only a specified cv_id.
    // We should receieve fewer suggestions when specifying the cv_id.
    $query = $connection->select('1:cv', 'cv');
    $query->condition('cv.name', 'sequence', '=');
    $query->fields('cv', ['cv_id']);
    $sequence_cv_id = $query->execute()->fetchField();

    $request = Request::create(
      'chado/cvterm/autocomplete/10000/0',
      'GET',
      ['q' => 'a']
    );
    $suggest = $autocomplete->handleAutocomplete($request, 10000, 0)
      ->getContent();
    $n_all = count(json_decode($suggest));
    $request = Request::create(
      'chado/cvterm/autocomplete/10000/' . $sequence_cv_id,
      'GET',
      ['q' => 'a']
    );
    $suggest = $autocomplete->handleAutocomplete($request, 10000, $sequence_cv_id)
      ->getContent();
    $n_sequence = count(json_decode($suggest));
    $this->assertGreaterThan(0, $n_all, 'Test with no CV limit returned no suggestions');
    $this->assertGreaterThan(0, $n_sequence, 'Test with CV limit returned no suggestions');
    $this->assertGreaterThan($n_sequence, $n_all, 'Limiting by CV did not reduce number of suggestions');


    // Test invalid values as id passed to GetId.
    // Not found
    $not_ids = [0, 'lorem.ipsum', 'null', '@$#%', 'null (abc:xyz)', ' ', '.'];
    foreach($not_ids as $i) {
      $id = $autocomplete->getCVtermId($i);
      $this->assertEquals($id, 0, 'Returned a cvterm_id for a non-existing CV term');
    }


    // Test format CVterm method.
    foreach(json_decode($suggest) as $item) {
      // ChadoCVTermAutocompleteController::getCVtermId()
      $id = $autocomplete->getCVtermId($item->value);
      // Reverse value - get formatted term.
      $term = $autocomplete->formatCVterm($id);

      $this->assertNotNull($term, 'NULL returned for formatted CV term');
      $this->assertIsString($term, 'Non-string value returned for formatted CV term');
      $this->assertEquals($term, $item->value, 'Returned formatted CV term does not match expected value');
    }
  }
}
