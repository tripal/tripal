<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class ChadoPreparerTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado', 'tripal_biodb', 'field_ui'];

  public function testChadoPreparer() {

    $test_chado = $this->chado;

    // Sanity check: make sure we have the necessary tables.
    $public = \Drupal::database();
    $schema = $public->schema();
    $this->assertTrue($schema->tableExists('tripal_custom_tables'),
        "The Tripal custom_table doesn't exist.");
    $this->assertTrue($schema->tableExists('tripal_mviews'),
        "The Tripal custom_table doesn't exist.");


    // First prepare Chado.
    $preparer = \Drupal::service('tripal_chado.preparer');
    $preparer->setParameters([
      'output_schemas' => [$test_chado->getSchemaName()],
    ]);
    $preparer->performTask();

  }
}