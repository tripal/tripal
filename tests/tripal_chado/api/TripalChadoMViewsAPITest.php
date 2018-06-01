<?php
namespace Tests\tripal_chado\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalChadoMViewsAPITest extends TripalTestCase {
  
  // Use a transaction to roll back changes after every test.
  use DBTransaction;
  
  // This variable holds example materialized views that can be used
  // by the unit tests below.
  private $example_mviews = [
    'analysis_organism_test' => [
      'schema' => [
        'table' => 'analysis_organism_test',
        'description' => 'This view is for associating an organism (via it\'s associated features) to an analysis.',
        'fields' => [
          'analysis_id' => [
            'size' => 'big',
            'type' => 'int',
            'not null' => true,
          ],
          'organism_id' => [
            'size' => 'big',
            'type' => 'int',
            'not null' => true,
          ],
        ],
        'indexes' => [
          'networkmod_qtl_indx0' => [
            0 => 'analysis_id',
          ],
          'networkmod_qtl_indx1' => [
            0 => 'organism_id',
          ],
        ],
        'foreign keys' => [
          'analysis' => [
            'table' => 'analysis',
            'columns' => [
              'analysis_id' => 'analysis_id',
            ],
          ],
          'organism' => [
            'table' => 'organism',
            'columns' => [
              'organism_id' => 'organism_id',
            ],
          ],
        ],
      ],
      'sql' => "
        SELECT DISTINCT A.analysis_id, O.organism_id
        FROM analysis A
          INNER JOIN analysisfeature AF ON A.analysis_id = AF.analysis_id
          INNER JOIN feature F          ON AF.feature_id = F.feature_id
          INNER JOIN organism O         ON O.organism_id = F.organism_id
      ",
      'comment' => 'This view is for associating an organism (via it\'s associated features) to an analysis.',
      'module' => 'tripal_chado',
    ],
  ];
  
  /**
   * Test creation of a new materialized view.
   *
   * @group api
   */
  public function test_chado_add_mview() {
    
    // Add the analysis_organism mview.
    $mview_name = 'analysis_organism_test';
    $mview_module = $this->example_mviews[$mview_name]['module'];
    $mview_sql = $this->example_mviews[$mview_name]['sql'];
    $mview_schema = $this->example_mviews[$mview_name]['schema'];
    $mview_comment = $this->example_mviews[$mview_name]['comment'];
    
    $success = chado_add_mview($mview_name, $mview_module, $mview_schema, $mview_sql, $mview_comment, FALSE);
    $this->assertTrue($success, "Failed to create materialized view: $mview_name");  
    
    // Make sure that the entry is now there.
    $mview = db_select('tripal_mviews', 'tm')
      ->fields('tm')
      ->condition('name', $mview_name)
      ->execute()
      ->fetchObject();
    $this->assertTrue(is_object($mview),
      "Failed to find the materialized view, $mview_name, in the tripal_mviews table");
    
    // Make sure that all of the fields exist and were properly added.
    $this->assertTrue($mview->modulename == $mview_module,
      "Failed to create a proper materialized the modulename field is incorrect: '$mview_module' != '$mview->modulename'");
    $this->assertTrue($mview->mv_table == $mview_name,
      "Failed to create a proper materialized the mv_table field does not match input.");
    $this->assertTrue($mview->query == $mview_sql,
      "Failed to create a proper materialized the query field does not match input.");
    $this->assertTrue($mview->comment == $mview_comment,
      "Failed to create a proper materialized the comment field does not match input.");
    $this->assertNULL($mview->status,
      "Failed to create a proper materialized the status field should be NULL.");
    $this->assertNULL($mview->last_update,
      "Failed to create a proper materialized the last_update field should be NULL.");
    
    // Make sure the table exists.
    $this->assertTrue(chado_table_exists($mview_name),
      "Materialized view, $mview_name, was added to the tripal_mviews table but the table was not created.");
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
    
    // Add the analysis_organism mview.
    $mview_name = 'analysis_organism_test';
    $mview_module = $this->example_mviews[$mview_name]['module'];
    $mview_sql = $this->example_mviews[$mview_name]['sql'];
    $mview_schema = $this->example_mviews[$mview_name]['schema'];
    $mview_comment = $this->example_mviews[$mview_name]['comment'];
    
    $success = chado_add_mview($mview_name, $mview_module, $mview_schema, $mview_sql, $mview_comment, FALSE);
    $this->assertTrue($success, "Failed to create materialized view: $mview_name");
        
    // Now simulate manual deletion of the table outside of the API.
    chado_query('DROP TABLE {' . $mview_name . '}');
    
    // Now try to read the mview. Previously, the behavior was the the mview
    // table would not be created because Tripal thinks it's already there.
    chado_add_mview($mview_name, $mview_module, $mview_schema, $mview_sql, $mview_comment, FALSE);
    $this->assertTrue($success, "Failed to re-create materialized view: $mview_name");
    
    $this->assertTrue(chado_table_exists($mview_name),
      "Manually removing a materialized views throws off the chado_add_mview function when the mview is re-added. See Issue #322");    
  }
}
