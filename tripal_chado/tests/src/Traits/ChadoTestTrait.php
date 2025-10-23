<?php

namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal_chado\Database\ChadoConnection;
use Symfony\Component\Yaml\Yaml;
use PHPUnit\Framework\Attributes\Group;

/**
 * This is a PHP Trait for Chado tests.
 *
 * It provides the functions and member variables that are
 * used for both any test class that needs testing with Chado.
 *
 * @group Tripal
 * @group Tripal Chado
 */
#[Group('Tripal')]
#[Group('Tripal Chado')]

trait ChadoTestTrait {

  /**
   * Tripal DBX tool instance.
   *
   * @var \Drupal\tripal\TripalDBX\TripalDbx
   */
  protected $tripal_dbx = NULL;

  /**
   * Real (Drupal live, not test) config factory.
   *
   * @var \Drupal\Core\Config\ConfigFactory
   */
  protected $realConfigFactory;

  /**
   * Base name for test schemas.
   *
   * @var array
   */
  protected $testSchemaBaseNames;

  /**
   * List of tested schemas.
   *
   * Keys are schema names and values are boolean. At test shutdown, when the
   * test schema cleanup method is called, if a schema name is set to TRUE and
   * could not be removed, an error message will be reported.
   * Use: when a temporary test schema is created, it should be added to the
   * list `self::$testSchemas[$schema_name] = TRUE;` and when it has been
   * removed by a test, its value should be set to FALSE, indicating it is ok if
   * it cannot be dropped again `self::$testSchemas[$schema_name] = FALSE;`.
   *
   * @var array
   */
  protected static $testSchemas = [];

  /**
   * A string indicating the name of the current chado test schema.
   *
   * @var string
   */
  protected $testSchemaName = NULL;

  /**
   * A database connection.
   *
   * It should be set if not set in any test function that adds schema names to
   * $testSchemas: `self::$db = self::$db ?? \Drupal::database();`
   * This connection will be used during ::tearDownAfterClass when it will not
   * be possible to instantiate a new connection so it needs to be instantiated
   * before, when a test schema is created.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected static $db = NULL;

  /**
   * Gets the chado cvterm_id for the term with the given ID space + accession.
   *
   * This is completely independant of Tripal terms.
   */
  protected function getCvtermID($idspace, $accession) {
    $connection = $this->getTestSchema();

    $query = $connection->select('1:cvterm', 'cvt');
    $query->fields('cvt', ['cvterm_id']);
    $query->join('1:dbxref', 'dbx', 'cvt.dbxref_id = dbx.dbxref_id');
    $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
    $query->condition('db.name', $idspace, '=');
    $query->condition('dbx.accession', $accession, '=');
    $result = $query->execute();

    return $result->fetchField();

  }

  /**
   * Retrieve a record in the testchado.cv table based on it's name.
   *
   * @param string $cvname
   *   The name of the cv to lookup.
   *
   * @return object
   *   An object containing the columns of the cv table (e.g. name, definition).
   *   Returns FALSE if the cv record doesn't exist.
   */
  protected function getChadoCvRecord(string $cvname): FALSE|object {
    $chado = $this->getTestSchema();

    $query = $chado->select('1:cv', 'cv')
      ->condition('cv.name', $cvname, '=')
      ->fields('cv');
    $result = $query->execute();
    if (!$result) {
      return FALSE;
    }
    return $result->fetch();
  }

  /**
   * Retrieve a record in the testchado.db table based on it's name.
   *
   * @param string $dbname
   *   The name of the database to lookup.
   *
   * @return object|false
   *   An object containing the columns of the db table (e.g. name).
   *   Returns FALSE if the db record doesn't exist.
   */
  protected function getChadoDbRecord($dbname): FALSE|object {
    $chado = $this->getTestSchema();

    $query = $chado->select('1:db', 'db')
      ->condition('db.name', $dbname, '=')
      ->fields('db');
    $result = $query->execute();
    if (!$result) {
      return FALSE;
    }
    return $result->fetch();
  }

  /**
   * Retrieve a record in the testchado.cvterm table based on it's name + cv.
   *
   * @param string $cvname
   *   The name of the cv the cvterm is part of.
   * @param string $cvterm_name
   *   The name of the cvterm to lookup.
   *
   * @return object|false
   *   An object containing the columns of the cvterm table (e.g. name).
   *   Returns FALSE if the cvterm record doesn't exist.
   */
  protected function getChadoCvtermRecord(string $cvname, string $cvterm_name): FALSE|object {
    $chado = $this->getTestSchema();

    $query = $chado->select('1:cvterm', 'CVT');
    $query->join('1:cv', 'CV', '"CV".cv_id = "CVT".cv_id');
    $query->fields('CVT')
      ->condition('CVT.name', $cvterm_name, '=')
      ->condition('CV.name', $cvname, '=');
    $result = $query->execute();
    if (!$result) {
      return FALSE;
    }
    return $result->fetch();
  }

  /**
   * {@inheritdoc}
   */
  public static function tearDownAfterClass() :void {
    // Try to cleanup.
    if (isset(self::$db)) {
      $errors = [];
      foreach (self::$testSchemas as $test_schema => $in_use) {
        try {
          self::$db->query("DROP SCHEMA $test_schema CASCADE;");
        }
        catch (\Exception $e) {
          if ($in_use) {
            $errors[] =
              'Unable to remove temporary tests schema "'
              . $test_schema
              . '": ' . $e->getMessage();
          }
        }
      }
      if (!empty($errors)) {
        trigger_error(
          implode("\n", $errors),
          E_USER_WARNING
        );
      }
    }
  }

  /**
   * Get real config data.
   */
  protected function getRealConfig() {
    // Get original config from Drupal real installation.
    // This is done by getting a connection to the real database first.
    // Then instantiate a new config factory that will use that database through
    // a new instance of config storage using that database.
    // Get Drupal real database.
    $drupal_db = \Drupal\Core\Database\Database::getConnection(
      'default',
      'simpletest_original_default'
    );
    // Instantiate a new config storage.
    $config_storage = new \Drupal\Core\Config\DatabaseStorage(
      $drupal_db,
      'config'
    );
    // Get an event dispatcher.
    $event_dispatcher = \Drupal::service('event_dispatcher');
    // Get a typed config (note: this will use the test config storage).
    $typed_config = \Drupal::service('config.typed');
    // Instanciate a new config factory.
    $this->realConfigFactory = new \Drupal\Core\Config\ConfigFactory(
      $config_storage,
      $event_dispatcher,
      $typed_config
    );
  }

  /**
   * Initializes TripalDbx member.
   */
  protected function initTripalDbx() {
    $this->tripal_dbx = \Drupal::service('tripal.dbx');
    // Hack to clear TripalDbx cache on each run.
    $clear = function () {
      TripalDbx::$drupalSchema = NULL;
      TripalDbx::$reservedSchemaPatterns = NULL;
    };
    $clear->call(new TripalDbx());
    // Adds live schema reservation.
    $reserved_schema_patterns = $this->realConfigFactory->get('tripal.settings')
      ->get('reserved_schema_patterns', []);
    foreach ($reserved_schema_patterns as $pattern => $description) {
      $this->tripal_dbx->reserveSchemaPattern($pattern, $description);
    }
  }

  /**
   * Allows a test to use reserved Chado test schema names.
   */
  protected function allowTestSchemas() {
    $this->testSchemaBaseNames = $this->realConfigFactory
      ->get('tripal.settings')
      ->get('test_schema_base_names', []);
    $this->tripal_dbx->freeSchemaPattern(
      $this->testSchemaBaseNames['chado'],
      TRUE
    );
  }

  /**
   * Creates Chado installations table.
   */
  protected function createChadoInstallationsTable() {
    $db = \Drupal::database();
    if (!$db->schema()->tableExists('chado_installations')) {
      $db->schema()->createTable('chado_installations',
        [
          'fields' => [
            'install_id' => [
              'type' => 'serial',
              'unsigned' => TRUE,
              'not null' => TRUE,
            ],
            'schema_name' => [
              'type' => 'varchar',
              'length' => 255,
              'not null' => TRUE,
            ],
            'version' => [
              'type' => 'varchar',
              'length' => 255,
              'not null' => TRUE,
            ],
            'created' => [
              'type' => 'varchar',
              'length' => 255,
            ],
            'updated' => [
              'type' => 'varchar',
              'length' => 255,
            ],
          ],
          'indexes' => [
            'schema_name' => ['schema_name'],
          ],
          'primary key' => ['install_id'],
        ]
      );
    }
  }

  /**
   * Gets a new Chado schema for testing.
   *
   * Retrieves the current test schema. If there is not currently a test schema
   * set-up then one will be created.
   *
   * @param int|null $init_level
   *   One of the constant to select the schema initialization level.
   *   If this is supplied then it forces a new connection to be made for
   *   backwards compatibility.
   * @param string $version
   *   Indicate the version of Chado to test against.
   *
   * @return \Drupal\tripal\TripalDBX\TripalDbxConnection
   *   A bio database connection using the generated schema.
   */
  protected function getTestSchema(?int $init_level = NULL, string $version = '1.3') {

    if ($init_level !== NULL) {
      return $this->createTestSchema($init_level, $version);
    }
    elseif ($this->testSchemaName === NULL) {
      return $this->createTestSchema(0, $version);
    }
    else {
      return new ChadoConnection($this->testSchemaName);
    }
  }

  /**
   * Creates a new Chado schema for testing.
   *
   * @param int $init_level
   *   One of the constant to select the schema initialization level.
   * @param string $version
   *   Indicate the version of Chado to test against.
   *
   * @return \Drupal\tripal\TripalDBX\TripalDbxConnection
   *   A bio database connection using the generated schema.
   */
  protected function createTestSchema(int $init_level = 0, string $version = '1.3') {
    $schema_name = $this->testSchemaBaseNames['chado']
      . '_'
      . bin2hex(random_bytes(8));
    $tripaldbx_db = new ChadoConnection($schema_name);
    // Make sure schema is free.
    if ($tripaldbx_db->schema()->schemaExists()) {
      $this->markTestSkipped(
        "Failed to generate a free test schema ($schema_name)."
      );
    }

    // Determine the name of the chado schema file.
    $chado_schema_file = __DIR__ . '/../../../chado_schema/chado-only/version-' . $version . '.sql';
    $this->assertFileIsReadable($chado_schema_file, "Schema file does not exist for the version you specified (i.e. '$version').");

    switch ($init_level) {
      case static::INIT_CHADO_DUMMY:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          $chado_schema_file,
          ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado schema loaded.');

        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/fill_chado_test_prepare.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Prepared chado records added.');

        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/fill_chado.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Dummy Chado schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::INIT_CHADO_EMPTY:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          $chado_schema_file,
          ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');

        // Add version information to the schema so the tests don't fail.
        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/version.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado version loaded.');
        break;

      case static::PREPARE_TEST_CHADO:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          $chado_schema_file,
          ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');

        // Add version information to the schema so the tests don't fail.
        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/fill_chado_test_prepare.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Prepared chado records added.');
        break;

      case static::INIT_DUMMY:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          __DIR__ . '/../../fixtures/test_schema.sql',
          'none'
        );
        $this->assertTrue($success, 'Dummy schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::CREATE_SCHEMA:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        break;

      case static::SCHEMA_NAME_ONLY:
        break;

      default:
        break;
    }
    self::$db = self::$db ?? \Drupal::database();
    // Set the test schema name.
    // If the schema was actually created then set the value to TRUE because
    // it's actually in use and needs to be dropped but otherwise, set it FALSE.
    if ($init_level === static::SCHEMA_NAME_ONLY) {
      self::$testSchemas[$schema_name] = FALSE;
    }
    else {
      self::$testSchemas[$schema_name] = TRUE;
    }
    $this->testSchemaName = $schema_name;

    // Make sure that the version is correct in the chado property table.
    if (($init_level > 1) && ($version !== '1.3')) {
      $this->setChadoTestSchemaVersion($tripaldbx_db, $version);
    }

    // Make sure that any other connections to TripalDBX will see this new test
    // schema as the default schema.
    $config = \Drupal::service('config.factory')->getEditable('tripal_chado.settings');
    $config->set('default_schema', $schema_name)->save();

    // As a safety check, make sure that the tripalDBX object is using the test
    // schema. We don't want to perform tests in a live schema.
    $this->assertTrue($tripaldbx_db->getSchemaName() == $schema_name, 'TripalDBX is not using the test schema.');

    // Set this to be the Chado connection used in the current test schema.
    $container = \Drupal::getContainer();
    $container->set('tripal_chado.database', $tripaldbx_db);

    return $tripaldbx_db;
  }

  /**
   * Sets the chado version of the test schema.
   *
   * @param Drupal\tripal_chado\Database\ChadoConnection $chado_connection
   *   The test schema to set the version for.
   * @param string $version
   *   The version of Chado to set (e.g. 1.3).
   */
  protected function setChadoTestSchemaVersion(ChadoConnection &$chado_connection, string $version) {

    // Get the version cvterm ID.
    $result = $chado_connection->select('1:cvterm', 'cvt')
      ->fields('cvt', ['cvterm_id']);
    $result->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
    $result->condition('cv.name', 'chado_properties');
    $result->condition('cvt.name', 'version');
    $result = $result->execute();
    $version_cvterm_id = $result->fetchField();
    $this->assertNotEquals(0, $version_cvterm_id, "We were unable to retrieve the cvterm needed for the chado property verion.");

    // Now update the current version.
    $chado_connection->update('1:chadoprop')
      ->fields([
        'value' => $version,
      ])
      ->condition('type_id', $version_cvterm_id)
      ->execute();

    // Confirm the version was set properly.
    // Note: we use findVersion() because getVersion() is cached and this
    // resets the cache.
    $this->assertEquals(
      $version,
      $chado_connection->findVersion($chado_connection->getSchemaName(), TRUE),
      "We expect that the chado version returned by the connection matches what we requested."
    );
  }

  /**
   * Updates an existing test schema to a new version.
   *
   * @param Drupal\tripal_chado\Database\ChadoConnection $chado_connection
   *   The test schema to migrate.
   * @param string $current_version
   *   The current version of the test schema (e.g. 1.3).
   * @param string $version_to_upgrade_to
   *   The version to upgrade the test schema to (e.g. 1.3.3.013).
   */
  protected function upgradeTestSchema(ChadoConnection &$chado_connection, string $current_version, string $version_to_upgrade_to) {

    $target_schema = $chado_connection->getSchemaName();
    $migrations = $this->getChadoMigrations();

    // Determine which migrations need to be applied.
    // -- Where to start.
    // If the current version is 1.3 then we need to start applying at the
    // beginning of the list. If not, we need to find where to start so lets
    // do that now.
    $start_index = 0;
    if ($current_version !== '1.3') {
      foreach ($migrations as $key => $details) {
        if ($details['version'] === $current_version) {
          $start_index = $key;
          break;
        }
      }
      if ($start_index === 0) {
        $this->assertTrue(FALSE, "We were unable to find the '$current_version' in our list of migrations.");
      }
    }

    // -- Now apply migrations from there onwards.
    // Now that we know where to start, lets begin applying migrations to the
    // current test schema until we reach the version to update to.
    $last_key_possible = array_key_last($migrations);
    for ($i = $start_index; $i <= $last_key_possible; $i++) {
      $migration_file = __DIR__ . '/../../../chado_schema/migrations/' . $migrations[$i]['filename'];
      $success = $chado_connection->executeSqlFile(
        $migration_file,
        FALSE,
        $target_schema
      );
      $this->assertTrue(
        $success,
        "We should have been able to apply $migration_file to $target_schema but could not."
      );

      // Check to see if this is the last migration we should apply.
      if ($migrations[$i]['version'] === $version_to_upgrade_to) {
        break;
      }
    }

    // Now report the new version.
    $this->setChadoTestSchemaVersion($chado_connection, $version_to_upgrade_to);

  }

  /**
   * Retrieve information about the current chado migrations available.
   *
   * @return array
   *   The parsed array structure from the migrations YAML not include the
   *   file definition but instead only that under `migrations`.
   */
  protected function getChadoMigrations() {
    $yaml_file = __DIR__ . '/../../../chado_schema/migrations/tripal_chado.chado_migrations.yml';

    $this->assertFileIsReadable(
      $yaml_file,
      "Cannot open YAML file $yaml_file in order to set the fields for testing chadostorage."
    );

    $file_contents = file_get_contents($yaml_file);
    $this->assertNotEmpty(
      $file_contents,
      "Unable to retrieve contents for YAML file $yaml_file in order to set the fields for testing chadostorage."
    );

    $yaml_data = Yaml::parse($file_contents);
    $this->assertNotEmpty(
      $yaml_data,
      "Unable to parse YAML file $yaml_file in order to set the fields for testing chadostorage."
    );
    $this->assertArrayHasKey(
      'migrations',
      $yaml_data,
      "The key 'migrations' does not exist in the parsed YAML file: $yaml_file."
    );

    return $yaml_data['migrations'];
  }

  /**
   * Removes a Chado test schema and keep track it has been removed correctly.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $tripaldbx_db
   *   A bio database connection using the test schema.
   */
  protected function freeTestSchema(\Drupal\tripal\TripalDBX\TripalDbxConnection $tripaldbx_db) {
    self::$testSchemas[$tripaldbx_db->getSchemaName()] = FALSE;
    try {
      $tripaldbx_db->schema()->dropSchema();
    }
    catch (\Exception $e) {
      // Ignore issues.
    }
  }

  /**
   * Warns test developers if required modules are missing in a kernel test.
   *
   * This is needed because otherwise the exceptions thrown are not very obvious
   * and complicate debugging kernel tests.
   *
   * @param array $functionality
   *   A list of functionality you need to support. Although this method handles
   *   dependencies, you should include all items in the supported keys below
   *   that you need. This is because in some cases you will want to mock rather
   *   than include in your kernel tests. This way, this method supports that.
   *   Supported keys are:
   *     - TripalTerm
   *     - TripalEntity
   *     - ChadoField.
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

}
