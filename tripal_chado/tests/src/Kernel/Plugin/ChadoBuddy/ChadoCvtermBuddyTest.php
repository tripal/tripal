<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoBuddy\ChadoTestBuddyBase;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests the Chado Cvterm Buddy.
 *
 * @group ChadoBuddy
 */
class ChadoCvtermBuddyTest extends ChadoTestBuddyBase {

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
    $test_records = [];
    $test_records['set'] = $instance->insertCv(['cv.name' => 'newCv001', 'cv.definition' => 'def001']);
    $test_records['get'] = $instance->getCv(['cv.name' => 'newCv001', 'cv.definition' => 'def001']);
    $values = $this->multiAssert('insertCv', $test_records, 'cv', 'cv.cv_id', 'cv "newCv001"', 3);
    $cv_id = $values['get']['cv.cv_id'];
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new CV "newCv001"');

    // TEST: Updating a non-existent CV should return FALSE.
    $chado_buddy_records = $instance->updateCv(['cv.name' => 'newCv002', 'cv.definition' => 'def002'],
                                               ['cv.name' => 'does-not-exist']);
    $this->assertFalse($chado_buddy_records, "We received a value other than FALSE for an update to a CV that does not exist");

    // TEST: We should be able to update an existing CV record.
    $test_records = [];
    $test_records['set'] = $instance->updateCv(['cv.name' => 'newCv002', 'cv.definition' => 'def002'],
                                               ['cv.name' => 'newCv001']);
    $test_records['get'] = $instance->getCv(['cv.name' => 'newCv002']);
    $values = $this->multiAssert('updateCv', $test_records, 'cv', 'cv.cv_id', 'cv "newCv002"', 3);
    $this->assertEquals('newCv002', $values['get']['cv.name'], 'The CV name was not updated for CV "newCv001"');
    $this->assertEquals('def002', $values['get']['cv.definition'], 'The CV definition was not updated for CV "newCv001"');

    // TEST: Upsert should insert a CV record that doesn't exist.
    $test_records = [];
    $test_records['set'] = $instance->upsertCv(['cv.name' => 'newCv003', 'cv.definition' => 'def003']);
    $test_records['get'] = $instance->getCv(['cv.name' => 'newCv003', 'cv.definition' => 'def003']);
    $values = $this->multiAssert('upsertCv', $test_records, 'cv', 'cv.cv_id', 'cv "newCv003"', 3);
    $cv_id = $values['get']['cv.cv_id'];
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new CV "newCv003"');

    // TEST: Upsert should update a CV record that does exist.
    // Conditions should not include definition
    $test_records = [];
    $test_records['set'] = $instance->upsertCv(['cv.name' => 'newCv003', 'cv.definition' => 'def004']);
    $test_records['get'] = $instance->getCv(['cv.name' => 'newCv003']);
    $values = $this->multiAssert('upsertCv', $test_records, 'cv', 'cv.cv_id', 'cv "newCv003"', 3);
    $cv_id = $values['get']['cv.cv_id'];
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the upserted CV "newCv003"');
    $this->assertEquals('def004', $values['get']['cv.definition'], 'The CV definition was not updated for the upserted CV "newDb003"');

    // TEST: We should not be able to insert a CV record if it does exist.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_records = $instance->insertCv(['cv.name' => 'local', 'cv.definition' => 'def003']);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when inserting a CV record that already exists.');
    $this->assertStringContainsString('already exists', $exception_message, "We did not get the exception message we expected when inserting a CV record that already exists.");
    $chado_buddy_records = $instance->getCv(['cv.definition' => 'def003']);
    $this->assertEquals(0, count($chado_buddy_records), "A cv was incorrectly inserted when it already exists");

    // TEST: we should be able to get the two records created above. Will also catch if upsert did an insert instead of update.
    foreach (['newCv002', 'newCv003'] as $cv_name) {
      $test_records = [];
      $test_records['get'] = $instance->getCv(['cv.name' => $cv_name]);
      $this->multiAssert('getCv', $test_records, 'cv', 'cv.cv_id', 'cv "'.$cv_name.'"', 3);
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
    $test_records = [];
    $test_records['set'] = $instance->insertCvterm(['cvterm.name' => 'newCvterm001', 'cvterm.definition' => 'def001',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc001']);
    $test_records['get'] = $instance->getCvterm(['cvterm.name' => 'newCvterm001', 'cvterm.definition' => 'def001',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc001']);
    $values = $this->multiAssert('insertCvterm', $test_records, 'cvterm', 'cvterm.cvterm_id', 'cvterm "newCvterm001"', 20);
    $cvterm_id = $values['get']['cvterm.cvterm_id'];
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the new Cvterm "newCvterm001"');
    $cv_id = $values['get']['cv.cv_id'];
    $this->assertTrue(is_numeric($cv_id), 'We did not retrieve an integer cv_id for the new Cvterm "newCvterm001"');

    // TEST: Updating a non-existent Cvterm should return FALSE.
    $chado_buddy_records = $instance->updateCvterm(['cvterm.name' => 'newCvterm002', 'cvterm.definition' => 'def002'],
                                                   ['cvterm.name' => 'does-not-exist']);
    $this->assertFalse($chado_buddy_records, "We received a value other than FALSE for an update to a Cvterm that does not exist");

    // TEST: We should be able to update an existing Cvterm record.
    $test_records = [];
    $test_records['set'] = $instance->updateCvterm(['cvterm.name' => 'newCvterm002', 'cvterm.definition' => 'def002'],
                                                   ['cvterm.name' => 'newCvterm001']);
    $test_records['get'] = $instance->getCvterm(['cvterm.name' => 'newCvterm002', 'cvterm.definition' => 'def002']);
    $values = $this->multiAssert('updateCvterm', $test_records, 'cvterm', 'cvterm.cvterm_id', 'cvterm "newCvterm001"', 20);
    $this->assertEquals('newCvterm002', $values['get']['cvterm.name'], 'The Cvterm name was not updated for Cvterm "newCvterm001"');
    $this->assertEquals('def002', $values['get']['cvterm.definition'], 'The Cvterm definition was not updated for Cvterm "newCvterm001"');

    // TEST: Upsert should insert a Cvterm record that doesn't exist.
    $test_records = [];
    $test_records['set'] = $instance->upsertCvterm(['cvterm.name' => 'newCvterm003', 'cvterm.definition' => 'def003',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc003']);
    $test_records['get'] = $instance->getCvterm(['cvterm.name' => 'newCvterm003', 'cvterm.definition' => 'def003',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc003']);
    $values = $this->multiAssert('upsertCvterm', $test_records, 'cvterm', 'cvterm.cvterm_id', 'cvterm "newCvterm003"', 20);
    $cvterm_id = $values['get']['cvterm.cvterm_id'];
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the new Cvterm "newCvterm003"');
    $this->assertEquals(0, $values['get']['cvterm.is_obsolete'], 'The Cvterm is_obsolete value was not set to its default value for the new Cvterm "newCvterm003"');

    // TEST: Upsert should update a Cvterm record that does exist.
    // Conditions should not include definition or is_relationshiptype.
    // Note that is_obsolete is part of a unique constraint and is an integer, as is is_relationshiptype
    $test_records = [];
    $test_records['set'] = $instance->upsertCvterm(['cvterm.name' => 'newCvterm003', 'cvterm.definition' => 'def004',
                                                    'cvterm.is_obsolete' => 0, 'cvterm.is_relationshiptype' => 1]);
    $test_records['get'] = $instance->getCvterm(['cvterm.name' => 'newCvterm003', 'cvterm.definition' => 'def004',
                                                 'cvterm.is_obsolete' => 0, 'cvterm.is_relationshiptype' => 1]);
    $values = $this->multiAssert('upsertCvterm', $test_records, 'cvterm', 'cvterm.cvterm_id', 'cvterm "def004"', 20);
    $cvterm_id = $values['get']['cvterm.cvterm_id'];
    $this->assertTrue(is_numeric($cvterm_id), 'We did not retrieve an integer cvterm_id for the upserted Cvterm "newCvterm003"');
    $this->assertEquals('def004', $values['get']['cvterm.definition'], 'The Cvterm definition was not updated for the upserted Cvterm "newCvterm003"');
    $this->assertEquals(0, $values['get']['cvterm.is_obsolete'], 'The Cvterm is_obsolete value was incorrectly updated for the upserted Cvterm "newCvterm003"');
    $this->assertEquals(1, $values['get']['cvterm.is_relationshiptype'], 'The Cvterm is_relationshiptype value was not updated for the upserted Cvterm "newCvterm003"');

    // TEST: We should not be able to insert a Cvterm record if it does exist.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_buddy_record = $instance->insertCvterm(['cvterm.name' => 'newCvterm001', 'cvterm.definition' => 'def001',
        'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc001']);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when inserting a Cvterm record that already exists.');
    $this->assertStringContainsString('already exists', $exception_message, "We did not get the exception message we expected when inserting a Cvterm record that already exists.");
    $chado_buddy_records = $instance->getCvterm(['cvterm.name' => 'newCvterm001', 'cvterm.definition' => 'def001',
      'cv.name' => 'local', 'db.name' => 'local', 'dbxref.accession' => 'newAcc001']);
    $this->assertEquals(0, count($chado_buddy_records), "A cv was incorrectly inserted when it already exists");

    // TEST: we should be able to get the two records created above. Will also catch if upsert did an insert instead of update.
    foreach (['newCvterm002', 'newCvterm003'] as $cvterm_name) {
      $test_records = [];
      $test_records['get'] = $instance->getCvterm(['cvterm.name' => $cvterm_name]);
      $values = $this->multiAssert('getCvterm', $test_records, 'cvterm', 'cvterm.cvterm_id', 'cvterm "'.$cvterm_name.'"', 20);
    }

    // TEST: query should be case sensitive
    $chado_buddy_records = $instance->getCvterm(['db.name' => 'LOCAL', 'cv.name' => 'Local', 'cvterm.name' => 'NEWCvTerm003'], []);
    $this->assertEquals(0, count($chado_buddy_records), "We received case insensitive results for getCvterm when we should not have");

    // TEST: case insensitive override should work
    $chado_buddy_records = $instance->getCvterm(['db.name' => 'LOCAL', 'cv.name' => 'Local', 'cvterm.name' => 'NEWCvTerm003'],
                                                ['case_insensitive' => ['db.name', 'cv.name', 'cvterm.name']]);
    $this->assertEquals(1, count($chado_buddy_records), "We did not receive case insensitive results for getCvterm when we should have");

    // TEST: We should be able to retrieve an existing Cvterm record by its dbxref accession.
    $test_records = [];
    $test_records['get'] = $instance->getCvterm(['dbxref.accession' => 'newAcc003']);
    $values = $this->multiAssert('getCvterm', $test_records, 'cvterm', 'cvterm.cvterm_id', 'cvterm from accession "newAcc003"', 20);

    // TEST: associate a cvterm with a base table.
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

    // TEST: associate a cvterm with a base table where there are required columns
    // in the linking table (i.e. pub_id), but we disable automatic lookup and
    // we don't include pub_id. Exception expected.
    $base_table = 'organism';
    $query = $this->connection->insert('1:' . $base_table)
      ->fields(['genus' => 'org005', 'species' => 'org005'])
      ->execute();
    $linking_table = $base_table . '_cvterm';
    $options = [
      'lookup_columns' => FALSE,
    ];
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $status = $instance->associateCvterm($base_table, 1, $chado_buddy_records[0], $options);
    } catch (ChadoBuddyException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue($exception_caught, 'We should get an exception when inserting associating a Cvterm without pub_id.');
    $this->assertStringContainsString('Not null violation', $exception_message, "We did not get the exception message we expected when associating a Cvterm without pub_id.");

    // TEST: associate a cvterm with a base table where there are required columns
    // in the linking table (i.e. pub_id). Tests the default auto-lookup functionality.
    $base_table = 'stock';
    $query = $this->connection->insert('1:' . $base_table)
      ->fields(['uniquename' => 'stock006', 'type_id' => 1])
      ->execute();
    $linking_table = $base_table . '_cvterm';
    $options = [];
    $status = $instance->associateCvterm($base_table, 1, $chado_buddy_records[0], $options);
    $this->assertIsBool($status, "We did not retrieve a boolean when associating a cvterm with the base table \"$base_table\"");
    $this->assertTrue($status, "We did not retrieve TRUE when associating a cvterm with the base table \"$base_table\"");
    $query = $this->connection->select('1:' . $linking_table, 'lt')
      ->fields('lt', ['cvterm_id', 'pub_id'])
      ->execute();
    $results = $query->fetchAll();
    $this->assertIsArray($results, "We should have been able to select from the \"$linking_table\" table");
    $this->assertCount(1, $results, "There should only be a single \"$linking_table\" record inserted");
    $expected_pub_id = 1; // The NULL publication
    $retrieved_pub_id = $results[0]->pub_id;
    $this->assertEquals($expected_pub_id, $retrieved_pub_id,
      "We did not get the pub_id from \"$linking_table\" that should have been set by associateCvterm");

    // TEST: We should be able to create a synonym, knowing only the name of the cv vocabulary and of the cv term
    $test_records = [];
    $test_records['set'] = $instance->insertCvtermSynonym(['cv.name' => 'local', 'cvterm.name' => 'newCvterm003',
                                                           'cvtermsynonym.synonym' => 'syn005', 'cvtermsynonym.type_id' => 5]);
    $test_records['get'] = $instance->getCvtermSynonym(['cv.name' => 'local', 'cvterm.name' => 'newCvterm003',
                                                        'cvtermsynonym.synonym' => 'syn005', 'cvtermsynonym.type_id' => 5]);
    $values = $this->multiAssert('insertCvtermSynonym', $test_records, 'cvterm', 'cvtermsynonym.cvtermsynonym_id', 'synonym "syn005"', 24);
    $retrieved_cvterm_id = $values['get']['cvterm.cvterm_id'];
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the existing Cvterm with synonym "syn005"');

    // TEST: We should be able to update this synonym and change its name
    $test_records = [];
    $test_records['set'] = $instance->updateCvtermSynonym(['cvtermsynonym.synonym' => 'syn006', 'cvtermsynonym.type_id' => 6],
                                                          ['cvtermsynonym.synonym' => 'syn005'], []);
    $test_records['get'] = $instance->getCvtermSynonym(['cvtermsynonym.synonym' => 'syn006', 'cvtermsynonym.type_id' => 6], []);
    $values = $this->multiAssert('updateCvtermSynonym', $test_records, 'cvterm', 'cvtermsynonym.cvtermsynonym_id', 'synonym "syn006"', 24);
    $retrieved_cvterm_id = $values['get']['cvterm.cvterm_id'];
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the dated Cvterm with synonym "syn005"');
    $this->assertEquals('syn006', $values['get']['cvtermsynonym.synonym'],
      'We did not update the synonym name for the updated Cvterm with synonym "syn005"');
    $this->assertEquals(6, $values['get']['cvtermsynonym.type_id'],
      'We did not update the type_id for the updated Cvterm with synonym "syn005"');

    // TEST: We should be able to upsert this synonym
    $test_records = [];
    $test_records['set'] = $instance->upsertCvtermSynonym(['cv.name' => 'local', 'cvtermsynonym.synonym' => 'syn006', 'cvtermsynonym.type_id' => 7], []);
    $test_records['get'] = $instance->getCvtermSynonym(['cv.name' => 'local', 'cvtermsynonym.synonym' => 'syn006', 'cvtermsynonym.type_id' => 7], []);
    $values = $this->multiAssert('upsertCvtermSynonym', $test_records, 'cvterm', 'cvtermsynonym.cvtermsynonym_id', 'synonym "syn006"', 24);
    $retrieved_cvterm_id = $values['get']['cvterm.cvterm_id'];
    $this->assertEquals($expected_cvterm_id, $retrieved_cvterm_id,
      'We did not get the correct cvterm_id for the upserted Cvterm with synonym "syn006"');
    $this->assertEquals(7, $values['get']['cvtermsynonym.type_id'],
      'We did not update the type_id for the upserted Cvterm with synonym "syn006"');

    // TEST: If we disable auto-lookup of linking columns and specify one
    // then we should be fine...

  }
}
