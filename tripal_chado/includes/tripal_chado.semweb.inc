<?php

/**
 * Adds defaults to the chado_semweb table.
 */
function tripal_chado_populate_chado_semweb_table() {

  // Add in all tables and fields into the chado_semweb table.
  $chado_tables = chado_get_table_names(TRUE);
  foreach ($chado_tables as $chado_table) {
    chado_add_semweb_table($chado_table);
  }

  // TODO: should this code be in the tripal_chado module? Some of these terms
  // are used solely by web services (e.g. rdfs:label) and are not used to
  // map chado terms to vocabularies.

  // Perhaps we should have an API for working with terms where these can be
  // inserted.

  // Now set defaults!
  tripal_chado_populate_vocab_CO_010();
  tripal_chado_populate_vocab_DC();
  tripal_chado_populate_vocab_EDAM();
  tripal_chado_populate_vocab_ERO();
  tripal_chado_populate_vocab_EFO();
  tripal_chado_populate_vocab_FOAF();
  tripal_chado_populate_vocab_HYDRA();
  tripal_chado_populate_vocab_IAO();
  tripal_chado_populate_vocab_LOCAL();
  tripal_chado_populate_vocab_NCIT();
  tripal_chado_populate_vocab_NCBITAXON();
  tripal_chado_populate_vocab_OBCS();
  tripal_chado_populate_vocab_OBI();
  tripal_chado_populate_vocab_OGI();
  tripal_chado_populate_vocab_RDFS();
  tripal_chado_populate_vocab_SBO();
  tripal_chado_populate_vocab_SCHEMA();
  tripal_chado_populate_vocab_SEP();
  tripal_chado_populate_vocab_SIO();
  tripal_chado_populate_vocab_SO();
  tripal_chado_populate_vocab_SWO();
  tripal_chado_populate_vocab_TAXRANK();
  tripal_chado_populate_vocab_TCONTACT();
  tripal_chado_populate_vocab_TPUB();
  tripal_chado_populate_vocab_UO();

}

/**
 * Adds the friend of a friend database and terms.
 */
function tripal_chado_populate_vocab_FOAF() {

  chado_insert_db([
    'name' => 'foaf',
    'description' => 'Friend of a Friend',
    'url' => 'http://www.foaf-project.org/',
    'urlprefix' => 'http://xmlns.com/foaf/spec/#',
  ]);
  chado_insert_cv(
    'foaf',
    'Friend of a Friend. A dictionary of people-related terms that can be used in structured data).'
  );
}

/**
 * Adds the Hydra vocabulary
 */
function tripal_chado_populate_vocab_HYDRA() {

  // For the HydraConsole to work with webservices the URL must be set as
  // http://www.w3.org/ns/hydra/core
  chado_insert_db([
    'name' => 'hydra',
    'description' => 'A Vocabulary for Hypermedia-Driven Web APIs',
    'url' => 'http://www.w3.org/ns/hydra/core',
    'urlprefix' => 'http://www.w3.org/ns/hydra/core#{accession}',
  ]);
  chado_insert_cv(
    'hydra',
    'A Vocabulary for Hypermedia-Driven Web APIs.'
  );

  $name = chado_insert_cvterm([
    'id' => 'hydra:Collection',
    'name' => 'Collection',
    'cv_name' => 'hydra',
    'definition' => 'A collection holding references to a number of related resources.',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'hydra:member',
    'name' => 'member',
    'cv_name' => 'hydra',
    'definition' => 'A member of the collection',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'hydra:description',
    'name' => 'description',
    'cv_name' => 'hydra',
    'definition' => 'A description.',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'hydra:totalItems',
    'name' => 'totalItems',
    'cv_name' => 'hydra',
    'definition' => 'The total number of items referenced by a collection.',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'hydra:title',
    'name' => 'title',
    'cv_name' => 'hydra',
    'definition' => 'A title, often used along with a description.',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'hydra:PartialCollectionView',
    'name' => 'PartialCollectionView',
    'cv_name' => 'hydra',
    'definition' => 'A PartialCollectionView describes a partial view of a Collection. Multiple PartialCollectionViews can be connected with the the next/previous properties to allow a client to retrieve all members of the collection.',
  ]);
}

/**
 * Adds the RDFS database and terms.
 */
function tripal_chado_populate_vocab_RDFS() {

  chado_insert_db([
    'name' => 'rdf',
    'description' => 'Resource Description Framework',
    'url' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns',
    'urlprefix' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#',
  ]);
  chado_insert_cv(
    'rdf',
    'Resource Description Framework'
  );

  chado_insert_db([
    'name' => 'rdfs',
    'description' => 'Resource Description Framework Schema',
    'url' => 'https://www.w3.org/TR/rdf-schema/',
    'urlprefix' => 'http://www.w3.org/2000/01/rdf-schema#{accession}',
  ]);
  chado_insert_cv(
    'rdfs',
    'Resource Description Framework Schema'
  );

  $name = chado_insert_cvterm([
    'id' => 'rdfs:type',
    'name' => 'type',
    'cv_name' => 'rdfs',
    'definition' => 'The type of resource.',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'rdfs:label',
    'name' => 'label',
    'cv_name' => 'rdfs',
    'definition' => 'A human-readable version of a resource\'s name.',
  ]);
  $name = chado_insert_cvterm([
    'id' => 'rdfs:comment',
    'name' => 'comment',
    'cv_name' => 'rdfs',
    'definition' => 'A human-readable description of a resource\'s name.',
  ]);
}

/**
 * Adds the Schema.org database and terms.
 */
function tripal_chado_populate_vocab_SCHEMA() {
  chado_insert_db([
    'name' => 'schema',
    'description' => 'Schema.org.',
    'url' => 'https://schema.org/',
    'urlprefix' => 'https://schema.org/{accession}',
  ]);
  chado_insert_cv(
    'schema',
    'Schema.org. Schema.org is sponsored by Google, Microsoft, Yahoo and Yandex. The vocabularies are developed by an open community process.'
  );

  $term = chado_insert_cvterm([
    'id' => 'schema:name',
    'name' => 'name',
    'cv_name' => 'schema',
    'definition' => 'The name of the item.',
  ]);
  chado_associate_semweb_term(NULL, 'name', $term);
  chado_associate_semweb_term('analysis', 'sourcename', $term);

  $term = chado_insert_cvterm([
    'id' => 'schema:alternateName',
    'name' => 'alternateName',
    'cv_name' => 'schema',
    'definition' => 'An alias for the item.',
  ]);
  chado_associate_semweb_term(NULL, 'synonym_id', $term);
  chado_associate_semweb_term('cvtermsynonym', 'synonym', $term);

  $term = chado_insert_cvterm([
    'id' => 'schema:comment',
    'name' => 'comment',
    'cv_name' => 'schema',
    'definition' => 'Comments, typically from users.',
  ]);
  chado_associate_semweb_term(NULL, 'comment', $term);

  $term = chado_insert_cvterm([
    'id' => 'schema:description',
    'name' => 'description',
    'cv_name' => 'schema',
    'definition' => 'A description of the item.',
  ]);
  chado_associate_semweb_term(NULL, 'description', $term);
  chado_associate_semweb_term('organism', 'comment', $term);
  chado_associate_semweb_term('protocol', 'protocoldescription', $term);

  $term = chado_insert_cvterm([
    'id' => 'schema:publication',
    'name' => 'publication',
    'cv_name' => 'schema',
    'definition' => 'A publication event associated with the item.',
  ]);
  chado_associate_semweb_term(NULL, 'pub_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'schema:url',
    'name' => 'url',
    'cv_name' => 'schema',
    'definition' => 'URL of the item.',
  ]);
  chado_associate_semweb_term('db', 'url', $term);

  // Typically the type_id field is used for distinguishing between records
  // but in the case that it isn't then we need to associate a term with it
  // An entity already has a type so if that type is not dicated by the
  // type_id field then what is in the type_id should therefore be an
  // "additionalType".  Therefore we need to add and map this term to all
  // of the appropriate type_id fields.
  $term = chado_insert_cvterm([
    'id' => 'schema:additionalType',
    'name' => 'additionalType',
    'cv_name' => 'schema',
    'definition' => 'An additional type for the item, typically used for adding more specific types from external vocabularies in microdata syntax. This is a relationship between something and a class that the thing is in.',
  ]);
  $tables = chado_get_table_names(TRUE);
  foreach ($tables as $table) {
    $schema = chado_get_schema($table);
    // The type_id for the organism is infraspecific type, so don't make
    // the association for that type.
    if ($table == 'organism') {
      continue;
    }
    if (in_array("type_id", array_keys($schema['fields']))) {
      chado_associate_semweb_term($table, 'type_id', $term);
    }
  }

  $term = chado_insert_cvterm([
    'id' => 'schema:ItemPage',
    'name' => 'ItemPage',
    'cv_name' => 'schema',
    'definition' => 'A page devoted to a single item, such as a particular product or hotel.',
  ]);

}

/**
 * Adds the Sample processing and separation techniques database and terms.
 */
function tripal_chado_populate_vocab_SEP() {
  chado_insert_db([
    'name' => 'sep',
    'description' => 'Sample processing and separation techniques.',
    'url' => 'http://psidev.info/index.php?q=node/312',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv('sep', 'A structured controlled vocabulary for the annotation of sample processing and separation techniques in scientific experiments.');
  $term = chado_insert_cvterm([
    'id' => 'sep:00195',
    'name' => 'biological sample',
    'cv_name' => 'sep',
    'definition' => 'A biological sample analysed by a particular technology.',
  ]);
  chado_associate_semweb_term(NULL, 'biomaterial_id', $term);

  $term = tripal_insert_cvterm([
    'id' => 'sep:00101',
    'name' => 'protocol',
    'cv_name' => 'sep',
    'definition' => 'A protocol is a process which is a parameterizable description of a process.',
  ]);
  chado_associate_semweb_term(NULL, 'protocol_id', $term);
  chado_associate_semweb_term(NULL, 'nd_protocol_id', $term);
}

/**
 * Adds the SemanticScience database and terms.
 */
function tripal_chado_populate_vocab_SIO() {
  chado_insert_db([
    'name' => 'SIO',
    'description' => 'Semanticscience Integrated Ontology.',
    'url' => 'http://sio.semanticscience.org/',
    'urlprefix' => 'http://semanticscience.org/resource/{db}_{accession}',
  ]);
  chado_insert_cv('SIO', ' The Semanticscience Integrated Ontology (SIO) provides a simple, integrated ontology of types and relations for rich description of objects, processes and their attributes.');

  $term = chado_insert_cvterm([
    'id' => 'SIO:000493',
    'name' => 'clause',
    'cv_name' => 'SIO',
    'definition' => 'A clause consists of a subject and a predicate.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'SIO:000631',
    'name' => 'references',
    'cv_name' => 'SIO',
    'definition' => 'references is a relation between one entity and the entity that it makes reference to by name, but is not described by it.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'SIO:000056',
    'name' => 'position',
    'cv_name' => 'SIO',
    'definition' => 'A measurement of a spatial location relative to a frame of reference or other objects.',
  ]);
  chado_associate_semweb_term('featurepos', 'mappos', $term);

  $term = chado_insert_cvterm([
    'id' => 'SIO:001166',
    'name' => 'annotation',
    'cv_name' => 'SIO',
    'definition' => 'An annotation is a written explanatory or critical description, or other in-context information (e.g., pattern, motif, link), that has been associated with data or other types of information.',
  ]);
  chado_associate_semweb_term('feature_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('analysis_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('cell_line_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('environment_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('expression_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('library_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('organism_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('phenotype_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('stock_cvterm', 'cvterm_id', $term);
  chado_associate_semweb_term('stock_relationship_cvterm', 'cvterm_id', $term);


  $term = chado_insert_cvterm([
    'id' => 'SIO:000281',
    'name' => 'negation',
    'cv_name' => 'SIO',
    'definition' => 'NOT is a logical operator in that has the value true if its operand is false.',
  ]);
  chado_associate_semweb_term('feature_cvterm', 'is_not', $term);
  chado_associate_semweb_term('analysis_cvterm', 'is_not', $term);
  chado_associate_semweb_term('organism_cvterm', 'is_not', $term);
  chado_associate_semweb_term('stock_cvterm', 'is_not', $term);

  $term = chado_insert_cvterm([
    'id' => 'SIO:001080',
    'name' => 'vocabulary',
    'cv_name' => 'SIO',
    'definition' => 'A vocabulary is a collection of terms.',
  ]);
  chado_associate_semweb_term('cvterm', 'cv_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'SIO:001323',
    'name' => 'email address',
    'cv_name' => 'SIO',
    'definition' => 'an email address is an identifier to send mail to particular electronic mailbox.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'SIO:001007',
    'name' => 'assay',
    'cv_name' => 'SIO',
    'definition' => 'An assay is an investigative (analytic) procedure in ' .
      'laboratory medicine, pharmacology, environmental biology, and ' .
      'molecular biology for qualitatively assessing or quantitatively ' .
      'measuring the presence or amount or the functional activity of a ' .
      'target entity (the analyte) which can be a drug or biochemical ' .
      'substance or a cell in an organism or organic sample.',
  ]);
  chado_associate_semweb_term(NULL, 'assay_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'SIO:010054',
    'name' => 'cell line',
    'cv_name' => 'SIO',
    'definition' => 'A cell line is a collection of genetically identifical cells.',
  ]);
  chado_associate_semweb_term(NULL, 'cell_line_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'SIO:001066',
    'name' => 'study',
    'cv_name' => 'SIO',
    'definition' => 'A study is a process that realizes the steps of a study design.',
  ]);
  chado_associate_semweb_term(NULL, 'study_id', $term);

}

/**
 * Adds the details for the SO vocab and db.
 */
function tripal_chado_populate_vocab_SO() {
  chado_insert_db([
    'name' => 'SO',
    'description' => 'The sequence ontology.',
    'url' => 'http://www.sequenceontology.org/',
    'urlprefix' => 'http://www.sequenceontology.org/browser/current_svn/term/{db}:{accession}',
  ]);
  chado_insert_cv('sequence', 'The sequence ontology.');

  $term = chado_get_cvterm([
    'cv_id' => ['name' => 'sequence'],
    'name' => 'sequence_feature',
  ]);
  chado_associate_semweb_term(NULL, 'feature_id', $term);
}


/**
 * Adds the Crop Ontology terms.
 */
function tripal_chado_populate_vocab_CO_010() {
  chado_insert_db([
    'name' => 'CO_010',
    'description' => 'Crop Germplasm Ontology',
    'url' => 'http://www.cropontology.org/get-ontology/CO_010',
    'urlprefix' => 'http://www.cropontology.org/terms/CO_010:{accession}',
  ]);
  chado_insert_cv(
    'germplasm_ontology',
    'GCP germplasm ontology'
  );
  $term = chado_insert_cvterm([
    'id' => 'CO_010:0000044',
    'name' => 'accession',
    'cv_name' => 'germplasm_ontology',
    'definition' => '',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'CO_010:0000255',
    'name' => 'generated germplasm',
    'cv_name' => 'germplasm_ontology',
    'definition' => '',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'CO_010:0000029',
    'name' => 'cultivar',
    'cv_name' => 'germplasm_ontology',
    'definition' => '',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'CO_010:0000162',
    'name' => '414 inbred line',
    'cv_name' => 'germplasm_ontology',
    'definition' => '',
  ]);
}

/**
 * Adds the DC database.
 */
function tripal_chado_populate_vocab_DC() {
  chado_insert_db([
    'name' => 'dc',
    'description' => 'DCMI Metadata Terms.',
    'url' => 'http://purl.org/dc/dcmitype/',
    'urlprefix' => 'http://purl.org/dc/terms/{accession}',
  ]);
  chado_insert_cv(
    'dc',
    'DCMI Metadata Terms.'
  );
  $term = chado_insert_cvterm([
    'id' => 'dc:Service',
    'name' => 'Service',
    'cv_name' => 'dc',
    'definition' => 'A system that provides one or more functions.',
  ]);
}

/**
 * Adds the EDAM database and terms.
 */
function tripal_chado_populate_vocab_EDAM() {

  chado_insert_db([
    'name' => 'data',
    'description' => 'Bioinformatics operations, data types, formats, identifiers and topics.',
    'url' => 'http://edamontology.org/page',
    'urlprefix' => 'http://edamontology.org/{db}_{accession}',
  ]);
  chado_insert_db([
    'name' => 'format',
    'description' => 'A defined way or layout of representing and structuring data in a computer file, blob, string, message, or elsewhere. The main focus in EDAM lies on formats as means of structuring data exchanged between different tools or resources. ',
    'url' => 'http://edamontology.org/page',
    'urlprefix' => 'http://edamontology.org/{db}_{accession}',
  ]);
  chado_insert_db([
    'name' => 'operation',
    'description' => 'A function that processes a set of inputs and results in a set of outputs, or associates arguments (inputs) with values (outputs). Special cases are: a) An operation that consumes no input (has no input arguments).',
    'url' => 'http://edamontology.org/page',
    'urlprefix' => 'http://edamontology.org/{db}_{accession}',
  ]);
  chado_insert_db([
    'name' => 'topic',
    'description' => 'A category denoting a rather broad domain or field of interest, of study, application, work, data, or technology. Topics have no clearly defined borders between each other.',
    'url' => 'http://edamontology.org/page',
    'urlprefix' => 'http://edamontology.org/{db}_{accession}',
  ]);
  chado_insert_db([
    'name' => 'EDAM',
    'description' => 'Bioinformatics operations, data types, formats, identifiers and topics.',
    'url' => 'http://edamontology.org/page',
    'urlprefix' => 'http://edamontology.org/{db}_{accession}',
  ]);
  chado_insert_cv(
    'EDAM',
    'EDAM is an ontology of well established, familiar concepts that are ' .
    'prevalent within bioinformatics, including types of data and data ' .
    'identifiers, data formats, operations and topics. EDAM is a simple ' .
    'ontology - essentially a set of terms with synonyms and definitions - ' .
    'organised into an intuitive hierarchy for convenient use by curators, ' .
    'software developers and end-users. EDAM is suitable for large-scale ' .
    'semantic annotations and categorization of diverse bioinformatics ' .
    'resources. EDAM is also suitable for diverse application including ' .
    'for example within workbenches and workflow-management systems, ' .
    'software distributions, and resource registries.'
  );

  $term = chado_insert_cvterm([
    'id' => 'data:1249',
    'name' => 'Sequence length',
    'cv_name' => 'EDAM',
    'definition' => 'The size (length) of a sequence, subsequence or region in a sequence, or range(s) of lengths.',
  ]);
  chado_associate_semweb_term('feature', 'seqlen', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:2190',
    'name' => 'Sequence checksum',
    'cv_name' => 'EDAM',
    'definition' => 'A fixed-size datum calculated (by using a hash function) for a molecular sequence, typically for purposes of error detection or indexing.',
  ]);
  chado_associate_semweb_term(NULL, 'md5checksum', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:2091',
    'name' => 'Accession',
    'cv_name' => 'EDAM',
    'definition' => 'A persistent (stable) and unique identifier, typically identifying an object (entry) from a database.',
  ]);
  chado_associate_semweb_term(NULL, 'dbxref_id', $term);
  chado_associate_semweb_term('dbxref', 'accession', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:2044',
    'name' => 'Sequence',
    'cv_name' => 'EDAM',
    'definition' => 'One or more molecular sequences, possibly with associated annotation.',
  ]);
  chado_associate_semweb_term('feature', 'residues', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:0849',
    'name' => 'Sequence record',
    'cv_name' => 'EDAM',
    'definition' => 'A molecular sequence and associated metadata.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'data:0842',
    'name' => 'Identifier',
    'cv_name' => 'EDAM',
    'definition' => 'A text token, number or something else which identifies an entity, but which may not be persistent (stable) or unique (the same identifier may identify multiple things).',
  ]);
  chado_associate_semweb_term(NULL, 'uniquename', $term);
  chado_associate_semweb_term('assay', 'arrayidentifier', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:2976',
    'name' => 'Protein sequence',
    'cv_name' => 'EDAM',
    'definition' => 'One or more protein sequences, possibly with associated annotation.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'data:2968',
    'name' => 'Image',
    'cv_name' => 'EDAM',
    'definition' => 'Biological or biomedical data has been rendered into an image, typically for display on screen.',
  ]);
  chado_associate_semweb_term(NULL, 'eimage_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:1274',
    'name' => 'Map',
    'cv_name' => 'EDAM',
    'definition' => 'A map of (typically one) DNA sequence annotated with positional or non-positional features.',
  ]);
  chado_associate_semweb_term(NULL, 'featuremap_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:1278',
    'name' => 'Genetic map',
    'cv_name' => 'EDAM',
    'definition' => 'A map showing the relative positions of genetic markers in a nucleic acid sequence, based on estimation of non-physical distance such as recombination frequencies.',
  ]);
  chado_associate_semweb_term('featuremap', 'featuremap_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:1280',
    'name' => 'Physical map',
    'cv_name' => 'EDAM',
    'definition' => 'A map of DNA (linear or circular) annotated with physical features or landmarks such as restriction sites, cloned DNA fragments, genes or genetic markers, along with the physical distances between them. Distance in a physical map is measured in base pairs. A physical map might be ordered relative to a reference map (typically a genetic map) in the process of genome sequencing.',
  ]);
  chado_associate_semweb_term('featuremap', 'featuremap_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:2012',
    'name' => 'Sequence coordinates',
    'cv_name' => 'EDAM',
    'definition' => 'A position in a map (for example a genetic map), either a single position (point) or a region / interval.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'data:1056',
    'name' => 'Database name',
    'cv_name' => 'EDAM',
    'definition' => 'The name of a biological or bioinformatics database.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'data:1048',
    'name' => 'Database ID',
    'cv_name' => 'EDAM',
    'definition' => 'An identifier of a biological or bioinformatics database.',
  ]);
  chado_associate_semweb_term('db', 'name', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:1047',
    'name' => 'URI',
    'cv_name' => 'EDAM',
    'definition' => 'The name of a biological or bioinformatics database.',
  ]);
  chado_associate_semweb_term('analysis', 'sourceuri', $term);
  chado_associate_semweb_term(NULL, 'uri', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:2336',
    'name' => 'Translation phase specification',
    'cv_name' => 'EDAM',
    'definition' => 'Phase for translation of DNA (0, 1 or 2) relative to a fragment of the coding sequence.',
  ]);
  chado_associate_semweb_term('featureloc', 'phase', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:0853',
    'name' => 'DNA sense specification',
    'cv_name' => 'EDAM',
    'definition' => 'The strand of a DNA sequence (forward or reverse).',
  ]);
  chado_associate_semweb_term('featureloc', 'strand', $term);
  $term = chado_insert_cvterm([
    'id' => 'data:3002',
    'name' => 'Annotation track',
    'cv_name' => 'EDAM',
    'definition' => 'Annotation of one particular positional feature on a ' .
      'biomolecular (typically genome) sequence, suitable for import and ' .
      'display in a genome browser. Synonym: Sequence annotation track.',
  ]);
  chado_associate_semweb_term('featureloc', 'srcfeature_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'operation:2945',
    'name' => 'Analysis',
    'cv_name' => 'EDAM',
    'definition' => 'Apply analytical methods to existing data of a specific type.',
  ]);
  chado_associate_semweb_term(NULL, 'analysis_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'data:0872',
    'name' => 'Phylogenetic tree',
    'cv_name' => 'EDAM',
    'definition' => 'The raw data (not just an image) from which a phylogenetic tree is directly generated or plotted, such as topology, lengths (in time or in expected amounts of variance) and a confidence interval for each length.',
  ]);
  chado_associate_semweb_term(NULL, 'phylotree_id', $term);
  $term = chado_insert_cvterm([
    'id' => 'data:3272',
    'name' => 'Species tree',
    'cv_name' => 'EDAM',
    'definition' => 'A phylogenetic tree that reflects phylogeny of the taxa from which the characters (used in calculating the tree) were sampled.',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'data:3271',
    'name' => 'Gene tree',
    'cv_name' => 'EDAM',
    'definition' => 'A phylogenetic tree that is an estimate of the character\'s phylogeny.',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'operation:0567',
    'name' => 'Phylogenetic tree visualisation',
    'cv_name' => 'EDAM',
    'definition' => 'A phylogenetic tree that is an estimate of the character\'s phylogeny.',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'operation:0564',
    'name' => 'Sequence visualisation',
    'cv_name' => 'EDAM',
    'definition' => 'Visualise, format or render a molecular sequence or sequences such as a sequence alignment, possibly with sequence features or properties shown.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'operation:0525',
    'name' => 'genome assembly',
    'cv_name' => 'EDAM',
    'definition' => '',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'operation:0362',
    'name' => 'Genome annotation ',
    'cv_name' => 'EDAM',
    'definition' => '',
  ]);

}

/**
 * Adds the Experimental Factor Ontology and terms.
 */
function tripal_chado_populate_vocab_EFO() {
  chado_insert_db([
    'name' => 'EFO',
    'description' => 'Experimental Factor Ontology',
    'url' => 'http://www.ebi.ac.uk/efo/efo.owl',
    'urlprefix' => 'http://www.ebi.ac.uk/efo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'efo',
    'The Experimental Factor Ontology (EFO) provides a systematic description of many experimental variables available in EBI databases, and for external projects such as the NHGRI GWAS catalogue. It combines parts of several biological ontologies, such as anatomy, disease and chemical compounds. The scope of EFO is to support the annotation, analysis and visualization of data handled by many groups at the EBI and as the core ontology for OpenTargets.org'
  );

  $term = chado_insert_cvterm([
    'id' => 'EFO:0000548',
    'name' => 'instrument',
    'cv_name' => 'efo',
    'definition' => 'An instrument is a device which provides a mechanical or electronic function.',
  ]);
  chado_associate_semweb_term('protocol', 'hardwaredescription', $term);

  $term = chado_insert_cvterm([
    'id' => 'EFO:0000269',
    'name' => 'array design',
    'cv_name' => 'efo',
    'definition' => 'An instrument design which describes the design of the array.',
  ]);
  chado_associate_semweb_term('assay', 'arraydesign_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'EFO:0005522',
    'name' => 'substrate type',
    'cv_name' => 'efo',
    'definition' => 'Controlled terms for descriptors of types of array substrates.',
  ]);
  chado_associate_semweb_term('arraydesign', 'substratetype_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'EFO:0001728',
    'name' => 'array manufacturer',
    'cv_name' => 'efo',
    'definition' => '',
  ]);
  chado_associate_semweb_term('arraydesign', 'manufacturer_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'EFO:0000269',
    'name' => 'array design',
    'cv_name' => 'efo',
    'definition' => 'An instrument design which describes the design of the array.',
  ]);
  chado_associate_semweb_term('element', 'arraydesign_id', $term);
}

/**
 * Adds the Eagle-i Resource Ontology database and terms.
 */
function tripal_chado_populate_vocab_ERO() {
  chado_insert_db([
    'name' => 'ERO',
    'description' => 'The Eagle-I Research Resource Ontology',
    'url' => 'http://purl.bioontology.org/ontology/ERO',
    'urlprefix' => 'http://purl.bioontology.org/ontology/ERO/{db}:{accession}',
  ]);
  chado_insert_cv(
    'ero',
    'The Eagle-I Research Resource Ontology models research resources such instruments. protocols, reagents, animal models and biospecimens. It has been developed in the context of the eagle-i project (http://eagle-i.net/).'
  );

  $term = chado_insert_cvterm([
    'id' => 'ERO:0001716',
    'name' => 'database',
    'cv_name' => 'ero',
    'definition' => 'A database is an organized collection of data, today typically in digital form.',
  ]);
  chado_associate_semweb_term(NULL, 'db_id', $term);
  $term = chado_insert_cvterm([
    'id' => 'ERO:0000387',
    'name' => 'data acquisition',
    'cv_name' => 'ero',
    'definition' => 'A technique that samples real world physical conditions and conversion of the resulting samples into digital numeric values that can be manipulated by a computer.',
  ]);
  chado_associate_semweb_term(NULL, 'acquisition_id', $term);
}

/**
 * Adds the Information Artifact Ontology database and terms.
 */
function tripal_chado_populate_vocab_OBCS() {
  chado_insert_db([
    'name' => 'OBCS',
    'description' => 'Ontology of Biological and Clinical Statistics.',
    'url' => 'https://github.com/obcs/obcs',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'OBCS',
    'Ontology of Biological and Clinical Statistics.'
  );

  $term = chado_insert_cvterm([
    'id' => 'OBCS:0000117',
    'name' => 'rank order',
    'cv_name' => 'OBCS',
    'definition' => 'A data item that represents an arrangement according to a rank, i.e., the position of a particular case relative to other cases on a defined scale.',
  ]);
  chado_associate_semweb_term(NULL, 'rank', $term);
}

/**
 * Adds the Information Artifact Ontology database and terms.
 */
function tripal_chado_populate_vocab_OBI() {
  chado_insert_db([
    'name' => 'OBI',
    'description' => 'The Ontology for Biomedical Investigation.',
    'url' => 'http://obi-ontology.org/page/Main_Page',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'obi',
    'Ontology for Biomedical Investigation. The Ontology for Biomedical Investigations (OBI) is build in a collaborative, international effort and will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data. This ontology arose from the Functional Genomics Investigation Ontology (FuGO) and will contain both terms that are common to all biomedical investigations, including functional genomics investigations and those that are more domain specific.'
  );

  $term = chado_insert_cvterm([
    'id' => 'OBI:0100026',
    'name' => 'organism',
    'cv_name' => 'obi',
    'definition' => 'A material entity that is an individual living system, such as animal, plant, bacteria or virus, that is capable of replicating or reproducing, growth and maintenance in the right environment. An organism may be unicellular or made up, like humans, of many billions of cells divided into specialized tissues and organs.',
  ]);
  chado_associate_semweb_term(NULL, 'organism_id', $term);
  chado_associate_semweb_term('biomaterial', 'taxon_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'OBI:0000070',
    'name' => 'assay',
    'cv_name' => 'obi',
    'definition' => 'A planned process with the objective to produce information about the material entity that is the evaluant, by physically examining it or its proxies.',
  ]);
}

/**
 * Adds the Ontology for genetic interval database and terms.
 */
function tripal_chado_populate_vocab_OGI() {
  chado_insert_db([
    'name' => 'OGI',
    'description' => 'Ontology for genetic interval.',
    'url' => 'http://purl.bioontology.org/ontology/OGI',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'ogi',
    'Ontology for Biomedical Investigation. The Ontology for Biomedical Investigations (OBI) is build in a collaborative, international effort and will serve as a resource for annotating biomedical investigations, including the study design, protocols and instrumentation used, the data generated and the types of analysis performed on the data. This ontology arose from the Functional Genomics Investigation Ontology (FuGO) and will contain both terms that are common to all biomedical investigations, including functional genomics investigations and those that are more domain specific.'
  );

  $term = chado_insert_cvterm([
    'id' => 'OGI:0000021',
    'name' => 'location on map',
    'cv_name' => 'ogi',
    'definition' => '',
  ]);

}

/**
 * Adds the Information Artifact Ontology database and terms.
 */
function tripal_chado_populate_vocab_IAO() {

  chado_insert_db([
    'name' => 'IAO',
    'description' => 'The Information Artifact Ontology (IAO).',
    'url' => 'https://github.com/information-artifact-ontology/IAO/',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'IAO',
    'Information Artifact Ontology  is a new ' .
    'ontology of information entities, originally driven by work by the ' .
    'OBI digital entity and realizable information entity branch.'
  );

  $term = chado_insert_cvterm([
    'id' => 'IAO:0000115',
    'name' => 'definition',
    'cv_name' => 'IAO',
    'definition' => 'The official OBI definition, explaining the meaning of ' .
      'a class or property. Shall be Aristotelian, formalized and normalized. ' .
      'Can be augmented with colloquial definitions.',
  ]);
  chado_associate_semweb_term(NULL, 'definition', $term);

  $term = chado_insert_cvterm([
    'id' => 'IAO:0000129',
    'name' => 'version number',
    'cv_name' => 'IAO',
    'definition' => 'A version number is an ' .
      'information content entity which is a sequence of characters ' .
      'borne by part of each of a class of manufactured products or its ' .
      'packaging and indicates its order within a set of other products ' .
      'having the same name.',
  ]);
  chado_associate_semweb_term('analysis', 'programversion', $term);
  chado_associate_semweb_term('analysis', 'sourceversion', $term);
  chado_associate_semweb_term(NULL, 'version', $term);

  $term = chado_insert_cvterm([
    'id' => 'IAO:0000064',
    'name' => 'algorithm',
    'cv_name' => 'IAO',
    'definition' => 'An algorithm is a set of instructions for performing a paticular calculation.',
  ]);
  chado_associate_semweb_term('analysis', 'algorithm', $term);
}

/**
 * Adds terms to the 'local' database.
 *
 * These are terms where an appropriate match could not be found in any other
 * ontology.
 */
function tripal_chado_populate_vocab_LOCAL() {
  global $base_path;

  chado_insert_db([
    'name' => 'null',
    'description' => 'No online database.',
    'url' => $base_path . 'cv/lookup/null',
    'urlprefix' => $base_path . 'cv/lookup/{db}/{accession}',
  ]);
  chado_insert_db([
    'name' => 'local',
    'description' => 'Terms created for this site.',
    'url' => $base_path . 'cv/lookup/local',
    'urlprefix' => $base_path . 'cv/lookup/{db}/{accession}',
  ]);


  // ----------------
  // Add the various CV's that fall under the local DB.
  // ----------------
  chado_insert_cv(
    'local',
    'Locally created terms.'
  );
  chado_insert_cv(
    'organism_property',
    'A local vocabulary that contains locally defined properties for organisms'
  );
  chado_insert_cv(
    'analysis_property',
    'A local vocabulary that contains locally defined properties for analyses'
  );
  chado_insert_cv(
    'tripal_phylogeny',
    'Terms used by the Tripal phylotree module for phylogenetic and taxonomic trees.'
  );
  // Add cv for relationship types
  chado_insert_cv(
    'feature_relationship',
    'A local vocabulary that contains types of relationships between features.'
  );

  // The feature_property CV may already exists. It comes with Chado, but
  // we need to  add it just in case it doesn't get added before the feature
  // module is installed. But as of Tripal v3.0 the Chado version of this
  // vocabulary is no longer loaded by default.
  chado_insert_cv(
    'feature_property',
    'A local vocabulary that contains properties for genomic features'
  );
  // Add the cv for contact properties. This is a default vocabulary in the event
  // that a user does not want to use the tripal_contact vocabulary
  chado_insert_cv(
    'contact_property',
    'A local vocabulary that contains properties for contacts. This can be used if the tripal_contact vocabulary (which is default for contacts in Tripal) is not desired.'
  );

  // add the cv for the contact type. This is a default vocabulary in the event
  // that a user does not want to use the tripal_contact vocabulary
  chado_insert_cv(
    'contact_type',
    'A local vocabulary that contains types of contacts. This can be used if the tripal_contact vocabulary (which is default for contacts in Tripal) is not desired.'
  );

  // Add the cv for the tripal_contact vocabulary which is loaded via the OBO
  chado_insert_cv(
    'tripal_contact',
    'A local vocabulary that contains a heirarchical set of terms for describing a contact. It is intended to be used as the default vocabularies in Tripal for contact types and contact properties.'
  );

  // add the cv for contact relationships
  chado_insert_cv(
    'contact_relationship',
    'A local vocabulary that contains types of relationships between contacts.'
  );
  chado_insert_cv(
    'featuremap_units',
    'A local vocabulary that contains map unit types for the unittype_id column of the featuremap table.'
  );

  chado_insert_cv(
    'featurepos_property',
    'A local vocabulary that contains terms map properties.'
  );

  chado_insert_cv(
    'featuremap_property',
    'A local vocabulary that contains positional types for the feature positions'
  );
  chado_insert_cv(
    'library_property',
    'A local vocabulary that contains properties for libraries.'
  );
  chado_insert_cv(
    'library_type',
    'A local vocabulary that contains terms for types of libraries (e.g. BAC, cDNA, FOSMID, etc).'
  );
  // Add the cv for project properties
  chado_insert_cv(
    'project_property',
    'A local vocabulary that contains properties for projects.'
  );
  // Add the cv for project properties
  chado_insert_cv(
    'study_property',
    'A local vocabulary that contains properties for studies.'
  );
  // Add cv for relationship types
  chado_insert_cv(
    'project_relationship',
    'A local vocabulary that contains Types of relationships between projects.'
  );
  // Add the cv for pub properties
  chado_insert_cv(
    'tripal_pub',
    'A local vocabulary that contains a heirarchical set of terms for describing a publication. It is intended to be used as the default vocabularies in Tripal for publication types and contact properties.'
  );

  // Add the cv for pub types
  chado_insert_cv(
    'pub_type',
    'A local vocabulary that contains types of publications. This can be used if the tripal_pub vocabulary (which is default for publications in Tripal) is not desired.'
  );

  // Add the cv for pub properties
  chado_insert_cv(
    'pub_property',
    'A local vocabulary that contains properties for publications. This can be used if the tripal_pub vocabulary (which is default for publications in Tripal) is not desired.'
  );

  // Add cv for relationship types
  chado_insert_cv(
    'pub_relationship',
    'A local vocabulary that contains types of relationships between publications.'
  );

  // Add cv for relationship types
  chado_insert_cv(
    'stock_relationship',
    'A local vocabulary that contains types of relationships between stocks.'
  );
  chado_insert_cv(
    'stock_property',
    'A local vocabulary that contains properties for stocks.'
  );
  chado_insert_cv(
    'stock_type',
    'A local vocabulary that contains a list of types for stocks.'
  );
  chado_insert_cv(
    'tripal_analysis',
    'A local vocabulary that contains terms used for analyses.'
  );

  //-----------------------------
  // Misc Terms
  //-----------------------------
  $term = chado_insert_cvterm([
    'id' => 'local:property',
    'name' => 'property',
    'cv_name' => 'local',
    'definition' => 'A generic term indicating that represents an attribute, quality or characteristic of something.',
  ]);

  //-----------------------------
  // Terms for base table fields
  //-----------------------------
  $term = chado_insert_cvterm([
    'id' => 'local:timelastmodified',
    'name' => 'time_last_modified',
    'cv_name' => 'local',
    'definition' => 'The time at which the record was last modified.',
  ]);
  chado_associate_semweb_term(NULL, 'timelastmodified', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:timeaccessioned',
    'name' => 'time_accessioned',
    'cv_name' => 'local',
    'definition' => 'The time at which the record was first added.',
  ]);
  chado_associate_semweb_term(NULL, 'timeaccessioned', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:timeexecuted',
    'name' => 'time_executed',
    'cv_name' => 'local',
    'definition' => 'The time when the task was executed.',
  ]);
  chado_associate_semweb_term(NULL, 'timeexecuted', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:infraspecific_type',
    'name' => 'infraspecific_type',
    'definition' => 'The connector type (e.g. subspecies, varietas, forma, etc.) for the infraspecific name',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('organism', 'type_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:abbreviation',
    'name' => 'abbreviation',
    'cv_name' => 'local',
    'definition' => 'A shortened name (or abbreviation) for the item.',
  ]);
  chado_associate_semweb_term('organism', 'abbreviation', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:expression',
    'name' => 'expression',
    'definition' => 'Curated expression data',
    'cv_name' => 'local',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'local:is_analysis',
    'name' => 'is_analysis',
    'definition' => 'Indicates if this feature was predicted computationally using another feature.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('feature', 'is_analysis', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:is_obsolete',
    'name' => 'is_obsolete',
    'definition' => 'Indicates if this record is obsolete.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term(NULL, 'is_obsolete', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:is_current',
    'name' => 'is_current',
    'definition' => 'Indicates if this record is current.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term(NULL, 'is_current', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:is_internal',
    'name' => 'is_internal',
    'definition' => 'Indicates if this record is internal and not normally available outside of a local setting.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term(NULL, 'is_internal', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:miniref',
    'name' => 'Mini-ref',
    'definition' => 'A small in-house unique identifier for a publication.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('pub', 'miniref', $term);


  $term = chado_insert_cvterm([
    'id' => 'local:array_batch_identifier',
    'name' => 'Array Batch Identifier',
    'definition' => 'A unique identifier for an array batch.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('assay', 'arraybatchidentifier', $term);

  //-----------------------------
  // Relationship Terms
  //-----------------------------
  $term = chado_insert_cvterm([
    'id' => 'local:relationship_subject',
    'name' => 'clause subject',
    'definition' => 'The subject of a relationship clause.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term(NULL, 'subject_id', $term);
  chado_associate_semweb_term(NULL, 'subject_reagent_id', $term);
  chado_associate_semweb_term(NULL, 'subject_project_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:relationship_object',
    'name' => 'clause predicate',
    'definition' => 'The object of a relationship clause.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term(NULL, 'object_id', $term);
  chado_associate_semweb_term(NULL, 'object_reagent_id', $term);
  chado_associate_semweb_term(NULL, 'object_project_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'local:relationship_type',
    'name' => 'relationship type',
    'definition' => 'The relationship type.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('acquisition_relationship', 'type_id', $term);
  chado_associate_semweb_term('biomaterial_relationship', 'type_id', $term);
  chado_associate_semweb_term('cell_line_relationship', 'type_id', $term);
  chado_associate_semweb_term('contact_relationship', 'type_id', $term);
  chado_associate_semweb_term('element_relationship', 'type_id', $term);
  chado_associate_semweb_term('elementresult_relationship', 'type_id', $term);
  chado_associate_semweb_term('feature_relationship', 'type_id', $term);
  chado_associate_semweb_term('nd_reagent_relationship', 'type_id', $term);
  chado_associate_semweb_term('phylonode_relationship', 'type_id', $term);
  chado_associate_semweb_term('project_relationship', 'type_id', $term);
  chado_associate_semweb_term('pub_relationship', 'type_id', $term);
  chado_associate_semweb_term('quantification_relationship', 'type_id', $term);
  chado_associate_semweb_term('stock_relationship', 'type_id', $term);
  chado_associate_semweb_term('cvterm_relationship', 'type_id', $term);

  //-----------------------------
  // NCBI Organism Property Terms
  //-----------------------------
  // TODO: these probably have real terms we can use.

  $term = chado_insert_cvterm([
    'id' => 'local:rank',
    'name' => 'rank',
    'definition' => 'A taxonmic rank',
    'cv_name' => 'local',
  ]);

  $terms = [
    'lineage',
    'genetic_code',
    'genetic_code_name',
    'mitochondrial_genetic_code',
    'mitochondrial_genetic_code_name',
    'division',
    'genbank_common_name',
    'synonym',
    'other_name',
    'equivalent_name',
    'anamorph',
  ];
  $options = ['update_existing' => TRUE];
  foreach ($terms as $term) {
    $value = [
      'name' => $term,
      'definition' => '',
      'cv_name' => 'organism_property',
      'db_name' => 'local',
    ];
    chado_insert_cvterm($value, $options);
  }

  //---------------------
  // Phylogeny Tree Terms
  //---------------------

  // Add the terms used to identify nodes in the tree.
  chado_insert_cvterm([
    'name' => 'phylo_leaf',
    'definition' => 'A leaf node in a phylogenetic tree.',
    'cv_name' => 'tripal_phylogeny',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  // Add the terms used to identify nodes in the tree.
  chado_insert_cvterm([
    'name' => 'phylo_root',
    'definition' => 'The root node of a phylogenetic tree.',
    'cv_name' => 'tripal_phylogeny',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);

  // Add the terms used to identify nodes in the tree.
  chado_insert_cvterm([
    'name' => 'phylo_interior',
    'definition' => 'An interior node in a phylogenetic tree.',
    'cv_name' => 'tripal_phylogeny',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);

  // Add the terms used to identify nodes in the tree.
  // DEPRECATED: use EDAM's data 'Species tree' term instead.
  chado_insert_cvterm([
    'name' => 'taxonomy',
    'definition' => 'A term used to indicate if a phylotree is a taxonomic tree',
    'cv_name' => 'tripal_phylogeny',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);

  //--------------
  // Project Terms
  //--------------
  // Insert cvterm 'Project Description' into cvterm table of chado
  // database. This CV term is used to keep track of the project
  // description in the projectprop table.
  chado_insert_cvterm([
    'name' => 'Project Description',
    'definition' => 'Description of a project',
    'cv_name' => 'project_property',
    'db_name' => 'local',
  ]);

  chado_insert_cvterm([
    'name' => 'Project Type',
    'definition' => 'A type of project',
    'cv_name' => 'project_property',
    'db_name' => 'local',
  ]);

  //--------------
  // Natural Diversity Terms
  //--------------
  // add cvterms for the nd_experiment_types
  chado_insert_cvterm([
    'name' => 'Genotyping',
    'definition' => 'An experiment where genotypes of individuals are identified.',
    'cv_name' => 'nd_experiment_types',
    'db_name' => 'local',
  ]);

  chado_insert_cvterm([
    'name' => 'Phenotyping',
    'definition' => 'An experiment where phenotypes of individuals are identified.',
    'cv_name' => 'nd_experiment_types',
    'db_name' => 'local',
  ]);

  chado_insert_cvterm([
    'name' => 'Location',
    'definition' => 'The name of the location.',
    'cv_name' => 'nd_geolocation_property',
    'db_name' => 'local',
  ]);


  //--------------
  // Library Terms
  //--------------
  $term = chado_insert_cvterm([
    'id' => 'local:library',
    'name' => 'Library',
    'definition' => 'A group of physical entities organized into a collection',
    'cv_name' => 'local',
    'db_name' => 'local',
  ]);
  chado_associate_semweb_term(NULL, 'library_id', $term);

  // Insert cvterm 'library_description' into cvterm table of chado
  // database. This CV term is used to keep track of the library
  // description in the libraryprop table.
  $term = chado_insert_cvterm([
    'id' => 'local:library_description',
    'name' => 'Library Description',
    'definition' => 'Description of a library',
    'cv_name' => 'library_property',
    'db_name' => 'local',
  ]);

  // add cvterms for the map unit types
  $term = chado_insert_cvterm([
    'id' => 'local:cdna_library',
    'name' => 'cdna_library',
    'definition' => 'cDNA library',
    'cv_name' => 'library_type',
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'local:bac_library',
    'name' => 'bac_library',
    'definition' => 'Bacterial Artifical Chromsome (BAC) library',
    'cv_name' => 'library_type',
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'local:fosmid_library',
    'name' => 'fosmid_library',
    'definition' => 'Fosmid library',
    'cv_name' => 'library_type',
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'local:cosmid_library',
    'name' => 'cosmid_library',
    'definition' => 'Cosmid library',
    'cv_name' => 'library_type',
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'local:yac_library',
    'name' => 'yac_library',
    'definition' => 'Yeast Artificial Chromosome (YAC) library',
    'cv_name' => 'library_type',
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'id' => 'local:genomic_library',
    'name' => 'genomic_library',
    'definition' => 'Genomic Library',
    'cv_name' => 'library_type',
    'db_name' => 'local',
  ]);

  //--------
  // Feature
  //--------
  $term = chado_insert_cvterm([
    'name' => 'fasta_definition',
    'definition' => 'The definition line for a FASTA formatted sequence',
    'cv_name' => 'local',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);

  //--------------
  // Feature Map
  //--------------
  // add cvterms for the map unit types
  $term = chado_insert_cvterm([
    'name' => 'cM',
    'definition' => 'Centimorgan units',
    'cv_name' => 'featuremap_units',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'bp',
    'definition' => 'Base pairs units',
    'cv_name' => 'featuremap_units',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'bin_unit',
    'definition' => 'The bin unit',
    'cv_name' => 'featuremap_units',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'marker_order',
    'definition' => 'Units simply to define marker order.',
    'cv_name' => 'featuremap_units',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'undefined',
    'definition' => 'A catch-all for an undefined unit type',
    'cv_name' => 'featuremap_units',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  // featurepos properties
  $term = chado_insert_cvterm([
    'name' => 'start',
    'definition' => 'The start coordinate for a map feature.',
    'cv_name' => 'featurepos_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'stop',
    'definition' => 'The end coordinate for a map feature',
    'cv_name' => 'featurepos_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  // add cvterms for map properties
  $term = chado_insert_cvterm([
    'name' => 'Map Dbxref',
    'definition' => 'A unique identifer for the map in a remote database.  The '
      . 'format is a database abbreviation and a unique accession separated '
      . 'by a colon.  (e.g. Gramene:tsh1996a)',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'Map Type',
    'definition' => 'The type of Map (e.g. QTL, Physical, etc.)',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'Genome Group',
    'definition' => '',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'URL',
    'definition' => 'A univeral resource locator (URL) reference where the '
      . 'publication can be found.  For maps found online, this would be '
      . 'the web address for the map.',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'Population Type',
    'definition' => 'A brief description of the population type used to generate '
      . 'the map (e.g. RIL, F2, BC1, etc).',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'Population Size',
    'definition' => 'The size of the population used to construct the map.',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'Methods',
    'definition' => 'A brief description of the methods used to construct the map.',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  $term = chado_insert_cvterm([
    'name' => 'Software',
    'definition' => 'The software used to construct the map.',
    'cv_name' => 'featuremap_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);

  $term = chado_insert_cvterm([
    'name' => 'Reference Feature',
    'definition' => 'A genomic or genetic feature on which other features are mapped.',
    'cv_name' => 'local',
    'is_relationship' => 0,
    'db_name' => 'local',
  ]);
  chado_associate_semweb_term('featurepos', 'map_feature_id', $term);


  //--------------
  // Featureloc Terms
  //--------------
  $term = chado_insert_cvterm([
    'id' => 'local:fmin',
    'name' => 'minimal boundary',
    'definition' => 'The leftmost, minimal boundary in the linear range ' .
      'represented by the feature location. Sometimes this is called ' .
      'start although this is confusing because it does not necessarily ' .
      'represent the 5-prime coordinate.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('featureloc', 'fmin', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:fmax',
    'name' => 'maximal boundary',
    'definition' => 'The rightmost, maximal boundary in the linear range ' .
      'represented by the featureloc. Sometimes this is called end although ' .
      'this is confusing because it does not necessarily represent the ' .
      '3-prime coordinate',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('featureloc', 'fmax', $term);


  //--------------
  // Analysis Terms
  //--------------
  // add analysis_date.  This is no longer used (as far as we can tell) but we don't
  // get rid of it in case it is used, so just keep it in the Tripal CV
  $term = chado_insert_cvterm([
    'name' => 'analysis_date',
    'definition' => 'The date that an analysis was performed.',
    'cv_name' => 'tripal_analysis',
    'is_relationship' => 0,
    'db_name' => 'local',
  ], ['update_existing' => TRUE]);

  // add analysis_short_name.  This is no longer used (as far as we can tell) but we don't
  // get rid of it in case it is used, so just keep it in the Tripal CV
  $term = chado_insert_cvterm([
    'name' => 'analysis_short_name',
    'definition' => 'A computer legible (no spaces or special characters) '
      . 'abbreviation for the analysis.',
    'cv_name' => 'tripal_analysis',
    'is_relationship' => 0,
    'db_name' => 'local',
  ], ['update_existing' => TRUE]);


  // the 'analysis_property' vocabulary is for user definable properties wo we
  // will add an 'Analysis Type' to this vocubulary
  $term = chado_insert_cvterm([
    'id' => 'local:Analysis Type',
    'name' => 'Analysis Type',
    'definition' => 'The type of analysis that was performed.',
    'cv_name' => 'analysis_property',
    'is_relationship' => 0,
    'db_name' => 'local',
  ], ['update_existing' => TRUE]);

  // Add a term to be used for an inherent 'type_id' for the organism table.
  $term = chado_insert_cvterm([
    'id' => 'local:analysis',
    'name' => 'analysis',
    'definition' => 'A process as a method of studying the nature of something ' .
      'or of determining its essential features and their relations. ' .
      '(Random House Kernerman Webster\'s College Dictionary, © 2010 K ' .
      'Dictionaries Ltd).',
    'cv_name' => 'local',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'local:source_data',
    'name' => 'source_data',
    'definition' => 'The location where data that is being used come from.',
    'cv_name' => 'local',
  ]);

  //--------------
  // Terms for Content Types
  //--------------
  $term = chado_insert_cvterm([
    'id' => 'local:contact',
    'name' => 'contact',
    'definition' => 'An entity (e.g. individual or organization) through ' .
      'whom a person can gain access to information, favors, ' .
      'influential people, and the like.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('biomaterial', 'biosourceprovider_id', $term);
  chado_associate_semweb_term(NULL, 'contact_id', $term);


  $term = chado_insert_cvterm([
    'id' => 'local:relationship',
    'name' => 'relationship',
    'definition' => 'The way in which two things are connected.',
    'cv_name' => 'local',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'local:biomaterial',
    'name' => 'biomaterial',
    'definition' => 'A biomaterial represents the MAGE concept of BioSource, BioSample, ' .
      'and LabeledExtract. It is essentially some biological material (tissue, cells, serum) that ' .
      'may have been processed. Processed biomaterials should be traceable back to raw ' .
      'biomaterials via the biomaterialrelationship table.',
    'cv_name' => 'local',
  ]);

  //
  // Terms for arraydesign table
  //
  $term = chado_insert_cvterm([
    'id' => 'local:array_dimensions',
    'name' => 'array_dimensions',
    'definition' => 'The dimensions of an array.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'array_dimensions', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:element_dimensions',
    'name' => 'element_dimensions',
    'definition' => 'The dimensions of an element.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'element_dimensions', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_of_elements',
    'name' => 'num_of_elements',
    'definition' => 'The number of elements.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_of_elements', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_array_columns',
    'name' => 'num_array_columns',
    'definition' => 'The number of columns in an array.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_array_columns', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_array_rows',
    'name' => 'num_array_rows',
    'definition' => 'The number of rows in an array.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_array_rows', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_grid_columns',
    'name' => 'num_grid_columns',
    'definition' => 'The number of columns in a grid.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_grid_columns', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_grid_rows',
    'name' => 'num_grid_rows',
    'definition' => 'The number of rows in a grid.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_grid_rows', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_sub_columns',
    'name' => 'num_sub_columns',
    'definition' => 'The number of sub columns.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_sub_columns', $term);
  $term = chado_insert_cvterm([
    'id' => 'local:num_sub_rows',
    'name' => 'num_sub_rows',
    'definition' => 'The number of sub rows.',
    'cv_name' => 'local',
  ]);
  chado_associate_semweb_term('arraydesign', 'num_sub_rows', $term);


  //
  // Terms for Study
  //
  chado_insert_cvterm([
    'name' => 'Study Type',
    'definition' => 'A type of study',
    'cv_name' => 'study_property',
    'db_name' => 'local',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'local:Genome Project',
    'name' => 'Genome Project',
    'definition' => 'A project for whole genome analysis that can include assembly and annotation.',
    'cv_name' => 'local',
  ]);

}

/**
 * Adds the Systems Biology Ontology database and terms.
 */
function tripal_chado_populate_vocab_SBO() {
  chado_insert_db([
    'name' => 'SBO',
    'description' => 'Systems Biology.',
    'url' => 'http://www.ebi.ac.uk/sbo/main/',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'sbo',
    'Systems Biology.  Terms commonly used in Systems Biology, and in particular in computational modeling.'
  );

  $dbxref = chado_insert_cvterm([
    'id' => 'SBO:0000358',
    'name' => 'phenotype',
    'cv_name' => 'sbo',
    'definition' => 'A biochemical network can generate phenotypes or affects biological processes. Such processes can take place at different levels and are independent of the biochemical network itself.',
  ]);

  $dbxref = chado_insert_cvterm([
    'id' => 'SBO:0000554',
    'name' => 'database cross reference',
    'cv_name' => 'sbo',
    'definition' => 'An annotation which directs one to information contained within a database.',
  ]);

  $relationship = chado_insert_cvterm([
    'id' => 'SBO:0000374',
    'name' => 'relationship',
    'cv_name' => 'sbo',
    'definition' => 'Connectedness between entities and/or interactions representing their relatedness or influence.',
  ]);
}

/**
 * Adds the "Bioinformatics operations, data types, formats, identifiers and
 * topics" database and terms.
 */
function tripal_chado_populate_vocab_SWO() {
  chado_insert_db([
    'name' => 'SWO',
    'description' => 'Bioinformatics operations, data types, formats, identifiers and topics',
    'url' => 'http://purl.obolibrary.org/obo/swo',
    'urlprefix' => 'http://www.ebi.ac.uk/swo/{db}_{accession}',
  ]);
  chado_insert_cv('swo', 'Bioinformatics operations, data types, formats, identifiers and topics.');

  $term = chado_insert_cvterm([
    'id' => 'SWO:0000001',
    'name' => 'software',
    'cv_name' => 'swo',
    'definition' => 'Computer software, or generally just software, is any ' .
      'set of machine-readable instructions (most often in the form of a ' .
      'computer program) that conform to a given syntax (sometimes ' .
      'referred to as a language) that is interpretable by a given ' .
      'processor and that directs a computer\'s processor to perform ' .
      'specific operations.',
  ]);
  chado_associate_semweb_term('analysis', 'program', $term);
  chado_associate_semweb_term('protocol', 'softwaredescription', $term);
}

/**
 * Adds the contact table mapping.
 */
function tripal_chado_populate_vocab_TCONTACT() {
  chado_insert_db([
    'name' => 'TCONTACT',
    'description' => 'Tripal Contact Ontology. A temporary ontology until a more formal appropriate ontology an be identified.',
    'url' => 'cv/lookup/TCONTACT',
    'urlprefix' => 'cv/lookup/TCONTACT/{accession}',
  ]);
  chado_insert_cv('tripal_contact', 'Tripal Contact Ontology. A temporary ontology until a more formal appropriate ontology an be identified.');
}

/**
 * Adds the pub table mappings.
 */
function tripal_chado_populate_vocab_TPUB() {

  chado_insert_db([
    'name' => 'TPUB',
    'description' => 'Tripal Publication Ontology. A temporary ontology until a more formal appropriate ontology an be identified.',
    'url' => 'cv/lookup/TPUB',
    'urlprefix' => 'cv/lookup/TPUB/{accession}',
  ]);
  chado_insert_cv('tripal_pub', 'Tripal Publication Ontology. A temporary ontology until a more formal appropriate ontology an be identified.');

  // make sure we have our supported databases
  chado_insert_db(
    [
      'name' => 'PMID',
      'description' => 'PubMed',
      'url' => 'http://www.ncbi.nlm.nih.gov/pubmed',
      'urlprefix' => 'http://www.ncbi.nlm.nih.gov/pubmed/{accession}',
    ],
    ['update_existing' => TRUE]
  );
  chado_insert_db(
    [
      'name' => 'AGL',
      'description' => 'USDA National Agricultural Library',
      'url' => 'http://agricola.nal.usda.gov/',
    ],
    ['update_existing' => TRUE]
  );
  $term = chado_get_cvterm(['id' => 'TPUB:0000039']);
  chado_associate_semweb_term('pub', 'title', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000243']);
  chado_associate_semweb_term('pub', 'volumetitle', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000042']);
  chado_associate_semweb_term('pub', 'volume', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000256']);
  chado_associate_semweb_term('pub', 'series_name', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000043']);
  chado_associate_semweb_term('pub', 'issue', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000059']);
  chado_associate_semweb_term('pub', 'pyear', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000044']);
  chado_associate_semweb_term('pub', 'pages', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000244']);
  chado_associate_semweb_term('pub', 'publisher', $term);

  $term = chado_get_cvterm(['id' => 'TPUB:0000245']);
  chado_associate_semweb_term('pub', 'pubplace', $term);
}

/**
 * Adds the Uni Ontology database, terms and mappings.
 */
function tripal_chado_populate_vocab_UO() {
  chado_insert_db([
    'name' => 'UO',
    'description' => 'Units of Measurement Ontology',
    'url' => 'http://purl.obolibrary.org/obo/uo',
    'urlprefix' => 'http://purl.obolibrary.org/obo/TAXRANK_',
  ]);
  chado_insert_cv('uo', 'Units of Measurement Ontology');

  $term = chado_insert_cvterm([
    'id' => 'UO:0000000',
    'name' => 'unit',
    'cv_name' => 'uo',
    'description' => 'A unit of measurement is a standardized quantity of a physical quality.',
  ]);
  chado_associate_semweb_term('featuremap', 'unittype_id', $term);
}

/**
 * Adds the Taxonomic Rank Ontology database and terms.
 */
function tripal_chado_populate_vocab_TAXRANK() {

  chado_insert_db([
    'name' => 'TAXRANK',
    'description' => 'A vocabulary of taxonomic ranks (species, family, phylum, etc)',
    'url' => 'http://www.obofoundry.org/ontology/taxrank.html',
    'urlprefix' => 'http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv('taxonomic_rank', 'A vocabulary of taxonomic ranks (species, family, phylum, etc)');

  $term = chado_get_cvterm(['id' => 'TAXRANK:0000005']);
  chado_associate_semweb_term('organism', 'genus', $term);

  $term = chado_get_cvterm(['id' => 'TAXRANK:0000006']);
  chado_associate_semweb_term('organism', 'species', $term);

  $term = chado_get_cvterm(['id' => 'TAXRANK:0000045']);
  chado_associate_semweb_term('organism', 'infraspecific_name', $term);
}

/**
 * Adds the NCIT vocabulary database and terms.
 */
function tripal_chado_populate_vocab_NCIT() {
  chado_insert_db([
    'name' => 'NCIT',
    'description' => 'NCI Thesaurus OBO Edition.',
    'url' => 'http://purl.obolibrary.org/obo/ncit.owl',
    'urlprefix' => ' http://purl.obolibrary.org/obo/{db}_{accession}',
  ]);
  chado_insert_cv(
    'ncit',
    'The NCIt OBO Edition project aims to increase integration of the NCIt with OBO Library ontologies. NCIt is a reference terminology that includes broad coverage of the cancer domain, including cancer related diseases, findings and abnormalities. NCIt OBO Edition releases should be considered experimental.'
  );

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C25164',
    'name' => 'Date',
    'cv_name' => 'ncit',
    'definition' => 'The particular day, month and year an event has happened or will happen.',
  ]);
  chado_associate_semweb_term('assay', 'assaydate', $term);
  chado_associate_semweb_term('acquisition', 'acquisitiondate', $term);
  chado_associate_semweb_term('quantification', 'quantificationdate', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C48036',
    'name' => 'Operator',
    'cv_name' => 'ncit',
    'definition' => 'A person that operates some apparatus or machine',
  ]);
  chado_associate_semweb_term(NULL, 'operator_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C45378',
    'name' => 'Technology Platform',
    'cv_name' => 'ncit',
    'definition' => 'The specific version (manufacturer, model, etc.) of a technology that is used to carry out a laboratory or computational experiment.',
  ]);
  chado_associate_semweb_term('arraydesign', 'platformtype_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C25712',
    'name' => 'Value',
    'cv_name' => 'ncit',
    'definition' => 'A numerical quantity measured or assigned or computed.',
  ]);
  chado_associate_semweb_term(NULL, 'value', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C44170',
    'name' => 'Channel',
    'cv_name' => 'ncit',
    'definition' => 'An independent acquisition scheme, i.e., a route or conduit through which flows data consisting of one particular measurement using one particular parameter.',
  ]);
  chado_associate_semweb_term(NULL, 'channel_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C48697',
    'name' => 'Controlled Vocabulary',
    'cv_name' => 'ncit',
    'definition' => 'A set of terms that are selected and defined based on the requirements set out by the user group, usually a set of vocabulary is chosen to promote consistency across data collection projects. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'cv_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C45559',
    'name' => 'Term',
    'cv_name' => 'ncit',
    'definition' => 'A word or expression used for some particular thing. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'cvterm_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C80488',
    'name' => 'Expression',
    'cv_name' => 'ncit',
    'definition' => 'A combination of symbols that represents a value. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'expression_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C16977',
    'name' => 'Phenotype',
    'cv_name' => 'ncit',
    'definition' => 'The assemblage of traits or outward appearance of an individual. It is the product of interactions between genes and between genes and the environment. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'phenotype_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C16631',
    'name' => 'Genotype',
    'cv_name' => 'ncit',
    'definition' => 'The genetic constitution of an organism or cell, as distinct from its expressed features or phenotype. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'genotype_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C25341',
    'name' => 'Location',
    'cv_name' => 'ncit',
    'definition' => 'A position, site, or point in space where something can be found. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'nd_geolocation_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C802',
    'name' => 'Reagent',
    'cv_name' => 'ncit',
    'definition' => 'Any natural or synthetic substance used in a chemical or biological reaction in order to produce, identify, or measure another substance. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'nd_reagent_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C16551',
    'name' => 'Environment',
    'cv_name' => 'ncit',
    'definition' => 'The totality of surrounding conditions. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'environment_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C42765',
    'name' => 'Tree Node',
    'cv_name' => 'ncit',
    'definition' => 'A term that refers to any individual item or entity in a hierarchy or pedigree. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'phylonode_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C15320',
    'name' => 'Study Design',
    'cv_name' => 'ncit',
    'definition' => 'A plan detailing how a study will be performed in order to represent the phenomenon under examination, to answer the research questions that have been asked, and defining the methods of data analysis. Study design is driven by research hypothesis being posed, study subject/population/sample available, logistics/resources: technology, support, networking, collaborative support, etc. [ NCI ]',
  ]);
  chado_associate_semweb_term(NULL, 'studydesign_id', $term);

  // The Company term is missing for the Tripal Contact ontology, but is
  // useful for the arraydesign.manufacturer which is an FK to Contact.
  // It seems better to use a term from a curated ontology than to add to
  // Tripal Contact.
  $term = chado_insert_cvterm([
    'id' => 'NCIT:C54131',
    'name' => 'Company',
    'cv_name' => 'ncit',
    'definition' => 'Any formal business entity for profit, which may be a corporation, a partnership, association or individual proprietorship.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C47885',
    'name' => 'Project',
    'cv_name' => 'ncit',
    'definition' => 'Any specifically defined piece of work that is undertaken or attempted to meet a single requirement.',
  ]);
  chado_associate_semweb_term(NULL, 'project_id', $term);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C16223',
    'name' => 'DNA Library',
    'cv_name' => 'ncit',
    'definition' => 'A collection of DNA molecules that have been cloned in vectors.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C85496',
    'name' => 'Trait',
    'cv_name' => 'ncit',
    'definition' => 'Any genetically determined characteristic.',
  ]);

  $term = chado_insert_cvterm([
    'id' => 'NCIT:C25693',
    'name' => 'Subgroup',
    'cv_name' => 'ncit',
    'definition' => 'A subdivision of a larger group with members often exhibiting similar characteristics. [ NCI ]',
  ]);

}

/**
 * Adds the NCBI Taxon vocabulary database and terms.
 */
function tripal_chado_populate_vocab_NCBITAXON() {
  chado_insert_db([
    'name' => 'NCBITaxon',
    'description' => 'NCBI organismal classification.',
    'url' => 'http://www.berkeleybop.org/ontologies/ncbitaxon/',
    'urlprefix' => 'https://www.ncbi.nlm.nih.gov/Taxonomy/Browser/wwwtax.cgi?id={accession}',
  ]);
  chado_insert_cv(
    'ncbitaxon',
    'NCBI organismal classification. An ontology representation of the NCBI organismal taxonomy.'
  );

  $term = chado_insert_cvterm([
    'id' => 'NCBITaxon:common_name',
    'name' => 'common name',
    'cv_name' => 'ncbitaxon',
  ]);
  chado_associate_semweb_term('organism', 'common_name', $term);
}

/**
 * Adds defaults to the chado_semweb table.
 */
function tripal_chado_semweb_form($form, &$form_state, $chado_table = NULL) {
  if (array_key_exists('values', $form_state)) {
    $chado_table = $form_state['values']['chado_table'];
  }

  $chado_tables = chado_get_table_names(TRUE);
  $chado_tables = array_merge(['Select a Chado Table'], $chado_tables);
  // Make sure the table name exists. If not, reset it.
  $chado_table = in_array($chado_table, $chado_tables) ? $chado_table : NULL;

  $form['chado_table'] = [
    '#type' => 'select',
    '#title' => 'Chado Table',
    '#description' => t('Select a chado table to set vocabulary terms used for its columns.'),
    '#options' => $chado_tables,
    '#default_value' => $chado_table,
    '#ajax' => [
      'callback' => "tripal_chado_semweb_form_ajax_callback",
      'wrapper' => "tripal-chado-semweb-form",
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];

  // If the user has selected a chado table, then we need to
  // show the columns for setting terms.
  if ($chado_table) {
    $schema = chado_get_schema($chado_table);
    $pk = $schema['primary key'][0];
    //     $cv_default = db_select('tripal_cv_defaults', 'tc')
    //       ->fields('tc', array('field_name'))
    //       ->condition('table_name', $chado_table)
    //       ->execute()
    //       ->fetchField();
    $columns = $schema['fields'];
    $headers = [
      'Field Name',
      'Vocabulary',
      [
        'data' => 'Name',
        'nowrap' => TRUE,
      ],
      'Term',
      'Term Description',
      'Action',
    ];
    $rows = [];
    foreach ($columns AS $column => $detail) {
      // Do not show column if it's the primary key or default cv
      if ($column != $pk && $column != $cv_default) {
        $cvterm_id =
          db_select('chado_semweb', 'cs')
            ->fields('cs', ['cvterm_id'])
            ->condition('chado_table', $chado_table)
            ->condition('chado_column', $column)
            ->execute()
            ->fetchField();
        $sw_voc = '';
        $sw_term = '';
        $sw_desc = '';
        $sw_accession = '';
        if ($cvterm_id) {
          $term = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
          $term = chado_expand_var($term, 'field', 'cvterm.definition');
          $sw_voc = $term->cv_id->name;
          $sw_term = $term->name;
          $sw_accession = l($term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession,
            'cv/lookup/' . $term->dbxref_id->db_id->name . '/' . $term->dbxref_id->accession,
            ['attributes' => ['target' => '_blank']]);
          $sw_desc = $term->definition;
        }
        $rows[] = [
          $column,
          $sw_voc,
          $sw_term,
          $sw_accession,
          $sw_desc,
          [
            'data' => l('Edit', '/admin/tripal/storage/chado/semweb/edit/' . $chado_table . '/' . $column) . ' | ' .
              l('Reset', '/admin/tripal/storage/chado/semweb/reset/' . $chado_table . '/' . $column),
            'nowrap' => TRUE,
          ],
        ];
      }
    }
    $output = theme('table', [
      'header' => $headers,
      'rows' => $rows,
    ]);
    $form['table'] = [
      '#markup' => $output,
    ];
  }

  $form['#prefix'] = '<div id="tripal-chado-semweb-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Implements hook_form()
 * Edit terms used by the semantic web
 *
 * @param $form
 * @param $form_state
 * @param $table
 * @param $column
 *
 * @return $form
 */
function tripal_chado_semweb_edit_form($form, &$form_state, $table = NULL, $column = NULL) {

  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

  $form['chado_table'] = [
    '#markup' => 'Term used for the <strong>' . t($column) . '</strong> column of the chado <strong>' . t($table) . '</strong> table:',
  ];
  $form['table_name'] = [
    '#type' => 'value',
    '#value' => $table,
  ];
  $form['column'] = [
    '#type' => 'value',
    '#value' => $column,
  ];
  // If no term has been selected yet then provide the auto complete field.
  $form['term_name'] = [
    '#title' => t('Term'),
    '#type' => 'textfield',
    '#description' => t("The content type must be the name of a term in
        a controlled vocabulary and the controlled vocabulary should
        already be loaded into Tripal.  For example, to create a content
        type for storing 'genes', use the 'gene' term from the
        Sequence Ontology (SO)."),
    '#required' => TRUE,
    '#default_value' => $term_name,
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
  ];
  $form['select_button'] = [
    '#type' => 'button',
    '#value' => t('Lookup Term'),
    '#name' => 'select_cvterm',
    '#ajax' => [
      'callback' => "tripal_chado_semweb_form_ajax_callback",
      'wrapper' => "tripal-chado-semweb-edit-form",
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];
  if ($term_name) {
    $form['terms_list'] = [
      '#type' => 'fieldset',
      '#title' => t('Matching Terms'),
      '#description' => t('Please select the term the best matches the
          content type you want to create. If the same term exists in
          multiple vocabularies you will see more than one option below.'),
    ];
    $match = [
      'name' => $term_name,
    ];
    $terms = chado_generate_var('cvterm', $match, ['return_array' => TRUE]);
    $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
    $num_terms = 0;
    foreach ($terms as $term) {
      // Save the user a click by setting the default value as 1 if there's
      // only one matching term.
      $default = FALSE;
      $attrs = [];
      if ($num_terms == 0 and count($terms) == 1) {
        $default = TRUE;
        $attrs = ['checked' => 'checked'];
      }
      $form['terms_list']['term-' . $term->cvterm_id] = [
        '#type' => 'checkbox',
        '#title' => $term->name,
        '#default_value' => $default,
        '#attributes' => $attrs,
        '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
          '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
          '<br><b>Definition:</b>  ' . $term->definition,
      ];
      $num_terms++;
    }
    if ($num_terms == 0) {
      $form['terms_list']['none'] = [
        '#type' => 'item',
        '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>',
      ];
    }
    // Add in the button for the cases of no terms or too many.
    $form['submit_button'] = [
      '#type' => 'submit',
      '#value' => t('Use this term'),
      '#name' => 'use_cvterm',
    ];
  }

  $form['cancel_button'] = [
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#name' => 'cancel_button',
    '#limit_validation_errors' => [],
  ];

  $form['#prefix'] = '<div id = "tripal-chado-semweb-edit-form">';
  $form['#suffix'] = '</div>';

  return $form;
}


/**
 * Implements hook_form_validate()
 *
 * Validate function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_edit_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state)) {
    if ($form_state['clicked_button']['#name'] == 'use_cvterm') {
      $cvterm_id = NULL;
      // Make sure we have a cvterm selected
      $num_selected = 0;
      foreach ($form_state['values'] as $key => $value) {
        $matches = [];
        if (preg_match("/^term-(\d+)$/", $key, $matches) and
          $form_state['values']['term-' . $matches[1]]) {
          $cvterm_id = $matches[1];
          $num_selected++;
        }
      }
      if ($num_selected == 0) {
        form_set_error('', 'Please select at least one term.');
      }
      else {
        if ($num_selected > 1) {
          form_set_error('term-' . $cvterm_id, 'Please select only one term from the list below.');
        }
        else {
          $form_state['values']['#selected_cvterm_id'] = $cvterm_id;
        }
      }
    }
    else {
      if ($form_state['clicked_button']['#name'] == 'cancel_button') {
        $table_name = $form_state['values']['table_name'];
        drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
      }
    }
  }
}

/**
 * Implements hook_form_submit()
 *
 * Submit function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_edit_form_submit($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state) && $form_state['clicked_button']['#name'] == 'use_cvterm') {
    $table_name = $form_state['values']['table_name'];
    $column = $form_state['values']['column'];
    $cvterm_id = $form_state['values']['#selected_cvterm_id'];
    // Check if there is already a record
    $record_id =
      db_select('chado_semweb', 'cs')
        ->fields('cs', ['chado_semweb_id'])
        ->condition('chado_table', $table_name)
        ->condition('chado_column', $column)
        ->execute()
        ->fetchField();

    // If the record exists, update it
    if ($record_id) {
      db_update('chado_semweb')
        ->fields([
          'cvterm_id' => $cvterm_id,
        ])
        ->condition('chado_semweb_id', $record_id)
        ->execute();
    }
    // Otherwise, insert a new record
    else {
      db_insert('chado_semweb')
        ->fields([
          'chado_table' => $table_name,
          'chado_column' => $column,
          'cvterm_id' => $cvterm_id,
        ])
        ->execute();
    }
    drupal_set_message('The term settings have been saved.');
    drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
  }
}


/**
 * Implements hook_form()
 * Reset term used by semantic web
 *
 * @param $form
 * @param $form_state
 * @param $table
 * @param $column
 *
 * @return $form
 */
function tripal_chado_semweb_reset_form($form, &$form_state, $table = NULL, $column = NULL) {

  $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

  $form['chado_table'] = [
    '#markup' => 'Are you sure you want to remove the use of this term? ',
  ];
  $form['table_name'] = [
    '#type' => 'value',
    '#value' => $table,
  ];
  $form['column'] = [
    '#type' => 'value',
    '#value' => $column,
  ];

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#name' => 'reset_term',
  ];

  $form['cancel_button'] = [
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#name' => 'cancel_button',
    '#limit_validation_errors' => [],
  ];

  return $form;
}

/**
 * Implements hook_form_validate()
 *
 * Validate function for resetting the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_reset_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state)) {
    if ($form_state['clicked_button']['#name'] == 'use_cvterm') {
      $cvterm_id = NULL;
      // Make sure we have a cvterm selected
      $num_selected = 0;
      foreach ($form_state['values'] as $key => $value) {
        $matches = [];
        if (preg_match("/^term-(\d+)$/", $key, $matches) and
          $form_state['values']['term-' . $matches[1]]) {
          $cvterm_id = $matches[1];
          $num_selected++;
        }
      }
      if ($num_selected == 0) {
        form_set_error('', 'Please select at least one term.');
      }
      else {
        if ($num_selected > 1) {
          form_set_error('term-' . $cvterm_id, 'Please select only one term from the list below.');
        }
        else {
          $form_state['values']['#selected_cvterm_id'] = $cvterm_id;
        }
      }
    }
    else {
      if ($form_state['clicked_button']['#name'] == 'cancel_button') {
        $table_name = $form_state['values']['table_name'];
        drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
      }
    }
  }
}

/**
 * Implements hook_form_submit()
 *
 * Submit function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_chado_semweb_reset_form_submit($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state) && $form_state['clicked_button']['#name'] == 'reset_term') {
    $table_name = $form_state['values']['table_name'];
    $column = $form_state['values']['column'];
    // Check if there is already a record
    $record_id =
      db_select('chado_semweb', 'cs')
        ->fields('cs', ['chado_semweb_id'])
        ->condition('chado_table', $table_name)
        ->condition('chado_column', $column)
        ->execute()
        ->fetchField();

    // If the record exists, reset it
    if ($record_id) {
      db_update('chado_semweb')
        ->fields([
          'cvterm_id' => NULL,
        ])
        ->condition('chado_semweb_id', $record_id)
        ->execute();
    }
    drupal_set_message('The term settings have been reset.');
    drupal_goto('/admin/tripal/storage/chado/semweb/' . $table_name);
  }
}

/**
 *
 */
function tripal_chado_semweb_form_ajax_callback($form, $form_state) {
  return $form;
}
