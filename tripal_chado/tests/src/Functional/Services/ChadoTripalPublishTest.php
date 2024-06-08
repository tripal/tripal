<?php

namespace Drupal\Tests\tripal_chado\Functional\Service;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Symfony\Component\Validator\Constraints\IsNull;
use Drupal\bootstrap\Theme;



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
   * A helper function for adding an organism record to Chado.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $chado
   *   A chado database object.
   * @param array $details
   *   The key/value pairs of entries for the organism. The keys correspond
   *   to the columns of the organism table.
   * @return int
   *   The organism_id
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
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $chado
   *   A chado database object.
   * @param array $details
   *   The key/value pairs of entries for the project. The keys correspond
   *   to the columns of the project table.
   * @return int
   *   The project_id
   */
  public function addChadoProject($chado, $details) {
    $insert = $chado->insert('1:project');
    $insert->fields([
      'name' => $details['name'],
      'description' => array_key_exists('description', $details) ? $details['description'] : NULL,
    ]);
    return $insert->execute();
  }

  /**
   * A helper function for adding a contact record to Chado.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $chado
   *   A chado database object.
   * @param array $details
   *   The key/value pairs of entries for the contact. The keys correspond
   *   to the columns of the contact table.
   * @return int
   *   The contact_id
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
   * A helper function for adding a project_contact record to Chado.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $chado
   *   A chado database object.
   * @param array $details
   *   The key/value pairs of entries for the project_contact. The keys correspond
   *   to the columns of the project_contact table.
   * @return int
   *   The project_contact_id
   */
  public function addChadoProjectContact($chado, $details) {
    $insert = $chado->insert('1:project_contact');
    $insert->fields([
      'project_id' => $details['project_id'],
      'contact_id' => $details['contact_id'],
    ]);
    return $insert->execute();
  }

  /**
   * A helper function for adding a property to a record in Chado.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $chado
   *   A chado database object.
   * @param string $base_table
   *   The base table to which the property should be added.
   * @param array $details
   *   The key/value pairs of entries for the property. The keys correspond
   *   to the columns of the property table.
   *
   * @return int
   *   The property primary key.
   */
  public function addProperty($chado, $base_table, $details) {

    $insert = $chado->insert('1:' . $base_table . 'prop');
    $insert->fields([
      $base_table . '_id' => $details[$base_table . '_id'],
      'value' => $details['value'],
      'type_id' => $details['type_id'],
      'rank' => $details['rank'],
    ]);
    return $insert->execute();
  }


  /**
   * A helper function to test if the elements of a field item are present.
   *
   * @param string $bundle
   *   The content type bundle name (e.g. 'organism').
   * @param string $field_name
   *   The name of the field that should be queried.
   * @param int $num_expected
   *   The number of items that are expected to be found when applying the
   *   conditions specified in the $match argument.
   * @param array $match
   *   An array of key/value pairs where the keys are the column names of
   *   field table in Drupal and the values are those to match in a select
   *   condition.  All fields other than the `entity_id', 'bundle', 'delta'
   *   'deleted',  'langcode', and 'revision' have the field name as a prefix.
   *   But the keys need not include the prefix, just the field property key.
   *   The field name prefix will be added automatically.
   * @param array $check
   *   An array of key/value pairs where the keys are the column names of the
   *   field table in Drupal and the values are checked that they match
   *   what is in the table. The same rules apply for the key naming as in
   *   the $match argument.
   */
  public function checkFieldItem($bundle, $field_name, $num_expected, $match, $check) {

    $drupal_columns = ['bundle', 'entity_id', 'revision' ,'delta', 'deleted', 'langcode'];

    $public = \Drupal::service('database');
    $select = $public->select('tripal_entity__' . $field_name, 'f');
    $select->fields('f');
    $select->condition('bundle', $bundle);
    foreach ($match as $key => $val) {
      $column_name = $key;
      if (!in_array($key, $drupal_columns)) {
        $column_name = $field_name . '_' . $key;
      }
      $select->condition($column_name, $val);
    }
    $select->orderBy('delta');
    $result = $select->execute();
    $records = $result->fetchAll();

    $this->assertTrue(count($records) == $num_expected,
        'The number of items expected for field "' . $field_name .'" with bundle "'
        . $bundle . '" is not correct: ' . count($records) . ' != ' . $num_expected);

    foreach ($records as $delta => $record) {

      // Make sure we have an entity ID for the specified record.
      $this->assertNotNull($record->entity_id,
        'The entity_id for a published item is missing for the field "'
          . $field_name . '" at delta ' . $delta);

      // Make sure the expected values are present.
      foreach ($check as $key => $val) {
        $column_name = $key;
        if (!in_array($key, $drupal_columns)) {
          $column_name = $field_name . '_' . $key;
        }
        $this->assertTrue($record->$column_name == $val,
          'The value for, "' . $column_name . '", is not correct: '
            . $record->$column_name . ' (actual) != ' . $val . ' (expected)');
      }
    }
  }

  /**
   * A helper function to add fields to the organism content types used in the tests.
   */
  public function attachOrganismPropertyFields() {

    /** @var \Drupal\tripal\Services\TripalFieldCollection $fields_service **/
    // Now add a ChadoProperty field for the two types of properties.
    $fields_service = \Drupal::service('tripal.tripalfield_collection');
    $prop_field1 = [
      'name' => 'field_note',
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
      'name' => 'field_comment',
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

    // Make sure we have the content types and fields that we want to test.
    $collection_ids = ['general_chado'];
    $content_type_setup = \Drupal::service('tripal.tripalentitytype_collection');
    $fields_setup = \Drupal::service('tripal.tripalfield_collection');
    $content_type_setup->install($collection_ids);
    $fields_setup->install($collection_ids);

    /** @var \Drupal\tripal\Services\TripalPublish $publish */
    $publish = \Drupal::service('tripal.publish');

    //
    // Test publishing when no records are available.
    //
    $publish->init('organism', 'chado_storage');
    $entities = $publish->publish();
    $this->assertTrue(count($entities) == 0,
      'The TripalPublish service should return 0 entities when no records are available.');

    //
    // Test publishing a single record.
    //
    $taxrank_db = $idsmanager->loadCollection('TAXRANK', "chado_id_space");
    $subspecies_term_id = $taxrank_db->getTerm('0000023')->getInternalId();

    $organism_id = $this->addChadoOrganism($chado, [
      'genus' => 'Oryza',
      'species' => 'species',
      'abbreviation' => 'O. sativa',
      'type_id' => $subspecies_term_id,
      'infraspecific_name' => 'Japonica',
      'comment' => 'rice is nice'
    ]);
    $entities = $publish->publish();
    $this->assertTrue(count(array_keys($entities)) == 1,
      'The TripalPublish service should have published 1 organism.');

    // Test that entries were added for all field items and that fields that
    // shouldn't be saved in Drupal are NULL.
    $this->checkFieldItem('organism', 'organism_genus', 1,
        ['record_id' => $organism_id],
        ['bundle' => 'organism', 'entity_id' => 1, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_species', 1,
        ['record_id' => $organism_id],
        ['bundle' => 'organism', 'entity_id' => 1, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_abbreviation', 1,
        ['record_id' => $organism_id],
        ['bundle' => 'organism', 'entity_id' => 1, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_infraspecific_name', 1,
        ['record_id' => $organism_id],
        ['bundle' => 'organism', 'entity_id' => 1, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_infraspecific_type', 1,
        ['record_id' => $organism_id],
        ['bundle' => 'organism', 'entity_id' => 1, 'type_id' => NULL]);

    $this->checkFieldItem('organism', 'organism_comment', 1,
        ['record_id' => $organism_id],
        ['bundle' => 'organism', 'entity_id' => 1, 'value' => NULL]);

    // Test that the title via token replacement is working.
    $this->assertTrue(array_values($entities)[0] == '<em>Oryza species</em> subspecies <em>Japonica</em>',
        'The title of a Chado organism is incorrect after publishing: ' . array_values($entities)[0] . '!=' . '<em>Oryza species</em> subspecies <em>Japonica</em>');

    //
    // Test a second entity. Also use a title without all tokens
    //
    $organism_id2 = $this->addChadoOrganism($chado, [
      'genus' => 'Gorilla',
      'species' => 'gorilla',
      'abbreviation' => 'G. gorilla',
      'comment' => 'Gorilla'
    ]);
    $entities = $publish->publish();
    $this->assertTrue(array_values($entities)[0] == '<em>Gorilla gorilla</em> <em></em>',
        'The title of Chado organism with missing tokens is incorrect after publishing: "' . array_values($entities)[0] . '" != "<em>Gorilla gorilla</em> <em></em>"');


    $this->checkFieldItem('organism', 'organism_genus', 1,
        ['record_id' => $organism_id2],
        ['bundle' => 'organism', 'entity_id' => 2, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_species', 1,
        ['record_id' => $organism_id2],
        ['bundle' => 'organism', 'entity_id' => 2, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_abbreviation', 1,
        ['record_id' => $organism_id2],
        ['bundle' => 'organism', 'entity_id' => 2, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_infraspecific_name', 0,
        ['record_id' => $organism_id2],
        ['bundle' => 'organism', 'entity_id' => 2, 'value' => NULL]);

    $this->checkFieldItem('organism', 'organism_infraspecific_type', 1,
        ['record_id' => $organism_id2],
        ['bundle' => 'organism', 'entity_id' => 2, 'type_id' => NULL]);

    $this->checkFieldItem('organism', 'organism_comment', 1,
        ['record_id' => $organism_id2],
        ['bundle' => 'organism', 'entity_id' => 2, 'value' => NULL]);

    //
    // Test publishing properties.
    //
    $comment_type_id = $schema_db->getTerm('comment')->getInternalId();
    $note_type_id = $local_db->getTerm('Note')->getInternalId();
    $this->attachOrganismPropertyFields();
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $note_type_id,
      'value' => 'Note 1',
      'rank' => 1,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $note_type_id,
      'value' => 'Note 0',
      'rank' => 0,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $note_type_id,
      'value' => 'Note 2',
      'rank' => 2,
    ]);

    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $comment_type_id,
      'value' => 'Comment 0',
      'rank' => 0,
    ]);
    $this->addProperty($chado, 'organism', [
      'organism_id' => $organism_id,
      'type_id' => $comment_type_id,
      'value' => 'Comment 1',
      'rank' => 1,
    ]);


    // Now publish the organism content type again.
    $publish->init('organism', 'chado_storage');
    $entities = $publish->publish();

    // Because we added properties for the first organism we should set it's
    // entity in those returned, but not the gorilla organism.
    $this->assertTrue(array_values($entities)[0] == '<em>Oryza species</em> subspecies <em>Japonica</em>',
      'The Oryza species subspecies Japonica organism should appear in the published list because it has new properties.');
    $this->assertTrue(count(array_values($entities)) == 1,
      'There should only be one published entity for a single organism with new properties.');

    // Check that the property values got published.  The type_id should be
    // NULL because that's not stored in DRupal.
    $this->checkFieldItem('organism', 'field_note', 1,
        ['record_id' => $organism_id, 'prop_id' => 1],
        ['type_id' => NULL, 'linker_id' => $organism_id,
         'bundle' => 'organism', 'entity_id' => 1]);

    $this->checkFieldItem('organism', 'field_note', 1,
        ['record_id' => $organism_id, 'prop_id' => 2],
        ['type_id' => NULL, 'linker_id' => $organism_id,
         'bundle' => 'organism', 'entity_id' => 1]);

    $this->checkFieldItem('organism', 'field_note', 1,
        ['record_id' => $organism_id, 'prop_id' => 3],
        ['type_id' => NULL, 'linker_id' => $organism_id,
         'bundle' => 'organism', 'entity_id' => 1]);

    $this->checkFieldItem('organism', 'field_comment', 1,
        ['record_id' => $organism_id, 'prop_id' => 4],
        ['type_id' => NULL, 'linker_id' => $organism_id,
         'bundle' => 'organism', 'entity_id' => 1]);

    $this->checkFieldItem('organism', 'field_comment', 1,
        ['record_id' => $organism_id, 'prop_id' => 5],
        ['type_id' => NULL, 'linker_id' => $organism_id,
         'bundle' => 'organism', 'entity_id' => 1]);

    // Check that only the exact number of properties were published.
    $this->checkFieldItem('organism', 'field_note', 3, ['entity_id' => 1], []);
    $this->checkFieldItem('organism', 'field_comment', 2, ['entity_id' => 1], []);

    //
    // Test publishing a field that uses a linker table.
    //

    // Create an and publish the contacts and the project.
    $contact_db = $idsmanager->loadCollection('TCONTACT', "chado_id_space");
    $person_term_id = $contact_db->getTerm('0000003')->getInternalId();
    $contact_id1 = $this->addChadoContact($chado, [
      'name' => 'John Doe',
       'type_id' => $person_term_id,
      'description' => 'Bioinformaticist extrodinaire'
    ]);
    $contact_id2 = $this->addChadoContact($chado, [
      'name' => 'Lady Gaga',
      'type_id' => $person_term_id,
      'description' => 'Pop star'
    ]);
    $project_id1 = $this->addChadoProject($chado, [
      'name' => 'Bad Project',
      'description' => 'Want your bad project'
    ]);
    $project_id2 = $this->addChadoProject($chado, [
      'name' => 'Project Face',
      'description' => 'I wanna project like they do in Texas, please'
    ]);
    $project_contact_id1 = $this->addChadoProjectContact($chado, [
      'project_id' => $project_id1,
      'contact_id' => $contact_id1,
    ]);
    $project_contact_id2 = $this->addChadoProjectContact($chado, [
      'project_id' => $project_id1,
      'contact_id' => $contact_id2,
    ]);
    $project_contact_id3 = $this->addChadoProjectContact($chado, [
      'project_id' => $project_id2,
      'contact_id' => $contact_id2,
    ]);

    // Now publish the projects and contacts. We check that 3 items are
    // published because there is a null contact and currently there is
    // nothing to prevent that contact from being published.
    $publish->init('contact', 'chado_storage');
    $entities = $publish->publish();
    $this->assertTrue(count(array_values($entities)) == 3,
        'Failed to publish 3 contact entities.');

    $publish->init('project', 'chado_storage');
    $entities = $publish->publish();
    $this->assertTrue(count(array_values($entities)) == 2,
      'Failed to publish 2 project entities.');

    // Make sure that the linked records are also published for each project.
    // The chado_contact_type_default is the field we're testing got published.
    $this->checkFieldItem('project', 'project_contact', 1,
        ['record_id' => $project_id1, 'linker_id' => $project_contact_id1],
        ['link' => $project_id1, 'bundle' => 'project', 'entity_id' => 6, 'contact_id' => $contact_id1]);

    $this->checkFieldItem('project', 'project_contact', 1,
        ['record_id' => $project_id1, 'linker_id' => $project_contact_id2],
        ['link' => $project_id1, 'bundle' => 'project', 'entity_id' => 6, 'contact_id' => $contact_id2]);

    $this->checkFieldItem('project', 'project_contact', 1,
        ['record_id' => $project_id2, 'linker_id' => $project_contact_id3],
        ['link' => $project_id2, 'bundle' => 'project', 'entity_id' => 7, 'contact_id' => $contact_id2]);

    // Check that only the exact number of linked items were published.
    $this->checkFieldItem('project', 'project_contact', 2, ['entity_id' => 6], []);
    $this->checkFieldItem('project', 'project_contact', 1, ['entity_id' => 7], []);

  }
}
