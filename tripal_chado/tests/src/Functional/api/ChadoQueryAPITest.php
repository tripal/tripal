<?php

namespace Drupal\Tests\tripal_chado;

use Drupal\Core\Url;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;

/**
 * Testing the tripal_chado/api/tripal_chado.query.api.php functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 */
class ChadoQueryAPITest extends ChadoTestKernelBase {

  protected $defaultTheme = 'stark';

  protected $connection;

  /**
   * Modules to enable.
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests chado_query().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoQuery() {
    $connection = \Drupal\Core\Database\Database::getConnection();

    // Check that chado exists.
    $check_schema = "SELECT true FROM pg_namespace WHERE nspname = :schema";
    $exists = $connection->query($check_schema, [':schema' => $this->testSchemaName])
      ->fetchField();
    $this->assertEquals(1, $exists, 'Cannot check chado schema api without chado.
      Please ensure chado is installed in the schema named "testchado".');

    // --------------
    // Check that errors are thrown if the correct parameters are not supplied.
    // -- SQL must be a string.
    $sql = $args =  ['Fred', 'Sarah', 'Jane'];
    $dbq = chado_query($sql, $args);
    $this->assertEquals(FALSE, $dbq);

    // -- Arguments must be an array.
    $sql = $args = 'SELECT * FROM {organism}';
    $dbq = chado_query($sql, $args);
    $this->assertEquals(FALSE, $dbq);

    // -- Arguments should be in the SQL string.
    $sql = 'SELECT * FROM {organism} WHERE genus=:genus';
    $args = [':genus' => 'Tripalus', ':species' => 'databasica'];
    $dbq = chado_query($sql, $args);
    $this->assertEquals(FALSE, $dbq);
    array_shift($args);
    $dbq = chado_query($sql, $args);
    $this->assertEquals(FALSE, $dbq);

    // --------------
    // Now check that a correctly formatted insert query works.
    $sql = 'INSERT INTO {organism}
      (genus, species, type_id, infraspecific_name, common_name, abbreviation)
      VALUES (:genus, :species, :type_id, :infra, :common, :abbrev)';
    $args = [
      ':genus' => 'Tripalus',
      ':species' => 'databasica' . uniqid(),
      ':type_id' => 2, //version
      ':infra' => 'Quad',
      ':common' => 'Cultivated Tripal',
      ':abbrev' => 'T. databasica',
    ];
    $dbq = chado_query($sql, $args, [], 'testchado');
    $this->assertNotEquals(FALSE, $dbq, 'chado_query() unable to insert.');
    // Now select to ensure it was actually inserted.
    $result = $connection->query('SELECT * FROM testchado.organism
      WHERE genus=:g AND species=:s',
      [':g' => $args[':genus'], ':s' => $args[':species']])->fetchObject();
    $this->assertIsObject($result);
    $this->assertEquals($args[':species'], $result->species);

    // Now check we can select it using chado_query().
    $resource = chado_query('SELECT * FROM {organism}
      WHERE genus=:g AND species=:s',
      [':g' => $args[':genus'], ':s' => $args[':species']], [], 'testchado');
    $this->assertIsObject($resource, 'chado_query() unable to select.');
    $result_cq = $resource->fetchObject();
    $this->assertIsObject($result_cq, 'Should be able to fetch result.');
    $this->assertEquals($args[':species'], $result_cq->species);
    $this->assertEquals($result, $result_cq);

    // Update it using chado_query().
    $sql = 'UPDATE {organism} SET abbreviation = :new WHERE species = :s';
    $resource = chado_query($sql,
      [':new' => 'CHANGED', ':s' => $args[':species']], [], 'testchado');
    $this->assertIsObject($resource, 'chado_query() unable to update.');
    // Now select to ensure it was actually inserted.
    $result = $connection->query('SELECT * FROM testchado.organism
      WHERE genus=:g AND species=:s',
      [':g' => $args[':genus'], ':s' => $args[':species']])->fetchObject();
    $this->assertIsObject($result);
    $this->assertEquals($args[':species'], $result->species);
    $this->assertEquals('CHANGED', $result->abbreviation);

    // Then delete it using chado_query().
    $sql = 'DELETE FROM {organism} WHERE species = :s';
    $resource = chado_query($sql,
      [':s' => $args[':species']], [], 'testchado');
    $this->assertNotFalse($resource, 'chado_query() unable to delete.');
    // Now select to ensure it was actually deleted.
    $result = $connection->query('SELECT * FROM testchado.organism
      WHERE genus=:g AND species=:s',
      [':g' => $args[':genus'], ':s' => $args[':species']])->fetchObject();
    $this->assertIsNotObject($result);
  }

  /**
   * Tests chado_insert(), chado_select(), chado_update(), and chado_delete().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoQueryHelpers() {
    $connection = \Drupal\Core\Database\Database::getConnection();

    // Check that chado exists.
    $check_schema = "SELECT true FROM pg_namespace WHERE nspname = :schema";
    $exists = $connection->query($check_schema, [':schema' => $this->testSchemaName])
      ->fetchField();
    $this->assertEquals(1, $exists, 'Cannot check chado schema api without chado.
      Please ensure chado is installed in the schema named "testchado".');

    // INSERT.
    $values = [
      'genus' => 'Tripalus',
      'species' => 'ferox' . uniqid(),
      'type_id' => 2, //version
      'infraspecific_name' => 'Quad',
      'common_name' => 'Wild Tripal',
      'abbreviation' => 'T. ferox',
    ];
    $dbq = chado_insert_record('organism', $values, [], 'testchado');
    $this->assertNotEquals(FALSE, $dbq, 'chado_insert_record() unable to insert.');
    // Now select to ensure it was actually inserted.
    $result = $connection->query('SELECT * FROM testchado.organism
      WHERE genus=:g AND species=:s',
      [':g' => $values['genus'], ':s' => $values['species']])->fetchObject();
    $this->assertIsObject($result);
    $this->assertEquals($values['species'], $result->species);


    // SELECT.
    $resource = chado_select_record(
      'organism', ['*'], $values, [], 'testchado');
    $this->assertIsArray($resource, 'chado_select_record() unable to select.');
    $this->assertNotEmpty($resource, 'No results were returned.');
    $result_cq = $resource[0];
    $this->assertIsObject($result_cq, 'Should be able to fetch result.');
    $this->assertEquals($values['species'], $result_cq->species);
    $this->assertEquals($result, $result_cq);

    // UPDATE.
    $sql = 'UPDATE {organism} SET abbreviation = :new WHERE species = :s';
    $resource = chado_update_record(
      'organism', $values, ['abbreviation' => 'CHANGED'], [], 'testchado');
    $this->assertTrue($resource, 'chado_update_record() unable to update.');
    // Now select to ensure it was actually inserted.
    $result = $connection->query('SELECT * FROM testchado.organism
      WHERE genus=:g AND species=:s',
      [':g' => $values['genus'], ':s' => $values['species']])->fetchObject();
    $this->assertIsObject($result);
    $this->assertEquals($values['species'], $result->species);
    $this->assertEquals('CHANGED', $result->abbreviation);

    // DELETE.
    unset($values['abbreviation']);
    $resource = chado_delete_record('organism', $values, [], 'testchado');
    $this->assertNotFalse($resource, 'chado_delete_record() unable to delete.');
    // Now select to ensure it was actually deleted.
    $result = $connection->query('SELECT * FROM testchado.organism
      WHERE genus=:g AND species=:s',
      [':g' => $values['genus'], ':s' => $values['species']])->fetchObject();
    $this->assertIsNotObject($result);
  }

  /**
   * Tests chado_get_table_max_rank().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoTableMaxRank() {
    $connection = \Drupal\Core\Database\Database::getConnection();
    $this->markTestIncomplete('This test has not been implemented yet.');
  }

  /**
   * Tests chado_set_active().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoSetActive() {
    $connection = \Drupal\Core\Database\Database::getConnection();
    $this->markTestIncomplete('This test has not been implemented yet.');
  }

  /**
   * Tests chado_pager_query() and chado_pager_get_count().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoPagerQuery() {
    $connection = \Drupal\Core\Database\Database::getConnection();
    $this->markTestIncomplete('This test has not been implemented yet.');
  }

  /**
   * Tests chado_schema_get_foreign_key().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoSchemaGetFK() {
    $connection = \Drupal\Core\Database\Database::getConnection();
    $this->markTestIncomplete('This test has not been implemented yet.');
  }

}
