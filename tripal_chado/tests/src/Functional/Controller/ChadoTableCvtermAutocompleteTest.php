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
    $web_assert = $this->assertSession();

    // Test autocomplete cvterm route to search null cvterm.
    // Null term (null:local:null)
    $this->drupalGet('chado/cvterm/autocomplete/5', ['query' => ['q' => 'null']]);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('null');

    // Test autocomplete cvterm route to search for empty string.
    // Null value returned
    $this->drupalGet('chado/cvterm/autocomplete/5', ['query' => ['q' => '']]);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('{}');

    // Test autocomplete cvterm route to search for partial keywords.
    // Null value returned
    $this->drupalGet('chado/cvterm/autocomplete/5', ['query' => ['q' => 'ull']]);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('null');

    // Test autocomplete cvterm route to search for terms in uppercase (case insensitive).
    // Null value returned
    $this->drupalGet('chado/cvterm/autocomplete/5', ['query' => ['q' => 'NULL']]);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('null');

    // Test autocomplete cvterm route to search for keywords < 2.
    // Null value returned
    $this->drupalGet('chado/cvterm/autocomplete/5', ['query' => ['q' => 'n']]);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('{}');

    // Test autocomplete cvterm route to search for cvterm but less than 1 count.
    // Null value returned
    $this->drupalGet('chado/cvterm/autocomplete/0', ['query' => ['q' => 'ull']]);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('{}');

    // Test autocomplete cvterm route incomplete parameters no count.
    // Page not found
    $this->drupalGet('chado/cvterm/autocomplete/');
    $web_assert->statusCodeEquals(404);

    // Test autocomplete cvterm route incomplete parameters no query string.
    // Null value returned
    $this->drupalGet('chado/cvterm/autocomplete/5', []);
    $web_assert->statusCodeEquals(200);
    $web_assert->pageTextContains('{}');

    
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
    $null_cvterm_id = \Drupal::service('tripal_chado.database')
      ->query("SELECT cvterm_id FROM {1:cvterm} WHERE name = 'null' LIMIT 1")
      ->fetchField();

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
    $not_ids = [0, 'lorem.ipsum', 'null', '@$#%', 'null (abc:xyz)', ' '];
    foreach($not_ids as $i) {
      $id = $autocomplete->getCVtermId($i);
      $this->assertEquals($id, 0);
    }
  }
}
