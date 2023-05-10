<?php

namespace Drupal\Tests\tripal_chado;

use Drupal\Core\Url;
use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;

/**
 * Testing the tripal_chado/api/tripal_chado.organism.api.inc functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 */
class ChadoOrganismAPITest extends ChadoTestBrowserBase {

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
  protected $schemaName;

  /**
   * Tests the following organism API functions:
   *   chado_get_organism()
   *   chado_get_organism_scientific_name()
   *   chado_get_organism_select_options()
   *   chado_abbreviate_infraspecific_rank()
   *   chado_unabbreviate_infraspecific_rank()
   *   @to-do: The following API functions do not have tests yet:
   *   chado_get_organism_image_url($organism)
   *   chado_autocomplete_organism($text)
   *
   * @group tripal-chado
   * @group chado-organism
   */
  public function test_chado_organism_api_functions() {

    putenv('TRIPAL_SUPPRESS_ERRORS=TRUE');

    // Create a Test Chado schema which contains the expected cvterms added during the prepare step.
    $this->schemaName = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

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
    $identifiers = 1;  // not an array
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
                          . $organism_ids[0] . ' using chado_get_organism()');

    // Test ambiguous $identifiers = Should fail
    $identifiers = ['genus' => 'Tripalus', 'species' => $species];  // subspecies not specified, thus ambiguous
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertNull($org, 'Returned an organism from ambiguous $identifiers passed to chado_get_organism()');

    // Test unambiguous $identifiers = Should succeed
    $identifiers = ['genus' => 'Tripalus', 'species' => $species, 'type_id' => $subspecies_id, 'infraspecific_name' => 'selvaticus'];
    $org = chado_get_organism($identifiers, [], $this->schemaName);
    $this->assertIsObject($org, 'Did not return an organism from unambiguous $identifiers passed to chado_get_organism()');

    // Test getting scientific name = Should succeed
    $name = chado_get_organism_scientific_name($org, $this->schemaName);
    $expect = 'Tripalus ' . $species . ' subsp. selvaticus';
    $this->assertEquals($name, $expect, 'Did not return the expected scientific name ' . $expect
                        . ' instead returned ' . $name . ' using chado_get_organism_scientific_name()');

    // Test organism select options with default parameters, and test that an array is returned = should succeed
    $select_options = chado_get_organism_select_options(FALSE, FALSE, $this->schemaName);
    $this->assertIsArray($select_options, 'Did not return an array from chado_get_organism_select_options()');

    // Test that the array contains at least the two test organisms = should succeed
    $count = count($select_options);
    $this->assertGreaterThanOrEqual(2, $count, 'Did not return at least two organisms from chado_get_organism_select_options()');

    // Test that both of the test organisms are in the returned array = should succeed
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
      $this->assertEqual($result, $abbreviation, 'Did not properly abbreviate ' . $full
                         . ' returned ' . $result . ' using chado_abbreviate_infraspecific_rank()');
    }

    // Test unabbreviation of infraspecific rank
    foreach ($expected_unabbreviated as $abbreviation => $full) {
      $result = chado_unabbreviate_infraspecific_rank($full);
      $this->assertEqual($result, $full, 'Did not properly unabbreviate ' . $abbreviation
                         . ' returned ' . $result . ' using chado_unabbreviate_infraspecific_rank()');
    }

    putenv('TRIPAL_SUPPRESS_ERRORS=FALSE');
  }

}
