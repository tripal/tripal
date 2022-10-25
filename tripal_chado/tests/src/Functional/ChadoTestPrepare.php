<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;

class ChadoTestPrepare extends ChadoTestBrowserBase {
      /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testChadoPrepareSimpleTest() {
    $public = \Drupal::database();
    $chado = $this->chado;
    $schema_name = $chado->getSchemaName();

    print_r('Schema name:' . $schema_name . "\n");

    $this->prepareTestChado();

    $db_results = $chado->query('SELECT * FROM db');
    foreach ($db_results as $row) {
        print_r($row);
    }
  }
}