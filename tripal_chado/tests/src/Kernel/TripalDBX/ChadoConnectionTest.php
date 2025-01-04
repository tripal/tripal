<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests for ChadoConnection.
 *
 * @group Tripal Chado
 * @group ChadoConnection
 */
class ChadoConnectionTest extends ChadoTestKernelBase {

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
   *   Each scenario is just a version of chado as a string.
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
   * Tests that we can create use CRUD on all Chado schema versions.
   *
   * @dataProvider provideChadoSchemaVersions
   */
  public function testCRUDChadoSchema(string $version, int $init_level) {

    // Get Chado in place
    $chado_connection = $this->getTestSchema(
      $init_level,
      $version
    );
    $this->assertInstanceOf(
      'Drupal\tripal_chado\Database\ChadoConnection',
      $chado_connection,
      "Unable to create test chado with the specified version (i.e. $version)."
    );

    // Ensure the version returned by ChadoConnection matches what we asked for.
    $this->assertEquals(
      $version,
      $chado_connection->getVersion(),
      "We expect that the chado version returned by the connection matches what we requested."
    );

  }

}
