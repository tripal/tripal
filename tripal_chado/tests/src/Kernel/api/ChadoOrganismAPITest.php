<?php

namespace Drupal\Tests\tripal_chado\Kernel\Api;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;


/**
 * Tests for API functions dealing with organisms.
 * Testing the tripal_chado/api/tripal_chado.organism.api.php functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 * @group Tripal Organism
 */
class ChadoOrganismAPITest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_biodb', 'tripal_chado'];

  /**
   * The test chado connection. It is also set in the container.
   *
   * @var ChadoConnection
   */
  protected $chado_connection;

  /**
   * Schema to do testing out of.
   * @var string
   */
  protected $schemaName;

  /**
   * Tests the following organism API functions:
   * @cover ::chado_get_organism
   * @cover ::chado_get_organism_scientific_name
   * @cover ::chado_get_organism_id_from_scientific_name
   * @cover ::chado_get_organism_select_options
   * @cover ::chado_abbreviate_infraspecific_rank
   * @cover ::chado_unabbreviate_infraspecific_rank
   * to-do: The following API functions do not have tests yet:
   *        chado_get_organism_image_url
   *        chado_autocomplete_organism
   *
   * @group tripal-chado
   * @group chado-organism
   */
  public function testChadoOrganismAPIFunctions() {

    putenv('TRIPAL_SUPPRESS_ERRORS=TRUE');

    // Create a new test schema for us to use, and retrieve its name.
    $this->chado_connection = $this->createTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
    $this->schemaName = $this->chado_connection->getSchemaName();

    // Lookup cvterm_id for 'subspecies'
    $cvterm = chado_get_cvterm(['name' => 'subspecies'], [], $this->schemaName);
    $this->assertNotNull($cvterm, 'Unable to retrieve cvterm for "subspecies" using chado_get_cvterm()');
    $subspecies_id = $cvterm->cvterm_id;

    // Create two test organisms of the same genus and species
    $species = 'bogusii' . uniqid();
    $organism_ids = [];
    $org = [
            'genus' => 'Tripalus',
            'species' => $species,
            'type_id' => $subspecies_id,
            'infraspecific_name' => 'sativus',
            'common_name' => 'False Tripal',
            'abbreviation' => 'T. ' . $species . ' subsp. sativus',
           ];
    $dbq = chado_insert_record('organism', $org, [], $this->schemaName);
    $this->assertNotNull($dbq, 'Unable to insert test organism 1.');
    $organism_ids[0] = $dbq['organism_id'];

    $org['infraspecific_name'] = 'selvaticus';
    $org['abbreviation'] = 'T. ' . $species . ' subsp. selvaticus';
    $dbq = chado_insert_record('organism', $org, [], $this->schemaName);
    $this->assertNotNull($dbq, 'Unable to insert test organism 2.');
    $organism_ids[1] = $dbq['organism_id'];

    // Test invalid $identifiers ($identifiers must be an array) = Should fail, and in fact causes an exception
    $identifiers = 'not-an-array';
    try {
      $org = chado_get_organism($identifiers, []);
    }
    catch (\Exception $e) {
      $org = NULL;
    }
    $this->assertNull($org, 'Did not flag invalid $identifiers passed to chado_get_organism() (not an array)');

    // Test using an empty array for $identifiers = Should fail
    $identifiers = [];
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertNull($org, 'Did not flag invalid $identifiers passed to chado_get_organism() (empty array)');

    // Test retrieving organism that does not exist = Should fail
    $identifiers = ['genus' => 'Wrong', 'species' => 'incorrect'];
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertNull($org, 'Returned an organism from invalid $identifiers passed to chado_get_organism()');

    // Get organism from organism_id = Should succeed
    $identifiers = ['organism_id' => $organism_ids[0]];
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertIsObject($org, 'Did not return the organism with organism_id='
                          . $organism_ids[0] . ' using chado_get_organism() and schema '
                          . $this->schemaName . ' is specified');

    // Get organism from organism_id without specifying schema = Should succeed
    // This is testing the default schema lookup in the API functions.
    $identifiers = ['organism_id' => $organism_ids[0]];
    $org = chado_get_organism($identifiers, []);
    $this->assertIsObject($org, 'Did not return the organism with organism_id='
                          . $organism_ids[0] . ' using chado_get_organism() and using default schema');

    // Test ambiguous $identifiers = Should fail
    $identifiers = ['genus' => 'Tripalus', 'species' => $species];  // subspecies not specified, thus ambiguous
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertNull($org, 'Returned an organism from ambiguous $identifiers passed to chado_get_organism()');

    // Test unambiguous $identifiers = Should succeed
    $identifiers = ['genus' => 'Tripalus', 'species' => $species, 'type_id' => $subspecies_id, 'infraspecific_name' => 'selvaticus'];
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertIsObject($org, 'Did not return an organism from unambiguous $identifiers passed to chado_get_organism()');

    // Test getting scientific name = Should succeed
    $expect = 'Tripalus ' . $species . ' subsp. selvaticus';
    $name = chado_get_organism_scientific_name($org, $this->schemaName);
    $this->assertEquals($name, $expect, 'Did not return the expected scientific name ' . $expect
                        . ' instead returned ' . $name . ' using chado_get_organism_scientific_name()');

    // Get organism object from scientific name = Should succeed
    $identifiers = ['scientific_name' => $expect];
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertIsObject($org, 'Did not return the organism with scientific_name '
                          . $name . ' using chado_get_organism()');

    // Get organism_id from scientific name = Should succeed
    $found_ids = chado_get_organism_id_from_scientific_name($name, [], $this->schemaName);
    $this->assertIsArray($found_ids, 'Did not return an array from chado_get_organism_id_from_scientific_name()');
    $this->assertEquals($found_ids[0], $organism_ids[1], 'Did not return the expected organism_id from scientific name '
                        . $name . ' using chado_get_organism_id_from_scientific_name()');
    // Test get organism_id from common name = Should succeed
    $found_ids = chado_get_organism_id_from_scientific_name($name, ['check_common_name' => 1], $this->schemaName);
    $this->assertIsArray($found_ids, 'Did not return an array from chado_get_organism_id_from_scientific_name()');
    $this->assertEquals($found_ids[0], $organism_ids[1], 'Did not return the expected organism_id from common name '
                        . $name . ' using chado_get_organism_id_from_scientific_name()');
    // Test get organism_id from non-existing name = Should fail for all options
    $name = 'Imaginarium nomen';
    $found_ids = chado_get_organism_id_from_scientific_name($name, ['check_common_name' => 1, 'check_abbreviation' => 1], $this->schemaName);
    $this->assertIsArray($found_ids, 'Did not return an array from chado_get_organism_id_from_scientific_name()');
    $this->assertEquals(count($found_ids), 0, 'Returned an organism_id for a non-existing name '
                        . $name . ' using chado_get_organism_id_from_scientific_name()');

    // Test organism select options with default parameters, and test that an array is returned = Should succeed
    $select_options = chado_get_organism_select_options(FALSE, FALSE, $this->schemaName);
    $this->assertIsArray($select_options, 'Did not return an array from chado_get_organism_select_options()');

    // Test that the array contains at least the two test organisms = Should succeed
    $count = count($select_options);
    $this->assertGreaterThanOrEqual(2, $count, 'Did not return at least two organisms from chado_get_organism_select_options()');

    // Test that both of the test organisms are in the returned array = Should succeed
    $expect = 'Tripalus ' . $species . ' subsp. sativus';
    $this->assertArrayHasKey($organism_ids[0], $select_options, 'Returned array does not contain the expected organism id '
                             . $organism_ids[0] . ' using chado_get_organism_select_options()');
    $this->assertEquals($expect, $select_options[$organism_ids[0]], 'Array element 0 does not contain the expected organism text '
                        . $expect . ' using chado_get_organism_select_options()');
    $expect = 'Tripalus ' . $species . ' subsp. selvaticus';
    $this->assertArrayHasKey($organism_ids[1], $select_options, 'Returned array does not contain the expected organism id '
                             . $organism_ids[1] . ' using chado_get_organism_select_options()');
    $this->assertEquals($expect, $select_options[$organism_ids[1]], 'Array element 1 does not contain the expected organism text '
                        . $expect . ' using chado_get_organism_select_options()');

    // Data to test abbreviation of infraspecific rank
    $expected_abbreviated = [
      'no_rank' => '',
      'subspecies' => 'subsp.',
      'varietas' => 'var.',
      'subvarietas' => 'subvar.',
      'cultivar' => 'cv.',
      'forma' => 'f.',
      'subforma' => 'subf.',
      'anything_else' => 'anything_else',
    ];

    // Data to test unabbreviation of infraspecific rank
    $expected_unabbreviated = [
      '' => '',
      'subsp' => 'subspecies',
      'subsp.' => 'subspecies',
      'ssp' => 'subspecies',
      'ssp.' => 'subspecies',
      'var' => 'varietas',
      'var.' => 'varietas',
      'subvar' => 'subvarietas',
      'subvar.' => 'subvarietas',
      'cv' => 'cultivar',
      'cv.' => 'cultivar',
      'f' => 'forma',
      'f.' => 'forma',
      'subf' => 'subforma',
      'subf.' => 'subforma',
      'anything_else' => 'anything_else',
      'anything_else.' => 'anything_else.',
    ];

    // Test abbreviation of infraspecific rank
    foreach ($expected_abbreviated as $full => $abbreviation) {
      $result = chado_abbreviate_infraspecific_rank($full);
      $this->assertEquals($result, $abbreviation, 'Did not properly abbreviate ' . $full
                          . ' returned ' . $result . ' using chado_abbreviate_infraspecific_rank()');
    }

    // Test unabbreviation of infraspecific rank
    foreach ($expected_unabbreviated as $abbreviation => $full) {
      $result = chado_unabbreviate_infraspecific_rank($full);
      $this->assertEquals($result, $full, 'Did not properly unabbreviate ' . $abbreviation
                          . ' returned ' . $result . ' using chado_unabbreviate_infraspecific_rank()');
    }

    putenv('TRIPAL_SUPPRESS_ERRORS=FALSE');
  }

}
