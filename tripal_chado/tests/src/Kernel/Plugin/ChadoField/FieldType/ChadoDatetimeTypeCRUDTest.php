<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoField\FieldType;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoFieldTestTrait;
use Drupal\tripal\Entity\TripalEntity;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the ChadoDatetimeTypeDefault field type.
 *
 * Covers create, load, and update operations through TripalEntity,
 * ChadoStorage, and the field plugin layer. Uses
 * chado.analysis.timeexecuted as the backing column since it is
 * the simplest Chado table with a timestamp column.
 *
 * @group tripal-field
 * @group chado-field
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[RunTestsInSeparateProcesses]
class ChadoDatetimeTypeCRUDTest extends ChadoTestKernelBase {

  use ChadoFieldTestTrait;

  /**
   * The theme to use when testing.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * The modules that this test depends on.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'field', 'datetime', 'tripal', 'tripal_chado'];

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected object $chado_connection;

  /**
   * The test drupal connection. It is also set in the container.
   *
   * @var object
   */
  protected object $drupal_connection;

  /**
   * The YAML file indicating the scenarios to test and how to setup the enviro.
   *
   * @var string
   */
  protected string $yaml_info_file = __DIR__ . '/ChadoDatetimeType-TestInfo.yml';

  /**
   * Describes the environment to setup for this test.
   *
   * @var array
   */
  protected array $system_under_test;

  /**
   * The TripalEntityType id of the bundle being used in this test.
   *
   * @var string
   */
  protected string $bundle_name;

  /**
   * Describes the scenarios to test.
   *
   * This will be used in combination with the data provider. It can't be
   * accessed directly in the dataProvider due to the way that PHPUnit is
   * setup.
   *
   * @var array
   */
  protected array $scenarios;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $this->drupal_connection = $this->container->get('database');

    [$this->system_under_test, $this->scenarios] = $this->getTestInfoFromYaml($this->yaml_info_file);
    $this->bundle_name = $this->system_under_test['bundle']['id'];

    $this->chado_connection = $this->getTestSchema(
      ChadoTestKernelBase::PREPARE_TEST_CHADO,
      $this->system_under_test['chado_version'] ?? '1.3'
    );

    $this->setupChadoEntityFieldTestEnvironment($this->system_under_test);
  }

  /**
   * Data provider: scenarios from the YAML file.
   */
  public static function provideScenarios(): array {
    // The second value is largely useless for our purpose.
    return [
      [0, 'Create and load a datetime value'],
      [1, 'Datetime with sub-second precision'],
    ];
  }

  /**
   * Tests the ChadoDatetimeTypeDefault field through TripalEntity->save().
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testChadoDatetimeTypeEntityCrud(int $current_scenario_key, string $current_scenario_label): void {
    $current_scenario = $this->getYamlScenario($current_scenario_key, $current_scenario_label);

    // 1. Create.
    $entity = TripalEntity::create([
      'title' => $this->randomString(),
      'type' => $this->bundle_name,
    ] + $current_scenario['create']['user_input']);
    $this->assertInstanceOf(TripalEntity::class, $entity,
      "Could not create TripalEntity for scenario: " . $current_scenario['label']);
    $status = $entity->save();
    $this->assertEquals(SAVED_NEW, $status,
      "Expected SAVED_NEW for scenario: " . $current_scenario['label']);

    // 2. Load and verify the created values.
    $created_entity = TripalEntity::load($entity->id());
    $this->assertFieldValuesMatch(
      $current_scenario['create']['expected'],
      $created_entity,
      $current_scenario['label'] . ' CREATE '
    );

    // 3. Edit.
    foreach ($current_scenario['edit']['user_input'] as $field_name => $new_values) {
      $created_entity->set($field_name, $new_values);
    }
    $status = $created_entity->save();
    $this->assertEquals(SAVED_UPDATED, $status,
      "Expected SAVED_UPDATED for scenario: " . $current_scenario['label']);

    // 4. Load and verify the updated values.
    $updated_entity = TripalEntity::load($created_entity->id());
    $this->assertFieldValuesMatch(
      $current_scenario['edit']['expected'],
      $updated_entity,
      $current_scenario['label'] . ' EDIT '
    );
  }

}
