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
          $this->log_output .= $message . "\n";
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

    $buddy_manager = \Drupal::service('tripal_chado.chado_buddy');
    $dbxref_buddy = $buddy_manager->createInstance('chado_dbxref_buddy', []);
    $cvterm_buddy = $buddy_manager->createInstance('chado_cvterm_buddy', []);

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
      'Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.');
    $this->assertStringContainsString('[OK] There are no warnings', $command_output,
      'Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.');

    // Now add in some CV inconsistencies ;-p
    // CASE: alter the vocabulary record.
    // ----------------------------------------.
    $this->chado_connection->update('1:cv')
      ->fields(['definition' => 'CHANGED CV DEFINITION'])
      ->condition('cv.name', 'germplasm_ontology')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    // This should trigger an error.
    $this->assertStringContainsString('[OK] There are no errors', $command_output,
      'Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.');
    $this->assertStringNotContainsString('[OK] There are no warnings', $command_output,
      'Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.');
    $this->assertStringContainsString('We have detected 1 vocabularies in your chado instance that differ from those defined in the YAML in small ways', $command_output,
      'We expect the germplasm ontology to show a change in the cv description.');
    $this->assertStringContainsString('CHANGED CV DEFINITION', $command_output,
      'We expect the germplasm ontology to show a change in the cv description.');
    $this->assertStringContainsString('[OK] Vocabularies have been updated to match our expectations.', $command_output,
      'We indicated to auto-fix cv issues so we expect to see a confirmation that it was done.');

    // CASE: alter the database record.
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
      'Ensure that the trp-check-terms command does not find any errors in the prepared test chado instance.');
    // But now we expect some warnings...
    $this->assertStringNotContainsString('[OK] There are no warnings', $command_output,
      'Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.');
    $this->assertStringContainsString('We have detected 1 ID Spaces in your chado instance that differ from those defined in the YAML in small ways', $command_output,
      'We expect the PMID database to show a change.');
    $this->assertStringContainsString('CHANGED DB DESCRIPTION', $command_output,
      'We expect PMID db to show a change in the description.');
    $this->assertStringContainsString('CHANGED URL PREFIX', $command_output,
      'We expect PMID db to show a change in the urlprefix.');
    $this->assertStringContainsString('CHANGED URL', $command_output,
      'We expect PMID db to show a change in the url.');
    $this->assertStringContainsString('[OK] ID Spaces have been updated to match our expectations.', $command_output,
      'We indicated to auto-fix db issues so we expect to see a confirmation that it was done.');

    // CASE: alter the cvterm record's CV.
    // This cannot be auto-fixed.
    $cvterm = $cvterm_buddy->getCvterm(['cvterm.name' => 'Identifier']);
    $cv_id = $cvterm[0]->getValue('cvterm.cv_id');
    $this->chado_connection->update('1:cvterm')
      ->fields([
        'cv_id' => 1,
      ])
      ->condition('cvterm.name', 'Identifier')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    // This case will trigger an error but no warnings.
    $this->assertStringNotContainsString('[OK] There are no errors', $command_output,
      'Expect an error with a cvterm pointing to the wrong CV.');
    $this->assertStringContainsString('[OK] There are no warnings', $command_output,
      'Ensure that the trp-check-terms command does not find any warnings in the prepared test chado instance.');
    $this->assertStringContainsString('We have detected 1 Term(s) with a key deviation from what is expected', $command_output,
      'We expect the "Identifier" cvterm to show a change.');
    // Repair the damage to the test DB.
    $this->chado_connection->update('1:cvterm')
      ->fields([
        'cv_id' => $cv_id,
      ])
      ->condition('cvterm.name', 'Identifier')
      ->execute();

    // CASE: Test for YAML duplication.
    // We define the same term twice.
    $config_factory = \Drupal::service('config.factory');
    $config_key = 'tripal.tripal_content_terms.chado_content_terms';
    $config = $config_factory->getEditable($config_key);
    $okay_vocabs = $config->get('vocabularies');
    // Duplicate the first term in the first vocabulary.
    $bad_vocabs = $okay_vocabs;
    $term = $bad_vocabs[0]['terms'][0];
    $bad_vocabs[0]['terms'][] = $term;
    $config->set('vocabularies', $bad_vocabs)->save();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('YAML Issues: Duplicated term definitions in the site YAML', $command_output,
      'YAML duplication should be reported');
    $this->assertStringContainsString('was defined more than once', $command_output,
      'YAML duplication should be reported');

    // CASE: Term has nonexistent DB in YAML.
    // Set the first term in the first vocabulary to have invalid DB.
    // ------------------------------- ---- ------- -------- --------
    //  YAML Term                       CV   DB      CVTERM   DBXREF
    // ------------------------------- ---- ------- -------- --------
    //  accession (XXXCO_010:0000044)   5    X(red)  3         3(red).
    $bad_vocabs = $okay_vocabs;
    $bad_vocabs[0]['terms'][0]['id'] = 'XXX' . $bad_vocabs[0]['terms'][0]['id'];
    $config->set('vocabularies', $bad_vocabs)->save();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('We have detected 1 ID Space(s) missing from your YAML file', $command_output,
      'Nonexistent DB in term should be reported');
    $config->set('vocabularies', $okay_vocabs)->save();

    // CASE: Missing ID Space definitions, vocabulary with no defined DBs.
    $bad_vocabs = $okay_vocabs;
    unset($bad_vocabs[0]['idSpaces']);
    $config->set('vocabularies', $bad_vocabs)->save();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('There were no ID Spaces at all defined for this vocabulary', $command_output,
      'Nonexistent DB definition should be reported');
    $config->set('vocabularies', $okay_vocabs)->save();

    // CASE: Only CV term definition differs.
    $this->chado_connection->update('1:cvterm')
      ->fields([
        'definition' => 'CHANGED CVTERM DEFINITION',
      ])
      ->condition('cvterm.name', 'Identifier')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('We have detected 1 Terms in your chado instance that differ from those defined in the YAML in small ways', $command_output,
      'Expect a warning if term definition differs');
    $this->assertStringContainsString('CHANGED CVTERM DEFINITION', $command_output,
      'Expect a warning if term definition differs');
    // Verify that it was fixed.
    $this->drush_command->chadoCheckTermsAreAsExpected([
        'chado_schema' => $this->testSchemaName,
        'auto-expand' => TRUE,
        'auto-fix' => TRUE,
      ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('[OK] There are no errors associated with this chado instance', $command_output,
      'Expect no errors after auto-fix');
    $this->assertStringContainsString('[OK] There are no warnings associated with this chado instance', $command_output,
      'Expect no warnings after auto-fix');

    // CASE: CV term connected to wrong dbxref.
    // Change the dbxref on an existing term. Not auto-fixable.
    $dbxref = $dbxref_buddy->insertDbxref(['db.name' => 'local', 'dbxref.accession' => 'TREE3']);
    $new_dbxref_id = $dbxref->getValue('dbxref.dbxref_id');
    $term = $cvterm_buddy->getCvterm(['cvterm.name' => 'array design']);
    $cvterm_id = $term[0]->getValue('cvterm.cvterm_id');
    $old_dbxref_id = $term[0]->getValue('dbxref.dbxref_id');
    $this->chado_connection->update('1:cvterm')
      ->fields([
        'dbxref_id' => $new_dbxref_id,
      ])
      ->condition('cvterm_id', $cvterm_id, '=')
      ->execute();
    $term[0]->setValue('dbxref.dbxref_id', $new_dbxref_id, ['cvterm.cvterm_id' => $term[0]->getValue('cvterm.cvterm_id')]);
    $this->drush_command->chadoCheckTermsAreAsExpected([
      'chado_schema' => $this->testSchemaName,
      'auto-expand' => TRUE,
      'auto-fix' => TRUE,
    ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('Broken Connection between cvterm + dbxref', $command_output,
      'Should detect when cvterm has wrong dbxref');
    $this->assertStringContainsString('We have detected 1 Term(s) with a key deviation from what is expected', $command_output,
      'Should detect when cvterm has wrong dbxref');
    // Repair the damage to the test DB.
    $this->chado_connection->update('1:cvterm')
      ->fields([
        'dbxref_id' => $old_dbxref_id,
      ])
      ->condition('cvterm_id', $cvterm_id, '=')
      ->execute();

    // CASE: Term linked to dbxref with wrong DB. DB should be 'operation'.
    $term = $cvterm_buddy->getCvterm(['cvterm.name' => 'genome assembly']);
    $dbxref_id = $term[0]->getValue('dbxref.dbxref_id');
    $db_id = $term[0]->getValue('db.db_id');
    $accession = $term[0]->getValue('dbxref.accession');
    $this->chado_connection->update('1:dbxref')
      ->fields([
        'db_id' => $db_id + 1,
      ])
      ->condition('dbxref_id', $dbxref_id, '=')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
      'chado_schema' => $this->testSchemaName,
      'auto-expand' => TRUE,
      'auto-fix' => TRUE,
    ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('We have detected 1 Term(s) with a key deviation from what is expected', $command_output,
      'Should detect when cvterm has dbxref pointing to wrong db');
    $this->assertStringContainsString('Wrong db but dbxref connected to right cvterm', $command_output,
      'Should detect when cvterm has dbxref pointing to wrong db');
    // Repair the damage to the test DB.
    $this->chado_connection->update('1:dbxref')
      ->fields([
        'db_id' => $db_id,
      ])
      ->condition('dbxref_id', $dbxref_id, '=')
      ->execute();

    // CASE: Dbxref accession is not correct.
    $this->chado_connection->update('1:dbxref')
      ->fields([
        'accession' => 'WRONG_ACCESSION',
      ])
      ->condition('dbxref_id', $dbxref_id, '=')
      ->execute();
    $this->drush_command->chadoCheckTermsAreAsExpected([
      'chado_schema' => $this->testSchemaName,
      'auto-expand' => TRUE,
      'auto-fix' => TRUE,
    ]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('We have detected 1 Term(s) with a key deviation from what is expected', $command_output,
      'Should detect when cvterm has dbxref pointing to wrong db');
    $this->assertStringContainsString('Wrong or Missing dbxref', $command_output,
      'Should detect when cvterm has dbxref pointing to wrong db');
    // Repair the damage to the test DB.
    $this->chado_connection->update('1:dbxref')
      ->fields([
        'accession' => $accession,
      ])
      ->condition('dbxref_id', $dbxref_id, '=')
      ->execute();

    // CASE: Test for obsolete CVs removed in PR #1727.
    $cv_values = [
      'cv.name' => 'organism_property',
      'cv.definition' => 'A local vocabulary that contains locally defined properties for organisms',
    ];
    $term_values = $cv_values + [
      'db.name' => 'local',
      'dbxref.accession' => '0000000X',
      'cvterm.name' => 'plant_mood',
      'cvterm.definition' => 'how happy the plant is',
    ];
    $cvterm_buddy->insertCv($cv_values, []);
    $cvterm_buddy->insertCvterm($term_values, []);
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
    // Try again but this time fix it.
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
    $check = $cvterm_buddy->getCv(['cv.name' => 'organism_property']);
    $this->assertEmpty($check, 'organism_property CV should be deleted if we specify auto-fix');
  }

}
