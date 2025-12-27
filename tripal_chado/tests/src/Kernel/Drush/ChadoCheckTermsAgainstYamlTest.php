<?php

namespace Drupal\Tests\tripal_chado\Kernel\Drush;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Commands\ChadoCheckTermsAgainstYaml;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Symfony\Component\Console\Output\OutputInterface;

use \Drupal\tripal_chado\Entity\ChadoTermMapping;

/**
 * Tests the Drush Command tripal-chado:trp-check-terms.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Drush
 */
#[Group('bio-cv')]
#[Group('drush-command')]
#[RunTestsInSeparateProcesses]
class ChadoCheckTermsAgainstYamlTest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'tripal', 'tripal_chado'];

  /**
   * The database connection to the test chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * An object of a drush commands class.
   *
   * @var Drupal\tripal_chado\Commands\ChadoCheckTermsAgainstYaml
   */
  protected ChadoCheckTermsAgainstYaml $drush_command;

  /**
   * Stores output from the mock logger, accessed using getLogOutput().
   *
   * @var string
   */
  protected string $log_output = '';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Create a new test schema for us to use.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Install needed configurations.
    ChadoTermMapping::refreshMapping('config/install/tripal.tripal_content_terms.chado_content_terms');

    // Create a mock output to access output.
    $mock_output = $this->createMock(OutputInterface::class);
    $mock_output->method('writeln')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message;
          return NULL;
      });

    // An instance of the ChadoCheckTermsAgainstYaml drush command class.
    $this->drush_command = new ChadoCheckTermsAgainstYaml(
      $this->container->get('config.factory'),
      $this->container->get('tripal.dbx'),
      $this->chado_connection,
    );
    $this->drush_command->setOutput($mock_output);
  }

  /**
   * Retrieves stored mocked log output and then resets it.
   */
  protected function getLogOutput(): string {
    $output = $this->log_output;
    $this->log_output = '';
    return $output;
  }

  /**
   * Tests the drush command directly.
   */
  public function testCheckTermsDrushCommand() {

    // CASE: Check for drush command required parameters.
    $this->drush_command->chadoCheckTermsAreAsExpected([]);
    $this->assertStringContainsString('The --chado_schema argument is required', $this->getLogOutput(),
      'Expect argument error for missing argument');
    $this->drush_command->chadoCheckTermsAreAsExpected(['chado_schema' => 'teapot']);
    $this->assertStringContainsString('The specified chado schema "teapot" does not exist', $this->getLogOutput(),
      'Expect argument error for nonexistent chado schema');

    // CASE: no errors.
    // First run the drush command on our test chado schema with no changes.
    // We expect there to be no errors or warnings in our test chado.
    $this->drush_command->chadoCheckTermsAreAsExpected(['chado_schema' => $this->testSchemaName]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('[OK] There are no errors', $command_output,
      "Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.");
    $this->assertStringContainsString('[OK] There are no warnings', $command_output,
      "Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.");

    // Now add in some CV inconsistencies ;-p
    // CASE: alter the vocabulary description.
    // ----------------------------------------.
    $this->chado_connection->update('1:cv')
      ->fields(['definition' => 'CHANGED CV DESCRIPTION'])
      ->condition('cv.name', 'germplasm_ontology')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    // There should still not be any errors.
    $this->assertStringContainsString('[OK] There are no errors', $command_output,
      "Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.");
    // But now we expect some warnings...
    $this->assertStringNotContainsString('[OK] There are no warnings', $command_output,
      "Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.");
    $this->assertStringContainsString('We have detected 1 vocabularies in your chado instance that differ from those defined in the YAML in small ways', $command_output,
      "We expect the germplasm ontology to show a change in the cv description.");
    $this->assertStringContainsString('CHANGED CV DESCRIPTION', $command_output,
      "We expect the germplasm ontology to show a change in the cv description.");
    $this->assertStringContainsString('[OK] Vocabularies have been updated to match our expectations.', $command_output,
      "We indicated to auto-fix cv issues so we expect to see a confirmation that it was done.");

    // CASE: alter the database description.
    // ----------------------------------------.
    $this->chado_connection->update('1:db')
      ->fields([
        'description' => 'CHANGED DB DESCRIPTION',
        'urlprefix' => 'CHANGED URL PREFIX',
        'url' => 'CHANGED URL',
      ])
      ->condition('db.name', 'PMID')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    // There should still not be any errors.
    $this->assertStringContainsString('[OK] There are no errors', $command_output,
      "Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.");
    // But now we expect some warnings...
    $this->assertStringNotContainsString('[OK] There are no warnings', $command_output,
      "Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.");
    $this->assertStringContainsString('We have detected 1 ID Spaces in your chado instance that differ from those defined in the YAML in small ways', $command_output,
      "We expect the PMID database to show a changes.");
    $this->assertStringContainsString('CHANGED DB DESCRIPTION', $command_output,
      "We expect PMID db to show a change in the description.");
    $this->assertStringContainsString('CHANGED URL PREFIX', $command_output,
      "We expect PMID db to show a change in the urlprefix.");
    $this->assertStringContainsString('CHANGED URL', $command_output,
      "We expect PMID db to show a change in the url.");
    $this->assertStringContainsString('[OK] ID Spaces have been updated to match our expectations.', $command_output,
      "We indicated to auto-fix db issues so we expect to see a confirmation that it was done.");

    // CASE: Test for obsolete CVs removed in PR #1727.
    $this->chado_connection->insert('1:cv')
      ->fields([
        'name' => 'organism_property',
        'definition' => 'A local vocabulary that contains locally defined properties for organisms',
      ])
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
      'chado_schema' => $this->testSchemaName,
      'auto-expand' => TRUE,
      'auto-fix' => FALSE,
    ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('We have detected 1 obsolete vocabularies (CVs)', $command_output,
      'Obsolete vocabulary should be detected');
    $this->assertStringNotContainsString('Removed 1 obsolete controlled vocabularies', $command_output,
      'Obsolete vocabulary should not be corrected with auto-fix false');
    $this->drush_command->chadoCheckTermsAreAsExpected([
      'chado_schema' => $this->testSchemaName,
      'auto-expand' => TRUE,
      'auto-fix' => TRUE,
    ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('We have detected 1 obsolete vocabularies (CVs)', $command_output,
      'Obsolete vocabulary should be detected');
    $this->assertStringContainsString('Removed 1 obsolete controlled vocabularies', $command_output,
      'Obsolete vocabulary should be corrected with auto-fix true');


  }

}
