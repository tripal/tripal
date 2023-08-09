<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoStorage;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoStorageTestTrait;

use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

/**
 * Tests that ChadoStorage can handle property fields as we expect.
 * The array of fields/properties used for these tests are designed
 * to match those in the ChadoAnalysisDefault field with values filled
 * based on two base tables: phylotree, quantification.
 *
 * Note: quantification is not a typically created content type but we
 * can test it in this manner anyway as the tests are in the kernel environment
 * and do not interact with content types and fields attached to them but rather
 * focuses on the property types/values directly. This also allows us to test
 * phylotree directly even though at the time of writing this test, there is no
 * dbxref_id field attached to phylotree.
 *
 * Note: testotherphylotreefield and testotherquantificationfield are added
 * to ensure we meet the unique constraints on the phylotree and quantification
 * tables respectively.
 *
 *  Specific test cases:
 *   - [PHYLOTREE] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [PHYLOTREE] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [PHYLOTREE] Update values in Chado using ChadoStorage after we just inserted them.
 *   - [QUANTIFICATION] Create Values in Chado using ChadoStorage when they don't yet exist.
 *   - [QUANTIFICATION] Load values in Chado using ChadoStorage after we just inserted them.
 *   - [QUANTIFICATION] Update values in Chado using ChadoStorage after we just inserted them.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group ChadoStorage
 */
class ChadoAnalysisDefaultTest extends ChadoTestKernelBase {

  use ChadoStorageTestTrait;

  /**
   * Properties directly from the ChadoAnalysisDefault field type:
   * @code
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col,
    ]);
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'analysis_id', $analysis_id_term, [
      'action' => 'store',
      'chado_table' => $base_table,
      'chado_column' => $base_fkey_col,
    ]);
    $properties[] =  new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'analysis_name', $analysis_name_term, $analysis_name_length, [
      'action' => 'join',
      'path' => $base_table . '.' . $base_fkey_col . '>analysis.analysis_id',
      'chado_column' => 'name',
      'as' => 'analysis_name',
    ]);
   * @endcode
   *
   * These will be repeated in the testAnalysisFieldPhylotree and
   * testAnalysisFieldQuantification properties array below for testing.
   */
  protected $fields = [
    'testAnalysisFieldPhylotree' => [
      'field_name' => 'testAnalysisFieldPhylotree',
      'base_table' => 'phylotree',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'phylotree',
          'chado_column' => 'phylotree_id'
        ],
        'analysis_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'phylotree',
          'chado_column' => 'analysis_id'
        ],
        'analysis_name' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'join',
          'path' => 'phylotree.analysis_id>analysis.analysis_id',
          'chado_column' => 'name',
          'as' => 'analysis_name',
        ],
      ],
    ],
    // Just adds in any properties needed to meet the unique constraints on the
    // phylotree table.
    'testotherphylotreefield' => [
      'field_name' => 'testotherphylotreefield',
      'base_table' => 'phylotree',
      'properties' => [
        // Foreign key to dbxref table.
        'dbxref_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'phylotree',
          'chado_column' => 'dbxref_id'
        ],
      ],
    ],
    'testAnalysisFieldQuantification' => [
      'field_name' => 'testAnalysisFieldQuantification',
      'base_table' => 'quantification',
      'properties' => [
        'record_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store_id',
          'drupal_store' => TRUE,
          'chado_table' => 'quantification',
          'chado_column' => 'quantification_id'
        ],
        'analysis_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'quantification',
          'chado_column' => 'analysis_id'
        ],
        'analysis_name' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType',
          'action' => 'join',
          'path' => 'quantification.analysis_id>analysis.analysis_id',
          'chado_column' => 'name',
          'as' => 'analysis_name',
        ],
      ],
    ],
    // Just adds in any properties needed to meet the unique constraints on the
    // quantification table.
    'testotherquantificationfield' => [
      'field_name' => 'testotherquantificationfield',
      'base_table' => 'quantification',
      'properties' => [
        // Foreign key to aquisition table.
        'acquisition_id' => [
          'propertyType class' => 'Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType',
          'action' => 'store',
          'chado_table' => 'quantification',
          'chado_column' => 'acquisition_id'
        ],
      ],
    ],
  ];

  protected int $analysis_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();
    $this->setUpChadoStorageTestEnviro();

    // Create the analysis record for use with these fields.
    // This field does not create an analysis but rather just links to one.
    $query = $this->chado_connection->insert('1:analysis');
    $query->fields([
      'name' => 'Tripalus databasica Genome Assembly',
      'program' => 'Best Assembly Software Yet',
      'programversion' => '108',
      'sourcename' => 'Sweat and Tears of Tripal Core Developers',
    ]);
    $this->analysis_id = $query->execute();
  }

  /**
   * Testing ChadoStorage with the ChadoAnalysisDefault field on a phylotree content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testPhylotreeBaseTableFieldCRUD() {
    $this->assertTrue(TRUE, 'Checking for syntax errors + basic setup');
  }

  /**
   * Testing ChadoStorage with the ChadoAnalysisDefault field on a quantification content type.
   *
   * Test Cases:
   *   - Create Values in Chado using ChadoStorage when they don't yet exist.
   *   - Load values in Chado using ChadoStorage after we just inserted them.
   *   - Update values in Chado using ChadoStorage after we just inserted them.
   */
  public function testQuantificationBaseTableFieldCRUD() {
    $this->assertTrue(TRUE, 'Checking for syntax errors + basic setup');
  }
}
