<?php

// This Tripal_cv.owl_loader.php and OWLStanza.inc(CLASS) file is being developed to read and parse through any scientific
// ontology XML Owl file to have the vocabularies be inserted into the Chado database for the open source project
// Tripal.info to be used.

/**
 * @file
 * @add file from header
 */
require_once('OWLStanza.inc');
/**
 * Parses an OWL XML file and imports the CV terms into Chado
 *
 * @param $filename The
 *          full path to the OWL XML file.
 *
 * @return No return value.
 *
 * @throws Exception
 */
function tripal_cv_parse_owl($filename) {
  // TODO: need to pass in the $db_name as the cv name into the function when you see the
  // oboInOwl:default-namespace that is not the same as $db_name.
  //
  // TODO: this all should occur inside of a transaction.

  // Opening the OWL file for parsing.
  $owl = new XMLReader();
  // This command will open and read the any Owl file.
  if (!$owl->open($filename)) {
    print "ERROR opening OWL file: '$filename'\n";
    exit();
  }

  // Get the RDF stanza. We pass FALSE as the second parameter to prevent
  // the object from reading the entire file into memory.
  $rdf = new OWLStanza($owl, FALSE);

  // Get the ontology stanza. It will contain the values for the database
  // name for this ontology.
  $ontology = new OWLStanza($owl);

  // Look for the db name in using the 'oboInOwl:default-namespace term. If it's
  // not present then we'll use the 'about' element to get the namespace.
  $namespace = $ontology->getChild('oboInOwl:default-namespace');
  if ($namespace) {
    $db_name = $namespace->getValue();
  }
  else {
    // Insert the database record into Chado using the owl:Ontology stanza of the owl file.
    $about = $ontology->getAttribute('rdf:about');

    // We wrote the regular expression on the rdf.about line to get the database name for any particular Owl file.
    if (preg_match('/^.*\/(.*?)\..*$/', $about, $matches)) {
      $db_name = strtoupper($matches[1]);
    }
  }

  //
  // Step 1: Make sure that all dependencies (database names) are met for each Owl ontology file.
  //

  // loop through each stanza, one at a time, and handle each one
  // based on the tag name.
  $stanza = new OWLStanza($owl);

  // Set an empty array for the dependencies to go in.
  $deps = [
    'db' => [],
    'dbxref' => [],
  ];

  // Start looping and parsing through the owl:Class stanza section of the Owl file.
  while (!$stanza->isFinished()) {
    // Use the tag name from OWLStanza.inc to identify which function should be called.
    switch ($stanza->getTagName()) {
      case 'owl:Class':
        tripal_owl_check_class_depedencies($stanza, $vocab_db_name, $deps);
        break;
    }
    // Get to the next stanza in the OWL file.
    $stanza = new OWLStanza($owl);
  }

  if (count(array_keys($deps['db'])) > 0 or count(array_keys($deps['dbxref'])) > 0) {
    // We have unmet dependencies. Print those out and return.
    // The deps array will have DB’s, then terms' . "\n");
    if (count($deps['db']) > 0) {
      drupal_set_message('Cannot import ontology, "' . $db_name . '", as the following ' . 'dependent vocabularies must first be imported: ' . print_r(array_keys($deps['db']), TRUE) . '\n', 'error');
    }
    if (count($deps['dbxref']) > 0) {
      drupal_set_message('Cannot import ontology, "' . $db_name . '", as the following ' . 'dependent terms must first be imported: ' . print_r(array_keys($deps['dbxref']), TRUE) . '\n', 'error');
    }
    return;
  }

  //
  // Step 2: If we pass the dependency check in step 1 then we can insert
  // the terms.
  //

  // Holds an array of CV and DB records that have already been
  // inserted (reduces number of queires).
  $vocabs = [
    'db' => [],
    'cv' => [],
    'this' => [],
  ];

  // Reload the ontology to reposition at the beginning of the OWl file for inserting the
  // new terms into Chado.

  $owl = new XMLReader();
  if (!$owl->open($filename)) {
    print "ERROR opening OWL file: '$filename'\n";
    exit();
  }
  $rdf = new OWLStanza($owl, FALSE);
  $ontology = new OWLStanza($owl);

  // Insert the database record into Chado using the
  // owl:Ontology stanza.
  $url = '';
  $homepage = $ontology->getChild('foaf:homepage');
  if ($homepage) {
    $url = $homepage->getValue();
  }
  $db = [
    'url' => $url,
    'name' => $db_name,
  ];

  // Using the Tripal API function to insert the term into the Chado database.
  $db = chado_insert_db($db);

  // Get the description for this vocabulary. This should be in the
  // dc:description element. If that element is missing then the
  // description should default to the empty string.
  $cv_description = '';
  $description = $ontology->getChild('dc:description');
  if ($description) {
    $cv_description = $description->getValue();
  }

  // Get the name for the CV. This should be in the 'dc:title' element. If the
  // title is not present then the cv name should default to the database name.
  $cv_name == $namespace = $ontology->getChild('oboInOwl:default-namespace');
  if ($namespace) {
    $cv_name = $namespace->getValue();
  }

  $title = $ontology->getChild('dc:title');
  if ($title) {
    $cv_name = preg_replace("/[^\w]/", "_", strtolower($title->getValue()));
  }

  // Insert the CV record into Chado.
  $cv = chado_insert_cv($cv_name, $cv_description);

  // Add this CV and DB to our vocabs array so we can reuse it later.
  $vocabs[$db_name]['cv'] = $namespace_cv;
  $vocabs[$db_name]['db'] = $db;
  $vocabs['this'] = $db_name;

  // loop through each stanza of the owl file, one at a time, and handle each one
  // based on the tag name from the OWLStanza.inc file.
  $stanza = new OWLStanza($owl);
  while (!$stanza->isFinished()) {

    // Use the tag name to identify which function should be called.
    switch ($stanza->getTagName()) {
      case 'owl:AnnotationProperty':
        // tripal_owl_handle_annotation_property($stanza, $vocabs);
        break;
      case 'rdf:Description':
        // tripal_owl_handle_description($stanza, $vocabs);
        break;
      case 'owl:ObjectProperty':
        // tripal_owl_handle_object_property($stanza, $vocabs);
        break;
      case 'owl:Class':
        tripal_owl_handle_class($stanza, $vocabs);
        break;
      case 'owl:Axiom':
        break;
      case 'owl:Restriction':
        break;
      default:
        throw new Exception("Unhandled stanza: " . $stanza->getTagName());
        exit();
        break;
    }

    // Get the next stanza in the OWL file.
    $stanza = new OWLStanza($owl);
  }

  // Close the XMLReader $owl object.
  $owl->close();
}

/**
 * Checks for required vocabularies that are not loaded into Chado.
 *
 * Some vocabularies use terms from other ontologies. If this is happens
 * we need to ensure that the dependent vocabularies are present in the
 * database prior to loading this one. This function adds to the $deps
 * array all of the database names and term accessions that are missing in
 * Chado.
 *
 * @param $stanza The
 *          OWLStanza object for the current stanza from the OWL file.
 * @param $vocab_db_name The
 *          name of the database for the vocabulary being loded.
 * @param $deps The
 *          dependencies array. The missing databases are provided in array
 *          using a 'db' key, and missing terms are in a second array using a
 *          'dbxref' key.
 */
function tripal_owl_check_class_depedencies(OWLStanza $stanza, $vocab_db_name, &$deps) {

  // Initialize the variables.
  $db_name = '';
  $accession = '';
  $db = NULL;

  // Get the DB name and accession from the "rdf:about" attribute.
  $about = $stanza->getAttribute('rdf:about');
  if (!$about) {
    // TODO: some owl:Class stanzas do not have an about. What are these?
    // how should we handle them.
    return;
  }

  // We wrote the regular expression on the rdf.about line to get the database
  // name and accession term for any particular Owl file.
  if (preg_match('/.*\/(.+)_(.+)/', $about, $matches)) {
    $db_name = strtoupper($matches[1]);
    $accession = $matches[2];
  }
  else {
    throw new Exception("owl:Class stanza 'rdf:about' attribute is not formated as expected: '$about'. " . "This is necessary to determine the term's accession: \n\n" . $stanza->getXML());
  }

  // If the database name for this term is the same as the vocabulary
  // we are trying to load, then don't include it in the $deps array.
  if ($db_name !== $vocab_db_name) {
    return;
  }

  // Check if the db_name does not exist in the chado.db table. If it
  // does not exist then add it to our $deps array. If the query fails then
  // throw an exception.
  $db = chado_select_record('db', [
    'db_id',
  ], [
    'name' => $db_name,
  ]);
  if ($db === FALSE) {
    throw new Exception("Failed to execute query to find vocabulary in chado.db table\n\n" . $stanza->getXML());
  }
  else {
    if (count($db) == 0) {
      $deps['db'][$db_name] = TRUE;

      // Does this stanza provide the URL for the OWL file of this missing
      // dependency. If so then add it to our deps array.
      $imported_from = $stanza->getChild('obo:IAO_0000412');

      if ($imported_from == NULL) {
        return;
      }
      $url = $imported_from->getAttribute('rdf:resource');
      if ($url) {
        $deps['db'][$db_name] = $url;
      }
      return;
    }
  }

  // If the db_name exists, then check if the accession exists in
  // the chado.dbxref table. If it doesn't exist then add an entry to the
  // $deps array. If the query fails then throw an exception.
  $values = [
    'db_id' => $db[0]->db_id,
    'accession' => $accession,
  ];

  $dbxref = chado_select_record('dbxref', [
    'dbxref_id',
    'db_id',
  ], $values);
  if ($dbxref === FALSE) {
    throw new Exception("Failed to execute query to find vocabulary term in chado.dbxref table\n\n" . $stanza->getXML());
  }
  elseif (count($accession) == 0) {
    $deps['dbxref'][$db_name . ':' . $accesson] = TRUE;
  }
  return;
}

/**
 *
 * @param
 *          $stanza
 * @param
 *          $vocabs
 *
 * @throws Exception
 */
function tripal_owl_handle_object_property($stanza, $vocabs) {
}

/**
 *
 * @param
 *          $stanza
 * @param
 *          $vocabs
 *
 * @throws Exception
 */
function tripal_owl_handle_annotation_property($stanza, $vocabs) {

  // $matches = array();
  // $db_name = '';
  // $accession = '';
  // $about = $stanza->getAttribute('rdf:about');
  // // Get the DB name and accession from the about attribute using the preg match function.
  //   if (preg_match('/.*\/(.+)_(.+)/', $about, $matches)) {
  //     $db_name = ($matches[1]);
  //     $accession = $matches[2];
  //   }
  //   else {
  //         throw new Exception("owl:Class stanza 'rdf:about' attribute is not formated as expected: '$about'. " . "This is necessary to determine the term's accession: \n\n" . $stanza->getXML());
  //       }

  //   // Insert a DB Record
  // if (array_key_exists($db_name, $vocabs)) {
  //     $db = $vocabs[$db_name]['db'];
  //     $default_namespace_cv = $vocabs[$db_name]['cv'];
  //   }
  //   else {
  //     // Unfortunately, all we have is the name. The OWL format
  //     // doesn't provides us the URL, description, etc.
  //     $values = array(
  //       'name' => $db_name
  //     );
  //     $db = chado_insert_db($values);
  // // Insert a dbxref record.
  //   $values = array(
  //     'db_id' => $db->db_id,
  //     'accession' => $accession
  //   );
  //   $dbxref = chado_insert_dbxref($values);

  //   $imported_from = $stanza->getChild('obo:IAO_0000114');
  //     if ($imported_from == NULL) {
  //       return;
  //     }
  //     $url = $imported_from->getAttribute('rdf:resource');
  //     if ($url) {
  //       $vocabs['db'][$db_name] = $url;
  //     }
  //     return;
  //   }

  // // Insert a new cvterm record.
  // $cvterm_name = '';
  // $definition = '';

  // $cvterm_name = $stanza->getChild('rdfs:label');
  //   if ($cvterm_name) {
  //   $cvterm_name = $stanza->getValue();
  //   }
  //   $definition = $stanza->getChild('obo:IAO_0000115');
  //   if ($definition) {
  //   $definition = $stanza->getValue();
  //   }

  //   $term = array(
  //   	'id' => $db->name .':'. $dbxref->accession,
  //   	'name' => $cvterm_name,
  //   	'cv_name' => $cv->name,
  //   	'definition' => $definition,
  //   );
  //   $option =array();
  //   if ($vocabs['this'] != $db->name){
  //   	$option['update_existing'] = FALSE;
  //   }
  //   $cvterm = chado_insert_cvterm($term, $option);
  // }
}

/**
 *
 * @param
 *          $stanza
 * @param
 *          $vocabs
 *
 * @throws Exception
 */
function tripal_owl_handle_description($stanza, $vocabs) {
}

/**
 *
 * The function goes through owl:Class stanza to insert new vocabularies.
 *
 * @param $stanza The
 *          OWLStanza object for the current stanza from the OWL file.
 * @param
 *          $vocabs
 *
 * @throws Exception
 */
function tripal_owl_handle_class(OWLStanza $stanza, $vocabs) {

  // Initialize the database and cv variables.
  $db_name = $vocabs['this'];
  $accession = '';
  $is_a = '';
  $namespace_cv = $vocabs[$db_name]['cv'];
  $db = $vocabs[$db_name]['db'];

  // Insert the dbxref record into Chado using the owl:Class stanza of the owl file.
  // Any oboInOwl:id supercedes what we find in the rdf:about in the owl file.
  $obo_id = $stanza->getChild('oboInOwl:id');

  if ($obo_id) {
    if (preg_match('/.*>(.+):(.+)<.*/', $about, $matches)) {
      $db_name = strtoupper($matches[1]);
      $accession = $matches[2];
    }
    else {
      $about = $stanza->getAttribute('rdf:about');
      // We wrote the regular expression on the rdf.about line to get the
      // db_name and accession for any particular Owl file.
      if (preg_match('/.*\/(.+)_(.+)/', $about, $matches)) {
        $db_name = strtoupper($matches[1]);
        $accession = $matches[2];
      }
      else {
        throw new Exception("owl:Class stanza 'rdf:about' attribute is not formated as expected: '$about'. " . "This is necessary to determine the term's accession: \n\n" . $stanza->getXML());
      }
    }
  }

  // If the database name for this term is the same as the vocabulary
  // we are trying to load, then do include it in the $vocabs array.
  if ($db_name == $vocabs['this']) {
    return;
  }

  // insert dbxref
  $values = [
    'db_id' => $db->db_id,
    'accession' => $accession,
  ];
  $dbxref = chado_insert_dbxref($values);

  $cvterm_name = $stanza->getChild('rdfs:label');
  if ($cvterm_name) {
    $cvterm_name = $stanza->getValue();
  }

  $definition = $stanza->getChild('obo:IAO_0000115');
  if ($definition) {
    $definition = $stanza->getValue();
  }

  $term = [
    'id' => $db->name . ':' . $dbxref->accession,
    'name' => $db->name,
    'cv_name' => $stanza->getValue(),
    'definition' => $stanza->getValue(),
  ];

  $options = [];
  if ($vocabs['this'] != $db->name) {
    $options['update_existing'] = FALSE;
  }
  $cvterm = chado_insert_cvterm($term, $options);

  // // Add a record to the chado relationship table if an ‘rdfs:subClassOf’ child exists.

  // $cvterm_name = $stanza->getChild('rdfs:subClassOf');

  // Insert a new cvterm record.
  // $cvterm_name = '';
  // $definition = '';
  // $cvterm_name = $stanza->getChild('rdfs:label');
  // if ($cvterm_name) {
  // $cvterm_name = $stanza->getValue();
  //}

  // $definition = $stanza->getChild('obo:IAO_0000115');
  // if ($definition) {
  //$definition = $stanza->getValue();
  //}


  // $term = array (
  // 'id' => $db->name . ':' . $dbxref->accession,
  // 'name' => $cvterm_name,
  // 'cv_name' => $cv->name,
  // 'definition' => $definition
  // );
  // $option = array ();
  // if ($vocabs['this'] != $db->name) {
  // $option['update_existing'] = FALSE;
  // }
  // $cvterm = chado_insert_cvterm($term, $option);
}
