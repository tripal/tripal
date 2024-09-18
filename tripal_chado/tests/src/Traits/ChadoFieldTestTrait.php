<?php
namespace Drupal\Tests\tripal_chado\Traits;

use \Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\field\Entity\FieldConfig;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Provides functions related to testing Chado Fields.
 */
trait ChadoFieldTestTrait {

  use UserCreationTrait;

  protected FieldStorageConfig $fieldStorage;
  protected FieldConfig $fieldConfig;
  protected TripalEntityType $TripalEntityType;
  protected TripalEntity $tripalEntity;

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
    // -- we need the chado term mapping for our properties.
    $this->installEntitySchema('chado_term_mapping');
    // -- we need the field module configuration.
    $this->installConfig(['field']);
    // -- we need access to the core term mappings.
    tripal_chado_rebuild_chado_term_mappings();

  }

  /**
   * Confirms that the retrieved values match the expected ones.
   *
   * More specifically, it checks
   *  1. the retrieved values matches the expected format
   *  2. Each expected property type exists in the retrieved values for
   *     each delta by checking the property key is at values[delta][property key]
   *  3. For each delta[property key]
   *       - the value is an array
   *       - the array has a 'value' key
   *       - the delta[property key][value] is a StoragePropertyValue instance
   *       - the delta[property key][value]->value matches the expected value
   *
   * @param array $expected_values
   *  A nested array of expected values following the format:
   *    - delta (e.g. 0):
   *      - property key => expected value
   * @param array $expected_property_types
   *  A nested array of the expected property types for this field following the format:
   *    - property key (e.g. record_id):
   *      - key (string; e.g. record_id)
   *      - term (string of IDSPACE:ACCESSION; e.g. schema:additionalType)
   * @param array $retrieved_values
   *  A nested array keyed in the following levels:
   *    - 3rd: Delta value of the field item.
   *    - 4th: the property key.
   *    - 5th: One of the following keys:
   *      - 'value': the property value object.
   *      - 'operation': the operation to use when matching this value.
   * @return void
   */
  public function assertFieldValuesMatch($expected_values, $expected_property_types, $retrieved_values) {
    foreach ($retrieved_values as $delta => $ret_values) {
      foreach ($expected_property_types as $property_key => $details) {
        $this->assertArrayHasKey($property_key, $ret_values, "The expected property type did not appear in the retrieved values for delta $delta.");
        $this->assertIsArray($ret_values[$property_key], "Each property key should be an array with a 'value' and 'operation' but [$delta][$property_key] is not an array.");
        /** Currently not always the case...
        $this->assertArrayHasKey('operation', $ret_values[$property_key], "Each property key should be an array with a 'value' and 'operation' but [$delta][$property_key] does not have a operation key.");*/
        $this->assertArrayHasKey('value', $ret_values[$property_key], "Each property key should be an array with a 'value' and 'operation' but [$delta][$property_key] does not have a value key.");
        $this->assertInstanceOf(\Drupal\tripal\TripalStorage\StoragePropertyValue::class, $ret_values[$property_key]['value'], "The value for [$delta][$property_key] is not a StoragePropertyValue object.");
        $this->assertEquals($expected_values[$delta][$property_key], $ret_values[$property_key]['value']->getValue(), "The value of [$delta][$property_key] does not match what we expected.");
      }
    }
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
   *    - settings (array) an array of additional settings for the field
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
    $values['settings'] = $values['settings'] ?? [];

    // Now create the main term for the field.
    $term = $this->createTripalTerm($term_values, 'chado_id_space', 'chado_vocabulary');

    // Now for the field storage.
    $fieldStorage = FieldStorageConfig::create([
      'field_name' => $values['field_name'],
      'entity_type' => $entity_type,
      'type' => $values['field_type'],
      'settings' => [
        'termIdSpace' => $term_values['id_space_name'],
        'termAccession' => $term_values['term']['accession'],
      ] + $values['settings'],
    ]);
    $fieldStorage
      ->save();

    $this->fieldStorage = $fieldStorage;
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

    // Defaults
    $random = $this->getRandomGenerator();
    $values['formatter_id'] = $values['formatter_id'] ?? 'default_tripal_string_type_formatter';
    $values['field_type'] = $values['field_type'] ?? 'tripal_string_type';
    // -- Bundle
    if (!array_key_exists('bundle_name', $values)) {
      $bundle = $this->createTripalContentType();
      $values['bundle_name'] = $bundle->getID();
    }
    else {
      $bundle = \Drupal::entityTypeManager()
        ->getStorage('tripal_entity_type')
        ->loadByProperties(['id' => $values['bundle_name']]);
      $bundle = array_pop($bundle);
    }
    // -- Field Storage Config
    if (!array_key_exists('fieldStorage', $values)) {
      $values['fieldStorage'] = $this->createFieldType(
        'tripal_entity',
        $values
      );
    }

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

    $this->fieldConfig = $fieldConfig;
    $this->TripalEntityType = $bundle;
    return $fieldConfig;
  }
}
