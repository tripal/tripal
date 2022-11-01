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


    // $this->prepareTestChado(); // This does not work :(
    $this->prepareTestChado(); // This works :( or :)

    // Test to see if cv table data got imported
    $cv_results = $chado->query("SELECT * FROM {1:cv} WHERE name LIKE 'feature_property'");
    $cv_found = false;
    foreach ($cv_results as $row) {
        $cv_found = true;
    } 
    $this->assertTrue($cv_found, 'Found feature_property CV'); 
      
    // Test to see whether db table data got imported
    $db_results = $chado->query("SELECT * FROM db WHERE name LIKE 'TAXRANK';");
    $db_found = true;
    foreach ($db_results as $row) {
        $db_found = true;
    }
    $this->assertTrue($db_found, 'Found TAXRANK DB'); 

    // Test to see whether cvterm table data got imported
    $cvterm_results = $chado->query("SELECT * FROM cvterm WHERE name LIKE 'accession';");
    $cvterm_found = true;
    foreach ($cvterm_results as $row) {
        $cvterm_found = true;
    }
    $this->assertTrue($cvterm_found, 'Found accession cvterm');     
  }
}