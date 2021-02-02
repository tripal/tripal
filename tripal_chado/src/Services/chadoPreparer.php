<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal\Entity\TripalEntityType;

class ChadoPreparer {

  /**
   * The name of the schema we are interested in installing/updating chado for.
   */
  protected $schemaName;

  /**
   * The DRUPAL-managed database connection.
   */
  protected $connection;

  /**
   * The drupal logger for tripal.
   */
  protected $logger;

  /**
   * Holds the Job object
   */
  protected $job = NULL;

  /**
   * Constructor: initialize connections.
   */
  public function __construct() {
    $this->connection = \Drupal::database();

    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');
  }

  /**
   * Set the schema name.
   */
  public function setSchema($schema_name) {
    // Schema name must be all lowercase with no special characters.
    // It should also be a single word.
    if (preg_match('/^[a-z][a-z0-9]+$/', $schema_name) === 0) {
      $this->logger->error('The schema name must be a single word containing only lower case letters or numbers and cannot begin with a number.');
      return FALSE;
    }
    else {
      $this->logger->info('Setting Schema to "' . $schema_name . '".');
      $this->schemaName = $schema_name;
      return TRUE;
    }
}

  /**
   * A setter for the job object if this class is being run using a Tripal job.
   */
  public function setJob(\Drupal\tripal\Services\TripalJob $job) {
    $this->job = $job;
    $this->logger->setJob($job);
    return TRUE;
  }

  /**
   * Retrieve the Drupal connection to the database.
   *
   * @return Drupal\database
   *   Current Drupal connection.
   */
  public function getDrupalConnection() {
    return $this->connection;
  }

  /**
   * Retrieves the message logger.
   *
   * @return object
   *
   */
  public function getLogger() {
    return $this->logger;
  }

  /**
   *
   */
  public function prepare() {
    $this->logger->info("Loading ontologies...");
    $this->loadOntologies();

    $this->logger->info("Creating default content types...");
    $this->contentTypes();
  }

  /**
   * Loads ontologies necessary for creation of default Tripal content types.
   */
  protected function loadOntologies() {
    \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
      'idspace' => 'NCIT',
      'namespace' => 'ncit',
      'name' => 'The NCIt OBO Edition project aims to increase integration of the NCIt with OBO Library ontologies. NCIt is a reference terminology that includes broad coverage of the cancer domain, including cancer related diseases, findings and abnormalities. NCIt OBO Edition releases should be considered experimental.',
      'description' => 'NCI Thesaurus OBO Edition.',
      'url' => 'http://purl.obolibrary.org/obo/ncit.owl',
      'urlprefix' => ' http://purl.obolibrary.org/obo/{db}_{accession}',
    ]);

    \Drupal::service('tripal.tripalTerm.manager')->addTerm([
      'accession' => 'C25693',
      'name' => 'Subgroup',
      'vocabulary' => [
        'name' => 'ncit',
        'idspace' => 'NCIT',
      ],
      'definition' => 'A subdivision of a larger group with members often exhibiting similar characteristics. [ NCI ]',
    ]);

    \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
      'idspace' => 'rdfs',
      'namespace' => 'rdfs',
      'name' => 'Resource Description Framework Schema',
      'description' => 'Resource Description Framework Schema',
      'url' => 'https://www.w3.org/TR/rdf-schema/',
      'urlprefix' => 'http://www.w3.org/2000/01/rdf-schema#{accession}',
    ]);

    \Drupal::service('tripal.tripalTerm.manager')->addTerm([
      'accession' => 'comment',
      'name' => 'comment',
      'vocabulary' => [
        'name' => 'rdfs',
        'idspace' => 'rdfs',
      ],
      'definition' => 'A human-readable description of a resource\'s name.',
    ]);

    // TODO:
    // T3 loads many terms we need for default content types through the OBO
    // Importer. As of Jan 12 2021 the OBO importer has not yet been migrated
    // to T4 and therefore cannot be used. As a result, I will be adding a few
    // terms here individually using the vocab and term managers. In the
    // future, this should be replaced with calls to the OBO importer so that
    // the terms can be imported automatically.

    // accession|name|dbname|cvname|cvdesc|dbdesc|url|urlprefix|definition
    $terms = [
      '0100026|organism|OBI|obi|Ontology for Biomedical Investigation. The Ontology for Biomedical Investigations (OBI) is build in a collaborative, international effort and will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data. This ontology arose from the Functional Genomics Investigation Ontology (FuGO) and will contain both terms that are common to all biomedical investigations, including functional genomics investigations and those that are more domain specific.|The Ontology for Biomedical Investigation.|http://obi-ontology.org/page/Main_Page|http://purl.obolibrary.org/obo/{db}_{accession}|A material entity that is an individual living system, such as animal, plant, bacteria or virus, that is capable of replicating or reproducing, growth and maintenance in the right environment. An organism may be unicellular or made up, like humans, of many billions of cells divided into specialized tissues and organs.',
      '0000704|gene|SO|sequence|The sequence ontology.|The sequence ontology.|http://www.sequenceontology.org/|http://www.sequenceontology.org/browser/current_svn/term/{db}:{accession}',
      '0000044|accession|CO_010|germplasm_ontology|GCP germplasm ontology|Crop Germplasm Ontology|http://www.cropontology.org/get-ontology/CO_010|http://www.cropontology.org/terms/CO_010:{accession}',
      '2945|Analysis|operation|EDAM|EDAM is an ontology of well established, familiar concepts that are prevalent within bioinformatics, including types of data and data identifiers, data formats, operations and topics. EDAM is a simple ontology - essentially a set of terms with synonyms and definitions - organised into an intuitive hierarchy for convenient use by curators, software developers and end-users. EDAM is suitable for large-scale semantic annotations and categorization of diverse bioinformatics resources. EDAM is also suitable for diverse application including for example within workbenches and workflow-management systems, software distributions, and resource registries|A function that processes a set of inputs and results in a set of outputs, or associates arguments (inputs) with values (outputs). Special cases are: a) An operation that consumes no input (has no input arguments).|http://edamontology.org/page|http://edamontology.org/{db}_{accession}|Apply analytical methods to existing data of a specific type.',
    ];

    foreach ($terms as $term) {
      $term = explode('|', $term);
      \Drupal::service('tripal.tripalVocab.manager')->addVocabulary([
        'idspace' => $term[2],
        'namespace' => $term[3],
        'name' => $term[4],
        'description' => $term[5],
        'url' => $term[6],
        'urlprefix' => $term[7],
      ]);

      \Drupal::service('tripal.tripalTerm.manager')->addTerm([
        'accession' => $term[0],
        'name' => $term[1],
        'vocabulary' => [
          'name' => $term[3],
          'idspace' => $term[2],
        ],
        'definition' => $term[8],
      ]);
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
      'id' => 'organism',
      'name' => 'organism',
      'label' => 'Organism',
      'term_id' => $term->getID(),
      'help_text' => 'help',
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
      'name' => 'analysis',
      'label' => 'Analysis',
      'term_id' => $term->getID(),
      'help_text' => 'help',
      'category' => 'General',
    ]);
    $analysis->save();

    // TODO: Create the 'Project' entity type.

    // TODO: Create the 'Study' entity type.

    // TODO: Create the 'Contact' entity type.

    // TODO: Create the 'Publication' entity type.

    // TODO: Create the 'Protocol' entity type.
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
      'id' => 'gene',
      'name' => 'gene',
      'label' => 'Gene',
      'term_id' => $term->getID(),
      'help_text' => 'help',
      'category' => 'Genomic',
    ]);
    $gene->save();

    // TODO: Create the 'mRNA' entity type.

    // TODO: Create the 'Phylogenetic tree' entity type.

    // TODO: Create the 'Physical Map' entity type.

    // TODO: Create the 'DNA Library' entity type.

    // TODO: Create the 'Genome Assembly' entity type.

    // TODO: Create the 'Genome Annotation' entity type.

    // TODO: Create the 'Genome Project' entity type.
  }

  /**
   * Creates the "Genetic" category of content types.
   */
  protected function geneticContentTypes() {
    // TODO: Create the 'Genetic Map' entity type.

    // TODO: Create the 'QTL' entity type.

    // TODO: Create the 'Sequence Variant' entity type.

    // TODO: Create the 'Genetic Marker' entity type.

    // TODO: Create the 'Heritable Phenotypic Marker' entity type.
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
      'id' => 'germplasm_accession',
      'name' => 'germplasm_accession',
      'label' => 'Germplasm Accession',
      'term_id' => $term->getID(),
      'help_text' => 'help',
      'category' => 'Germplasm/Breeding',
    ]);
    $germplasm_accession->save();

    // TODO: Create the 'Breeding Cross' entity type.

    // TODO: Create the 'Germplasm Variety' entity type.

    // TODO: Create the 'Recombinant Inbred Line' entity type.
  }

  /**
   * Creates the "Expression" category of content types.
   */
  protected function expressionContentTypes() {
    // TODO: Create the 'biological sample' entity type.

    // TODO: Create the 'Assay' entity type.

    // TODO: Create the 'Array Design' entity type.
  }
}
