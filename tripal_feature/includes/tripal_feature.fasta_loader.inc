<?php
/**
 * @file
 * Provides fasta loading functionality. Creates features based on their specification
 * in a fasta file.
 */

/**
 * @defgroup fasta_loader FASTA Feature Loader
 * @ingroup tripal_feature
 * @{
 * Provides fasta loading functionality. Creates features based on their specification
 * in a fasta file.
 * @}
 *
 */

/**
 * The form to submit a fasta loading job
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_load_form() {

  $form['fasta_file']= array(
    '#type'          => 'textfield',
    '#title'         => t('FASTA File'),
    '#description'   => t('Please enter the full system path for the FASTA file, or a path within the Drupal
                           installation (e.g. /sites/default/files/xyz.obo).  The path must be accessible to the
                           server on which this Drupal instance is running.'),
    '#required' => TRUE,
  );

  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $org_rset = chado_query($sql);
  $organisms = array();
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['organism_id'] = array(
   '#title'       => t('Organism'),
   '#type'        => t('select'),
   '#description' => t("Choose the organism to which these sequences are associated"),
   '#required'    => TRUE,
   '#options'     => $organisms,
  );

  // get the sequence ontology CV ID
  $values = array('name' => 'sequence');
  $cv = chado_select_record('cv', array('cv_id'), $values);
  $cv_id = $cv[0]->cv_id;
  
  $form['seqtype']= array(
    '#type' => 'textfield',
    '#title' => t('Sequence Type'),
    '#required' => TRUE,
    '#description' => t('Please enter the Sequence Ontology (SO) term name that describes the sequences in the FASTA file (e.g. gene, mRNA, polypeptide, etc...)'),
    '#autocomplete_path' => "admin/tripal/chado/tripal_cv/cvterm/auto_name/$cv_id",
  );
  
  $form['method']= array(
    '#type' => 'radios',
    '#title' => 'Method',
    '#required' => TRUE,
    '#options' => array(
      t('Insert only'),
      t('Update only'),
      t('Insert and update'),
    ),
    '#description' => t('Select how features in the FASTA file are handled.
       Select "Insert only" to insert the new features. If a feature already
       exists with the same name or unique name and type then it is skipped.
       Select "Update only" to only update featues that already exist in the
       database.  Select "Insert and Update" to insert features that do
       not exist and upate those that do.'),
    '#default_value' => 2,
  );
  $form['match_type']= array(
    '#type' => 'radios',
    '#title' => 'Name Match Type',
    '#required' => TRUE,
    '#options' => array(
      t('Name'),
      t('Unique name'),
    ),
    '#description' => t('Used for "updates only" or "insert and update" methods. Not required if method type is "insert".
      Feature data is stored in Chado with both a human-readable
      name and a unique name. If the features in your FASTA file are uniquely identified using
      a human-readable name then select the "Name" button. If your features are
      uniquely identified using the unique name then select the "Unique name" button.  If you
      loaded your features first using the GFF loader then the unique name of each
      features were indicated by the "ID=" attribute and the name by the "Name=" attribute.
      By default, the FASTA loader will use the first word (character string
      before the first space) as  the name for your feature. If
      this does not uniquely identify your feature consider specifying a regular expression in the advanced section below.
      Additionally, you may import both a name and a unique name for each sequence using the advanced options.'),
    '#default_value' => 1,
  );

  $form['analysis'] = array(
    '#type' => 'fieldset',
    '#title' => t('Analysis Used to Derive Features'),
    '#collapsed' => TRUE
  );
  $form['analysis']['desc'] = array(
    '#markup' => t("Why specify an analysis for a data load?  All data comes
       from some place, even if downloaded from Genbank. By specifying
       analysis details for all data uploads, it allows an end user to reproduce the
       data set, but at least indicates the source of the data."),
  );

  // get the list of organisms
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $org_rset = chado_query($sql);
  $analyses = array();
  $analyses[''] = '';
  while ($analysis = $org_rset->fetchObject()) {
    $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
  }
  $form['analysis']['analysis_id'] = array(
    '#title'       => t('Analysis'),
    '#type'        => t('select'),
    '#description' => t("Choose the analysis to which these features are associated"),
    '#required'    => TRUE,
    '#options'     => $analyses,
  );

  // Advanced Options
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  $form['advanced']['re_help']= array(
    '#type' => 'item',
    '#value' => t('A regular expression is an advanced method for extracting information from a string of text.
                   Your FASTA file may contain both a human-readable name and a unique name for each sequence.
                   If you want to import
                   both the name and unique name for all sequences, then you must provide regular expressions
                   so that the loader knows how to separate them.
                   Otherwise the name and uniquename will be the same.
                   By default, this loader will use the first word in the definition
                   lines of the FASTA file
                   as the name or unique name of the feature.'),
  );
  $form['advanced']['re_name']= array(
    '#type' => 'textfield',
    '#title' => t('Regular expression for the name'),
    '#required' => FALSE,
    '#description' => t('Enter the regular expression that will extract the
       feature name from the FASTA definition line. For example, for a
       defintion line with a name and unique name separated by a bar \'|\' (>seqname|uniquename),
       the regular expression for the name would be, "^(.*?)\|.*$".'),
  );
  $form['advanced']['re_uname']= array(
    '#type' => 'textfield',
    '#title' => t('Regular expression for the unique name'),
    '#required' => FALSE,
    '#description' => t('Enter the regular expression that will extract the
       feature name from the FASTA definition line. For example, for a
       defintion line with a name and unique name separated by a bar \'|\' (>seqname|uniquename),
       the regular expression for the unique name would be "^.*?\|(.*)$").'),
  );


  // Advanced database cross-reference optoins
  $form['advanced']['db'] = array(
    '#type' => 'fieldset',
    '#title' => t('External Database Reference'),
    '#weight' => 6,
    '#collapsed' => TRUE
  );
  $form['advanced']['db']['re_accession']= array(
    '#type' => 'textfield',
    '#title' => t('Regular expression for the accession'),
    '#required' => FALSE,
    '#description' => t('Enter the regular expression that will extract the accession for the external database for each feature from the FASTA definition line.'),
    '#weight' => 2
  );

  // get the list of databases
  $sql = "SELECT * FROM {db} ORDER BY name";
  $db_rset = chado_query($sql);
  $dbs = array();
  $dbs[''] = '';
  while ($db = $db_rset->fetchObject()) {
    $dbs[$db->db_id] = "$db->name";
  }
  $form['advanced']['db']['db_id'] = array(
   '#title'       => t('External Database'),
   '#type'        => t('select'),
   '#description' => t("Plese choose an external database for which these sequences have a cross reference."),
   '#required'    => FALSE,
   '#options'     => $dbs,
   '#weight'      => 1,
  );

  $form['advanced']['relationship'] = array(
    '#type' => 'fieldset',
    '#title' => t('Relationships'),
    '#weight' => 6,
    '#collapsed' => TRUE
  );
  $rels = array();
  $rels[''] = '';
  $rels['part_of'] = 'part of';
  $rels['derives_from'] = 'produced by';


  // Advanced references options
  $form['advanced']['relationship']['rel_type']= array(
   '#title'       => t('Relationship Type'),
   '#type'        => t('select'),
   '#description' => t("Use this option to create associations, or relationships between the
                        features of this FASTA file and existing features in the database. For
                        example, to associate a FASTA file of peptides to existing genes or transcript sequence,
                        select the type 'produced by'. For a CDS sequences select the type 'part of'"),
   '#required'    => FALSE,
   '#options'     => $rels,
   '#weight'      => 5,
  );
  $form['advanced']['relationship']['re_subject']= array(
    '#type' => 'textfield',
    '#title' => t('Regular expression for the parent'),
    '#required' => FALSE,
    '#description' => t('Enter the regular expression that will extract the unique
                         name needed to identify the existing sequence for which the
                         relationship type selected above will apply.'),
    '#weight' => 6
  );
  $form['advanced']['relationship']['parent_type']= array(
    '#type' => 'textfield',
    '#title' => t('Parent Type'),
    '#required' => FALSE,
    '#description' => t('Please enter the Sequence Ontology term for the parent.  For example
                         if the FASTA file being loaded is a set of proteins that are
                         products of genes, then use the SO term \'gene\' or \'transcript\' or equivalent. However,
                         this type must match the type for already loaded features.'),
    '#weight' => 7
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Import FASTA file'),
    '#weight' => 10,
  );
  return $form;
}

/**
 * Validate the fasta loader job form
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_load_form_validate($form, &$form_state) {
  $fasta_file   = trim($form_state['values']['fasta_file']);
  $organism_id  = $form_state['values']['organism_id'];
  $type         = trim($form_state['values']['seqtype']);
  $method       = trim($form_state['values']['method']);
  $match_type   = trim($form_state['values']['match_type']);
  $re_name      = trim($form_state['values']['re_name']);
  $re_uname     = trim($form_state['values']['re_uname']);
  $re_accession = trim($form_state['values']['re_accession']);
  $db_id        = $form_state['values']['db_id'];
  $rel_type     = $form_state['values']['rel_type'];
  $re_subject   = trim($form_state['values']['re_subject']);
  $parent_type  = trim($form_state['values']['parent_type']);

  if ($method == 0) {
    $method = 'Insert only';
  }
  if ($method == 1) {
    $method = 'Update only';
  }
  if ($method == 2) {
    $method = 'Insert and update';
  }

  if ($match_type == 0) {
    $match_type = 'Name';
  }

  if ($match_type == 1) {
    $match_type = 'Unique name';
  }


  if ($re_name and !$re_uname and strcmp($match_type, 'Unique name')==0) {
    form_set_error('re_uname', t("You must provide a regular expression to identify the sequence unique name"));
  }

  if (!$re_name and $re_uname and strcmp($match_type, 'Name')==0) {
    form_set_error('re_name', t("You must provide a regular expression to identify the sequence name"));
  }

  // check to see if the file is located local to Drupal
  $fasta_file = trim($fasta_file);
  $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $fasta_file;
  if (!file_exists($dfile)) {
    // if not local to Drupal, the file must be someplace else, just use
    // the full path provided
    $dfile = $fasta_file;
  }
  if (!file_exists($dfile)) {
    form_set_error('fasta_file', t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
  }

  // make sure if a relationship is specified that all fields are provided.
  if (($rel_type or $parent_type) and !$re_subject) {
    form_set_error('re_subject', t("Please provide a regular expression for the parent"));
  }
  if (($rel_type or $re_subject) and !$parent_type) {
    form_set_error('parent_type', t("Please provide a SO term for the parent"));
  }
  if (($parent_type or $re_subject) and !$rel_type) {
    form_set_error('rel_type', t("Please select a relationship type"));
  }


  // make sure if a database is specified that all fields are provided
  if ($db_id and !$re_accession) {
    form_set_error('re_accession', t("Please provide a regular expression for the accession"));
  }
  if ($re_accession and !$db_id) {
    form_set_error('db_id', t("Please select a database"));
  }

  // check to make sure the types exists
  $cvtermsql = "SELECT CVT.cvterm_id
               FROM {cvterm} CVT
                  INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                  LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
               WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)";
  $cvterm = chado_query($cvtermsql, array(':cvname' => 'sequence', ':name' => $type, ':synonym' => $type))->fetchObject();
  if (!$cvterm) {
    form_set_error('type', t("The Sequence Ontology (SO) term selected for the sequence type is not available in the database. Please check spelling or select another."));
  }
  if ($rel_type) {
    $cvterm = chado_query($cvtermsql, array(':cvname' => 'sequence', ':name' => $parent_type, ':synonym' => $parent_type))->fetchObject();
    if (!$cvterm) {
      form_set_error('parent_type', t("The Sequence Ontology (SO) term selected for the parent relationship is not available in the database. Please check spelling or select another."));
    }
  }

  // check to make sure the 'relationship' and 'sequence' ontologies are loaded
  $form_state['storage']['dfile'] = $dfile;
}

/**
 * Submit a fasta loading job
 *
 * @ingroup fasta_loader
 */
function tripal_feature_fasta_load_form_submit($form, &$form_state) {
  global $user;

  $dfile        = $form_state['storage']['dfile'];
  $organism_id  = $form_state['values']['organism_id'];
  $type         = trim($form_state['values']['seqtype']);
  $method       = trim($form_state['values']['method']);
  $match_type   = trim($form_state['values']['match_type']);
  $re_name      = trim($form_state['values']['re_name']);
  $re_uname     = trim($form_state['values']['re_uname']);
  $re_accession = trim($form_state['values']['re_accession']);
  $db_id        = $form_state['values']['db_id'];
  $rel_type     = $form_state['values']['rel_type'];
  $re_subject   = trim($form_state['values']['re_subject']);
  $parent_type   = trim($form_state['values']['parent_type']);
  $analysis_id = $form_state['values']['analysis_id'];

  if ($method == 0) {
    $method = 'Insert only';
  }
  if ($method == 1) {
    $method = 'Update only';
  }
  if ($method == 2) {
    $method = 'Insert and update';
  }

  if ($match_type == 0) {
    $match_type = 'Name';
  }

  if ($match_type == 1) {
    $match_type = 'Unique name';
  }

  $args = array($dfile, $organism_id, $type, $re_name, $re_uname,
          $re_accession, $db_id, $rel_type, $re_subject, $parent_type, $method,
          $user->uid, $analysis_id, $match_type);

  $fname = preg_replace("/.*\/(.*)/", "$1", $dfile);
  tripal_add_job("Import FASTA file: $fname", 'tripal_feature',
    'tripal_feature_load_fasta', $args, $user->uid);
}

/**
 * Actually load a fasta file. This is the function called by tripal jobs
 *
 * @param $dfile
 *  The full path to the fasta file to load
 * @param $organism_id
 *  The organism_id of the organism these features are from
 * @param $type
 *  The type of features contained in the fasta file
 * @param $re_name
 *  A regular expression to extract the feature.name from the fasta header
 * @param $re_uname
 *  A regular expression to extract the feature.uniquename from the fasta header
 * @param $re_accession
 *  A regular expression to extract the accession of the feature.dbxref_id
 * @param $db_id
 *  The db_id of the above dbxref
 * @param $rel_type
 *  The type of relationship when creating a feature_relationship between this
 *   feature (object) and an extracted subject
 * @param $re_subject
 *  The regular expression to extract the uniquename of the feature to be the subject
 *   of the above specified relationship
 * @param $parent_type
 *  The type of the parent feature
 * @param $method
 *  The method of feature adding. (ie: 'Insert only', 'Update only', 'Insert and update')
 * @param $uid
 *   The user id of the user who submitted the job
 * @param $analysis_id
 *   The analysis_id to associate the features in this fasta file with
 * @param $match_type
 *   Whether to match existing features based on the 'Name' or 'Unique name'
 * @param $job = NULL
 *   The tripal job
 *
 * @ingroup fasta_loader
 */
function tripal_feature_load_fasta($dfile, $organism_id, $type,
  $re_name, $re_uname, $re_accession, $db_id, $rel_type,
  $re_subject, $parent_type, $method, $uid, $analysis_id,
  $match_type, $job = NULL) {

  $transaction = db_transaction();
  print "\nNOTE: Loading of this GFF file is performed using a database transaction. \n" .
       "If the load fails or is terminated prematurely then the entire set of \n" .
       "insertions/updates is rolled back and will not be found in the database\n\n";
  try {

    // First get the type for this sequence.
    $cvtermsql = "
      SELECT CVT.cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)
    ";
    $cvterm = chado_query($cvtermsql, array(':cvname' => 'sequence', ':name' => $type, ':synonym' => $type))->fetchObject();
    if (!$cvterm) {
      tripal_report_error("T_fasta_loader", TRIPAL_ERROR, "Cannot find the term type: '%type'", array('%type' => $type));
      return 0;
    }
    
    // Second, if there is a parent type then get that.
    if ($parent_type) {
      $parentcvterm = chado_query($cvtermsql, array(':cvname' => 'sequence', ':name' => $parent_type, ':synonym' => $parent_type))->fetchObject();
      if (!$parentcvterm) {
        tripal_report_error("T_fasta_loader", TRIPAL_ERROR, "Cannot find the paretne term type: '%type'", array('%type' => $parentcvterm));
        return 0;
      }
    }
    
    // Third, if there is a relationship type then get that.
    if ($rel_type) {
      $relcvterm = chado_query($cvtermsql, array(':cvname' => 'relationship', ':name' => $rel_type, ':synonym' => $rel_type))->fetchObject();
      if (!$relcvterm) {
        tripal_report_error("T_fasta_loader", TRIPAL_ERROR, "Cannot find the relationship term type: '%type'", array('%type' => $relcvterm));
        return 0;
      }
    }
    
    // We need to get the table schema to make sure we don't overrun the
    // size of fields with what our regular expressions retrieve
    $feature_tbl = chado_get_schema('feature');
    $dbxref_tbl = chado_get_schema('dbxref');
  
    print "Step 1: finding sequences\n";
    $filesize = filesize($dfile);
    $fh = fopen($dfile, 'r');
    if (!$fh) {
      tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "cannot open file: %dfile", array('%dfile' => $dfile));
      return 0;
    }
    
    // Calculate the interval at which we will print to the screen that status.
    $interval = intval($filesize * 0.01);
    if ($interval < 1) {
      $interval = 1;
    }
    $inv_read = 0;
    $num_read = 0;

    // Iterate through the lines of the file.  Keep a record for
    // where in the file each line is at for later import.
    $seqs = array();
    $num_seqs = 0;
    $prev_pos = 0;
    $set_start = FALSE;
    while ($line = fgets($fh)) {
      $num_read += strlen($line);
      $intv_read += strlen($line);

      // If we encounter a definition line then get the name, uniquename,
      // accession and relationship subject from the definition line.
      if (preg_match('/^>/', $line)) {

        // Remove the > symbol from the defline.
        $defline = preg_replace("/^>/", '', $line); 
  
        // Get the feature name if a regular expression is provided.
        if ($re_name) {
          if (!preg_match("/$re_name/", $defline, $matches)) {
            tripal_report_error('trp-fasta', 
              "ERROR: Regular expression for the feature name finds nothing. Line %line.", 
              array('%line' => $i), 'error');
          }
          elseif (strlen($matches[1]) > $feature_tbl['fields']['name']['length']) {
            tripal_report_error('trp-fasta', 
              "WARNING: Regular expression retrieves a value too long for the feature name. Line %line.", 
              array('%line' => $i), 'error');
          }
          else {
            $name = trim($matches[1]);
          }
        }
        // If the match_type is name and no regular expression was provided
        // then use the first word as the name, otherwise we don't set the name.
        elseif (strcmp($match_type, 'Name')==0) {
          if (preg_match("/^\s*(.*?)[\s\|].*$/", $defline, $matches)) {
            if (strlen($matches[1]) > $feature_tbl['fields']['name']['length']) {
              tripal_report_error('trp-fasta', 
                "WARNING: Regular expression retrieves a feature name too long for the feature name. Line %line.", 
                array('%line' => $i), 'error');
            }
            else {
              $name = trim($matches[1]);
            }
          }
          else {
            tripal_report_error('trp-fasta', 
              "ERROR: Cannot find a feature name. Line %line.", 
              array('%line' => $i), 'error');
          }
        }
  
        // Get the feature uniquename if a regular expression is provided.
        if ($re_uname) {
          if (!preg_match("/$re_uname/", $defline, $matches)) {
            tripal_report_error('trp-fasta', 
              "ERROR: Regular expression for the feature unique name finds nothing. Line %line.", 
              array('%line' => $i), 'error');
          }
          $uname = trim($matches[1]);
        }
        // If the match_type is name and no regular expression was provided
        // then use the first word as the name, otherwise, we don't set the 
        // unqiuename.
        elseif (strcmp($match_type, 'Unique name')==0) {
          if (preg_match("/^\s*(.*?)[\s\|].*$/", $defline, $matches)) {
            $uname = trim($matches[1]);
          }
          else {
            tripal_report_error('trp-fasta', 
              "ERROR: Cannot find a feature unique name. Line %line.", 
              array('%line' => $i), 'error');
          }
        }

        // Get the accession if a regular expression is provided.
        preg_match("/$re_accession/", $defline, $matches);
        if (strlen($matches[1]) > $dbxref_tbl['fields']['accession']['length']) {
          tripal_report_error('trp-fasta', 
            "WARNING: Regular expression retrieves an accession too long for the feature name. " .
            "Cannot add cross reference. Line %line.", 
            array('%line' => $i), 'warning');
        }
        else {
          $accession = trim($matches[1]);
        }
  
        // Get the relationship subject
        preg_match("/$re_subject/", $line, $matches);
        $subject = trim($matches[1]);
        
        // Add the details to the sequence.
        $seqs[$num_seqs] = array(
          'name' => $name,
          'uname' => $uname,
          'accession' => $accession,
          'subject' => $subject,
          'seq_start' => ftell($fh),
        );
        $set_start = TRUE;
        // If this isn't the first sequence, then we want to specify where
        // the previous sequence ended.
        if ($num_seqs > 0) {
          $seqs[$num_seqs - 1]['seq_end'] = $prev_pos;
        }
        $num_seqs++;
      }
      // Keep the current file position so we can use it to set the sequence
      // ending position
      $prev_pos = ftell($fh);

      // update the job status every % bytes
      if ($job and $intv_read >= $interval) {
        $intv_read = 0;
        $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
        if ($name) {
          print "Parsing Line $i (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
        }
        else {
          print "Parsing Line $i (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
        }
        tripal_set_job_progress($job, intval(($num_read / $filesize) * 100));
      }
    }
    $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
    print "Parsing Line $i (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
    tripal_set_job_progress($job, 50);
    
    // Set the end position for the last sequence.
    $seqs[$num_seqs - 1]['seq_end'] = $num_read - strlen($line);

    // Now that we know where the sequences are in the file we need to add them.
    print "\nStep 2: Importing sequences\n";
    for ($i = 0; $i < $num_seqs; $i++) {
      $seq = $seqs[$i];
      print "Importing " . ($i + 1) ." of $num_seqs. ";
      if ($name) {
        print "Current feature: " . $seq['name'] . ".\n";
      }
      else {
        print "Current feature: " . $seq['uname'] . ".\n";
      }

      tripal_feature_load_fasta_feature($fh, $seq['name'], $seq['uname'], 
        $db_id, $seq['accession'], $seq['subject'], $rel_type, $parent_type, 
        $analysis_id, $organism_id, $cvterm, $source, $method, 
        $re_name, $match_type, $parentcvterm, $relcvterm, $seq['seq_start'], 
        $seq['seq_end']);
    }
    tripal_set_job_progress($job, 100);
    fclose($fh);
  }
  catch (Exception $e) {
    fclose($fh);
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    watchdog_exception('T_fasta_loader', $e);
    print "FAILED: Rolling back database changes...\n";
  }
  
  print "\nDone\n";
}

/**
 * A helper function for tripal_feature_load_fasta() to load a single feature
 *
 * @ingroup fasta_loader
 */
function tripal_feature_load_fasta_feature($fh, $name, $uname, $db_id, $accession,
  $parent, $rel_type, $parent_type, $analysis_id, $organism_id, $cvterm,
  $source, $method, $re_name, $match_type, $parentcvterm, $relcvterm,
  $seq_start, $seq_end) {

  // Check to see if this feature already exists if the match_type is 'Name'.
  if (strcmp($match_type, 'Name') == 0) {
    $values = array(
      'organism_id' => $organism_id,
      'name' => $name,
      'type_id' => $cvterm->cvterm_id,
    );
    $results = chado_select_record('feature', array('feature_id'), $values);
    if (count($results) > 1) {
      tripal_report_error('T_fasta_loader', "Multiple features exist with the name '%name' of type
               '%type' for the organism.  skipping", array('%name' => $name, '%type' => $type));
      return 0;
    }
    if (count($results) == 1) {
      $feature = $results[0];
    }
  }

  // Check if this feature already exists if the match_type is 'Unique Name'.
  if (strcmp($match_type, 'Unique name') == 0) {
    $values = array(
      'organism_id' => $organism_id,
      'uniquename' => $uname,
      'type_id' => $cvterm->cvterm_id,
    );

    $results = chado_select_record('feature', array('feature_id'), $values);
    if (count($results) > 1) {
      tripal_report_error('T_fasta_loader', TRIPAL_WARNING,
        "Multiple features exist with the name '%name' of type '%type' for the organism.  skipping", 
        array('%name' => $name, '%type' => $type));
      return 0;
    }
    if (count($results) == 1) {
      $feature = $results[0];
    }

    // If the feature exists but this is an "insert only" then skip.
    if ($feature and (strcmp($method, 'Insert only')==0)) {
      tripal_report_error('T_fasta_loader', TRIPAL_WARNING, 
        "Feature already exists '%name' ('%uname') while matching on %type. Skipping insert.",
        array('%name' => $name, '%uname' => $uname, '%type' => drupal_strtolower($match_type)));
      return 0;
    }
  }

  // If we don't have a feature and we're doing an insert then do the insert.
  $inserted = 0;
  if (!$feature and (
      strcmp($method, 'Insert only') == 0 or 
      strcmp($method, 'Insert and update') == 0)) {
    // If we have a unique name but not a name then set them to be the same 
    if (!$uname) {
      $uname = $name;
    }
    elseif (!$name) {
      $name = $uname;
    }

    // Insert the feature record.
    $values = array(
      'organism_id' => $organism_id,
      'name' => $name,
      'uniquename' => $uname,
      'type_id' => $cvterm->cvterm_id,
    );
    $success = chado_insert_record('feature', $values);
    if (!$success) {
      tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to insert feature '%name (%uname)'",
        array('%name' => $name, '%uname' => $numane));
      return 0;
    }

    // now get the feature we just inserted
    $values = array(
      'organism_id' => $organism_id,
      'uniquename' => $uname,
      'type_id' => $cvterm->cvterm_id,
    );
    $results = chado_select_record('feature', array('feature_id'), $values);
    if (count($results) == 1) {
       $inserted = 1;
       $feature = $results[0];
    }
    else {
      tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to retreive newly inserted feature '%name (%uname)'",
        array('%name' => $name, '%uname' => $numane));
      return 0;
    }
    
    // Add the residues for this feature
    tripal_feature_load_fasta_residues($fh, $feature->feature_id, $seq_start, $seq_end);
  }

  // if we don't have a feature and the user wants to do an update then fail
  if (!$feature and (strcmp($method, 'Update only') == 0 or 
       drupal_strcmp($method, 'Insert and update') == 0)) {
    tripal_report_error('T_fasta_loader', TRIPAL_ERROR, 
      "Failed to find feature '%name' ('%uname') while matching on " .
      drupal_strtolower($match_type), 
      array('%name' => $name, '%uname' => $uname));
    return 0;
  }

  // if we do have a feature and this is an update then proceed with the update
  if ($feature and !$inserted and 
       (strcmp($method, 'Update only') == 0 or 
        strcmp($method, 'Insert and update')==0)) {

    // if the user wants to match on the Name field
    if (strcmp($match_type, 'Name')==0) {

      // if we're matching on the name but do not have a unique name then we 
      // don't want to update the uniquename.
      $values = array();
      if ($uname) {

        // First check to make sure that by changing the unique name of this 
        // feature that we won't conflict with another existing feature of 
        // the same name
        $values = array(
          'organism_id' => $organism_id,
          'uniquename' => $uname,
          'type_id' => $cvterm->cvterm_id,
        );
        $results = chado_select_record('feature', array('feature_id'), $values);
        if (count($results) > 0) {
          tripal_report_error('T_fasta_loader', 
            "Cannot update the feature '%name' with a uniquename of '%uname' and type of '%type' as it
            conflicts with an existing feature with the same uniquename and type.",
            array('%name' => $name, '%uname' => $uname, '%type' => $type));
          return 0;
        }

        // the changes to the uniquename don't conflict so proceed with the update
        $values = array(
          'uniquename' => $uname,
        );
        $match = array(
          'name' => $name,
          'organism_id' => $organism_id,
          'type_id' => $cvterm->cvterm_id,
        );

        // perform the update
        $success = chado_update_record('feature', $match, $values);
        if (!$success) {
          tripal_report_error('T_fasta_loader', TRIPAL_ERROR,
          "Failed to update feature '%name' ('%name')",
          array('%name' => $name, '%uiname' => $uname));
          return 0;
        }
      }
    }

    // If the user wants to match on the unique name field.
    if (strcmp($match_type, 'Unique name')==0) {
      // If we're matching on the uniquename and have a new name then 
      // we want to update the name.
      $values = array();
      if ($name) {
        $values = array(
          'name' => $name,
        );
        $match = array(
          'uniquename' => $uname,
          'organism_id' => $organism_id,
          'type_id' => $cvterm->cvterm_id,
        );
        $success = chado_update_record('feature', $match, $values);
        if (!$success) {
          tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to update feature '%name' ('%name')",
          array('%name' => $name, '%uiname' => $uname));
          return 0;
        }
      }
    }
  }
  
  // Update the residues for this feature
  tripal_feature_load_fasta_residues($fh, $feature->feature_id, $seq_start, $seq_end);

  // add in the analysis link
  if ($analysis_id) {
    // if the association doens't alredy exist then add one
    $values = array(
      'analysis_id' => $analysis_id,
      'feature_id' => $feature->feature_id,
    );
    $results = chado_select_record('analysisfeature', array('analysisfeature_id'), $values);
    if (count($results) == 0) {
      $success = chado_insert_record('analysisfeature', $values);
      if (!$success) {
        tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to associate analysis and feature '%name' ('%name')",
          array('%name' => $name, '%uname' => $uname));
        return 0;
      }
    }
  }

  // now add the database cross reference
  if ($db_id) {
    // check to see if this accession reference exists, if not add it
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession
    );
    $results = chado_select_record('dbxref', array('dbxref_id'), $values);
    // if the accession doesn't exist then add it
    if (count($results) == 0) {
      $results = chado_insert_record('dbxref', $values);
      if (!$results) {
        tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to add database accession '%accession'",
          array('%accession' => $accession));
        return 0;
      }
      $results = chado_select_record('dbxref', array('dbxref_id'), $values);
      if (count($results) == 1) {
        $dbxref = $results[0];
      }
      else {
        tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to retreive newly inserted dbxref '%name (%uname)'",
          array('%name' => $name, '%uname' => $numane));
        return 0;
      }
    }
    else {
      $dbxref = $results[0];
    }

    // check to see if the feature dbxref record exists if not, then add it
    $values = array(
      'feature_id' => $feature->feature_id,
      'dbxref_id' => $dbxref->dbxref_id
    );
    $results = chado_select_record('feature_dbxref', array('feature_dbxref_id'), $values);
    if (count($results) == 0) {
      $success = chado_insert_record('feature_dbxref', $values);
      if (!$success) {
        tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to add associate database accession '%accession' with feature",
          array('%accession' => $accession));
        return 0;
      }
    }
  }

   // now add in the relationship if one exists. If not, then add it
  if ($rel_type) {
    $values = array(
      'organism_id' => $organism_id,
      'uniquename' => $parent,
      'type_id' => $parentcvterm->cvterm_id,
    );
    $results = chado_select_record('feature', array('feature_id'), $values);
    if (count($results) != 1) {
      tripal_report_error('T_fasta_loader', "Cannot find a unique fature for the parent '%parent' of type
               '%type' for the feature.", array('%parent' => $parent, '%type' => $parent_type));
      return 0;
    }
    $parent_feature = $results[0];

   // check to see if the relationship already exists if not then add it
    $values = array(
      'subject_id' => $feature->feature_id,
      'object_id' => $parent_feature->feature_id,
      'type_id' => $relcvterm->cvterm_id,
    );
    $results = chado_select_record('feature_relationship', array('feature_relationship_id'), $values);
    if (count($results) == 0) {
      $success = chado_insert_record('feature_relationship', $values);
      if (!$success) {
        tripal_report_error('T_fasta_loader', TRIPAL_ERROR, "Failed to add associate database accession '%accession' with feature",
          array('%accession' => $accession));
        return 0;
      }
    }
  }
}

/**
 * Adds the residues column to the feature.
 * 
 * This function seeks to the proper location in the file for the sequence
 * and reads in chunks of sequence and appends them to the feature.residues
 * column in the database.
 * 
 * @param unknown $fh
 * @param unknown $feature_id
 * @param unknown $seq_start
 * @param unknown $seq_end
 */
function tripal_feature_load_fasta_residues($fh, $feature_id, $seq_start, $seq_end) {

  // First position the file at the beginning of the sequence
  fseek($fh, $seq_start, SEEK_SET);
  $chunk_size = 100000000;
  $chunk = '';
  $seqlen = ($seq_end - $seq_start) + 1;

  // Calculate the interval at which we updated the precent complete.
  $interval = intval($seqlen * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }
  // We don't to repeat the update too often or it slows things down, so
  // if the interval is less than 1000 then bring it up to that.
  if ($interval < 100000) {
    $interval = 100000;
  }
  $chunk_intv_read = 0;
  $intv_read = 0;
  $num_read = 0;
  $total_seq_size = 0;
  
  // First, make sure we don't have a null in the residues
  $sql = "UPDATE {feature} SET residues = '' WHERE feature_id = :feature_id";
  chado_query($sql, array(':feature_id' => $feature_id));

  // Read in the lines until we reach the end of the sequence.  Once we 
  // get a specific bytes read then append the sequence to the one in the 
  // database.
  print "Sequence complete: 0%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";
  while ($line = fgets($fh)) {
    $num_read += strlen($line) + 1;
    $chunk_intv_read += strlen($line) + 1;
    $intv_read += strlen($line) + 1;
    $chunk .= trim($line);

    // If we've read in enough of the sequence then append it to the database.
    if ($chunk_intv_read >= $chunk_size) {
      $sql = "
        UPDATE {feature}
        SET residues = residues || :chunk
        WHERE feature_id = :feature_id
      ";
      $success = chado_query($sql, array(':feature_id' => $feature_id, ':chunk' => $chunk));
      if (!$success) {
        return FALSE;
      }
      $total_seq_size += strlen($chunk);
      $chunk = '';
      $chunk_intv_read = 0;
    }
    if ($intv_read >= $interval) {
      $percent = sprintf("%.2f", ($total_seq_size / $seqlen) * 100);
      print "Sequence complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";
      $intv_read = 0;
    }
    
    // If we've reached the ned of the sequence then break out of the loop
    if (ftell($fh) == $seq_end) {
      break;
    }
  }
  
  // write the last bit of sequence if it remains
  if (strlen($chunk) > 0) {
    $sql = "
        UPDATE {feature}
        SET residues = residues || :chunk
        WHERE feature_id = :feature_id
      ";
    $success = chado_query($sql, array(':feature_id' => $feature_id, ':chunk' => $chunk));
    if (!$success) {
      return FALSE;
    }
    $total_seq_size += strlen($chunk);
    $chunk = '';
    $chunk_intv_read = 0;
  }
  
  // Now update the seqlen and md5checksum fields
  $sql = "UPDATE {feature} SET seqlen = char_length(residues),  md5checksum = md5(residues) WHERE feature_id = :feature_id";
  chado_query($sql, array(':feature_id' => $feature_id));
  
  $percent = sprintf("%.2f", ($num_read / $seqlen) * 100);
  print "Sequence complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";
  

}
