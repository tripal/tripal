<?php

namespace Drupal\Tests\tripal_chado\Functional\Service;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Symfony\Component\Validator\Constraints\IsNull;



/**
 * Tests the TripalPublish service in the context of the Chado content types.
 *
 * @group Tripal
 * @group Tripal Content
 */
class ChadoTripalPublishTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = [
    'tripal',
    'tripal_chado',
  ];

  /**
   * Helper function to an organism to the Chado database.
   */
  public function addChadoOrganism($chado, $details) {

    $insert = $chado->insert('1:organism');
    $insert->fields([
      'genus' => $details['genus'],
      'species' => $details['species'],
      'type_id' => array_key_exists('type_id', $details) ? $details['type_id'] : NULL,
      'infraspecific_name' => array_key_exists('infraspecific_name', $details) ? $details['infraspecific_name'] : NULL,
      'abbreviation' => array_key_exists('abbreviation', $details) ? $details['abbreviation'] : NULL,
      'common_name' => array_key_exists('common_name', $details) ? $details['common_name'] : NULL,
      'comment' => array_key_exists('comment', $details) ? $details['comment'] : NULL,
    ]);
    return $insert->execute();
  }

  /**
   * A helper function for adding a project record to Chado.
   */
  public function addChadoProject($chado, $details) {
    $insert = $chado->insert('1:contact');
    $insert->fields([
      'name' => $details['name'],
      'description' => array_key_exists('description', $details) ? $details['description'] : NULL,
    ]);
    return $insert->execute();
  }

  /**
   *A helper function for adding a contact record to Chado.
   */
  public function addChadoContact($chado, $details) {
    $insert = $chado->insert('1:contact');
    $insert->fields([
      'name' => $details['name'],
      'type_id' => array_key_exists('type_id', $details) ? $details['type_id'] : NULL,
      'description' => array_key_exists('description', $details) ? $details['description'] : NULL,
    ]);
    return $insert->execute();
  }

  /**
   * A helper function for adding a property to a record in Chado.
   */
  public function addProperty($chado, $base_table, $details) {

    $insert = $chado->insert('1:' . $base_table . 'prop');
    $insert->fields([
      $base_table . '_id' => $details[$base_table . '_id'],
      'value' => $details['value'],
      'type_id' => $details['type_id'],
      'rank' => $details['rank'],
    ]);
    $insert->execute();
  }
  /**
   * helper fection toget a cvterm_id
   */
  public function getTypeID($chado, $accession) {
    $type_id = NULL;
    $matches = [];
    $this->assertTrue(preg_match('/^(.+):(.*+)/', $accession) == 1,
        'The CV term used for publishing is invalid: ' . $accession);
    if (preg_match('/^(.+):(.*+)/', $accession, $matches)) {
      $select = $chado->select('1:cvterm', 'cvt');
      $select->join('1:dbxref', 'dbx', 'dbx.dbxref_id = cvt.dbxref_id');
      $select->join('1:db', 'db', 'db.db_id = dbx.db_id');
      $select->fields('cvt', ['cvterm_id']);
      $select->condition('dbx.accession', $matches[2]);
      $select->condition('db.name', $matches[1]);
      $result = $select->execute();
      $type_id = $result->fetchField();
      $this->assertTrue(!is_int($type_id),
        'The CV term used for publishing could not be found: ' . $accession);
    }
    return $type_id;
  }

  /**
   * A helper function to test if the elements of a field item are present.
   */
  public function checkFieldItem($bundle, $field_name, $record_id, $values = [], $delta=0) {

    $public = \Drupal::service('database');
    $select = $public->select('tripal_entity__' . $field_name, 'f');
    $select->fields('f');
    $select->condition($field_name . '_record_id', $record_id);
    $select->orderBy('delta');
    $result = $select->execute();
    $records = $result->fetchAll();
    if ($field_name == 'organism_note' or $field_name == 'organism_comment') {
      print_r([$field_name, $records]);
    }

    // Make sure we have at least one  item.
    $this->assertTrue(count($records) > 0,
        "The published item value for field, '$field_name', is missing");

    // Make sure we have at least $index records.
    $this->assertTrue(count($records) >= $delta + 1,
      "There are missing published items for field , '$field_name'.");

    if (count($records) < $delta + 1) {
      return;
    }
    $record = $records[$delta];

    // Make sure we have an entity ID for the specified record.
    $this->assertTrue($record->bundle == $bundle,
        'The bundle for a published item is incorrect (' . $record->bundle . '!=' . $bundle . ') for the field ' . $field_name);

    // Make sure we have an entity ID for the specified record.
    $this->assertNotNull($record->entity_id,
      'The entity_id for a published item is missing for the field ' . $field_name);

    // Make sure the delta matches
    $this->assertTrue($record->delta == $delta,
      'The delta for a published item is incorrect for the field ' . $field_name);

    // Make sure the expected values are present.
    foreach ($values as $column => $value) {
      $column_name = $field_name . '_' . $column;
      $this->assertTrue($record->$column_name == $value,
        'The value for, "' . $column_name . '", is not correct: '
          . $record->$column_name . ' (actual) != ' . $value . ' (expected); delta: ' . $delta . '.');
    }
  }

  /**
   * A helper function to add fields to the content types used in the tests.
   */
  public function addOrganismCustomFields() {

    // Create the terms for the field property storage types.
    /** @var \Drupal\tripal\TripalVocabTerms\PluginManagers\TripalIdSpaceManager $idsmanager */
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');

    $local_db = $idsmanager->loadCollection('local', "chado_id_space");
    $note_term = new TripalTerm();
    $note_term->setName('Note');
    $note_term->setIdSpace('local');
    $note_term->setVocabulary('local');
    $note_term->setAccession('Note');
    $local_db->saveTerm($note_term);

    $schema_db = $idsmanager->loadCollection('schema', "chado_id_space");
    $comment_term = new TripalTerm();
    $comment_term->setName('comment');
    $comment_term->setIdSpace('schema');
    $comment_term->setVocabulary('schema');
    $comment_term->setAccession('comment');
    $schema_db->saveTerm($comment_term);

    /** @var \Drupal\tripal\Services\TripalFieldCollection $fields_service **/
    // Now add a ChadoProperty field for the two types of properties.
    $fields_service = \Drupal::service('tripal.tripalfield_collection');
    $prop_field1 = [
      'name' => 'organism_note',
      'content_type' => 'organism',
      'label' => 'Note',
      'type' => 'chado_property_type_default',
      'description' => "A note about this organism.",
      'cardinality' => -1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings'=> [
          'base_table' => 'organism',
          'prop_table' => 'organismprop'
        ],
      ],
      'settings' => [
        'termIdSpace' => 'local',
        'termAccession' => "Note",
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 15
          ],
        ],
        'form' => [
          'default'=> [
            'region'=> 'content',
            'weight' => 15
          ],
        ],
      ],
    ];
    $reason = '';
    $is_valid = $fields_service->validate($prop_field1, $reason);
    $this->assertTrue($is_valid, $reason);
    $is_added = $fields_service->addBundleField($prop_field1);
    $this->assertTrue($is_added, 'The organism property field "local:Note" could not be added.');

    // Now add a ChadoProperty field for the two types of properties.
    $prop_field2 = [
      'name' => 'organism_comment',
      'content_type' => 'organism',
      'label' => 'Comment',
      'type' => 'chado_property_type_default',
      'description' => "A comment about this organism.",
      'cardinality' => -1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'chado_storage',
        'storage_plugin_settings'=> [
          'base_table' => 'organism',
          'prop_table' => 'organismprop'
        ],
      ],
      'settings' => [
        'termIdSpace' => 'schema',
        'termAccession' => "comment",
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 15
          ],
        ],
        'form' => [
          'default'=> [
            'region'=> 'content',
            'weight' => 15
          ],
        ],
      ],
    ];
    $reason = '';
    $is_valid = $fields_service->validate($prop_field2, $reason);
    $this->assertTrue($is_valid, $reason);
    $is_added = $fields_service->addBundleField($prop_field2);
    $this->assertTrue($is_added,
        'The Organism property field "schema:comment" could not be added.');
  }

  /**
   * Tests the TripalContentTypes class public functions.
   */
  public function testChadoTripalPublishService() {

    // Prepare Chado
    $chado = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Add the CV terms. These normally get added during a prepare and
    // the Chado schema is prepared but not Drupal schema and it needs to
    // know about the terms used for content types and fields.
    $terms_setup = \Drupal::service('tripal_chado.terms_init');
    $terms_setup->installTerms();

    // Make sure we have the content types and fields that we want to test.
    $collection_ids = ['general_chado'];
    $content_type_setup = \Drupal::service('tripal.tripalentitytype_collection');
    $fields_setup = \Drupal::service('tripal.tripalfield_collection');
    $content_type_setup->install($collection_ids);
    $fields_setup->install($collection_ids);

    /** @var \Drupal\tripal\Services\TripalPublish $publish */
    $publish = \Drupal::service('tripal.publish');

    // Test publishing when no records are available.
    $publish->init('organism', 'chado_storage');
    $entities = $publish->publish();
    $this->assertTrue(count($entities) == 0,
      'The TripalPublish service should return 0 entities when no records are available.');

    // Test publishing a single record.
    $organism_id = $this->addChadoOrganism($chado, [
      'genus' => 'Oryza',
      'species' => 'species',
      'abbreviation' => 'O. sativa',
      'type_id' => $this->getTypeID($chado, 'TAXRANK:0000023'),
      'infraspecific_name' => 'Japonica',
      'comment' => 'rice is nice'
    ]);
    $entities = $publish->publish();
    $this->assertTrue(count(array_keys($entities)) == 1,
      'The TripalPublish service should have published 1 organism.');

    // Test that entries were added for all field items.
    $this->checkFieldItem('organism', 'organism_genus', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_species', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_abbreviation', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_infraspecific_name', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_infraspecific_type', $organism_id, []);
    $this->checkFieldItem('organism', 'organism_comment', $organism_id, ['value' => NULL]);

    // Test that the title via token replacement is working.
    $this->assertTrue(array_values($entities)[0] == 'Oryza species subspecies Japonica',
      'The title of a Chado organism is incorrect after publishing.');

    // Test a title without all tokens
    $organism_id2 = $this->addChadoOrganism($chado, [
      'genus' => 'Gorilla',
      'species' => 'gorilla',
      'abbreviation' => 'G. gorilla',
      'comment' => 'Gorilla'
    ]);
    $entities = $publish->publish();
    $this->assertTrue(array_values($entities)[0] == 'Gorilla gorilla',
        'The title of Chado organism with missing tokens is incorrect after publishing: "' . array_keys($entities)[0] . '" != "Gorilla gorilla"');

    // Make sure the second organism has published fields.
    $this->checkFieldItem('organism', 'organism_genus', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_species', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_abbreviation', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_infraspecific_name', $organism_id, ['value' => NULL]);
    $this->checkFieldItem('organism', 'organism_infraspecific_type', $organism_id, []);
    $this->checkFieldItem('organism', 'organism_comment', $organism_id, ['value' => NULL]);

    // Test cardinality. We'll use the property field for this. First, lets
    // add three properties of the same type and three properties of
    // anothe rtype.
    $this->addOrganismCustomFields();
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $this->getTypeID($chado, 'local:Note'),
      'value' => 'This is the first note',
      'rank' => 1,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $this->getTypeID($chado, 'local:Note'),
      'value' => 'This is the second note',
      'rank' => 0,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $this->getTypeID($chado, 'local:Note'),
      'value' => 'This is the third note',
      'rank' => 2,
    ]);

    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $this->getTypeID($chado, 'schema:comment'),
      'value' => 'This is the first comment',
      'rank' => 0,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $this->getTypeID($chado, 'schema:comment'),
      'value' => 'This is the second comment',
      'rank' => 1,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $this->getTypeID($chado, 'schema:comment'),
      'value' => 'This is the third comment',
      'rank' => 2,
    ]);

    // Now publish the organism content type again.
    $publish->init('organism', 'chado_storage');
    $entities = $publish->publish();

    // Because we added properties for the first organism we should set it's
    // entity in those returned, but not the gorilla organism.
    $this->assertTrue(array_values($entities)[0] == 'Oryza species subspecies Japonica',
      'The Oryza species subspecies Japonica organism should appear in the published list because it has new properties.');
    $this->assertTrue(count(array_values($entities)) == 1,
      'There should only be one published entity for a single organism with new properties.');

    // Check that all three properties were added.  The prop_id will be the
    // primary key in the prop table. Since we had no properties in the
    // organismprop table these should start with 1.  The properties should be
    // published in rank order (delta) and since we flipped the rank of the second
    // and third property of the Note property they should be flipped as well.
    // We didn't flip the order for the Comment property so they should be in
    // order.
    $this->checkFieldItem('organism', 'organism_note', $organism_id, ['prop_id' => 1], 0);
    $this->checkFieldItem('organism', 'organism_note', $organism_id, ['prop_id' => 3], 1);
    $this->checkFieldItem('organism', 'organism_note', $organism_id, ['prop_id' => 2], 2);
    $this->checkFieldItem('organism', 'organism_comment', $organism_id, ['prop_id' => 4], 0);
    $this->checkFieldItem('organism', 'organism_comment', $organism_id, ['prop_id' => 5], 1);
    $this->checkFieldItem('organism', 'organism_comment', $organism_id, ['prop_id' => 6], 2);
  }
}
