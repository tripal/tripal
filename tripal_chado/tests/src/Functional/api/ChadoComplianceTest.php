<?php

namespace Drupal\Tests\tripal_chado\Functional\api;

use Drupal\Core\Database\Database;
use Drupal\tripal_chado\api\ChadoSchema;
use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;

/**
 * Tests the current Chado Database is compliant with the schema definition
 * used by Tripal.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Database
 */
class ChadoComplianceTest extends ChadoTestBrowserBase {

  protected $defaultTheme = 'stark';

  protected $connection;

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Open connection to Chado
    $this->connection = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
  }

  /**
   * Tests Compliance for a given table.
   *
   * The following is tested:
   *   1. The table exists in the correct schema.
   *   2. It has all the fields we expect.
   *   3. Each field is the type we expect.
   *   4. It has all the constraints we expect.
   *   5. Each constraint consists of the columns we expect.
   *
   * @group api
   * @group chado
   * @group chado-compliance
   */
  public function testTableCompliance() {

    // FOR EVERY CHADO TABLE!
    $schema_version = 1.3;
    $tables = ['acquisition', 'acquisition_relationship', 'acquisitionprop',
    'analysis', 'analysis_cvterm', 'analysis_dbxref', 'analysis_pub',
    'analysis_relationship', 'analysisfeature', 'analysisfeatureprop',
    'analysisprop', 'arraydesign', 'arraydesignprop', 'assay',
    'assay_biomaterial', 'assay_project', 'assayprop', 'biomaterial',
    'biomaterial_dbxref', 'biomaterial_relationship', 'biomaterial_treatment',
    'biomaterialprop', 'cell_line', 'cell_line_cvterm', 'cell_line_cvtermprop',
    'cell_line_dbxref', 'cell_line_feature', 'cell_line_library', 'cell_line_pub',
    'cell_line_relationship', 'cell_line_synonym', 'cell_lineprop',
    'cell_lineprop_pub', 'chadoprop', 'channel', 'contact',
    'contact_relationship', 'contactprop', 'control', 'cv', 'cvprop', 'cvterm',
    'cvterm_dbxref', 'cvterm_relationship', 'cvtermpath', 'cvtermprop',
    'cvtermsynonym', 'db', 'dbprop', 'dbxref', 'dbxrefprop', 'eimage',
    'element', 'element_relationship', 'elementresult',
    'elementresult_relationship', 'environment', 'environment_cvterm',
    'expression', 'expression_cvterm', 'expression_cvtermprop',
    'expression_image', 'expression_pub', 'expressionprop', 'feature',
    'feature_contact', 'feature_cvterm', 'feature_cvterm_dbxref',
    'feature_cvterm_pub', 'feature_cvtermprop', 'feature_dbxref',
    'feature_expression', 'feature_expressionprop', 'feature_genotype',
    'feature_phenotype', 'feature_pub', 'feature_pubprop', 'feature_relationship',
    'feature_relationship_pub', 'feature_relationshipprop',
    'feature_relationshipprop_pub', 'feature_synonym', 'featureloc',
    'featureloc_pub', 'featuremap', 'featuremap_contact', 'featuremap_dbxref',
    'featuremap_organism', 'featuremap_pub', 'featuremapprop', 'featurepos',
    'featureposprop', 'featureprop', 'featureprop_pub', 'featurerange', 'genotype',
    'genotypeprop', 'library', 'library_contact', 'library_cvterm', 'library_dbxref',
    'library_expression', 'library_expressionprop', 'library_feature',
    'library_featureprop', 'library_pub', 'library_relationship',
    'library_relationship_pub', 'library_synonym', 'libraryprop', 'libraryprop_pub',
    'magedocumentation', 'mageml', 'nd_experiment',
    'nd_experiment_analysis', 'nd_experiment_contact', 'nd_experiment_dbxref',
    'nd_experiment_genotype', 'nd_experiment_phenotype', 'nd_experiment_project',
    'nd_experiment_protocol', 'nd_experiment_pub', 'nd_experiment_stock',
    'nd_experiment_stock_dbxref', 'nd_experiment_stockprop', 'nd_experimentprop',
    'nd_geolocation', 'nd_geolocationprop', 'nd_protocol', 'nd_protocol_reagent',
    'nd_protocolprop', 'nd_reagent', 'nd_reagent_relationship', 'nd_reagentprop',
    'organism', 'organism_cvterm', 'organism_cvtermprop', 'organism_dbxref',
    'organism_pub', 'organism_relationship', 'organismprop', 'organismprop_pub',
    'phendesc', 'phenotype', 'phenotype_comparison', 'phenotype_comparison_cvterm',
    'phenotype_cvterm', 'phenotypeprop', 'phenstatement', 'phylonode',
    'phylonode_dbxref', 'phylonode_organism', 'phylonode_pub', 'phylonode_relationship',
    'phylonodeprop', 'phylotree', 'phylotree_pub', 'phylotreeprop', 'project',
    'project_analysis', 'project_contact', 'project_dbxref', 'project_feature',
    'project_pub', 'project_relationship', 'project_stock', 'projectprop',
    'protocol', 'protocolparam', 'pub', 'pub_dbxref', 'pub_relationship',
    'pubauthor', 'pubauthor_contact', 'pubprop', 'quantification',
    'quantification_relationship', 'quantificationprop', 'stock', 'stock_cvterm',
    'stock_cvtermprop', 'stock_dbxref', 'stock_dbxrefprop', 'stock_feature',
    'stock_featuremap', 'stock_genotype', 'stock_library', 'stock_pub',
    'stock_relationship', 'stock_relationship_cvterm', 'stock_relationship_pub',
    'stockcollection', 'stockcollection_db', 'stockcollection_stock',
    'stockcollectionprop', 'stockprop', 'stockprop_pub', 'study', 'study_assay',
    'studydesign', 'studydesignprop', 'studyfactor', 'studyfactorvalue',
    'studyprop', 'studyprop_feature', 'synonym', 'tableinfo', 'treatment'];

    foreach ($tables as $table_name) {

      // Create the ChadoSchema class to aid in testing.
      $chado_schema = new \Drupal\tripal_chado\api\ChadoSchema($schema_version, $this->testSchemaName);
      $version = $chado_schema->getVersion();
      $schema_name = $chado_schema->getSchemaName();

      // Check #1: The table exists in the correct schema.
      $this->assertTrue(
        $chado_schema->checkTableExists($table_name),
        t('":table_name" should exist in the ":chado" schema v:version.',
          [
            ':table_name' => $table_name,
            ':chado' => $schema_name,
            ':version' => $version,
          ])
      );

      // Retrieve the schema for this table.
      $table_schema = $chado_schema->getTableSchema($table_name);

      // For each column in this table...
      foreach ($table_schema['fields'] as $column_name => $column_details) {

        // Check #2: The given field exists in the table.
        $this->assertTrue(
          $chado_schema->checkColumnExists($table_name, $column_name),
          t('The column ":column" must exist in ":table" for chado v:version.',
            [
              ':column' => $column_name,
              ':table' => $table_name,
              ':version' => $version,
            ])
        );

        // Check #3: The field is the type we expect.
        $this->assertTrue(
          $chado_schema->checkColumnType($table_name, $column_name, $column_details['type']),
          t('The column ":table.:column" must be of type ":type" for chado v:version.',
            [
              ':column' => $column_name,
              ':table' => $table_name,
              ':version' => $version,
              ':type' => $column_details['type'],
            ])
        );
      }

      // There are three types of constraints:
      // primary key, unique keys, and foreign keys.
      //.......................................

      // For the primary key:
      // Check #4: The constraint exists.
      if (isset($table_schema['primary key'][0]) AND !empty($table_schema['primary key'][0])) {
        $pkey_column = $table_schema['primary key'][0];
        $this->assertTrue(
          $chado_schema->checkPrimaryKey($table_name, $pkey_column),
          t('The column ":table.:column" must be a primary key with an associated sequence and constraint for chado v:version.',
            [
              ':column' => $pkey_column,
              ':table' => $table_name,
              ':version' => $version,
            ])
        );
      }

      // For each unique key:
      if (isset($table_schema['unique keys'])) {
        foreach ($table_schema['unique keys'] as $constraint_name => $columns) {
          // @debug print "Check '$constraint_name' for '$table_name': ".implode(', ', $columns).".\n";

          // Check #4: The constraint exists.
          $this->assertTrue(
            $chado_schema->checkConstraintExists($table_name, $constraint_name, 'UNIQUE'),
            t('The unique constraint ":name" for ":table" must exist for chado v:version.',
              [
                ':name' => $constraint_name,
                ':table' => $table_name,
                ':version' => $version,
              ])
          );

          // Check #5: The constraint consists of the columns we expect.
          // @todo
        }
      }

      // For each foreign key:
      if (isset($table_schema['foreign keys'])) {
        foreach ($table_schema['foreign keys'] as $fk_table => $details) {
          foreach ($details['columns'] as $base_column => $fk_column) {
            // @debug print "Check '$table_name.$base_column =>  $fk_table.$fk_column ' foreign key.";

            // Check #4: The constraint exists.
            $constraint_name = $table_name . '_' . $base_column . '_fkey';
            $this->assertTrue(
              $chado_schema->checkFKConstraintExists($table_name, $base_column),
              t('The foreign key constraint ":name" for ":table.:column" => ":fktable.:fkcolumn" must exist for chado v:version.',
                [
                  ':name' => $constraint_name,
                  ':table' => $table_name,
                  ':column' => $base_column,
                  ':fktable' => $fk_table,
                  ':fkcolumn' => $fk_column,
                  ':version' => $version,
                ])
            );

            // Check #5: The constraint consists of the columns we expect.
            // @todo
          }
        }
      }
    }
  }
}
