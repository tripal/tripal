<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField\FieldType;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use Drupal\Core\Entity\Entity\EntityViewDisplay;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\field\Entity\FieldConfig;
use Drupal\Core\Form\FormState;

/**
 * Tests the ChadoPropertyTypeDefault Field Type.
 *
 * @group TripalField
 * @group ChadoField
 */
class ChadoPropertyTypeCRUDTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'user', 'field', 'tripal', 'tripal_chado'];

  use ChadoFieldTestTrait;

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected object $chado_connection;

  /**
   * Details for the field type to test.
   *
   * @var array
   *   contains the keys 'class' and 'id' which indicate the class name and
   *   field type id for the field being tested.
   */
  protected array $field_type = [
    'class' => 'Drupal\tripal_chado\Plugin\Field\FieldType\ChadoPropertyTypeDefault',
    'id' => 'chado_property_type_default'
  ];

  protected array $property_type_terms = [
    'record_id' => [
      'key' => 'record_id',
      'vocab_name' => 'SIO',
      'id_space_name' => 'SIO',
      'term' => [
        'accession' => '000729',
      ]
    ],
    'prop_id' => [
      'key' => 'prop_id',
      'vocab_name' => 'SIO',
      'id_space_name' => 'SIO',
      'term' => [
        'accession' => '000729',
      ],
    ],
    'linker_id' => [
      'key' => 'linker_id',
      'vocab_name' => 'SIO',
      'id_space_name' => 'SIO',
      'term' => [
        'accession' => '000729',
      ],
    ],
    'value' => [
      'key' => 'value',
      'vocab_name' => 'ncit',
      'id_space_name' => 'NCIT',
      'term' => [
        'accession' => 'C25712',
      ],
    ],
    'rank' => [
      'key' => 'rank',
      'vocab_name' => 'OBCS',
      'id_space_name' => 'OBCS',
      'term' => [
        'accession' => '0000117',
      ],
    ],
    'type_id' => [
      'key' => 'type_id',
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'accession' => 'additionalType',
      ],
    ],
  ];

  /**
   * Details for the field widgets valid to be used with the $field_type.
   *
   * @var array
   *   A list of widgets supported by the field type. Each item in the list is
   *   and array with the keys 'class' and 'id' which indicate the class name and
   *   field widget id for that specific widget. The key of the list is a short
   *   name indicating that specific widget.
   */
  protected array $widgets = [
    'long_text' => [
      'class' => 'Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoPropertyWidgetDefault',
      'id' => 'chado_property_type_default',
    ],
    'short_text' => [
      'class' => 'Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoPropertyStringWidgetDefault',
      'id' => 'chado_property_string_widget_default',
    ],
    'select' => [
      'class' => 'Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoPropertySelectWidgetDefault',
      'id' => 'chado_property_string_widget_default',
    ],
  ];

  /**
   * Details for the field formatters valid to be used with the $field_type.
   *
   * @var array
   *   A list of formatters supported by the field type. Each item in the list is
   *   and array with the keys 'class' and 'id' which indicate the class name and
   *   field formatter id for that specific formatter. The key of the list is
   *   a short name indicating that specific formatter.
   */
  protected array $formatters = [
    'ul_list' => [
      'class' => 'Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoPropertyFormatterDefault',
      'id' => 'chado_property_formatter_default',
    ],
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Get Chado in place
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Prepare kernel environment for field testing.
    $this->setupFieldTestEnvironment();

  }

  /**
   * This method tests that we can create an entity with multiple property fields.
   */
  public function testCreateEntityWithField() {

    // Setup the field to be tested based on the data provider values.
    $field_name = $this->randomMachineName();

    $fieldConfig = $this->createFieldInstance(
      'tripal_entity',
      $this->property_type_terms,
      [
        'field_name' => $field_name,
        'field_type' => $this->field_type['id'],
        'formatter_id' => $this->formatters['ul_list']['id'],
      ]
    );

    // Create an entity with a specific value for this field
    // -- use the sample value generating to get a value for this field.
    $field_value = $this->field_type['class']::generateSampleValue($fieldConfig);
    $this->assertIsArray($field_value,
      "The ".$this->field_type['class']."::generateSampleValue() method for this field type did not return a valid value.");
    // -- create the entity with that value set
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->TripalEntityType->getID(),
      $field_name => $field_value,
    ]);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $this->field_type['id'] . " field.");
    // -- confirm the values in the created entity match those we set.
    foreach ($field_value as $property_key => $expected_property_value) {
      $this->assertEquals($expected_property_value, $entity->{$field_name}->{$property_key},
        "The value of the property $property_key was not what we expected for this field.");
      }
  }
}
