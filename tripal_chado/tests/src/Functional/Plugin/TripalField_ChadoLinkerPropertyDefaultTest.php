<?php

namespace Drupal\Tests\tripal_chado\Functional\Plugin;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal\Entity\TripalEntity;

/**
 * Tests for the the ChadoLinkerPropertyDefault field.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\FieldType\ChadoLinkerPropertyDefault
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 * @group Tripal Chado Fields
 */
class TripalField_ChadoLinkerPropertyDefaultTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado', 'tripal_biodb', 'field_ui'];

  /**
   *
   * {@inheritDoc}
   */
  protected function setUp() :void {
    parent::setup();

    // Use the Prepared test chado schema.
    $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

  }

  /**
   * Tests the ChadoLinkerPropertyDefault field
   *
   */
  public function testChadoLinkerPropField() {

    // Create an organism entity.
    $entity = $this->createTestOrganismEntity('Citrus', 'sinensis');

    // Create the note term we'll use to model a property field.
    $note_term = $this->createTripalTerm([
      'vocab_name' => 'local',
      'id_space_name' => 'local',
      'term' => [
        'name' => 'Note',
        'definition' => 'A note',
        'accession' =>'Note',
      ]],
      'chado_id_space', 'chado_vocabulary'
    );

    // Add the ChadoLinkerPropertyDefault to the content type.
    $this->createTripalField('bio_data_1', [
      'field_name' => 'bio_data_1_local_note',
      'field_type' => 'chado_linker_property_default',
      'term' => $note_term,
      'is_required' => FALSE,
      'cardinality' => -1,
      'storage_plugin_settings' => [
        'base_table' => 'organism',
        'type_table' => 'organismprop',
        'type_column' => 'type_id',
      ],
    ]);

//     // Reload the entity to get the field.
//     $entity = TripalEntity::load($entity->getID());

//     // Verify that the note field got added to the organism entity.
//     $this->assertTrue($entity->hasField('bio_data_1_local_note'),
//       "The organism entity is missing the note field.");

//     //
//     // Test a single property value.
//     //

//     // Test adding a single value.
//     $entity->set('bio_data_1_local_note', 'note1');
//     $entity->set('bio_data_1_local_abbreviation', 'C. siensis');
//     $entity->save();
//     $entity = TripalEntity::load($entity->getID());

    /*
    // Make sure the field has a only one value
    $fields = $entity->getFields();
    $field_items = $fields['bio_data_1_local_abbreviation'];
    $this->assertEquals(1, $field_items->count(),
        "The note field should have one value. Reported: " . $field_items->count());

    $note_items = $fields['bio_data_1_local_note'];
    $this->assertEquals(1, $note_items->count(),
      "The note field should have one value. Reported: " . $note_items->count());

    // Make sure the field has a record ID.
    $organism_id = $note_items->get(0)->get("record_id")->getValue();
    $this->assertNotNull($organism_id, "The ChadoLinkerPropertyDefault did not set a record_id");

    // Chado should have the value.
    $query = $this->chado->select('1:organismprop', 'OP');
    $query->fields('OP', ['organismprop_id']);
    $query->condition('organism_id', $organism_id);
    $query->condition('value', 'note1');
    $organismprop_id = $query->execute()->fetchField();
    $this->assertNotNull($organismprop_id, "The ChadoLinkerPropertyDefault did not insert the record into Chado");
    $organism_id = $query->execute()->fetchField();
    */

  }
}
