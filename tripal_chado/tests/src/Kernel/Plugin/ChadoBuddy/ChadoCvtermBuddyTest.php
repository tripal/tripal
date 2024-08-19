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
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests the xxxCv() methods.
   */
  public function testCvMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_cvterm_buddy', []);

    // TEST: if there is no record then it should return an empty array when we try to get it.
    $chado_buddy_records = $instance->getCv(['cv.name' => 'nowaydoesthisexist']);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a CV that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a CV that does not exist');

    // TEST: We should be able to insert a CV record if it doesn't exist.
    $chado_buddy_record = $instance->insertCv(['cv.name' => 'newCv001', 'cv.definition' => 'def001']);
    $this->assertIsObject($chado_buddy_record, 'We did not insert a new CV "newCv001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new CV "newCv001"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the new CV "newCv001"');
    $cv_id = $chado_buddy_record->getValue('cv.cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new CV "newCv001"');

    // TEST: We should be able to update an existing CV record.
    $chado_buddy_record = $instance->updateCv(['cv.name' => 'newCv002', 'cv.definition' => 'def002'], ['cv.name' => 'newCv001']);
    $this->assertIsObject($chado_buddy_record, 'We did not update an existing CV "newCv001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated CV "newCv001"');
    $this->assertEquals('newCv002', $values['cv.name'], 'The CV name was not updated for CV "newCv001"');
    $this->assertEquals('def002', $values['cv.definition'], 'The CV definition was not updated for CV "newCv001"');

    // TEST: Upsert should insert a CV record that doesn't exist.
    $chado_buddy_record = $instance->upsertCv(['cv.name' => 'newCv003', 'cv.definition' => 'def003']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert a new CV "newCv003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new CV "newCv003"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the new CV "newCv003"');
    $cv_id = $chado_buddy_record->getValue('cv.cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new CV "newCv003"');

    // TEST: Upsert should update a CV record that does exist.
    // Conditions should not include definition
    $chado_buddy_record = $instance->upsertCv(['cv.name' => 'newCv003', 'cv.definition' => 'def004']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert an existing CV "newCv003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted CV "newCv003"');
    $this->assertEquals(3, count($values), 'The values array is of unexpected size for the upserted CV "newCv003"');
    $cv_id = $chado_buddy_record->getValue('cv.cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the upserted CV "newCv003"');
    $this->assertEquals('def004', $values['cv.definition'], 'The CV definition was not updated for the upserted CV "newDb003"');

    // TEST: we should be able to get the two records created above. Will also catch if upsert did an insert instead of update.
    foreach (['newCv002', 'newCv003'] as $cv_name) {
      $chado_buddy_records = $instance->getCv(['cv.name' => $cv_name]);
      $this->assertEquals(1, count($chado_buddy_records), "We did not retrieve the existing CV \"$cv_name\"");
      $values = $chado_buddy_records[0]->getValues();
      $base_table = $chado_buddy_records[0]->getBaseTable();
      $schema_name = $chado_buddy_records[0]->getSchemaName();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing CV \"$cv_name\"");
      $this->assertEquals(3, count($values), "The values array is of unexpected size for the existing CV \"$cv_name\"");
      $this->assertEquals('cv', $base_table, 'The base table is incorrect for the existing CV \"$cv_name\"');
      $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the existing CV \"$cv_name\"');
    }

    // TEST: query should be case sensitive
    $chado_buddy_records = $instance->getCv(['cv.name' => 'NEWcv003'], []);
    $this->assertEquals(0, count($chado_buddy_records), "We received case insensitive results for getCv when we should not have");

    // TEST: case insensitive override should work
    $chado_buddy_records = $instance->getCv(['cv.name' => 'NEWcv003'],
                                            ['case_insensitive' => ['cv.name']]);
    $this->assertEquals(1, count($chado_buddy_records), "We did not receive case insensitive results for getCv when we should have");

    // TEST: we can pass values as a ChadoBuddyRecord
    $values = ['buddy_record' => $chado_buddy_records[0]];
    $chado_buddy_records = $instance->getCv($values, []);
    $this->assertEquals(1, count($chado_buddy_records), "We did not receive the Cv when querying using a ChadoBuddyRecord");

    // TEST: We should not be able to insert a CV record if it does exist.
    // Run last because this causes an exception.
    $this->expectException(\Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException::class);
    $chado_buddy_records = $instance->insertCv(['cv.name' => 'local', 'cv.definition' => 'def003']);

  }

  /**
   * Tests the xxxCvterm() methods.
   */
  public function testCvtermMethods() {

    $type = \Drupal::service('tripal_chado.chado_buddy');
    $instance = $type->createInstance('chado_cvterm_buddy', []);

    // TEST: if there is no record then it should return an empty array when we try to get it.
    $chado_buddy_records = $instance->getCvterm(['cvterm.name' => 'nowaydoesthisexist']);
    $this->assertIsArray($chado_buddy_records, 'We did not retrieve an array for a Cvterm that does not exist');
    $this->assertEquals(0, count($chado_buddy_records), 'We did not retrieve an empty array for a Cvterm that does not exist');

    // TEST: We should be able to insert a Cvterm record if it doesn't exist. We must include enough info to create a dbxref also.
    $chado_buddy_record = $instance->insertCvterm(['cvterm.name' => 'newCvterm001', 'cvterm.definition' => 'def001',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc001']);
    $this->assertIsObject($chado_buddy_record, 'We did not insert a new Cvterm "newCvterm001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Cvterm "newCvterm001"');
    $this->assertEquals(20, count($values), 'The values array is of unexpected size for the new Cvterm "newCvterm001"');
    $cvterm_id = $chado_buddy_record->getValue('cvterm.cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the new Cvterm "newCvterm001"');
    $cv_id = $chado_buddy_record->getValue('cv.cv_id');
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new Cvterm "newCvterm001"');

    // TEST: We should be able to update an existing Cvterm record.
    $chado_buddy_record = $instance->updateCvterm(['cvterm.name' => 'newCvterm002', 'cvterm.definition' => 'def002'], ['cvterm.name' => 'newCvterm001']);
    $this->assertIsObject($chado_buddy_record, 'We did not update an existing Cvterm "newCvterm001"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated CV "newCvterm001"');
    $this->assertEquals('newCvterm002', $values['cvterm.name'], 'The Cvterm name was not updated for Cvterm "newCvterm001"');
    $this->assertEquals('def002', $values['cvterm.definition'], 'The Cvterm definition was not updated for Cvterm "newCvterm001"');

    // TEST: Upsert should insert a Cvterm record that doesn't exist.
    $chado_buddy_record = $instance->upsertCvterm(['cvterm.name' => 'newCvterm003', 'cvterm.definition' => 'def003',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc003']);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert a new Cvterm "newCvterm003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the new Cvterm "newCvterm003"');
    $this->assertEquals(20, count($values), 'The values array is of unexpected size for the new Cvterm "newCvterm003"');
    $cvterm_id = $chado_buddy_record->getValue('cvterm.cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the new Cvterm "newCvterm003"');
    $this->assertEquals(0, $values['cvterm.is_obsolete'], 'The Cvterm is_obsolete value was not set to its default value for the new Cvterm "newCvterm003"');

    // TEST: Upsert should update a Cvterm record that does exist.
    // Conditions should not include definition or is_relationshiptype.
    // Note that is_obsolete is part of a unique constraint and is an integer, as is is_relationshiptype
    $chado_buddy_record = $instance->upsertCvterm(['cvterm.name' => 'newCvterm003', 'cvterm.definition' => 'def004',
                                                   'cvterm.is_obsolete' => 0, 'cvterm.is_relationshiptype' => 1]);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert an existing Cvterm "newCvterm003"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted Cvterm "newCvterm003"');
    $this->assertEquals(20, count($values), 'The values array is of unexpected size for the upserted Cvterm "newCvterm003"');
    $cvterm_id = $chado_buddy_record->getValue('cvterm.cvterm_id');
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the upserted Cvterm "newCvterm003"');
    $this->assertEquals('def004', $values['cvterm.definition'], 'The Cvterm definition was not updated for the upserted Cvterm "newCvterm003"');
    $this->assertEquals(0, $values['cvterm.is_obsolete'], 'The Cvterm is_obsolete value was incorrectly updated for the upserted Cvterm "newCvterm003"');
    $this->assertEquals(1, $values['cvterm.is_relationshiptype'], 'The Cvterm is_relationshiptype value was not updated for the upserted Cvterm "newCvterm003"');

    // TEST: we should be able to get the two records created above. Will also catch if upsert did an insert instead of update.
    foreach (['newCvterm002', 'newCvterm003'] as $cvterm_name) {
      $chado_buddy_records = $instance->getCvterm(['cvterm.name' => $cvterm_name]);
      $this->assertEquals(1, count($chado_buddy_records), "We did not retrieve the existing Cvterm \"$cvterm_name\"");
      $values = $chado_buddy_records[0]->getValues();
      $base_table = $chado_buddy_records[0]->getBaseTable();
      $schema_name = $chado_buddy_records[0]->getSchemaName();
      $this->assertIsArray($values, "We did not retrieve an array of values for the existing Cvterm \"$cvterm_name\"");
      $this->assertEquals(20, count($values), "The values array is of unexpected size for the existing Cvterm \"$cvterm_name\"");
      $this->assertEquals('cvterm', $base_table, 'The base table is incorrect for the existing Cvterm \"$cvterm_name\"');
      $this->assertTrue(str_contains($schema_name, '_test_chado_'), 'The schema is incorrect for the existing Cvterm \"$cvterm_name\"');
    }

    // TEST: query should be case sensitive
    $chado_buddy_records = $instance->getCvterm(['db.name' => 'LOCAL', 'cv.name' => 'Local', 'cvterm.name' => 'NEWCvTerm003'], []);
    $this->assertEquals(0, count($chado_buddy_records), "We received case insensitive results for getCvterm when we should not have");

    // TEST: case insensitive override should work
    $chado_buddy_records = $instance->getCvterm(['db.name' => 'LOCAL', 'cv.name' => 'Local', 'cvterm.name' => 'NEWCvTerm003'],
                                                ['case_insensitive' => ['db.name', 'cv.name', 'cvterm.name']]);
    $this->assertEquals(1, count($chado_buddy_records), "We did not receive case insensitive results for getCvterm when we should have");

    // TEST: We should be able to retrieve an existing Cvterm record by its dbxref accession.
    $chado_buddy_records = $instance->getCvterm(['dbxref.accession' => 'newAcc003']);
    $this->assertEquals(1, count($chado_buddy_records), 'We did not retrieve the existing Cvterm with dbxref "newAcc003"');
    $values = $chado_buddy_records[0]->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing Cvterm with dbxref "newAcc003"');
    $this->assertEquals(20, count($values), 'The values array is of unexpected size for the existing Cvterm with dbxref "newAcc003"');

    // TEST: associate a cvterm with a base table.
    // The minimal test environment won't be able to automatically look up
    // the primary key for the feature table, so we have to pass 'pkey' in.
    $base_table = 'phenotype';
    $query = $this->connection->insert('1:' . $base_table)
      ->fields(['uniquename' => 'phen005'])
      ->execute();
    $linking_table = $base_table . '_cvterm';
    $status = $instance->associateCvterm($base_table, 1, $chado_buddy_records[0], []);
    $this->assertIsBool($status, "We did not retrieve a boolean when associating a cvterm with the base table \"$base_table\"");
    $this->assertTrue($status, "We did not retrieve TRUE when associating a cvterm with the base table \"$base_table\"");
    $query = $this->connection->select('1:' . $linking_table, 'lt')
      ->fields('lt', ['cvterm_id'])
      ->execute();
    $results = $query->fetchAll();
    $this->assertIsArray($results, "We should have been able to select from the \"$linking_table\" table");
    $this->assertCount(1, $results, "There should only be a single \"$linking_table\" record inserted");
    $expected_cvterm_id = $chado_buddy_records[0]->getValue('cvterm.cvterm_id');
    $retrieved_cvterm_id = $results[0]->cvterm_id;
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      "We did not get the cvterm_id from \"$linking_table\" that should have been set by associateCvterm");

    // TEST: We should be able to create a synonym.
    $chado_buddy_record = $instance->insertCvtermSynonym(['cv.name' => 'local', 'cvterm.cvterm_id' => $expected_cvterm_id,
                                                           'cvtermsynonym.synonym' => 'syn005', 'cvtermsynonym.type_id' => 5]);
    $this->assertIsObject($chado_buddy_record, 'We did not retrieve the existing Cvterm with synonym "syn005"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing Cvterm with synonym "syn005"');
    $this->assertEquals(24, count($values), 'The values array is of unexpected size for the existing Cvterm with synonym "syn005"');
    $retrieved_cvterm_id = $chado_buddy_record->getValue('cvterm.cvterm_id');
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the existing Cvterm with synonym "syn005"');

    // TEST: We should be able to update this synonym and change its name
    $chado_buddy_record = $instance->updateCvtermSynonym(['cvtermsynonym.synonym' => 'syn006', 'cvtermsynonym.type_id' => 6],
                                                          ['cvtermsynonym.synonym' => 'syn005'], []);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert the existing Cvterm with synonym "syn005"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the updated Cvterm with synonym "syn005"');
    $this->assertEquals(24, count($values), 'The values array is of unexpected size for the updated Cvterm with synonym "syn005"');
    $retrieved_cvterm_id = $chado_buddy_record->getValue('cvterm.cvterm_id');
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the dated Cvterm with synonym "syn005"');
    $this->assertEquals('syn006', $values['cvtermsynonym.synonym'],
      'We did not update the synonym name for the updated Cvterm with synonym "syn005"');
    $this->assertEquals(6, $values['cvtermsynonym.type_id'],
      'We did not update the type_id for the updated Cvterm with synonym "syn005"');

    // TEST: We should be able to upsert this synonym
    $chado_buddy_record = $instance->upsertCvtermSynonym(['cv.name' => 'local', 'cvtermsynonym.synonym' => 'syn006', 'cvtermsynonym.type_id' => 7], []);
    $this->assertIsObject($chado_buddy_record, 'We did not upsert the existing Cvterm with synonym "syn006"');
    $values = $chado_buddy_record->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the upserted Cvterm with synonym "syn006"');
    $this->assertEquals(24, count($values), 'The values array is of unexpected size for the upserted Cvterm with synonym "syn006"');
    $retrieved_cvterm_id = $chado_buddy_record->getValue('cvterm.cvterm_id');
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the upserted Cvterm with synonym "syn006"');
    $this->assertEquals(7, $values['cvtermsynonym.type_id'],
      'We did not update the type_id for the upserted Cvterm with synonym "syn006"');

    // TEST: We should be able to retrieve this synonym
    $chado_buddy_records = $instance->getCvtermSynonym(['cv.name' => 'local', 'cvtermsynonym.synonym' => 'syn006']);
    $this->assertEquals(1, count($chado_buddy_records), 'We did not retrieve the existing Cvterm with synonym "syn006"');
    $values = $chado_buddy_records[0]->getValues();
    $this->assertIsArray($values, 'We did not retrieve an array of values for the existing Cvterm with synonym "syn006"');
    $this->assertEquals(24, count($values), 'The values array is of unexpected size for the existing Cvterm with synonym "syn006"');
    $retrieved_cvterm_id = $chado_buddy_records[0]->getValue('cvterm.cvterm_id');
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the existing Cvterm with synonym "syn006"');

    // TEST: We should not be able to insert a Cvterm if it does exist.
    // Run last because this causes an exception.
    $this->expectException(\Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException::class);
    $chado_buddy_records = $instance->insertCvterm(['cvterm.name' => 'newCvterm001', 'cvterm.definition' => 'def001',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc001']);
  }
}
