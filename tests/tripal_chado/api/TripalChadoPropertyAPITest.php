<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoPropertyAPITest extends TripalTestCase {

  use DBTransaction;

  /**
   * @group chado
   * @group api
   *
   */
  public function test_chado_insert_property() {

    $feature = factory('chado.feature')->create();
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => 'feature', 'id' => $feature->feature_id];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    $result = db_select('chado.featureprop', 'f')
      ->fields('f')
      ->condition('f.feature_id', $feature->feature_id)
      ->execute()
      ->fetchObject();

    $this->assertNotEmpty($result);
    $this->assertEquals($value, $result->value);
    $this->assertEquals($term->cvterm_id, $result->type_id);
    $this->assertEquals('0', $result->rank);

  }


  /**
   * @group chado
   * @group api
   * @group wip
   *
   */
  public function test_chado_get_property() {

    $feature = factory('chado.feature')->create();
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => 'feature', 'id' => $feature->feature_id];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    $prop = chado_insert_property($record, $property);
    $retrieved = chado_get_property($record, $property);
    $this->assertNotFalse($retrieved);
    $this->assertEquals($value, $retrieved->value);

    $record = ['prop_id' => $prop['featureprop_id'], 'table' => 'feature'];
    $retrieved = chado_get_property($record, $property);
    $this->assertNotFalse($retrieved);
    $this->assertEquals($value, $retrieved->value);
  }

  /**
   * @group chado
   * @group api
   */
  public function test_chado_update_property() {
    $feature = factory('chado.feature')->create();
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';
    $new_value = 'chado_API_new';

    // Linker column
    $record = ['table' => 'feature', 'id' => $feature->feature_id];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    $property['value'] = $new_value;

    chado_update_property($record, $property);


    $result = db_select('chado.featureprop', 'f')
      ->fields('f')
      ->condition('f.feature_id', $feature->feature_id)
      ->execute()
      ->fetchObject();

    $this->assertNotEmpty($result);
    $this->assertEquals($new_value, $result->value);
    $this->assertEquals($term->cvterm_id, $result->type_id);
    $this->assertEquals('0', $result->rank);

  }

  /**
   * @group chado
   * @group api
   */
  public function test_chado_delete_property() {
    $feature = factory('chado.feature')->create();
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => 'feature', 'id' => $feature->feature_id];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    chado_delete_property($record, $property);

    $result = db_select('chado.featureprop', 'f')
      ->fields('f')
      ->condition('f.feature_id', $feature->feature_id)
      ->execute()
      ->fetchObject();

    $this->assertFalse($result);


    $prop = chado_insert_property($record, $property);

    $record = ['prop_id' => $prop['featureprop_id'], 'table' => 'feature'];
    chado_delete_property($record, $property);
  }


  /**
   * @group wip
   * @group chado
   * @group api
   */
  function test_chado_get_record_with_property() {
    //  * Get all records in the base table assigned one or more properties.

    $feature = factory('chado.feature')->create();
    $term = factory('chado.cvterm')->create();

    $value = 'chado_API_test_value';

    // Linker column
    $record = ['table' => 'feature', 'id' => $feature->feature_id];
    $property = [
      'type_id' => $term->cvterm_id,
      'value' => $value,
    ];

    chado_insert_property($record, $property);

    unset($record['id']);
    $records = chado_get_record_with_property($record, $property);

    $this->assertNotEmpty($records);
    $this->assertEquals(1, count($records));

    $feature = factory('chado.feature')->create();
    $record = ['table' => 'feature', 'id' => $feature->feature_id];
    chado_insert_property($record, $property);
    $records = chado_get_record_with_property($record, $property);

    $this->assertNotEmpty($records);
    $this->assertEquals(2, count($records));
  }


}
