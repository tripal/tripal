<?php
namespace Tests\tripal_chado\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter.inc');


class OBOImporterTest extends TripalTestCase {
  
  use DBTransaction;
    
  /**
   * Tests that the OBO loader can import from a remote OBO.  
   *
   * @group api
   * @group chado
   * @group obo
   * 
   */
  public function testRemoteRemote() {
    
    // Make sure the sequence ontology OBO is there.
    chado_insert_obo('sequence', 'http://purl.obolibrary.org/obo/so.obo');
        
    // The loader is an instance of TripalImporter which
    // requires a job. So let's create one.
    $so = new TripalJob;
    $so->create([
      'job_name' => 'OBO test',
      'modulename' => 'tripal_chado',
      'callback' => 'NA',
      'arguments' => [
        'obo_id' => chado_get_obo('sequence'),
      ],
      'uid' => 1,
      'priority' => 10,
      'includes' => []]);
      
      return [
        $so,
      ];
    
    $loader = new \OBOImporter($job);
    $loader->run();
    
    $check_sql = "SELECT count(*) FROM " 
  }
}
