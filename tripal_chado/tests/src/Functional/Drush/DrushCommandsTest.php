<?php

namespace Drupal\Tests\tripal\Functional\Drush;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drush\TestTraits\DrushTestTrait;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

use Symfony\Component\Yaml\Yaml;

/**
 * Tests the drush command to populate materialized views.
 *
 * @group drush-command
 */
#[Group('drush-command')]
#[RunTestsInSeparateProcesses]
class DrushCommandsTest extends ChadoTestBrowserBase {

  use DrushTestTrait;

  /**
   * The default theme to use for this test.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * The database connection to the test chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  protected $mview_manager;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Initialize services.
    $this->connection = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
#$this->connection = $this->test_connection;
    $this->mview_manager = \Drupal::service('tripal_chado.materialized_views');
  }

  /**
   * Test the drush command to populate materialized views.
   */
  public function testDrushMviewPopulate() {

    // Drush command without required parameter.
    $this->drush('tripal-chado:populate-mview', [], []);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('Provide a materialized view name', $command_output,
      'View name is a required parameter');

    // Try an invalid schema.
    $this->drush('tripal-chado:populate-mview', ['cv_root_mview'], ['schema-name' => 'invalidschemaname']);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('The schema "invalidschemaname" does not exist', $command_output,
      'An invalid schema name generates an error');

    // So far no materialized views exist.
    $this->drush('tripal-chado:populate-mview', ['cv_root_mview, db2cv_mview'], ['schema-name' => $this->testSchemaName]);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('No materialized views exist', $command_output,
      'No materialized views exist prior to initialization');

    // Test the --list option with no views present.
    $this->drush('tripal-chado:populate-mview', [''], ['schema-name' => $this->testSchemaName, 'list' => NULL]);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('No materialized views exist', $command_output,
      'Expected log message should show that no views exist');

    // Create test materialized views.
    $this->createMviews();

    $this->drush('tripal-chado:populate-mview', ['db2cv_mview'], ['schema-name' => $this->testSchemaName]);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('Populating "db2cv_mview"', $command_output,
      'Expected log message should be generated');
    $nrecords = $this->countViewRecords('db2cv_mview');
    # Expect 30 records, but could change if we add dbs in the future.
    $this->assertGreaterThanOrEqual(30, $nrecords, 'At least 30 records should have been populated in the db2cv_mview');

    // Test the --list option with a view present.
    $this->drush('tripal-chado:populate-mview', [''], ['schema-name' => $this->testSchemaName, 'list' => NULL]);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('The following materialized views exist', $command_output,
      'Expected log message should list materialized views');
    $this->assertStringContainsString('db2cv_mview', $command_output,
      'Expected log message should list materialized views');

    // Test the --all option.
    $this->deleteViewRecords('db2cv_mview');
    $nrecords = $this->countViewRecords('db2cv_mview');
    $this->assertEquals(0, $nrecords, 'View should have been cleared');
    $this->drush('tripal-chado:populate-mview', [''], ['schema-name' => $this->testSchemaName, 'all' => NULL]);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('Populating "db2cv_mview"', $command_output,
      'Expected log message should be generated');
    $nrecords = $this->countViewRecords('db2cv_mview');
    # Expect 30 records again.
    $this->assertGreaterThanOrEqual(30, $nrecords, 'At least 30 records should have been repopulated in the db2cv_mview');

    // Test the --time option.
    $this->drush('tripal-chado:populate-mview', ['db2cv_mview'], ['schema-name' => $this->testSchemaName, 'time' => NULL]);
    $command_output = $this->getOutputRaw() . $this->getErrorOutputRaw();
    $this->assertStringContainsString('Elapsed time', $command_output,
      'Elapsed time should be included if --time option is specified.');
  }

  /**
   * Helper function to count the number of records in a materialized view.
   *
   * @param string $view_name
   *   The name of the materialized view.
   *
   * @return int
   *   The number of records in the materialized view.
   */
  private function countViewRecords(string $view_name): int {
    $count = $this->connection
      ->select('1:' . $view_name, 'v')
      ->fields('v', ['*'])
      ->countQuery()
      ->execute()
      ->fetchField();
    return $count;
  }

  /**
   * Helper function to empty a materialized view.
   *
   * @param string $view_name
   *   The name of the materialized view.
   *
   * @return void
   *   No return value.
   */
  private function deleteViewRecords(string $view_name): void {
    $count = $this->connection
      ->delete('1:' . $view_name)
      ->execute();
  }

  /**
   * Helper function to create some simple materialized views.
   *
   * @return void
   *   No return value.
   */
  private function createMviews(): void {
    // @todo If chado had materialized views defined in a yaml file, we could
    // use that and not need to duplicate here in the test.
    $view_name = 'db2cv_mview';
    $comment = 'A table for quick lookup of the vocabularies and the databases they are associated with.';
    $schema = [
      'table' => $view_name,
      'description' => $comment,
      'fields' => [
        'cv_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cvname' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'db_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'dbname' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'num_terms' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'cv_id_idx' => ['cv_id'],
        'cvname_idx' => ['cvname'],
        'db_id_idx' => ['db_id'],
        'dbname_idx' => ['db_id'],
      ],
    ];
    $sql = "
      SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
        COUNT(CVT.cvterm_id) as num_terms
      FROM cv CV
        INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
        INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN db DB on DB.db_id = DBX.db_id
      WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
      GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
      ORDER BY DB.name
    ";

    $schema_name = $this->connection->getSchemaName();
    $mview = $this->mview_manager->create($view_name, $schema_name);
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
    $mview->setLocked(TRUE);
  }

}
