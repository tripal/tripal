<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;

/**
 * Tests the basic functions of the TripalVocab Entity Type.
 *
 * @group Tripal
 * @group Tripal Vocab
 * @group Tripal Entities
 */
class TripalVocabEntityTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;
  protected $defaultTheme = 'stable';

  protected static $modules = ['tripal', 'block', 'field_ui'];

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
    $this->drupalGet('admin/structure/tripal-vocabularies/vocab');
    $assert->pageTextContains('Access denied');

    $this->drupalLogin($web_user);

    // TripalVocab Listing.
    //-------------------------------------------

    // First check that the listing shows up in the structure menu.
    $this->drupalGet('admin/structure');
    $assert->linkExists('Tripal Vocabularies');
    $this->clickLink('Tripal Vocabularies');

    // Web_user user has the right to view listing.
    // We should now be on admin/structure/tripal-vocabularies/vocab.
    // thus check for the expected title.
    $assert->pageTextContains('Tripal Vocabularies');

    // We start out without any content... thus check we are told there
    // are no controlled vocabularies.
    $msg = 'There are no tripal vocabulary entities yet.';
    $assert->pageTextContains($msg);

    // TripalVocab Add Form.
    //-------------------------------------------

    // Check that there is an "Add Vocabulary" link on the listing page.
    // @todo fails $assert->linkExists('Add Vocabulary');

    // Go to the Add Vocabulary page.
    // @todo fails $this->clickLink('Add Vocabulary');
    $this->drupalGet('admin/structure/tripal-vocabularies/vocab/add');
    // We should now be on admin/structure/tripal-vocabularies/vocab/add.
    $assert->pageTextContains('Add tripal vocabulary');
    $assert->fieldExists('Full Name');
    $assert->fieldValueEquals('Full Name', '');
    $assert->fieldExists('Namespace');
    $assert->fieldValueEquals('Namespace', '');
    $assert->fieldExists('Description');
    $assert->fieldValueEquals('Description', '');

    // Now fill out the form and submit.
    // Post content, save an instance. Go to the new page after saving.
    $vocab_name = 'test ' . date('Ymd');
    $add = [
      'name' => $vocab_name,
      'namespace' => uniqid(),
    ];
    $this->submitForm($add, 'Save');
    $assert->pageTextContains('Created the ' . $vocab_name . ' Tripal Vocabulary.');

    // Then go back to the listing.
    $this->drupalGet('admin/structure/tripal-vocabularies/vocab');

    // There should now be entities thus we shouldn't see the empty msg.
    $msg = 'There are no tripal vocabulary entities yet.';
    $assert->pageTextNotContains($msg);

    // We should also see our new record listed with edit/delete links.
    $assert->pageTextContains($vocab_name);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // TripalVocab Edit Form.
    //-------------------------------------------

    // Go to the edit form for our new entity.
    $this->clickLink('Edit');
    // We should now be on admin/structure/tripal-vocabularies/vocab/{tripal_vocab}/edit.
    $assert->pageTextContains('Edit');
    $assert->fieldExists('Name');
    $assert->fieldValueEquals('Name', $add['name']);
    $assert->fieldExists('Namespace');
    $assert->fieldValueEquals('Namespace', $add['namespace']);

    // Now fill out the form and submit.
    // Post content, save the instance.
    $new_vocab_name = 'CHANGED' . uniqid();
    $edit = $add;
    $edit['name'] = $new_vocab_name;
    $this->submitForm($edit, 'Save');
    $assert->pageTextContains('Saved the ' . $new_vocab_name . ' Tripal Vocabulary.');

    // Then go back to the listing.
    $this->drupalGet('admin/structure/tripal-vocabularies/vocab');
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
    $assert->pageTextContains('This action cannot be undone.');
    $assert->buttonExists('Delete');
    // @todo fails $assert->buttonExists('Cancel');

    // First we cancel and check the record is not deleted.
    // @todo fails $this->drupalPostForm(NULL, [], 'edit_cancel');
    $this->drupalGet('admin/structure/tripal-vocabularies/vocab');
    $assert->pageTextContains($new_vocab_name);
    $assert->linkExists('Edit');
    $assert->linkExists('Delete');

    // Now we delete the record.
    $this->clickLink('Delete');
    $this->submitForm([], 'Delete');
    $msg = 'The tripal vocabulary ' . $new_vocab_name . ' has been deleted.';
    $assert->pageTextContains($msg);

    // Ensure the vocab is no longer in the listing.
    $this->drupalGet('admin/structure/tripal-vocabularies/vocab');
    $assert->pageTextNotContains($new_vocab_name);
  }

  /**
   * Basic Tests for the Tripal Vocabulary API.
   *
   * @group tripal_vocab
   */
  public function testTripalVocabProceduralAPI() {

    $test_details = [
      'name' => 'Full name with unique bit ' . uniqid(),
      'idspace' => 'SO' . uniqid(),
      'description' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
    ];

    $success = tripal_add_vocabulary($test_details);
    $this->assertTrue($success, "tripal_add_vocabulary() returns TRUE if the vocabulary was created successfully.");

    $retrieved_details = tripal_get_vocabulary_details($test_details['name']);
    $this->assertContains($test_details['name'], $retrieved_details,
      "We should be able to retrieve what we created using the name.");

    $retrieved_details = tripal_get_vocabulary_details($test_details['idspace']);
    $this->assertContains($test_details['name'], $retrieved_details,
      "We should be able to retrieve what we created using the short name.");
  }

  /**
   * Basic Tests for the Tripal Vocabulary API.
   *
   * @group tripal_vocab
   */
  public function testTripalVocabManager() {

    $test_details = [
      'name' => 'Full name with unique bit ' . uniqid(),
      'idspace' => 'SO' . uniqid(),
      'description' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
    ];

    $manager = \Drupal::service('tripal.tripalVocab.manager');

    $success = $manager->addVocabulary($test_details);
    $this->assertTrue($success, "Unable to add Vocabulary using manager.");

    $exists = $manager->checkVocabExists($test_details);
    $this->assertEquals(1, $exists, "The vocabulary should exist because we just created it.");

    $vocab = $manager->getVocabularies($test_details);
    $this->assertIsObject($vocab, "We should have been able to retrieve the object.");

    $vocab_id = $vocab->id();
    $vocab2 = $manager->loadVocabularies([$vocab_id]);
    $this->assertEquals($vocab, $vocab2, "We should be able to retrieve the same object using getVocabularies and loadVocabularies.");

    // Now we can try updating them.
    $test_details['description'] = 'NEW SHORTER DESCRIPTION ' . uniqid();
    $success = $manager->updateVocabulary($test_details);
    $this->assertTrue($success, "We should have been able to update it.");
    $vocab3 = $manager->getVocabularies($test_details);
    $this->assertIsObject($vocab3, "We should have been able to retrieve the updated object.");
    $this->assertNotEquals($vocab, $vocab3, "The updated object should be different.");
    $vocab3_id = $vocab3->id();
    $this->assertEquals($vocab_id, $vocab3_id, "But the ID should be the same.");

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
        '/admin/structure/tripal-vocabularies/vocab/' . $vocab_id,
        'view controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal-vocabularies/vocab/' . $vocab_id,
        '',
      ],
      [
        200,
        '/admin/structure/tripal-vocabularies/vocab',
        'view controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal-vocabularies/vocab',
        '',
      ],
      [
        200,
        '/admin/structure/tripal-vocabularies/vocab/add',
        'add controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal-vocabularies/vocab/add',
        '',
      ],
      [
        200,
        '/admin/structure/tripal-vocabularies/vocab/' . $vocab_id . '/edit',
        'edit controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal-vocabularies/vocab/' . $vocab_id . '/edit',
        '',
      ],
      [
        200,
        '/admin/structure/tripal-vocabularies/vocab/' . $vocab_id . '/delete',
        'delete controlled vocabulary entities',
      ],
      [
        403,
        '/admin/structure/tripal-vocabularies/vocab/' . $vocab_id . '/delete',
        '',
      ],
    ];
  }

}
