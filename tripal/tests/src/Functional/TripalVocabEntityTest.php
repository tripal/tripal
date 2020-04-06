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
  protected $defaultTheme = 'stable';

  public static $modules = ['tripal', 'block', 'field_ui'];

  /**
   * Basic tests for Tripal Vocab Entity.
   *
   * @group tripal_vocab
   */
  public function testTripalVocabEntity() {
    $assert = $this->assertSession();

    $web_user = $this->drupalCreateUser([
      'view controlled vocabulary entities',
      'add controlled vocabulary entities',
      'edit controlled vocabulary entities',
      'delete controlled vocabulary entities',
      'administer controlled vocabulary entities',
      'access administration pages',
      'access controlled vocabulary overview',
    ]);

    // Anonymous User should not see the tripal vocab listing.
    // Thus we will try to go to the listing... then check we can't.
    $this->drupalGet('admin/structure/tripal_vocab');
    $assert->pageTextContains('Access denied');

    $this->drupalLogin($web_user);

    // TripalVocab Listing.
    //-------------------------------------------

    // First check that the listing shows up in the structure menu.
    $this->drupalGet('admin/structure');
    $assert->linkExists('Tripal Controlled Vocabularies');
    $this->clickLink('Tripal Controlled Vocabularies');

    // Web_user user has the right to view listing.
    // We should now be on admin/structure/tripal_vocab.
    // thus check for the expected title.
    $assert->pageTextContains('Tripal Controlled Vocabularies');

    // We start out without any content... thus check we are told there
    // are no controlled vocabularies.
    $msg = 'There are no tripal controlled vocabulary entities yet.';
    $assert->pageTextContains($msg);

    // TripalVocab Add Form.
    //-------------------------------------------

    // Check that there is an "Add Vocabulary" link on the listing page.
    // @todo fails $assert->linkExists('Add Vocabulary');

    // Go to the Add Vocabulary page.
    // @todo fails $this->clickLink('Add Vocabulary');
    $this->drupalGet('admin/structure/tripal_vocab/add');
    // We should now be on admin/structure/tripal_vocab/add.
    $assert->pageTextContains('Add tripal controlled vocabulary');
    $assert->fieldExists('Short Name');
    $assert->fieldValueEquals('Short Name', '');
    $assert->fieldExists('Full Name');
    $assert->fieldValueEquals('Full Name', '');
    $assert->fieldExists('Description');
    $assert->fieldValueEquals('Description', '');

    // Now fill out the form and submit.
    // Post content, save an instance. Go to the new page after saving.
    $vocab_name = 'test ' . date('Ymd');
    $add = [
      'vocabulary' => $vocab_name,
    ];
    $this->drupalPostForm(NULL, $add, 'Save');
    $assert->pageTextContains('Created the ' . $vocab_name . ' Controlled Vocabulary.');

    // Then go back to the listing.
    $this->drupalGet('admin/structure/tripal_vocab');

    // There should now be entities thus we shouldn't see the empty msg.
    $msg = 'There are no tripal controlled vocabulary entities yet.';
    $assert->pageTextNotContains($msg);

    // We should also see our new record listed with edit/delete links.
    $assert->pageTextContains($vocab_name);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // TripalVocab Edit Form.
    //-------------------------------------------

    // Go to the edit form for our new entity.
    $this->clickLink('Edit');
    // We should now be on admin/structure/tripal_vocab/{tripal_vocab}/edit.
    $assert->pageTextContains('Edit');
    $assert->fieldExists('Short Name');
    $assert->fieldValueEquals('Short Name', $vocab_name);

    // Now fill out the form and submit.
    // Post content, save the instance.
    $new_vocab_name = 'CHANGED' . uniqid();
    $edit = [
      'vocabulary' => $new_vocab_name,
    ];
    $this->drupalPostForm(NULL, $edit, 'Save');
    $assert->pageTextContains('Saved the ' . $new_vocab_name . ' Controlled Vocabulary.');

    // Then go back to the listing.
    $this->drupalGet('admin/structure/tripal_vocab');
    // We should also see our new record listed with edit/delete links.
    $assert->pageTextNotContains($vocab_name);
    $assert->pageTextContains($new_vocab_name);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // TripalVocab Delete Form.
    //-------------------------------------------
    // Go to the edit form for our new entity.
    $this->clickLink('Delete');

    // Check that we get the confirmation form.
    $msg = 'Are you sure you want to delete the tripal controlled vocabulary';
    $assert->pageTextContains($msg);
    $assert->pageTextContains('This action cannot be undone.');
    $assert->buttonExists('Delete');
    // @todo fails $assert->buttonExists('Cancel');

    // First we cancel and check the record is not deleted.
    // @todo fails $this->drupalPostForm(NULL, [], 'edit_cancel');
    $this->drupalGet('admin/structure/tripal_vocab');
    $assert->pageTextContains($new_vocab_name);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // Now we delete the record.
    $this->clickLink('Delete');
    $this->drupalPostForm(NULL, [], 'Delete');
    $msg = 'The tripal controlled vocabulary ' . $new_vocab_name . ' has been deleted.';
    $assert->pageTextContains($msg);

    // Ensure the vocab is no longer in the listing.
    $this->drupalGet('admin/structure/tripal_vocab');
    $assert->pageTextNotContains($new_vocab_name);
  }

  /**
   * Test all paths exposed by the module, by permission.
   *
   * @group tripal_vocab
   */
  public function testPaths() {
    $assert = $this->assertSession();

    // Generate a vocab so that we can test the paths against it.
    $vocab = TripalVocab::create([
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
   * @param int $tripal_vocab_id
   *   The id of an existing TripalVocab entity.
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
        '/admin/structure/tripal_vocab/' . $vocab_id,
        'view controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal_vocab/' . $vocab_id,
        '',
      ],
      [
        200,
        '/admin/structure/tripal_vocab',
        'view controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal_vocab',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_vocab/add',
        'add controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal_vocab/add',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_vocab/' . $vocab_id . '/edit',
        'edit controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal_vocab/' . $vocab_id . '/edit',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_vocab/' . $vocab_id . '/delete',
        'delete controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal_vocab/' . $vocab_id . '/delete',
        '',
      ],
    ];
  }

}
