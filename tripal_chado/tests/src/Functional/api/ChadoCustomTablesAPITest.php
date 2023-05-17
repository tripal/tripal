<?php

namespace Drupal\Tests\tripal_chado;

use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;
use Drupal\tripal_chado\api\DrupalSchemaExtended;

/**
 * Testing the tripal_chado/api/tripal_chado.custom_tables.api.php functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 */
class ChadoCustomTablesAPITest extends BrowserTestBase {

  protected $defaultTheme = 'stark';

  /**
   * Modules to enable.
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * Schema to do testing out of.
   * @var string
   */
  protected static $schemaName = 'testchado';


  /**
   * Tests chado.cv associated functions.
   *
   * @group tripal-chado
   * @group chado-cv
   */
  public function testcreatecustomtable() {
    // Reference: https://api.drupal.org/api/drupal/core%21tests%21Drupal%21KernelTests%21Core%21Database%21SchemaTest.php/class/SchemaTest/8.8.x
    $connection = \Drupal\Core\Database\Database::getConnection();

    // Create a new custom table
    $table_name = 'tripal_test';
    $table = array(
      'table' => 'tripal_test',
      'fields' => array (
        'feature_id' => array (
          'type' => 'serial',
          'not null' => true,
        ),
        'organism_id' => array (
          'type' => 'int',
          'not null' => true,
        ),
        'uniquename' => array (
          'type' => 'text',
          'not null' => true,
        ),
        'type_name' => array (
          'type' => 'varchar',
          'length' => '1024',
          'not null' => true,
        ),
      ),
    );

    // IMPORTANT: We decided to cancel testing these features
    // due to the way in which Drupal 8/9 handles Postgres 'schemas' within a db.

    // // This code will probably be broken since I reverted back to the standard
    // // code which does not have the argument isFunctionalTest = TRUE in the functions.
    // // $chado_schema = chado_get_schema_name('chado'); // Todo - is this really how we want to get the schema name?
    // // Does not seem like it would work with multi-chado instances.
    // global $chado_dot;
    // $chado_dot = "";

    // // Create a table
    // chado_create_custom_table($table_name, $table, FALSE, NULL, FALSE, TRUE);

    // // Test to see whether the table gets created
    // $this->assertTrue($connection->schema()->tableExists($table_name), 'ERROR: The custom table was not created.');

    // // Delete the table (THIS IS INCOMPATIBLE AND FAILS SINCE A TABLE ID IS NEEDED)
    // chado_delete_custom_table($table_name, TRUE);
    // $this->assertFalse($connection->schema()->tableExists($table_name), 'ERROR: The custom table was not deleted.');


  }

}
