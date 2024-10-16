INSERT INTO db VALUES (2, 'CO_010', 'Crop Germplasm Ontology', 'http://www.cropontology.org/terms/CO_010:{accession}', 'http://www.cropontology.org/get-ontology/CO_010');
INSERT INTO db VALUES (11, 'OBI', 'The Ontology for Biomedical Investigation', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'http://obi-ontology.org/page/Main_Page');
INSERT INTO db VALUES (3, 'dc', 'DCMI Metadata Terms', 'http://purl.org/dc/terms/{accession}', 'http://purl.org/dc/dcmitype/');
INSERT INTO db VALUES (17, 'PMID', 'PubMed', 'http://www.ncbi.nlm.nih.gov/pubmed/{accession}', 'http://www.ncbi.nlm.nih.gov/pubmed');
INSERT INTO db VALUES (12, 'OGI', 'Ontology for genetic interval', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'http://purl.bioontology.org/ontology/OGI');
INSERT INTO db VALUES (33, 'SIO', 'Semanticscience Integrated Ontology', 'http://semanticscience.org/resource/{db}_{accession}', 'http://sio.semanticscience.org/');
INSERT INTO db VALUES (26, 'TCONTACT', 'Tripal Contact Ontology. A temporary ontology until a more formal appropriate ontology can be identified.', 'cv/lookup/TCONTACT/{accession}  ', 'cv/lookup/TCONTACT');
INSERT INTO db VALUES (13, 'IAO', 'Information Artifact Ontology', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'https://github.com/information-artifact-ontology/IAO/');
INSERT INTO db VALUES (4, 'data', 'Bioinformatics operations, data types, formats, identifiers and topics.', 'http://edamontology.org/{db}_{accession}', 'http://edamontology.org/page');
INSERT INTO db VALUES (18, 'UO', 'Units of Measurement Ontology', 'http://purl.obolibrary.org/obo/UO_{accession}', 'http://purl.obolibrary.org/obo/uo');
INSERT INTO db VALUES (5, 'format', 'A defined way or layout of representing and structuring data in a computer file, blob, string, message, or elsewhere. The main focus in EDAM lies on formats as means of structuring data exchanged between different tools or resources.', 'http://edamontology.org/{db}_{accession}', 'http://edamontology.org/page');
INSERT INTO db VALUES (6, 'operation', 'A function that processes a set of inputs and results in a set of outputs, or associates arguments (inputs) with values (outputs). Special cases are: a) An operation that consumes no input (has no input arguments).', 'http://edamontology.org/{db}_{accession}', 'http://edamontology.org/page');
INSERT INTO db VALUES (23, 'GO', 'The Gene Ontology (GO) knowledgebase is the world’s largest source of information on the functions of genes', 'http://amigo.geneontology.org/amigo/term/{db}:{accession}', 'http://geneontology.org/');
INSERT INTO db VALUES (7, 'topic', 'A category denoting a rather broad domain or field of interest, of study, application, work, data, or technology. Topics have no clearly defined borders between each other.', 'http://edamontology.org/{db}_{accession}', 'http://edamontology.org/page');
--- INSERT INTO db VALUES (1, 'null', 'No database', 'cv/lookup/{db}/{accession}', 'cv/lookup/null');
INSERT INTO db VALUES (8, 'EFO', 'Experimental Factor Ontology', 'http://www.ebi.ac.uk/efo/{db}_{accession}', 'http://www.ebi.ac.uk/efo/efo.owl');
INSERT INTO db VALUES (19, 'NCIT', 'The NCIt is a reference terminology that includes broad coverage of the cancer domain, including cancer related diseases, findings and abnormalities. NCIt OBO Edition releases should be considered experimental.', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'http://purl.obolibrary.org/obo/ncit.owl');
INSERT INTO db VALUES (9, 'ERO', 'The Eagle-I Research Resource Ontology', 'http://purl.bioontology.org/ontology/ERO/{db}:{accession}', 'http://purl.bioontology.org/ontology/ERO');
INSERT INTO db VALUES (30, 'rdf', 'Resource Description Framework', 'http://www.w3.org/1999/02/22-rdf-syntax-ns#', 'http://www.w3.org/1999/02/22-rdf-syntax-ns');
INSERT INTO db VALUES (24, 'SO', 'The Sequence Ontology', 'http://www.sequenceontology.org/browser/current_svn/term/{db}:{accession}', 'http://www.sequenceontology.org');
INSERT INTO db VALUES (10, 'OBCS', 'Ontology of Biological and Clinical Statistics', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'https://github.com/obcs/obcs');
INSERT INTO db VALUES (20, 'NCBITaxon', 'NCBI organismal classification', 'https://www.ncbi.nlm.nih.gov/Taxonomy/Browser/wwwtax.cgi?id={accession}', 'http://www.berkeleybop.org/ontologies/ncbitaxon/');
INSERT INTO db VALUES (27, 'TPUB', 'Tripal Publication Ontology. A temporary ontology until a more formal appropriate ontology can be identified.', 'cv/lookup/TPUB/{accession}', 'cv/lookup/TPUB');
INSERT INTO db VALUES (21, 'rdfs', 'Resource Description Framework Schema', 'http://www.w3.org/2000/01/rdf-schema#{accession}', 'https://www.w3.org/TR/rdf-schema/');
INSERT INTO db VALUES (25, 'TAXRANK', 'A vocabulary of taxonomic ranks (species, family, phylum, etc)', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'http://www.obofoundry.org/ontology/taxrank.html');
INSERT INTO db VALUES (14, 'local', 'Terms created for this site', 'cv/lookup/{db}/{accession}', 'cv/lookup/local');
INSERT INTO db VALUES (22, 'RO', 'Relationship Ontology (legacy)', 'cv/lookup/RO/{accession}    ', 'cv/lookup/RO');
INSERT INTO db VALUES (15, 'SBO', 'Systems Biology Ontology', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'http://www.ebi.ac.uk/sbo/main/');
INSERT INTO db VALUES (28, 'foaf', 'Friend of a Friend', 'http://xmlns.com/foaf/spec/#', 'http://www.foaf-project.org/');
INSERT INTO db VALUES (31, 'schema', 'Schema.org', 'https://schema.org/{accession}', 'https://schema.org/');
INSERT INTO db VALUES (16, 'SWO', 'Bioinformatics operations, data types, formats, identifiers and topics', 'http://www.ebi.ac.uk/swo/{db}_{accession}', 'http://purl.obolibrary.org/obo/swo');
INSERT INTO db VALUES (29, 'hydra', 'A Vocabulary for Hypermedia-Driven Web APIs', 'http://www.w3.org/ns/hydra/core#{accession}', 'http://www.w3.org/ns/hydra/core');
INSERT INTO db VALUES (32, 'sep', 'Sample processing and separation techniques.', 'http://purl.obolibrary.org/obo/{db}_{accession}', 'http://psidev.info/index.php?q=node/312');
