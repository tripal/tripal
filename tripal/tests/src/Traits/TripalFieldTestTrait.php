<?php
namespace Drupal\Tests\tripal\Traits;

use \Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\field\Entity\FieldConfig;

/**
 * Provides functions related to testing Tripal Fields.
 */
trait TripalFieldTestTrait {

  use UserCreationTrait;

  /**
   * Called in the test setUp() for kernel tests to ensure all the needed
   * resources are available.
   */
  public function setupFieldTestEnvironment() {

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Setup the test environment based on the Entity kernel test base.
    $this->installSchema('system', 'sequences');
    // -- we need terms for TripalEntityType, fields and field properties.
    $this->installSchema('tripal', ['tripal_id_space_collection', 'tripal_terms_idspaces', 'tripal_vocabulary_collection', 'tripal_terms_vocabs', 'tripal_terms']);
    // -- we need a user to create an entity.
    $this->installEntitySchema('user');
    $this->setUpCurrentUser();
    // -- we need our tripal content entity to attach the fields to.
    $this->installEntitySchema('tripal_entity');
    // -- we need a tripal content type for our tripal content entity to belong to.
    $this->installEntitySchema('tripal_entity_type');
    // -- we need the field module configuration.
    $this->installConfig(['field']);

  }

  /**
   * Create a FieldStorage object for a given field type.
   *
   * @param string $entity_type
   *   The machine name of the entity to add the field to (e.g., organism)
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - termIdSpace (string)
   *    - termAccession (string)
   * @return FieldStorageConfig
   *   The field storage object that was just created.
   */
  public function createFieldType(string $entity_type, array $values = []) {

    // Defaults
    $random = $this->getRandomGenerator();
    $values['field_name'] = $values['field_name'] ?? $random->word(6) . '_' . $random->word(15);
    $values['field_type'] = $values['field_type'] ?? 'tripal_string_type';
    // -- Term
    $term_values = [];
    if (array_key_exists('termIdSpace', $values)) {
      $term_values['id_space_name'] = $values['termIdSpace'];
    }
    if (array_key_exists('termAccession', $values)) {
      $term_values['term'] = [];
      $term_values['term']['accession'] = $values['termAccession'];
    }
    $term = $this->createTripalTerm($term_values, 'tripal_default_id_space', 'tripal_default_vocabulary');

    // Now for the field storage.
    $fieldStorage = FieldStorageConfig::create([
      'field_name' => $values['field_name'],
      'entity_type' => $entity_type,
      'type' => $values['field_type'],
      'settings' => [
        'termIdSpace' => $term->getIdSpace(),
        'termAccession' => $term->getAccession(),
      ],
    ]);
    $fieldStorage
      ->save();

    return $fieldStorage;
  }

  /**
   * Create a FieldConfig object for a given field type on a given entity.
   *
   * @param string $entity_type
   *   The machine name of the entity to add the field to (e.g., organism)
   * @param array $values
   *   These values are passed directly to the create() method. Suggested values are:
   *    - field_name (string)
   *    - field_type (string)
   *    - term_id_space (string)
   *    - term_accession (string)
   *    - bundle_name (string)
   *    - formatter_id (string)
   *    - fieldStorage (FieldStorageConfig)
   * @return FieldConfig
   *   The field object that was just created.
   */
  public function createFieldInstance(string $entity_type, array $values = []) {

    $fieldConfig = FieldConfig::create([
      'field_storage' => $values['fieldStorage'],
      'bundle' => $values['bundle_name'],
      'required' => TRUE,
    ]);
    $fieldConfig
      ->save();
    $display_options = [
      'type' => $values['formatter_id'],
      'label' => 'hidden',
      'settings' => [],
    ];
    $display = EntityViewDisplay::create([
      'targetEntityType' => $fieldConfig->getTargetEntityTypeId(),
      'bundle' => $values['bundle_name'],
      'mode' => 'default',
      'status' => TRUE,
    ]);
    $display->setComponent($values['fieldStorage']->getName(), $display_options);
    $display->save();

    return $fieldConfig;
  }
}
