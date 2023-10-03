<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;

/**
 * Tests that specific ChadoStorage actions perform as expected.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 */
class ChadoStorageActionsTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  // We will populate this variable at the start of each test
  // with fields specific to that test.
  protected $fields = [];

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // We need to mock the logger to test the progress reporting.
    $container = \Drupal::getContainer();
    $mock_logger = $this->getMockBuilder(\Drupal\tripal\Services\TripalLogger::class)
      ->onlyMethods(['warning'])
      ->getMock();
    $mock_logger->method('warning')
      ->willReturnCallback(function($message, $context, $options) {
        print str_replace(array_keys($context), $context, $message);
        return NULL;
      });
    $container->set('tripal.logger', $mock_logger);

    $this->setUpChadoStorageTestEnviro();
  }

  /**
   * Test the read_value action.
   *
   * Chado Table: project
   *     Columns: project_id*, name*, description
   *
   * Specifically,
   *  - Ensure that a property with the read_value action has the value set
   *  - Ensure that a property with the read_value action can't change the value
   *  - That two fields accessing the same chado column do not conflict
   *      A. both read_value action for the same column
   *      B. one read_value and one store for the same column
   */
  public function testReadValueAction() {

    // This is the field we are actually testing.
    $this->fields['test_read'] = [
      'field_name' => 'test_read',
      'base_table' => 'project',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'chado_table' => 'project',
          'chado_column' => 'project_id'
        ],
        'name_read' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'read_value',
          'chado_table' => 'project',
          'chado_column' => 'name'
        ],
      ],
    ];
    // This is another field (STORE) which we want to ensure there are no conflicts with.
    $this->fields['other_field_store'] = [
      'field_name' => 'other_field_store',
      'base_table' => 'project',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'chado_table' => 'project',
          'chado_column' => 'project_id'
        ],
        'name_store' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'project',
          'chado_column' => 'name'
        ],
      ],
    ];
    // This is another field (READ) which we want to ensure there are no conflicts with.
    $this->fields['other_field_read'] = [
      'field_name' => 'other_field_read',
      'base_table' => 'project',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'chado_table' => 'project',
          'chado_column' => 'project_id'
        ],
        'name_read_again' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'read',
          'chado_table' => 'project',
          'chado_column' => 'name'
        ],
      ],
    ];
    // Needed to ensure these fields are added to the storage arrays e.g. field config.
    $this->cleanChadoStorageValues();

    // Test Case: Insert valid values when they do not yet exist in Chado.
    // ---------------------------------------------------------
    $insert_values = [
      'test_read' => [
        [
          'record_id' => NULL,
          // A read value should not set the value for insert
          // We are doing so here to make sure it cannot modify the
          // values of the table!
          'name_read' => 'Project Name Set By Read',
        ],
      ],
      'other_field_store' => [
        [
          'record_id' => NULL,
          'name_store' => 'Correct Project Name',
        ],
      ],
      'other_field_read' => [
        [
          'record_id' => NULL,
          'name_read_again' => NULL,
        ],
      ],
    ];
    $this->chadoStorageTestInsertValues($insert_values);

    // Check that there is now a single project record.
    $query = $this->chado_connection->select('1:project', 'p')
      ->fields('p', ['project_id', 'name'])
      ->execute();
    $projects = $query->fetchAll();
    $this->assertIsArray($projects,
    "We should have been able to select from the project table.");
    $this->assertCount(1, $projects,
      "There should only be a single project inserted by these 3 fields");

    // Check that the single project record has the name set by the `store` action.

  }
}
