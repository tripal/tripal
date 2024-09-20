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
 * Specifically focused on coverage using a very simple, single property field
 * test case and the generated value.
 *
 * @group TripalField
 * @group ChadoField
 */
class ChadoPropertyTypeSimpleCRUDTest extends ChadoTestKernelBase {
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

  /**
   * An array of terms that need to be created for this field to work.
   *
   * @var array
   *  A list of terms keyed by the IDSPACE:ACCESSION format where the value is
   *  and array including the parameters for createTripalTerm().
   */
  protected array $terms = [
    'SIO:000729' => [
      'vocab_name' => 'SIO',
      'id_space_name' => 'SIO',
      'term' => [
        'accession' => '000729',
      ]
    ],
    'NCIT:C25712' => [
      'vocab_name' => 'ncit',
      'id_space_name' => 'NCIT',
      'term' => [
        'accession' => 'C25712',
      ],
    ],
    'OBCS:0000117' => [
      'vocab_name' => 'OBCS',
      'id_space_name' => 'OBCS',
      'term' => [
        'accession' => '0000117',
      ],
    ],
    'schema:additionalType' => [
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'accession' => 'additionalType',
      ],
    ],
    'OBI:0100026' => [
      'vocab_name' => 'obi',
      'id_space_name' => 'OBI',
      'term' => [
        'accession' => '0100026',
      ],
    ],
  ];

  /**
   * A List of the expected property types for this field.
   *
   * @var array
   *  A list of property types keyed by the propertyType key where the value
   *  is an array defining the key, term (i.e. IDSPACE:ACCESSION).
   */
  protected array $property_types = [
    'record_id' => [
      'key' => 'record_id',
      'term' => 'SIO:000729',
    ],
    'prop_id' => [
      'key' => 'prop_id',
      'term' => 'SIO:000729',
    ],
    'linker_id' => [
      'key' => 'linker_id',
      'term' => 'SIO:000729',
    ],
    'value' => [
      'key' => 'value',
      'term' => 'NCIT:C25712',
    ],
    'rank' => [
      'key' => 'rank',
      'term' => 'OBCS:0000117',
    ],
    'type_id' => [
      'key' => 'type_id',
      'term' => 'schema:additionalType',
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
   * The random name of the field generated for this test.
   *
   * @var string
   */
  protected string $field_name;

  protected string $bundle_name = 'research_project';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Get Chado in place
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Prepare kernel environment for field testing.
    $this->setupFieldTestEnvironment();

    // Setup the field to be tested based on the data provider values.
    $this->field_name = $this->randomMachineName();

    // Next create the terms for the properies this field will use.
    foreach ($this->terms as $key => $term_deets) {
      $this->createTripalTerm($term_deets, 'chado_id_space', 'chado_vocabulary');
    }

    // Then create the Tripal Content Type.
    $bundle = $this->createTripalContentType([
      'id' => $this->bundle_name,
    ]);
    $bundle->setThirdPartySetting('tripal', 'chado_base_table', 'project');

    // Finally create the bundle, field type (i.e. FieldStorageConfig)
    // and field instance (i.e. FieldConfig).
    $fieldConfig = $this->createFieldInstance(
      'tripal_entity',
      [
        'field_name' => 'project_name',
        'bundle_name' => $this->bundle_name,
        'field_type' => 'chado_string_type_default',
        'formatter_id' => 'chado_string_type_formatter',
        'settings' => [
          'storage_plugin_settings' => [
            'base_table' => 'project',
            'base_column' => 'name',
          ],
        ],
      ]
    );

    // Finally create the bundle, field type (i.e. FieldStorageConfig)
    // and field instance (i.e. FieldConfig).
    $fieldConfig = $this->createFieldInstance(
      'tripal_entity',
      [
        'field_name' => $this->field_name,
        'bundle_name' => $this->bundle_name,
        'field_type' => $this->field_type['id'],
        'formatter_id' => $this->formatters['ul_list']['id'],
        'settings' => [
          'storage_plugin_settings' => [
            'base_table' => 'project',
            'prop_table' => 'projectprop',
          ],
        ],
      ]
    );

  }

  /**
   * This method tests that we can create an entity a property field and that the
   * sample value can be saved and loaded from chado storage using the field.
   */
  public function testCreateEntityWithField() {
    $project_name = 'Random Project for testing ' . uniqid();

    // Create an entity with a specific value for this field
    // -- use the sample value generating to get a value for this field.
    $field_value = [
      'record_id' => NULL,
      'prop_id' => NULL,
      'linker_id' => NULL,
      'value' => 'fred',
      'type_id' => 4,
      'rank' => 0,
    ];
    $this->assertIsArray($field_value,
      "The ".$this->field_type['class']."::generateSampleValue() method for this field type did not return a valid value.");
    // -- create the entity with that value set
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
      $this->field_name => $field_value,
      'project_name' => [
        'record_id' => NULL,
        'value' => $project_name,
      ]
    ]);
    $this->assertInstanceOf(TripalEntity::class, $entity, "We were not able to create a piece of tripal content to test our " . $this->field_type['id'] . " field.");
    // -- confirm the values in the created entity match those we set.
    foreach ($field_value as $property_key => $expected_property_value) {
      $this->assertEquals($expected_property_value, $entity->{$this->field_name}->{$property_key},
        "The value of the property $property_key was not what we expected for this field.");
    }

    // Retrieve values using the Drupal infrastructure.
    // Tests basic Tripal Storage and TripalField interactions.
    $expected_values = [0 => $field_value];
    list($retrieved_values, $tripalStorages) = TripalEntity::getValuesArray($entity);
    $this->assertArrayHasKey('chado_storage', $retrieved_values, "The retrieved values should include ChadoStorage since that is what this field uses.");
    $this->assertArrayHasKey($this->field_name, $retrieved_values['chado_storage'], "Next the field should be registered with ChadoStorage.");
    $this->assertCount(1, $retrieved_values['chado_storage'][$this->field_name],
      "Since this is the sample value we expect only one delta.");
    // -- check each of the values returned.
    $this->assertFieldValuesMatch($expected_values, $this->property_types, $retrieved_values['chado_storage'][$this->field_name]);

    // Save the entity and check it saves in chado.
    $entity->save();
    // @todo check that the records are in chado storage.

    // @todo load the entity again and check it matches what we tried to save.
  }

  /**
   * Tests the tripalTypes method of this field type.
   *
   * This test ensures there are no dependancies on the values of the field
   * when creating the property types.
   */
  public function testTripalTypes() {

    $property_types = $this->field_type['class']::tripalTypes($this->fieldConfig[$this->field_name]);
    $this->assertIsArray($property_types, "We were unable to retrieve the property types for this field.");
    $this->assertCount(sizeof($this->property_types), $property_types, "We did not get the number of property types returned that we expected.");
    foreach ($property_types as $retrieved_type) {
      $this->assertInstanceOf(\Drupal\tripal\TripalStorage\StoragePropertyTypeBase::class, $retrieved_type, "The retrieved property type does not inherit from the StoragePropertyTypeBase.");
      $this->assertArrayHasKey($retrieved_type->getKey(), $this->property_types,
       "The key of the retrieved property type does not match one of the ones we expected.");
    }
  }
}
