<?php

namespace Drupal\Tests\tripal_chado\Functional\Plugin;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;

/**
 * Tests for the the chado_linker__prop field.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\FieldType\chado_linker__prop
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 * @group Tripal Chado Fields
 */
class TripalField_chado_linker__propTest extends ChadoTestBrowserBase {

  /**
   * Holds the organism content type.
   *
   * @var \Drupal\tripal\Entity\TripalEntityType
   */
  private $organism_content_type = NULL;

  /**
   * Holds the organism entity.
   */


  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado', 'tripal_biodb', 'field_ui'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {

    parent::setUp();

    $prepared_chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Create the Organism Content Type
    $this->organism_content_type = $this->createTripalContentType([
      'label' => 'Organism',
      'termIdSpace' => 'OBI',
      'termAccession' => '0100026',
      'category' => 'General',
      'name' => 'bio_data_1',
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
      ],
    ]);
    $species_term = $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'species',
        'definition' => '',
        'accession' =>'0000006',
      ],
    ]);
    $infraspecies_term = $this->createTripalTerm([
      'vocab_name' => 'taxonomic_rank',
      'id_space_name' => 'TAXRANK',
      'term' => [
        'name' => 'infraspecies',
        'definition' => '',
        'accession' =>'0000045',
      ],
    ]);
    $description_term = $this->createTripalTerm([
      'vocab_name' => 'schema',
      'id_space_name' => 'schema',
      'term' => [
        'name' => 'description',
        'definition' => '',
        'accession' =>'description',
      ],
    ]);
    $abbreviation_term = $this->createTripalTerm([
      'vocab_name' => 'local',
      'id_space_name' => 'local',
      'term' => [
        'name' => 'abbreviation',
        'definition' => '',
        'accession' =>'abbreviation',
      ],
    ]);
    $common_name_term = $this->createTripalTerm([
      'vocab_name' => 'ncbitaxon',
      'id_space_name' => 'NCBITaxon',
      'term' => [
        'name' => 'common name',
        'definition' => '',
        'accession' =>'common_name',
      ],
    ]);

    ///
    // Create the fields for the Organism content type.
    //
    // We need these becaue the content type won't save properly. Techincally,
    // we only need the required fields, but to mimic reality we'll add them
    // all.
    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_taxrank_0000005',
      'field_type' => 'chado_string_type',
      'term' => $genus_term,
      'is_required' => TRUE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organism',
            'chado_column' => 'genus',
          ]
        ],
      ],
    ]);

    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_taxrank_0000006',
      'field_type' => 'chado_string_type',
      'term' => $species_term,
      'is_required' => TRUE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organism',
            'chado_column' => 'species',
          ],
        ],
      ],
    ]);

    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_taxrank_0000006',
      'field_type' => 'chado_string_type',
      'term' => $infraspecies_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organism',
            'chado_column' => 'infraspecific_name',
          ],
        ],
      ],
    ]);

    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_schema_description',
      'field_type' => 'chado_text_type',
      'term' => $description_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organism',
            'chado_column' => 'comment',
          ],
        ],
      ],
    ]);

    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_local_abbreviation',
      'field_type' => 'chado_string_type',
      'term' => $abbreviation_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organism',
            'chado_column' => 'abbreviation',
          ],
        ],
      ],
    ]);

    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_ncbitaxon_common_name',
      'field_type' => 'chado_string_type',
      'term' => $common_name_term,
      'is_required' => FALSE,
      'cardinality' => 1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organism',
            'chado_column' => 'common_name',
          ],
        ],
      ],
    ]);

    //
    // Now create an orgnism entity
    //
    $this->createTripalContent([
      'title' => 'Citrus sinensis',
      'type' => 'bio_data_1',
      'user_id' => 0,
      'status' => TRUE,
    ]);
  }

  /**
   * Tests the chado_linker__prop field
   *
   */
  public function testChadoLinkerPropField() {

    // Create the note term we'll use to model a property field.
    $note_term = $this->createTripalTerm([
      'vocab_name' => 'local',
      'id_space_name' => 'local',
      'term' => [
        'name' => 'Note',
        'definition' => 'A note',
        'accession' =>'Note',
      ],
    ]);

    // Add the field to the content type
    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_local_note',
      'field_type' => 'chado_linker__prop',
      'term' => $note_term,
      'is_required' => FALSE,
      'cardinality' => -1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'property_settings' => [
          'value' => [
            'action' => 'store',
            'chado_table' => 'organismprop',
            'chado_column' => 'value',
          ],
        ],
      ],
    ]);

    // Test that the field got added to the content type.
    $entityFieldManager = \Drupal::service('entity_field.manager');
    $fields = $entityFieldManager->getFieldDefinitions('tripal_entity', 'bio_data_1');
    $this->assertTrue(in_array('bio_data_1_local_note', array_keys($fields)));

    /**
     *
     * @var \Drupal\Core\Entity\EntityTypeManager $entityTypeManager
     */
    $entityTypeManager = \Drupal::service('entity_type.manager');
    $entityTypeManager->getStorage('tripal_entity');
  }
}