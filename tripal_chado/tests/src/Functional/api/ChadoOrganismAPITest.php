<?php

namespace Drupal\Tests\tripal_chado;

use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;

/**
 * Testing the tripal_chado/api/tripal_chado.organism.api.inc functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 */
class ChadoOrganismAPITest extends BrowserTestBase {

  protected $defaultTheme = 'stable';

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
   * Tests the following organism API functions:
   *   chado_get_organism()
   *   chado_get_organism_scientific_name()
   *   chado_get_organism_select_options()
   *   chado_abbreviate_infraspecific_rank()
   *   chado_unabbreviate_infraspecific_rank()
   *
   * @group tripal-chado
   * @group chado-organism
   */
  public function test_chado_organism_api_functions() {

    putenv('TRIPAL_SUPPRESS_ERRORS=TRUE');

    // Lookup cvterm_id for 'subspecies'
    $cvterm = chado_get_cvterm(['name' => 'subspecies'], [], self::$schemaName);
    $this->assertNotNull($cvterm, 'test_chado_organism_api_functions() unable to retrieve cvterm for subspecies');
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
    $dbq = chado_insert_record('organism', $org, [], self::$schemaName);
    $this->assertNotNull($dbq, 'test_chado_organism_api_functions() unable to insert test organism 1.');
    $organism_ids[0] = $dbq['organism_id'];

    $org['infraspecific_name'] = 'selvaticus';
    $org['abbreviation'] = 'T. ' . $species . ' subsp. selvaticus';
    $dbq = chado_insert_record('organism', $org, [], self::$schemaName);
    $this->assertNotNull($dbq, 'test_chado_organism_api_functions() unable to insert test organism 2.');
    $organism_ids[1] = $dbq['organism_id'];

    // Test invalid $identifiers ($identifiers must be an array) = Should fail, and in fact causes an exception
    $identifiers = 1;
    try {
      $org = chado_get_organism($identifiers, []);
    }
    catch (\Exception $e) {
      $org = NULL;
    }
    $this->assertNull($org, 'test_chado_organism_api_functions() did not flag invalid $identifiers (not an array)');

    // Test using an empty array for $identifiers = Should fail
    $identifiers = [];
    $org = chado_get_organism($identifiers, [], self::$schemaName);
    $this->assertNull($org, 'test_chado_organism_api_functions() did not flag invalid $identifiers (empty array)');

    // Test retrieving organism that does not exist = Should fail
    $identifiers = ['genus' => 'Wrong', 'species' => 'incorrect'];
    $org = chado_get_organism($identifiers, [], self::$schemaName);
    $this->assertNull($org, 'test_chado_organism_api_functions() returned an organism from invalid $identifiers');

    // Get organism from organism_id = Should succeed
    $identifiers = ['organism_id' => $organism_ids[0]];
    $org = chado_get_organism($identifiers, [], self::$schemaName);
    $this->assertIsObject($org, 'test_chado_organism_api_functions() did not return the organism with organism_id='.$organism_ids[0]);

    // Test ambiguous $identifiers = Should fail
    $identifiers = ['genus' => 'Tripalus', 'species' => $species];
    $org = chado_get_organism($identifiers, [], self::$schemaName);
    $this->assertNull($org, 'test_chado_organism_api_functions() returned an organism from ambiguous $identifiers');

    // Test unambiguous $identifiers = Should succeed
    $identifiers = ['genus' => 'Tripalus', 'species' => $species, 'type_id' => $subspecies_id, 'infraspecific_name' => 'selvaticus'];
    $org = chado_get_organism($identifiers, [], self::$schemaName);
    $this->assertIsObject($org, 'test_chado_organism_api_functions() did not return an organism from unambiguous $identifiers');

    // Test getting scientific name = Should succeed
    $name = chado_get_organism_scientific_name($org, self::$schemaName);
    $expect = 'Tripalus '.$species.' subsp. selvaticus';
    $this->assertEquals($name, $expect, 'test_chado_organism_api_functions() did not return the expected scientific name '.$expect.' but returned '.$name);

    // Test organism select options with default parameters and test that an array is returned = should succeed
    $select_options = chado_get_organism_select_options(FALSE, FALSE, self::$schemaName);
    $this->assertIsArray($select_options, 'test_chado_organism_api_functions() select options did not return an array');

    // Test that the array contains at least the two test organisms = should succeed
    $count = count($select_options);
    $this->assertGreaterThanOrEqual(2, $count, 'test_chado_organism_api_functions() select options did not return at least two organisms');

    // Test that both of the test organisms are in the returned array = should succeed
    $expect = 'Tripalus '.$species.' subsp. sativus';
    $this->assertArrayHasKey($organism_ids[0], $select_options, 'test_chado_organism_api_functions() the select options returned array does not contain the expected organism id '.$organism_ids[0]);
    $this->assertEquals($expect, $select_options[$organism_ids[0]], 'test_chado_get_organism_select_options() the select options array element does not contain the expected organism text '.$expect);
    $expect = 'Tripalus '.$species.' subsp. selvaticus';
    $this->assertArrayHasKey($organism_ids[1], $select_options, 'test_chado_organism_api_functions() the returned select options array does not contain the expected organism id '.$organism_ids[1]);
    $this->assertEquals($expect, $select_options[$organism_ids[1]], 'test_chado_organism_api_functions() the select options array element does not contain the expected organism text '.$expect);

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
      $this->assertEqual($result, $abbreviation, 'test_chado_organism_api_functions() did not properly abbreviate '.$full.' returned '.$result);
    }

    // Test unabbreviation of infraspecific rank
    foreach ($expected_unabbreviated as $abbreviation => $full) {
      $result = chado_unabbreviate_infraspecific_rank($full);
      $this->assertEqual($result, $full, 'test_chado_organism_api_functions() did not properly unabbreviate '.$abbreviation.' returned '.$result);
    }

    putenv('TRIPAL_SUPPRESS_ERRORS=FALSE');
  }

  // @to-do
  // The following API functions do not have tests yet:
  //   function chado_get_organism_image_url($organism)
  //   function chado_autocomplete_organism($text)
}
