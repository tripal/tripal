<?php

namespace Drupal\Tests\tripal\Functional\Entity;

use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Url;

/**
 * Tests the basic functions of Tripal Content.
 *
 * @group Tripal
 * @group Tripal Content
 */
class TripalContentTest extends TripalTestBrowserBase {

  /**
   * Test the CRUD actions for Tripal Content Type and Tripal Content Entities.
   */
  public function testTripalContentCRUD() {

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~8 latin capitalized words.
    $values['label'] = $random->sentences(8,TRUE);
    // Provides a category with ~3 latin capitalized words.
    $values['category'] = $random->sentences(3,TRUE);
    // Provides a title with ~8 latin capitalized words.
    $values['help_text'] = $random->sentences(25);
    // Provides a title with ~8 latin capitalized words.
    $values['title_format'] = $random->sentences(8,TRUE);
    // Provides a category with ~3 latin capitalized words separated by '/'.
    $values['url_format'] = str_replace(' ', '/', $random->sentences(3,TRUE));

    // Create a mock term to provide to the entity.
    $term_idspace = $random->sentences(3,TRUE);
    $term_accession = $random->sentences(3,TRUE);
    $term = $this->createMock('\Drupal\tripal\TripalVocabTerms\TripalTerm');
    $term->expects($this->any())
      ->method('getIdSpace')->will($this->returnValue($term_idspace));
    $term->expects($this->any())
      ->method('getAccession')->will($this->returnValue($term_accession));
    $values['term'] = $term;

    // Actually creating the type.
    $entity_type_obj = \Drupal\tripal\Entity\TripalEntityType::create($values);
    $this->assertIsObject($entity_type_obj, "Unable to create a test content type.");
    $entity_type_obj->save();

    // A quick double check before returning it.
    $entity_type_label = $entity_type_obj->getLabel();
    $this->assertEquals($values['label'], $entity_type_label, "Unable to retrieve label from the newly created entity type.");
  }

  /**
   * Testing that the Tripal content pages load without error
   * and that permissions are correct.
   */
  public function testTripalEmptyContentTypes() {
    $assert = $this->assertSession();

    $web_user = $this->drupalCreateUser([
      'administer tripal',
      'manage tripal content types',
      'administer tripal content',
    ]);

    $urls = [
      'Tripal Content Listing' => 'admin/content/bio_data',
      'Tripal Content Type Listing' => 'admin/structure/bio_data',
      'Add Tripal Content Listing/Form' => 'bio_data/add',
    ];

    // Anonymous User should not be able to see any of these urls.
    foreach ($urls as $msg => $url) {

      $this->drupalGet($url);
      $assert->statusCodeEquals(403);
      $assert->pageTextContains('Access denied');
    }

    // Perform a user login with the permissions specified above
    $this->drupalLogin($web_user);

    // Then check that we can load each page with the correct permissions.
    foreach ($urls as $msg => $url) {
      $this->drupalGet($url);
      $assert->statusCodeEquals(200);
    }
  }

  /**
   * HELPER: Create Tripal Term.
   *
   * NOTE: This function can be removed when PR tripal/t4d8 #274 is merged.
   * At this point you can replace it with TripalTestBrowserBase->createTripalTerm().
   *
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - vocab_name (string)
   *    - id_space_name (string)
   *    - term (array)
   *        - name (string)
   *        - definition (string)
   *        - accession (string)
   */
  private function helperCreateTripalTerm($values) {

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~4 latin capitalized words.
    $values['vocab_name'] = $values['vocab_name'] ?? $random->sentences(4, TRUE);
    // Provides a 4 character string.
    $values['id_space_name'] = $values['id_space_name'] ?? $random->word(4);
    $values['term'] = $values['term'] ?? array();
    // Provides a unique string with ~8 characters.
    $values['term']['accession'] = $values['term']['accession'] ?? $random->name(8, TRUE);
    // Provides a title with ~2 latin capitalized words.
    $values['term']['name'] = $values['term']['name'] ?? $random->sentences(2, TRUE);
    // Provides as collection of sentences with ~20 words.
    $values['term']['definition'] = $values['term']['definition'] ?? $random->sentences(20);

    // Create the Vocabulary.
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocabulary = $vmanager->loadCollection($values['vocab_name']);
    if (!$vocabulary) {
      $vocabulary = $vmanager->createCollection($values['vocab_name'], 'chado_vocabulary');
      $this->assertInstanceOf(TripalVocabularyInterface::class, $vocabulary, "Unable to create the Vocabulary.");
    }

    // Create the ID Space.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idsmanager->loadCollection($values['id_space_name']);
    if (!$idSpace) {
      $idSpace = $idsmanager->createCollection($values['id_space_name'], 'chado_id_space');
      $this->assertInstanceOf(TripalIdSpaceInterface::class, $idSpace, "Unable to create the ID Space.");
    }
    // Assign the vocabulary as the default for this ID Space.
    $idSpace->setDefaultVocabulary($vocabulary->getName());

    $term = $idSpace->getTerm($values['term']['accession']);
    if (!$term) {
      // Now create the term.
      $values['term']['idSpace'] = $idSpace->getName();
      $values['term']['vocabulary'] = $vocabulary->getName();
      $term = new TripalTerm($values['term']);
      $this->assertInstanceOf(TripalTerm::class, $term, "Unable to create the term object.");
      // and save it to the ID Space.
      $idSpace->saveTerm($term);
    }

    return $term;
  }

}
