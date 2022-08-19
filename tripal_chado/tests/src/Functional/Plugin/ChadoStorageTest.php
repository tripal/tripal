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

    //
    // Test property type classes
    //

    // Note: these currently add no additional functionality over the Tripal
    // versions of these property types. I'm adding basic tests that they create
    // for extra coverage of deprecation notices and as placeholders for
    // future expansion. Also as an extra test of the Tripal core contructors
    // that Chado-specific values + naming conventions create smoothly.

    // The gene name field single property.
    $name_type = new ChadoIntStoragePropertyType($entity_type, $name_field_type, 'schema:name');
    $this->assertIsObject($name_type, "Unable to create ChadoIntStoragePropertyType: $name_field_type, schema:name");
    $name_value = new StoragePropertyValue($entity_type, $name_field_type, 'schema:name', $entity_id);
    $this->assertIsObject($name_value, "Unable to create StoragePropertyValue: $name_field_type, 'schema:name', $entity_id");

    // The organism complex field properties.
    $abbreviation_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'local:abbreviation');
    $this->assertIsObject($abbreviation_type, "Unable to create ChadoVarCharStoragePropertyType: $organism_field_type, local:abbreviation");
    $genus_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000005', 255);
    $this->assertIsObject($genus_type, "Unable to create ChadoVarCharStoragePropertyType: $organism_field_type, TAXRANK:0000005");
    $species_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000006', 255);
    $this->assertIsObject($species_type, "Unable to create ChadoVarCharStoragePropertyType: $organism_field_type, TAXRANK:0000006");
    $common_name_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'NCBITaxon:common_name', 255);
    $this->assertIsObject($common_name_type, "Unable to create ChadoVarCharStoragePropertyType: $organism_field_type, NCBITaxon:common_name");
    $infra_name_type = new ChadoVarCharStoragePropertyType($entity_type, $organism_field_type, 'TAXRANK:0000045', 1024);
    $this->assertIsObject($infra_name_type, "Unable to create ChadoVarCharStoragePropertyType: $organism_field_type, TAXRANK:0000045");
    $comment_type = new ChadoTextStoragePropertyType($entity_type, $organism_field_type, 'schema:description');
    $this->assertIsObject($comment_type, "Unable to create ChadoTextStoragePropertyType: $organism_field_type, schema:description");
    $type_id_type = new ChadoIntStoragePropertyType($entity_type, $organism_field_type, 'local:infraspecific_type');
    $this->assertIsObject($type_id_type, "Unable to create ChadoIntStoragePropertyType: $organism_field_type, local:infraspecific_type");

    $abbreviation_value = new StoragePropertyValue($entity_type, $organism_field_type, 'local:abbreviation', $entity_id);
    $this->assertIsObject($abbreviation_value, "Unable to create StoragePropertyValue: $organism_field_type, 'local:abbreviation', $entity_id");
    $genus_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000005', $entity_id);
    $this->assertIsObject($genus_value, "Unable to create StoragePropertyValue: $organism_field_type, 'TAXRANK:0000005', $entity_id");
    $species_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000006', $entity_id);
    $this->assertIsObject($species_value, "Unable to create StoragePropertyValue: $organism_field_type, 'TAXRANK:0000006', $entity_id");
    $common_name_value = new StoragePropertyValue($entity_type, $organism_field_type, 'NCBITaxon:common_name', $entity_id);
    $this->assertIsObject($common_name_value, "Unable to create StoragePropertyValue: $organism_field_type, 'NCBITaxon:common_name', $entity_id");
    $infra_name_value = new StoragePropertyValue($entity_type, $organism_field_type, 'TAXRANK:0000045', $entity_id);
    $this->assertIsObject($infra_name_value, "Unable to create StoragePropertyValue: $organism_field_type, 'TAXRANK:0000045', $entity_id");
    $type_id_value = new StoragePropertyValue($entity_type, $organism_field_type, 'local:infraspecific_type', $entity_id);
    $this->assertIsObject($type_id_value, "Unable to create StoragePropertyValue: $organism_field_type, 'local:infraspecific_type', $entity_id");
    $comment_value = new StoragePropertyValue($entity_type, $organism_field_type, 'schema:description', $entity_id);
    $this->assertIsObject($comment_value, "Unable to create StoragePropertyValue: $organism_field_type, 'schema:description', $entity_id");

    // The note field single property with multiple values.
    $note_type = new ChadoIntStoragePropertyType($entity_type, $prop_field_type, 'local:note');
    $this->assertIsObject($note_type, "Unable to create ChadoIntStoragePropertyType: $prop_field_type, local:note");
    $note_type->setCardinality(0);
    $note_value = new StoragePropertyValue($entity_type, $prop_field_type, 'local:note', $entity_id);
    $this->assertIsObject($note_value, "Unable to create StoragePropertyValue: $prop_field_type, 'local:note', $entity_id");

    // Make sure the values start as empty.
    $this->assertTrue(empty($abbreviation_value->getValue()), 'The abbreviation property should not have a value.');
    $this->assertTrue(empty($genus_value->getValue()), 'The genus property should not have a value.');
    $this->assertTrue(empty($species_value->getValue()), 'The species property should not have a value.');
    $this->assertTrue(empty($common_name_value->getValue()), 'The species property should not have a value.');
    $this->assertTrue(empty($infra_name_value->getValue()), 'The infraspecific name property should not have a value.');
    $this->assertTrue(empty($type_id_value->getValue()), 'The infraspecific type_id property should not have a value.');
    $this->assertTrue(empty($comment_value->getValue()), 'The comment property should not have a value.');
    $this->assertTrue(empty($name_value->getValue()), 'The name property should not have a value.');

    // Add the types and load the values.
    $types = [
      $abbreviation_type, $genus_type, $species_type, $common_name_type,
      $infra_name_type, $type_id_type, $comment_type, $name_type, $note_type
    ];
    $values = [
      ['local__abbreviation' => [[$abbreviation_value]]],
      ['TAXRANK__0000005' => [[$genus_value]]],
      ['TAXRANK__0000008'=> [[$species_value]]],
      ['NCBITaxon__common_name' => [[$common_name_value]]],
      ['local__infraspecific_type' => [[$infra_name_value]]],
      ['local__infraspecific_type' => [[$type_id_value]]],
      ['schema__description' => [[$comment_value]]],
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
}
