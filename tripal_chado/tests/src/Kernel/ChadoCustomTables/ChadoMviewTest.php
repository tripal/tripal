<?php

declare(strict_types=1);

namespace Drupal\Tests\tripal_chado\Kernel\ChadoCustomTables;

use Drupal\Core\Database\Connection;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoCustomTables\ChadoMview;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use PHPUnit\Framework\Attributes\Test;

/**
 * Kernel test for ChadoMview create/populate/delete lifecycle.
 *
 * @group tripal-chado
 * @group service-chado-custom-table
 */
#[CoversClass(ChadoMview::class)]
#[Group('tripal-chado')]
#[Group('service-chado-custom-table')]
#[RunTestsInSeparateProcesses]
class ChadoMviewTest extends ChadoTestKernelBase {

  /**
   * Modules to enable for this Kernel test.
   *
   * @var array
   */
  protected static $modules = [
    'tripal',
    'tripal_chado',
  ];

  /**
   * Connection to Drupal public schema.
   *
   * @var Drupal\Core\Database\Connection
   */
  protected Connection $drupal_connection;

  /**
   * Connection to Chado schema.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * Record IDs of chado organisms.
   *
   * @var array
   */
  protected array $organism_ids;

  /**
   * Test initialization, get database connections.
   */
  protected function setUp(): void {
    parent::setUp();

    // Drupal DB connection for metadata assertions.
    $this->drupal_connection = \Drupal::database();

    // Initialize a prepared Chado schema (contains baseline data).
    // PREPARE_TEST_CHADO includes records loaded during "prepare".
    $this->chado_connection = $this->getTestSchema(self::PREPARE_TEST_CHADO);

    // Install required schemas.
    $this->installSchema('tripal_chado', ['tripal_custom_tables', 'tripal_mviews']);

    // Create organism records, common name length is 19 characters.
    $this->organism_ids[] = $this->chado_connection
      ->insert('1:organism')
      ->fields(['genus' => 'Tripalus', 'species' => 'bogusii', 'common_name' => 'Common false tripal'])
      ->execute();
    $this->organism_ids[] = $this->chado_connection
      ->insert('1:organism')
      ->fields(['genus' => 'Tripalus', 'species' => 'databasica'])
      ->execute();
  }

  /**
   * Kernel test for ChadoMview create/populate/delete lifecycle.
   */
  #[Test]
  public function createPopulateDeleteMviewTest(): void {
    // Define a minimal mview.
    $schema = [
      'table' => 'mv_test_organism_len',
      'fields' => [
        'organism_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'name_len' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
      ],
      'primary key' => ['organism_id'],
      'indexes' => [
        'mv_test_organism_len_idx1' => ['organism_id'],
      ],
    ];

    $populateSql = <<<SQL
      SELECT o.organism_id AS organism_id,
        LENGTH(COALESCE(o.common_name, '')) AS name_len
      FROM organism o
    SQL;

    // Create the ChadoMview.
    $schema_name = $this->chado_connection->getSchemaName();
    $mview = new ChadoMview($schema['table'], $schema_name);
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($populateSql);
    $mview->setComment('Comment Okay');
    $id = (int) $mview->getMviewId();
    $this->assertGreaterThanOrEqual(1, $id, 'Created mview has an ID value');

    // Assert metadata rows exist.
    $customTableCount = (int) $this->drupal_connection
      ->select('tripal_custom_tables', 'tct')
      ->condition('tct.table_name', $schema['table'], '=')
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals(1, $customTableCount, 'Custom tables metadata row is present after create().');

    $mviewMetaCount = (int) $this->drupal_connection
      ->select('tripal_mviews', 'tm')
      ->fields('tm', ['mview_id'])
      ->condition('tm.name', $schema['table'], '=')
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals(1, $mviewMetaCount, 'Mview metadata row is present after create().');

    // Table should be empty before being populated.
    $rowCount = (int) $this->chado_connection
      ->select('1:' . $schema['table'])
      ->fields('t', ['*'])
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals(0, $rowCount, 'Mview table is empty before being populated');

    // Populate and verify rows were inserted.
    $mview->populate();
    $organismCount = (int) $this->chado_connection
      ->select('1:organism', 't')
      ->fields('t', ['*'])
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals(2, $organismCount, 'Organism table has expected number of records');
    $rowCount = (int) $this->chado_connection
      ->select('1:' . $schema['table'], 't')
      ->fields('t', ['*'])
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals($organismCount, $rowCount, 'Mview table is populated with one record per organism');

    // Check mview values. First organism should be 19, second zero.
    $first_value = (int) $this->chado_connection
      ->select('1:' . $schema['table'], 't')
      ->fields('t', ['name_len'])
      ->condition('organism_id', $this->organism_ids[0], '=')
      ->execute()
      ->fetchField();
    $this->assertEquals(19, $first_value, 'Value for first organism is correct');
    $second_value = (int) $this->chado_connection
      ->select('1:' . $schema['table'], 't')
      ->fields('t', ['name_len'])
      ->condition('organism_id', $this->organism_ids[1], '=')
      ->execute()
      ->fetchField();
    $this->assertEquals(0, $second_value, 'Value for second organism is correct');

    // Delete and assert cleanup.
    $mview->delete();

    $tableExists = $this->chado_connection->schema()->tableExists($schema['table']);
    $this->assertFalse($tableExists, 'Physical mview table was deleted from chado');

    $customTableCountAfter = (int) $this->drupal_connection
      ->select('tripal_custom_tables', 'tct')
      ->condition('tct.table_name', $schema['table'])
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals(0, $customTableCountAfter, 'Custom table metadata was removed on delete().');

    $mviewMetaCountAfter = (int) $this->drupal_connection
      ->select('tripal_mviews', 'tm')
      ->condition('tm.name', $schema['table'], '=')
      ->countQuery()
      ->execute()
      ->fetchField();
    $this->assertEquals(0, $mviewMetaCountAfter, 'Mview metadata was removed on delete().');
  }

}
