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
  $org_rset = chado_query($sql);
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
  $form['import_options']['use_transaction']= array(
    '#type' => 'checkbox',
    '#title' => t('Use a transaction'),
    '#required' => FALSE,
    '#description' => t('Use a database transaction when loading the GFF file.  If an error occurs 
      the entire datset loaded prior to the failure will be rolled back and will not be available
      in the database.  If this option is unchecked and failure occurs all records up to the point
      of failure will be present in the database.'),
    '#weight' => 1
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
  $org_rset = chado_query($sql);
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
  $use_transaction   = $form_state['values']['use_transaction'];
  

  // check to see if the file is located local to Drupal
  $gff_file = trim($gff_file);
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
  if (($add_only AND ($update   OR $refresh  OR $remove)) OR
      ($update   AND ($add_only OR $refresh  OR $remove)) OR
      ($refresh  AND ($update   OR $add_only OR $remove)) OR
      ($remove   AND ($update   OR $refresh  OR $add_only))) {
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
  $use_transaction   = $form_state['values']['use_transaction'];

  $args = array($gff_file, $organism_id, $analysis_id, $add_only, 
    $update, $refresh, $remove, $use_transaction);
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
  $add_only =0, $update = 0, $refresh = 0, $remove = 0, $use_transaction = 1, 
  $job = NULL) {  

  // make sure our temporary table exists
  $ret = array(); 
  if (!db_table_exists('tripal_gff_temp')) { 
    $schema = tripal_feature_get_custom_tables('tripal_gff_temp');  
    $success = tripal_core_create_custom_table($ret, 'tripal_gff_temp', $schema['tripal_gff_temp']);
    if (!$success) {
      watchdog('T_gff3_loader', "Cannot create temporary loading table", array(), WATCHDOG_ERROR); 
      return;
    } 
  }
  // empty the temp table
  $sql = "DELETE FROM tripal_gff_temp";
  chado_query($sql);
    
  // begin the transaction
  if ($use_transaction) {
    $connection = tripal_db_start_transaction();
        
    // if we cannot get a connection then let the user know the loading will be slow
    if (!$connection) {
       print "A persistant connection was not obtained. Loading will be slow\n";
    }
    else {
       print "\nNOTE: Loading of this GFF file is performed using a database transaction. \n" .
             "If the load fails or is terminated prematurely then the entire set of \n" .
             "insertions/updates is rolled back and will not be found in the database\n\n";
    }
  }
  else {
    $connection = tripal_db_persistent_chado();
    if (!$connection) {
       print "A persistant connection was not obtained. Loading will be slow\n";
    }
  }

  // check to see if the file is located local to Drupal
  $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file;
  if (!file_exists($dfile)) {
    // if not local to Drupal, the file must be someplace else, just use
    // the full path provided
    $dfile = $gff_file;
  }
  if (!file_exists($dfile)) {
    watchdog('T_gff3_loader', "Cannot find the file: %dfile", 
      array('%dfile' => $dfile), WATCHDOG_ERROR);
    return 0;
  }

  print "Opening $gff_file\n";

  //$lines = file($dfile,FILE_SKIP_EMPTY_LINES);
  $fh = fopen($dfile, 'r');
  if (!$fh) {
    watchdog('T_gff3_loader', "cannot open file: %dfile", 
      array('%dfile' => $dfile), WATCHDOG_ERROR);
    return 0;
  }
  $filesize = filesize($dfile);

  // get the controlled vocaubulary that we'll be using.  The
  // default is the 'sequence' ontology
  // @coder-ignore: non-drupal schema thus table prefixing does not apply
  $sql = "SELECT * FROM cv WHERE name = '%s'";
  $cv = db_fetch_object(chado_query($sql, 'sequence'));
  if (!$cv) {   
    watchdog('T_gff3_loader', "Cannot find the 'sequence' ontology", 
      array(), WATCHDOG_ERROR);
    return '';
  }

  // get the organism for which this GFF3 file belongs
  // @coder-ignore: non-drupal schema thus table prefixing does not apply
  $sql = "SELECT * FROM organism WHERE organism_id = %d";
  $organism = db_fetch_object(chado_query($sql, $organism_id));

  $interval = intval($filesize * 0.01);
  if ($interval == 0) {
    $interval = 1;
  }
  $in_fasta = 0;
  $line_num = 0;
  $num_read = 0;
  $intv_read = 0;

  // iterate through each line of the GFF file
  print "Parsing Line $line_num (0.00%). Memory: " . number_format(memory_get_usage()) . " bytes\r";
  while ($line = fgets($fh)) {

    $line_num++;
    $num_read += drupal_strlen($line);   
    $intv_read += $num_read; 

    // update the job status every 1% features
    if ($job and $intv_read >= $interval) {
      $intv_read = 0;
      $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
      print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
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
      watchdog('T_gff3_loader', 'improper number of columns on line %line_num', 
        array('%line_num' => $line_num), WATCHDOG_ERROR);
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
    if (!tripal_core_is_sql_prepared('sel_cvterm_idnasy')) {
      $psql = "PREPARE sel_cvterm_idnasy (int, text, text) AS
               SELECT CVT.cvterm_id, CVT.cv_id, CVT.name, CVT.definition,
                  CVT.dbxref_id, CVT.is_obsolete, CVT.is_relationshiptype
               FROM {cvterm} CVT
                  INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
                  LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
               WHERE CV.cv_id = $1 and (CVT.name = $2 or CVTS.synonym = $3)";
       $status = chado_query($psql);
       if (!$status) {
         watchdog('T_gff3_loader', 'cannot prepare statement \'sel_cvterm_idnasy\' for ontology term %line_num', 
           array('%line_num' => $line_num), WATCHDOG_ERROR);
         return '';
      }
      
    } 
  
    $result = chado_query("EXECUTE sel_cvterm_idnasy (%d, '%s', '%s')", $cv->cv_id, $type, $type);
   
    $cvterm = db_fetch_object($result);
    if (!$cvterm) {
      watchdog('T_gff3_loader', 'cannot find ontology term \'%type\' on line %line_num', 
        array('%type' => $type, '%line_num' => $line_num), WATCHDOG_ERROR);
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
        watchdog('T_gff3_loader', 'Attribute is not correctly formatted on line %line_num: %attr', 
          array('%line_num' => $line_num, '%attr' => $attr), WATCHDOG_ERROR);
        return '';
      }

      // break apart each tag
      $tag = preg_split("/=/", $attr, 2);  // split by equals sign
      
      // multiple instances of an attribute are separated by commas
      $tag_name = $tag[0];
      if (!array_key_exists($tag_name, $tags)) {
        $tags[$tag_name] = array();
      }
      $tags[$tag_name] = array_merge($tags[$tag_name], explode(",", $tag[1]));  // split by comma
      
      
      // replace the URL escape codes for each tag
      for ($i = 0; $i < count($tags[$tag_name]); $i++) {
        $tags[$tag_name][$i] = urldecode($tags[$tag_name][$i]);                  
      }
      
      // get the name and ID tags
      if (strcmp($tag_name, 'ID') == 0) {
        $attr_uniquename =  urldecode($tag[1]);
      }
      elseif (strcmp($tag_name, 'Name') == 0) {
        $attr_name =  urldecode($tag[1]);
      }
      // get the list of non-reserved attributes
      elseif (strcmp($tag_name, 'Alias') !=0        and strcmp($tag_name, 'Parent') !=0 and
              strcmp($tag_name, 'Target') !=0       and strcmp($tag_name, 'Gap') !=0 and
              strcmp($tag_name, 'Derives_from') !=0 and strcmp($tag_name, 'Note') !=0 and
              strcmp($tag_name, 'Dbxref') !=0       and strcmp($tag_name, 'Ontology_term') !=0 and
              strcmp($tag_name, 'Is_circular') !=0) {
        foreach ($tags[$tag_name] as $value) {
          $attr_others[$tag_name][] = $value;
        }
      }
    }

    // if neither name nor uniquename are provided then generate one
    if (!$attr_uniquename and !$attr_name) {
      if (array_key_exists('Parent', $tags)) {
        $attr_uniquename = $tags['Parent'][0] . "-$type-$landmark:$fmin..$fmax";
      }
      else {
        watchdog('T_gff3_loader', 'Cannot generate a uniquename for feature on line %line_num', 
          array('%line_num' => $line_num), WATCHDOG_ERROR);
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
        watchdog('T_gff3_loader', "The landmark '%landmark' cannot be found for this organism. ".
              "Please add the landmark and then retry the import of this GFF3 ".
              "file", array('%landmark' => $landmark), WATCHDOG_ERROR);
        return '';

      }
      if ($count[0]->num_landmarks > 1) {
        watchdog('T_gff3_loader', "The landmark '%landmark' is not unique for this organism. ".
              "The features cannot be associated", array('%landmark' => $landmark), WATCHDOG_ERROR);
        return '';
      }  
    }
    
    // if the option is to remove or refresh then we want to remove
    // the feature from the database.
    if ($remove or $refresh) {
      $sql = "DELETE FROM {feature}
              WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
      $match = array(
         'organism_id' => $organism->organism_id,
         'uniquename'  => $attr_uniquename,
         'type_id'     => $cvterm->cvterm_id
      );
      $result = tripal_core_chado_delete('feature', $match);
      if (!$result) {
        watchdog('T_gff3_loader', "cannot delete feature %attr_uniquename", 
          array('%attr_uniquename' => $attr_uniquename), WATCHDOG_ERROR);
      }
      $feature = 0;
      unset($result);
    }

    // add or update the feature and all properties
    if ($update or $refresh or $add_only) {

      // add/update the feature
      $feature = tripal_feature_load_gff3_feature($organism, $analysis_id, $cvterm,
        $attr_uniquename, $attr_name, $residues, $attr_is_analysis,
        $attr_is_obsolete, $add_only, $score);  
   
      if ($feature) {
        
        // add a record for this feature to the tripal_gff_temp table for
        // later lookup
        $values = array(
          'feature_id' => $feature->feature_id,
          'organism_id' => $feature->organism_id,
          'type_name' => $type,
          'uniquename' => $feature->uniquename
        );
        // make sure this record doesn't already exist in oru temp table
        $options = array('statement_name' => 'sel_tripalgfftemp_all');
        $results = tripal_core_chado_select('tripal_gff_temp', array('*'), $values, $options);

        if (count($results) == 0) {
          $options = array('statement_name' => 'ins_tripalgfftemp');
          $result = tripal_core_chado_insert('tripal_gff_temp', $values, $options);
          if (!$result) {
            watchdog('T_gff3_loader', "Cound not save record in temporary table, Cannot continue.", array(), WATCHDOG_ERROR);
            return;
          }
        }

        // add/update the featureloc if the landmark and the ID are not the same
        // if they are the same then this entry in the GFF is probably a landmark identifier
        if (strcmp($landmark, $attr_uniquename) !=0 ) {
          tripal_feature_load_gff3_featureloc($feature, $organism,
            $landmark, $fmin, $fmax, $strand, $phase, $attr_fmin_partial,
            $attr_fmax_partial, $attr_residue_info, $attr_locgroup);
        }

        // add any aliases for this feature
        if (array_key_exists('Alias', $tags)) {
          tripal_feature_load_gff3_alias($feature, $tags['Alias']);
        }      
        // add any dbxrefs for this feature
        if (array_key_exists('Dbxref', $tags)) {
          tripal_feature_load_gff3_dbxref($feature, $tags['Dbxref']);
        }
        // add any ontology terms for this feature
        if (array_key_exists('Ontology_term', $tags)) {
          tripal_feature_load_gff3_ontology($feature, $tags['Ontology_term']);
        }       
        // add parent relationships
        if (array_key_exists('Parent', $tags)) {
          tripal_feature_load_gff3_parents($feature, $cvterm, $tags['Parent'], $organism_id, $fmin);
        }     
          
        // add target relationships
        if (array_key_exists('Target', $tags)) {
          // format is: "target_id start end [strand]", where strand is optional and may be "+" or "-"
          $matched = preg_match('/^(.*?)\s+(\d+)\s+(\d+)(\s+[\+|\-])*$/', trim($tags['Target'][0]), $matches);
          
          // if we have matches and the Target is in the correct format then load the alignment 
          if ($matched) {
            $target_feature = $matches[1]; 
            $start = $matches[2]; 
            $end = $matches[3]; 
            // if we have an optional strand, convert it to a numeric value. 
            if ($matches[4]) {
              if (preg_match('/^+$/', trim($matches[4]))) {
                $target_strand = 1;
              }
              elseif (preg_match('/^-$/', trim($matches[4]))) {
                $target_strand = -1;
              }
              else {
                $target_strand = 0;
              }
            }
            else {
               $target_strand = 0;
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
              $attr_fmax_partial, $attr_residue_info, $attr_locgroup);
          }
          // the target attribute is not correctly formatted
          else {
            watchdog('T_gff3_loader', "Could not add 'Target' alignment as it is improperly formatted:  '%target'",
              array('%target' => $tags['Target'][0]), WATCHDOG_ERROR);            
          }
        }
        
        
        // add gap information.  This goes in simply as a property
        if (array_key_exists('Gap', $tags)) {
          foreach ($tags['Gap'] as $value) {
            tripal_feature_load_gff3_property($feature, 'Gap', $value);
          }
        }
        // add notes. This goes in simply as a property
        if (array_key_exists('Note', $tags)) {
          foreach ($tags['Note'] as $value) {
              tripal_feature_load_gff3_property($feature, 'Note', $value);
          }
        }
        // add the Derives_from relationship (e.g. polycistronic genes).
        if (array_key_exists('Derives_from', $tags)) {
          tripal_feature_load_gff3_derives_from($feature, $tags['Derives_from'][0], $organism);
        }

        // add in the GFF3_source dbxref so that GBrowse can find the feature using the source column
        $source_ref = array('GFF_source:' . $source);
        tripal_feature_load_gff3_dbxref($feature, $source_ref);

        // add any additional attributes
        if ($attr_others) {
          foreach ($attr_others as $tag_name => $values) {
            foreach ($values as $value) {
              tripal_feature_load_gff3_property($feature, $tag_name, $value);
            }
          }
        }
      }
    }
  }

  print "\nSetting ranks of children...\n";
  
  // get features in a relationship that are also children of an alignment
  $sql = "SELECT DISTINCT F.feature_id, F.organism_id, F.type_id, 
            F.uniquename, FL.strand 
          FROM tripal_gff_temp TGT 
            INNER JOIN feature F                ON TGT.feature_id = F.feature_id
            INNER JOIN feature_relationship FR  ON FR.object_id = TGT.feature_id
            INNER JOIN cvterm CVT               ON CVT.cvterm_id = FR.type_id  
            INNER JOIN featureloc FL            ON FL.feature_id = F.feature_id    
          WHERE CVT.name = 'part_of'";
  $parents = chado_query($sql);
  
  // build and prepare the SQL for selecting the children relationship
  $sql = "SELECT DISTINCT FR.feature_relationship_id, FL.fmin, FR.rank
          FROM feature_relationship FR              
            INNER JOIN featureloc FL on FL.feature_id = FR.subject_id";
  if (!$connection) {
    $sql .= "WHERE FR.object_id = %d ".
            "ORDER BY FL.fmin ASC ";
  }
  else {
    $sql = "PREPARE sel_gffchildren (int) AS " . $sql . " WHERE FR.object_id = \$1 ORDER BY FL.fmin ASC";            
  }
  if (!tripal_core_is_sql_prepared('sel_gffchildren')) {
    $success = chado_query($sql);
    if (!$success) {
      watchdog("T_gff3_loader", "Cannot prepare statement 'sel_gffchildren' and cannot set children ranks.", 
         array(), WATCHDOG_WARNING);
      return 0;  
    }
  }
  
  // now set the rank of any parent/child relationships.  The order is based
  // on the fmin.  The start rank is 1.  This allows features with other
  // relationships to be '0' (the default), and doesn't interfer with the
  // ordering defined here.        
  while ($parent = db_fetch_object($parents)) {
    
    // get the children
    if ($connection) {      
      $result = chado_query('EXECUTE sel_gffchildren (%d)', $parent->feature_id);
    }
    else {
      $result = chado_query($sql, $parent->feature_id);
    }
    
    // build an array of the children
    $children = array();
    while ($child = db_fetch_object($result)) {
       $children[] = $child;  
    }
    
    // the children list comes sorted in ascending fmin
    // but if the parent is on the reverse strand we need to 
    // reverse the order of the children.
    if ($parent->strand == -1) {
      arsort($children);
    }    

    // first set the ranks to a negative number so that we don't
    // get a duplicate error message when we try to change any of them    
    $rank = -1;
    foreach ($children as $child) {
      $match = array('feature_relationship_id' => $child->feature_relationship_id);
      $options = array('statement_name' => 'upd_featurerelationship_rank');      
      $values = array('rank' => $rank);      
      tripal_core_chado_update('feature_relationship', $match, $values, $options);
      $rank--;
    }
    // now set the rank correctly. The rank should start at 0.
    $rank = 0;
    foreach ($children as $child) {
      $match = array('feature_relationship_id' => $child->feature_relationship_id);
      $options = array('statement_name' => 'upd_featurerelationship_rank');      
      $values = array('rank' => $rank); 
      //print "Was: " . $child->rank . " now $rank ($parent->strand)\n"     ;
      tripal_core_chado_update('feature_relationship', $match, $values, $options);
      $rank++;
    }
  }

  // commit the transaction
  if ($use_transaction) {
    tripal_db_commit_transaction();
  }
  print "Done\n";
  
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_derives_from($feature, $subject, $organism) {

  // get the subject type
  $values = array(
    'organism_id' => $organism->organism_id,
    'uniquename' => $subject,
  );
  $options = array('statement_name' => 'sel_tripalgfftemp_orun');
  $result = tripal_core_chado_select('tripal_gff_temp', array('type_name'), $values, $options);   
  if (count($result) == 0) {
    watchdog("T_gff3_loader", "Cannot find subject type for feature in 'derives_from' relationship: %subject", array('%subject' => $subject), WATCHDOG_WARNING);
     return ''; 
  }
  $subject_type = $result[0]->type_name;
  
  // get the subject feature
  $match = array(
    'organism_id' => $organism->organism_id,
    'uniquename' => $subject,
    'type_id' => array(
      'name' => $subject_type,
      'cv_id' => array(
        'name' => 'sequence'
      ),
    ),      
  );
  $options = array('statement_name' => 'sel_feature_orunty');
  $sfeature = tripal_core_chado_select('feature', array('feature_id'), $match, $options);
  if (count($sfeature)==0) {
    watchdog('T_gff3_loader', "Could not add 'Derives_from' relationship ".
      "for %uniquename and %subject.  Subject feature, '%subject', ".
      "cannot be found", array('%uniquename' => $feature->uniquename, '%subject' => $subject), WATCHDOG_ERROR);
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
    return;
  }

  // finally insert the relationship if it doesn't exist
  $options = array('statement_name' => 'ins_featurerelationship_objectid_subjectid_typeid_rank');
  $ret = tripal_core_chado_insert('feature_relationship', $values, $options);
  if (!$ret) {
    watchdog("T_gff3_loader", "Could not add 'Derives_from' relationship for $feature->uniquename and $subject", 
      array(), WATCHDOG_WARNING);
  }
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_parents($feature, $cvterm, $parents, $organism_id, $fmin) {

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
       watchdog("T_gff3_loader", "Cannot prepare statement 'sel_cvterm_cvname_cvtname_synonym' for ontology term", 
         array(), WATCHDOG_WARNING);
       return '';
    }
  }

  // iterate through the parents in the list
  foreach ($parents as $parent) {
    // get the parent cvterm
    $values = array(
      'organism_id' => $organism_id,
      'uniquename' => $parent,
    );
    $options = array('statement_name' => 'sel_tripalgfftemp_orun');
    $result = tripal_core_chado_select('tripal_gff_temp', array('type_name'), $values, $options);    
    if (count($result) == 0) {
      watchdog("T_gff3_loader", "Cannot find parent type: %parent", array('%parent' => $parent), WATCHDOG_WARNING);
       return '';  
    }
    $parent_type = $result[0]->type_name;

    // try to find the parent
    $parentcvterm = db_fetch_object(chado_query("EXECUTE sel_cvterm_cvname_cvtname_synonym ('%s', '%s', '%s')", 'sequence', $parent_type, $parent_type));
    $relcvterm = db_fetch_object(chado_query("EXECUTE sel_cvterm_cvname_cvtname_synonym ('%s', '%s', '%s')", 'relationship', $rel_type, $rel_type));
    $values = array(
        'organism_id' => $organism_id,
        'uniquename' => $parent,
        'type_id' => $parentcvterm->cvterm_id,
    );
    $options = array('statement_name' => 'sel_feature_orunty');
    $result = tripal_core_chado_select('feature', array('feature_id'), $values, $options);
    $parent_feature = $result[0];

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
          watchdog("T_gff3_loader", "Failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type)", 
            array(), WATCHDOG_WARNING);
        }
      }
    }
    else {
      watchdog("T_gff3_loader", "Cannot establish relationship '$uname' ($type) $rel_type '$parent' ($parent_type): Cannot find the parent", 
        array(), WATCHDOG_WARNING);      
    }
  }
}

/**
 *
 *
 * @ingroup gff3_loader
 */

function tripal_feature_load_gff3_dbxref($feature, $dbxrefs) {

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
    $values = array('name' => "DB:$dbname");
    $options = array('statement_name' => 'sel_db_name');
    $db = tripal_core_chado_select('db', array('db_id'), $values, $options);
    if (count($db) == 0) {
      $values = array('name' => "$dbname");
      $db = tripal_core_chado_select('db', array('db_id'), $values, $options);
    }
    if (count($db) == 0) {
      $values = array(
        'name' => $dbname,
        'description' => 'Added automatically by the GFF loader'
      );
      $options = array('statement_name' => 'ins_db_name');
      $success = tripal_core_chado_insert('db', $values, $options);
      if ($success) {
        $values = array('name' => "$dbname");
        $options = array('statement_name' => 'sel_db_name');        
        $db = tripal_core_chado_select('db', array('db_id'), $values, $options);
      }
      else {
        watchdog("T_gff3_loader", "Cannot find or add the database $dbname", array(), WATCHDOG_WARNING);
        return 0;
      }
    }
    $db = $db[0];

    // now check to see if the accession exists
    $values = array(
      'accession' => $accession, 
      'db_id' => $db->db_id
    );
    $options = array('statement_name' => 'sel_dbxref_accession_dbid');
    $dbxref = tripal_core_chado_select('dbxref', array('dbxref_id'), $values, $options);

    // if the accession doesn't exist then we want to add it
    if (sizeof($dbxref) == 0) {
      $values = array(
        'db_id' => $db->db_id,
        'accession' => $accession, 
        'version' => ''
      );
      $options = array('statement_name' => 'ins_dbxref_dbid_accession_version');
      $ret = tripal_core_chado_insert('dbxref', $values, $options);
      $values = array(
        'accession' => $accession, 
        'db_id' => $db->db_id
      );
      $options = array('statement_name' => 'sel_dbxref_accession_dbid');
      $dbxref = tripal_core_chado_select('dbxref', array('dbxref_id'), $values, $options);
    }
    $dbxref = $dbxref[0];

    // check to see if this feature dbxref already exists
    $values = array(
      'dbxref_id' => $dbxref->dbxref_id, 
      'feature_id' => $feature->feature_id
    );
    $options = array('statement_name' => 'sel_featuredbxref_dbxrefid_featureid');
    $fdbx = tripal_core_chado_select('feature_dbxref', array('feature_dbxref_id'), $values, $options);

    // now associate this feature with the database reference if it doesn't
    // already exist
    if (sizeof($fdbx) == 0) {
      $values = array(
        'dbxref_id' => $dbxref->dbxref_id,
        'feature_id' => $feature->feature_id
      );
      $options = array('statement_name' => 'ins_featuredbxref_dbxrefid_featureid');
      $success = tripal_core_chado_insert('feature_dbxref', $values, $options);
      if (!$success) {
        watchdog("T_gff3_loader", "Failed to insert Dbxref: $dbname:$accession", array(), WATCHDOG_WARNING);
        return 0;
      }
    }
  }
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_ontology($feature, $dbxrefs) {

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
      // now look for the name without the 'DB:' prefix.
      $db = tripal_core_chado_select('db', array('db_id'), array('name' => "$dbname"), $options);
      if (sizeof($db) == 0) {
        watchdog("T_gff3_loader", "Database, $dbname, is not present. Cannot associate term: $dbname:$accession", array(), WATCHDOG_WARNING);
        return 0;
      }
    }
    $db = $db[0];

    // now check to see if the accession exists
    $options = array('statement_name' => 'sel_dbxref_accession_dbid');
    $dbxref = tripal_core_chado_select('dbxref', array('dbxref_id'), 
      array('accession' => $accession, 'db_id' => $db->db_id), $options);
    if (sizeof($dbxref) == 0) {
      watchdog("T_gff3_loader", "Accession, $accession is missing for reference: $dbname:$accession", array(), WATCHDOG_WARNING);
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
      if (sizeof($cvterm) == 0) {
        watchdog("T_gff3_loader", "CV Term is missing for reference: $dbname:$accession", array(), WATCHDOG_WARNING);
        return 0;
      }
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
      $success = tripal_core_chado_insert('feature_cvterm', $values, $options);

      if (!$success) {
        watchdog("T_gff3_loader", "Failed to insert ontology term: $dbname:$accession", array(), WATCHDOG_WARNING);
        return 0;
      }
    }
  }
  return 1;
}
/**
 *
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_alias($feature, $aliases) {

  // make sure we have a 'synonym_type' vocabulary
  $select = array('name' => 'synonym_type');
  $options = array('statement_name' => 'sel_cv_name');  
  $results = tripal_core_chado_select('cv', array('*'), $select, $options);
  
  if (count($results) == 0) {
    // insert the 'synonym_type' vocabulary
    $values = array(
      'name' => 'synonym_type',
      'definition' => 'vocabulary for synonym types',
    );
    $options = array('statement_name' => 'ins_cv_name_definition');
    $success = tripal_core_chado_insert('cv', $values, $options);
    if (!$success) {
      watchdog("T_gff3_loader", "Failed to add the synonyms type vocabulary", array(), WATCHDOG_WARNING);
      return 0;
    }
    // now that we've added the cv we need to get the record
    $options = array('statement_name' => 'sel_cv_name');
    $results = tripal_core_chado_select('cv', array('*'), $select, $options);
    if (count($results) > 0) {
      $syncv = $results[0];
    }
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
  if (count($result) == 0) {
    $term = array(
      'name' => 'exact',
      'id' => "internal:exact",
      'definition' => '',
      'is_obsolete' => 0,
    );
    // TODO: fix the function so it uses prepared statements    
    $syntype = tripal_cv_add_cvterm($term, $syncv->name, 0, 1);
    if (!$syntype) {
      watchdog("T_gff3_loader", "Cannot add synonym type: internal:$type", array(), WATCHDOG_WARNING);
      return 0;
    }
  } 
  else { 
    $syntype = $result[0];
  }
 
  // iterate through all of the aliases and add each one
  foreach ($aliases as $alias) {

    // check to see if the alias already exists in the synonym table
    // if not, then add it
    $select = array(
       'name' => $alias,
       'type_id' => $syntype->cvterm_id,
    );
    $options = array('statement_name' => 'sel_synonym_name_typeid');
    $result = tripal_core_chado_select('synonym', array('*'), $select, $options);    
    if (count($result) == 0) {
      $values = array(
         'name' => $alias,
         'type_id' => $syntype->cvterm_id,
         'synonym_sgml' => '',
      );
      $options = array('statement_name' => 'ins_synonym_name_typeid_synonymsgml');
      $success = tripal_core_chado_insert('synonym', $values, $options);
      if (!$success) {
        watchdog("T_gff3_loader", "Cannot add alias $alias to synonym table", array(), WATCHDOG_WARNING);
        return 0;
      }
      $options = array('statement_name' => 'sel_synonym_name_typeid');
      $result = tripal_core_chado_select('synonym', array('*'), $select, $options);
      $synonym = $result[0];
    }
    else {
      $synonym = $result[0];  
    }

    // check to see if we have a NULL publication in the pub table.  If not,
    // then add one.
    // @coder-ignore: non-drupal schema thus table prefixing does not apply
    $select = array('uniquename' => 'null');
    $options = array('statement_name' => 'sel_pub_uniquename');
    $result = tripal_core_chado_select('pub', array('*'), $select, $options);    
    if (count($result) == 0) {
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
          watchdog("T_gff3_loader", "Cannot prepare statement 'ins_pub_uniquename_typeid", array(), WATCHDOG_WARNING);
          return 0;
        } 
      }    
      // insert the null pub 
      $result = db_fetch_object(chado_query("EXECUTE ins_pub_uniquename_typeid ('%s', '%s')", 'null', 'null'));
      if (!$result) {
        watchdog("T_gff3_loader", "Cannot add null publication needed for setup of alias", array(), WATCHDOG_WARNING);
        return 0;
      }
      $options = array('statement_name' => 'sel_pub_uniquename');
      $result = tripal_core_chado_select('pub', array('*'), $select, $options);
      $pub = $result[0];
    }
    else {
      $pub = $result[0];  
    }

    // check to see if the synonym exists in the feature_synonym table
    // if not, then add it.
    $values = array(
       'synonym_id' => $synonym->synonym_id,
       'feature_id' => $feature->feature_id,
       'pub_id' => $pub->pub_id,
    );
    $columns = array('feature_synonym_id');
    $options = array('statement_name' => 'sel_featuresynonym_syfepu');
    $result = tripal_core_chado_select('feature_synonym', $columns, $values, $options);
    if (count($result) == 0) {      
      $values = array(
         'synonym_id' => $synonym->synonym_id,
         'feature_id' => $feature->feature_id,
         'pub_id' => $pub->pub_id,
      );  
      $ins_options = array('statement_name' => 'ins_featuresynonym_syfepu');
      $success = tripal_core_chado_insert('feature_synonym', $values, $ins_options);   
     
      if (!$success) {
        watchdog("T_gff3_loader", "Cannot add alias $alias to feature synonym table", array(), WATCHDOG_WARNING);
        return 0;
      }
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
  $residues, $is_analysis = 'f', $is_obsolete = 'f', $add_only, $score) {

  // check to see if the feature already exists
  $feature = NULL;
  $fselect = array(
     'organism_id' => $organism->organism_id,
     'uniquename' => $uniquename,
     'type_id' => $cvterm->cvterm_id
  );
  $options = array('statement_name' => 'sel_feature_orunty');
  $columns = array('feature_id', 'name', 'uniquename', 'seqlen', 'organism_id', 'type_id');
  $result = tripal_core_chado_select('feature', $columns, $fselect, $options);
  if (count($result) > 0) {
    $feature = $result[0];
  }

  if (strcmp($is_obsolete, 'f')==0 or $is_obsolete == 0) {
    $is_obsolete = 'FALSE';
  }
  if (strcmp($is_obsolete, 't')==0 or $is_obsolete == 1) {
    $is_obsolete = 'TRUE';
  }
  if (strcmp($is_analysis, 'f')==0 or $is_analysis == 0) {
    $is_analysis = 'FALSE'; 
  }
  if (strcmp($is_analysis, 't')==0 or $is_analysis == 1) {
    $is_analysis = 'TRUE'; 
  }

  // insert the feature if it does not exist otherwise perform an update
  if (!$feature) {
    $values = array(
       'organism_id' => $organism->organism_id,
       'name' => $name,
       'uniquename' => $uniquename,
//       'residues' => $residues,
//       'seqlen' => drupal_strlen($residues),
       'md5checksum' => md5($residues),
       'type_id' => $cvterm->cvterm_id,
       'is_analysis' => $is_analysis,
       'is_obsolete' => $is_obsolete,
    );
    $options = array('statement_name' => 'ins_feature_all');
    $result = tripal_core_chado_insert('feature', $values, $options);
    if (!$result) {
      watchdog("T_gff3_loader", "Failed to insert feature '$uniquename' ($cvterm->name)", array(), WATCHDOG_WARNING);
      return 0;
    }
  }
  elseif (!$add_only) {
    $values = array(
      'name' => $name,
//      'residues' => $residues,
//      'seqlen' => drupal_strlen($residues),
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
      watchdog("T_gff3_loader", "Failed to update feature '$uniquename' ($cvterm->name)", array(), WATCHDOG_WARNING);
      return 0;
    }
  }
  else {
    // the feature exists and we don't want to update it so return
    // a value of 0.  This will stop all downstream property additions
    return 0;
  }

  // get the newly added feature
  $options = array('statement_name' => 'sel_feature_orunty');
  $columns = array('feature_id', 'name', 'uniquename', 'seqlen', 'organism_id', 'type_id');
  $result = tripal_core_chado_select('feature', $columns, $fselect, $options);  
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
      watchdog("T_gff3_loader", "Could not add analysisfeature record: $analysis_id, $feature->feature_id", array(), WATCHDOG_WARNING);
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
        watchdog("T_gff3_loader", "Could not update analysisfeature record: $analysis_id, $feature->feature_id", array(), WATCHDOG_WARNING);
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
  $fmax, $strand, $phase, $is_fmin_partial, $is_fmax_partial, $residue_info, $locgroup) {

  $select = array(
    'organism_id' => $organism->organism_id,
    'uniquename' => $landmark,
  );
  $options = array('statement_name' => 'sel_feature_orun');  
  $r = tripal_core_chado_select('feature', array('feature_id'), $select, $options);
  
  if (count($r)==0) {
    // so we couldn't find the landmark using the uniquename. Let's try the 'name'.
    // if we return only a singe result then we can proceed. Otherwise give an
    // error message
    $select = array(
      'organism_id' => $organism->organism_id,
      'name' => $landmark,
    );
    $options = array('statement_name' => 'sel_feature_organism_id_name');
    $r = tripal_core_chado_select('feature', array('feature_id'), $select, $options);
    if (count($r) == 0) {
       watchdog("T_gff3_loader", "Cannot find landmark feature: '$landmark'.", array(), WATCHDOG_WARNING);
       return 0;
    } 
    elseif (count($r) > 1) {
       watchdog("T_gff3_loader", "multiple landmarks exist with the name: '$landmark'.  Cannot resolve which one to use. Cannot add the feature location record", 
         array(), WATCHDOG_WARNING);
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
    $columns = array('feature_id', 'name');
    $locsfeature = tripal_core_chado_select('feature', $columns, $select, $options);   
    
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
      }
    }
    $rank = $featureloc->rank + 1;
  }
  if (!$exists) {

    // this feature location is new so add it
    if (strcmp($is_fmin_partial, 'f')==0 or !$is_fmin_partial) {
      $is_fmin_partial = 'FALSE';
    }
    elseif (strcmp($is_fmin_partial, 't')==0 or $is_fmin_partial = 1) {
      $is_fmin_partial = 'TRUE';
    }
    if (strcmp($is_fmax_partial, 'f')==0 or !$is_fmax_partial) {
      $is_fmax_partial = 'FALSE';
    }
    elseif (strcmp($is_fmax_partial, 't')==0 or $is_fmax_partial = 1) {
      $is_fmax_partial = 'TRUE';
    }
    $values = array(
       'feature_id'      => $feature->feature_id,
       'srcfeature_id'   => $srcfeature->feature_id,
       'fmin'            => $fmin,
       'is_fmin_partial' => $is_fmin_partial,
       'fmax'            => $fmax,
       'is_fmax_partial' => $is_fmax_partial,
       'strand'          => $strand,
       'residue_info'    => $residue_info,
       'locgroup'        => $locgroup,
       'rank'            => $rank 
    );
    $options = array('statement_name' => 'ins_featureloc_all');
    if ($phase) {
      $values['phase'] = $phase;
      $options = array('statement_name' => 'ins_featureloc_allphase');
    }
    
    $success = tripal_core_chado_insert('featureloc', $values, $options);
    if (!$success) {
      watchdog("T_gff3_loader", "Failed to insert featureloc", array(), WATCHDOG_WARNING);
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
function tripal_feature_load_gff3_property($feature, $property, $value) {

  // first make sure the cvterm exists.  if not, then add it
  $select = array(
     'name' => $property,
     'cv_id' => array(
        'name' => 'feature_property',
     ),
  );
  $options = array('statement_name' => 'sel_cvterm_name_cvid');
  $result = tripal_core_chado_select('cvterm', array('*'), $select, $options);
  if (count($cvterm) == 0) {
    $term = array(
      'id' => "null:$property",
      'name' => $property,
      'namespace' => 'feature_property',
      'is_obsolete' => 0,
    );    
    $cvterm = (object) tripal_cv_add_cvterm($term, 'feature_property', 0, 0);
    if (!$cvterm) {
      watchdog("T_gff3_loader", "Cannot add cvterm, $property", array(), WATCHDOG_WARNING);
      return 0;  
    }
  } 
  else {
    $cvterm = $result[0];
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
  foreach ($results as $prop) {
    if (strcmp($prop->value, $value)==0) {
      $add = NULL; // don't add it, it already exists
    }
    $rank = $prop->rank + 1;
  }

  // add the property if we pass the check above
  if ($add) {
    $values = array(
       'feature_id' => $feature->feature_id,
       'type_id' => $cvterm->cvterm_id,
       'value' => $value,
       'rank' => $rank,
    );
    $options = array('statement_name' => 'ins_featureprop_all');
    $result = tripal_core_chado_insert('featureprop', $values, $options);
    if (!$result) {
      watchdog("T_gff3_loader", "cannot add featureprop, $property", array(), WATCHDOG_WARNING);
    }
  }
}

