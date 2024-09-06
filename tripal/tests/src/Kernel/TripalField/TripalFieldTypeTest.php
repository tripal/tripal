<?php

namespace Drupal\Tests\tripal\Kernel\TripalField;

use Drupal\tripal\Plugin\Field\FieldType\TripalStringTypeItem;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use \Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\field\Entity\FieldConfig;
use Drupal\Core\Form\FormState;

/**
 * Tests the TripalFieldItemBase class indirectly.
 *
 * @group TripalField
 */
class TripalFieldTypeTest extends TripalTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'field', 'tripal'];

  use UserCreationTrait;

  /**
   * The entity the fields should be attached to for testing.
   *
   * @var string
   */
  protected string $entity_type_id = 'tripal_entity';

  /**
   * The name of the tripal entity type to use in testing (i.e. organism)
   *
   * @var string
   */
  protected string $bundle_name;

  /**
   * A term to be associated with the field being tested.
   */
  protected string $termVocab = 'The Best Vocabulary';
  protected string $termIdSpace = 'VOCAB';
  protected string $termAccession = '123456';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Setup the test environment based on the Entity kernel test base.
    $this->installSchema('system', 'sequences');
    $this->installSchema('tripal', ['tripal_id_space_collection', 'tripal_terms_idspaces', 'tripal_vocabulary_collection', 'tripal_terms_vocabs', 'tripal_terms']);
    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_entity');
    $this->installEntitySchema('tripal_entity_type');
    $this->installConfig(['field']);
    $this->setUpCurrentUser();

    // We need a term in order to test some things:
    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $idspace = $idsmanager->createCollection($this->termIdSpace, "tripal_default_id_space");
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $vocab = $vmanager->createCollection($this->termVocab, "tripal_default_vocabulary");
    $term = new \Drupal\tripal\TripalVocabTerms\TripalTerm([
      'name' => 'mock term',
      'idSpace' => $this->termIdSpace,
      'vocabulary' => $this->termVocab,
      'accession' => $this->termAccession,
      'definition' => 'This is simply a random term for use in my test.',
    ]);
    $idspace->saveTerm($term);

    // We also need a bundle with this storage type...
    $this->bundle_name = 'fake_bundle_' . uniqid();
    $bundle = \Drupal\tripal\Entity\TripalEntityType::create([
      'id' => $this->bundle_name,
      'label' => 'FAKE Bundle For Testing',
      'termIdSpace' => 'FAKE',
      'termAccession' => 'Term',
      'help_text' => '',
      'category' => '',
      'title_format' => '',
      'url_format' => '',
      'hide_empty_field' => '',
      'ajax_field' => '',
    ]);
    $this->assertIsObject(
      $bundle,
      "We were unable to create our Tripal Entity type during test setup."
    );
    $bundle->save();

  }

  public function provideFieldsToTest() {
    $senarios =  [];

    // BOOLEAN
    $senarios[] = [
      'type' => [
        'id' => 'tripal_boolean_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalBooleanTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_boolean_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalBooleanTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_boolean_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalBooleanTypeFormatter',
      ],
    ];

    // INTEGER
    $senarios[] = [
      'type' => [
        'id' => 'tripal_integer_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalIntegerTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_integer_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalIntegerTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_integer_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalIntegerTypeFormatter',
      ],
    ];

    // STRING
    $senarios[] = [
      'type' => [
        'id' => 'tripal_string_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalStringTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_string_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalStringTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_string_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalStringTypeFormatter',
      ],
    ];

    // TEXT
    $senarios[] = [
      'type' =>[
        'id' => 'tripal_text_type',
        'class' => 'Drupal\tripal\Plugin\Field\FieldType\TripalTextTypeItem',
      ],
      'widget' => [
        'id' => 'default_tripal_text_type_widget',
        'class' => 'Drupal\tripal\Plugin\Field\FieldWidget\TripalTextTypeWidget',
      ],
      'formatter' => [
        'id' => 'default_tripal_text_type_formatter',
        'class' => ' Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalTextTypeFormatter',
      ],
    ];

    return $senarios;
  }
  /**
   * This method tests that we can create an entity with this field.
   *
   * @dataProvider provideFieldsToTest
   */
  public function testCreateEntityWithField($field_type, $field_widget, $field_formatter) {

    // Setup the field to be tested based on the data provider values.
    $field_name = $this->randomMachineName();
    $fieldStorage = FieldStorageConfig::create([
      'field_name' => $field_name,
      'entity_type' => $this->entity_type_id,
      'type' => $field_type['id'],
      'settings' => [
        'termIdSpace' => $this->termIdSpace,
        'termAccession' => $this->termAccession,
      ],
    ]);
    $fieldStorage
      ->save();
    $fieldConfig = FieldConfig::create([
      'field_storage' => $fieldStorage,
      'bundle' => $this->bundle_name,
      'required' => TRUE,
    ]);
    $fieldConfig
      ->save();
    $display_options = [
      'type' => $field_formatter['id'],
      'label' => 'hidden',
      'settings' => [],
    ];
    $display = EntityViewDisplay::create([
      'targetEntityType' => $fieldConfig->getTargetEntityTypeId(),
      'bundle' => $fieldConfig->getTargetBundle(),
      'mode' => 'default',
      'status' => TRUE,
    ]);
    $display->setComponent($fieldStorage->getName(), $display_options);
    $display->save();

    $field_value = $field_type['class']::generateSampleValue($fieldConfig);
    $this->assertIsArray($field_value,
      "The ".$field_type['class']."::generateSampleValue() method for this field type did not return a valid value.");
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
      $field_name => $field_value,
    ]);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $field_type['id'] . " field.");
    foreach ($field_value as $property_key => $expected_property_value) {
      $this->assertEquals($expected_property_value, $entity->{$field_name}->{$property_key},
        "The value of the property $property_key was not what we expected for this field.");
    }
  }
}
