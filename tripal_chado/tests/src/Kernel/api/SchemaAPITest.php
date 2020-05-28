<?php

namespace Drupal\Tests\tripal_chado\Kernel;

use Drupal\Core\Url;
use Drupal\KernelTests\KernelTestBase;
use Drupal\Core\Database\Database;

/**
 * Testing the tripal_chado/api/tripal_chado.schema.api.inc functions.
 *
 * @group tripal_chado
 */
class SchemaAPITest extends KernelTestBase {

  protected $defaultTheme = 'stable';

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = ['tripal', 'tripal_chado'];

  /**
   * Tests chado_table_exists() and chado_column_exists().
   *
   * @group tripal-chado
   * @group chado-schema
   */
  public function testChadoTableColumnExists() {
    $connection = \Drupal\Core\Database\Database::getConnection();

    // First create our table in the chado schema (if it exists).
    $check_schema = "SELECT true FROM pg_namespace WHERE nspname = 'chado'";
    $exists = $connection->query($check_schema)->fetchField();
    if (!$exists) {
      $this->markTestSkipped('Cannot check chado schema api without chado.');
    }

    // Define a table name which cannot exist.
    $table_name = 'testChadoTableExists_' . uniqid();

    // Check that the table does not exist.
    $result = chado_table_exists($table_name);
    $this->assertFalse($result,
      "The table should NOT exists because we haven't created it yet.");

    // Now create the table.
    $sql = "CREATE TABLE chado." . $table_name . " (
        cte_id     SERIAL PRIMARY KEY,
        cte_name    varchar(40)
    )";
    $connection->query($sql);

    // And check that the table is there.
    $result = chado_table_exists($table_name);
    $this->assertTrue($result,
      "The table should exists because we just created it.");

    // -- COLUMNS.
    // Now check that a column NOT in the table is properly detected.
    $column = 'columndoesnotexist';
    $result = chado_column_exists($table_name, $column);
    $this->assertFalse($result,
      "The column does not exist in the table.");

    // Now check that a column in the table is properly detected.
    $column = 'cte_name';
    $result = chado_column_exists($table_name, $column);
    $this->assertTRUE($result,
      "The column does exist in the table but we were not able to detect it.");

    // -- SEQUENCE.
    // Now check for the sequence which allows the primary key to autoincrement.
    $sequence_name = strtolower($table_name . '_cte_id_seq');
    $result = chado_sequence_exists($sequence_name);
    $this->assertTRUE($result,
      "The sequence should exist for the primary key.");

    // There is no sequence on the name so lets confirm that.
    $sequence_name = strtolower($table_name . '_cte_name_seq');
    $result = chado_sequence_exists($sequence_name);
    $this->assertFALSE($result,
      "The sequence should NOT exist for the name.");

    // -- INDEX.
    // Now check for the index on the primary key.
    $result = chado_index_exists($table_name, 'pkey', TRUE);
    $this->assertTRUE($result,
      "The index should exist for the primary key.");

    // There is no index on the name so lets confirm that.
    $index = strtolower($table_name . '_cte_name_idx');
    $result = chado_index_exists($table_name, 'cte_name', $index);
    $this->assertFALSE($result,
      "The index should NOT exist for the name.");

    // -- ADD INDEX.
    // We've already proven there is no index on the name.
    // Now we are going to add one!
    $success = chado_add_index($table_name, '_someindexname', ['cte_name']);
    $result = chado_index_exists($table_name, '_someindexname');
    $this->assertTrue($result,
      "The index we just created should be available.");

    // Clean up after ourselves by dropping the table.
    \Drupal::database()->query("DROP TABLE chado." . $table_name);
  }

  /**
   * Tests chado_get_schema_name().
   *
   * @group tripal-chado
   * @group chado-schema
   */
  public function testChadoSchemaMetdata() {

    // First check the default schema.
    $schema_name = chado_get_schema_name('fred');
    $this->assertEquals('public', $schema_name,
      "The default schema is not what we expected. We expected the 'public' schema.");

    // Next check if chado is local.
    $is_local = chado_is_local();
    $this->assertIsBool($is_local, "Unable to check that chado is local.");
    $is_local_2X = chado_is_local();
    $this->assertIsBool($is_local_2X, "Unable to check that chado is local 2X.");
    $this->assertEquals($is_local, $is_local_2X,
      "When checking if chado is local we didn't get the same answer twice.");

    // Check if chado is installed.
    $installed = chado_is_installed();
    $this->assertTrue($installed, "Chado is not installed?");

    // Check the chado version.
    $version = chado_get_version();
    $this->assertGreaterThanOrEqual(1.3, $version,
      "We were unable to detect the version assuming it's 1.3");
  }
}
