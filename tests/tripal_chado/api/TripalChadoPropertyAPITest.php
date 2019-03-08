<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use StatonLab\TripalTestSuite\Database\Factory;

class TripalChadoPropertyAPITest extends TripalTestCase {

  use DBTransaction;

  /**
   * Tests chado_insert_property() with all prop tables.
   *
   * @dataProvider propTableProvider
   *
   * @group chado
   * @group api
   * @group chado-property
   */
  public function test_chado_insert_property($prop_table, $base_table) {

    $base_record = factory('chado.' . $base_table)->create();
    $base_pkey = $base_table . '_id';
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => $base_table, 'id' => $base_record->{$base_pkey}];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    $result = db_select('chado.' . $prop_table, 'p')
      ->fields('p')
      ->condition('p.' . $base_pkey, $base_record->{$base_pkey})
      ->execute()
      ->fetchObject();

    $this->assertNotEmpty($result);
    $this->assertEquals($value, $result->value);
    $this->assertEquals($term->cvterm_id, $result->type_id);
    $this->assertEquals('0', $result->rank);

  }


  /**
   * Tests chado_get_property() with all prop tables.
   *
   * @dataProvider propTableProvider
   *
   * @group chado
   * @group api
   * @group chado-property
   */
  public function test_chado_get_property($prop_table, $base_table) {

    $base_record = factory('chado.' . $base_table)->create();
    $base_pkey = $base_table . '_id';
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => $base_table, 'id' => $base_record->{$base_pkey}];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    $prop = chado_insert_property($record, $property);
    $retrieved = chado_get_property($record, $property);
    $this->assertNotFalse($retrieved);
    $this->assertEquals($value, $retrieved->value);

    $record = ['prop_id' => $prop[$prop_table . '_id'], 'table' => $base_table];
    $retrieved = chado_get_property($record, $property);

    $this->assertNotNull($retrieved);
    $this->assertEquals($value, $retrieved->value);
  }

  /**
   * Tests chado_update_property() with all prop tables.
   *
   * @dataProvider propTableProvider
   *
   * @group chado
   * @group api
   * @group chado-property
   */
  public function test_chado_update_property($prop_table, $base_table) {

    $base_record = factory('chado.' . $base_table)->create();
    $base_pkey = $base_table . '_id';
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';
    $new_value = 'chado_API_new';

    // Linker column
    $record = ['table' => $base_table, 'id' => $base_record->{$base_pkey}];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    $property['value'] = $new_value;

    chado_update_property($record, $property);


    $result = db_select('chado.' . $prop_table, 'p')
      ->fields('p')
      ->condition('p.' . $base_pkey, $base_record->{$base_pkey})
      ->execute()
      ->fetchObject();

    $this->assertNotEmpty($result);
    $this->assertEquals($new_value, $result->value);
    $this->assertEquals($term->cvterm_id, $result->type_id);
    $this->assertEquals('0', $result->rank);

  }

  /**
   * Tests chado_delete_property() with all prop tables.
   *
   * @dataProvider propTableProvider
   *
   * @group chado
   * @group api
   * @group chado-property
   */
  public function test_chado_delete_property($prop_table, $base_table) {

    $base_record = factory('chado.' . $base_table)->create();
    $base_pkey = $base_table . '_id';
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => $base_table, 'id' => $base_record->{$base_pkey}];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    chado_delete_property($record, $property);

    $result = db_select('chado.' . $prop_table, 'p')
      ->fields('p')
      ->condition('p.' . $base_pkey, $base_record->{$base_pkey})
      ->execute()
      ->fetchObject();

    $this->assertFalse($result);


    $prop = chado_insert_property($record, $property);

    $record = ['prop_id' => $prop[$prop_table . '_id'], 'table' => $base_table];
    chado_delete_property($record, $property);
  }


  /**
   * Tests chado_get_record_with_property() with all prop tables.
   *
   * Note: chado_get_record_with_property() gets all records in the base table
   *   assigned one or more properties.
   *
   * @dataProvider propTableProvider
   *
   * @group chado
   * @group api
   * @group chado-property
   */
  function test_chado_get_record_with_property($prop_table, $base_table) {

    $base_record = factory('chado.' . $base_table)->create();
    $base_pkey = $base_table . '_id';
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => $base_table, 'id' => $base_record->{$base_pkey}];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    unset($record['id']);
    $records = chado_get_record_with_property($record, $property);

    $this->assertNotEmpty($records);
    $this->assertEquals(1, count($records));

    $base_record = factory('chado.' . $base_table)->create();
    $record = ['table' => $base_table, 'id' => $base_record->{$base_pkey}];
    chado_insert_property($record, $property);
    $records = chado_get_record_with_property($record, $property);

    $this->assertNotEmpty($records);
    $this->assertEquals(2, count($records));
  }

  /**
   * Data Provider: All base tables with associated property tables.
   *
   * @return
   *   An array where each item specifies the property table
   *   and it's associated base table.
   */
  function propTableProvider() {
    $prop_tables = [];

    $base_tables = chado_get_base_tables();
    foreach ($base_tables as $base) {
      $prop = $base . 'prop';
      if (chado_table_exists($prop) AND Factory::exists('chado.' . $base)) {
        $prop_tables[] = [$prop, $base];
      }
    }

    return $prop_tables;
  }
}
