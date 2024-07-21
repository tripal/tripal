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

  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected ChadoConnection $connection;

  /**
   * Tests the getCv(), insertCv(), updateCv(), upsertCv() methods.
   * Focuses on those expected to work ;-)
   */
  public function testCvMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_cvterm_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getCv(['name' => 'nowaydoesthisexist']);
    $this->assertFalse($chado_buddy_records, 'We retrieved a CV when one does not exist');

    // TEST: We should be able to retrieve an existing CV record.
    $chado_buddy_records = $instance->getCv(['name' => 'local']);
    $this->assertIsObject($chado_buddy_records, 'We did not retrieve the existing CV "local"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing CV "local"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the existing CV "local"');

    // TEST: We should be able to insert a CV record if it doesn't exist.
    $chado_buddy_records = $instance->insertCv(['name' => 'newCv000001', 'definition' => 'definition000001']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new CV "newCv000001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new CV "newCv000001"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the new CV "newCv000001"');
    $cv_id = $chado_buddy_records->getValue('cv_id');
    $this->assertIsInt($cv_id, 'We did not retrieve an integer cv_id for the new CV "newCv000001"');

    // TEST: We should not be able to insert a CV record if it does exist.
    $this->expectException(\Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException::class);
    $chado_buddy_records = $instance->insertCv(['name' => 'newCv000001', 'definition' => 'definition000001']);

    // TEST: We should be able to update an existing CV record.
//not written yet    $chado_buddy_records = $instance->upsertCv(['name' => 'newCv000001', 'definition' => 'definition000002']);
//    $this->assertIsObject($chado_buddy_records, 'We did not upsert an existing CV "newCv000001"');
//    $values = $chado_buddy_records->getValues();
//    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted CV "newCv000001"');
//    $this->assertEquals('definition000002', $values['definition'], 'The CV definition was not updated for CV "newCv000001"');

    // TEST: Upsert should insert a record that doesn't exist.

    // TEST: Upsert should update a record that does exist.

    // TEST: we should be able to get the two records created above.

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
