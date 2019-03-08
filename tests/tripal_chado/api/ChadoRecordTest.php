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
    $record_column = $table . '_id';

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
    $record_column = $table . '_id';
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
    $record_column = $table . '_id';
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
    $record_column = $table . '_id';
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
    $record_column = $table . '_id';
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
   *
   *
   * @group chado
   * @group api
   *
   * @dataProvider recordProvider
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

  /**
   * Save should work for both an update and an insert
   *
   * @group chado
   * @group api
   *
   * @dataProvider  recordProvider
   */
  public function testSave($table, $values) {
    //first, test the insert case
    $record = new \ChadoRecord($table);
    $record->setValues($values);
    $record->save();
    $record_column = $table . '_id';

    $query = db_select('chado.' . $table, 't')
      ->fields('t', [$record_column]);
    foreach ($values as $key => $val) {
      $query->condition($key, $val);
    }
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result, 'we couldnt insert our record on a save!');

    //change the last key
    //NOTE this will break if the last key isn't a string!
    $values[$key] = 'new_value_that_i_wantTOBEUNIQUE';
    $record->setValues($values);
    $record->save();

    $query = db_select('chado.' . $table, 't')
      ->fields('t', [$record_column]);
    foreach ($values as $key => $val) {
      $query->condition($key, $val);
    }
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result, 'Our record wasnt updated when saving!');
  }

  /**
   *
   * @group chado
   * @group api
   *
   * @dataProvider  recordProvider
   */
  public function testInsert($table, $values) {
    //first, test the insert case
    $record = new \ChadoRecord($table);
    $record->setValues($values);
    $record->insert();
    $record_column = $table . '_id';

    $query = db_select('chado.' . $table, 't')
      ->fields('t', [$record_column]);
    foreach ($values as $key => $val) {
      $query->condition($key, $val);
    }
    $result = $query->execute()->fetchAll();
    $this->assertNotEmpty($result, 'we couldnt insert our record on a save!');

    //If we insert again, it should fail
    $this->expectException(EXCEPTION);
    $record->insert();
  }

  /**
   *
   * @group chado
   * @group api
   *
   * @dataProvider  recordProvider
   */
  public function testUpdate($table, $values) {
    $id = $this->genChadoRecord($table, $values);
    $record = new \ChadoRecord($table, $id);
    $record_column = $table . '_id';

    //$dump_vals = $record->getValues();
    // var_dump($dump_vals);

    $key = array_keys($values)[0];
    $string = 'some_random_new_string34792387';
    $values[$key] = $string;

    $record->setValues($values);
    $record->update();

    //$dump_vals = $record->getValues();
    // var_dump($dump_vals);

    $query = db_select('chado.' . $table, 't')
      ->fields('t', [$key]);
    foreach ($values as $key => $val) {
      $query->condition($key, $val);
    }
    $result = $query->execute()->fetchField();
    $this->assertNotFalse($result, 'we couldnt update our record.');
    $this->assertEquals($string, $result);
  }


  /**
   *
   * @group chado
   * @group api
   *
   * @dataProvider recordProvider
   *
   */
  public function testDelete($table, $values) {
    $id = $this->genChadoRecord($table, $values);
    $record = new \ChadoRecord($table, $id);
    $record_column = $table . '_id';

    $record->delete();
    $query = db_select('chado.' . $table, 't')
      ->fields('t', [$record_column]);
    foreach ($values as $key => $val) {
      $query->condition($key, $val);
    }
    $result = $query->execute()->fetchAll();
    $this->assertEmpty($result, 'we couldnt delete our record!');
  }


  private function genChadoRecord($table, $values) {
    $chado_record = factory('chado.' . $table)->create($values);
    $record_column = $table . '_id';
    $id = $chado_record->$record_column;
    return $id;
  }

}
