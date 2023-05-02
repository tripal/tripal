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
   * The cvterm_id for subspecies, used in multiple tests.
   * @var integer
   */
  protected static $subspecies_id = null;

  /**
   * A unique species used in multiple tests.
   * @var string
   */
  protected static $species = null;

  /**
   * Test organism database ids.
   * @var array
   */
  protected static $organism_ids = [];

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {
    parent::setUp();

    // Lookup cvterm_id for 'subspecies'
    $cvterm = chado_get_cvterm(['name' => 'subspecies'], [], self::$schemaName);
    $this->assertNotNull($cvterm, 'ChadoOrganismAPI setUp() unable to retrieve cvterm for subspecies');
    self::$subspecies_id = $cvterm->cvterm_id;

    // Create two test organisms
    putenv('TRIPAL_SUPPRESS_ERRORS=TRUE');
    self::$species = 'bogusii' . uniqid();
    $org = [
            'genus' => 'Tripalus',
            'species' => self::$species,
            'type_id' => self::$subspecies_id,
            'infraspecific_name' => 'sativus',
            'common_name' => 'False Tripal',
            'abbreviation' => 'T. ' . self::$species . ' subsp. sativus',
           ];
    $dbq = chado_insert_record('organism', $org, [], self::$schemaName);
    $this->assertNotNull($dbq, 'ChadoOrganismAPI setUp() unable to insert test organism 1.');
    self::$organism_ids[0] = $dbq['organism_id'];

    $org['infraspecific_name'] = 'selvaticus';
    $org['abbreviation'] = 'T. ' . self::$species . ' subsp. selvaticus';
    $dbq = chado_insert_record('organism', $org, [], self::$schemaName);
    $this->assertNotNull($dbq, 'ChadoOrganismAPI setUp() unable to insert test organism 2.');
    self::$organism_ids[1] = $dbq['organism_id'];
  }

  /**
   * {@inheritdoc}
   */
  protected function tearDown() :void {
    parent::tearDown();

    putenv('TRIPAL_SUPPRESS_ERRORS=FALSE');
  }

  /**
   * Tests the chado_get_organism() API function.
   *
   * @group tripal-chado
   * @group chado-organism
   */
  public function test_chado_get_organism() {

    // Invalid $identifiers ($identifiers must be an array) = Should fail, and in fact causes an exception
    $options = [];
    $identifiers = 1;
    try {
      $org = chado_get_organism($identifiers, $options);
    }
    catch (\Exception $e) {
      $org = NULL;
    }
    $this->assertNull($org, 'test_chado_get_organism() did not flag invalid $identifiers (not an array)');

    // Empty array for $identifiers = Should fail
    $identifiers = [];
    $org = chado_get_organism($identifiers, $options, self::$schemaName);
    $this->assertNull($org, 'test_chado_get_organism() did not flag invalid $identifiers (empty array)');

    // Organism that does not exist = Should fail
    $identifiers = ['genus' => 'Wrong', 'species' => 'incorrect'];
    $org = chado_get_organism($identifiers, $options, self::$schemaName);
    $this->assertNull($org, 'test_chado_get_organism() returned an organism from invalid $identifiers');

    // Get organism from organism_id = Should succeed
    $identifiers = ['organism_id' => self::$organism_ids[0]];
    $org = chado_get_organism($identifiers, $options, self::$schemaName);
    $this->assertIsObject($org, 'test_chado_get_organism() did not return the organism with organism_id='.self::$organism_ids[0]);

    // Ambiguous $identifiers = Should fail
    $identifiers = ['genus' => 'Tripalus', 'species' => self::$species];
    $org = chado_get_organism($identifiers, $options, self::$schemaName);
    $this->assertNull($org, 'test_chado_get_organism() returned an organism from ambiguous $identifiers');

    // Unambiguous $identifiers = Should succeed
    $identifiers = ['genus' => 'Tripalus', 'species' => self::$species, 'type_id' => self::$subspecies_id, 'infraspecific_name' => 'selvaticus'];
    $org = chado_get_organism($identifiers, $options, self::$schemaName);
    $this->assertIsObject($org, 'test_chado_get_organism() did not return an organism from unambiguous $identifiers');
  }

  /**
   * Tests the chado_get_organism_scientific_name() API function.
   *
   * @group tripal-chado
   * @group chado-organism
   */
  public function test_chado_get_organism_scientific_name() {

    // Get organism from organism_id = Should succeed
    $options = [];
    $identifiers = ['organism_id' => self::$organism_ids[1]];
    $org = chado_get_organism($identifiers, $options, self::$schemaName);
    $this->assertIsObject($org, 'test_chado_get_organism_scientific_name() did not return the organism with organism_id='.self::$organism_ids[1]);

    // Get scientific name = Should succeed
    $name = chado_get_organism_scientific_name($org, self::$schemaName);
    $expect = 'Tripalus '.self::$species.' subsp. selvaticus';
    $this->assertEquals($name, $expect, 'test_chado_get_organism_scientific_name() did not return the correct scientific name');
  }
  // @@@ TO-DO list
  // x function chado_get_organism($identifiers, $options = []) {
  // x function chado_get_organism_scientific_name($organism) {
  //   function chado_get_organism_select_options($published_only = FALSE, $show_common_name = FALSE) {
  //   function chado_get_organism_image_url($organism) {
  //   function chado_autocomplete_organism($text) {
  //   function chado_abbreviate_infraspecific_rank($rank) {
  //   function chado_unabbreviate_infraspecific_rank($rank) {
}
