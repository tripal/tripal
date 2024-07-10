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
    // Provides a machine name for the content type.
    $values['id'] = $random->sentences(1,TRUE);
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

}
