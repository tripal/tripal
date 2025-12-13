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
class DrushMviewPopulateTest extends ChadoTestBrowserBase {

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

#    // Ensure we see all logging in tests.
#    \Drupal::state()->set('is_a_test_environment', TRUE);

#    // Ensure we install the schema/modules we need.
#    $this->prepareEnvironment(['TripalTerm', 'TripalEntity']);
#    // -- additionally we need tripal_chado config to access the yaml files.
#    $this->installConfig('tripal_chado');

    // Initialize services.
    $this->chado_connection = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
    $this->mview_manager = \Drupal::service('tripal_chado.materialized_views');

#    // Create three projects in chado.
#    for ($i = 1; $i <= 3; $i++) {
#      $this->connection->insert('1:project')
#        ->fields([
#          'name' => 'Project No. ' . $i,
#        ])->execute();
#    }

#    // Create three analyses in chado.
#    for ($i = 1; $i <= 3; $i++) {
#      $this->connection->insert('1:analysis')
#        ->fields([
#          'name' => 'Analysis No. ' . $i,
#          'program' => 'PHP',
#          'programversion' => 'Version ' . $i,
#        ])->execute();
#    }


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

    // Create the standard chado materialized views.
    $this->createMviews();

  }

  /**
   * Helper function to count the number of records in a materialized view.
   *
   * @param string $view_name
   *   The name of the materialized view.
   *
   * @return int
   *   The number of records in the materialized view.
  private function countViewLines(string $view_name): int {
    $query = $this->chado_connection->select('1:' . $view_name, 'v');
    $query->fields('v', ['*']);
    $query->countQuery();
    $count = $query->execute();
    return $count;
  }

  /**
   * Helper function to create some simple materialized views.
   *
   * @return void
   *   No return value.
   */
  private function createMviews(): void {
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

$sql = trim(preg_replace('/[\n ]+/', ' ', $sql));
$x[0] = [
       'name' => $view_name,
       'comment' => $comment,
       'schema' => $schema,
       'sql' => $sql,
     ];
file_put_contents('/tmp/xxx', Yaml::dump($x));


#    $mviews = \Drupal::service('tripal_chado.materialized_views');
#    $mview = $mviews->create($view_name, $this->chado_schema_main);
#    $mview->setTableSchema($schema);
#    $mview->setSqlQuery($sql);
#    $mview->setComment($comment);
#    $mview->setLocked(TRUE);

  }

}
