<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;

/**
 * Simple test to ensure that main page loads with module enabled.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Services
 * @group Tripal Database
 */
class chadoInstallerTest extends BrowserTestBase {

  protected $defaultTheme = 'stark';

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * Tests that we can install chado and also drop it.
   *
   * @group chado-install
   */
  public function testInstallSchema() {

    // We installed a chado schema in order to run these tests.
    // It was installed using the chadoInstaller service.
    // NOTE: we don't just run the service here because it takes too long
    // and there are problems with transactions.
    $chado_schema = 'testchado';

    // Next check that the schema is there.
    $connection = \Drupal\Core\Database\Database::getConnection();
    $check_schema = "SELECT true FROM pg_namespace WHERE nspname = :schema";
    $exists = $connection->query($check_schema, [':schema' => $chado_schema])
      ->fetchField();
    $this->assertEquals(1, $exists, 'The schema we just installed was not in the database.');

    // Next check that the schema is not empty.
    $num_tables_sql = "SELECT count(*) FROM information_schema.tables
      WHERE table_schema=:schema";
    $count = $connection->query($num_tables_sql, [':schema' => $chado_schema])
      ->fetchField();
    $this->assertGreaterThanOrEqual(240, $count, 'There should be tables in the chado
      schema we just created.');

  }

}
