<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;

/**
 * Testing the tripal_chado/api/tripal_chado.schema.api.inc functions.
 *
 * @group tripal_chado
 */
class SchemaAPITest extends BrowserTestBase {

  protected $defaultTheme = 'stable';

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = ['tripal', 'tripal_chado'];

  /**
   * A user with permission to administer site configuration.
   *
   * @var \Drupal\user\UserInterface
   */
  protected $user;

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    $this->user = $this->drupalCreateUser(['administer site configuration']);
    $this->drupalLogin($this->user);
  }

  /**
   * Tests chado_table_exists() and chado_column_exists().
   *
   * @group wip-lacey
   */
  public function testChadoTableColumnExists() {

    // First create our table in the chado schema (if it exists).
    $check_schema = "SELECT true FROM information_schema.schemata
      WHERE schema_name = 'chado'";
    $exists = \Drupal::database()->query($check_schema)->fetchField();
    if (!$exists) {
      \Drupal::database()->query("CREATE SCHEMA chado");
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
    \Drupal::database()->query($sql);

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

}
