<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField\FieldType;

use Drupal\file\Entity\File;
use Drupal\Tests\tripal_file\Kernel\TripalFileWidgetTestKernelBase;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the widgets on a File entity.
 *
 * Specifically focused on create + update actions performed on the entity
 * directly. Both TripalEntity, ChadoStorage and the field will be covered.
 *
 * @group TripalField
 * @group ChadoField
 * @group tripal-file
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[Group('tripal-file')]
#[RunTestsInSeparateProcesses]
class ChadoFileLocationWidgetFormTest extends TripalFileWidgetTestKernelBase {

  /**
   * A file in the public:// space for testing.
   *
   * @var \Drupal\file\Entity\File
   */
  protected File $test_file;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    $this->yaml_info_file = __DIR__ . '/ChadoFileLocationWidgetForm-TestInfo.yml';
    parent::setUp();

    // Create one license record in chado for testing (not published).
    $this->record_ids[0] = $this->chado_connection
      ->insert('1:license')
      ->fields([
        'name' => 'Public Domain',
        'summary' => 'You can do anything at zombo com',
        'uri' => 'https://zombo.com',
      ])
      ->execute();
    $this->assertNotEmpty($this->record_ids[0], 'Did not create license');

    // Create a managed file with a specific md5 checksum
    // 58ce66d7df0a1cf9b360cabf43da3ea5. Conveniently, this file will
    // not persist outside the testing environment.
    $filepath = 'public://tripal_genome.fna';
    $contents = "ACGT\n";
    file_put_contents($filepath, $contents);
    $this->test_file = File::create([
      'uri' => $filepath,
      'uid' => 1,
    ]);
    $this->test_file->save();
    $this->assertFileExists($filepath, 'Test file ' . $filepath . ' was not created');
  }

  /**
   * Data Provider: works with the YAML to provide scenarios for testing.
   *
   * @return array
   *   List of scenarios to test where each one matches a key and label in the
   *   associated YAML scenarios.
   */
  public static function provideScenarios() {
    $scenarios = [];

    $scenarios[] = [
      0,
      'Base Fields Only',
    ];

    return $scenarios;
  }

  /**
   * Retrieves the current scenario based on the data provider.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @return array
   *   The scenario to be tested as defined in the YAML.
   */
  public function retrieveCurrentScenario(int $current_scenario_key, string $current_scenario_label) {
    $current_scenario = parent::retrieveCurrentScenario($current_scenario_key, $current_scenario_label);

    // Insert the test license actual ID into the scenario.
    $current_scenario['create']['user_input']['file_license'][0]['license_id'] = $this->record_ids[0];
    $current_scenario['create']['expected_field_values']['file_license'][0]['license_id'] = $this->record_ids[0];
    $current_scenario['edit']['expected_field_values']['file_license'][0]['license_id'] = $this->record_ids[0];

    return $current_scenario;
  }

  /**
   * Tests the ChadoFileType field through entity form + field widget.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testChadoFileWidgetUpdate(int $current_scenario_key, string $current_scenario_label) {
    $this->checkTripalFileWidget($current_scenario_key, $current_scenario_label);
  }

}
