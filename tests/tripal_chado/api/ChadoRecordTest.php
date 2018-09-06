<?php

namespace Tests;

use PHPUnit\Exception;
use Faker\Factory;
use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_chado', 'includes/api/ChadoRecord');


class ChadoRecordTest extends TripalTestCase {

  use DBTransaction;


  /**
   * Data provider.  A variety of chado records.
   *
   * @return array
   */
  public function recordProvider() {
    //table, factory or NULL, record_id or NULL

    $faker = \Faker\Factory::create();
    $analysis = [
      'name' => $faker->word,
      'description' => $faker->text,
      'program' => $faker->word,
      'programversion' => $faker->word,
    ];
    $organism = [
      'genus' => $faker->word,
      'species' => $faker->word,
      'common_name' => $faker->word,
    ];

    return [
      ['analysis', $analysis],
      ['organism', $organism],
    ];
  }

  /**
   * Tests that the class can be initiated with or without a record specified
   *
   * @group api
   * @group chado
   * @group wip
   * @dataProvider recordProvider
   */
  public function testInitClass($table, $values) {
    $record = new \ChadoRecord($table);
    $this->assertNotNull($record);
    $chado_record = factory('chado.' . $table)->create($values);
    $record_column = $table.'_id';
    $record = new \ChadoRecord($table, $chado_record->$record_column);
    $this->assertNotNull($record);
  }


  /**
   * @group api
   * @group chado
   * @group wip
   * @throws \Exception
   * @dataProvider recordProvider
   */

  public function testGetTable($table, $values) {
    $record = new \ChadoRecord($table);
    $this->assertEquals($table, $record->getTable());
  }

  /**
   * @group wip
   * @group api
   * @group chado
   * @dataProvider recordProvider
   *
   * @throws \Exception
   */
  public function testGetID($table, $values) {
    $chado_record = factory('chado.' . $table)->create();
    $record_column = $table.'_id';
    $id = $chado_record->$record_column;

    $record = new \ChadoRecord($table, $id);
    $returned_id = $record->getID();
    $this->assertEquals($id, $returned_id);
  }

  /**
   * @group api
   * @group wip
   * @group chado
   * @dataProvider recordProvider
   *
   *
   */
  public function testGetValues($table, $values) {
    $chado_record = factory('chado.' . $table)->create($values);
    $record_column = $table.'_id';
    $id = $chado_record->$record_column;
    $record = new \ChadoRecord($table, $id);

    $values = $record->getValues();
    $this->assertNotEmpty($values);
    foreach ($values as $key => $value) {
      $this->assertArrayHasKey($key, $values);
      $this->assertEquals($value, $values[$key]);
    }
  }

  /**
   * @group api
   * @group wip
   * @group chado
   * @dataProvider recordProvider
   *
   */
  public function testGetValue($table, $values) {

    $chado_record = factory('chado.' . $table)->create($values);
    $record_column = $table.'_id';
    $id = $chado_record->$record_column;

    $record = new \ChadoRecord($table, $id);
    foreach ($values as $key => $value) {
      $returned_value = $record->getValue($key);
      $this->assertEquals($value, $returned_value);
    }
  }

  /**
   * @group wip
   * @group chado
   * @group api
   * @dataProvider recordProvider
   */

  public function testFind($table, $values) {

    $chado_record = factory('chado.' . $table)->create($values);
    $record_column = $table.'_id';
    $id = $chado_record->$record_column;
    
    $record = new \ChadoRecord($table);

    $record->setValues($values);
    $found = $record->find();

    $this->assertNotNull($found);
    $this->assertEquals(1, $found);

  }

  /**
   * Check that the find method throws an exception when it cant find anything.
   *
   * @throws \Exception
   */

  public function testFindFail() {
    $table = 'organism';
    $record = new \ChadoRecord($table);

    $record->setValue($table . '_id', 'unfindable');
    $this->expectException(Exception);
    $found = $record->find();
  }

  /**
   * @param $table
   * @param $values
   *
   * @throws \Exception
   */
  public function testSetandGetValue($table, $values) {

    $record = new \ChadoRecord($table);
    $record->setValues($values);
    $vals = $record->getValues();

    foreach ($vals as $val_key => $val) {
      $this->assertEquals($values[$val_key], $val, "The getValues did not match what was provided for setValues");
    }
  }

}
