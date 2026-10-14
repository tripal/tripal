<?php

namespace Drupal\Tests\tripal_chado\Kernel\ChadoField\FieldType;

use Drupal\Tests\tripal_file\Kernel\TripalFileWidgetTestKernelBase;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the widgets on a License entity.
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
class ChadoLicenseWidgetFormTest extends TripalFileWidgetTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    $this->yaml_info_file = __DIR__ . '/ChadoLicenseWidgetForm-TestInfo.yml';
    parent::setUp();

    // Create three license records in chado for testing (not published).
    // We do this to make sure we support multiple NULL values in the uri
    // column, both through direct sql here, and through the widget.
    // @see https://github.com/tripal/tripal/issues/2419
    $uris = [1 => 'https://fake', 2 => NULL, 3 => NULL];
    for ($i = 1; $i <= 3; $i++) {
      $this->record_ids[$i] = $this->chado_connection
        ->insert('1:license')
        ->fields(['name' => 'License ' . $i, 'summary' => 'License summary ' . $i, 'uri' => $uris[$i]])
        ->execute();
      $this->assertNotEmpty($this->record_ids[$i], 'Did not create license');
    }

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
      'Create new license',
    ];

    return $scenarios;
  }

  /**
   * Tests the license widget.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testChadoLicenseWidget(int $current_scenario_key, string $current_scenario_label) {
    $this->checkTripalFileWidget($current_scenario_key, $current_scenario_label);
  }

}
