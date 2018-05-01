<?php


use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoAPITest extends TripalTestCase {

  use DBTransaction;


  /**
   *
   *@test
   */

  public function test_tripal_chado_publish_records(){

    //create an organism, publish it
    $organism = factory('chado.organism')->create([
      'genus' => 'a_genius_genus',
      'species' => 'fake_species',
    ]);
    //get bundle ID for organism
    $bundle = db_select('public.chado_bundle', 'CB')
    ->fields('CB', ['bundle_id'])
    ->condition('data_table', 'organism')
    ->execute()->fetchField();

    $values = ['bundle_name' => 'bio_data_' . $bundle];

    ob_start();//dont display the job message
    $bool = tripal_chado_publish_records($values);
    ob_end_clean();

    $this->assertTrue($bool, 'Publishing a fake organism record failed');
}

}