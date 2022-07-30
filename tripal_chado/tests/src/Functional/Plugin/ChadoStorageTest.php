<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;



/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class ChadoStorageTest extends ChadoTestBrowserBase {

  /**
   * A helper function to create the `bio_data_x` table.
   *
   * @param string $entity_type
   */
  protected function createEntityTypeTable($entity_type) {
    $table_def = [
      'description' => 'The linker table that associates TripalEntities with Chado records for entities of type ' . $entity_type . '.',
      'fields' => [
        'mapping_id' => [
          'type' => 'serial',
          'not null' => TRUE,
        ],
        'entity_id' => [
          'description' => 'The unique entity id.',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'record_id' => [
          'description' => 'The unique numerical identifier for the record that this entity is associated with (e.g. feature_id, stock_id, library_id, etc.).',
          'type' => 'int',
          'not null' => TRUE,
        ],
        'nid' => [
          'description' => 'Optional. For linking nid to the entity when migrating Tripal v2 content',
          'type' => 'int',
        ],
      ],
      'primary key' => [
        'mapping_id',
      ],
      'indexes' => [
        'record_id' => ['record_id'],
        'entity_id' => ['entity_id'],
        'nid' => ['nid'],
      ],
      'unique keys' => [
        'table_record' => ['record_id'],
        'entity_id' => ['entity_id'],
      ],
    ];
    $public = \Drupal::database();
    $public->schema()->createTable('chado_' . $entity_type, $table_def);
  }

  /**
   * A helper function to add a recrod to the `tripal_bundle` table.
   *
   * A bundle_id is needed for the `chado_bundle` table that maps
   * entity types to chado tables.
   *
   * @param string $entity_type
   */
  protected function addTripalBundleRecord($entity_type, $cvterm, $label) {
    $public = \Drupal::database();
    $public->insert('tripal_bundle')
      ->fields([
        'type' => 'TripalEntity',
        'term_id' => $cvterm->getInternalId(),
        'name' => $entity_type,
        'label' => $label
      ])
      ->execute();

    return $public->select('tripal_bundle', 'TB')
      ->fields('TB', ['id'])
      ->condition('TB.name', $entity_type)
      ->execute()
      ->fetchField();
  }


  /**
   * a helper function to add a record to the `chado_bundle` table.
   */
  protected function addChadoBundleRecord($bundle_id, $data_table,
      $type_linker_table = NULL, $type_column = NULL, $type_id = NULL,
      $type_value = NULL, $base_type_id = NULL) {

    $public = \Drupal::database();
    $public->insert('chado_bundle')
      ->fields([
        'bundle_id' => $bundle_id,
        'data_table' => $data_table,
        'type_linker_table' => $type_linker_table,
        'type_column' => $type_column,
        'type_id' => $type_id,
        'type_value' => $type_value,
        'base_type_id' => $base_type_id,
      ])
      ->execute();

    return $public->select('chado_bundle', 'CB')
      ->fields('CB')
      ->condition('CB.bundle_id', $bundle_id, '')
      ->execute()
      ->fetchObject();

  }

  /**
   * A helper function to add the TAXRANK:species_subgroup term to Chado.
   */
  protected function addTaxRankSubGroupCVTerm() {

    // First add the vocabulary term for the organism.type_id column.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $taxrank = $idsmanager->createCollection('TAXRANK', 'chado_id_space');
    $vmanager->createCollection('taxonomic_rank', 'chado_vocabulary');
    $species_group = new TripalTerm([
      'name' => 'species_group',
      'idSpace' => 'TAXRANK',
      'vocabulary' => 'taxonomic_rank',
      'accession' => '0000010',
    ]);
    $taxrank->saveTerm($species_group);
    return $species_group;
  }

  /**
   * A helper function to add the local::note term to Chado.
   */
  protected function addLocalNoteCVTerm() {

    // First add the vocabulary term for the organism.type_id column.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $local = $idsmanager->createCollection('local', 'chado_id_space');
    $vmanager->createCollection('local', 'chado_vocabulary');
    $note = new TripalTerm([
      'name' => 'note',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'accession' => 'note',
    ]);
    $local->saveTerm($note);
    return $note;
  }


  /**
   * A helper function to add an organism record to Chado.
   */
  protected function addOryzaSativaRecord($type_term) {

    $this->chado->insert('1:organism')
      ->fields([
        'genus' => 'Oryza',
        'species' => 'sativa',
        'common_name' => 'rice',
        'abbreviation' => 'O.sativa',
        'infraspecific_name' => 'Japonica',
        'type_id' => $type_term->getInternalId(),
        'comment' => 'This is rice'
      ])
      ->execute();

    return $this->chado->select('1:organism', 'O')
      ->fields('O')
      ->condition('species', 'sativa')
      ->execute()
      ->fetchObject();
  }

  /**
   * A helper function to add the SO:0000704 (gene) term.
   * @return unknown
   */
  protected function addSOGeneCVterm() {
    // First add the vocabulary term for the organism.type_id column.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $sequence = $idsmanager->createCollection('SO', 'chado_id_space');
    $vmanager->createCollection('sequence', 'chado_vocabulary');
    $gene = new TripalTerm([
      'name' => 'gene',
      'idSpace' => 'SO',
      'vocabulary' => 'sequence',
      'accession' => '0000704)',
    ]);
    $sequence->saveTerm($gene);
    return $gene;
  }

  /**
   * A helper function for adding a gene recrod to the feature table.
   */
  protected function addFeatureRecord($name, $uniquename, $type, $organism) {

    $this->chado->insert('1:feature')
      ->fields([
        'name' => $name,
        'uniquename' => $uniquename,
        'type_id' => $type->getInternalId(),
        'organism_id' => $organism->organism_id,
      ])
      ->execute();

    return $this->chado->select('1:feature', 'F')
      ->fields('F')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();
  }

  /**
   * A helper function for adding notes values to the featureprop table.
   */
  protected function addFeaturePropRecords($feature, $term, $value, $rank) {
    $this->chado->insert('1:featureprop')
      ->fields([
        'feature_id' => $feature->feature_id,
        'type_id' => $term->getInternalId(),
        'value' => $value,
        'rank' => $rank,
      ])
      ->execute();
  }

  /**
   * A helper function to add a record to the `chado_bio_data_x` table.
   *
   * @param string $field_type
   * @param int $organism_id
   * @param int $entity_id
   */
  protected function addChadoBioDataRecord($entity_type, $entity_id, $record_id) {
    $public = \Drupal::database();
    $public->insert('chado_' . $entity_type)
      ->fields([
        'entity_id' => $entity_id,
        'record_id' => $record_id,
      ])
      ->execute();
  }

  /**
   * A helper function for adding a record to the chado_field table.
   */
  protected function addChadoFieldRecord($entity_type, $field_type, $key, $mapping) {
    $table = NULL;
    $column = NULL;
    $type_col = NULL;
    $type_id = NULL;
    $base_fk_col = NULL;
    $table_fk_col = NULL;
    if (array_key_exists('table', $mapping)) {
      $table = $mapping['table'];
    }
    if (array_key_exists('column', $mapping)) {
      $column = $mapping['column'];
    }
    if (array_key_exists('type_col', $mapping)) {
      $type_col = $mapping['type_col'];
    }
    if (array_key_exists('type_id', $mapping)) {
      $type_id = $mapping['type_id'];
    }
    if (array_key_exists('base_fk_col', $mapping)) {
      $base_fk_col = $mapping['base_fk_col'];
    }
    if (array_key_exists('table_fk_col', $mapping)) {
      $table_fk_col = $mapping['table_fk_col'];
    }
    $public = \Drupal::database();
    $public->insert('chado_fields')
      ->fields([
        'entity_type' => $entity_type,
        'field_type' => $field_type,
        'key' => $key,
        'table' => $table,
        'column' => $column,
        'type_col' => $type_col,
        'type_id' => $type_id,
        'base_fk_col' => $base_fk_col,
        'table_fk_col' => $table_fk_col,
      ])
      ->execute();
  }


  /**
   * Tests the ChadoIdSpace Class
   *
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testChadoStorage() {

    $storage_manager = \Drupal::service('tripal.storage');
    $chado_storage = $storage_manager->createInstance('chado_storage');

    // We'll simulare a "Gene" entity type (bundle).
    $entity_id = 1;
    $entity_type = 'bio_data_8';

    // Simularte A field with one property value.
    $name_field_type  = 'schema__name';

    // Simulate A field with multiple values for one property.
    $prop_field_type = 'local__note';

    // Simluate Acomplex field with multiple properties.
    $organism_field_type = 'obi__organism';


    //
    // Populate Chado with Data for Testing.
    //

    // Add the organism record.
    $type_term = $this->addTaxRankSubGroupCVTerm();
    $organism = $this->addOryzaSativaRecord($type_term);

    // Add the gene record.
    $gene_term = $this->addSOGeneCVterm();
    $gene = $this->addFeatureRecord('test_gene_name', 'test_gene_uname', $gene_term, $organism);

    // Add featureprop notes:
    $note_term = $this->addLocalNoteCVTerm();
    $this->addFeaturePropRecords($gene, $note_term, "Note 1", 0);
    $this->addFeaturePropRecords($gene, $note_term, "Note 2", 2);
    $this->addFeaturePropRecords($gene, $note_term, "Note 3", 1);

    // Create entries in the `tripal_bundle`, `chado_bundle` and
    // `chado_bio_data_x` tables.
    // @todo the createEntityTypeTable() creates the chado_bio_data_x table
    // and it can be removed from this test once those tables are created
    // by the prepare step. At the time of writing this test that part
    // isn't yet working.
    $this->createEntityTypeTable($entity_type);
    $bundle_id = $this->addTripalBundleRecord($entity_type, $gene_term, 'Gene');
    $this->addChadoBundleRecord($bundle_id, 'feature', NULL, 'type_id', $gene_term->getInternalID());
    $this->addChadoBioDataRecord($entity_type, $entity_id, $gene->feature_id);


    //
    // Create Properties
    //

    // First add the records to the chado_fields table that maps each
    // of the propeties to fields in Chado.
    $this->addChadoFieldRecord($entity_type, $name_field_type, 'schema:name', [
      'table' => 'feature',
      'column' => 'name'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'local:abbreviation', [
      'table' => 'organism',
      'column' => 'abbreviation',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'TAXRANK:0000005', [
      'table' => 'organism',
      'column' => 'genus',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'TAXRANK:0000006', [
      'table' => 'organism',
      'column' => 'species',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'NCBITaxon:common_name', [
      'table' => 'organism',
      'column' => 'common_name',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'TAXRANK:0000045', [
      'table' => 'organism',
      'column' => 'infraspecific_name',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'local:infraspecific_type', [
      'table' => 'organism',
      'column' => 'type_id',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'schema:description', [
      'table' => 'organism',
      'column' => 'comment',
      'base_fk_col' => 'organism_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $prop_field_type, 'local:note', [
      'table' => 'featureprop',
      'column' => 'value',
      'type_col' => 'type_id',
      'type_id' => $note_term->getInternalId(),
      'table_fk_col' => 'feature_id'
    ]);
    $this->addChadoFieldRecord($entity_type, $organism_field_type, 'organism_id', [
      'table' => 'feature2',
      'column' => 'organism_id'
    ]);

    // Note: we won't do any assertions of these constructors because they
    // should be tested in the Tripal module and the Chado implementations
    // just stores Chado table info and if that's broken all the tests
    // below will fail.

    // The gene name field single property.
    $name_type = new ChadoIntStoragePropertyType($entity_type, $name_field_type, 'schema:name');
    $name_value = new StoragePropertyValue($entity_type, $name_field_type, 'schema:name', $entity_id);

    // The organism complex field properties.
    $abbreviation_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'local:abbreviation');
    $genus_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000005', 255);
    $species_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000006', 255);
    $common_name_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'NCBITaxon:common_name', 255);
    $infra_name_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000045', 1024);
    $comment_type = new ChadoTextStoragePropertyType($entity_type, $organism_field_type, 'schema:description');

    $type_id_type = new ChadoIntStoragePropertyType($entity_type, $organism_field_type, 'local:infraspecific_type',);
    $abbreviation_value = new StoragePropertyValue($entity_type, $organism_field_type, 'local:abbreviation', $entity_id);
    $genus_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000005', $entity_id);
    $species_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000006', $entity_id);
    $common_name_value = new StoragePropertyValue($entity_type, $organism_field_type, 'NCBITaxon:common_name', $entity_id);
    $infra_name_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000045', $entity_id);
    $type_id_value = new StoragePropertyValue($entity_type, $organism_field_type, 'local:infraspecific_type', $entity_id);
    $comment_value = new StoragePropertyValue($entity_type, $organism_field_type, 'schema:description', $entity_id);

    // The note field single property with multiple values.
    $note_type = new ChadoIntStoragePropertyType($entity_type, $prop_field_type, 'local:note');
    $note_type->setCardinality(0);
    $note_value = new StoragePropertyValue($entity_type, $prop_field_type, 'local:note', $entity_id);

    // Bad properties.
    $bad_type = new ChadoIntStoragePropertyType($entity_type, $organism_field_type, 'organism_id');
    $bad_value = new StoragePropertyValue($entity_type, $organism_field_type, 'organism_id', $entity_id);


    //
    // Test Property Type and Value Creation.
    //

    // Test that the Chado table mapping is set in the property types.
    $this->assertTrue($genus_type->getTable() == 'organism', 'The table mapping is incorrect for the property type.');
    $this->assertTrue($genus_type->getColumn() == 'genus', 'The column mapping is incorrect for the property type.');
    $this->assertTrue($genus_type->getBaseFkColumn() == 'organism_id', 'The base FK mapping is incorrect for the property type.');
    $this->assertTrue($note_type->getTableFkColumn() == 'feature_id', 'The table FK mapping is incorrect for the property type.');
    $this->assertTrue($note_type->getTypeId() == $note_term->getInternalId(), 'The type ID mapping is incorrect for the property type.');
    $this->assertTrue($note_type->getTypeColumn() == 'type_id', 'The type column mapping is incorrect for the property type.');

    // Test validity of fields.
    $this->assertFalse($bad_type->isValid(), 'The bad property type should be invalid.');
    $this->assertTrue($abbreviation_type->isValid(), 'The abbreviation property type should be valid.');
    $this->assertTrue($genus_type->isValid(), 'The genus property type should be valid.');
    $this->assertTrue($species_type->isValid(), 'The species property type should be valid.');
    $this->assertTrue($common_name_type->isValid(), 'The common name property type should be valid.');
    $this->assertTrue($infra_name_type->isValid(), 'The infraspecific name property type should be valid.');
    $this->assertTrue($type_id_type->isValid(), 'The type_id property type should be valid.');
    $this->assertTrue($comment_type->isValid(), 'The comment property type should be valid.');
    $this->assertTrue($name_type->isValid(), 'The name property type should be valid.');
    $this->assertTrue($note_type->isValid(), 'The note property type should be valid.');


    //
    // Testing Loading of Property Values from Chado.
    //

    // Make sure the values start as empty.
    $this->assertTrue(empty($abbreviation_value->getValue()), 'The abbreviation property should not have a value.');
    $this->assertTrue(empty($genus_value->getValue()), 'The genus property should not have a value.');
    $this->assertTrue(empty($species_value->getValue()), 'The species property should not have a value.');
    $this->assertTrue(empty($common_name_value->getValue()), 'The species property should not have a value.');
    $this->assertTrue(empty($infra_name_value->getValue()), 'The infraspecific name property should not have a value.');
    $this->assertTrue(empty($type_id_value->getValue()), 'The infraspecific type_id property should not have a value.');
    $this->assertTrue(empty($comment_value->getValue()), 'The comment property should not have a value.');
    $this->assertTrue(empty($name_value->getValue()), 'The name property should not have a value.');
    $this->assertTrue(empty($note_value->getValue()), 'The note property should not have a value.');
    $this->assertTrue(empty($bad_value->getValue()), 'The bad property should not have a value.');

    // Add the types and load the values.
    $types = [
      ['local__abbreviation' => ['value' => $abbreviation_type]],
      ['TAXRANK__0000005' => ['value' => $genus_type]],
      ['TAXRANK__0000008'=> ['value' => $species_type]],
      ['NCBITaxon__common_name' => ['value' => $common_name_type]],
      ['TAXRANK__0000045' => ['value' => $infra_name_type]],
      ['local__infraspecific_type' => ['value' => $type_id_type]],
      ['schema__description' => ['value' => $comment_type]],
      ['local__infraspecific_type'=> ['value' => $bad_type]],
      ['schema__name'=> ['value' => $name_type]],
      ['local_note'=> ['value' => $note_type]],
    ];
    $values = [
      ['local__abbreviation' => [[$abbreviation_value]]],
      ['TAXRANK__0000005' => [[$genus_value]]],
      ['TAXRANK__0000008'=> [[$species_value]]],
      ['NCBITaxon__common_name' => [[$common_name_value]]],
      ['local__infraspecific_type' => [[$infra_name_value]]],
      ['local__infraspecific_type' => [[$type_id_value]]],
      ['schema__description' => [[$comment_value]]],
      ['organism_id' => [[$bad_value]]],
      ['schema__name'=> [[$name_value]]],
      ['local_note'=> [[$note_value]]]
    ];
    $chado_storage->addTypes($types);
    $chado_storage->loadValues($values);

    // Tests loading a property from a base table with a single value.
    $this->assertTrue($name_value->getValue() == 'test_gene_name', 'The name value was not loaded properly.');

    // Tests loading properites each with a single value from a linking table
    // where the foreign key is in the base table.
    $this->assertTrue($abbreviation_value->getValue() == 'O.sativa', 'The abbreviation value was not loaded properly.');
    $this->assertTrue($genus_value->getValue() == 'Oryza', 'The genus value was not loaded properly.');
    $this->assertTrue($species_value->getValue() == 'sativa', 'The species value was not loaded properly.');
    $this->assertTrue($common_name_value->getValue() == 'rice', 'The species common name value was not loaded properly.');
    $this->assertTrue($infra_name_value->getValue() == 'Japonica', 'The infraspecific name value was not loaded properly.');
    $this->assertTrue($type_id_value->getValue() == '2', 'The infraspecific type_id value was not loaded properly.');
    $this->assertTrue($comment_value->getValue() == 'This is rice', 'The comment value was not loaded properly.');

    // Tests loading a property from a linking table where the forkeign key
    // is in the linking table and the property has mutiple values.
    $this->assertTrue(is_array($note_value->getValue()), 'The note value should be an array.');
    $this->assertTrue(count($note_value->getValue()) == 3, 'The note value had the wrong number of elements.');
    $this->assertTrue($note_value->getValue()[0] == 'Note 1', 'The note first element is incorrect.');
    $this->assertTrue($note_value->getValue()[1] == 'Note 3', 'The note second element is incorrect.');
    $this->assertTrue($note_value->getValue()[2] == 'Note 2', 'The note third element is incorrect.');
    $this->assertTrue(empty($bad_value->getValue()), 'The bad property should have no value');

    // Test cardinality.
    $note_type->setCardinality(1);
    $values = [$note_value];
    $chado_storage->loadValues($values);
    $this->assertTrue(empty($note_value->getValue()), 'The note property should not have a value.');
    $note_type->setCardinality(3);
    $chado_storage->loadValues($values);
    $this->assertTrue(count($note_value->getValue()) == 3, 'The note value had the wrong number of elements.');
  }
}


