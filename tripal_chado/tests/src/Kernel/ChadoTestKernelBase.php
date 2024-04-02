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
   * Creates a content type and associated fields using the
   * tripalentitytype_collection and tripalfield_collection configuration.
   *
   * @param string $config_id
   *   The id from a tripalentitytype_collection config file. Fields will also be
   *   added if there is a tripalfield_collection with this same id.
   * @param string $content_type_id
   *   The id of the content type to create. It must exist in the specified YAML.
   */
  protected function createContentTypeFromConfig($config_id, $content_type_id, $createTerms = FALSE) {
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $field_service = \Drupal::service('tripal.tripalfield_collection');
    $config_factory = \Drupal::service('config.factory');
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    // FIRST THE CONTENT TYPE.
    $yaml_contentTypes = 'tripal.tripalentitytype_collection.' . $config_id;

    // check that config is installed.
    $config = $config_factory->get($yaml_contentTypes);
    $this->assertIsObject($config,
      'You need to have called $this->installConfig for the module containing the configuration for the content type you want to use in this test.');
    $specific_config = $config->get('content_types');
    $this->assertIsArray($specific_config,
      'You need to have called $this->installConfig for the module containing the configuration for the content type you want to use in this test.');

    foreach ($specific_config as $content_type) {

      if (!array_key_exists('id', $content_type) OR $content_type['id'] != $content_type_id) {
        continue;
      }

      list($termIdSpace, $termAccession) = explode(':', $content_type['term']);
      $idspace = $idsmanager->loadCollection($termIdSpace);
      $this->assertIsObject($idspace, "We were not able to get the id space " . $termIdSpace);
      $term =  $idspace->getTerm($termAccession);
      $this->assertIsObject($term, "We were not able to get the term " . $content_type['term']);
      $content_type['term'] = $term;

      // Add the content type
      $content_type = $content_type_service->createContentType($content_type);
      $this->assertIsObject($content_type,
        "We were not able to create the $content_type_id content type in the testing environment.");
    }

    // NOW THE FIELDS
    $yaml_fields = 'tripal.tripalfield_collection.' . $config_id;

    // check that config is installed.
    $config = $config_factory->get($yaml_fields);
    if (!is_object($config)) {
      return 2;
    }
    $specific_config = $config->get('fields');
    if (!is_array($specific_config)) {
      return 2;
    }

    foreach ($specific_config as $field) {
      if (array_key_exists('content_type', $field) AND $field['content_type'] === $content_type_id) {
        // @debug print "\nAdding Field to Bundle: " . print_r($field,TRUE);
        $field_service->addBundleField($field);
      }
    }

    return 1;
  }
}
