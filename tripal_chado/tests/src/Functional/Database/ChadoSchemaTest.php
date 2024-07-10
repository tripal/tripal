<?php

namespace Drupal\Tests\tripal_chado\Functional;

/**
 * Tests for Chado Schema implementation of Tripal DBX Connection.
 *
 * @group Tripal
 * @group Tripal TripalDBX
 * @group Tripal TripalDBX Chado
 * @group TripalDBX Chado
 */
class ChadoSchemaTest extends ChadoTestBrowserBase {
  /**
   * Test the ChadoSchema::getDefault() method.
   *
   * We will test that the default chado in this test returns the name set
   * by the chadoTestTrait when a new test schema was created. The chadoTestTrait
   * also sets the new chado schema created for testing to the default within
   * the test environment.
   *
   * There is no way to programatically set the default chado, so we will not
   * test changing the default and seeing if the default is still reported
   * correctly afterwards.
   */
  public function testGetDefault() {
    $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);
    $chado = \Drupal::service('tripal_chado.database');
    $default_chado_schema = $chado->schema()->getDefault();

    // Test if the reported Chado version matches the test chado format,
    // e.g. _test_chado_h87g97hkln64vy76
    $this->assertRegExp('/(\_test\_chado\_[\w]{16})\b/', $default_chado_schema, "The default Chado schema returned did not match the format we expected within the testing environment.");

    // Test if the default chado schema returned matches the
    // one we created.
    $this->assertEquals($this->testSchemaName, $default_chado_schema,
      "The default chado schema name we retrieved did not match the test chado schema we just created.");
  }
}
