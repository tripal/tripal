<?php

namespace Drupal\Tests\tripal\Functional\Services;

use Drupal\Core\Database\Database;
use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Test\FunctionalTestSetupTrait;
use Drupal\tripal\TripalStorage\TripalStorageBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use \Drupal\tripal\Services\TripalTokenParser;

/**
 * Tests the basic functions of the TripalContentTypes Service.
 *
 * @group Tripal
 * @group Tripal Services
 * @group TripalPublish
 */
class TripalTokenParserTest extends TripalTestBrowserBase {

  /**
   * Tests the TripalContentTypes class public functions.
   */
  public function testTripalTokenParser() {

    //
    // Create an organism content type.
    //

    // Create the vocabulary term
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $idspace = $idsmanager->createCollection('OBI', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('OBI', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'organism',
      'idSpace' => 'OBI',
      'vocabulary' => 'OBI',
      'accession' => '0100026',
      'definition' => '',
    ]);
    $created = $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create organism term');

    // Create the content type.
    $oganism_config = [
      'label' => 'Organism',
      'term' => $term,
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1']
    ];
    /** @var \Drupal\tripal\Services\TripalEntityTypeCollection $content_type_service **/
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $organism = $content_type_service->createContentType($oganism_config);
    $this->assertTrue(!is_null($organism), "Failed to create the organism content type with a valid definition.");

    //
    // Add fields to the content type.
    //
    /** @var \Drupal\tripal\Services\TripalFieldCollection $fields_service **/
    $fields_service = \Drupal::service('tripal.tripalfield_collection');

    // Add TAXRANK Terms
    $idspace = $idsmanager->createCollection('TAXRANK', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('TAXRANK', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'genus',
      'idSpace' => 'TAXRANK',
      'vocabulary' => 'TAXRANK',
      'accession' => '0000005',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create genus term');
    $term = new TripalTerm([
      'name' => 'species',
      'idSpace' => 'TAXRANK',
      'vocabulary' => 'TAXRANK',
      'accession' => '0000006',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create species term');
    $term = new TripalTerm([
      'name' => 'infraspecies',
      'idSpace' => 'TAXRANK',
      'vocabulary' => 'TAXRANK',
      'accession' => '0000045',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create infraspecies term');

    // Add local Terms
    $idspace = $idsmanager->createCollection('local', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('local', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'infrasepecific type',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'accession' => 'infraspecific_type',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create infrasepecific type term');
    $term = new TripalTerm([
      'name' => 'abbreviation',
      'idSpace' => 'local',
      'vocabulary' => 'local',
      'accession' => 'abbreviation',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create abbreviation term');

    // Add NCBITaxon Terms
    $idspace = $idsmanager->createCollection('NCBITaxon', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('NCBITaxon', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'common name',
      'idSpace' => 'NCBITaxon',
      'vocabulary' => 'NCBITaxon',
      'accession' => 'common_name',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create common name term');

    // Add schema Terms
    $idspace = $idsmanager->createCollection('schema', "tripal_default_id_space");
    $vocab = $vmanager->createCollection('schema', "tripal_default_vocabulary");
    $term = new TripalTerm([
      'name' => 'description',
      'idSpace' => 'schema',
      'vocabulary' => 'schema',
      'accession' => 'description',
      'definition' => '',
    ]);
    $idspace->saveTerm($term);
    $this->assertTrue($created, 'Could not create description term');

    $genus_field = [
      'name' => 'organism_genus',
      'content_type' => 'organism',
      'label' => 'Genus',
      'type' => 'tripal_string_type',
      'description' => 'The genus name of the organism.',
      'cardinality' => 1,
      'required' => TRUE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'TAXRANK',
        'termAccession' => '0000005',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 15,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 15,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($genus_field);
    $this->assertTrue($is_valid, "The genus field definition is invalid.");
    $is_added = $fields_service->addBundleField($genus_field);
    $this->assertTrue($is_added, "The genus field could not be added to the bundle.");

    $species_field = [
      'name' => 'organism_species',
      'content_type' => 'organism',
      'label' => 'Species',
      'type' => 'tripal_string_type',
      'description' => 'The species name of the organism.',
      'cardinality' => 1,
      'required' => TRUE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'TAXRANK',
        'termAccession' => '0000006',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 20,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 20,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($species_field);
    $this->assertTrue($is_valid, "The species field definition is invalid.");
    $is_added = $fields_service->addBundleField($species_field);
    $this->assertTrue($is_added, "The species field could not be added to the bundle.");


    $infraspecific_name_field = [
      'name' => 'organism_infraspecific_name',
      'content_type' => 'organism',
      'label' => 'Infraspecies',
      'type' => 'tripal_string_type',
      'description' => 'The infraspecfic name for the organism.',
      'cardinality' => 1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 1024,
      ],
      'settings' => [
        'termIdSpace' => 'TAXRANK',
        'termAccession' => '0000045',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 30,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 30,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($infraspecific_name_field);
    $this->assertTrue($is_valid, "The infraspecific name field definition is invalid.");
    $is_added = $fields_service->addBundleField($infraspecific_name_field);
    $this->assertTrue($is_added, "The infraspecific name field could not be added to the bundle.");

    $comment_field = [
      'name' => 'organism_comment',
      'content_type' => 'organism',
      'label' => 'Description',
      'type' => 'tripal_string_type',
      'description' => 'A description of the organism.',
      'cardinality' => 1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'schema',
        'termAccession' => 'description',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 35,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 35,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($comment_field);
    $this->assertTrue($is_valid, "The comment field definition is invalid.");
    $is_added = $fields_service->addBundleField($comment_field);
    $this->assertTrue($is_added, "The comment field could not be added to the bundle.");

    $infraspecific_type_field = [
      'name' => 'organism_infraspecific_type',
      'content_type' => 'organism',
      'label' => 'Infraspecific Type',
      'type' => 'tripal_string_type',
      'description' => 'The connector type (e.g. subspecies, varietas, forma, etc.) for the infraspecific name.',
      'cardinality' => 1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
      ],
      'settings' => [
        'termIdSpace' => 'local',
        'termAccession' => 'infraspecific_type',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 10,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 10,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($infraspecific_type_field);
    $this->assertTrue($is_valid, "The infraspecific type field definition is invalid.");
    $is_added = $fields_service->addBundleField($infraspecific_type_field);
    $this->assertTrue($is_added, "The infraspecific type field could not be added to the bundle.");

    $abbreviation_field = [
      'name' => 'organism_abbreviation',
      'content_type' => 'organism',
      'label' => 'Abbreviation',
      'type' => 'tripal_string_type',
      'description' => 'A shortened name (or abbreviation) for the organism (e.g. O. sativa).',
      'cardinality' => 1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'local',
        'termAccession' => 'abbreviation',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 10,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 10,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($abbreviation_field);
    $this->assertTrue($is_valid, "The abbreviation field definition is invalid.");
    $is_added = $fields_service->addBundleField($abbreviation_field);
    $this->assertTrue($is_added, "The abbreviation field could not be added to the bundle.");

    $common_name_field = [
      'name' => 'organism_common_name',
      'content_type' => 'organism',
      'label' => 'Common Name',
      'type' => 'tripal_string_type',
      'description' => 'The common name for the organism.',
      'cardinality' => 1,
      'required' => FALSE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'NCBITaxon',
        'termAccession' => 'common_name',
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 25,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 25,
          ],
        ],
      ],
    ];
    $is_valid = $fields_service->validate($common_name_field);
    $this->assertTrue($is_valid, "The common name field definition is invalid.");
    $is_added = $fields_service->addBundleField($common_name_field);
    $this->assertTrue($is_added, "The common name field could not be added to the bundle.");

    //
    // Test the functions of the token parser service.
    //
    /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
    $token_parser = \Drupal::service('tripal.token_parser');
    $token_parser->initParser($organism);
    $this->assertTrue($token_parser->getBundle()->getId() === 'organism', "The tripal token parser didn't set the bundle properly.");
    $this->assertTrue(is_null($token_parser->getEntity()), "The tripal token parser should have a null entity.");
    $field_names = $token_parser->getFieldNames();

    // Make sure all of the fields are present in the token parser.
    // This ensures that the bundle was found and it was able to
    // create instances of all the fields attached to it.
    $this->assertTrue(in_array('organism_genus', $field_names), 'The tripal token parser is missing the organism_genus field.');
    $this->assertTrue(in_array('organism_species', $field_names), 'The tripal token parser is missing the organism_species field.');
    $this->assertTrue(in_array('organism_abbreviation', $field_names), 'The tripal token parser is missing the organism_abbreviation field.');
    $this->assertTrue(in_array('organism_common_name', $field_names), 'The tripal token parser is missing the organism_common_name field.');
    $this->assertTrue(in_array('organism_infraspecific_type', $field_names), 'The tripal token parser is missing the organism_infraspecific_type field.');
    $this->assertTrue(in_array('organism_infraspecific_name', $field_names), 'The tripal token parser is missing the organism_infraspecific_name field.');
    $this->assertTrue(in_array('organism_comment', $field_names), 'The tripal token parser is missing the organism_comment field.');
    $this->assertTrue(count($field_names) == 7, 'The tripal token parser has more fields than expected.');

    // Add some values without an entity
    $token_parser->addFieldValue('organism_genus', 'value', 'Oryza');
    $token_parser->addFieldValue('organism_species', 'value', 'sativa');
    $token_parser->addFieldValue('organism_abbreviation', 'value', 'O. sativa');
    $token_parser->addFieldValue('organism_common_name', 'value', 'Japonica rice');
    // Comment is set to Basic HTML, so test with some HTML markup.
    $token_parser->addFieldValue('organism_comment', 'value', '<p>rice is <em>nice</em></p>');
    $values = $token_parser->getValues();

    $this->assertTrue(in_array('organism_genus', array_keys($values)), 'The tripal token parser is missing the organism_genus value field name.');
    $this->assertTrue(in_array('organism_species', array_keys($values)), 'The tripal token parser is missing the organism_species value field name.');
    $this->assertTrue(in_array('organism_abbreviation', array_keys($values)), 'The tripal token parser is missing the organism_abbreviation value field name.');
    $this->assertTrue(in_array('organism_common_name', array_keys($values)), 'The tripal token parser is missing the organism_common_name value field name.');
    $this->assertTrue(in_array('organism_comment', array_keys($values)), 'The tripal token parser is missing the organism_comment value field name.');

    $this->assertTrue(array_key_exists('value', $values['organism_genus']), 'The tripal token parser is missing the organism_genus value key.');
    $this->assertTrue(array_key_exists('value', $values['organism_species']), 'The tripal token parser is missing the organism_species value key.');
    $this->assertTrue(array_key_exists('value', $values['organism_abbreviation']), 'The tripal token parser is missing the organism_abbreviation value key.');
    $this->assertTrue(array_key_exists('value', $values['organism_common_name']), 'The tripal token parser is missing the organism_common_name value key.');
    $this->assertTrue(array_key_exists('value', $values['organism_comment']), 'The tripal token parser is missing the organism_comment value key.');

    $this->assertTrue($values['organism_genus']['value'] == 'Oryza', 'The tripal token parser is missing the organism_genus value.');
    $this->assertTrue($values['organism_species']['value'] == 'sativa', 'The tripal token parser is missing the organism_species value.');
    $this->assertTrue($values['organism_abbreviation']['value'] == 'O. sativa', 'The tripal token parser is missing the organism_abbreviation value.');
    $this->assertTrue($values['organism_common_name']['value'] == 'Japonica rice', 'The tripal token parser is missing the organism_common_name value.');
    $this->assertTrue($values['organism_comment']['value'] == '<p>rice is <em>nice</em></p>', 'The tripal token parser is missing the organism_comment value.');
    $this->assertTrue(count(array_keys($values)) == 5, 'The tripal token parser has more values than expected.');


    $replaced = $token_parser->replaceTokens(['[organism_genus] [organism_species]']);
    $this->assertTrue(is_array($replaced), 'TripalTokenParser::replaceTokens() does not return an array');
    $this->assertTrue(count($replaced) == 1, 'TripalTokenParser::replaceTokens() should have returned only one replaced string');
    $this->assertTrue($replaced[0] == 'Oryza sativa', 'TripalTokenParser did not correctly replace tokens');

    $replaced = $token_parser->replaceTokens([
      '[organism_genus] [organism_species]',
      '[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]'
    ]);
    $this->assertTrue(count($replaced) == 2, 'TripalTokenParser::replaceTokens() should have returned only two replaced string');
    $this->assertTrue($replaced[0] == 'Oryza sativa', 'TripalTokenParser did not return a correctly replaced string when multiple were provided..');
    $this->assertTrue($replaced[1] == 'Oryza sativa [organism_infraspecific_type] [organism_infraspecific_name]', 'TripalTokenParser did not return unparsed tokens in the input string.');

    $token_parser->addFieldValue('organism_infraspecific_type', 'value', '');
    $replaced = $token_parser->replaceTokens([
      '[organism_genus] [organism_species] [organism_infraspecific_type]'
    ]);
    $this->assertTrue($replaced[0] == 'Oryza sativa', 'TripalTokenParser did not correctly replace a token with an empty value and trim the result.');

    $token_parser->addFieldValue('organism_infraspecific_type', 'value', 'subspecies');
    $token_parser->addFieldValue('organism_infraspecific_name', 'value', 'Japonica');
    $replaced = $token_parser->replaceTokens([
      '<i>[organism_genus] [organism_species]</i> [organism_infraspecific_type] <em>[organism_infraspecific_name]</em>'
    ]);
    $this->assertTrue($replaced[0] == '<i>Oryza sativa</i> subspecies <em>Japonica</em>', 'TripalTokenParser did not correctly replace all of the tokens.');

    //
    // Test Entity / Bundle tokens.
    //
    /** @var \Drupal\tripal\Entity\TripalEntity $entity **/
    $values = [];
    $values['title'] = 'Test ' . uniqid();
    $values['type'] = 'organism';
    $values['organism_genus'] = [['value' => 'Oryza']];
    $values['organism_species'] = [['value' => 'sativa']];
    $values['organism_infraspecific_type'] = [['value' => 'subspecies']];
    $values['organism_infraspecific_name'] = [['value' => 'Japonica']];
    $values['organism_abbreviation'] = [['value' => 'O. sativa']];
    $values['organism_common_name'] = [['value' => 'Japonica rice']];
    $values['organism_comment'] = [['value' => 'rice is nice']];
    $entity = \Drupal\tripal\Entity\TripalEntity::create($values);
    $this->assertIsObject($entity, "Unable to create an organism entity.");
    $entity->save();

    $token_parser->setEntity($entity);
    $this->assertTrue($token_parser->getEntity()->getID() == $entity->getId(), 'The tripal token parser did not return the entity as expected.');
    $replaced = $token_parser->replaceTokens([
      '[TripalBundle__bundle_id]',
      '[TripalEntityType__label]',
      '[TripalEntity__entity_id]'
    ]);
    $this->assertTrue($replaced[0] == 'organism', 'The [TripalBundle__bundle_id] token is not being replaced.');
    $this->assertTrue($replaced[1] == 'Organism', 'The [TripalEntityType__label] token is not being replaced.');
    $this->assertTrue($replaced[2] == '1', 'The [TripalEntity__entity_id] token is not being replaced.');

    // Test calling the getFieldValues() function directly
    $field_values = $entity->getFieldValues();
    $this->assertIsArray($field_values, "getFieldValues did not return an array.");
    $this->assertEquals(14, count($field_values), "getFieldValues returned an array of unexpected size.");
    $value = $field_values['organism_infraspecific_type'][0]['value'] ?? NULL;
    $this->assertEquals('subspecies', $value, "getFieldValues did not return the correct infraspecific type.");

    // Test calling the getEntityTitle() function directly
    $entity_title = $token_parser->getEntityTitle($token_parser->getBundle(), $field_values);
    $this->assertEquals('Oryza sativa subspecies Japonica', $entity_title, "getEntityTitle did not return the correct title");
  }
}
