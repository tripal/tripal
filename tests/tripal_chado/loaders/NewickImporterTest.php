<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class NewickImporterTest extends TripalTestCase {

  // Auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Confirm Newick importer functionality for tree with nodes linked to features.
   *
   * @group newick
   */
  public function testNewickImporterFeatureTree() {
    $faker = \Faker\Factory::create();
    $newick_file = ['file_local' => __DIR__ . '/../data/newick_T92076.tree'];
    $analysis = factory('chado.analysis')->create();
    $tree_name = 'newick_test_1';
    $description = $faker->text;
    $tree_term_name = 'DNA';
    $tree_term = chado_get_cvterm(['id' => 'SO:0000352']); // 'DNA'
    $tree_term_id = $tree_term->cvterm_id;

    // We need to create the organisms with the same names as the test
    // newick file, and then a feature for each one that matches the newick file.
    // Do a simplistic parse to extract them, knowing that they start with 'S'.
    $content = preg_replace('/[:\n\r].*$/', '', preg_split('/[\(,]/', file_get_contents($newick_file['file_local'])));
    $organisms = [];
    $features = [];
    foreach ($content as $record) {
      if ($record and preg_match('/^S/', $record)) {
        $organism_parts = explode('_', $record);
        $organism = factory('chado.organism')->create([
          'genus' => $organism_parts[1],
          'species' => $organism_parts[2],
          'type_id' => NULL,
        ]);
        $organism_id = $organism->organism_id;
        $organisms[] = $organism;
        $features[] = factory('chado.feature')->create([
          'type_id' => $tree_term_id,
          'organism_id' => $organism_id,
          'name' => $record,
          'uniquename' => $record,
        ]);
      }
    }

    $run_args = [
      'tree_name' => $tree_name,
      'description' => $description,
      'analysis_id' => $analysis->analysis_id,
      'leaf_type' => $tree_term_name,
      'tree_file' => $newick_file,
      'format' => 'newick',
      'dbxref' => '',
      'match' => 1,  // 0=use name, 1=use uniquename. from 'Use Unique Feature Name" checkbox
      'name_re' => '',
      'load_later' => FALSE,
    ];
    $this->runNewickLoader($run_args, $newick_file);

    // Check that the tree is present in chado.phylotree table
    $query = db_select('chado.phylotree', 't')
      ->fields('t', ['name'])
      ->condition('t.name', $tree_name)
      ->execute()
      ->fetchField();
    $this->assertEquals($tree_name, $query, "A tree with name '$tree_name' was not inserted into the phylotree table");

    // Check that all nodes are in the chado.phylonode table.
    foreach ($features as $feature_id) {
      $name = $feature->uniquename;
      $query = db_select('chado.phylonode', 'n')
        ->fields('n', ['label'])
        ->condition('n.label', $name)
        ->execute()
        ->fetchField();
      $this->assertEquals($name, $label, "A node with name '$name' was not inserted into the phylonode table");
    }
  }

  /**
   * Confirm Newick importer functionality for a 'taxonomy' (species) tree.
   *
   * @group newick
   */
  public function testNewickImporterTaxonomyTree() {
    $faker = \Faker\Factory::create();
    $newick_file = ['file_local' => __DIR__ . '/../data/newick_T92076.tree'];
    $analysis = factory('chado.analysis')->create();
    $tree_name = 'newick_test_2';
    $description = $faker->text;
    $tree_term_name = 'taxonomy';

    // We need to create the organisms with the same names as the test
    // newick file to allow association of nodes to organisms.
    // Do a simplistic parse to extract them, knowing that they start with 'S'.
    $content = preg_replace('/[:\n\r].*$/', '', preg_split('/[\(,]/', file_get_contents($newick_file['file_local'])));
    $organisms = [];
    $features = [];
    foreach ($content as $record) {
      if ($record and preg_match('/^S/', $record)) {
        $organism_parts = explode('_', $record);
        $organism = factory('chado.organism')->create([
          'genus' => $organism_parts[1],
          'species' => $organism_parts[2],
          'type_id' => NULL,
        ]);
        $organism_id = $organism->organism_id;
        $organisms[] = $organism;
      }
    }

    $run_args = [
      'tree_name' => $tree_name,
      'description' => $description,
      'analysis_id' => $analysis->analysis_id,
      'leaf_type' => $tree_term_name,
      'tree_file' => $newick_file,
      'format' => 'newick',
      'dbxref' => '',
      'match' => 0,  // not applicable for taxonomy trees
      'name_re' => '^S\d+_(.*)$',  // Removes S18540_ prefix from test file
      'load_later' => FALSE,
    ];
    $this->runNewickLoader($run_args, $newick_file);

    // Check that the tree is present in chado.phylotree table
    $query = db_select('chado.phylotree', 't')
      ->fields('t', ['name'])
      ->condition('t.name', $tree_name)
      ->execute()
      ->fetchField();
    $this->assertEquals($tree_name, $query, "A tree with name '$tree_name' was not inserted into the phylotree table");

    // Check that all nodes are in the chado.phylonode table.
    foreach ($features as $feature_id) {
      $name = $feature->uniquename;
      $query = db_select('chado.phylonode', 'n')
        ->fields('n', ['label'])
        ->condition('n.label', $name)
        ->execute()
        ->fetchField();
      $this->assertEquals($name, $label, "A node with name '$name' was not inserted into the phylonode table");
    }
  }

  private function runNewickLoader($run_args, $file) {
    module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/NewickImporter');
    $importer = new \NewickImporter();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    ob_start(); //dont display the progress messages
    $importer->run();
    ob_end_clean();
  }
}
