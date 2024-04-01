<?php
namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Tests\tripal_chado\Traits\ChadoTestTrait;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * This is a base class for Chado tests that need a full Drupal install.
 *
 * It enables Chado tests schemas and helper functions to efficiently perform
 * tests.
 *
 * Example:
 * @code
 * // Gets a Chado test schema with dummy data:
 * $biodb = $this->getTestSchema(ChadoTestBrowserBase::INIT_CHADO_DUMMY);
 * //... do some tests
 * // After all is done, remove the schema properly:
 * $this->freeTestSchema($biodb);
 * // Note: if a test fails, the tearDownAfterClass will remove unremoved
 * // schemas.
 * @endcode
 *
 * @group Tripal Chado
 */
abstract class ChadoTestBrowserBase extends TripalTestBrowserBase {

  use ChadoTestTrait;

  /**
   * Just get a free test schema name.
   */
  public const SCHEMA_NAME_ONLY = 0;

  /**
   * Create an empty schema.
   */
  public const CREATE_SCHEMA = 1;

  /**
   * Create a schema and initialize it with dummy data.
   */
  public const INIT_DUMMY = 2;

  /**
   * Create a Chado schema with default data.
   */
  public const INIT_CHADO_EMPTY = 3;

  /**
   * Create a Chado schema and initialize it with dummy data.
   */
  public const INIT_CHADO_DUMMY = 4;

  /**
   * Create a Chado schema and prepare both it and the associated drupal schema.
   */
  public const PREPARE_TEST_CHADO = 5;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_biodb', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Only initialize the connection to Chado once.
    if (!$this->tripal_dbx) {
      $this->createChadoInstallationsTable();
      $this->getRealConfig();
      $this->initTripalDbx();
      $this->allowTestSchemas();
    }
  }

  /**
   * Creates an entity pre-loaded with the given genus and species.
   *
   * This function creates the Organism content type, adds all of the default
   * fields used by Chado for an organism (including the controlled vocabulary
   * terms for the field) and then creates an organism entity. It uses
   * `organism` as the entity type ID.
   *
   * @param string $genus
   *   The genus name
   * @param string $species
   *   The species name.
   *
   * @return \Drupal\tripal\Entity\TripalEntity
   *   The organism entity.
   */
  protected function createTestOrganismEntity($genus, $species) {

    $chado = $this->getTestSchema();

    // Create the Organism Content Type
    $this->createTripalContentType([
      'label' => 'Organism',
      'termIdSpace' => 'OBI',
      'termAccession' => '0100026',
      'category' => 'General',
      'id' => 'organism',
      'help_text' => 'A material entity that is an individual living system, ' .
        'such as animal, plant, bacteria or virus, that is capable of replicating ' .
        'or reproducing, growth and maintenance in the right environment. An ' .
        'organism may be unicellular or made up, like humans, of many billions ' .
        'of cells divided into specialized tissues and organs.',
    ]);

    // Create the terms that are needed for this field.
    $genus_term = $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'genus',
        'definition' => '',
        'accession' =>'0000005',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );
    $species_term = $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'species',
        'definition' => '',
        'accession' =>'0000006',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );
    $infraspecies_term = $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'infraspecies',
        'definition' => '',
        'accession' =>'0000045',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );
    $description_term = $this->createTripalTerm([
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'name' => 'description',
        'definition' => '',
        'accession' =>'description',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );
    $abbreviation_term = $this->createTripalTerm([
      'vocab_name' => 'local',
      'id_space_name' => 'local',
      'term' => [
        'name' => 'abbreviation',
        'definition' => '',
        'accession' =>'abbreviation',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );
    $common_name_term = $this->createTripalTerm([
      'vocab_name' => 'ncbitaxon',
      'id_space_name' => 'NCBITaxon',
      'term' => [
        'name' => 'common name',
        'definition' => '',
        'accession' =>'common_name',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );

    ///
    // Create the fields for the Organism content type.
    //
    // We need these because the content type won't save properly. Technically,
    // we only need the required fields, but to mimic reality we'll add them
    // all.
    $this->createTripalField('organism', [
      'field_name' => 'organism_taxrank_0000005',
      'field_type' => 'chado_string_type_default',
      'term' => $genus_term,
      'is_required' => TRUE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'base_column' => 'genus'
      ],
    ]);

    $this->createTripalField('organism', [
      'field_name' => 'organism_taxrank_0000006',
      'field_type' => 'chado_string_type_default',
      'term' => $species_term,
      'is_required' => TRUE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'base_column' => 'species'
      ],
    ]);

    $this->createTripalField('organism', [
      'field_name' => 'organism_taxrank_0000045',
      'field_type' => 'chado_string_type_default',
      'term' => $infraspecies_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'base_column' => 'infraspecific_type'
      ],
    ]);

    $this->createTripalField('organism', [
      'field_name' => 'organism_schema_description',
      'field_type' => 'chado_text_type_default',
      'term' => $description_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'base_column' => 'comment'
      ],
    ]);

    $this->createTripalField('organism', [
      'field_name' => 'organism_local_abbreviation',
      'field_type' => 'chado_string_type_default',
      'term' => $abbreviation_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'base_column' => 'abbreviation'
      ],
    ]);

    $this->createTripalField('organism', [
      'field_name' => 'organism_ncbitaxon_common_name',
      'field_type' => 'chado_string_type_default',
      'term' => $common_name_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'base_column' => 'common_name'
      ],
    ]);

    /**
     * Create the organism entity.
     *
     * @var \Drupal\tripal\Entity\TripalEntity $entity
     */
    $entity = $this->createTripalContent([
      'title' => $genus . '_' . $species,
      'type' => 'organism',
      'user_id' => 0,
      'status' => TRUE,
    ]);


//     // Make sure that the entity has all of the fields.
//     $this->assertTrue($entity->hasField('organism_taxrank_0000005'), "The organism entity is missing the organism_taxrank_0000005 field");
//     $this->assertTrue($entity->hasField('organism_taxrank_0000006'), "The organism entity is missing the organism_taxrank_0000006 field");
//     $this->assertTrue($entity->hasField('organism_taxrank_0000045'), "The organism entity is missing the organism_taxrank_0000045 field");
//     $this->assertTrue($entity->hasField('organism_local_abbreviation'), "The organism entity is missing the organism_local_abbreviation field");
//     $this->assertTrue($entity->hasField('organism_ncbitaxon_common_name'), "The organism entity is missing the organism_ncbitaxon_common_name field");
//     $this->assertTrue($entity->hasField('organism_schema_description'), "The organism entity is missing the organism_schema_description field");

//     // Set field property values.
//     $entity->organism_taxrank_0000005->value =  $genus;
//     $entity->organism_taxrank_0000006->value =  $species;

//     // Save the entity.
//     $entity->enforceIsNew();
//     $entity->save();

//     // Make sure there is a record in the Chado database
//     $query = $chado->select('1:organism', 'organism');
//     $query->fields('organism', ['organism_id']);
//     $query->condition('genus', $genus);
//     $query->condition('species', $species);
//     $organism_id = $query->execute()->fetchField();

//     $this->assertNotEmpty($organism_id, "The organism entity did not create the organism record in Chado as expected");

//     $this->container->get('router.builder')->rebuild();

    return $entity;
  }
}
