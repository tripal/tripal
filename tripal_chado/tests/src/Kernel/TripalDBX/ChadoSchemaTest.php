<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;

/**
 * Tests for ChadoConnection.
 *
 * @group Tripal Chado
 * @group TripalDBX
 * @group ChadoDBX
 * @group ChadoSchema
 */
class ChadoSchemaTest extends ChadoTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * Lists all supported versions of chado.
   *
   * @var array
   */
  protected static array $supported_chado_versions = [
    '1.3',
    '1.3.3.001',
    '1.3.3.002',
    '1.3.3.003',
    '1.3.3.004',
    '1.3.3.005',
    '1.3.3.006',
    '1.3.3.007',
    '1.3.3.008',
    '1.3.3.009',
    '1.3.3.011',
    '1.3.3.013',
  ];

  /**
   * Lists init levels that are worth testing for schema actions.
   *
   * @var array
   */
  protected static array $init_levels = [
    3, // self::$INIT_CHADO_EMPTY,
    4, // self::$INIT_CHADO_DUMMY,
    5, // self::$PREPARE_TEST_CHADO,
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);
  }

  /**
   * Provides each supported version of chado to the tests.
   *
   * @return array
   *   Each scenario is a unique chado version and init level combination.
   */
  public static function provideChadoSchemaVersions() {
    $scenarios = [];

    foreach (self::$supported_chado_versions as $version_string) {
      foreach (self::$init_levels as $init_level) {
        $scenarios[] = [
          (string) $version_string,
          $init_level,
        ];
      }
    }

    return $scenarios;
  }

  /**
   * Tests ChadoSchema::getMainTables() on all Chado schema versions.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @param string $version
   *   The version of chado to test against.
   * @param int $init_level
   *   The init level to create the test database with.
   */
  public function testGetMainTables(string $version, int $init_level) {

    // Get Chado in place.
    $chado_connection = $this->createTestSchema(
      $init_level,
      $version
    );
    $this->assertInstanceOf(
      'Drupal\tripal_chado\Database\ChadoConnection',
      $chado_connection,
      "Unable to create test chado with the specified version (i.e. $version)."
    );
    $this->assertEquals(
      $version,
      $chado_connection->getVersion(),
      "We expect that the chado version returned by the connection matches what we requested."
    );

    $schema = $chado_connection->schema();

    $tables = $schema->getMainTables();
    $this->assertIsArray(
      $tables,
      "ChadoSchema::getMainTables did not return an array as expected"
    );
  }

  /**
   * Test the ChadoSchema::getDefault() method on all Chado schema versions.
   *
   * We will test that the default chado in this test returns the name set
   * by the chadoTestTrait when a new test schema was created. The chadoTestTrait
   * also sets the new chado schema created for testing to the default within
   * the test environment.
   *
   * There is no way to programatically set the default chado, so we will not
   * test changing the default and seeing if the default is still reported
   * correctly afterwards.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @param string $version
   *   The version of chado to test against.
   * @param int $init_level
   *   The init level to create the test database with.
   */
  public function testGetDefault(string $version, int $init_level) {

    // Get Chado in place.
    $chado_connection = $this->createTestSchema(
      $init_level,
      $version
    );
    $this->assertInstanceOf(
      'Drupal\tripal_chado\Database\ChadoConnection',
      $chado_connection,
      "Unable to create test chado with the specified version (i.e. $version)."
    );
    $this->assertEquals(
      $version,
      $chado_connection->getVersion(),
      "We expect that the chado version returned by the connection matches what we requested."
    );

    $default_chado_schema = $chado_connection->schema()->getDefault();

    // Test if the reported Chado version matches the test chado format,
    // e.g. _test_chado_h87g97hkln64vy76
    $this->assertMatchesRegularExpression('/(\_test\_chado\_[\w]{16})\b/', $default_chado_schema, "The default Chado schema returned did not match the format we expected within the testing environment.");

    // Test if the default chado schema returned matches the
    // one we created.
    $this->assertEquals(
      $this->testSchemaName,
      $default_chado_schema,
      "The default chado schema name we retrieved did not match the test chado schema we just created."
    );
  }

  /**
   * Provides scenarios to test ChadoSchema::getSchemaDef().
   *
   * @return array
   *   Each scenario is a unique chado version and init level combination with
   *   a specific option set for getSchemaDef() to be called with.
   */
  public static function provideSchemaDefParams() {
    $scenarios = [];

    $schema_version_combos = self::provideChadoSchemaVersions();

    // Create scenarios:
    // For each available init level and version combo...
    foreach ($schema_version_combos as $version_combo) {

      // Provide only required params: database.
      $scenarios[] = $version_combo + [
        'options' => [
          'source' => 'database',
        ],
      ];

      // Provide only required params: file.
      $scenarios[] = $version_combo + [
        'options' => [
          'source' => 'file',
        ],
      ];

    }

    return $scenarios;
  }

  /**
   * Tests ChadoSchema::getSchemaDef() on all Chado schema versions.
   *
   * @dataProvider provideSchemaDefParams
   *
   * @param string $version
   *   The version of chado to test against.
   * @param int $init_level
   *   The init level to create the test database with.
   * @param array $options
   *   Parameters to use when testing ChadoSchema::getSchemaDef().
   */
  public function testGetChadoSchemaDef(string $version, int $init_level, array $options) {

    // Get Chado in place.
    $chado_connection = $this->createTestSchema(
      $init_level,
      $version
    );
    $this->assertInstanceOf(
      'Drupal\tripal_chado\Database\ChadoConnection',
      $chado_connection,
      "Unable to create test chado with the specified version (i.e. $version)."
    );
    $this->assertEquals(
      $version,
      $chado_connection->getVersion(),
      "We expect that the chado version returned by the connection matches what we requested."
    );

    $schema = $chado_connection->schema();

    // Test with format none which will always return an empty array.
    $options_with_none_format = $options;
    $options_with_none_format['format'] = 'none';
    $schema_def = $schema->getSchemaDef($options_with_none_format);
    $this->assertIsArray(
      $schema_def,
      "ChadoSchema::getSchemaDef did not return an array as expected when format: none was supplied."
    );
    $this->assertCount(
      0,
      $schema_def,
      "ChadoSchema::getSchemaDef with format none should return empty array."
    );

    // Test with the options actually provided (i.e. with version not specified).
    $schema_def = $schema->getSchemaDef($options);
    $this->assertValidSchemaDef(
      $schema_def,
      "ChadoSchema::getSchemaDef did not return a valid schema array as expected."
    );

    // Test with the options provided and also the version.
    $options_with_version = $options;
    $options_with_version['version'] = $version;
    $schema_def_with_version = $schema->getSchemaDef($options_with_version);
    $this->assertValidSchemaDef(
      $schema_def_with_version,
      "ChadoSchema::getSchemaDef did not return a valid schema array as expected when the correct version was supplied."
    );
    $this->assertEquals(
      $schema_def,
      $schema_def_with_version,
      "The schema definition provided should be the same regardless of whether the version was supplied or not."
    );

    // Now test that clearing the cache still returns the same result as before.
    $options_with_clearCache = $options;
    $options_with_clearCache['clear'] = TRUE;
    $schema_def2 = $schema->getSchemaDef($options_with_clearCache);
    $this->assertValidSchemaDef(
      $schema_def2,
      "ChadoSchema::getSchemaDef did not return a valid schema array as expected after clearing the cache."
    );
    $this->assertEquals(
      $schema_def,
      $schema_def2,
      "The schema definition provided should be the same after clearing the cache as it was before."
    );
  }

  /**
   * Confirms that a Schema Definition array is valid.
   *
   * @param array $schema_def
   *   An array defining the schema as returned by getSchemaDef().
   * @param string $message
   *   A message prefix to use with the asserts.
   */
  public function assertValidSchemaDef(array $schema_def, string $message) {

    $this->assertIsArray(
      $schema_def,
      $message . " Not an array."
    );

    $this->assertNotCount(
      0,
      $schema_def,
      $message . " Shouldn't be an empty array."
    );

    $this->assertGreaterThan(
      5,
      count($schema_def),
      $message . " There should be at least 5 tables in a schema definition."
    );

    // Check for some key tables.
    $this->assertArrayHasKey(
      'chadoprop',
      $schema_def,
      $message . " All valid schema should have a chadoprop table to indicate version."
    );
    $this->assertArrayHasKey(
      'cvterm',
      $schema_def,
      $message . " All valid schema should have a cvterm table."
    );

    // Check structure for a single table.
    // @note cannot check table structure since it is different per source.
    // @note the following would be for a file source.
    /*
    $table_def = $schema_def['cvterm'];
    $this->assertArrayHasKey(
      'description',
      $table_def,
      $message . " Table definition does not have a description key."
    );
    $this->assertArrayHasKey(
      'fields',
      $table_def,
      $message . " Table definition does not have a fields key."
    );
    $this->assertIsArray(
      $table_def['fields'],
      $message . " Table definition fields should be an array."
    );
    */
  }
}
