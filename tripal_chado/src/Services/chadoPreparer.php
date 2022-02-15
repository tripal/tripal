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
    /* OLD METHOD -NEEDS UPGRADE
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
    ]; -/

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
   * Helper: Create a given set of types.
   *
   * @param $types
   *   An array of types to be created. The key is used to link the type to
   *   it's term and the value is an array of details to be passed to the
   *   create() method for Tripal Entity Types. Some keys should be:
   *    - id: the integer id for this type; this will go away since it
   *        should be automatic.
   *    - name: the machine name for this type. It should be bio_data_[id]
   *        where [id] matches the integer id above. Also should be automatic.
   *    - label: a human-readable label for the content type.
   *    - category: a grouping string to categorize content types in the UI.
   *    - help_text: a single sentence describing the content type -usually
   *        the default is the term definition.
   * @param $terms
   *   An array of terms which must already exist where the key maps to a
   *   content type in the $types array. The value for each item is an array of
   *   details to be passed to the term creation API. Some keys should be:
   *    - accession: the unique identifier for the term (i.e. 2945)
   *    - vocabulary:
   *       - namespace: The name of the vocabulary (i.e. EDAM).
   *       - idspace: the id space of the term (i.e. operation).
   */
  protected function createGivenContentTypes($types, $terms) {
    foreach($terms as $key => $term_details) {
      $type_details = $types[$key];

      $this->logger->info("\n  -- Creating " . $type_details['label'] . " (" . $type_details['name'] . ")...");

      // TODO: Create the term once the API is upgraded.
      // $term = \Drupal::service('tripal.tripalTerm.manager')->getTerms($term_details);

      // TODO: Set the term in the type details.
      if (is_object($term)) {
        // $type_details['term_id'] = $term->getID();
        if (!array_key_exists($type_details, 'help_text')) {
          // $type_details['help_text'] = $term->getDefinition();
        }
      }
      else {
        $this->logger->info("\tNo term attached -waiting on API update.");
      }

      // Check if the type already exists.
      // TODO: use term instead of label once it's available.
      $filter = ['label' => $type_details['label'] ];
      $exists = \Drupal::entityTypeManager()
        ->getStorage('tripal_entity_type')
        ->loadByProperties($filter);

      // Create the Type.
      if (empty($exists)) {
        $tripal_type = TripalEntityType::create($type_details);
        if (is_object($tripal_type)) {
          $tripal_type->save();
          $this->logger->info("\tSaved successfully.");
        }
        else {
          $this->logger->error("\tCreation Failed! Details provided were: " . print_r($type_details));
        }
      }
      else {
        $this->logger->info("\tSkipping as the content type already exists.");
      }
    }
  }

  /**
   * Creates the "General" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'General',
   ];
   * @endcode
   */
  protected function generalContentTypes() {

    // The 'Organism' entity type. This uses the obi:organism term.
    $terms['organism'] =[
      'accession' => '0100026',
      'vocabulary' => [
        'idspace' => 'OBI',
      ],
    ];
    $types['organism']= [
      'id' => 1,
      'name' => 'bio_data_1',
      'label' => 'Organism',
      'category' => 'General',
    ];

    // The 'Analysis' entity type. This uses the EDAM:analysis term.
    $terms['analysis'] = [
      'accession' => '2945',
      'vocabulary' => [
        'namespace' => 'EDAM',
        'idspace' => 'operation',
      ],
    ];
    $types['analysis'] = [
      'id' => 2,
      'name' => 'bio_data_2',
      'label' => 'Analysis',
      'category' => 'General',
    ];

    // The 'Project' entity type. bio_data_3
    $terms['project'] =[
      'accession' => 'C47885',
      'vocabulary' => [
        'idspace' => 'NCIT',
      ],
    ];
    $types['project']= [
      'id' => 3,
      'name' => 'bio_data_3',
      'label' => 'Project',
      'category' => 'General',
    ];

    // The 'Study' entity type. bio_data_4
    $terms['study'] =[
      'accession' => '001066',
      'vocabulary' => [
        'idspace' => 'SIO',
      ],
    ];
    $types['study']= [
      'id' => 4,
      'name' => 'bio_data_4',
      'label' => 'Study',
      'category' => 'General',
    ];

    // The 'Contact' entity type. bio_data_5
    $terms['contact'] =[
      'accession' => 'contact',
      'vocabulary' => [
        'idspace' => 'local',
      ],
    ];
    $types['contact']= [
      'id' => 5,
      'name' => 'bio_data_5',
      'label' => 'Contact',
      'category' => 'General',
    ];

    // The 'Publication' entity type. bio_data_6
    $terms['publication'] =[
      'accession' => '0000002',
      'vocabulary' => [
        'idspace' => 'TPUB',
      ],
    ];
    $types['publication']= [
      'id' => 6,
      'name' => 'bio_data_6',
      'label' => 'Publication',
      'category' => 'General',
    ];

    // The 'Protocol' entity type. bio_data_7
    $terms['protocol'] =[
      'accession' => '00101',
      'vocabulary' => [
        'idspace' => 'sep',
      ],
    ];
    $types['protocol']= [
      'id' => 7,
      'name' => 'bio_data_7',
      'label' => 'Protocol',
      'category' => 'General',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Genomic" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Genomic',
   ];
   * @endcode
   */
  protected function genomicContentTypes() {

    // The 'Gene' entity type. This uses the sequence:gene term.
    $terms['gene'] = [
      'accession' => '0000704',
      'vocabulary' => [
        'namespace' => 'sequence',
        'idspace' => 'SO',
      ],
    ];
    $types['gene'] = [
      'id' => 8,
      'name' => 'bio_data_8',
      'label' => 'Gene',
      'category' => 'Genomic',
    ];

    // the 'mRNA' entity type. bio_data_9
    $terms['mRNA'] =[
      'accession' => '0000234',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['mRNA']= [
      'id' => 9,
      'name' => 'bio_data_9',
      'label' => 'mRNA',
      'category' => 'Genomic',
    ];

    // The 'Phylogenetic tree' entity type. bio_data_10
    $terms['phylo'] =[
      'accession' => '0872',
      'vocabulary' => [
        'idspace' => 'data',
      ],
    ];
    $types['phylo']= [
      'id' => 10,
      'name' => 'bio_data_10',
      'label' => 'Phylogenetic Tree',
      'category' => 'Genomic',
    ];

    // The 'Physical Map' entity type. bio_data_11
    $terms['map'] =[
      'accession' => '1280',
      'vocabulary' => [
        'idspace' => 'data',
      ],
    ];
    $types['map']= [
      'id' => 11,
      'name' => 'bio_data_11',
      'label' => 'Physical Map',
      'category' => 'Genomic',
    ];

    // The 'DNA Library' entity type. bio_data_12
    $terms['library'] =[
      'accession' => 'C16223',
      'vocabulary' => [
        'idspace' => 'NCIT',
      ],
    ];
    $types['library']= [
      'id' => 12,
      'name' => 'bio_data_12',
      'label' => 'DNA Library',
      'category' => 'Genomic',
    ];

    // The 'Genome Assembly' entity type. bio_data_13
    $terms['assembly'] =[
      'accession' => '0525',
      'vocabulary' => [
        'idspace' => 'operation',
      ],
    ];
    $types['assembly']= [
      'id' => 13,
      'name' => 'bio_data_13',
      'label' => 'Genome Assembly',
      'category' => 'Genomic',
    ];

    // The 'Genome Annotation' entity type. bio_data_14
    $terms['annotation'] =[
      'accession' => '0362',
      'vocabulary' => [
        'idspace' => 'operation',
      ],
    ];
    $types['annotation']= [
      'id' => 14,
      'name' => 'bio_data_14',
      'label' => 'Genome Assembly',
      'category' => 'Genomic',
    ];

    // The 'Genome Project' entity type. bio_data_15
    $terms['genomeproject'] =[
      'accession' => 'Genome Project',
      'vocabulary' => [
        'idspace' => 'local',
      ],
    ];
    $types['genomeproject']= [
      'id' => 15,
      'name' => 'bio_data_15',
      'label' => 'Genome Project',
      'category' => 'Genomic',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Genetic" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Genetic',
   ];
   * @endcode
   */
  protected function geneticContentTypes() {

    // The 'Genetic Map' entity type. bio_data_16
    $terms['map'] =[
      'accession' => '1278',
      'vocabulary' => [
        'idspace' => 'data',
      ],
    ];
    $types['map']= [
      'id' => 16,
      'name' => 'bio_data_16',
      'label' => 'Genetic Map',
      'category' => 'Genetic',
    ];

    // The 'QTL' entity type. bio_data_17
    $terms['qtl'] =[
      'accession' => '0000771',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['qtl']= [
      'id' => 17,
      'name' => 'bio_data_17',
      'label' => 'QTL',
      'category' => 'Genetic',
    ];

    // The 'Sequence Variant' entity type. bio_data_18
    $terms['variant'] =[
      'accession' => '0001060',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['variant']= [
      'id' => 18,
      'name' => 'bio_data_18',
      'label' => 'Sequence Variant',
      'category' => 'Genetic',
    ];

    // The 'Genetic Marker' entity type. bio_data_19
    $terms['marker'] =[
      'accession' => '0001645',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['marker']= [
      'id' => 19,
      'name' => 'bio_data_19',
      'label' => 'Genetic Marker',
      'category' => 'Genetic',
    ];

    // The 'Heritable Phenotypic Marker' entity type. bio_data_20
    $terms['hpn'] =[
      'accession' => '0001500',
      'vocabulary' => [
        'idspace' => 'SO',
      ],
    ];
    $types['hpn']= [
      'id' => 20,
      'name' => 'bio_data_20',
      'label' => 'Heritable Phenotypic Marker',
      'category' => 'Genetic',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Germplasm/Breeding" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Germplasm',
   ];
   * @endcode
   */
  protected function germplasmContentTypes() {

    // The 'Phenotypic Trait' entity type. bio_data_28
    $terms['trait'] =[
      'accession' => 'C85496',
      'vocabulary' => [
        'idspace' => 'NCIT',
      ],
    ];
    $types['trait']= [
      'id' => 28,
      'name' => 'bio_data_28',
      'label' => 'Phenotypic Trait',
      'category' => 'Germplasm',
    ];

    // The 'Germplasm Accession' entity type. bio_data_21
    $terms['accession'] = [
      'accession' => '0000044',
      'vocabulary' => [
        'namespace' => 'germplasm_ontology',
        'idspace' => 'CO_010',
      ],
    ];
    $types['accession'] = [
      'id' => 21,
      'name' => 'bio_data_21',
      'label' => 'Germplasm Accession',
      'category' => 'Germplasm/Breeding',
    ];

    // The 'Breeding Cross' entity type. bio_data_22
    $terms['cross'] =[
      'accession' => '0000255',
      'vocabulary' => [
        'idspace' => 'CO_010',
      ],
    ];
    $types['cross']= [
      'id' => 22,
      'name' => 'bio_data_22',
      'label' => 'Breeding Cross',
      'category' => 'Germplasm',
    ];

    // The 'Germplasm Variety' entity type. bio_data_23
    $terms['variety'] =[
      'accession' => '0000029',
      'vocabulary' => [
        'idspace' => 'CO_010',
      ],
    ];
    $types['variety']= [
      'id' => 23,
      'name' => 'bio_data_23',
      'label' => 'Germplasm Variety',
      'category' => 'Germplasm',
    ];

    // The 'Recombinant Inbred Line' entity type. bio_data_24
    $terms['ril'] =[
      'accession' => '0000162',
      'vocabulary' => [
        'idspace' => 'CO_010',
      ],
    ];
    $types['ril']= [
      'id' => 24,
      'name' => 'bio_data_24',
      'label' => 'Recombinant Inbred Line',
      'category' => 'Germplasm',
    ];

    $this->createGivenContentTypes($types, $terms);
  }

  /**
   * Creates the "Expression" category of content types.
   *
   * @code
   $terms[''] =[
     'accession' => '',
     'vocabulary' => [
       'idspace' => '',
     ],
   ];
   $types['']= [
     'id' => ,
     'name' => '',
     'label' => '',
     'category' => 'Expression',
   ];
   * @endcode
   */
  protected function expressionContentTypes() {

    // The 'biological sample' entity type. bio_data_25
    $terms['sample'] =[
      'accession' => '00195',
      'vocabulary' => [
        'idspace' => 'sep',
      ],
    ];
    $types['sample']= [
      'id' => 25,
      'name' => 'bio_data_25',
      'label' => 'Biological Sample',
      'category' => 'Expression',
    ];

    // The 'Assay' entity type. bio_data_26
    $terms['assay'] =[
      'accession' => '0000070',
      'vocabulary' => [
        'idspace' => 'OBI',
      ],
    ];
    $types['assay']= [
      'id' => 26,
      'name' => 'bio_data_26',
      'label' => 'Assay',
      'category' => 'Expression',
    ];

    // The 'Array Design' entity type. bio_data_27
    $terms['design'] =[
      'accession' => '0000269',
      'vocabulary' => [
        'idspace' => 'EFO',
      ],
    ];
    $types['design']= [
      'id' => 27,
      'name' => 'bio_data_27',
      'label' => 'Array Design',
      'category' => 'Expression',
    ];

    $this->createGivenContentTypes($types, $terms);
  }
}
