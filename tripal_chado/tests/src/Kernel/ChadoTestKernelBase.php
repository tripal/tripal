<?php
namespace Drupal\Tests\tripal_chado\Kernel;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Tests\tripal_chado\Traits\ChadoTestTrait;

/**
 * This is a base class for Chado tests.
 *
 * It enables Chado tests schemas and helper functions to efficiently perform
 * tests.
 *
 * Example:
 * @code
 * // Gets a Chado test schema with dummy data:
 * $biodb = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);
 * //... do some tests
 * // After all is done, remove the schema properly:
 * $this->freeTestSchema($biodb);
 * // Note: if a test fails, the tearDownAfterClass will remove unremoved
 * // schemas.
 * @endcode
 *
 * @group Tripal
 * @group Tripal Chado
 */
abstract class ChadoTestKernelBase extends TripalTestKernelBase {

  use ChadoTestTrait;

  protected static $modules = ['tripal', 'tripal_biodb', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */

   /**
    * Just get a free test schema name.
    */
   public const SCHEMA_NAME_ONLY = 0;

   /**
    * Create an empty schema.
    */
   public const CREATE_SCHEMA = 1;

   /**
    * Create a schema and initialize it with dummy data.
    */
   public const INIT_DUMMY = 2;

   /**
    * Create a Chado schema with default data.
    */
   public const INIT_CHADO_EMPTY = 3;

   /**
    * Create a Chado schema and initialize it with dummy data.
    */
   public const INIT_CHADO_DUMMY = 4;

   /**
    * Create a Chado schema and prepare both it and the associated drupal schema.
    */
   public const PREPARE_TEST_CHADO = 5;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Only initialize the connection to Chado once.
    if (!$this->tripal_dbx) {
      $this->createChadoInstallationsTable();
      $this->getRealConfig();
      $this->initTripalDbx();
      $this->allowTestSchemas();

      // We also lose the tripal.settings config in Kernel tests
      // This is needed when getting available schema, for example.
      // As such we are going to manually set some needed ones within
      // the test config based on the real config.
      $fromReal = $this->realConfigFactory->get('tripal.settings')
      ->get('test_schema_base_names', []);
      \Drupal::configFactory()
        ->getEditable('tripal.settings')
        ->set('test_schema_base_names', $fromReal)
        ->save();
    }
  }

  /**
   * Prepare kernel environments to suppor specific functionality.
   *
   * This method is foccused on making it easier to write kernel test for Tripal
   * functionality. Simply pass in the parts of Tripal core you need in your
   * tests and this method will handle any dependencies to install all the needed
   * schema + config associated with that functionality. Additionally it will
   * try to warn you if your modules array is missing entries with a more user
   * friendly failure then the typical one provided by Drupal.
   *
   * @param array $functionality
   *  A list of functionality you need to support. Although this method handles
   *  dependencies, you should include all items in the supported keys below
   *  that you need. This is because in some cases you will want to mock rather
   *  then include in your kernel tests and this way, this method supports that.
   *  Supported keys are:
   *   - TripalTerm
   *   - TripalEntity
   *   - TripalField
   *
   * @return void
   */
  protected function prepareEnvironment(array $functionality) {

    // We need to check the modules required first so you get good warnings
    // if you are missing one.
    $this->suggestRequiredModules($functionality);

    // Then we come back and actually install things if requested.
    if (in_array('TripalTerm', $functionality)) {
      $this->installSchema('tripal', ['tripal_id_space_collection', 'tripal_terms_idspaces', 'tripal_vocabulary_collection', 'tripal_terms_vocabs', 'tripal_terms']);
    }

    if (in_array('TripalEntity', $functionality)) {
      // Install key entity schema.
      $this->installEntitySchema('user');
      $this->installEntitySchema('path_alias');
      $this->installEntitySchema('tripal_entity');
      $this->installEntitySchema('tripal_entity_type');
    }

    if (in_array('TripalField', $functionality)) {
      if (floatval(\Drupal::VERSION) < 11) {
        $this->installSchema('system', 'sequences');
      }
      $this->installConfig(['field']);
    }

    if (in_array('TripalImporter', $functionality)) {
      $this->installConfig('system');
      $this->installEntitySchema('user');
      $this->installEntitySchema('file');
      $this->installSchema('file', ['file_usage']);
      $this->installSchema('tripal', ['tripal_import', 'tripal_jobs']);
    }
  }
}
