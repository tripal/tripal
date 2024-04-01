<?php
namespace Drupal\Tests\tripal\Traits;

use Drupal\tripal\TripalVocabTerms\Interfaces\TripalVocabularyInterface;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;
use Drupal\tripal\TripalVocabTerms\TripalTerm;

/**
 * Provides functions related to setting up Tripal test environments
 * and can be used in either Kernel or Functional tests.
 *
 *
 */
trait TripalTestTrait {

  /**
   * Creates a Tripal Field for testing purposes.
   *
   * @param string $entity_type
   *   The machine name of the entity to add the field to (e.g., organism)
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - term (TripalTerm)
   *    - is_required (boolean)
   *    - cardinality (integer)
   *    - storage_settings (array)
   */
  public function createTripalField(string $entity_type, array $values = []) {

    // Setting the default values:
    $random = $this->getRandomGenerator();
    $values['field_name'] = $values['field_name'] ?? $random->word(6) . '_' . $random->word(15);
    $values['field_type'] = $values['field_type'] ?? 'tripal_string_type';
    $values['term'] = $values['term'] ?? $this->createTripalTerm();
    $values['cardinality'] = $values['cardinality'] ?? 1;
    $values['storage_plugin_settings'] = $values['storage_plugin_settings'] ?? [];
    $values['is_required'] = $values['is_required'] ?? FALSE;

    $term = $values['term'];

    $field = [
      'name' => $values['field_name'],
      'label' => ucwords($term->getName()),
      'type' => $values['field_type'],
      'description' => $term->getDefinition(),
      'cardinality' => $values['cardinality'],
      'required' => $values['is_required'],
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings' => $values['storage_plugin_settings'],
      ],
      'settings' => [
        'termIdSpace' => $term->getIdSpace(),
        'termAccession' => $term->getAccession(),
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 5,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 5,
          ],
        ],
      ],
    ];

    /**
     * @var \Drupal\tripal\Services\TripalFieldCollection $tripal_fields
     */
    $tripal_fields = \Drupal::service('tripal.tripalfield_collection');
    $tripal_fields->addBundleField($entity_type, $field);

    return $field;
  }

  /**
   * Creates a Tripal Vocabulary / ID Space / Term for testing purposes.
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
  public function createTripalTerm($values, $idspace_plugin_id, $vocab_plugin_id) {

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
      $vocabulary = $vmanager->createCollection($values['vocab_name'], $vocab_plugin_id);
      $this->assertInstanceOf(TripalVocabularyInterface::class, $vocabulary, "Unable to create the Vocabulary.");
    }

    // Create the ID Space.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idSpace = $idsmanager->loadCollection($values['id_space_name']);
    if (!$idSpace) {
      $idSpace = $idsmanager->createCollection($values['id_space_name'], $idspace_plugin_id);
      $this->assertInstanceOf(TripalIdSpaceInterface::class, $idSpace, "Unable to create the ID Space.");
      $idSpace->setDefaultVocabulary($vocabulary->getName());
    }


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

  /**
   * Creates a Tripal Entity (i.e. a piece of Tripal content) for testing purposes.
   *
   * Currently this function creates a real TripalEntity and saves it.
   * Because this is a testing environment, this does get saved in the
   * Drupal prefixed test tables and dropped after the fact.
   *
   * We went this route intially for maximum code coverage in order to catch
   * all deprication notices posed by Drupal. We may want to replace this approach
   * at a later date.
   *
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - title (string)
   *    - type (string; eg. organism)
   *    - user_id (integer)
   *    - status (boolean; TRUE if published)
   */
  public function createTripalContent($values = []) {

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~8 latin capitalized words.
    $values['title'] = $values['title'] ?? $random->sentences(8, TRUE);
    $values['id'] = $values['id'] ?? $random->sentences(1, TRUE);

    // Creates a type if one is not provided.
    if (!isset($values['type'])) {
      $content_type = $this->createTripalContentType();
      $values['type'] = $content_type->id();
    }

    $entity = \Drupal\tripal\Entity\TripalEntity::create($values);
    $this->assertIsObject($entity, "Unable to create a test entity.");

    return $entity;
  }

  /**
   * Creates a Tripal Entity Type object for testing purposes.
   *
   * Currently this function creates a real TripalEntityType and saves it.
   * Because this is a testing environment, this does get saved in the
   * Drupal prefixed test tables and dropped after the fact.
   *
   * We went this route intially for maximum code coverage in order to catch
   * all deprication notices posed by Drupal. We may want to replace this approach
   * at a later date.
   *
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    -     id (string)
   *    -     label (label; string)
   *    -     termIdSpace (string)
   *    -     termAccession (string)
   *    -     help_text (text)
   *    -     category (string)
   *    -     title_format (string)
   *    -     url_format (string)
   *    -     hide_empty_field (boolean)
   *    -     ajax_field (boolean)
   */
  public function createTripalContentType($values = []) {

    // Setting the default values:
    $random = $this->getRandomGenerator();
    // Provides a title with ~3 latin capitalized words.
    $values['label'] = $values['label'] ?? $random->sentences(3,TRUE);
    $values['id'] = $values['id'] ?? $random->sentences(1,TRUE);
    // Provides a random non-unique 4 character string.
    $values['termIdSpace'] = $values['termIdSpace'] ?? $random->string(4);
    // Provides a random non-unique 10 character string.
    $values['termAccession'] = $values['termAccession'] ?? $random->string(10);
    // Provides a few of sentences with ~50 words total.
    $values['help_text'] = $values['help_text'] ?? $random->sentences(50);
    $values['category'] = $values['category'] ?? 'Testing Types';
    $values['title_format'] = $values['title_format'] ?? 'This is a title format with no tokens';
    $values['url_format'] = $values['url_format'] ?? '/url/format/with/no/tokens';
    $values['hide_empty_field'] = $values['hide_empty_field'] ?? FALSE;
    $values['ajax_field'] = $values['ajax_field'] ?? FALSE;

    // Actually creating the type.
    $entity_type_obj = \Drupal\tripal\Entity\TripalEntityType::create($values);
    $this->assertIsObject($entity_type_obj, "Unable to create a test content type.");
    $entity_type_obj->save();

    // A quick double check before returning it.
    $this->assertEquals($values['label'], $entity_type_obj->getLabel(), "Unable to retrieve label from the newly created entity type.");

    return $entity_type_obj;
  }
}
