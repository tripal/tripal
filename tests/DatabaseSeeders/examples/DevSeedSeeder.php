<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;
use \Exception;

class DevSeedSeeder extends Seeder
{
    /**
     * Part one:
     * Chado records.
     * These are records used by many of the below importers.
     * ALL importers require an organism.
     * The expression data loader will associate the data with the
     * $expression_analysis record. All other importers associate data with the
     * $sequence_analysis. Uncomment the array to create that chado record.
     */

protected $organism = [
        'common_name' => 'F. excelsior miniature',
        'genus' => 'Fraxinus',
        'species' => 'excelsior',
        'abbreviation' => 'F. excelsor',
        'comment' => 'Loaded with TripalDev Seed.',
      ];

protected $sequence_analysis = [
      'name' => 'Fraxinus exclesior miniature dataset',
      'description' => 'Tripal Dev Seed',
    ];

    protected $expression_analysis = [

      'name' => 'Fraxinus exclesior miniature dataset Expression Analysis',
      'description' => 'Tripal Dev Seed',
    ];

  protected $blastdb = [
    'name' => 'DevSeed Database: TREMBL',
    'description' => 'A dummy database created by DevSeed',
  ];

    /**
     * Part 2:
     * Files.
     * Each importer will take a file argument.  This argument should be an array
     * with one of the following two keys: file_remote => url where the file is
     * located file_local => server path where the file is located.
     */

    protected $landmark_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/sequences/empty_landmarks.fasta'];

    protected $landmark_type = 'supercontig';

    protected $mRNA_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/sequences/mrna_mini.fasta'];

    protected $protein_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/sequences/polypeptide_mini.fasta'];

    protected $gff_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/gff/filtered.gff'];

    protected $blast_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/gff/filtered.gff'];

    protected $biomaterial_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/biomaterials/biomaterials.xml'];

    protected $expression_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/expression/expression.tsv'];

    protected $interpro_file = ['file_remote' => 'https://raw.githubusercontent.com/statonlab/tripal_dev_seed/master/Fexcel_mini/ips/polypeptide_mini.fasta.xml'];

    // Regular expression that will link the protein name to the mRNA parent feature name.
   // protected $prot_regexp = '/(FRA.*?)(?=:)/';

    protected $prot_regexp = null;

    public function __construct()
    {

        if ($this->organism) {

            try {
                $organism = $this->fetch_chado_record('chado.organism', [
                    'common_name',
                    'organism_id',
                ], $this->organism);
            } catch (\Exception $e) {
                echo $e->getMessage();
                exit;
            }

            $this->organism = $organism;

            if ($this->sequence_analysis) {

                try {
                    $seq_analysis = $this->fetch_chado_record('chado.analysis', ['analysis_id'],
                        $this->sequence_analysis);
                } catch (\Exception $e) {
                    echo $e->getMessage();
                    exit;
                }
                $this->sequence_analysis = $seq_analysis;
            }

            if ($this->expression_analysis) {
                try {
                    $expression_analysis = $this->fetch_chado_record('chado.analysis', ['analysis_id'],
                        $this->expression_analysis);
                } catch (\Exception $e) {
                    echo $e->getMessage();
                    exit;
                }

                $this->expression_analysis = $expression_analysis;
            }
        }

        if ($this->blastdb) {
            try {
                $blastdb = $this->fetch_chado_record('chado.db', ['db_id'], $this->blastdb);
            } catch (\Excetion $e) {
                echo $e->getMessage();
            }

            $this->blastdb = $blastdb;
        }
    }

    /**
     * Runs all loaders.
     * Will only run loaders where the files have been uncommented at the start
     * of the class.
     */
    public function up()
    {

        if ($this->landmark_file) {

            $run_args = [
                'organism_id' => $this->organism->organism_id,
                'analysis_id' => $this->sequence_analysis->analysis_id,
                'seqtype' => $this->landmark_type,
                'method' => 2, //default insert and update
                'match_type' => 1, //unique name default
                //optional
                're_name' => null,
                're_uname' => null,
                're_accession' => null,
                'db_id' => null,
                'rel_type' => null,
                're_subject' => null,
                'parent_type' => null,
            ];
            $this->load_landmarks($run_args, $this->landmark_file);
        }

        if ($this->gff_file) {
            $run_args = [
                'analysis_id' => $this->sequence_analysis->analysis_id,
                'organism_id' => $this->organism->organism_id,
                'use_transaction' => 1,
                'add_only' => 0,
                'update' => 1,
                'create_organism' => 0,
                'create_target' => 0,

                ///regexps for mRNA and protein.
                're_mrna' => null,
                're_protein' => $this->prot_regexp,
                //optional
                'target_organism_id' => null,
                'target_type' => null,
                'start_line' => null,
                'landmark_type' => null,
                'alt_id_attr' => null,
            ];
            $this->load_GFF($run_args, $this->gff_file);
        }

        if ($this->mRNA_file) {

            $run_args = [
                'organism_id' => $this->organism->organism_id,
                'analysis_id' => $this->sequence_analysis->analysis_id,
                'seqtype' => 'mRNA',
                'method' => 2, //default insert and update
                'match_type' => 1, //unique name default
                //optional
                're_name' => null,
                're_uname' => null,
                're_accession' => null,
                'db_id' => null,
                'rel_type' => null,
                're_subject' => null,
                'parent_type' => null,
            ];
            $this->load_mRNA_FASTA($run_args, $this->mRNA_file);
        }

        if ($this->protein_file) {
            $run_args = [
                'organism_id' => $this->organism->organism_id,
                'analysis_id' => $this->sequence_analysis->analysis_id,
                'seqtype' => 'polypeptide',
                'method' => 2,
                'match_type' => 1,
                //optional
                're_name' => null,
                're_uname' => null,
                're_accession' => null,
                'db_id' => null,
            ];

            if ($this->prot_regexp) {
                //links polypeptide to mRNA
                $run_args['rel_type'] = 'derives_from';
                $run_args['re_subject'] = $this->prot_regexp;
                $run_args['parent_type'] = 'mRNA';
            }
            $this->load_polypeptide_FASTA($run_args, $this->protein_file);
        }

        if ($this->interpro_file) {

            $run_args = [
                'analysis_id' => $this->sequence_analysis->analysis_id,
                //optional
                'query_type' => 'mRNA',
                'query_re' => $this->prot_regexp,
                'query_uniquename' => null,
                'parsego' => true,
            ];

            $this->load_interpro_annotations($run_args, $this->interpro_file);
        }

        if ($this->blast_file) {
            $run_args = [
                'analysis_id' => $this->sequence_analysis->analysis_id,
                'no_parsed' => 25,//number results to parse
                'query_type' => 'mRNA',
                //optional
                'blastdb' => $this->blastdb->db_id,
                'blastfile_ext' => null,
                'is_concat' => 0,
                'query_re' => null,
                'query_uniquename' => 0,
            ];

            $this->load_blast_annotations($run_args, $this->blast_file);
        }

        if ($this->biomaterial_file) {
            $run_args = [
                'organism_id' => $this->organism->organism_id,
                'analysis_id' => $this->sequence_analysis->analysis_id,
            ];
            //optional: specifies specific CVterms for properties/property values.  Not used here.
            //'cvterm_configuration' => NULL,
            //'cvalue_configuration' => NULL];

            $this->load_biomaterials($run_args, $this->biomaterial_file);
        }

        if ($this->expression_file) {
            $run_args = [
                'filetype' => 'mat', //matrix file type
                'organism_id' => $this->organism->organism_id,
                'analysis_id' => $this->sequence_analysis->analysis_id,
                //optional
                'fileext' => null,
                're_start' => null,
                're_stop' => null,
                'feature_uniquenames' => null,
                'quantificationunits' => null,
                'seqtype' => 'mRNA',
            ];
            $this->load_expression($run_args, $this->expression_file);
        }
    }

    private function load_landmarks($run_args, $file)
    {
        module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/FASTAImporter');

        $importer = new \FASTAImporter();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_mRNA_FASTA($run_args, $file)
    {
        module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/FASTAImporter');

        $importer = new \FASTAImporter();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_polypeptide_FASTA($run_args, $file)
    {
        module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/FASTAImporter');

        $importer = new \FASTAImporter();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_interpro_annotations($run_args, $file)
    {
        module_load_include('inc', 'tripal_analysis_interpro', 'includes/TripalImporter/InterProImporter');

        $importer = new \InterProImporter();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_GFF($run_args, $file)
    {
        module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/GFF3Importer');

        $importer = new \GFF3Importer();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_blast_annotations($run_args, $file)
    {
        module_load_include('inc', 'tripal_analysis_blast', 'includes/TripalImporter/BlastImporter');

        $importer = new \BlastImporter();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_biomaterials($run_args, $file)
    {
        module_load_include('inc', 'tripal_biomaterial', 'includes/TripalImporter/tripal_biomaterial_loader_v3');

        $importer = new \tripal_biomaterial_loader_v3();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function load_expression($run_args, $file)
    {
        module_load_include('inc', 'tripal_analysis_expression',
            'includes/TripalImporter/tripal_expression_data_loader');

        $importer = new \tripal_expression_data_loader();
        $importer->create($run_args, $file);
        $importer->prepareFiles();
        $importer->run();
    }

    private function fetch_chado_record($table, $fields, $factory_array)
    {
        $query = db_select($table, 't')->fields('t', $fields);

        foreach ($factory_array as $key => $value) {
            $query->condition($key, $value);
        }

        $count_query = $query;
        $count = (int) $count_query->countQuery()->execute()->fetchField();

        if ($count === 0) {
            return factory($table)->create($factory_array);
        }

        if ($count === 1) {
            return $query->execute()->fetchObject();
        }

        throw new Exception("Error creating object for: ".$table.".\n Array supplied matches ".$count_query." results, must match 1.");
    }
}
