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
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated CV "newCv001"');
    $this->assertEquals('newCv002', $values['name'], 'The CV name was not updated for CV "newCv001"');
    $this->assertEquals('def002', $values['definition'], 'The CV definition was not updated for CV "newCv001"');

    // TEST: Upsert should insert a CV record that doesn't exist.
    $chado_buddy_records = $instance->upsertCv(['name' => 'newCv003', 'definition' => 'def003']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert a new CV "newCv003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new CV "newCv003"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the new CV "newCv003"');
    $cv_id = $chado_buddy_records->getValue('cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new CV "newCv003"');

    // TEST: Upsert should update a CV record that does exist.
    $chado_buddy_records = $instance->upsertCv(['name' => 'newCv003', 'definition' => 'def003']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert an existing CV "newCv003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted CV "newCv003"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the upserted CV "newCv003"');
    $cv_id = $chado_buddy_records->getValue('cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the upserted CV "newCv003"');

    // TEST: we should be able to get the two records created above.
    foreach (['newCv002', 'newCv003'] as $cv_name) {
      $chado_buddy_records = $instance->getCv(['name' => $cv_name]);
      $this->assertIsObject($chado_buddy_records, "We did not retrieve the existing CV \"$cv_name\"");
      $values = $chado_buddy_records->getValues();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing CV \"$cv_name\"");
      $this->assertEquals(3, count($values), "The values array is of unexpected size for the existing CV \"$cv_name\"");
    }

    // TEST: We should not be able to insert a CV record if it does exist.
    // Run last because this causes an exception.
    $this->expectException(\Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException::class);
    $chado_buddy_records = $instance->insertCv(['name' => 'test_cv', 'definition' => 'def003']);

  }

  /**
   * Tests the getCvterm(), insertCvterm(), updateCvterm(), upsertCvterm() methods.
   * Focuses on those expected to work ;-)
   */
  public function testCvtermMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_cvterm_buddy', []);

    // TEST: if there is no record then it should return false when we try to get it.
    $chado_buddy_records = $instance->getCvterm(['name' => 'nowaydoesthisexist']);
    $this->assertFalse($chado_buddy_records, 'We did not retrieve FALSE for a Cvterm that does not exist');

    // TEST: We should be able to retrieve an existing Cvterm record. Dummy chado has 'test_cvterm', 'CV term for testing', 1
    $chado_buddy_records = $instance->getCvterm(['name' => 'test_cvterm']);
    $this->assertIsObject($chado_buddy_records, 'We did not retrieve the existing Cvterm "test_cvterm"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing Cvterm "test_cvterm"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the existing Cvterm "test_cvterm"');

    // TEST: We should be able to retrieve an existing Cvterm record by its dbxref accession.
    $chado_buddy_records = $instance->getCvterm(['term_accession' => 'test_dbxref']);
    $this->assertIsObject($chado_buddy_records, 'We did not retrieve the existing Cvterm with dbxref "test_dbxref"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing Cvterm with dbxref "test_dbxref"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the existing Cvterm with dbxref "test_dbxref"');

    // TEST: We should be able to insert a Cvterm record if it doesn't exist.
    $chado_buddy_records = $instance->insertCvterm(['name' => 'newCvterm001', 'definition' => 'def001', 'cv_name' => 'test_cv']);
    $this->assertIsObject($chado_buddy_records, 'We did not insert a new Cvterm "newCvterm001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Cvterm "newCvterm001"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the new Cvterm "newCvterm001"');
    $cvterm_id = $chado_buddy_records->getValue('cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the new Cvterm "newCvterm001"');
    $cv_id = $chado_buddy_records->getValue('cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new Cvterm "newCvterm001"');

    // TEST: We should be able to update an existing Cvterm record.
    $chado_buddy_records = $instance->updateCvterm(['name' => 'newCvterm002', 'definition' => 'def002'], ['name' => 'newCvterm001']);
    $this->assertIsObject($chado_buddy_records, 'We did not update an existing Cvterm "newCvterm001"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated CV "newCvterm001"');
    $this->assertEquals('newCvterm002', $values['name'], 'The Cvterm name was not updated for Cvterm "newCvterm001"');
    $this->assertEquals('def002', $values['definition'], 'The Cvterm definition was not updated for Cvterm "newCvterm001"');

    // TEST: Upsert should insert a Cvterm record that doesn't exist.
    $chado_buddy_records = $instance->upsertCvterm(['name' => 'newCvterm003', 'definition' => 'def003', 'cv_name' => 'test_cv']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert a new Cvterm "newCvterm003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Cvterm "newCvterm003"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the new Cvterm "newCvterm003"');
    $cvterm_id = $chado_buddy_records->getValue('cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the new Cvterm "newCvterm003"');

    // TEST: Upsert should update a Cvterm record that does exist.
    $chado_buddy_records = $instance->upsertCvterm(['name' => 'newCvterm003', 'definition' => 'def003']);
    $this->assertIsObject($chado_buddy_records, 'We did not upsert an existing Cvterm "newCvterm003"');
    $values = $chado_buddy_records->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted Cvterm "newCvterm003"');
    $this->assertEquals(10, count($values), 'The values array is of unexpected size for the upserted Cvterm "newCvterm003"');
    $cvterm_id = $chado_buddy_records->getValue('cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the upserted Cvterm "newCvterm003"');

    // TEST: we should be able to get the two records created above.
    foreach (['newCvterm002', 'newCvterm003'] as $cvterm_name) {
      $chado_buddy_records = $instance->getCvterm(['name' => $cvterm_name]);
      $this->assertIsObject($chado_buddy_records, "We did not retrieve the existing Cvterm \"$cvterm_name\"");
      $values = $chado_buddy_records->getValues();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing Cvterm \"$cvterm_name\"");
      $this->assertEquals(10, count($values), "The values array is of unexpected size for the existing Cvterm \"$cvterm_name\"");
    }

    // TEST: associate a cvterm with a base table.
    // The minimal test environment won't be able to automatically look up
    // the primary key for the feature table, so we have to pass 'pkey' in.
    $base_table = 'feature';
    $linking_table = $base_table . '_cvterm';
    $status = $instance->associateCvterm($base_table, 1, $chado_buddy_records, ['pkey' => $base_table . '_id']);
    $this->assertIsBool($status, "We did not retrieve a boolean when associating a cvterm with the base table \"$base_table\"");
    $this->assertTrue($status, "We did not retrieve TRUE when associating a cvterm with the base table \"$base_table\"");
    $query = $this->connection->select('1:' . $linking_table, 'lt')
      ->fields('lt', ['cvterm_id'])
      ->execute();
    $results = $query->fetchAll();
    $this->assertIsArray($results, "We should have been able to select from the \"$linking_table\" table");
    $this->assertCount(1, $results, "There should only be a single \"$linking_table\" record inserted");
    $expected_cvterm_id = $chado_buddy_records->getValue('cvterm_id');
    $retrieved_cvterm_id = $results[0]->cvterm_id;
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      "We did not get the cvterm_id from \"$linking_table\" that should have been set by associateCvterm");
  }
}
