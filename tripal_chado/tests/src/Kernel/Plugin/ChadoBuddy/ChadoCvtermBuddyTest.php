<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Cvterm Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoCvtermBuddyTest extends ChadoTestKernelBase {
  protected $defaultTheme = 'stark';

  protected ChadoConnection $connection;

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to a test Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::INIT_DUMMY);
  }

  /**
   * Tests the getCv(), insertCv(), updateCv(), upsertCv() methods.
   * Focuses on those expected to work ;-)
   */
  public function testCvMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_cvterm_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getCv(['name' => 'nowaydoesthisexist']);
    $this->assertFalse($chado_buddy_records, 'We did not retrieve FALSE for a CV that does not exist');

    // TEST: We should be able to retrieve an existing CV record. Dummy chado has 'test_cv', 'CV for testing'
    $chado_buddy_records = $instance->getCv(['name' => 'test_cv']);
    $this->assertIsObject($chado_buddy_records, 'We did not retrieve the existing CV "test_cv"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing CV "test_cv"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the existing CV "test_cv"');

    // TEST: We should be able to insert a CV record if it doesn't exist.
    $chado_buddy_records = $instance->insertCv(['name' => 'newCv001', 'definition' => 'def001']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new CV "newCv001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new CV "newCv001"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the new CV "newCv001"');
    $cv_id = $chado_buddy_records->getValue('cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new CV "newCv001"');

    // TEST: We should be able to update an existing CV record.
    $chado_buddy_records = $instance->updateCv(['name' => 'newCv002', 'definition' => 'def002'], ['name' => 'newCv001']);
    $this->assertIsObject($chado_buddy_records, 'We did not update an existing CV "newCv001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updateed CV "newCv001"');
    $this->assertEquals('newCv002', $values['name'], 'The CV name was not updated for CV "newCv001"');
    $this->assertEquals('def002', $values['definition'], 'The CV definition was not updated for CV "newCv001"');

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

    // TEST: We should not be able to insert a CV record if it does exist. Run last because this causes an exception.
    $this->expectException(\Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException::class);
    $chado_buddy_records = $instance->insertCv(['name' => 'test_cv', 'definition' => 'def003']);

  }

  /**
   * Tests the getCvterm(), insertCvterm(), updateCvterm(), upsertCvterm() methods.
   * Focuses on those expected to work ;-)
   */
  public function testCvtermMethods() {

    $this->markTestIncomplete(
          'This test has not been implemented yet.'
    );

    // TEST: if there is no record then it should return false when we try to get it.

    // TEST: We should be able to insert a record if it doesn't exist.

    // TEST: We should be able to update an existing record.

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

  }
}
