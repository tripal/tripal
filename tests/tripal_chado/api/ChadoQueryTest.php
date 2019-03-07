<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class ChadoQueryTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group filter
   * See PR 827.
   */
  public function test_filter_level() {
    $stock = factory('chado.stock')->create(['uniquename' => 'octopus_core_test_name']);

    // Test 1. Pass a single filter.
    $selector = [
      'stock_id' => $stock->stock_id,
      'uniquename' => [
        'op' => 'LIKE',
        'data' => 'octopus%',
      ],
    ];

    $object = chado_generate_var('stock', $selector);

    $this->assertNotNull($object->stock_id);
    $this->assertEquals($stock->stock_id, $object->stock_id);

    // Test 2 Pass an array of filters with a single item.
    $selector = [
      'stock_id' => $stock->stock_id,
      'uniquename' => [
        [
          'op' => 'LIKE',
          'data' => 'octopus%',
        ],
      ],
    ];
    $object = chado_generate_var('stock', $selector);

    $this->assertNotNull($object->stock_id);
    $this->assertEquals($stock->stock_id, $object->stock_id);

    // Test 3 Pass an array of filters with multiple items.
    $selector = [
      'type_id' => [
        [
          'op' => '>',
          'data' => ($stock->type_id - 1),
        ],
        [
          'op' => '<',
          'data' => ($stock->type_id + 1),
        ],
      ],
    ];

    $object = chado_generate_var('stock', $selector);
    $this->assertNotNull($object->stock_id);
    $this->assertEquals($stock->stock_id, $object->stock_id);
  }

  /**
   * @group api
   * @group chado
   */
  public function test_chado_db_select_works_for_chado_tables() {
    $analysis_record = factory('chado.analysis')->create();

    $id = $analysis_record->analysis_id;


    // Test passing a table name without brackets or braces.
    $query = chado_db_select('analysis', 't');

    $analysis = $query
      ->condition('analysis_id', $id)
      ->fields('t')
      ->execute()
      ->fetchObject();

    $this->assertNotFalse($analysis);
    $this->assertNotEmpty($analysis);
    $this->assertEquals($id, $analysis->analysis_id);
  }

  /**
   * @group api
   * @group chado
   */
  public function test_chado_db_select_should_throw_an_exception_if_table_is_undefined() {
    $this->expectException(\Exception::class);
    chado_db_select('some_nonexistent_table', 'd')->execute();
  }

  /**
   * @group api
   * @group chado
   */
  public function test_chado_db_select_recognizes_non_chado_tables() {
    $query = chado_db_select('users');
    $query->range(0, 1);
    $results = $query->execute()->fetchAll();

    $this->assertNotEmpty($results);
  }

  /**
   * @group api
   * @group chado
   */
  public function test_joining_chado_tables_in_chado_db_select() {
    $feature = factory('chado.feature')->create();
    $cvterm = factory('chado.cvterm')->create();
    $pub = factory('chado.pub')->create();

    $feature_cvterm = chado_insert_record('feature_cvterm', [
      'feature_id' => $feature->feature_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'pub_id' => $pub->pub_id
    ]);

    $query = chado_db_select('feature', 'f');
    $query->join('feature_cvterm', 'fcvt', 'f.feature_id = fcvt.feature_id');
    $query->fields('f', ['name']);
    $query->fields('fcvt', ['cvterm_id']);
    $query->condition('f.feature_id', $feature->feature_id);
    $found = $query->execute()->fetchObject();

    $this->assertNotEmpty($found);
    $this->assertEquals($feature_cvterm['cvterm_id'], $found->cvterm_id);
  }
}
