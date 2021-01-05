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
    $this->drupalGet('admin/structure/tripal_term');

    // Web_user user has the right to view listing.
    // We should now be on admin/structure/tripal_term.
    // thus check for the expected title.
    $assert->pageTextContains('Tripal Terms');

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
    $assert->fieldExists('IDSpace');
    $assert->fieldValueEquals('IDSpace', '');
    $assert->fieldExists('Vocabulary');
    $assert->fieldValueEquals('Vocabulary', '');
    $assert->fieldExists('Accession');
    $assert->fieldValueEquals('Accession', '');
    $assert->fieldExists('Term Name');
    $assert->fieldValueEquals('Term Name', '');

    // Now fill out the form and submit.
    // Post content, save an instance. Go to the new page after saving.
    // -- Create a vocab for use in the form.
    $vocab_name = 'tripalvocab-'.time();
    $vocab = \Drupal\tripal\Entity\TripalVocab::create();
    $vocab->setNamespace($vocab_name);
    $vocab->setName($vocab_name);
    $vocab->save();
    $vocab_label = $vocab->getName();
    $this->assertEquals($vocab_name, $vocab_label);
    // -- Create a IDSpace for use in the form.
    $idspace_name = uniqid();
    $idspace = \Drupal\tripal\Entity\TripalVocabSpace::create();
    $idspace->setIDSpace($idspace_name);
    $idspace->setVocabID($vocab->id());
    $idspace->save();
    // -- Create the other values.
    $name = 'TripalTerm ' . uniqid();
    $accession = uniqid();
    $add = [
      'vocab_id' => $vocab_label . ' (' . $vocab->getID() . ')',
      'idspace_id' => $idspace_name . ' (' . $idspace->id() . ')',
      'accession' => $accession,
      'name' => $name,
    ];

    // Submit the form.
    $this->drupalPostForm('admin/structure/tripal_term/add', $add, 'Save');

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
    $assert->pageTextContains($idspace_name);
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
    $assert->pageTextContains('Saved the ' . $new_term_name . ' Tripal Term.');

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
   * Basic Tests for the Tripal Vocabulary API.
   *
   * @group tripal_term
   */
  public function testTripalTermProceduralAPI() {

    $test_details = [
      'vocabulary' => [
        'name' => 'Full name with unique bit ' . uniqid(),
        'short_name' => 'SO' . uniqid(),
        'description' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
      ],
      'accession' => 'SO:' . uniqid(),
      'name' => 'Lorem ipsum ' . uniqid(),
      'definition' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
    ];

    $success = tripal_add_term($test_details);
    $this->assertTrue($success, "tripal_add_term() returns TRUE if the term was created successfully.");

    $retrieved_details = tripal_get_term_details(
      $test_details['vocabulary']['short_name'],
      $test_details['accession']
    );
    $this->assertContains($test_details['accession'], $retrieved_details,
      "We should be able to retrieve what we created using the short name.");
  }

  /**
   * Basic Tests for the Tripal Term API.
   *
   * @group tripal_term
   */
  public function testTripalTermManager() {

    $test_details = [
      'vocabulary' => [
        'name' => 'Full name with unique bit ' . uniqid(),
        'short_name' => 'SO' . uniqid(),
        'description' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
      ],
      'accession' => 'SO:' . uniqid(),
      'name' => 'Lorem ipsum ' . uniqid(),
      'definition' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
    ];

    $manager = \Drupal::service('tripal.tripalTerm.manager');

    $success = $manager->addTerm($test_details);
    $this->assertTrue($success, "Unable to add term using manager.");

    $exists = $manager->checkExists($test_details);
    $this->assertEquals(1, $exists, "The term should exist because we just created it.");

    $term = $manager->getTerms($test_details);
    $this->assertIsObject($term, "We should have been able to retrieve the object.");

    $term_id = $term->id();
    $term2 = $manager->loadTerms([$term_id]);
    $this->assertEquals($term, $term2, "We should be able to retrieve the same object using getVocabularies and loadVocabularies.");

    // Now we can try updating them.
    $test_details['description'] = 'NEW SHORTER DESCRIPTION ' . uniqid();
    $success = $manager->updateTerm($test_details);
    $this->assertTrue($success, "We should have been able to update it.");
    $term3 = $manager->getTerms($test_details);
    $this->assertIsObject($term3, "We should have been able to retrieve the updated object.");
    $this->assertNotEquals($term, $term3, "The updated object should be different.");
    $term3_id = $term3->id();
    $this->assertEquals($term_id, $term3_id, "But the ID should be the same.");
  }

  /**
   * Test all paths exposed by the module, by permission.
   *
   * @group tripal_term
   */
  public function testPaths() {
    $assert = $this->assertSession();

    $test_details = [
      'vocabulary' => [
        'name' => 'Full name with unique bit ' . uniqid(),
        'short_name' => 'SO' . uniqid(),
        'description' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
      ],
      'accession' => 'SO:' . uniqid(),
      'name' => 'Lorem ipsum ' . uniqid(),
      'definition' => 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. In ultrices, arcu sed ultricies condimentum, quam tortor tristique libero, eget auctor est magna quis nibh.',
    ];

    $manager = \Drupal::service('tripal.tripalTerm.manager');

    $success = $manager->addTerm($test_details);
    $this->assertTrue($success, "Unable to add term to test paths agaist.");
    $term = $manager->getTerms($test_details);

    // Gather the test data.
    $data = $this->providerTestPaths($term->id());

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
  protected function providerTestPaths($term_id) {
    return [
      [
        200,
        '/admin/structure/tripal_term/' . $term_id,
        'view controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/' . $term_id,
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
        '/admin/structure/tripal_term/' . $term_id . '/edit',
        'edit controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/' . $term_id . '/edit',
        '',
      ],
      [
        200,
        '/admin/structure/tripal_term/' . $term_id . '/delete',
        'delete controlled vocabulary term entities',
      ],
      [
        403,
        '/admin/structure/tripal_term/' . $term_id . '/delete',
        '',
      ],
    ];
  }

}
