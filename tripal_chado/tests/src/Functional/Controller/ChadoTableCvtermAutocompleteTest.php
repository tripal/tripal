<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Controller\ChadoCVTermAutocompleteController;
use Symfony\Component\HttpFoundation\Request;

/**
 * Test autocomplete cvterm name.
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
    // Prepare a Request:$request entry. Search for null term
    // and suggest at least 5 items.
    $request = Request::create(
      'chado/cvterm/autocomplete/5',
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

    $this->assertTrue($null_found > 0);
    

    // Test Get Id.
    // Each item in the result for term null should have
    // an integer value which is the cvterm id number.
    foreach(json_decode($suggest) as $item) {
      // ChadoCVTermAutocompleteController::getCVtermId()
      $id = $autocomplete->getCVtermId($item->value);

      $this->assertNotNull($id);
      $this->assertIsInt($id);
    }


    // Test limit.
    $request = Request::create(
      'chado/cvterm/autocomplete/5',
      'GET',
      ['q' => 'ul']
    );
    // This will return > 6 terms ie. n[ul]l, vocab[ul]ary, pop[ul]ation, form[ul]a etc.
    // but should only suggest exactly 6 items.

    $suggest = $autocomplete->handleAutocomplete($request, 6)
      ->getContent();
    
    $this->assertEquals(count(json_decode($suggest)), 6);


    // Test exact term and 1 suggestion (exact match).
    $query = $connection->select('1:cvterm', 'c');
    $query->condition('c.name', 'null', '=');
    $query->fields('c', ['cvterm_id']);
    $null_cvterm_id = $query->execute()->fetchField();
    
    $request = Request::create(
      'chado/cvterm/autocomplete/5',
      'GET',
      ['q' => 'null']
    );  

    $suggest = $autocomplete->handleAutocomplete($request, 1)
      ->getContent();
    
    foreach(json_decode($suggest) as $item) {
      // ChadoCVTermAutocompleteController::getCVtermId()
      $id = $autocomplete->getCVtermId($item->value);

      $this->assertNotNull($id);
      $this->assertIsInt($id); 
      $this->assertEquals($id, $null_cvterm_id);
    }


    // Test invalid values as id passed to GetId.
    // Not found
    $not_ids = [0, 'lorem.ipsum', 'null', '@$#%', 'null (abc:xyz)', ' ', '.'];
    foreach($not_ids as $i) {
      $id = $autocomplete->getCVtermId($i);
      $this->assertEquals($id, 0);
    }
  }
}
