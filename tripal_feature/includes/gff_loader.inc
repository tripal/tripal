<?php
/**
 * @file
 * @todo Add file header description
 */


/**
 * @defgroup gff3_loader GFF3 Feature Loader
 * @{
 * Provides gff3 loading functionality. Creates features based on their specification in a GFF3 file.
 * @}
 * @ingroup tripal_feature
 */

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form() {

  $form['gff_file']= array(
    '#type'          => 'textfield',
    '#title'         => t('GFF3 File'),
    '#description'   => t('Please enter the full system path for the GFF file, or a path within the Drupal
                           installation (e.g. /sites/default/files/xyz.gff).  The path must be accessible to the
                           server on which this Drupal instance is running.'),
    '#required' => TRUE,
    '#weight'        => 1
  );
  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $previous_db = tripal_db_set_active('chado');  // use chado database
  $org_rset = db_query($sql);
  tripal_db_set_active($previous_db);  // now use drupal database
  $organisms = array();
  $organisms[''] = '';
  while ($organism = db_fetch_object($org_rset)) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['organism_id'] = array(
    '#title'       => t('Organism'),
    '#type'        => t('select'),
    '#description' => t("Choose the organism to which these sequences are associated"),
    '#required'    => TRUE,
    '#options'     => $organisms,
  );
  $form['import_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import Options'),
    '#weight' => 6,
    '#collapsed' => TRUE
  );
  $form['import_options']['add_only']= array(
    '#type' => 'checkbox',
    '#title' => t('Import only new features'),
    '#required' => FALSE,
    '#description' => t('The job will skip features in the GFF file that already
                         exist in the database and import only new features.'),
    '#weight' => 2
  );
  $form['import_options']['update']= array(
    '#type' => 'checkbox',
    '#title' => t('Import all and update'),
    '#required' => FALSE,
    '#default_value' => 'checked',
    '#description' => t('Existing features will be updated and new features will be added.  Attributes
                         for a feature that are not present in the GFF but which are present in the
                         database will not be altered.'),
    '#weight' => 3
  );
  $form['import_options']['refresh']= array(
    '#type' => 'checkbox',
    '#title' => t('Import all and replace'),
    '#required' => FALSE,
    '#description' => t('Existing features will be updated and feature properties not
                         present in the GFF file will be removed.'),
    '#weight' => 4
  );
  $form['import_options']['remove']= array(
    '#type' => 'checkbox',
    '#title' => t('Delete features'),
    '#required' => FALSE,
    '#description' => t('Features present in the GFF file that exist in the database
                         will be removed rather than imported'),
    '#weight' => 5
  );

  $form['analysis'] = array(
    '#type' => 'fieldset',
    '#title' => t('Analysis Used to Derive Features'),
    '#weight' => 6,
    '#collapsed' => TRUE
  );
  $form['analysis']['desc'] = array(
    '#type' => 'markup',
    '#value' => t("Why specify an analysis for a data load?  All data comes
       from some place, even if downloaded from Genbank. By specifying
       analysis details for all data uploads, it allows an end user to reproduce the
       data set, but at least indicates the source of the data."),
  );

  // get the list of analyses
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $previous_db = tripal_db_set_active('chado');  // use chado database
  $org_rset = db_query($sql);
  tripal_db_set_active($previous_db);  // now use drupal database
  $analyses = array();
  $analyses[''] = '';
  while ($analysis = db_fetch_object($org_rset)) {
    $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
  }
  $form['analysis']['analysis_id'] = array(
   '#title'       => t('Analysis'),
   '#type'        => t('select'),
   '#description' => t("Choose the analysis to which these features are associated"),
   '#required'    => TRUE,
   '#options'     => $analyses,
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Import GFF3 file'),
    '#weight' => 10,
  );


  return $form;
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form_validate($form, &$form_state) {

  $gff_file = $form_state['values']['gff_file'];
  $organism_id = $form_state['values']['organism_id'];
  $add_only = $form_state['values']['add_only'];
  $update   = $form_state['values']['update'];
  $refresh  = $form_state['values']['refresh'];
  $remove   = $form_state['values']['remove'];

  // check to see if the file is located local to Drupal
  $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file;
  if (!file_exists($dfile)) {
    // if not local to Drupal, the file must be someplace else, just use
    // the full path provided
    $dfile = $gff_file;
  }
  if (!file_exists($dfile)) {
    form_set_error('gff_file', t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
  }

  // @coder-ignore: there are no functions being called here
  // @todo: break each line of this conditional into separate variables to make more readable
  if (($add_only AND ($update OR $refresh OR $remove)) OR
      ($update AND ($add_only OR $refresh OR $remove)) OR
      ($refresh AND ($update OR $add_only OR $remove)) OR
      ($remove AND ($update OR $refresh OR $add_only))) {
      form_set_error('add_only', t("Please select only one checkbox from the import options section"));
  }
}

/**
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form_submit($form, &$form_state) {
  global $user;

  $gff_file = $form_state['values']['gff_file'];
  $organism_id = $form_state['values']['organism_id'];
  $add_only = $form_state['values']['add_only'];
  $update   = $form_state['values']['update'];
  $refresh  = $form_state['values']['refresh'];
  $remove   = $form_state['values']['remove'];
  $analysis_id = $form_state['values']['analysis_id'];

  $args = array($gff_file, $organism_id, $analysis_id, $add_only, $update, $refresh, $remove);
  $type = '';
  if ($add_only) {
    $type = 'import only new features';
  }
  if ($update) {
    $type = 'import all and update';
  }
  if ($refresh) {
    $type = 'import all and replace';
  }
  if ($remove) {
    $type = 'delete features';
  }
  $fname = preg_replace("/.*\/(.*)/", "$1", $gff_file);
  tripal_add_job("$type GFF3 file: $fname", 'tripal_feature',
    'tripal_feature_load_gff3', $args, $user->uid);

  return '';
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3($gff_file, $organism_id, $analysis_id, 
  $add_only =0, $update = 0, $refresh = 0, $remove = 0, $job = NULL) {

  // Prepare log
  $filename = preg_replace("/.*\/(.*)/", "$1", $blastfile);
  $logfile = tempnam(sys_get_temp_dir(), "gff_loader_log.");
  $log = fopen($logfile, 'a'); // append parsing results to log file
  if (!$log) {
    print "ERROR: cannot open log file: $logfile\n";
    exit;
  }
  print "Writing to log file: $logfile\n";
  fwrite($log, date("D M j G:i:s Y") . ". Loading $gff_file\n");

  // this array is used to cache all of the features in the GFF file and
  // used to lookup parent and target relationships
  $gff_features = array();

  // check to see if the file is located local to Drupal
  $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file;
  if (!file_exists($dfile)) {
    // if not local to Drupal, the file must be someplace else, just use
    // the full path provided
    $dfile = $gff_file;
  }
  if (!file_exists($dfile)) {
    print "ERROR: cannot find the file: $dfile\n";
    return 0;
  }

  //$previous_db = tripal_db_set_active('chado');
  print "Opening $gff_file\n";

  //$lines = file($dfile,FILE_SKIP_EMPTY_LINES);
  $fh = fopen($dfile, 'r');
  if (!$fh) {
    print "ERROR: cannot open file: $dfile\n";
    return 0;
  }
  $filesize = filesize($dfile);

  // get the controlled vocaubulary that we'll be using.  The
  // default is the 'sequence' ontology
  // @coder-ignore: non-drupal schema thus table prefixing does not apply
  $sql = "SELECT * FROM cv WHERE name = '%s'";
  $cv = db_fetch_object(db_query($sql, 'sequence'));
  if (!$cv) {
    print "ERROR:  cannot find the 'sequence' ontology\n";
    return '';
  }

  // get the organism for which this GFF3 file belongs
  // @coder-ignore: non-drupal schema thus table prefixing does not apply
  $sql = "SELECT * FROM organism WHERE organism_id = %d";
  $organism = db_fetch_object(db_query($sql, $organism_id));

  $interval = intval($filesize * 0.01);
  if ($interval == 0) {
    $interval = 1;
  }
  $in_fasta = 0;
  $line_num = 0;
  $num_read = 0;
  $intv_read = 0;

  // iterate through each line of the GFF file
  print "Parsing Line $line_num (0.00%).\r";
  while ($line = fgets($fh)) {
  
    $line_num++;
    $num_read += drupal_strlen($line);   
    $intv_read += $num_read; 

    // update the job status every 1% features
    if ($job and $intv_read >= $interval) {
      $intv_read = 0;
      $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
      print "Parsing Line $line_num (" . $percent . "%).\r";
      fwrite($log, "Parsing line $line_num (" . $percent . "%).\n");
      tripal_job_set_progress($job, intval(($num_read / $filesize) * 100));
    }
    
    // check to see if we have FASTA section, if so then set the variable
    // to start parsing
    if (preg_match('/^##FASTA/i', $line)) {
      $in_fasta = 1;
      break;
    }
    
    // skip comments
    if (preg_match('/^#/', $line)) {
      continue;
    }
    
    // skip empty lines
    if (preg_match('/^\s*$/', $line)) {
      continue;
    }
    
    // TODO: handle FASTA section

    // get the columns
    $cols = explode("\t", $line);
    if (sizeof($cols) != 9) {
      fwrite($log, "ERROR: improper number of columns on line $line_num\n");
      fwrite($log, print_r($cols,1));
      return '';
    }
    
    // get the column values
    $landmark = $cols[0];
    $source   = $cols[1];
    $type     = $cols[2];
    $start    = $cols[3];
    $end      = $cols[4];
    $score    = $cols[5];
    $strand   = $cols[6];
    $phase    = $cols[7];
    $attrs    = explode(";", $cols[8]);  // split by a semicolon

    // ready the start and stop for chado.  Chado expects these positions
    // to be zero-based, so we substract 1 from the fmin
    $fmin = $start - 1;
    $fmax = $end;
    if ($end < $start) {
      $fmin = $end - 1;
      $fmax = $start;
    }
    
    // format the strand for chado
    if (strcmp($strand, '.') == 0) {
      $strand = 0;
    }
    elseif (strcmp($strand, '+') == 0) {
      $strand = 1;
    }
    elseif (strcmp($strand, '-') == 0) {
      $strand = -1;
    }
    if (strcmp($phase, '.') == 0) {
      $phase = '';
    }

    // get the type record
    if (!tripal_core_is_sql_prepared('sel_cvterm_cvid_cvtname_synonym')) {
      $psql = "PREPARE sel_cvterm_cvid_cvtname_synonym (int, text, text) AS
               SELECT CVT.cvterm_id, CVT.cv_id, CVT.name, CVT.definition,
                  CVT.dbxref_id, CVT.is_obsolete, CVT.is_relationshiptype
               FROM {cvterm} CVT
                  INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                  LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
               WHERE CV.cv_id = $1 and (CVT.name = $2 or CVTS.synonym = $3)";
       $status = chado_query($psql);
       if (!$status) {
         fwrite($log, "ERROR: cannot prepare statement 'sel_cvterm_cvid_cvtname_synonym' for ontology term $line_num\n");
         return '';
      }
      
    } 

    $result = chado_query("EXECUTE sel_cvterm_cvid_cvtname_synonym (%d, '%s', '%s')", $cv->cv_id, $type, $type);
    $cvterm = db_fetch_object($result);
    if (!$cvterm) {
      fwrite($log, "ERROR: cannot find ontology term '$type' on line $line_num.\n");
      return '';
    }

    // break apart each of the attributes
    $tags = array();
    $attr_name = '';
    $attr_uniquename = '';
    $attr_residue_info = '';
    $attr_locgroup = 0;
    $attr_fmin_partial = 'f';
    $attr_fmax_partial = 'f';
    $attr_is_obsolete = 'f';
    $attr_is_analysis = 'f';
    $attr_others = '';
    $residues = '';
    foreach ($attrs as $attr) {
      $attr = rtrim($attr);
      $attr = ltrim($attr);
      if (strcmp($attr, '')==0) {
        continue;
      }
      if (!preg_match('/^[^\=]+\=.+$/', $attr)) {
        fwrite($log, "ERROR: attribute is not correctly formatted on line $line_num: $attr\n");
        return '';
      }

      // break apart each tag
      $tag = preg_split("/=/", $attr, 2);  // split by equals sign
      
      // multiple instances of an attribute are separated by commas
      $tags[$tag[0]] = explode(",", $tag[1]);  // split by comma
      
      // replace the URL escape codes for each tag
      for ($i = 0; $i < count($tags[$tag[0]]); $i++) {
        $tags[$tag[0]][$i] = urldecode($tags[$tag[0]][$i]);          
      }
      
      // get the name and ID tags
      if (strcmp($tag[0], 'ID')==0) {
        $attr_uniquename = $tag[1];
      }
      elseif (strcmp($tag[0], 'Name')==0) {
        $attr_name = $tag[1];
      }
      // get the list of non-reserved attributes
      elseif (strcmp($tag[0], 'Alias')!=0        and strcmp($tag[0], 'Parent')!=0 and
              strcmp($tag[0], 'Target')!=0       and strcmp($tag[0], 'Gap')!=0 and
              strcmp($tag[0], 'Derives_from')!=0 and strcmp($tag[0], 'Note')!=0 and
              strcmp($tag[0], 'Dbxref')!=0       and strcmp($tag[0], 'Ontology_term')!=0 and
              strcmp($tag[0], 'Is_circular')!=0) {
          $attr_others[$tag[0]] = $tag[1];
      }
    }

    // if neither name nor uniquename are provided then generate one
    if (!$attr_uniquename and !$attr_name) {
      if (array_key_exists('Parent', $tags)) {
        $attr_uniquename = $tags['Parent'][0] . "-$type-$landmark:$fmin..$fmax";
      }
      else {
        fwrite($log, "ERROR: cannot generate a uniquename for feature on line $line_num\n");
        exit;
      }
      $attr_name = $attr_uniquename;
    }

    // if a name is not specified then use the unique name
    if (strcmp($attr_name, '')==0) {
      $attr_name = $attr_uniquename;
    }

    // if an ID attribute is not specified then use the attribute name and
    // hope for the best
    if (!$attr_uniquename) {
      $attr_uniquename = $attr_name;
    }

    // make sure the landmark sequence exists in the database.  We don't
    // know the type of the landmark so we'll hope that it's unique across
    // all types. If not we'll error out.  This test is only necessary if
    // if the landmark and the uniquename are different.  If they are the same
    // then this is the information for the landmark
    if (strcmp($landmark, $attr_uniquename) != 0 ) {
      $select = array(
         'organism_id' => $organism_id,
         'uniquename'  => $landmark,
      );
      $columns = array('count(*) as num_landmarks');
      $options = array('statement_name' => 'sel_feature_organismid_uniquename');
      $count = tripal_core_chado_select('feature', $columns, $select, $options);
      if (!$count or $count[0]->num_landmarks == 0) {
        fwrite($log,  "ERROR: the landmark '$landmark' cannot be found for this organism. ".
              "Please add the landmark and then retry the import of this GFF3 ".
              "file.\n");
        return '';

      }
      if ($count[0]->num_landmarks > 1) {
        fwrite($log,  "ERROR: the landmark '$landmark' is not unique for this organism. ".
              "The features cannot be associated.\n");
        return '';
      }  
    }

    // if the option is to remove or refresh then we want to remove
    // the feature from the database.
    if ($remove or $refresh) {
      fwrite($log,  "Removing feature '$attr_uniquename'\n");
      $sql = "DELETE FROM {feature}
              WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
      $match = array(
         'organism_id' => $organism->organism_id,
         'uniquename'  => $attr_uniquename,
         'type_id'     => $cvterm->cvterm_id
      );
      $result = tripal_core_chado_delete('feature',$match);
      if (!$result) {
        fwrite($log,  "ERROR: cannot delete feature $attr_uniquename\n");
      }
      $feature = 0;
      unset($result);
    }

    // add or update the feature and all properties
    if ($update or $refresh or $add_only) {

      // add/update the feature
      $feature = tripal_feature_load_gff3_feature($organism, $analysis_id, $cvterm,
        $attr_uniquename, $attr_name, $residues, $attr_is_analysis,
        $attr_is_obsolete, $add_only, $score, $log);

      // store all of the features for use later by parent and target
      // relationships
      $gff_features[$feature->uniquename]['type'] = $type;
      $gff_features[$feature->uniquename]['strand'] = $strand;

      if ($feature) {

        // add/update the featureloc if the landmark and the ID are not the same
        // if they are the same then this entry in the GFF is probably a landmark identifier
        if (strcmp($landmark, $attr_uniquename) !=0 ) {
          tripal_feature_load_gff3_featureloc($feature, $organism,
            $landmark, $fmin, $fmax, $strand, $phase, $attr_fmin_partial,
            $attr_fmax_partial, $attr_residue_info, $attr_locgroup, $log);
        }
        // add any aliases for this feature
        if (array_key_exists('Alias', $tags)) {
          tripal_feature_load_gff3_alias($feature, $tags['Alias'], $log);
        }
        // add any dbxrefs for this feature
        if (array_key_exists('Dbxref', $tags)) {
          tripal_feature_load_gff3_dbxref($feature, $tags['Dbxref'], $log);
        }
        // add any ontology terms for this feature
        if (array_key_exists('Ontology_term', $tags)) {
          tripal_feature_load_gff3_ontology($feature, $tags['Ontology_term'], $log);
        }
        // add parent relationships
        if (array_key_exists('Parent', $tags)) {
          tripal_feature_load_gff3_parents($feature, $cvterm, $tags['Parent'], $gff_features, $organism_id, $fmin, $log);
        }
        // add target relationships
        if (array_key_exists('Target', $tags)) {
          preg_match('/^(.*)\s+\d+\s+\d+\s+(\+|\-)*$/', $tags['Target'][0], $matches);
          $target_feature = $matches[1]; 
          $start = $matches[2]; 
          $end = $matches[3]; 
          if ($matches[4]) {
             $target_strand = $matches[4]; 
          }
          else {
             $target_strand = '.';
          }
          $target_fmin = $start - 1;
          $target_fmax = $end;
          if ($end < $start) {
            $target_fmin = $end - 1;
            $target_fmax = $start;
          }
          
          #print "Target: $target_feature, $target_fmin-$target_fmax $target_dir\n";
          tripal_feature_load_gff3_featureloc($feature, $organism,
            $target_feature, $target_fmin, $target_fmax, $target_strand, $phase, $attr_fmin_partial,
            $attr_fmax_partial, $attr_residue_info, $attr_locgroup, $log);
        }
        // add gap information.  This goes in simply as a property
        if (array_key_exists('Gap', $tags)) {
          tripal_feature_load_gff3_property($feature, 'Gap', $tags['Gap'][0], $log);
        }
        // add notes. This goes in simply as a property
        if (array_key_exists('Note', $tags)) {
          tripal_feature_load_gff3_property($feature, 'Note', $tags['Note'][0], $log);
        }
        // add the Derives_from relationship (e.g. polycistronic genes).
        if (array_key_exists('Derives_from', $tags)) {
          tripal_feature_load_gff3_derives_from($feature, $tags['Derives_from'][0], $gff_features, $organism, $log);
        }

        // add in the GFF3_source dbxref so that GBrowse can find the feature using the source column
        $source_ref = array('GFF_source:' . $source);
        tripal_feature_load_gff3_dbxref($feature, $source_ref, $log);

        // add any additional attributes
        if ($attr_others) {
          foreach ($attr_others as $property => $value) {
            tripal_feature_load_gff3_property($feature, $property, $value, $log);
          }
        }
      }
    }
  }
  // now set the rank of any parent/child relationships.  The order is based
  // on the fmin.  The start rank is 1.  This allows features with other
  // relationships to be '0' (the default), and doesn't interfer with the
  // ordering defined here.
  foreach ($gff_features as $parent => $details) {
    // only iterate through parents that have children
    if (array_key_exists('children',$details)) {
      // get the parent
      $values = array(
        'organism_id' => $organism->organism_id,
        'uniquename' => $parent,
        'type_id' => array(
           'cv_id' => array(
              'name' => 'sequence'
            ),
           'name' => $details['type'],
        ),
      );

      $options = array('statement_name' => 'sel_feature_organismid_uniquename_typeid');
      $result = tripal_core_chado_select('feature', array('*'), $values, $options);
      $pfeature = $result[0];

      // sort the children by order of their fmin positions (values of assoc. array)
      // if the parent is on the reverse strand then sort in reverse
      if ($details['strand'] == -1) {
        arsort($details['children']);
      }
      else {
        asort($details['children']);
      }

      // now iterate through the children and set their rank
      $rank = 1;
      fwrite($log,  "Updating child ranks for $parent (" . $details['type'] . ")\n");
      foreach ($details['children'] as $kfeature_id => $kfmin) {
        $match = array(
           'object_id' => $pfeature->feature_id,
           'subject_id' => $kfeature_id,
           'type_id' => array(
              'cv_id' => array(
                 'name' => 'relationship'
              ),
              'name' => 'part_of',
            ),
        );
        $values = array(
           'rank' => $rank,
        );

        $options = array('statement_name' => 'upd_featurerelationship_all');
        tripal_core_chado_update('feature_relationship', $match, $values, $options);
        $rank++;
      }
    }
  }

  //tripal_db_set_active($previous_db);
  
  print "Done.\nSuccessful and failed entries have been saved in the log file:\n $logfile\n";
  fwrite($log, "\n");
  fclose($log);
  
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_derives_from($feature, $subject, $gff_features, $organism, $log) {

   // first get the subject feature
  $match = array(
    'organism_id' => $organism->organism_id,
    'uniquename' => $subject,
    'type_id' => array(
      'name' => $gff_features[$subject]['type'],
      'cv_id' => array(
        'name' => 'sequence'
      ),
    ),
  );
  $options = array('statement_name' => 'sel_feature_organismid_uniquename_typeid');
  $sfeature = tripal_core_chado_select('feature', array('*'), $match, $options);
  if (count($sfeature)==0) {
    fwrite($log,  "ERROR: could not add 'Derives_from' relationship for $feature->uniquename and $subject.  Subject feature, '$subject', cannot be found\n");
    return;
  }

   // now check to see if the relationship already exists
  $values = array(
    'object_id' => $sfeature[0]->feature_id,
    'subject_id' => $feature->feature_id,
    'type_id' => array(
       'cv_id' => array(
          'name' => 'relationship'
        ),
       'name' => 'derives_from',
    ),
    'rank' => 0
  );
  $options = array('statement_name' => 'sel_featurerelationship_objectid_subjectid_typeid_rank');
  $rel = tripal_core_chado_select('feature_relationship', array('*'), $values, $options);
  if (count($rel) > 0) {
    fwrite($log, "   Relationship already exists: $feature->uniquename derives_from $subject\n");
    return;
  }

  // finally insert the relationship if it doesn't exist
  $options = array('statement_name' => 'ins_featurerelationship_objectid_subjectid_typeid_rank');
  $ret = tripal_core_chado_insert('feature_relationship', $values, $options);
  if (!$ret) {
    fwrite($log, "ERROR: could not add 'Derives_from' relationship for $feature->uniquename and $subject\n");
  }
  else {
    fwrite($log, "   Added relationship: $feature->uniquename derives_from $subject\n");
  }
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_parents($feature, $cvterm, $parents, &$gff_features, $organism_id, $fmin, $log) {

  $uname = $feature->uniquename;
  $type = $cvterm->name;
  $rel_type = 'part_of';

  // prepare these SQL statements that will be used repeatedly.
  if (!tripal_core_is_sql_prepared('sel_cvterm_cvname_cvtname_synonym')) {
    $psql = "PREPARE sel_cvterm_cvname_cvtname_synonym (text, text, text) AS
             SELECT CVT.cvterm_id
             FROM cvterm CVT
               INNER JOIN cv CV on CVT.cv_id = CV.cv_id
               LEFT JOIN cvtermsynonym CVTS on CVTS.cvterm_id = CVT.cvterm_id
             WHERE cv.name = $1 and (CVT.name = $2 or CVTS.synonym = $3)";
    $status = chado_query($psql);
    if (!$status) {
       fwrite($log, "ERROR: cannot prepare statement 'sel_cvterm_cvname_cvtname_synonym' for ontology term\n");
       return '';
    }
  }

  // iterate through the parents in the list
  foreach ($parents as $parent) {
    $parent_type = $gff_features[$parent]['type'];

    // try to find the parent
    $parentcvterm = db_fetch_object(chado_query("EXECUTE sel_cvterm_cvname_cvtname_synonym ('%s', '%s', '%s')", 'sequence', $parent_type, $parent_type));
    $relcvterm = db_fetch_object(chado_query("EXECUTE sel_cvterm_cvname_cvtname_synonym ('%s', '%s', '%s')", 'relationship', $rel_type, $rel_type));
    $values = array(
        'organism_id' => $organism_id,
        'uniquename' => $parent,
        'type_id' => $parentcvterm->cvterm_id,
    );
    $options = array('statement_name' => 'sel_feature_organismid_uniquename_typeid');
    $result = tripal_core_chado_select('feature', array('*'), $values, $options);
    $parent_feature = $result[0];

    // we want to add this feature to the child list for the parent
    // when the loader finishes, it will go back through the parent
    // features and rank the children by position
    $gff_features[$parent]['children'][$feature->feature_id] = $fmin;

    // if the parent exists then add the relationship otherwise print error and skip
    if ($parent_feature) {

      // check to see if the relationship already exists
      $values = array(
        'object_id' => $parent_feature->feature_id,
        'subject_id' => $feature->feature_id,
        'type_id' => $relcvterm->cvterm_id,
      );
      $options = array('statement_name' => 'sel_featurerelationship_objectid_subjectid_typeid');
      $rel = tripal_core_chado_select('feature_relationship', array('*'), $values, $options);

      if (count($rel) > 0) {
        fwrite($log, "   Relationship already exists, skipping '$uname' ($type) $rel_type '$parent' ($parent_type)\n");
      }
      else {
        // the relationship doesn't already exist, so add it.
        $values = array(
           'subject_id' => $feature->feature_id,
           'object_id'  => $parent_feature->feature_id,
           'type_id' => $relcvterm->cvterm_id,
        );
        $options = array('statement_name' => 'ins_featurerelationship_subjectid_objectid_typeid');
        $result = tripal_core_chado_insert('feature_relationship', $values, $options);
        if (!$result) {
          fwrite($log, "WARNING: failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type)\n");
        }
        else {
          fwrite($log, "   Inserted relationship relationship: '$uname' ($type) $rel_type '$parent' ($parent_type)\n");
        }
      }
    }
    else {
      fwrite($log, "WARNING: cannot establish relationship '$uname' ($type) $rel_type '$parent' ($parent_type): Cannot find the parent\n");
    }
  }
}

/**
 *
 *
 * @ingroup gff3_loader
 */

function tripal_feature_load_gff3_dbxref($feature, $dbxrefs, $log) {

  // iterate through each of the dbxrefs
  foreach ($dbxrefs as $dbxref) {

    // get the database name from the reference.  If it doesn't exist then create one.
    $ref = explode(":", $dbxref);
    $dbname = $ref[0];
    $accession = $ref[1];

    // first look for the database name if it doesn't exist then create one.
    // first check for the fully qualified URI (e.g. DB:<dbname>. If that
    // can't be found then look for the name as is.  If it still can't be found
    // the create the database
    $options = array('statement_name' => 'sel_db_name');
    $db = tripal_core_chado_select('db', array('db_id'), array('name' => "DB:$dbname"), $options);
    if (count($db) == 0) {
      $db = tripal_core_chado_select('db', array('db_id'), array('name' => "$dbname"), $options);
    }
    if (count($db) == 0) {
      $options = array('statement_name' => 'ins_db_name');
      $ret = tripal_core_chado_insert('db', array('name' => $dbname,
        'description' => 'Added automatically by the GFF loader'), $options);
      if ($ret) {
        fwrite($log, "   Added new database: $dbname\n");
        $options = array('statement_name' => 'sel_db_name');
        $db = tripal_core_chado_select('db', array('db_id'), array('name' => "$dbname"), $options);
      }
      else {
        fwrite($log, "ERROR: cannot find or add the database $dbname\n");
        exit;
        return 0;
      }
    }
    $db = $db[0];

    // now check to see if the accession exists
    $options = array('statement_name' => 'sel_dbxref_accession_dbid');
    $dbxref = tripal_core_chado_select('dbxref', array('dbxref_id'), array(
      'accession' => $accession, 'db_id' => $db->db_id), $options);

    // if the accession doesn't exist then we want to add it
    if (sizeof($dbxref) == 0) {
      $options = array('statement_name' => 'ins_dbxref_dbid_accession_version');
      $ret = tripal_core_chado_insert('dbxref', array('db_id' => $db->db_id,
        'accession' => $accession, 'version' => ''), $options);
      $options = array('statement_name' => 'sel_dbxref_accession_dbid');
      $dbxref = tripal_core_chado_select('dbxref', array('dbxref_id'), array(
        'accession' => $accession, 'db_id' => $db->db_id), $options);
    }
    $dbxref = $dbxref[0];

    // check to see if this feature dbxref already exists
    $options = array('statement_name' => 'sel_featuredbxref_dbxrefid_featureid');
    $fdbx = tripal_core_chado_select('feature_dbxref', array('feature_dbxref_id'),
      array('dbxref_id' => $dbxref->dbxref_id, 'feature_id' => $feature->feature_id), $options);

    // now associate this feature with the database reference if it doesn't
    // already exist
    if (sizeof($fdbx)==0) {
      $options = array('statement_name' => 'ins_featuredbxref_dbxrefid_featureid');
      $ret = tripal_core_chado_insert('feature_dbxref', array(
        'dbxref_id' => $dbxref->dbxref_id,
        'feature_id' => $feature->feature_id), $options);
      if ($ret) {
        fwrite($log, "   Adding Dbxref $dbname:$accession\n");
      }
      else {
        fwrite($log, "ERROR: failed to insert Dbxref: $dbname:$accession\n");
        return 0;
      }
    }
    else {
      fwrite($log, "   Dbxref already associated, skipping $dbname:$accession\n");
    }
  }
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_ontology($feature, $dbxrefs, $log) {

   // iterate through each of the dbxrefs
  foreach ($dbxrefs as $dbxref) {

    // get the database name from the reference.  If it doesn't exist then create one.
    $ref = explode(":", $dbxref);
    $dbname = $ref[0];
    $accession = $ref[1];

    // first look for the database name
    $options = array('statement_name' => 'sel_db_name');
    $db = tripal_core_chado_select('db', array('db_id'), array('name' => "DB:$dbname"), $options);
    if (sizeof($db) == 0) {
      $db = tripal_core_chado_select('db', array('db_id'), array('name' => "$dbname"), $options);
    }
    if (sizeof($db) == 0) {
      fwrite($log, "ERROR: Database, $dbname is missing for reference: $dbname:$accession\n");
      return 0;
    }
    $db = $db[0];

    // now check to see if the accession exists
    $options = array('statement_name' => 'sel_dbxref_accession_dbid');
    $dbxref = tripal_core_chado_select('dbxref', array('dbxref_id'), array(
       'accession' => $accession, 'db_id' => $db->db_id), $options);
    if (sizeof($dbxref) == 0) {
      fwrite($log, "ERROR: Accession, $accession is missing for reference: $dbname:$accession\n");
      return 0;
    }
    $dbxref = $dbxref[0];

    // now check to see if the cvterm exists
    $options = array('statement_name' => 'sel_cvterm_dbxrefid');
    $cvterm = tripal_core_chado_select('cvterm', array('cvterm_id'), array(
       'dbxref_id' => $dbxref->dbxref_id), $options);
    // if it doesn't exist in the cvterm table, look for an alternate id
    if (sizeof($cvterm) == 0) {
      $options = array('statement_name' => 'sel_cvtermdbxref_dbxrefid');
      $cvterm = tripal_core_chado_select('cvterm_dbxref', array('cvterm_id'), array(
        'dbxref_id' => $dbxref->dbxref_id), $options);
    }
    if (sizeof($cvterm) == 0) {
      fwrite($log, "ERROR: CV Term is missing for reference: $dbname:$accession\n");
      return 0;
    }
    $cvterm = $cvterm[0];


    // check to see if this feature cvterm already exists
    $options = array('statement_name' => 'sel_featurecvterm_cvtermid_featureid');
    $fcvt = tripal_core_chado_select('feature_cvterm', array('feature_cvterm_id'),
      array('cvterm_id' => $cvterm->cvterm_id, 'feature_id' => $feature->feature_id),
      $options);

    // now associate this feature with the cvterm if it doesn't already exist
    if (sizeof($fcvt)==0) {
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id,
        'feature_id' => $feature->feature_id,
        'pub_id' => array(
          'uniquename' => 'null',
        ),
      );
      $options = array('statement_name' => 'ins_featurecvterm_cvtermid_featureid_pubid');
      $ret = tripal_core_chado_insert('feature_cvterm', $values, $options);

      if ($ret) {
        fwrite($log, "   Adding ontology term $dbname:$accession\n");
      }
      else {
        fwrite($log, "ERROR: failed to insert ontology term: $dbname:$accession\n");
        return 0;
      }
    }
    else {
      fwrite($log, "   Ontology term already associated, skipping $dbname:$accession\n");
    }
  }
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_alias($feature, $aliases, $log) {

  // make sure we have a 'synonym_type' vocabulary
  $select = array('name' => 'synonym_type');
  $options = array('statement_name' => 'sel_cv_name');  
  $results = tripal_core_chado_select('cv', array('*'), $selct, $options);
  if (count($results) > 0) {
    // insert the 'synonym_type' vocabulary
    $values = array(
      'name' => 'synonym_type',
      'definition' => 'vocabulary for synonym types',
    );
    $options = array('statement_name' => 'ins_cv_name_definition');
    $result = tripal_core_chado_insert('cv', $values, $options);
    if (!result) {
      fwrite($log, "ERROR: Failed to add the synonyms type vocabulary");
      return 0;
    }
    // now that we've added the cv we need to get the record
    $options = array('statement_name' => 'sel_cv_name');
    $results = tripal_core_chado_select('cv', array('*'), $select, $options);
    $syncv = $results[0];
  } 
  else {
    $syncv = $results[0];
  }

  // get the 'exact' cvterm, which is the type of synonym we're adding
  $select = array(
     'name' => 'exact',
     'cv_id' => array(
        'name' => 'synonym_type'
     ),
  );
  $options = array('statement_name' => 'sel_cvterm_name_cvid');
  $result = tripal_core_chado_select('cvterm', array('*'), $select, $options);
  $syntype = $result[0];
  if (!$syntype) {
    $term = array(
      'name' => array('exact'),
      'id' => array("internal:exact"),
      'definition' => array(''),
      'is_obsolete' => array(0),
    );
    // TODO: fix the function so it uses prepared statements    
    $syntype = tripal_cv_add_cvterm($term, $syncv->name, 0, 1);
    if (!$syntype) {
      fwrite($log, "Cannot add synonym type: internal:$type");
      return 0;
    }
  }

  // iterate through all of the aliases and add each one
  foreach ($aliases as $alias) {
    fwrite($log, "   Adding Alias $alias\n");

    // check to see if the alias already exists in the synonym table
    // if not, then add it
    $select = array(
       'name' => $alias,
       'type_id' => $syntype->cvterm_id,
    );
    $options = array('statement_name' => 'sel_synonym_name_typeid');
    $result = tripal_core_chado_select('synonym', array('*'), $select, $options);
    $synonym = $result[0];
    if (!$synonym) {
      $values = array(
         'name' => $alias,
         'type_id' => $syntype->cvterm_id,
         'synonym_sgml' => '',
      );
      $options = array('statement_name' => 'ins_synonym_name_typeid_synonymsgml');
      $result = tripal_core_chado_insert('synonym', $values, $options);
      if (!$result) {
        fwrite($log, "ERROR: cannot add alias $alias to synonym table\n");
      }
    }
    $options = array('statement_name' => 'sel_synonym_name_typeid');
    $result = tripal_core_chado_select('synonym', array('*'), $select, $options);
    $synonym = $result[0];


    // check to see if we have a NULL publication in the pub table.  If not,
    // then add one.
    // @coder-ignore: non-drupal schema thus table prefixing does not apply
    $select = array('uniquename' => 'null');
    $options = array('statement_name' => 'sel_pub_uniquename');
    $result = tripal_core_chado_select('pub', array('*'), $select, $options);
    $pub = $result[0];
    if (!$pub) {
      // prepare the statement
      if (!tripal_core_is_sql_prepared('ins_pub_uniquename_typeid')) {
        $psql = "PREPARE ins_pub_uniquename_typeid (text, text) AS
                 INSERT INTO pub (uniquename,type_id) VALUES ('%s',
                 (SELECT cvterm_id
                  FROM cvterm CVT
                    INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
                    INNER JOIN db DB on DB.db_id = DBX.db_id
                  WHERE CVT.name = $1 and DB.name = $2)";
        $status = chado_query($psql);
        if (!$status) {
          fwrite($log, "ERROR: cannot prepare statement 'ins_pub_uniquename_typeid'\n");
          return 0;
        } 
      }    
      // insert the null pub 
      $result = db_fetch_object(chado_query("EXECUTE ins_pub_uniquename_typeid ('%s', '%s')", 'null', 'null'));
      if (!$result) {
        fwrite($log, "ERROR: cannot add null publication needed for setup of alias\n");
        return 0;
      }
    }
    $options = array('statement_name' => 'sel_pub_uniquename');
    $result = tripal_core_chado_select('pub', array('*'), $select, $options);
    $pub = $result[0];

    // check to see if the synonym exists in the feature_synonym table
    // if not, then add it.
    $select = array(
       'synonym_id' => $synonym->synonym_id,
       'feature_id' => $feature->feature_id,
       'pub_id' => $pub->pub_id,
    );
    $options = array('statement_name', 'sel_synonymfeature_synonymid_featureid_pubid');
    $result = tripal_core_chado_select('feature_synonym', array('*'), $select, $options);
    $fsyn = $result[0];
    if (!$fsyn) {
      $values = array(
        'synonym_id' => $synonym->synonym_id,
        'feature_id' => $feature->feature_id,
        'pub_id' => $pub->pub_id,
      );
      $options = array('statement_name', 'ins_synonymfeature_synonymid_featureid_pubid');
      $result = tripal_core_chado_insert('feature_synonym', $values, $options);
      if (!$result) {
        fwrite($log, "ERROR: cannot add alias $alias to feature synonym table\n");
        return 0;
      }
    }
    else {
      fwrite($log, "   Synonym $alias already exists. Skipping\n");
    }
  }
  return 1;
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_feature($organism, $analysis_id, $cvterm, $uniquename, $name,
  $residues, $is_analysis='f', $is_obsolete='f', $add_only, $score, $log) {

  // check to see if the feature already exists
  $fselect = array(
     'organism_id' => $organism->organism_id,
     'uniquename' => $uniquename,
     'type_id' => $cvterm->cvterm_id
  );
  $options = array('statement_name' => 'sel_feature_organismid_uniquename_typeid');
  $result = tripal_core_chado_select('feature', array('*'), $fselect, $options); 
  $feature = $result[0];

  if (strcmp($is_obsolete, 'f')==0 or $is_obsolete == 0) {
    $is_obsolete = 'false';
  }
  if (strcmp($is_obsolete, 't')==0 or $is_obsolete == 1) {
    $is_obsolete = 'true';
  }
  if (strcmp($is_analysis, 'f')==0 or $is_analysis == 0) {
    $is_analysis = 'false'; 
  }
  if (strcmp($is_analysis, 't')==0 or $is_analysis == 1) {
    $is_analysis = 'true'; 
  }

  // insert the feature if it does not exist otherwise perform an update
  if (!$feature) {
    fwrite($log, "   Adding feature '$uniquename' ($cvterm->name)\n");
    $values = array(
       'organism_id' => $organism->organism_id,
       'name' => $name,
       'uniquename' => $uniquename,
       'residues' => $residues,
       'seqlen' => drupal_strlen($residues),
       'md5checksum' => md5($residues),
       'type_id' => $cvterm->cvterm_id,
       'is_analysis' => $is_analysis,
       'is_obsolete' => $is_obsolete,
    );
    $options = array('statement_name' => 'ins_feature_all');
    $result = tripal_core_chado_insert('feature', $values, $options);
    if (!$result) {
      fwrite($log, "ERROR: failed to insert feature '$uniquename' ($cvterm->name)\n");
      return 0;
    }
  }
  elseif (!$add_only) {
    fwrite($log, "Updating feature '$uniquename' ($cvterm->name)\n");
    $values = array(
      'name' => $name,
      'residues' => $residues,
      'seqlen' => drupal_strlen($residues),
      'md5checksum' => md5($residues),
      'is_analysis' => $is_analysis,
      'is_obsolete' => $is_obsolete,
    );    
    $match = array(
      'organism_id' => $organism->organism_id,
      'uniquename' => $uniquename,
      'type_id' => $cvterm->cvterm_id,
    );
    $options = array('statement_name' => 'upd_feature');
    $result = tripal_core_chado_update('feature', $match, $values, $options);
    if (!$result) {
      fwrite($log, "ERROR: failed to update feature '$uniquename' ($cvterm->name)\n");
      return 0;
    }
  }
  else {
    // the feature exists and we don't want to update it so return
    // a value of 0.  This will stop all downstream property additions
    fwrite($log, "Skipping existing feature: '$uniquename' ($cvterm->name).\n");
    return 0;
  }

  // get the newly added feature
  $options = array('statement_name' => 'sel_feature_organismid_uniquename_typeid');
  $result = tripal_core_chado_select('feature', array('*'), $fselect, $options);  
  $feature = $result[0];

  // add the analysisfeature entry to the analysisfeature table if it doesn't already exist
  $af_values = array(
    'analysis_id' => $analysis_id,
    'feature_id' => $feature->feature_id
  );
  $options = array('statement_name' => 'sel_analysisfeature_analysisid_featureid');
  $afeature = tripal_core_chado_select('analysisfeature', array('analysisfeature_id'), $af_values, $options);
  if (count($afeature)==0) {
    // if a score is available then set that to be the significance field
    if (strcmp($score, '.') != 0) {
      $af_values['significance'] = $score;
      $options = array('statement_name' => 'ins_analysisfeature_analysisid_featureid_significance');
    } 
    else {
      $options = array('statement_name' => 'ins_analysisfeature_analysisid_featureid');
    }
    if (!tripal_core_chado_insert('analysisfeature', $af_values, $options)) {
      fwrite($log, "ERROR: could not add analysisfeature record: $analysis_id, $feature->feature_id\n");
    }
    else {
      fwrite($log, "   Added analysisfeature record\n");
    }
  }
  else {
    // if a score is available then set that to be the significance field
    $new_vals = array();
    if (strcmp($score, '.')!=0) {
      $new_vals['significance'] = $score;
    }
    else {
      $new_vals['significance'] = '__NULL__';
    }
    if (!$add_only) {
      $options = array('statement_name' => 'upd_analysisfeature');
      $ret = tripal_core_chado_update('analysisfeature', $af_values, $new_vals, $options);
      if (!$ret) {
        fwrite($log, "ERROR: could not update analysisfeature record: $analysis_id, $feature->feature_id\n");
      }
      else {
        fwrite($log, "   Updated analysisfeature record\n");
      }
    }
  }
  return $feature;
}

/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_featureloc($feature, $organism, $landmark, $fmin,
  $fmax, $strand, $phase, $is_fmin_partial, $is_fmax_partial, $residue_info, $locgroup, $log) {

  $select = array(
    'organism_id' => $organism->organism_id,
    'uniquename' => $landmark,
  );
  $options = array('statement_name' => 'sel_feature_organism_id_uniquename');
  $r = tripal_core_chado_select('feature', array('*'), $select, $options);
  if (count($r)==0) {
    // so we couldn't find it using the uniquename. Let's try the 'name'.
    // if we return only a singe result then we can proceed. Otherwise give an
    // error message
    $select = array(
      'organism_id' => $organism->organism_id,
      'name' => $landmark,
    );
    $options = array('statement_name' => 'sel_feature_organism_id_name');
    $r = tripal_core_chado_select('feature', array('*'), $select, $options);
    if (count($r) == 0){
       fwrite($log, "ERROR: cannot find landmark feature: '$landmark'.  Cannot add the feature location record\n");
       return 0;
    } 
    elseif (count($r) > 1) {
       fwrite($log, "ERROR: multiple landmarks exist with the name: '$landmark'.  Cannot resolve which one to use. Cannot add the feature location record\n");
       return 0;    
    }
    
  }
  $srcfeature = $r[0];

  // TODO: create an attribute that recognizes the residue_info,locgroup, 
  //  is_fmin_partial and is_fmax_partial, right now these are
  //  hardcoded to be false and 0 below.


  // check to see if this featureloc already exists, but also keep track of the
  // last rank value
  $rank = 0;
  $exists = 0;
  $select = array('feature_id' => $feature->feature_id);
  $options = array(
    'statement_name' => 'sel_featureloc_feature_id',
    'order_by' => array(
       'rank' => 'ASC'
    ),
  );  
  $locrecs = tripal_core_chado_select('featureloc', array('*'), $select, $options);

  foreach ($locrecs as $featureloc) {
    $select = array('feature_id' => $featureloc->srcfeature_id);
    $options = array('statement_name' => 'sel_feature_feature_id');
    $locsfeature = tripal_core_chado_select('feature', array('*'), $select, $options);

    // check to make sure we don't already have this featureloc record
    // if we do we don't want to readd it
//    print "Src Name:'" . $locsfeature[0]->name . "' == '$landmark'\n";
//    print "Fmin:    '$featureloc->fmin' == '$fmin'\n";
//    print "Fmax:    '$featureloc->fmax' == '$fmax'\n";
//    print "Strand:  '$featureloc->strand' == '$strand'\n";        
    
    // the source feature name and at least the fmin and fmax must be the same
    // for an update of the featureloc, otherwise we'll insert a new record.
    if (strcmp($locsfeature[0]->name, $landmark)==0 and 
       ($featureloc->fmin == $fmin or $featureloc->fmax == $fmax)) {
      $match = array('featureloc_id' => $featureloc->featureloc_id);
      $values = array();
      $exists = 1;
      if ($featureloc->fmin != $fmin) {
         $values['fmin'] = $fmin;
      }
      if ($featureloc->fmax != $fmax) {
         $values['fmax'] = $fmax;
      }
      if ($featureloc->strand != $strand) {
         $values['strand'] = $strand;
      }
      if (count($values) > 0) {
        $options = array('statement_name' => 'upd_featureloc_all');
        tripal_core_chado_update('featureloc', $match, $values, $options);
        fwrite($log, "   Updated featureloc\n");
      }
      else {
        fwrite($log, "   No change to featureloc\n");
      }
    }
    $rank = $featureloc->rank + 1;
  }
  if (!$exists) {

    // this feature location is new so add it
    if (!$phase) {
      $phase = 'NULL';
    }
    if (strcmp($is_fmin_partial, 'f')==0 or !$is_fmin_partial) {
      $is_fmin_partial = 'false';
    }
    elseif (strcmp($is_fmin_partial, 't')==0 or $is_fmin_partial = 1) {
      $is_fmin_partial = 'true';
    }
    if (strcmp($is_fmax_partial, 'f')==0 or !$is_fmax_partial) {
      $is_fmax_partial = 'false';
    }
    elseif (strcmp($is_fmax_partial, 't')==0 or $is_fmax_partial = 1) {
      $is_fmax_partial = 'true';
    }
    fwrite($log, "   Adding featureloc $srcfeature->uniquename fmin: $fmin (is_partial: $is_fmin_partial), fmax: $fmax (is_partial: $is_fmin_partial), strand: $strand, phase: $phase, rank: $rank\n");
    $values = array(
       'feature_id'      => $feature->feature_id,
       'srcfeature_id'   => $srcfeature->feature_id,
       'fmin'            => $fmin,
       'is_fmin_partial' => $is_fmin_partial,
       'fmax'            => $fmax,
       'is_fmax_partial' => $is_fmax_partial,
       'strand'          => $strand,
       'phase'           => $phase,
       'residue_info'    => $residue_info,
       'locgroup'        => $locgroup,
       'rank'            => $rank 
    );
    $options = array('statement_name' => 'ins_featureloc_all');
    $success = tripal_core_chado_insert('featureloc', $values, $options);
    if (!$success) {
      fwrite($log, "ERROR: failed to insert featureloc\n");
      exit;
      return 0;
    }
  }
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_property($feature, $property, $value, $log) {

  // first make sure the cvterm exists.  if not, then add it
  $select = array(
     'name' => $property,
     'cv_id' => array(
        'name' => 'feature_property',
     ),
  );
  $options = array('statement_name' => 'sel_cvterm_name_cvid');
  $result = tripal_core_chado_select('cvterm', array('*'), $select, $options);
  $cvterm = $result[0];
  if (!$cvterm) {
    $term = array(
      'id' => "null:$property",
      'name' => $property,
      'namespace' => 'feature_property',
      'is_obsolete' => 0,
    );
    fwrite($log, "   Adding cvterm, $property\n");
    $cvterm = (object) tripal_cv_add_cvterm($term, 'feature_property', 0, 0);
  }

  if (!$cvterm) {
    fwrite($log, "ERROR: cannot add cvterm, $property\n");
    exit;
  }

  // check to see if the property already exists for this feature
  // if it does but the value is unique then increment the rank and add it.
  // if the value is not unique then don't add it.
  $add = 1;
  $rank = 0;
  $select = array(
     'feature_id' => $feature->feature_id,
     'type_id' => $cvterm->cvterm_id,
  );
  $options = array(
    'statement_name' => 'sel_featureprop_featureid_typeid',
    'order_by' => array(
      'rank' => 'ASC',
    ),
  );
  $results = tripal_core_chado_select('featureprop', array('*'), $select, $options);
  foreach ($results as $prop){
    if (strcmp($prop->value, $value)==0) {
      $add = NULL; // don't add it, it already exists
      fwrite($log, "   Property already exists, skipping\n");
    }
    $rank = $prop->rank + 1;
  }

  // add the property if we pass the check above
  if ($add) {
    fwrite($log, "   Setting feature property. $property: $value\n");
    $values = array(
       'feature_id' => $feature->feature_id,
       'type_id' => $cvterm->cvterm_id,
       'value' => $value,
       'rank' => $rank,
    );
    $options = array('statement_name' => 'ins_featureprop_all');
    $result = tripal_core_chado_insert('featureprop', $values, $options);
    if(!$result){
      fwrite($log, "ERROR: cannot add featureprop, $property\n");
    }
  }
}

