<?php

namespace Drupal\Tests\tripal_chado\Functional\Drush;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drush\TestTraits\DrushTestTrait;

/**
 * Tests the Drush Command tripal-chado:trp-check-terms
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Drush
 */
class ChadoCheckTermsAgainstYaml extends ChadoTestBrowserBase {
  protected $defaultTheme = 'stark';

  protected static $modules = ['system', 'tripal', 'tripal_chado'];

  protected $connection;

  use DrushTestTrait;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Create a new test schema for us to use.
    $this->connection = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests the drush command directly.
   */
  public function testCheckTermsDrushCommand() {

    // First run the drush command on our test chado schema with no changes.
    // We expect there to be no errors or warnings in our test chado.
    $this->drush('tripal-chado:trp-check-terms', [], ['chado_schema' => $this->testSchemaName]);
    $command_output = $this->getOutputRaw();
    $this->assertStringContainsString('[OK] There are no errors', $command_output,
      "Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.");
    $this->assertStringContainsString('[OK] There are no warnings', $command_output,
      "Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.");
  }
}
