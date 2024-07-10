<?php
namespace Drupal\Tests\tripal_chado\Functional;

use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal_chado\Functional\MockClass\FieldConfigMock;

// FROM OLD CODE:
use Drupal\Core\Url;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;
use NewickImporter;

/**
 * Tests for the NewickImporter class
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado ChadoStorage
 */
class NewickImporterTest extends ChadoTestBrowserBase
{

  /**
   * Confirm basic GFF importer functionality.
   *
   * @group gff
   */
  public function testNewickImporterSimpleTest()
  {
    // Public schema connection
    $public = \Drupal::database();

    // Installs up the chado with the test chado data
    $chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);

    // Keep track of the schema name in case we need it
    $schema_name = $chado->getSchemaName();

    // Test to ensure cvterms are found in the cvterms table
    $cvterms_count_query = $chado->query("SELECT count(*) as c1 FROM {1:cvterm}");
    $cvterms_count_object = $cvterms_count_query->fetchObject();
    $this->assertNotEquals($cvterms_count_object->c1, 0);

    // Insert organism
    $organism_id = $chado->insert('1:organism')
      ->fields([
        'genus' => 'Citrus',
        'species' => 'sinensis',
        'common_name' => 'Sweet Orange',
      ])
      ->execute();

    // Insert Analysis
    $analysis_id = $chado->insert('1:analysis')
      ->fields([
        'name' => 'Test Analysis',
        'description' => 'Test Analysis',
        'program' => 'PROGRAM',
        'programversion' => '1.0',
      ])
      ->execute();

    // Check to make sure polypeptide is in cvterms table
    $results = $chado->query('SELECT COUNT(*) as c1 FROM {1:cvterm} WHERE name = :name', [
      'name' => 'polypeptide'
    ]);
    $count = 0;
    foreach ($results as $row) {
      $count = $row->c1;
    }
    $this->assertEquals($count, 1, "Polypeptide was not found in cvterms");

    // Verify that polypeptide is now in the cvterm table
    $result_polypeptide_cvterm = $chado->query("SELECT * FROM {1:cvterm}
      WHERE name = 'polypeptide' LIMIT 1;");
    $polypeptide_object = null;
    $polypeptide_object = $result_polypeptide_cvterm->fetchObject();
    $this->assertNotEquals($polypeptide_object, null);

    $polypeptide_cvterm_id = $polypeptide_object->cvterm_id;


    // Perform the Newick test by creating an instance of the Newick loader
    $importer_manager = \Drupal::service('tripal.importer');
    $newick_importer = $importer_manager->createInstance('chado_newick_tree_loader');

    // This tree file takes 13 minutes so replaced it with Douglas' tree file
    // $run_args = [
    //   'files' => [
    //     0 => [
    //       'file_path' => __DIR__ . '/../../../fixtures/newick_loader/mrna_mini.fasta.tree'
    //     ]
    //   ],
    //   'analysis_id' => $analysis_id,
    //   'schema_name' => $schema_name,
    //   'tree_name' => 'Tree 2',
    //   'leaf_type' => 'polypeptide (SO:0000104)',
    //   'dbxref' => NULL,
    //   'description' => 'No description',
    //   'name_re' => NULL,
    //   'match' => 0,
    //   'load_later' => 0
    // ];

    // $file_details = [
    //   'file_local' => __DIR__ . '/../../../fixtures/fasta_loader/mrna_mini.fasta.tree',
    // ];


    // Create feature S18540_Klotzschia_glaziovii
    $values = [
      'organism_id' => $organism_id,
      'name' => 'S18540_Klotzschia_glaziovii',
      'uniquename' => 'S18540_Klotzschia_glaziovii',
      'type_id' => $polypeptide_cvterm_id,
      'dbxref_id' => 1,
      'seqlen' => 0,
    ];
    $chado->insert('1:feature')->fields($values)->execute();

    // Check to see if the feature now exists
    $results = $chado->query('SELECT * FROM {1:feature} WHERE name = :name', [
      'name' => 'S18540_Klotzschia_glaziovii'
    ]);
    $feature_object = $results->fetchObject();
    $this->assertEquals('S18540_Klotzschia_glaziovii', $feature_object->name, 'Feature could not be found');


    $run_args = [
      'files' => [
        0 => [
          'file_path' => __DIR__ . '/../../../fixtures/newick_loader/newick_T92076.tree'
        ]
      ],
      'analysis_id' => $analysis_id,
      'schema_name' => $schema_name,
      'tree_name' => 'Tree 2',
      'leaf_type' => 'polypeptide (SO:0000104)',
      'dbxref' => NULL,
      'description' => 'No description',
      'name_re' => "",
      'match' => 0,
      // 'load_later' => 0
    ];

    $file_details = [
      'file_local' => __DIR__ . '/../../../fixtures/newick_loader/newick_T92076.tree',
    ];

    $newick_importer->createImportJob($run_args, $file_details);
    $newick_importer->prepareFiles();
    $newick_importer->run();
    $newick_importer->postRun();

    // Check for phylotree based on name
    $results = $chado->query('SELECT COUNT(*) as c1 FROM {1:phylotree} WHERE name = :name', [
      ':name' => 'Tree 2'
    ]);
    $count = 0;
    foreach ($results as $row) {
      $count = $row->c1;
    }
    $this->assertGreaterThan(0, $count, "Should have created at least one phylotree but did not.");


    // We need to get the type id - polypeptide
    $results = $chado->query('SELECT * FROM {1:cvterm} WHERE name = :name', [
      'name' => 'polypeptide'
    ]);
    $type_id = NULL;
    foreach ($results as $row) {
      $type_id = $row->cvterm_id;
    }


    // Check for phylotree based on name and also type_id which should be there as well
    $results = $chado->query('SELECT COUNT(*) as c1 FROM {1:phylotree} WHERE name = :name and type_id = :type_id', [
      ':name' => 'Tree 2',
      ':type_id' => $type_id
    ]);
    $count = 0;
    foreach ($results as $row) {
      $count = $row->c1;
    }
    $this->assertGreaterThan(0, $count, "Should have created at least one phylotree with associated type_id but did not.");

    // Check phylonode table - count should be 23 for this specific tree using this test
    $results = $chado->query('SELECT COUNT(*) as c1 FROM {1:phylonode}');
    $count = 0;
    foreach ($results as $row) {
      $count = $row->c1;
    }
    // print_r($row->c1);
    $this->assertEquals($count, 23, "Should have created 23 phylonode records but didn't.");

    // Check to see if there's a phylonode with a feature_id attached to it since we added S18540_Klotzschia_glaziovii
    // as a feature
    $results = $chado->query('SELECT * FROM {1:phylonode} WHERE label = :label AND feature_id IS NOT NULL', [
      ':label' => 'S18540_Klotzschia_glaziovii'
    ]);
    $count = 0;
    foreach ($results as $row) {
      $count++;
      //$count = $row->c1;
    }
    // print_r($row->c1);
    $this->assertGreaterThan(0, $count, "Should have created at least one phylonode record with a feature not being null.");

    // Check to see if there's a phylonode without connected features, this happens if the feature names do not exist
    // in the features table. There should be 22 out of 23 phylonodes like this
    $results = $chado->query('SELECT COUNT(*) as c1 FROM {1:phylonode} WHERE feature_id IS NULL');
    $count = 0;
    foreach ($results as $row) {
      $count = $row->c1;
    }
    // print_r($row->c1);
    $this->assertGreaterThan(20, $count, "Should have created at least 20 phylonode records with a features being null.");


    // Let's check to see if one phylotree exists
    $results = $chado->query('SELECT COUNT(*) as c1 FROM {1:phylotree}');
    $count = 0;
    foreach ($results as $row) {
      $count = $row->c1;
    }
    $this->assertEquals(1, $count, "There should be one phylotree created");

    // Get a phylotree_id for further testing
    $results = $chado->query('SELECT phylotree_id FROM {1:phylotree} LIMIT 1');
    $phylotree_id = NULL;
    foreach ($results as $row) {
      $phylotree_id = $row->phylotree_id;
    }

    // Let's try to cover more code for phylotree.api.php by testing chado_update_phylotree
    $options = [
      'job' => NULL,
      'name' => 'New name',
      'analysis_id' => 1,
      'dbxref_id' => 'null:local:null',
      'comment' => 'test comment',
    ];
    chado_update_phylotree($phylotree_id, $options, $schema_name);


    // Let's try to cover more code for phylotree.api.php
    // This will DELETE ALL TEST PHYLOTREES SO DO THIS LAST
    // TO AVOID OTHER TESTS FAILING
    $results = $chado->query('SELECT phylotree_id FROM {1:phylotree}');
    foreach ($results as $row) {
      $phylotree_id = $row->phylotree_id;
      chado_delete_phylotree($phylotree_id, $schema_name);
    }


    // Initialize a drupal user with the following permissions
    $account = $this->drupalCreateUser([
      // 'administer rules',
      'access administration pages',
      'administer tripal',
      'administer users',
      'administer permissions',
      'access tripal content overview',
      'allow tripal import',
      'administer tripal content',
      'admin tripal files',
      'add tripal content entities',
      'manage tripal jobs',
      'use chado_newick_tree_loader importer',
      'view tripal content entities',
      'upload files'
    ]);
    // Login the drupal user
    $this->drupalLogin($account);

    // Check if the tripal loaders page is loadable / viewable
    $this->drupalGet('admin/tripal/loaders');
    $this->assertSession()->statusCodeEquals(200);

    // Check if the newick tree loader form is viewable
    $this->drupalGet('admin/tripal/loaders/chado_newick_tree_loader');
    $this->assertSession()->statusCodeEquals(200);

    // $this->drupalGet('admin/config/workflow/rules');
    // $this->assertSession()->statusCodeEquals(200);

    // Test that there is an empty reaction rule listing.
    // $this->assertSession()->pageTextContains('There is no Reaction Rule yet.');
  }

}