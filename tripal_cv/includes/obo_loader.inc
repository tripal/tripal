<?php

/**
 * @file
 * Tripal Ontology Loader
 *
 * @defgroup tripal_obo_loader Tripal Ontology Loader
 * @ingroup tripal_cv
 */

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2_id($obo_id, $jobid = NULL) {

  // get the OBO reference
  $sql = "SELECT * FROM {tripal_cv_obo} WHERE obo_id = %d";
  $obo = db_fetch_object(db_query($sql, $obo_id));

  // if the reference is for a remote URL then run the URL processing function
  if (preg_match("/^http:\/\//", $obo->path) or preg_match("/^ftp:\/\//", $obo->path)) {
    tripal_cv_load_obo_v1_2_url($obo->name, $obo->path, $jobid, 0);
  }
  // if the reference is for a local file then run the file processing function
  else {
    // check to see if the file is located local to Drupal
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $obo->path;
    if (file_exists($dfile)) {
      tripal_cv_load_obo_v1_2_file($obo->name, $dfile , $jobid, 0);
    }
    // if not local to Drupal, the file must be someplace else, just use
    // the full path provided
    else {
      if (file_exists($obo->path)) {
        tripal_cv_load_obo_v1_2_file($obo->name, $obo->path, $jobid, 0);
      }
      else {
        print "ERROR: counld not find OBO file: '$obo->path'\n";
      }
    }
  }
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2_file($obo_name, $file, $jobid = NULL, $is_new = TRUE) {
  $newcvs = array();

  tripal_cv_load_obo_v1_2($file, $jobid, $newcvs);
  if ($is_new) {
    tripal_cv_load_obo_add_ref($obo_name, $file);
  }

  // update the cvtermpath table
  tripal_cv_load_update_cvtermpath($newcvs, $jobid);
  print "Ontology Sucessfully loaded!\n";

}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2_url($obo_name, $url, $jobid = NULL, $is_new = TRUE) {

  $newcvs = array();

  // first download the OBO
  $temp = tempnam(sys_get_temp_dir(), 'obo_');
  print "Downloading URL $url, saving to $temp\n";
  $url_fh = fopen($url, "r");
  $obo_fh = fopen($temp, "w");
  if (!$url_fh) {
    tripal_cv_obo_quiterror("Unable to download the remote OBO file at $url. Could a firewall be blocking outgoing connections? ".
          " if you are unable to download the file you may manually downlod the OBO file and use the web interface to ".
          " specify the location of the file on your server.");

  }
  while (!feof($url_fh)) {
    fwrite($obo_fh, fread($url_fh, 255), 255);
  }
  fclose($url_fh);
  fclose($obo_fh);

  // second, parse the OBO
  tripal_cv_load_obo_v1_2($temp, $jobid, $newcvs);

  // now remove the temp file
  unlink($temp);

  if ($is_new) {
    tripal_cv_load_obo_add_ref($obo_name, $url);
  }

  // update the cvtermpath table
  tripal_cv_load_update_cvtermpath($newcvs, $jobid);

  print "Ontology Sucessfully loaded!\n";
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_update_cvtermpath($newcvs, $jobid) {

  print "\nUpdating cvtermpath table.  This may take a while...\n";
  foreach ($newcvs as $namespace => $cvid) {
    tripal_cv_update_cvtermpath($cvid, $jobid);
  }
}

/**
 * Add the obo to the tripal_cv_obo table in the Drupal database
 */
function tripal_cv_load_obo_add_ref($name, $path) {
  $isql = "INSERT INTO {tripal_cv_obo} (name,path) VALUES ('%s','%s')";
  db_query($isql, $name, $path);
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_load_obo_v1_2($file, $jobid = NULL, &$newcvs) {
  
  $header = array();
    
  // make sure our temporary table exists
  $ret = array(); 
  if (!db_table_exists('tripal_obo_temp')) { 
    $schema = tripal_cv_get_custom_tables('tripal_obo_temp');  
    $success = tripal_core_create_custom_table($ret, 'tripal_obo_temp', $schema['tripal_obo_temp']);
    if (!$success) {
      watchdog('T_obo_loader', "Cannot create temporary loading table", array(), WATCHDOG_ERROR); 
      return;
    } 
  }
  // empty the temp table
  $sql = "DELETE FROM tripal_obo_temp";
  chado_query($sql);

  // get a persistent connection
  $connection = tripal_db_persistent_chado();
  if (!$connection) {
     print "A persistant connection was not obtained. Loading will be slow\n";
  }
          
  // if we cannot get a connection then let the user know the loading will be slow
  tripal_db_start_transaction();
  if ($connection) {
     print "\nNOTE: Loading of this OBO file is performed using a database transaction. \n" .
           "If the load fails or is terminated prematurely then the entire set of \n" .
           "insertions/updates is rolled back and will not be found in the database\n\n";
  }

  print "Step 1: Preloading File $file\n";  

  // make sure we have an 'internal' and a '_global' database
  if (!tripal_db_add_db('internal')) {
    tripal_cv_obo_quiterror("Cannot add 'internal' database");
  }
  if (!tripal_db_add_db('_global')) {
    tripal_cv_obo_quiterror("Cannot add '_global' database");
  }

  // parse the obo file
  $default_db = tripal_cv_obo_parse($file, $header, $jobid);

  // add the CV for this ontology to the database
  $defaultcv = tripal_cv_add_cv($header['default-namespace'][0], '');
  if (!$defaultcv) {
    tripal_cv_obo_quiterror('Cannot add namespace ' . $header['default-namespace'][0]);
  }
  $newcvs[$header['default-namespace'][0]] = $defaultcv->cv_id;

  // add any typedefs to the vocabulary first
  $sql = "
    SELECT * FROM tripal_obo_temp
    WHERE type = 'Typedef' 
  ";
  $typedefs = chado_query($sql);
  while ($typedef = db_fetch_object($typedefs)) {
    $term = unserialize(base64_decode($typedef->stanza));
    tripal_cv_obo_process_term($term, $defaultcv->name, 1, $newcvs, $default_db);
  }

  // next add terms to the vocabulary
  print "\nStep 2: Loading terms...\n";  
  if (!tripal_cv_obo_process_terms($defaultcv->name, $jobid, $newcvs, $default_db)) {
    tripal_cv_obo_quiterror('Cannot add terms from this ontology');
  }

  // transaction is complete
  tripal_db_commit_transaction();
  return;
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_quiterror($message) {

  watchdog("T_obo_loader", $message, array(), WATCHDOG_ERROR);;
  exit;

}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_process_terms($defaultcv, $jobid = NULL, &$newcvs, $default_db) {

  $i = 0;

  // iterate through each term from the OBO file and add it
  $sql = "
    SELECT * FROM tripal_obo_temp
    WHERE type = 'Term' 
    ORDER BY id
  ";
  $terms = chado_query($sql);
  $count = pg_num_rows($terms);
  
  // calculate the interval for updates
  $interval = intval($count * 0.0001);
  if ($interval < 1) {
    $interval = 1;
  }
  while($t = db_fetch_object($terms)) {
    $term = unserialize(base64_decode($t->stanza));
    
    // update the job status every interval
    if ($jobid and $i % $interval == 0) {
      $complete = ($i / $count) * 50;
      tripal_job_set_progress($jobid + 50, intval($complete)); 
      printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 2, number_format(memory_get_usage()));                                                             
    }                                 
    
    // add/update this term
    if (!tripal_cv_obo_process_term($term, $defaultcv, 0, $newcvs, $default_db)) {
      tripal_cv_obo_quiterror("Failed to process terms from the ontology");
    }

    $i++;
  }
  
  // set the final status
  if ($jobid) {
    $complete = ($i / $count) * 50;
    tripal_job_set_progress($jobid + 50, intval($complete)); 
    printf("%d of %d records. (%0.2f%%) Memory: %s bytes\r", $i, $count, $complete * 2, number_format(memory_get_usage()));
  }                                                             
  
  return 1;
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_process_term($term, $defaultcv, $is_relationship = 0, &$newcvs, $default_db) {
 
  // construct the term array for sending to the tripal_cv_add_cvterm function
  // for adding a new cvterm
  $t = array(); 
  $t['id'] = $term['id'][0];
  $t['name'] = $term['name'][0];
  if (array_key_exists('def', $term)) {
    $t['def'] = $term['def'][0];
  }
  if (array_key_exists('subset', $term)) {
    $t['subset'] = $term['subset'][0];  
  }  
  if (array_key_exists('namespace', $term)) {
    $t['namespace'] = $term['namespace'][0];
  }
  if (array_key_exists('is_obsolete', $term)) {
    $t['is_obsolete'] = $term['is_obsolete'][0];
  } 
  
  // add the cvterm
  $cvterm = tripal_cv_add_cvterm($t, $defaultcv, $is_relationship, 1, $default_db); 
  if (!$cvterm) {
    tripal_cv_obo_quiterror("Cannot add the term " . $term['id']);
  }  
 
  if (array_key_exists('namespace', $term)) {
    $newcvs[$term['namespace'][0]] = $cvterm->cv_id;
  }
  
  // now handle other properites
  if (array_key_exists('is_anonymous', $term)) {
    //print "WARNING: unhandled tag: is_anonymous\n";
  }
  if (array_key_exists('alt_id', $term)) {
    foreach ($term['alt_id'] as $alt_id) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $alt_id)) {
        tripal_cv_obo_quiterror("Cannot add alternate id $alt_id");
      }
    }
  }
  
  if (array_key_exists('subset', $term)) {
    //print "WARNING: unhandled tag: subset\n";
  }
  // add synonyms for this cvterm
  if (array_key_exists('synonym', $term)) {    
    if (!tripal_cv_obo_add_synonyms($term, $cvterm)) {
      tripal_cv_obo_quiterror("Cannot add synonyms");
    }
  }

  // reformat the deprecated 'exact_synonym, narrow_synonym, and broad_synonym'
  // types to be of the v1.2 standard
  if (array_key_exists('exact_synonym', $term) or array_key_exists('narrow_synonym', $term) or array_key_exists('broad_synonym', $term)) {
    if (array_key_exists('exact_synonym', $term)) {
      foreach ($term['exact_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 EXACT $2', $synonym);
        $term['synonym'][] = $new;
      }
    }
    if (array_key_exists('narrow_synonym', $term)) {
      foreach ($term['narrow_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 NARROW $2', $synonym);
        $term['synonym'][] = $new;
      }
    }
    if (array_key_exists('broad_synonym', $term)) {
      foreach ($term['broad_synonym'] as $synonym) {
        $new = preg_replace('/^\s*(\".+?\")(.*?)$/', '$1 BROAD $2', $synonym);
        $term['synonym'][] = $new;
      }
    }

    if (!tripal_cv_obo_add_synonyms($term, $cvterm)) {
      tripal_cv_obo_quiterror("Cannot add/update synonyms");
    }
  }
  
  // add the comment to the cvtermprop table
  if (array_key_exists('comment', $term)) {
    $comments = $term['comment'];
    $j = 0;
    foreach ($comments as $comment) {
      if (!tripal_cv_obo_add_cvterm_prop($cvterm, 'comment', $comment, $j)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm property");
      }
      $j++;
    }
  }

  // add any other external dbxrefs
  if (array_key_exists('xref', $term)) {
    foreach ($term['xref'] as $xref) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
      }
    }
  }
  
  if (array_key_exists('xref_analog', $term)) {
    foreach ($term['xref_analog'] as $xref) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
      }
    }
  }
  if (array_key_exists('xref_unk', $term)) {
    foreach ($term['xref_unk'] as $xref) {
      if (!tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref)) {
        tripal_cv_obo_quiterror("Cannot add/update cvterm database reference (dbxref).");
      }
    }
  }

  // add is_a relationships for this cvterm
  if (array_key_exists('is_a', $term)) {
    foreach ($term['is_a'] as $is_a) {
      if (!tripal_cv_obo_add_relationship($cvterm, $defaultcv, 'is_a', $is_a, $is_relationship, $default_db)) {
        tripal_cv_obo_quiterror("Cannot add relationship is_a: $is_a");
      }
    }
  }

  if (array_key_exists('intersection_of', $term)) {
    //print "WARNING: unhandled tag: intersection_of\n";
  }
  if (array_key_exists('union_of', $term)) {
    //print "WARNING: unhandled tag: union_on\n";
  }
  if (array_key_exists('disjoint_from', $term)) {
    //print "WARNING: unhandled tag: disjoint_from\n";
  }
  if (array_key_exists('relationship', $term)) {
    foreach ($term['relationship'] as $value) {
      $rel = preg_replace('/^(.+?)\s.+?$/', '\1', $value);
      $object = preg_replace('/^.+?\s(.+?)$/', '\1', $value);
      if (!tripal_cv_obo_add_relationship($cvterm, $defaultcv, $rel, $object, $is_relationship, $default_db)) {
        tripal_cv_obo_quiterror("Cannot add relationship $rel: $object");
      }
    }
  }
  if (array_key_exists('replaced_by', $term)) {
   //print "WARNING: unhandled tag: replaced_by\n";
  }
  if (array_key_exists('consider', $term)) {
    //print "WARNING: unhandled tag: consider\n";
  }
  if (array_key_exists('use_term', $term)) {
    //print "WARNING: unhandled tag: user_term\n";
  }
  if (array_key_exists('builtin', $term)) {
    //print "WARNING: unhandled tag: builtin\n";
  }
  return 1;
}

/**
 * Add a cvterm relationship
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_relationship($cvterm, $defaultcv, $rel, 
  $objname, $object_is_relationship = 0, $default_db = 'OBO_REL') {

  // make sure the relationship cvterm exists
  $term = array(
    'name' => $rel,
    'id' => "$default_db:$rel",
    'definition' => '',
    'is_obsolete' => 0,
  );
  $relcvterm = tripal_cv_add_cvterm($term, $defaultcv, 1, 0, $default_db);
  
  if (!$relcvterm) {
    // if the relationship term couldn't be found in the default_db provided 
    // then do on more check to find it in the relationship ontology
    $term = array(
      'name' => $rel,
      'id' => "OBO_REL:$rel",
      'definition' => '',
      'is_obsolete' => 0,
    ); 
    $relcvterm = tripal_cv_add_cvterm($term, $defaultcv, 1, 0, 'OBO_REL');
    if (!$relcvterm) {
      tripal_cv_obo_quiterror("Cannot find the relationship term in the current ontology or in the relationship ontology: $rel\n");
    }
  }

  // get the object term
  $oterm = tripal_cv_obo_get_term($objname);
  if (!$oterm) {
    tripal_cv_obo_quiterror("Could not find object term $objname\n");
  }
  
  $objterm = array(); 
  $objterm['id']            = $oterm['id'][0];
  $objterm['name']          = $oterm['name'][0];
  if (array_key_exists('def', $oterm)) {
    $objterm['def']           = $oterm['def'][0];
  }
  if (array_key_exists('subset', $oterm)) {
    $objterm['subset']      = $oterm['subset'][0];  
  }  
  if (array_key_exists('namespace', $oterm)) {
    $objterm['namespace']   = $oterm['namespace'][0];
  }
  if (array_key_exists('is_obsolete', $oterm)) {
    $objterm['is_obsolete'] = $oterm['is_obsolete'][0];
  }
  $objcvterm = tripal_cv_add_cvterm($objterm, $defaultcv, $object_is_relationship, 1, $default_db);  
  if (!$objcvterm) {
    tripal_cv_obo_quiterror("Cannot add cvterm " . $oterm['name'][0]);
  }

  // check to see if the cvterm_relationship already exists, if not add it
  $values = array(
    'type_id'    => $relcvterm->cvterm_id,
    'subject_id' => $cvterm->cvterm_id,
    'object_id'  => $objcvterm->cvterm_id
  );
  $options = array('statement_name' => 'sel_cvtermrelationship_tysuob');
  $result = tripal_core_chado_select('cvterm_relationship', array('*'), $values, $options);
  if (count($result) == 0) {
    $options = array(
      'statement_name' => 'ins_cvtermrelationship_tysuob',
      'return_record' => FALSE
    );
    $success = tripal_core_chado_insert('cvterm_relationship', $values, $options);
    if (!$success) {
      tripal_cv_obo_quiterror("Cannot add term relationship: '$cvterm->name' $rel '$objcvterm->name'");
    }
  }

  return TRUE;
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_get_term($id) {
  $values = array('id' => $id);
  $options = array('statement_name' => 'sel_tripalobotemp_id');
  $result = tripal_core_chado_select('tripal_obo_temp', array('stanza'), $values, $options);
  if (count($result) == 0) {
    return FALSE;
  }
  return unserialize(base64_decode($result[0]->stanza));
}

/**
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_synonyms($term, $cvterm) {

  // make sure we have a 'synonym_type' vocabulary
  $syncv = tripal_cv_add_cv('synonym_type', 'A vocabulary added by the Tripal CV module OBO loader for storing synonym types.');

  // now add the synonyms
  if (array_key_exists('synonym', $term)) {
    foreach ($term['synonym'] as $synonym) {
      
      // separate out the synonym definition and the synonym type
      $def = preg_replace('/^\s*"(.*)"\s*.*$/', '\1', $synonym);
      // the scope will be 'EXACT', etc...
      $scope = drupal_strtolower(preg_replace('/^.*"\s+(.*?)\s+.*$/', '\1', $synonym));
      if (!$scope) {  // if no scope then default to 'exact'
        $scope = 'exact'; 
      } 

      // make sure the synonym type exists in the 'synonym_type' vocabulary
      $values = array(
        'name' => $scope,
        'cv_id' => array(
          'name' => 'synonym_type',
        ),
      );
      $options = array('statement_name' => 'sel_cvterm_nacv', 'is_updlicate' => 1);
      $results = tripal_core_chado_select('cvterm', array('*'), $values, $options);

      // if it doesn't exist then add it
      if (!$results) {
        // build a 'term' object so we can add the missing term
        $term = array(
           'name' => $scope,
           'id' => "internal:$scope",
           'definition' => '',
           'is_obsolete' => 0,
        );
        $syntype = tripal_cv_add_cvterm($term, $syncv->name, 0, 1);
        if (!$syntype) {
          tripal_cv_obo_quiterror("Cannot add synonym type: internal:$scope");
        }
      }
      else {
        $syntype = $results[0];
      }

      // make sure the synonym doesn't already exists
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id, 
        'synonym' => $def
      );
      $options = array('statement_name' => 'sel_cvtermsynonym_cvsy');
      $results = tripal_core_chado_select('cvtermsynonym', array('*'), $values, $options);
      if (count($results) == 0) {
        $values = array(
          'cvterm_id' => $cvterm->cvterm_id, 
          'synonym' => $def,
          'type_id' => $syntype->cvterm_id
        );
        $options = array(
          'statement_name' => 'ins_cvtermsynonym_cvsy',
          'return_record' => FALSE
        );
        $success = tripal_core_chado_insert('cvtermsynonym', $values, $options);
        if (!$success) {
          tripal_cv_obo_quiterror("Failed to insert the synonym for term: $name ($def)");
        }
      }

      // now add the dbxrefs for the synonym if we have a comma in the middle
      // of a description then this will cause problems when splitting os lets
      // just change it so it won't mess up our splitting and then set it back
      // later.
      /**
      $synonym = preg_replace('/(".*?),\s(.*?")/','$1,_$2',$synonym);
      $dbxrefs = preg_split("/, /",preg_replace('/^.*\[(.*?)\]$/','\1',$synonym));
      foreach ($dbxrefs as $dbxref) {
       $dbxref = preg_replace('/,_/',", ",$dbxref);
        if ($dbxref) {
          tripal_cv_obo_add_cvterm_dbxref($syn,$dbxref);
        }
      }
      */
    }
  }

  return TRUE;
}

/**
 * Actually parse the OBO file
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_parse($obo_file, &$header, $jobid) {
  $in_header = 1;
  $stanza = array();
  $default_db = '_global';
  $line_num = 0;
  $num_read = 0;
  $intv_read = 0;
  
  $filesize = filesize($obo_file); 
  $interval = intval($filesize * 0.01);
  if ($interval < 1) {
    $interval = 1;
  } 

  // iterate through the lines in the OBO file and parse the stanzas
  $fh = fopen($obo_file, 'r');
  while ($line = fgets($fh)) {
    
    $line_num++;
    $size = drupal_strlen($line);
    $num_read += $size;
    $intv_read += $size; 
    $line = trim($line);      

    // update the job status every 1% features
    if ($jobid and $intv_read >= $interval) {            
      $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
      print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
      tripal_job_set_progress($jobid, intval(($num_read / $filesize) * 50));
      $intv_read = 0;      
    }
    
    // remove newlines
    $line = rtrim($line);
        
    // remove any special characters that may be hiding
    $line = preg_replace('/[^(\x20-\x7F)]*/', '', $line);

    // skip empty lines
    if (strcmp($line, '') == 0) {
      continue;
    }

    //remove comments from end of lines
    $line = preg_replace('/^(.*?)\!.*$/', '\1', $line);  // TODO: if the explamation is escaped

    // at the first stanza we're out of header
    if (preg_match('/^\s*\[/', $line)) {  
      $in_header = 0;

      // store the stanza we just finished reading
      if (sizeof($stanza) > 0) {
        // add the term to the temp table
        $values = array(
          'id' => $stanza['id'][0],
          'stanza' => base64_encode(serialize($stanza)),
          'type' => $type,
        );
        $options = array('statement_name' => 'ins_tripalobotemp_all');
        $success = tripal_core_chado_insert('tripal_obo_temp', $values, $options);
        if (!$success) {
          watchdog('T_obo_loader', "ERROR: Cannot insert stanza into temporary table.", array(), 'error');
          exit;
        }
        
      }
      // get the stanza type:  Term, Typedef or Instance
      $type = preg_replace('/^\s*\[\s*(.+?)\s*\]\s*$/', '\1', $line);

      // start fresh with a new array
      $stanza = array();
      continue;
    }
    // break apart the line into the tag and value but ignore any escaped colons
    preg_replace("/\\:/", "|-|-|", $line); // temporarily replace escaped colons
    $pair = explode(":", $line, 2);
    $tag = $pair[0];
    $value = ltrim(rtrim($pair[1]));// remove surrounding spaces
    
    // if this is the ID then look for the default DB
    $matches = array();
    if ($tag == 'id' and preg_match('/^(.+?):.*$/', $value, $matches)) {
       $default_db = $matches[1];
    }
    
    $tag = preg_replace("/\|-\|-\|/", "\:", $tag); // return the escaped colon
    $value = preg_replace("/\|-\|-\|/", "\:", $value);
    if ($in_header) {
      if (!array_key_exists($tag, $header)) {
        $header[$tag] = array();
      }
      $header[$tag][] = $value;
    }
    else {
      if (!array_key_exists($tag, $stanza)) {
        $stanza[$tag] = array();
      }
      $stanza[$tag][] = $value;
    }
  }
  // now add the last term in the file
  if (sizeof($stanza) > 0) {
    $values = array(
      'id' => $stanza['id'][0],
      'stanza' => base64_encode(serialize($stanza)),
      'type' => $type,
    );
    $options = array('statement_name' => 'ins_tripalobotemp_all');
    tripal_core_chado_insert('tripal_obo_temp', $values, $options);
    if (!$success) {
      watchdog('T_obo_loader', "ERROR: Cannot insert stanza into temporary table.", array(), 'error');
      exit;
    }
  }
  return $default_db;
}

/**
 * Add database reference to cvterm
 *
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_cvterm_dbxref($cvterm, $xref) {

  $dbname = preg_replace('/^(.+?):.*$/', '$1', $xref);
  $accession = preg_replace('/^.+?:\s*(.*?)(\{.+$|\[.+$|\s.+$|\".+$|$)/', '$1', $xref);
  $description = preg_replace('/^.+?\"(.+?)\".*?$/', '$1', $xref);
  $dbxrefs = preg_replace('/^.+?\[(.+?)\].*?$/', '$1', $xref);

  if (!$accession) {
    tripal_cv_obo_quiterror();
    watchdog("T_obo_loader", "Cannot add a dbxref without an accession: '$xref'", NULL, WATCHDOG_WARNING);
    return FALSE;
  }

  // if the xref is a database link, handle that specially
  if (strcmp($dbname, 'http') == 0) {
    $accession = $xref;
    $dbname = 'URL';
  }

  // add the database
  $db = tripal_db_add_db($dbname);
  if (!$db) {
    tripal_cv_obo_quiterror("Cannot find database '$dbname' in Chado.");
  }

  // now add the dbxref
  $dbxref = tripal_cv_obo_add_dbxref($db->db_id, $accession, '', $description);
  if (!$dbxref) {
    tripal_cv_obo_quiterror("Cannot find or add the database reference (dbxref)");
  }

  // finally add the cvterm_dbxref but first check to make sure it exists
  $values = array(
    'cvterm_id' => $cvterm->cvterm_id,
    'dbxref_id' => $dbxref->dbxref_id,
  );
  $options = array('statement_name' => 'sel_cvtermdbxref_cvdb');
  $result = tripal_core_chado_select('cvterm_dbxref', array('*'), $values, $options);
  if (count($result) == 0) {    
    $ins_options = array(
      'statement_name' => 'ins_cvtermdbxref_cvdb',
      'return_record' => FALSE
    );
    $result = tripal_core_chado_insert('cvterm_dbxref', $values, $ins_options);
    if (!$result){
      tripal_cv_obo_quiterror("Cannot add cvterm_dbxref: $xref");
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * Add property to CVterm
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_cvterm_prop($cvterm, $property, $value, $rank) {

  // make sure the 'cvterm_property_type' CV exists
  $cv = tripal_cv_add_cv('cvterm_property_type', '');
  if (!$cv) {
    tripal_cv_obo_quiterror("Cannot add/find cvterm_property_type cvterm");
  }

  // get the property type cvterm.  If it doesn't exist then we want to add it
  $values = array(
    'name' => $property,
    'cv_id' => $cv->cv_id,
  );
  $options = array('statement_name' => 'sel_cvterm_nacv_na');
  $results = tripal_core_chado_select('cvterm', array('*'), $values, $options);
  if (count($results) == 0) {    
    $term = array(
      'name' => $property,
      'id' => "internal:$property",
      'definition' => '',
      'is_obsolete' => 0,
    );
    $cvproptype = tripal_cv_add_cvterm($term, $cv->name, 0, 0);
    if (!$cvproptype) {
      tripal_cv_obo_quiterror("Cannot add cvterm property: internal:$property");
      return FALSE;
    }
  }
  else {
    $cvproptype = $results[0];
  }

  // remove any properties that currently exist for this term.  We'll reset them
  if ($rank == 0) {
    $values = array('cvterm_id' => $cvterm->cvterm_id);
    $options = array('statement_name' => 'del_cvtermprop_cv');
    $success = tripal_core_chado_delete('cvtermprop', $values, $options);
    if (!$success) {
       tripal_cv_obo_quiterror("Could not remove existing properties to update property $property for term\n");
       return FALSE;
    }    
  }

  // now add the property
  $values = array(
    'cvterm_id' => $cvterm->cvterm_id,
    'type_id' => $cvproptype->cvterm_id,
    'value' => $value,
    'rank' => $rank,
  );
  $options = array(
    'statement_name' => 'ins_cvtermprop_cvtyvara',
    'return_record' => FALSE,
  );
  $result = tripal_core_chado_insert('cvtermprop', $values, $options);
  if (!$result) {
    tripal_cv_obo_quiterror("Could not add property $property for term\n");
    return FALSE;
  }
  return TRUE;
}


/**
 * Add Database Reference
 * @ingroup tripal_obo_loader
 */
function tripal_cv_obo_add_dbxref($db_id, $accession, $version='', $description='') {

  // check to see if the dbxref exists if not, add it
  $values = array(
    'db_id' => $db_id,
    'accession' => $accession,
  );
  $options = array('statement_name' => 'sel_dbxref_idac');
  $result = tripal_core_chado_select('dbxref', array('dbxref_id'), $values, $options);
  if (count($result) == 0){
    $ins_values = array(
      'db_id'       => $db_id,
      'accession'   => $accession,
      'version'     => $version,
      'description' => $description,
    );
    $ins_options = array(
      'statement_name' => 'ins_dbxref_idacvede',
      'return_record' => FALSE
    );
    $result = tripal_core_chado_insert('dbxref', $ins_values, $ins_options);
    if (!$result) {
      tripal_cv_obo_quiterror("Failed to insert the dbxref record $accession");
      return FALSE;
    } 
    $result = tripal_core_chado_select('dbxref', array('dbxref_id'), $values, $options);    
  }
  return $result[0];
}

