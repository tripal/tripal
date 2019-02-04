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
  public function test_filter_level(){

    $stock = factory('chado.stock')->create(['uniquename' => 'octopus_core_test_name']);

    // Test 1. Pass a single filter.
    $selector = array(
      'stock_id' => $stock->stock_id,
      'uniquename' => array(
        'op' => 'LIKE',
        'data' => 'octopus%',
      ),
    );

    $object = chado_generate_var('stock', $selector);

    $this->assertNotNull($object->stock_id);
    $this->assertEquals($stock->stock_id, $object->stock_id);


    // Test 2 Pass an array of filters with a single item.
    $selector = array(
      'stock_id' => $stock->stock_id,
      'uniquename' => array(
        array(
          'op' => 'LIKE',
          'data' => '01%',
        ),
      ),
    );
    $object = chado_generate_var('stock', $selector);

    $this->assertNotNull($object->stock_id);
    $this->assertEquals($stock->stock_id, $object->stock_id);


    // Test 3 Pass an array of filters with multiple items.
    $selector = array(
      'uniquename' => array(
        array(
          'op' => '>',
          'data' => ($stock->type_id - 1),
        ),
        array(
          'op' => '<',
          'data' => ($stock->type_id + 1),
        ),
      ),
    );
    $object = chado_generate_var('stock',[], $selector);
    $this->assertNotNull($object->stock_id);
    $this->assertEquals($stock->stock_id, $object->stock_id);

  }

}
