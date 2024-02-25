<?php

namespace Drupal\Tests\tripal_chado;

use Drupal\Core\Url;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;

/**
 * Testing the tripal_chado/api/tripal_chado.variables.api.php functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 */
class ChadoVariablesAPITest extends ChadoTestKernelBase {

  protected $defaultTheme = 'stark';

  protected $connection;

  /**
   * Modules to enable.
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests chado_generate_var().
   *
   * @group tripal-chado
   * @group chado-query
   */
  public function testChadoGenerateVariables() {
    $connection = \Drupal\Core\Database\Database::getConnection();

    // Check that chado exists.
    $check_schema = "SELECT true FROM pg_namespace WHERE nspname = :schema";
    $exists = $connection->query($check_schema, [':schema' => $this->testSchemaName])
      ->fetchField();
    $this->assertEquals(1, $exists, 'Cannot check chado schema api without chado.
      Please ensure chado is installed in the schema named "testchado".');

    // TEST DATA.
    // We need something to pull the data for.
    $org = [
      'genus' => 'Tripalus',
      'species' => 'ferox' . uniqid(),
      'type_id' => 2, //version
      'infraspecific_name' => 'Quad',
      'common_name' => 'Wild Tripal',
      'abbreviation' => 'T. ferox',
    ];
    $dbq = chado_insert_record('organism', $org, [], 'testchado');
    $organism_id = $dbq['organism_id'];
    $this->assertNotEquals(FALSE, $dbq, 'chado_insert_record() unable to insert.');
    $values = [
      'uniquename' => 'gene'.uniqid(),
      'organism_id' => $dbq['organism_id'],
      'type_id' => 2, //version
      'name' => 'FakeGene1',
      'residues' => str_repeat('ATGC', 100),
    ];
    $dbq = chado_insert_record('feature', $values, [], 'testchado');
    $this->assertNotEquals(FALSE, $dbq, 'chado_insert_record() unable to insert.');
    for ($i=1; $i<=5; $i++) {
      $prop = [
        'value' => str_repeat('BOO!', $i),
        'type_id' => 2,
        'feature_id' => $dbq['feature_id'],
        'rank' => $i,
      ];
      $dbq = chado_insert_record('featureprop', $prop, [], 'testchado');
    }

    // GENERATE.
    $var = chado_generate_var(
      'feature',
      ['uniquename' => $values['uniquename']],
      ['include_fk' => ['type_id' => 1]],
      'testchado'
    );
    $this->assertNotFalse($var,
      "chado_generate_var() failed.");
    $this->assertNotEmpty($var,
      "There should be a result from chado_generate_var() for the record we just inserted.");
    $this->assertIsObject($var,
      "There should only be a single result so an object should be returned.");
    $this->assertEquals($values['uniquename'], $var->uniquename,
      "The object returned should match the record we asked for.");

    // EXPAND.
    // -- FOREIGN KEY.
    // Above we chose not to expand the organism_id so check it did not.
    $this->assertEquals($organism_id, $var->organism_id,
      "The organism_id should not be expanded: " . print_r($var->organism_id, TRUE));
    $expanded = chado_expand_var($var, 'foreign_key', 'feature.organism_id => organism', [], 'testchado');
    $this->assertNotFalse($var,
      "chado_expand_var() failed for foreign key.");
    $this->assertNotEmpty($var,
      "There should be a result from chado_expand_var() for the variable we just generated.");
    $this->assertIsObject($var,
      "There should only be a single result so an object should be returned.");
    $this->assertIsObject($var->organism_id,
      "The object returned should have the organism_id expanded.");
    $this->assertEquals($org['species'], $var->organism_id->species,
      "The object returned should have the organism_id expanded to specify the genus.");

    // -- TABLE.
    $expanded = chado_expand_var($var, 'table', 'featureprop', [], 'testchado');
    $this->assertTrue(property_exists($var, 'featureprop'),
      "The feature properties should be present once expanded.");

    // -- FIELD.
    // By default the residues field is not expanded...
    $this->assertFalse(property_exists($var, 'residues'),
      "The residues should be missing by default.");
    $expanded = chado_expand_var($var, 'field', 'feature.residues', [], 'testchado');
    $this->assertTrue(property_exists($var, 'residues'),
      "The residues should be present once expanded.");
  }
}
