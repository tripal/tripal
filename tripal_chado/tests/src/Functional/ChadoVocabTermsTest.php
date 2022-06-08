<?php 

namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\Core\Database\Database;
use Drupal\Core\Test\FunctionalTestSetupTrait;


/**
 * Tests for the ChadoCVTerm classes
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoVocabTerms
 */
class ChadoVocabTermsTest extends ChadoTestBrowserBase {
     
    
  /**
   * Tests task.
   * 
   * @Depends Drupal\tripal_chado\Task\ChadoInstallerTest::testPerformTaskInstaller
   *
   */
  public function testIdSpace() {    
    
    
    // Create a temporary schema.
    $biodb = $this->getTestSchema(ChadoTestBrowserBase::INIT_DUMMY);  
           
    // Create instances of the plugin managers for ID Space and Vocabulary.
    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
    $vmanager = \Drupal::service('tripal.collection_plugin_manager.vocabulary');          
    
    // We need to create an instance of the ID space collection using an identifier for
    // our collection.  We'll use the same name as the CV ID space.
    // We'll model this test after the Gene Ontology which has one ID Space (i.e., GO) but
    // three vocabularies.
    $GO = $idsmanager->createCollection("GO", "chado_id_space");    
    $cc = $vmanager->createCollection("cellular_component", "chado_vocabulary");
    $mf = $vmanager->createCollection("molecular_function", "chado_vocabulary");
    $bp = $vmanager->createCollection("biological_process", "chado_vocabulary");    

    // An ID Space needs a default vocabulary.  This will also add the ID Space
    // to the vocabulary (two birds with one stone).
    $GO->setDefaultVocabulary("cellular_component");    
    
    // Now add the other two ID Spaces to the vocabularies.    
    $mf->addIdSpace("GO");
    $bp->addIdSpace("GO"); 
    
    // Make sure the ID space was added to the db table of Chado.
    
    // Make sure teh vocabulary was added to the cv table of Chado.
    
        
  }
}


