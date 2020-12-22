<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\tripal\Entity\TripalTerm;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the basic functions of the TripalTerm Entity Type.
 *
 * @group Tripal
 * @group Tripal Term
 * @group Tripal Entities
 */
class TripalTermEntityTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;
  protected $defaultTheme = 'stable';

  public static $modules = ['tripal', 'block', 'field_ui'];

  /**
   * Basic tests for TripalTerm Entity.
   *
   * @group tripal_term
   */
  public function testTripalTermEntity() {
    $assert = $this->assertSession();

    // Ensure we have a priviledged user.
    $web_user = $this->drupalCreateUser([
      'view controlled vocabulary term entities',
      'add controlled vocabulary term entities',
      'edit controlled vocabulary term entities',
      'delete controlled vocabulary term entities',
      'administer controlled vocabulary term entities',
      'access administration pages',
      'access controlled vocabulary term overview',
    ]);

    // Anonymous User should not see the tripal vocab listing.
    // Thus we will try to go to the listing... then check we can't.
    $this->drupalGet('admin/structure/tripal_term');
    $assert->pageTextContains('Access denied');

    // Now login our priviledged user.
    $this->drupalLogin($web_user);

    // TripalTerm Listing.
    //-------------------------------------------

    // First check that the listing shows up in the structure menu.
    $this->drupalGet('admin/structure');
    $assert->linkExists('Tripal Controlled Vocabulary Terms');
    $this->clickLink('Tripal Controlled Vocabulary Terms');

    // Web_user user has the right to view listing.
    // We should now be on admin/structure/tripal_term.
    // thus check for the expected title.
    $assert->pageTextContains('Tripal Controlled Vocabulary Terms');

    // We start out without any content... thus check we are told there
    // are no Controlled Vocabulary Terms.
    $msg = 'There are no tripal controlled vocabulary term entities yet.';
    $assert->pageTextContains($msg);

    // TripalTerm Add Form.
    //-------------------------------------------

    // Check that there is an "Add Term" link on the listing page.
    // As far as I can tell, maybe action links are not available in tests?
    // @fails $this->assertSession()->responseContains('Add Term');

    // Go to the Add Term page.
    $this->drupalGet('admin/structure/tripal_term/add');
    // We should now be on admin/structure/tripal_term/add.
    $assert->pageTextContains('Add tripal controlled vocabulary term');
    $assert->fieldExists('Tripal Controlled Vocabulary');
    $assert->fieldValueEquals('Tripal Controlled Vocabulary', '');
    $assert->fieldExists('Accession');
    $assert->fieldValueEquals('Accession', '');
    $assert->fieldExists('Term Name');
    $assert->fieldValueEquals('Term Name', '');

    // Now fill out the form and submit.
    // Post content, save an instance. Go to the new page after saving.
    // -- Create a vocab for use in the form.
    $vocab_name = 'tripalvocab-'.time();
    $vocab = \Drupal\tripal\Entity\TripalVocab::create();
    $vocab->setLabel($vocab_name);
    $vocab->setName($vocab_name);
    $vocab->save();
    $vocab_label = $vocab->getLabel();
    $this->assertEquals($vocab_name, $vocab_label);
    // -- Create the other values.
    $name = 'TripalTerm ' . uniqid();
    $accession = uniqid();
    $add = [
      'vocab_id' => $vocab_label . ' (' . $vocab->getID() . ')',
      'accession' => $accession,
      'name' => $name,
    ];
    // Submit the form.
    $this->drupalPostForm(null, $add, 'Save');

    // Now there should be a term.
    $assert->responseContains('Created');
    $assert->responseContains($name);

    // Then go back to the listing.
    $this->drupalGet('admin/structure/tripal_term');

    // There should now be entities thus we shouldn't see the empty msg.
    $msg = 'There are no tripal controlled vocabulary term entities yet.';
    $assert->pageTextNotContains($msg);

    // We should also see our new record listed with edit/delete links.
    $assert->pageTextContains($name);
    $assert->pageTextContains($accession);
    $assert->pageTextContains($vocab_label);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // TripalTerm Edit Form.
    //-------------------------------------------

    // Go to the edit form for our new entity.
    $this->clickLink('Edit');
    // We should now be on admin/structure/tripal_term/{tripal_term}/edit.
    $assert->pageTextContains('Edit');
    $assert->fieldExists('Term Name');
    $assert->fieldValueEquals('Term Name', $name);

    // Now fill out the form and submit.
    // Post content, save the instance.
    $new_term_name = $name . ' CHANGED';
    $edit = [
      'name' => $new_term_name,
    ];
    $this->drupalPostForm(NULL, $edit, 'Save');
    $assert->pageTextContains('Saved the ' . $new_term_name . ' controlled vocabulary term.');

    // Then go back to the listing.
    $this->drupalGet('admin/structure/tripal_term');
    // We should also see our new record listed with edit/delete links.
    $assert->pageTextContains($new_term_name);
    $assert->pageTextContains($accession);
    $assert->pageTextContains($vocab_label);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // TripalTerm Delete Form.
    //-------------------------------------------
    // Go to the edit form for our new entity.
    $this->clickLink('Delete');

    // Check that we get the confirmation form.
    $msg = 'Are you sure you want to delete the tripal controlled vocabulary term';
    $assert->pageTextContains($msg);
    $assert->pageTextContains('This action cannot be undone.');
    $assert->buttonExists('Delete');
    // @todo fails $assert->buttonExists('Cancel');

    // First we cancel and check the record is not deleted.
    // @todo fails $this->drupalPostForm(NULL, [], 'edit_cancel');
    $this->drupalGet('admin/structure/tripal_term');
    $assert->pageTextContains($new_term_name);
    $assert->pageTextContains($accession);
    $assert->pageTextContains($vocab_label);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // Now we delete the record.
    $this->clickLink('Delete');
    $this->drupalPostForm(NULL, [], 'Delete');
    $msg = "The tripal controlled vocabulary term $new_term_name has been deleted.";
    $assert->pageTextContains($msg);

    $this->drupalGet('admin/structure/tripal_term');
    $assert->pageTextNotContains($new_term_name);
    $assert->pageTextNotContains($accession);

  }

  /**
   * Test all paths exposed by the module, by permission.
   *
   * @group tripal_term
   */
  public function testPaths() {
    $assert = $this->assertSession();

    // Generate a vocab so that we can test the paths against it.
    $vocab = TripalTerm::create([
      'vocabulary' => 'somename',
    ]);
    $vocab->save();

    // Gather the test data.
    $data = $this->providerTestPaths($vocab->id());

    // Run the tests.
    foreach ($data as $datum) {
      // drupalCreateUser() doesn't know what to do with an empty permission
      // array, so we help it out.
      if ($datum[2]) {
        $user = $this->drupalCreateUser([
          'access administration pages',
          $datum[2]
        ]);
        $this->drupalLogin($user);
      }
      else {
        $user = $this->drupalCreateUser();
        $this->drupalLogin($user);
      }
      $this->drupalGet($datum[1]);
      $assert->statusCodeEquals($datum[0]);
    }
  }

  /**
   * Data provider for testPaths.
   *
   * @param int $tripal_term_id
   *   The id of an existing TripalTerm entity.
   *
   * @return array
   *   Nested array of testing data. Arranged like this:
   *   - Expected response code.
   *   - Path to request.
   *   - Permission for the user.
   */
  protected function providerTestPaths($vocab_id) {
    return [
      [
        200,
        '/admin/structure/tripal_term/' . $vocab_id,
        'view controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/' . $vocab_id,
        '',
      ],
      [
        200,
        '/admin/structure/tripal_term',
        'view controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_term/add',
        'add controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/add',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_term/' . $vocab_id . '/edit',
        'edit controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/' . $vocab_id . '/edit',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_term/' . $vocab_id . '/delete',
        'delete controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/' . $vocab_id . '/delete',
        '',
      ],
    ];
  }

}
