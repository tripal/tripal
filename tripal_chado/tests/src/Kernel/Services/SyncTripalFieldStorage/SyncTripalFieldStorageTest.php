<?php

namespace Drupal\Tests\tripal\Kernel;

use Drupal\Core\Database\Database;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

/**
 * Tests the SyncTripalFieldStorage service.
 *
 * @group SyncTripalFieldStorage
 */
class SyncTripalFieldStorageTest extends ChadoTestKernelBase {

  /**
   * The theme under which testing should be done.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * The modules that this test relies on.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal', 'tripal_chado', 'views', 'field'];

  /**
   * Connection to the test chado instance.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection;
   */
  protected ChadoConnection $chado_connection;

  /**
   * Connection to the test drupal instance.
   *
   * @var \Drupal\Core\Database\Database;
   */
  protected Database $drupal_connection;

  /**
   * Provides information about specific versions of Chado to test for
   * discrepancies against field installed on Chado 1.3.
   */
  public static function provideChadoVersionsToTest() {
    $scenarios = [];

    return $scenarios;
  }

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalTerm', 'TripalEntity']);
    // -- additionally we need tripal_chado config to access the yaml files.
    $this->installConfig('tripal_chado');

    // Create chado 1.3 test instance which will later be upgraded.
    $this->chado_connection = $this->createTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO, '1.3');

    // Add terms needed.
    // @todo update the createContentType method to add terms needed!
    // Create the terms for the field property storage types.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    foreach (['local', 'SIO', 'schema', 'data', 'NCIT', 'operation', 'OBCS', 'SWO', 'IAO', 'TPUB', 'SBO', 'sep', 'ERO', 'EFO'] as $termIdSpace) {
      $idsmanager->createCollection($termIdSpace, "chado_id_space");
    }
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    foreach (['local', 'SIO', 'schema', 'EDAM', 'ncit', 'OBCS', 'swo', 'IAO', 'tripal_pub', 'sbo', 'sep', 'ero', 'efo'] as $termVocab) {
      $vmanager->createCollection($termVocab, "chado_vocabulary");
    }

  }

  /**
   * Tests SyncTripalFieldStorage::detectDifferences().
   */
  public function testDetectDifferences() {
    $chado_verison_under_test = '1.3.3.013';
    $bundles_under_test = ['general_chado.project'];

    // Create an instance of the specified bundle(s) with all associated fields.
    [$bundle_category, $bundle_name] = explode('.', $bundles_under_test[0]);
    $this->createContentTypeFromConfig($bundle_category, $bundle_name, TRUE);

    // Upgrade the test environment to the specified chado version.
    $this->upgradeTestSchema($this->chado_connection, '1.3', $chado_verison_under_test);
    $this->assertEquals(
      $chado_verison_under_test,
      $this->chado_connection->getVersion(),
      "We were unable to upgrade our test schema to the version we intended to."
    );
  }
}
