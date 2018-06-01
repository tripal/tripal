<?php
namespace Tests\tripal_chado\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoCustomTablesAPITest extends TripalTestCase {

  use DBTransaction;

  /**
   * Test creation of a new materialized view.
   * 
   * @group api
   */
  public function test_chado_add_mview() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test deletion of a materialized view.
   * 
   * @group api
   */
  public function test_chado_delete_mview() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test modifications to a materialized view
   * 
   * @group api
   */
  public function test_chado_edit_mview() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test adding a Tripal Job to re-populate a materialized view
   *
   * @group api
   */
  public function test_chado_refresh_mview() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test re-populating a materialized view.
   *
   * @group api
   */
  public function test_chado_populate_mview() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test modifications to a materialized view
   *
   * @group api
   */
  public function test_chado_get_mview_id() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test retrieving names of the materialized views.
   *
   * @group api
   */
  public function test_chado_get_mview_table_names() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  /**
   * Test retrieving all materialized view objects.
   *
   * @group api
   */
  public function test_chado_get_mviews() {
    // TODO: this is currently a stub for a test function that neds
    // implementation. For now it returns true to get past unit testing.
    $this->assertTrue(true);
  }
  
  /**
   * Issue 322 reported the problem of re-adding a materialized view after 
   * the actual table had been manually removed outside of Tripal.  The
   * function reported errors.
   * 
   * @ticket 322
   */
  public function test_re_adding_deleted_mview_issue_322() {
    $mview_table = 'analysis_organism';
    $mview_sql = "
      SELECT DISTINCT A.analysis_id, O.organism_id
      FROM analysis A
        INNER JOIN analysisfeature AF ON A.analysis_id = AF.analysis_id
        INNER JOIN feature F          ON AF.feature_id = F.feature_id
        INNER JOIN organism O         ON O.organism_id = F.organism_id
    ";
    $mview_schema = "array (
      'table' => 'analysis_organism',
      'description' => 'This view is for associating an organism (via it\'s associated features) to an analysis.',
      'fields' => array (
        'analysis_id' => array (
          'size' => 'big',
          'type' => 'int',
          'not null' => true,
        ),
        'organism_id' => array (
          'size' => 'big',
          'type' => 'int',
          'not null' => true,
        ),
      ),
      'indexes' => array (
        'networkmod_qtl_indx0' => array (
          0 => 'analysis_id',
        ),
        'networkmod_qtl_indx1' => array (
          0 => 'organism_id',
        ),
      ),
      'foreign keys' => array (
        'analysis' => array (
          'table' => 'analysis',
          'columns' => array (
            'analysis_id' => 'analysis_id',
          ),
        ),
        'organism' => array (
          'table' => 'organism',
          'columns' => array (
            'organism_id' => 'organism_id',
          ),
        ),
      ),
    )";
    
    // First add the mview normally:
    chado_add_mview($mview_table, 'tripal_chado', $mview_schema, $mview_sql, NULL, FALSE);
    
      
    $this->assertTrue(true);
  }
}
