<?php

namespace Drupal\Tests\tripal\Functional;

use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;

/**
 * Tests the basic functions of the Bulk PostgreSQL Schema Installer.
 *
 * @group Tripal
 * @group Tripal Database
 */
class bulkPgSchemaInstallerTest extends BrowserTestBase {

  // protected $htmlOutputEnabled = TRUE;
  protected $defaultTheme = 'stark';

  protected static $modules = ['tripal'];

	/**
	 * Tests the contructor of bulkPgSchemaInstaller.
   *
	 * @group pgsql
	 * @group services
	 */
	public function testInitialization() {

		$service = \Drupal::service('tripal.bulkPgSchemaInstaller');
		$this->assertIsObject($service,
			"Unable to initialize the tripal.bulkPgSchemaInstaller service.");
		$this->assertIsObject($service->getDrupalConnection(),
			"Unable to initialize Drupal database connection");
		$this->assertNotFalse($service->getPgConnection(),
			"Unable to initialize postgresql-specific database connection");
		$this->assertIsObject($service->getLogger(),
			"Unable to initialize the message/error logger.");
	}

	/**
	 * Tests createSchema and dropSchema.
	 *
	 * @group pgsql
	 * @group services
	 */
	public function testCRUDSchema() {

		$service = \Drupal::service('tripal.bulkPgSchemaInstaller');
		$schema_name = 'testschema' . uniqid();

		// Test creating a schema.
		$return_code = $service->createSchema($schema_name);
		$this->assertTrue($return_code, "createSchema($schema_name) did not return TRUE.");

		$checksql = "
	    SELECT true
	    FROM pg_namespace
	    WHERE
	      has_schema_privilege(nspname, 'USAGE') AND
	      nspname = :nspname
	  ";
	  $query = \Drupal::database()->query($checksql, [':nspname' => $schema_name]);
	  $schema_exists = $query->fetchField();
		$this->assertEquals(1, $schema_exists, "Unable to find newly created schema $schema_name.");

		// Test applying SQL to a schema.
		$sql_file = \Drupal::service('extension.list.module')->getPath('tripal') . '/tests/fixtures/smallTestSchema.sql';
		$return_code = $service->applySQL($sql_file, $schema_name, TRUE);
		$this->assertTrue($return_code, "applySQL($schema_name) did not return TRUE.");
		// Check specific tables exist.
		$sql = "SELECT true FROM pg_tables WHERE schemaname = :schema AND tablename  = :table";
		foreach (['regions', 'countries', 'locations', 'castles', 'knights', 'ancestors'] as $table) {
			$result = \Drupal::database()->query($sql,
				[':schema' => $schema_name, ':table' => $table])->fetchField();
			$this->assertEquals(1, $result, "Table ($table) did not exist when it should have.");
		}
		// Check data exists.
		// -- regions.
		$sql = 'SELECT count(*) FROM ' . $schema_name . '.regions';
		$result = \Drupal::database()->query($sql)->fetchField();
		$this->assertEquals(9, $result, "Not the expected number of regions.");
		// -- countries.
		$sql = 'SELECT count(*) FROM ' . $schema_name . '.countries';
		$result = \Drupal::database()->query($sql)->fetchField();
		$this->assertEquals(25, $result, "Not the expected number of countries.");
		// -- locations.
		$sql = 'SELECT count(*) FROM ' . $schema_name . '.locations';
		$result = \Drupal::database()->query($sql)->fetchField();
		$this->assertEquals(7, $result, "Not the expected number of locations.");
		// -- castles.
		$sql = 'SELECT count(*) FROM ' . $schema_name . '.castles';
		$result = \Drupal::database()->query($sql)->fetchField();
		$this->assertEquals(11, $result, "Not the expected number of castles.");

		// Test dropping the schema.
		$return_code = $service->dropSchema($schema_name);
		$this->assertTrue($return_code, "dropSchema($schema_name) did not return TRUE.");
		$query = \Drupal::database()->query($checksql, [':nspname' => $schema_name]);
	  $schema_exists = $query->fetchField();
		$this->assertFalse($schema_exists, "Still able to find supposedly dropped schema $schema_name.");
	}
}
