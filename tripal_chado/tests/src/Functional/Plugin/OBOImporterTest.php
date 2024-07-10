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
 * @group TripalImporter
 * @group ChadoImporter
 * @group OntologyImporter
 * @group OBOImporter
 */
class OBOImporterTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado', 'tripal_biodb'];

  public function testOBOImporter() {

    $this->markTestIncomplete(
      'This test will be completed in a separate PR.'
    );

    $public = \Drupal::database();
    $test_chado = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);

    // Insert a record into the tripal_cv_obo table for a vocabulary to load.
    $insert = $public->insert('tripal_cv_obo');
    $insert->fields([
      'name' => 'TCONTACT',
      'path' => '{tripal_chado}/files/tcontact.obo',
    ]);
    $obo_id = $insert->execute();

    // Create an instance of the OBO importer.
    /**
     *
     * @var \Drupal\tripal_chado\Plugin\TripalImporter\OBOImporter $obo_importer
     */
    $importer_manager = \Drupal::service('tripal.importer');
    $obo_importer = $importer_manager->createInstance('chado_obo_loader');
    $obo_importer->createImportJob([
      'obo_id' => $obo_id,
      'schema_name' => $test_chado->getSchemaName()
    ]);

    // Run the importer.
    $obo_importer->run();

    // Run the post run.
    $obo_importer->postRun();

  }
}
