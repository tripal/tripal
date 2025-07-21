<?php

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal\TripalVocabTerms\TripalTerm;

/**
 * Tests the discover() functions for Chado.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Discover
 */
class ChadoDiscoverTest extends ChadoTestBrowserBase {

  /**
   *  Tests the Chado discover() function
   */
  public function testDiscover() {
    // Create and then get the existing test chado schema name.
    $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);
    $chado = \Drupal::service('tripal_chado.database');
    $default_chado_schema = $chado->schema()->getDefault();

    /** @var \Drupal\tripal\Services\TripalFieldCollection $fields_service **/
    $fields_service = \Drupal::service('tripal.tripalfield_collection');

    /** @var \Drupal\tripal\Services\TripalContentTypes $content_type_setup **/
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');

    // Create the vocabulary term needed for testing the content type.
    // We'll use the default Tripal IdSpace and Vocabulary plugins.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');
    $idspaces = [];
    $new_collections = [
      'OBI' => 'OBI',
      'TPUB' => 'TPUB',
      'CO_010' => 'CO_010',
      'SO' => 'sequence',
      'data' => 'EDAM',
      'UO' => 'uo',
      'operation' => 'EDAM',
    ];
    foreach ($new_collections as $id => $cv) {
      $idspace[$id] = $idsmanager->createCollection($id, 'tripal_default_id_space');
      $vocab = $vmanager->createCollection($cv, 'tripal_default_vocabulary');
    }
    $terms = [];

    $terms['organism'] = new TripalTerm([
      'name' => 'organism',
      'idSpace' => 'OBI',
      'vocabulary' => 'OBI',
      'accession' => '0100026',
      'definition' => 'Test organism',
    ]);
    $idspace['OBI']->saveTerm($terms['organism']);

    $terms['stock'] = new TripalTerm([
      'name' => 'stock',
      'idSpace' => 'CO_010',
      'vocabulary' => 'CO_010',
      'accession' => 'accession',
      'definition' => 'Test stock',
    ]);
    $idspace['CO_010']->saveTerm($terms['stock']);

    $terms['pub'] = new TripalTerm([
      'name' => 'pub',
      'idSpace' => 'TPUB',
      'vocabulary' => 'TPUB',
      'accession' => '0000002',
      'definition' => 'Test pub',
    ]);
    $idspace['TPUB']->saveTerm($terms['pub']);

    $terms['speciestree'] = new TripalTerm([
      'name' => 'speciestree',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'accession' => '3272',
      'definition' => 'Test speciestree',
    ]);
    $idspace['data']->saveTerm($terms['speciestree']);

    $terms['map'] = new TripalTerm([
      'name' => 'map',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'accession' => '1278',
      'definition' => 'Test map',
    ]);
    $idspace['data']->saveTerm($terms['map']);
    $terms['unit'] = new TripalTerm([
      'name' => 'unit',
      'idSpace' => 'UO',
      'vocabulary' => 'uo',
      'accession' => '0000000',
      'definition' => 'Test unit',
    ]);
    $idspace['UO']->saveTerm($terms['unit']);

    $terms['gene'] = new TripalTerm([
      'name' => 'gene',
      'idSpace' => 'SO',
      'vocabulary' => 'sequence',
      'accession' => '0000704',
      'definition' => 'Test gene',
    ]);
    $idspace['SO']->saveTerm($terms['gene']);
    $terms['Sequence'] = new TripalTerm([
      'name' => 'Sequence',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'accession' => '2044',
      'definition' => 'Test sequence residues',
    ]);
    $idspace['data']->saveTerm($terms['Sequence']);
    $terms['Sequence'] = new TripalTerm([
      'name' => 'Sequence length',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'accession' => '1249',
      'definition' => 'Test sequence length',
    ]);
    $idspace['data']->saveTerm($terms['Sequence']);
    $terms['Sequence'] = new TripalTerm([
      'name' => 'Sequence checksum',
      'idSpace' => 'data',
      'vocabulary' => 'EDAM',
      'accession' => '2190',
      'definition' => 'Test sequence checksum',
    ]);
    $idspace['data']->saveTerm($terms['Sequence']);
    $terms['phylotreevis'] = new TripalTerm([
      'name' => 'Phylogenetic tree visualisation',
      'idSpace' => 'operation',
      'vocabulary' => 'EDAM',
      'accession' => '0567',
      'definition' => 'Render or visualise a phylogenetic tree.',
    ]);
    $idspace['operation']->saveTerm($terms['phylotreevis']);

    $content_types = [];

    $ct_details = [
      'label' => 'Organism',
      'term' => $terms['organism'],
      'help_text' => 'Generic organism content type',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => '[organism_genus] [organism_species]',
      'url_format' => 'organism/[TripalEntity__entity_id]',
      'synonyms' => ['bio_data_1'],
      'settings' => [
        'chado_base_table' => 'organism',
      ],
    ];
    $content_types['organism'] = $content_type_service->createContentType($ct_details);
    $content_types['organism']->setThirdPartySetting('tripal', 'chado_base_table', 'organism');

    $ct_details = [
      'label' => 'Stock',
      'term' => $terms['stock'],
      'help_text' => 'Generic stock content type',
      'category' => 'General',
      'id' => 'stock',
      'title_format' => '[stock_name]',
      'url_format' => 'stock/[TripalEntity__entity_id]',
      'synonyms' => ['bio_data_2'],
      'settings' => [
        'chado_base_table' => 'pub',
      ],
    ];
    $content_types['stock'] = $content_type_service->createContentType($ct_details);
    $content_types['stock']->setThirdPartySetting('tripal', 'chado_base_table', 'stock');

    $ct_details = [
      'label' => 'Publication',
      'term' => $terms['pub'],
      'help_text' => 'Generic publication content type',
      'category' => 'General',
      'id' => 'pub',
      'title_format' => '[pub_title]',
      'url_format' => 'pub/[TripalEntity__entity_id]',
      'synonyms' => ['bio_data_3'],
      'settings' => [
        'chado_base_table' => 'pub',
      ],
    ];
    $content_types['pub'] = $content_type_service->createContentType($ct_details);
    $content_types['pub']->setThirdPartySetting('tripal', 'chado_base_table', 'pub');

    $ct_details = [
      'label' => 'Species Tree',
      'term' => $terms['speciestree'],
      'help_text' => 'Taxonomy tree content type',
      'category' => 'General',
      'id' => 'speciestree',
      'title_format' => '[speciestree_name]',
      'url_format' => 'speciestree/[TripalEntity__entity_id]',
      'settings' => [
        'chado_base_table' => 'phylotree',
      ],
    ];
    $content_types['speciestree'] = $content_type_service->createContentType($ct_details);
    $content_types['speciestree']->setThirdPartySetting('tripal', 'chado_base_table', 'phylotree');

    $ct_details = [
      'label' => 'Map',
      'term' => $terms['map'],
      'help_text' => 'Generic map content type',
      'category' => 'General',
      'id' => 'map',
      'title_format' => '[map_name]',
      'url_format' => 'map/[TripalEntity__entity_id]',
      'synonyms' => ['bio_data_5'],
      'settings' => [
        'chado_base_table' => 'featuremap',
      ],
    ];
    $content_types['map'] = $content_type_service->createContentType($ct_details);
    $content_types['map']->setThirdPartySetting('tripal', 'chado_base_table', 'featuremap');

    $ct_details = [
      'label' => 'Gene',
      'term' => $terms['gene'],
      'help_text' => 'Generic Gene content type',
      'category' => 'General',
      'id' => 'gene',
      'title_format' => '[gene_name]',
      'url_format' => 'gene/[TripalEntity__entity_id]',
      'synonyms' => ['bio_data_4'],
      'settings' => [
        'chado_base_table' => 'feature',
      ],
    ];
    $content_types['gene'] = $content_type_service->createContentType($ct_details);
    $content_types['gene']->setThirdPartySetting('tripal', 'chado_base_table', 'feature');

    // Perform discover() on the stock content type
    // This has a direct link to organism (organism_id) and an indirect link
    // to pub through the stock_pub table, so we can test both linking methods.
    $discovered_fields = $fields_service->discover($content_types['stock']);

    // Test that the discover function returns the expected keys.
    $expected_keys = ['new', 'existing', 'invalid'];
    foreach ($expected_keys as $expected_key) {
      $this->assertArrayHasKey($expected_key, $discovered_fields, 'Missing the "' . $expected_key . '" key in the array returned by the discover() function.');
    }

    // So far there should be no existing fields
    $this->assertEmpty($discovered_fields['existing'], 'We did not expect to find any existing fields yet');

    // Test that the direct field was discovered, i.e. organism
    $this->assertNotEmpty($discovered_fields['new'], 'No new fields were discovered for stock');
    $this->assertArrayHasKey('stock_organism', $discovered_fields['new'], 'The organism field was not discovered for stock');
    // The type field should also have been discovered
    $this->assertArrayHasKey('stock_type', $discovered_fields['new'], 'The type field was not discovered for stock');

    // Test that a field through a linker table was discovered
    $this->assertArrayHasKey('stock_pub', $discovered_fields['new'], 'The publication field was not discovered for stock');

    // Because we didn't set up terms for these types, they should be flagged as invalid
    $this->assertNotEmpty($discovered_fields['invalid'], 'No invalid fields were discovered for stock');
    $this->assertArrayHasKey('stock_feature', $discovered_fields['invalid'], 'The feature field was not flagged as invalid for stock');
    $this->assertArrayHasKey('stock_featuremap', $discovered_fields['invalid'], 'The featuremap field was not flagged as invalid for stock');
    $this->assertArrayHasKey('stock_project', $discovered_fields['invalid'], 'The project field was not flagged as invalid for stock');

    // Test of the unit field on a generic map (featuremap table)
    $discovered_fields = $fields_service->discover($content_types['map']);
    $this->assertArrayHasKey('map_unittype_id', $discovered_fields['new'], 'The unit type field was not discovered for featuremap');

    // Test discovery of a field using a column in the base table
    $discovered_fields = $fields_service->discover($content_types['gene']);
    $this->assertArrayHasKey('gene_residues', $discovered_fields['new'], 'The sequence residues field was not discovered for gene');
    $this->assertArrayHasKey('gene_seqlen', $discovered_fields['new'], 'The sequence length field was not discovered for gene');
    $this->assertArrayHasKey('gene_md5checksum', $discovered_fields['new'], 'The MD5 checksum field was not discovered for gene');

    // Test discovery of a function-based field. This has different
    // processing in its discover function because table is the same
    // as base_table.
    $discovered_fields = $fields_service->discover($content_types['speciestree']);
    $this->assertArrayHasKey('speciestree_phylotreevis', $discovered_fields['new'], 'The phylotreevis field was not discovered for speciestree');
  }
}
