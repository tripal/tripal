<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;
use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests the base functionality for chado custom tables.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Custom Tables
 */
#[Group('Tripal')]
#[Group('Tripal Chado')]
#[Group('Tripal Chado Custom Tables')]
class ChadoCustomTableTest extends ChadoTestBrowserBase {

  /**
   * A serialized schema for testing creation of a custom table.
   *
   * @var string
   */
  private string $table_schema = 'a:6:{s:5:"table";s:4:"file";s:6:"fields";a:4:{s:7:"file_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:6:"serial";s:8:"not null";b:1;}s:4:"name";a:2:{s:4:"type";s:4:"text";s:8:"not null";b:1;}s:7:"type_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:11:"description";a:1:{s:4:"type";s:4:"text";}}s:11:"primary key";a:1:{i:0;s:7:"file_id";}s:11:"unique keys";a:1:{s:7:"file_c1";a:1:{i:0;s:4:"name";}}s:7:"indexes";a:2:{s:9:"file_idx1";a:1:{i:0;s:4:"name";}s:9:"file_idx2";a:1:{i:0;s:7:"type_id";}}s:12:"foreign keys";a:1:{s:6:"cvterm";a:2:{s:5:"table";s:6:"cvterm";s:7:"columns";a:1:{s:7:"type_id";s:9:"cvterm_id";}}}}';

  /**
   * Tests focusing on Chado Custom Tables.
   *
   * @group service manager
   */
  #[Group('service manager')]
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
    $schema_array = unserialize($this->table_schema);
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

    // Test the creation of the custom table in the database.
    $custom_table_obj->setTableSchema($schema_array, FALSE);
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($existing_schema, $schema_array, 'Custom table schema is not as expected.');
    $exists = $chado->query($sql)->fetchField();
    $this->assertEquals(1, $exists, 'The custom table was not created in the database');
    $table_id = $custom_table_obj->getTableId();
    $this->assertIsInt($table_id, 'The table ID is not an integer');
    $this->assertGreaterThan(0, $table_id, 'The table ID is not a positive value');

    // Test manager get list of chado custom tables again now that one has been added.
    $custom_tables = $manager->getTables($chado_schema_name);
    $this->assertIsArray($custom_tables, 'The return value of Custom Table manager getTables is expected to be an array.');
    $this->assertContains($table_name, $custom_tables, 'The newly created custom table was not returned by the manager.');

    // Test manager loadById.
    $custom_table_obj = $manager->loadById(1000000);
    $this->assertNull($custom_table_obj, 'A custom table was returned for an invalid ID');
    $custom_table_obj = $manager->loadById($table_id);
    $this->assertIsObject($custom_table_obj, 'A custom table was not returned for a valid ID');
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($existing_schema, $schema_array, 'Custom table schema is not as expected.');

    // Test manager find by name.
    $found_id = $manager->findByName('i_do_not_exist');
    $this->assertFalse($found_id, 'Found an ID for a table that does not exist');
    $found_id = $manager->findByName($table_name);
    $this->assertEquals($table_id, $found_id, 'Did not find the custom table ID by its name');

    // Test manager load by name.
    $custom_table_obj = $manager->loadByName($table_name);
    $existing_schema = $custom_table_obj->getTableSchema();
    $this->assertEquals($existing_schema, $schema_array, 'Custom table schema is not as expected.');

    // Test table locking.
    $locked = $custom_table_obj->isLocked();
    $this->assertFalse($locked, 'Custom table should be unlocked by default');
    $custom_table_obj->setLocked(TRUE);
    $locked = $custom_table_obj->isLocked();
    $this->assertTrue($locked, 'Custom table should have been locked');
    $custom_table_obj->setLocked(FALSE);
    $locked = $custom_table_obj->isLocked();
    $this->assertFalse($locked, 'Custom table should have been unlocked');

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
        $this->assertNotEmpty($table_def, "Did not retrieve a table definition for test index $index");
        $this->assertArrayHasKey('primary key', $table_def, "The table schema should include a primary key for test index $index");
      }
      else {
        $this->assertEmpty($table_def, "Returned a definition, but did not expect one for test index $index");
      }
    }
  }

}
