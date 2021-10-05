<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\tripal\Entity\TripalEntityType;

class ChadoPreparer extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'preparer';

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one output
   * schema and no input schema as shown:
   * ```
   * ['output_schemas' => ['schema_name'], ]
   * ```
   *
   * @throws \Drupal\tripal_chado\Exception\ParameterException
   *   A descriptive exception is thrown in cas of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      // Check input.
      if (!empty($this->parameters['input_schemas'])) {
        throw new ParameterException(
          "No input schema must be specified."
        );
      }

      // Check output.
      if (empty($this->parameters['output_schemas'])
          || (1 != count($this->parameters['output_schemas']))
      ) {
        throw new ParameterException(
          "Invalid number of output schemas. Only one output schema must be specified."
        );
      }

      $bio_tool = \Drupal::service('tripal_biodb.tool');
      $output_schema = $this->outputSchemas[0];

      // Note: schema names have already been validated through BioConnection.
      // Check if the target schema exists.
      if (!$output_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Output schema "'
          . $output_schema->getSchemaName()
          . '" does not exist.'
        );
      }
    }
    catch (\Exception $e) {
      // Log.
      $this->logger->error($e->getMessage());
      // Rethrow.
      throw $e;
    }
  }

  /**
   * Prepare a given chado schema by inserting minimal data.
   *
   * Task parameter array provided to the class constructor includes:
   * - 'input_schemas' array: no input schema
   * - 'output_schemas' array: one output Chado schema that must exist
   *   (required)
   *
   * Example:
   * ```
   * ['output_schemas' => ['chado_schema'], ]
   * ```
   *
   * @return bool
   *   TRUE if the task was performed with success and FALSE if the task was
   *   completed but without the expected success.
   *
   * @throws Drupal\tripal_chado\Exception\TaskException
   *   Thrown when a major failure prevents the task from being performed.
   *
   * @throws \Drupal\tripal_chado\Exception\ParameterException
   *   Thrown if parameters are incorrect.
   *
   * @throws Drupal\tripal_chado\Exception\LockException
   *   Thrown when the locks can't be acquired.
   */
  public function performTask() :bool {
    // Task return status.
    $task_success = FALSE;

    // Validate parameters.
    $this->validateParameters();

    // Acquire locks.
    $success = $this->acquireTaskLocks();
    if (!$success) {
      throw new LockException("Unable to acquire all locks for task. See logs for details.");
    }

    try
    {
      $chado_schema = $this->outputSchemas[0];

      $this->setProgress(0.1);
      $this->logger->notice("Loading ontologies...");
      $this->loadOntologies();

      $this->setProgress(0.5);
      $this->logger->notice("Creating default content types...");
      $this->contentTypes();

      $this->setProgress(1);
      $task_success = TRUE;

      // Release all locks.
      $this->releaseTaskLocks();

      // Cleanup state API.
      $this->state->delete(static::STATE_KEY_DATA_PREFIX . $this->id);
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
      // Cleanup state API.
      $this->state->delete(static::STATE_KEY_DATA_PREFIX . $this->id);
      // Release all locks.
      $this->releaseTaskLocks();

      throw new TaskException(
        "Failed to complete schema integration task.\n"
        . $e->getMessage()
      );
    }

    return $task_success;
  }

  /**
   * Set progress value.
   *
   * @param float $value
   *   New progress value.
   */
  protected function setProgress(float $value) {
    $data = ['progress' => $value];
    $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);
  }

  /**
   * {@inheritdoc}
   */
  public function getProgress() :float {
    $data = $this->state->get(static::STATE_KEY_DATA_PREFIX . $this->id, []);

    if (empty($data)) {
      // No more data available. Assume process ended.
      $progress = 1;
    }
    else {
      $progress = $data['progress'];
    }
    return $progress;
  }

  /**
   * {@inheritdoc}
   */
  public function getStatus() :string {
    $status = '';
    $progress = $this->getProgress();
    if (1 > $progress) {
      $status = 'Integration in progress.';
    }
    else {
      $status = 'Integration done.';
    }
    return $status;
  }

  /**
   * Loads ontologies necessary for creation of default Tripal content types.
   */
  protected function loadOntologies() {
    \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
      'idspace' => 'NCIT',
      'namespace' => 'ncit',
      'name' => 'NCI Thesaurus OBO Edition',
      'description' => 'The NCIt OBO Edition project aims to increase integration of the NCIt with OBO Library ontologies.',
      'url' => 'http://purl.obolibrary.org/obo/ncit.owl',
      'urlprefix' => ' http://purl.obolibrary.org/obo/{db}_{accession}',
    ]);

    \Drupal::service('tripal.tripalTerm.manager')->addTerm([
      'accession' => 'C25693',
      'name' => 'Subgroup',
      'vocabulary' => [
        'namespace' => 'ncit',
        'idspace' => 'NCIT',
      ],
      'definition' => 'A subdivision of a larger group with members often exhibiting similar characteristics. [ NCI ]',
    ]);

    \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
      'idspace' => 'rdfs',
      'namespace' => 'rdfs',
      'name' => 'Resource Description Framework Schema',
      'description' => 'RDF Schema provides a data-modelling vocabulary for RDF data.',
      'url' => 'https://www.w3.org/TR/rdf-schema/',
      'urlprefix' => 'http://www.w3.org/2000/01/rdf-schema#{accession}',
    ]);

    \Drupal::service('tripal.tripalTerm.manager')->addTerm([
      'accession' => 'comment',
      'name' => 'comment',
      'vocabulary' => [
        'namespace' => 'rdfs',
        'idspace' => 'rdfs',
      ],
      'definition' => 'A human-readable description of a resource\'s name.',
    ]);

    // TODO:
    // T3 loads many terms we need for default content types through the OBO
    // Importer. As of April 19, 2021 the OBO importer has not yet been migrated
    // to T4 and therefore cannot be used. As a result, I will be adding a few
    // terms here individually using the vocab and term managers. In the
    // future, this should be replaced with calls to the OBO importer so that
    // the terms can be imported automatically.
    $terms = [];

    // Organism.
    $terms['SO:0100026'] = [
      'accession' => '0100026',
      'name' => 'organism',
      'definition' => 'A material entity that is an individual living system, such as animal, plant, bacteria or virus, that is capable of replicating or reproducing, growth and maintenance in the right environment. An organism may be unicellular or made up, like humans, of many billions of cells divided into specialized tissues and organs.',
      'vocabulary' => [
        'name' => 'The Ontology for Biomedical Investigation',
        'namespace' => 'obi',
        'idspace' => 'OBI',
        'description' => 'The Ontology for Biomedical Investigations (OBI) will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data.',
        'url' => 'http://obi-ontology.org/page/Main_Page',
        'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
      ],
    ];

    // Gene.
    $terms['SO:0000704'] = [
      'accession' => '0000704',
      'name' => 'gene',
      'definition' => 'A region (or regions) that includes all of the sequence elements necessary to encode a functional transcript. A gene may include regulatory regions, transcribed regions and/or other functional sequence regions.',
      'vocabulary' => [
        'name' => 'The Sequence Ontology',
        'namespace' => 'sequence',
        'idspace' => 'SO',
        'description' => 'The Sequence Ontology (SO) is a collaborative ontology project for the definition of sequence features used in biological sequence annotation.',
        'url' => 'http://www.sequenceontology.org/',
        'urlprefix' => 'http://www.sequenceontology.org/browser/current_svn/term/{db}:{accession}',
      ],
    ];

    // Germplasm Accession.
    $terms['CO_010:0000044'] = [
      'accession' => '0000044',
      'name' => 'accession',
      'definition' => '',
      'vocabulary' => [
        'name' => 'GCP germplasm ontology',
        'namespace' => 'germplasm_ontology',
        'idspace' => 'CO_010',
        'description' => 'Provides desciptors for germplasm collections. Adapted from Descriptors for Banana (Musa spp.) (1996) and Descriptors for Mango by Bioversity.',
        'url' => 'http://www.cropontology.org/ontology/CO_010/Germplasm',
        'urlprefix' => 'http://www.cropontology.org/terms/{db}:{accession}/index.html',
      ],
    ];

    // Analysis.
    $terms['2945'] = [
      'accession' => '2945',
      'name' => 'Analysis',
      'definition' => 'Apply analytical methods to existing data of a specific type.',
      'vocabulary' => [
        'name' => 'EDAM - Ontology of bioscientific data analysis',
        'namespace' => 'EDAM',
        'idspace' => 'operation',
        'description' => 'EDAM is a comprehensive ontology of well-established, familiar concepts that are prevalent within computational biology, bioinformatics, and bioimage informatics.',
        'url' => 'http://edamontology.org/page',
        'urlprefix' => 'http://edamontology.org/{db}_{accession}',
      ],
    ];

    /* Template for adding more.
    $terms[''] = [
      'accession' => '',
      'name' => '',
      'definition' => '',
      'vocabulary' => [
        'name' => '',
        'namespace' => '',
        'idspace' => '',
        'description' => '',
        'url' => '',
        'urlprefix' => '',
      ],
    ]; */

    // Actually add the terms described above.
    // The term manager will create the vocabulary if it doesn't exist.
    foreach ($terms as $term) {
      \Drupal::service('tripal.tripalTerm.manager')->addTerm($term);
    }

    // ###################################################################
    // Begin T3 code to be converted to D8 when the OBO importer is ready.
    // ###################################################################
    /*
      // Insert commonly used ontologies into the tables.
      $ontologies = [
        [
          'name' => 'Relationship Ontology (legacy)',
          'path' => '{tripal_chado}/files/legacy_ro.obo',
          'auto_load' => FALSE,
          'cv_name' => 'ro',
          'db_name' => 'RO',
        ],
        [
          'name' => 'Gene Ontology',
          'path' => 'http://purl.obolibrary.org/obo/go.obo',
          'auto_load' => FALSE,
          'cv_name' => 'cellualar_component',
          'db_name' => 'GO',
        ],
        [
          'name' => 'Taxonomic Rank',
          'path' => 'http://purl.obolibrary.org/obo/taxrank.obo',
          'auto_load' => TRUE,
          'cv_name' => 'taxonomic_rank',
          'db_name' => 'TAXRANK',
        ],
        [
          'name' => 'Tripal Contact',
          'path' => '{tripal_chado}/files/tcontact.obo',
          'auto_load' => TRUE,
          'cv_name' => 'tripal_contact',
          'db_name' => 'TContact',
        ],
        [
          'name' => 'Tripal Publication',
          'path' => '{tripal_chado}/files/tpub.obo',
          'auto_load' => TRUE,
          'cv_name' => 'tripal_pub',
          'db_name' => 'TPUB',
        ],
        [
          'name' => 'Sequence Ontology',
          'path' => 'http://purl.obolibrary.org/obo/so.obo',
          'auto_load' => TRUE,
          'cv_name' => 'sequence',
          'db_name' => 'SO',
        ],
      ];

      module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter');
      for ($i = 0; $i < count($ontologies); $i++) {
        $obo_id = chado_insert_obo($ontologies[$i]['name'], $ontologies[$i]['path']);
        if ($ontologies[$i]['auto_load'] == TRUE) {
          // Only load ontologies that are not already in the cv table.
          $cv = chado_get_cv(['name' => $ontologies[$i]['cv_name']]);
          $db = chado_get_db(['name' => $ontologies[$i]['db_name']]);
          if (!$cv or !$db) {
            print "Loading ontology: " . $ontologies[$i]['name'] . " ($obo_id)...\n";
            $obo_importer = new OBOImporter();
            $obo_importer->create(['obo_id' => $obo_id]);
            $obo_importer->run();
            $obo_importer->postRun();
          }
          else {
            print "Ontology already loaded (skipping): " . $ontologies[$i]['name'] . "...\n";
          }
        }
      }
    */
  }

  /**
   * Creates default content types.
   */
  protected function contentTypes() {
    $this->generalContentTypes();
    $this->genomicContentTypes();
    $this->geneticContentTypes();
    $this->germplasmContentTypes();
    $this->expressionContentTypes();
  }

  /**
   * Creates the "General" category of content types.
   */
  protected function generalContentTypes() {
    // Create the 'Organism' entity type. This uses the obi:organism term.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '0100026',
      'vocabulary' => [
        'namespace' => 'obi',
        'idspace' => 'OBI',
      ],
    ]);

    $organism = TripalEntityType::create([
      'id' => 1,
      'name' => 'bio_data_1',
      'label' => 'Organism',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'General',
    ]);
    $organism->save();

    // Create the 'Analysis' entity type. This uses the EDAM:analysis term.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '2945',
      'vocabulary' => [
        'namespace' => 'EDAM',
        'idspace' => 'operation',
      ],
    ]);

    $analysis = TripalEntityType::create([
      'id' => 2,
      'name' => 'bio_data_2',
      'label' => 'Analysis',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'General',
    ]);
    $analysis->save();

    // TODO: Create the 'Project' entity type. bio_data_3

    // TODO: Create the 'Study' entity type. bio_data_4

    // TODO: Create the 'Contact' entity type. bio_data_5

    // TODO: Create the 'Publication' entity type. bio_data_6

    // TODO: Create the 'Protocol' entity type. bio_data_7
  }

  /**
   * Creates the "Genomic" category of content types.
   */
  protected function genomicContentTypes() {
    // Create the 'Gene' entity type. This uses the sequence:gene term.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '0000704',
      'vocabulary' => [
        'namespace' => 'sequence',
        'idspace' => 'SO',
      ],
    ]);

    $gene = TripalEntityType::create([
      'id' => 8,
      'name' => 'bio_data_8',
      'label' => 'Gene',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'Genomic',
    ]);
    $gene->save();

    // TODO: Create the 'mRNA' entity type. bio_data_9

    // TODO: Create the 'Phylogenetic tree' entity type. bio_data_10

    // TODO: Create the 'Physical Map' entity type. bio_data_11

    // TODO: Create the 'DNA Library' entity type. bio_data_12

    // TODO: Create the 'Genome Assembly' entity type. bio_data_13

    // TODO: Create the 'Genome Annotation' entity type. bio_data_14

    // TODO: Create the 'Genome Project' entity type. bio_data_15
  }

  /**
   * Creates the "Genetic" category of content types.
   */
  protected function geneticContentTypes() {
    // TODO: Create the 'Genetic Map' entity type. bio_data_16

    // TODO: Create the 'QTL' entity type. bio_data_17

    // TODO: Create the 'Sequence Variant' entity type. bio_data_18

    // TODO: Create the 'Genetic Marker' entity type. bio_data_19

    // TODO: Create the 'Heritable Phenotypic Marker' entity type. bio_data_20
  }

  /**
   * Creates the "Germplasm/Breeding" category of content types.
   */
  protected function germplasmContentTypes() {
    // TODO: Create the 'Phenotypic Trait' entity type.

    // Create the 'Germplasm Accession' entity type.
    $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms([
      'accession' => '0000044',
      'vocabulary' => [
        'namespace' => 'germplasm_ontology',
        'idspace' => 'CO_010',
      ],
    ]);

    $germplasm_accession = TripalEntityType::create([
      'id' => 21,
      'name' => 'bio_data_21',
      'label' => 'Germplasm Accession',
      'term_id' => $term->getID(),
      'help_text' => $term->getDefinition(),
      'category' => 'Germplasm/Breeding',
    ]);
    $germplasm_accession->save();

    // TODO: Create the 'Breeding Cross' entity type. bio_data_22

    // TODO: Create the 'Germplasm Variety' entity type. bio_data_23

    // TODO: Create the 'Recombinant Inbred Line' entity type. bio_data_24
  }

  /**
   * Creates the "Expression" category of content types.
   */
  protected function expressionContentTypes() {
    // TODO: Create the 'biological sample' entity type. bio_data_25

    // TODO: Create the 'Assay' entity type. bio_data_26

    // TODO: Create the 'Array Design' entity type. bio_data_27
  }
}
