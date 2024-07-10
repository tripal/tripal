<?php

namespace Drupal\Tests\tripal\Functional\Plugin;

use Drupal\Core\Database\Database;
use Drupal\Tests\tripal\Functional\TripalTestBrowserBase;
use Drupal\Core\Test\FunctionalTestSetupTrait;
use Drupal\tripal\TripalVocabTerms\TripalTerm;



/**
 * Tests for the TripalDefaultIdSpace classes
 *
 * @group Tripal
 * @group TripalDefaultIdSpace
 */
class TripalDefaultIdSpaceTest extends TripalTestBrowserBase {


  /**
   * A helper function to retrieve an id_space record.
   *
   * @param string $dbname
   *   The name of the id_space to lookup.
   *
   * @return A database query result.
   */
  protected function getIdSpace($id_space) {

    $conn = \Drupal::service('database');
    $query = $conn->select('tripal_terms_idspaces', 'idspace');
    $query = $query->condition('name', $id_space, '=');
    $query = $query->fields('idspace');
    $result = $query->execute();
    if (!$result) {
      return [];
    }
    return $result->fetchAssoc();
  }

  /**
   * Tests the TripalDefaultIdSpace and TripalDefaultVocabulary Classes
   *
   */
  public function testTripaDefaultIdSpace() {

    // Create Collection managers.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');

    // These are the values we'll use for the ID space and vocablary.
    $GO_idspace = 'GO';
    $GO_cc_namespace = 'cellular_component';
    $GO_bp_namespace = 'biological_process';
    $GO_mf_namespace = 'molecular_function';
    $GO_description = "The Gene Ontology (GO) knowledgebase is the world’s largest source of information on the functions of genes";
    $GO_cc_label = 'Gene Ontology Cellular Component Vocabulary';
    $GO_bp_label = 'Gene Ontology Biological Process Vocabulary';
    $GO_mf_label = 'Gene Ontology Molecular Function Vocabulary';
    $GO_urlprefix = "http://amigo.geneontology.org/amigo/term/{db}:{accession}";
    $GO_url = 'http://geneontology.org/';

    //
    // Testing TripalDefaultIdSpace Functionality
    //

    // Create the ID space object and make sure a Chado record got created.
    $GO = $idsmanager->createCollection($GO_idspace, "tripal_default_id_space");
    $db = $this->getIdSpace($GO_idspace);
    $this->assertTrue($db['name'] == $GO_idspace, 'The name was not set correctly by the TripaDefaultIdSpace object.');
    $this->assertEmpty($db['description'], 'The description should not be set by the TripaDefaultIdSpace object just yet.');
    $this->assertEmpty($db['urlprefix'], 'The URL prefix should not be set by the TripaDefaultIdSpace object just yet.');

    // Set the description to make sure it gets set in Chado.
    $GO->setDescription($GO_description);
    $db = $this->getIdSpace($GO_idspace);
    $this->assertTrue($db['name'] == $GO_idspace, 'The name was not set correctly after updating by the TripaDefaultIdSpace object.');
    $this->assertTrue($db['description'] == $GO_description, 'The description was not set correctly by the TripaDefaultIdSpace object.');
    $this->assertEmpty($db['urlprefix'], 'The URL prefix should not be set by the TripaDefaultIdSpace object just yet.');

    // Set the URL prefix to make sure it gets set in Chado.
    $GO->setURLPrefix($GO_urlprefix);
    $db = $this->getIdSpace($GO_idspace);
    $this->assertTrue($db['name'] == $GO_idspace, 'The name was not set correctly after updating by the TripaDefaultIdSpace object.');
    $this->assertTrue($db['description'] == $GO_description, 'The description was not set correctly after updating by the TripaDefaultIdSpace object.');
    $this->assertTrue($db['urlprefix'] == $GO_urlprefix, 'The URL prefix was not set correctly by the TripaDefaultIdSpace object.');

    // Make sure the getters work.
    $this->assertTrue($GO->getURLPrefix() == $GO_urlprefix, "The TripaDefaultIdSpace object did not return a correct URL prefix.");
    $this->assertTrue($GO->getDescription() == $GO_description, "The TripaDefaultIdSpace object did not return a correct description.");

    // Change the description and URL prefix and make sure it updates
    $GO->setDescription('Changed');
    $GO->setURLPrefix('Changed');
    $this->assertTrue($GO->getDescription() == 'Changed', "The TripaDefaultIdSpace object did not update the description.");
    $this->assertTrue($GO->getURLPrefix() == 'Changed', "The TripaDefaultIdSpace object did not update the URL prefix.");

  }
}
