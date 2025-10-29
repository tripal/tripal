<?php

namespace Drupal\Tests\tripal_chado\Functional\ChadoCustomTables;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the base functionality for chado custom tables.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Custom Tables
 */
#[Group('service-chado-custom-table')]
#[RunTestsInSeparateProcesses]
class ChadoCustomTableTest extends ChadoTestBrowserBase {

  /**
   * A serialized schema for testing creation of a custom table.
   *
   * @var string
   */
  private string $table_schema = 'a:6:{s:5:"table";s:4:"file";s:6:"fields";a:4:{s:7:"file_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:6:"serial";s:8:"not null";b:1;}s:4:"name";a:2:{s:4:"type";s:4:"text";s:8:"not null";b:0;}s:7:"type_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:11:"description";a:1:{s:4:"type";s:4:"text";}}s:11:"primary key";a:1:{i:0;s:7:"file_id";}s:11:"unique keys";a:1:{s:7:"file_c1";a:1:{i:0;s:4:"name";}}s:7:"indexes";a:2:{s:9:"file_idx1";a:1:{i:0;s:4:"name";}s:9:"file_idx2";a:1:{i:0;s:7:"type_id";}}s:12:"foreign keys";a:1:{s:6:"cvterm";a:2:{s:5:"table";s:6:"cvterm";s:7:"columns";a:1:{s:7:"type_id";s:9:"cvterm_id";}}}}';

  /**
   * Tests focusing on Chado Custom Tables.
   *
   * @group service manager
   */
  #[Group('service')]
  public function testChadoCustomTables() {
    $manager = \Drupal::service('tripal_chado.custom_tables');
    $this->assertIsObject($manager, 'We were not able to retrieve the custom table service manager.');

    $chado = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);
    $chado_schema_name = $chado->getSchemaName();

    // Test manager get list of chado custom tables.
    $custom_tables = $manager->getTables($chado_schema_name);
    $this->assertIsArray($custom_tables, 'The return value of Custom Table manager getTables is expected to be an array.');
    $this->assertEmpty($custom_tables, 'We just created this test schema so the Custom Table manager should not be able to find any tables yet.');

    // Test manager create. This just creates the object.
    $schema_array = unserialize($this->table_schema, ['allowed_classes' => FALSE]);
    $table_name = $schema_array['table'];
    $custom_table_obj = $manager->create($table_name, $chado_schema_name);
    $this->assertIsObject($custom_table_obj, 'Unable to create a custom table object using the service manager.');
    $this->assertInstanceOf(
      ChadoCustomTable::class,
      $custom_table_obj,
      'Ensure the object the custom table manager created is in fact a custom table object.'
    );
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEmpty($existing_schema, 'A schema should not exist for a newly created custom table object.');
    $sql = "SELECT EXISTS (SELECT 1 FROM information_schema.tables
      WHERE table_schema = '$chado_schema_name'
      AND table_name = '$table_name')";
    $exists = $chado->query($sql)->fetchField();
    $this->assertEquals(0, $exists, 'The custom table should not yet exist in the database');

    // An invalid schema should not be allowed.
    $invalid_schemas = [
      0 => [],
      1 => ['not' => 'valid'],
      2 => ['table' => 'ShouldBeAllLowerCase'],
      3 => [
        'table' => 'goodname',
        'indexes' => [
          'indexnamemustbelessthan60characterslongthisislongerthanthatbyseveralcharacters' => 'test',
        ],
      ],
    ];
    foreach ($invalid_schemas as $index => $invalid_schema) {
      $status = $custom_table_obj->setTableSchema($invalid_schema, FALSE);
      $this->assertFalse($status, "We were able to set the table schema with an invalid schema, index=$index");
    }

    // Test the creation of the custom table in the database, schema is valid.
    $status = $custom_table_obj->setTableSchema($schema_array, FALSE);
    $this->assertTrue($status, 'We were not able to set the table schema with a valid schema.');
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($schema_array, $existing_schema, 'Custom table schema is not as expected.');
    $exists = $chado->query($sql)->fetchField();
    $this->assertEquals(1, $exists, 'The custom table was not created in the database.');
    $table_id = $custom_table_obj->getTableId();
    $this->assertIsInt($table_id, 'The table ID is not an integer.');
    $this->assertGreaterThan(0, $table_id, 'The table ID is not a positive value.');

    // Test manager get list of chado custom tables again now that
    // one has been added.
    $custom_tables = $manager->getTables($chado_schema_name);
    $this->assertIsArray($custom_tables, 'The return value of Custom Table manager getTables is expected to be an array.');
    $this->assertContains($table_name, $custom_tables, 'The newly created custom table was not returned by the manager.');
    // Omitting the schema name should at least return an array.
    $custom_tables = $manager->getTables();
    $this->assertIsArray($custom_tables, 'The return value of Custom Table manager getTables is expected to be an array.');

    // Test manager loadById.
    $custom_table_obj = $manager->loadById(1000000);
    $this->assertNull($custom_table_obj, 'A custom table was returned for an invalid ID.');
    $custom_table_obj = $manager->loadById($table_id);
    $this->assertIsObject($custom_table_obj, 'A custom table was not returned for a valid ID.');
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($existing_schema, $schema_array, 'Custom table schema is not as expected.');

    // Test that the schema in postgresql contains foreign keys (Issue #2335).
    $db_schema_def = $chado->schema()->getTableDef($table_name, ['source' => 'database', 'format' => 'drupal']);
    $this->assertArrayHasKey('foreign keys', $db_schema_def, 'Custom table schema foreign keys were not saved to the database.');
    $this->assertEquals(count($db_schema_def['foreign keys']), count($schema_array['foreign keys']), 'Custom table schema number of foreign keys in the database do not match expected.');
    // Note that the array keys will be different, 'cvterm' in our schema
    // vs. 'file_type_id_fkey' in the db.
    $this->assertEquals(reset($db_schema_def['foreign keys']), reset($schema_array['foreign keys']), 'Custom table schema foreign keys in the database do not match expected.');

    // Test manager find by name.
    $found_id = $manager->findByName('i_do_not_exist');
    $this->assertFalse($found_id, 'Found an ID for a table that does not exist.');
    $found_id = $manager->findByName($table_name);
    $this->assertEquals($table_id, $found_id, 'Did not find the custom table ID by its name.');

    // Test manager load by name.
    $custom_table_obj = $manager->loadByName('i_do_not_exist');
    $this->assertNull($custom_table_obj, 'Returned a custom table object for a table that does not exist.');
    $custom_table_obj = $manager->loadByName($table_name);
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($schema_array, $existing_schema, 'Custom table schema is not as expected.');
    $check_name = $custom_table_obj->getTableName();
    $this->assertEquals($table_name, $check_name, 'The table name is not as expected.');
    $check_schema = $custom_table_obj->getChadoSchema();
    $this->assertEquals($chado_schema_name, $check_schema, 'The table schema is not as expected.');

    // Test creation of an existing table.
    // With force FALSE, nothing should change.
    $schema_array2 = $schema_array;
    $table_name2 = 'file2';
    $schema_array2['table'] = $table_name2;
    $status = $custom_table_obj->setTableSchema($schema_array2, FALSE);
    $this->assertFalse($status, 'We were able to set the table schema with a different name but force is FALSE.');
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($schema_array, $existing_schema, 'Custom table schema is not as expected.');
    // Expect success with force TRUE but table name unchanged.
    $status = $custom_table_obj->setTableSchema($schema_array, TRUE);
    $this->assertTrue($status, 'We were not able to recreate the custom table with the same name.');
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($schema_array, $existing_schema, 'Custom table schema is not as expected.');
    // Expect success with force TRUE and table name changed.
    $status = $custom_table_obj->setTableSchema($schema_array2, TRUE);
    $this->assertTrue($status, 'We were not able to recreate the custom table with a new name.');
    $table_name = $custom_table_obj->getTableName();
    $this->assertEquals($table_name2, $table_name, 'The table name is not as expected.');
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($schema_array2, $existing_schema, 'Custom table schema is not as expected.');

    // Test table locking.
    $locked = $custom_table_obj->isLocked();
    $this->assertFalse($locked, 'Custom table should be unlocked by default.');
    $custom_table_obj->setLocked(TRUE);
    $locked = $custom_table_obj->isLocked();
    $this->assertTrue($locked, 'Custom table should have been locked.');
    $custom_table_obj->setLocked(FALSE);
    $locked = $custom_table_obj->isLocked();
    $this->assertFalse($locked, 'Custom table should have been unlocked.');

    // Test TripalDBX getSchemaDef using all three sources.
    $configs = [
      0 => [
        'table' => 'analysis',
        'source' => NULL,
        'expect' => TRUE,
      ],
      1 => [
        'table' => 'analysis',
        'source' => 'file',
        'expect' => TRUE,
      ],
      2 => [
        'table' => 'analysis',
        'source' => 'tripal',
        'expect' => FALSE,
      ],
      3 => [
        'table' => 'analysis',
        'source' => 'database',
        'expect' => TRUE,
      ],
      4 => [
        'table' => 'analysis',
        'source' => ['file', 'tripal', 'database'],
        'expect' => TRUE,
      ],
      5 => [
        'table' => $table_name,
        'source' => NULL,
        'expect' => FALSE,
      ],
      6 => [
        'table' => $table_name,
        'source' => 'file',
        'expect' => FALSE,
      ],
      7 => [
        'table' => $table_name,
        'source' => 'tripal',
        'expect' => TRUE,
      ],
      8 => [
        'table' => $table_name,
        'source' => 'database',
        'expect' => TRUE,
      ],
      9 => [
        'table' => $table_name,
        'source' => ['file', 'tripal', 'database'],
        'expect' => TRUE,
      ],
      10 => [
        'table' => 'project',
        'source' => ['file', 'tripal', 'database'],
        'format' => 'none',
        'expect' => FALSE,
      ],
    ];
    foreach ($configs as $index => $config) {
      $parameters = [
        'format' => $config['format'] ?? 'drupal',
        'source' => $config['source'],
      ];
      $table_def = $chado->schema()->getTableDef($config['table'], $parameters);
      if ($config['expect']) {
        $this->assertNotEmpty($table_def, "Did not retrieve a table definition for test index $index.");
        $this->assertArrayHasKey('primary key', $table_def, "The table schema should include a primary key for test index $index.");
      }
      else {
        $this->assertEmpty($table_def, "Returned a definition, but did not expect one for test index $index.");
      }
    }

    // Test table deletion.
    // We should be able to call delete() on a table not in chado.
    $table_name3 = 'noschemaadded';
    $table_without_schema = $manager->create($table_name3, $chado_schema_name);
    $status = $table_without_schema->delete();
    $this->assertTrue($status, 'We were able to call delete on a table without a schema.');

    // This will delete the custom table both in DB and in custom table list.
    $status = $custom_table_obj->delete();
    $exists = $chado->query($sql)->fetchField();
    $this->assertEquals(0, $exists, 'The custom table should no longer exist in the database.');
    $deleted_table_id = $custom_table_obj->getTableId();
    $this->assertEquals(0, $deleted_table_id, 'The ID for the deleted table has not been set to zero.');
    $status = $custom_table_obj->setTableSchema($schema_array, FALSE);
    $this->assertFalse($status, 'We should not have been able to set the schema on a fully deleted table object.');

    // Test NULLS NOT DISTINCT.
    $schema_array4 = $schema_array;
    $table_name4 = 'file4';
    $schema_array4['table'] = $table_name4;
    $schema_array4['nulls not distinct'] = [
      'file_c1' => TRUE,
      'notexist' => TRUE,
      'ignore' => FALSE,
    ];
    $table_name = $schema_array4['table'];
    $custom_table_obj4 = $manager->create($table_name, $chado_schema_name);
    $this->assertIsObject($custom_table_obj4, 'Unable to create a custom table object using the service manager.');
    $status = $custom_table_obj4->setTableSchema($schema_array4, FALSE);
    $this->assertTrue($status, 'We were not able to set the table schema with a nulls not distinct schema.');
    $existing_schema = $custom_table_obj4->getTableSchema();
    $this->assertEquals($schema_array4, $existing_schema, 'Custom table schema is not as expected.');
    $sql = "SELECT EXISTS (SELECT 1 FROM information_schema.tables
      WHERE table_schema = '$chado_schema_name'
      AND table_name = '$table_name4')";
    $exists = $chado->query($sql)->fetchField();
    $this->assertEquals(1, $exists, 'The custom table was not created in the database.');

    // The 'name' column is set up with a unique constraint, but it is
    // nullable. With this test we should not be able to insert two null
    // records because we added 'nulls not distinct'.
    // Since this is only supported on Postgresql >= 15, we will not
    // expect an exception for versions < 15.
    $psql_version = $chado->version();
    // Remove distro info, e.g. "13.22 (Debian 13.22-1.pgdg12+1)" -> "13.22".
    $psql_version = preg_replace('/[^\d\.].*$/', '', $psql_version);
    $expect_exception = TRUE;
    if (version_compare($psql_version, '15.0') < 0) {
      $expect_exception = FALSE;
    }

    $sql = "INSERT INTO $table_name4 (name, type_id, description) VALUES (NULL, 1, 'Dup')";
    $result1 = $chado->query($sql);
    $threw_exception = FALSE;
    try {
      $result2 = $chado->query($sql);
    }
    catch (\Exception $e) {
      $threw_exception = TRUE;
    }
    $this->assertEquals($expect_exception, $threw_exception, 'Unexpected exception status adding a second null record under postgresql version ' . $psql_version);
  }

}
