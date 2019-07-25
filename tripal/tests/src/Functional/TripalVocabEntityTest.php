<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the basic functions of the TripalVocab Entity Type.
 *
 * @ingroup tripal
 *
 * @group TripalVocab
 * @group entities
 */
class TripalVocabEntityTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;

  public static $modules = ['tripal', 'block', 'field_ui'];

  /**
   * Basic tests for Content Entity Example.
   */
  public function testTripalVocabEntity() {
    $assert = $this->assertSession();

    $web_user = $this->drupalCreateUser([
      'view controlled vocabulary entities',
      'add controlled vocabulary entities',
      'edit controlled vocabulary entities',
      'delete controlled vocabulary entities',
      'administer controlled vocabulary entities',
      'access controlled vocabulary overview',
    ]);

    // Anonymous User should not see the tripal vocab listing.
    // Thus we will try to go to the listing... then check we can't.
    $this->drupalGet('admin/structure/tripal_vocab');
    $assert->pageTextContains('Access denied');

    $this->drupalLogin($web_user);

    // TripalVocab Listing.
    //-------------------------------------------

    // Web_user user has the right to view listing.
    $this->drupalGet('admin/structure/tripal_vocab');
    $assert->pageTextContains('Tripal Controlled Vocabularies');

    // We start out without any content... thus check we are told there
    // are no controlled vocabularies.
    $msg = 'There are no controlled vocabulary entities yet.';
    $assert->pageTextContains($msg);

    // TripalVocab Add Form.
    //-------------------------------------------

    // TripalVocab Edit Form.
    //-------------------------------------------

    // TripalVocab Delete Form.
    //-------------------------------------------

  }

}
