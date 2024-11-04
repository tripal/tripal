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

      // We also lose the tripaldbx.settings config in Kernel tests
      // This is needed when getting available schema, for example.
      // As such we are going to manually set some needed ones within
      // the test config based on the real config.
      $fromReal = $this->realConfigFactory->get('tripaldbx.settings')
      ->get('test_schema_base_names', []);
      \Drupal::configFactory()
        ->getEditable('tripaldbx.settings')
        ->set('test_schema_base_names', $fromReal)
        ->save();
    }
  }


  /**
   * Warns test developers if they are missing required modules in a kernel test.
   *
   * This is needed because otherwise the exceptions thrown are not as obvious
   * and complicate debbugging kernel tests.
   *
   * @param array $functionality
   *  A list of functionality you need to support. Although this method handles
   *  dependencies, you should include all items in the supported keys below
   *  that you need. This is because in some cases you will want to mock rather
   *  then include in your kernel tests and this way, this method supports that.
   *  Supported keys are:
   *   - TripalTerm
   *   - TripalEntity
   *   - ChadoField
   * @return void
   */
  protected function suggestRequiredModules(array $functionality) {
    $suggested_modules = [];

    // We need to do the suggested modules first so that you get better
    // warnings if you do not have the right combination.
    if (in_array('TripalTerm', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['tripal'] = 'tripal_chado';
    }
    if (in_array('TripalEntity', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['user'] = 'user';
      $suggested_modules['path'] = 'path';
      $suggested_modules['path_alias'] = 'path_alias';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['tripal'] = 'tripal_chado';
      $suggested_modules['field'] = 'field';
    }
    if (in_array('TripalField', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['user'] = 'user';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['tripal'] = 'tripal_chado';
      $suggested_modules['field'] = 'field';
    }
    if (in_array('TripalImporter', $functionality)) {
      $suggested_modules['system'] = 'system';
      $suggested_modules['user'] = 'user';
      $suggested_modules['tripal'] = 'tripal';
      $suggested_modules['tripal'] = 'tripal_chado';
      $suggested_modules['file'] = 'file';
    }

    // Now warn you about your modules array.
    $missing_modules = [];
    foreach ($suggested_modules as $check) {
      if (!in_array($check, static::$modules)) {
        $missing_modules[] = $check;
      }
    }
    $modules_array_code = 'protected static $modules = [\'' . implode("','", $suggested_modules) . '\'];';
    $this->assertEmpty($missing_modules, 'You are missing some modules in your static $modules array. For the functionality you requested, we suggest the following: ' . $modules_array_code);
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
      $this->installSchema('system', 'sequences');
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
