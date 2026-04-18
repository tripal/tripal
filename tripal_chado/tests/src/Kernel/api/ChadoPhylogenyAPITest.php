<?php

namespace Drupal\Tests\tripal_chado\Kernel\api;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use PHPUnit\Framework\Attributes\CoversFunction;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests for API functions dealing with phylogenetic trees.
 * Testing the tripal_chado/api/tripal_chado.phylotree.api.php functions.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal API
 * @group Tripal Phylotree
 *
 * @covers ::chado_phylogeny_lookup_organism_by_name
 * @covers ::chado_phylogeny_get_node_types_vocab
 * not yet implemented because newick importer not completed
 *   ::chado_phylogeny_import_tree_file
 *   ::chado_validate_phylotree
 *   ::chado_insert_phylotree
 *   ::chado_update_phylotree
 *   ::chado_delete_phylotree
 *   ::chado_assign_phylogeny_tree_indices
 *   ::chado_phylogeny_import_tree
 */
#[CoversFunction('chado_phylogeny_lookup_organism_by_name')]
#[CoversFunction('chado_phylogeny_get_node_types_vocab')]
#[Group('legacy-api')]
#[Group('bio-organism')]
#[Group('bio-phylogeny')]
#[RunTestsInSeparateProcesses]
class ChadoPhylogenyAPITest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
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
   *
   * @var string
   */
  protected $schemaName;

  /**
   * Tests the following phylotree API functions:
   */
  public function testChadoPhylotreeAPIFunctions() {

    // Create a new test schema for us to use, and retrieve its name.
    $this->chado_connection = $this->createTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
    $this->schemaName = $this->chado_connection->getSchemaName();

    // Lookup cvterm_id for 'subspecies'.
    $query = $this->chado_connection->select('1:cvterm', 't');
    $query->condition('name', 'subspecies', '=');
    $query->addField('t', 'cvterm_id', 'cvterm_id');
    $subspecies_id = $query->execute()->fetchField();
    $this->assertNotNull($subspecies_id, 'Unable to retrieve cvterm_id for "subspecies"');

    // Create two test organisms of the same genus and species.
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
    $query = $this->chado_connection->insert('1:organism')
      ->fields($org);
    $organism_id = $query->execute();
    $this->assertEquals('1', $organism_id, 'Unable to insert test organism 1.');
    $organism_ids[0] = $organism_id;

    $org['infraspecific_name'] = 'selvaticus';
    $org['abbreviation'] = 'T. ' . $species . ' subsp. selvaticus';
    $query = $this->chado_connection->insert('1:organism')
      ->fields($org);
    $organism_id = $query->execute();
    $this->assertEquals('2', $organism_id, 'Unable to insert test organism 2.');
    $organism_ids[1] = $organism_id;

    // Test chado_phylogeny_lookup_organism_by_name().
    // This function is expected to return an organism_id. It returns FALSE if
    // unable to lookup organism. Test with underscores like a Newick file might
    // do, and extra flanking white space.
    $organism_name = '  Tripalus_' . $species . '_subsp._sativus  ';
    $organism_id = chado_phylogeny_lookup_organism_by_name($organism_name);
    $this->assertNotFalse($organism_id, 'chado_phylogeny_lookup_organism_by_name() could not lookup ' . $organism_name);
    $this->assertEquals($organism_id, $organism_ids[0],
                        'chado_phylogeny_lookup_organism_by_name() did not return the correct organism_id for ' . $organism_name);
    $organism_name = 'Nonexisticus nulla';
    $organism_id = chado_phylogeny_lookup_organism_by_name($organism_name);
    $this->assertFalse($organism_id, 'chado_phylogeny_lookup_organism_by_name() returned an organism_id for a nonexistent organism');

    // Test chado_phylogeny_get_node_types_vocab().
    // This function is expected to return an array with three terms.
    // It returns FALSE on error.
    $vocab = chado_phylogeny_get_node_types_vocab([], $this->schemaName);
    $this->assertIsArray($vocab, 'Did not return an array from chado_phylogeny_get_node_types_vocab()');
    $this->assertEquals(count($vocab), 3, 'Did not return the expected three node types from chado_phylogeny_get_node_types_vocab()');
  }

}
