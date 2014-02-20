<?php
/**
 * @file
 * Provides gff3 loading functionality. Creates features based on their specification
 * in a GFF3 file.
 */

/**
 * @defgroup gff3_loader GFF3 Feature Loader
 * @ingroup tripal_feature
 * @{
 * Provides gff3 loading functionality. Creates features based on their specification in a GFF3 file.
 * @}
 */

/**
 * The form to submit a GFF3 loading job
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

  // get the list of analyses
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $org_rset = chado_query($sql);
  $analyses = array();
  $analyses[''] = '';
  while ($analysis = $org_rset->fetchObject()) {
    $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
  }
  $form['analysis_id'] = array(
   '#title'       => t('Analysis'),
   '#type'        => t('select'),
   '#description' => t("Choose the analysis to which these features are associated.
       Why specify an analysis for a data load?  All data comes
       from some place, even if downloaded from Genbank. By specifying
       analysis details for all data imports it allows an end user to reproduce the
       data set, but at least indicates the source of the data."),
   '#required'    => TRUE,
   '#options'     => $analyses,
  );

  $form['line_number']= array(
    '#type'          => 'textfield',
    '#title'         => t('Start Line Number'),
    '#description'   => t('Enter the line number in the GFF file where you would like to begin processing.  The
      first line is line number 1.  This option is useful for examining loading problems with large GFF files.'),
    '#size' => 10,
  );

  $form['landmark_type'] = array(
    '#title'       => t('Landmark Type'),
    '#type'        => t('textfield'),
    '#description' => t("Optional. Use this field to specify a Sequence Ontology type
       for the landmark sequences in the GFF fie (e.g. 'chromosome'). If the GFF file
       contains a '##sequence-region' line that describes the landmark sequences to
       which all others are aligned and a type is provided here then the features
       will be created if they do not already exist.  If they do exist then this
       field is not used."),
  );

  $form['alt_id_attr'] = array(
    '#title'       => t('ID Attribute'),
    '#type'        => t('textfield'),
    '#description' => t("Optional. Sometimes lines in the GFF file are missing the
      required ID attribute that specifies the unique name of the feature.  If so,
      you may specify an the name of an existing  attribute to use for the name."),
  );

  $form['import_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import Options'),
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
    '#default_value' => 1,
  );
  $form['import_options']['add_only']= array(
    '#type' => 'checkbox',
    '#title' => t('Import only new features'),
    '#required' => FALSE,
    '#description' => t('The job will skip features in the GFF file that already
                         exist in the database and import only new features.'),
  );
  $form['import_options']['update']= array(
    '#type' => 'checkbox',
    '#title' => t('Import all and update'),
    '#required' => FALSE,
    '#default_value' => 'checked',
    '#description' => t('Existing features will be updated and new features will be added.  Attributes
                         for a feature that are not present in the GFF but which are present in the
                         database will not be altered.'),
    '#default_value' => 1,
  );
  $form['import_options']['refresh']= array(
    '#type' => 'checkbox',
    '#title' => t('Import all and replace'),
    '#required' => FALSE,
    '#description' => t('Existing features will be updated and feature properties not
                         present in the GFF file will be removed.'),
  );
  $form['import_options']['remove']= array(
    '#type' => 'checkbox',
    '#title' => t('Delete features'),
    '#required' => FALSE,
    '#description' => t('Features present in the GFF file that exist in the database
                         will be removed rather than imported'),
  );
  $form['import_options']['create_organism']= array(
    '#type' => 'checkbox',
    '#title' => t('Create organism'),
    '#required' => FALSE,
    '#description' => t('The Tripal GFF loader supports the "organism" attribute. This allows features of a
       different organism to be aligned to the landmark sequence of another species.  The format of the
       attribute is "organism=[genus]:[species]", where [genus] is the organism\'s genus and [species] is the
       species name. Check this box to automatically add the organism to the database if it does not already exists.
       Otherwise lines with an oraganism attribute where the organism is not present in the database will be skipped.'),
  );

  $form['targets'] = array(
    '#type' => 'fieldset',
    '#title' => t('Targets'),
    '#collapsed' => TRUE
  );
  $form['targets']['adesc'] = array(
    '#markup' => t("When alignments are represented in the GFF file (e.g. such as
       alignments of cDNA sequences to a whole genome, or blast matches), they are
       represented using two feature types: 'match' (or cDNA_match, EST_match, etc.)
       and 'match_part'.  These features may also have a 'Target' attribute to
       specify the sequence that is being aligned.
       However, the organism to which the aligned sequence belongs may not be present in the
       GFF file.  Here you can specify the organism and feature type of the target sequences.
       The options here will apply to all targets unless the organism and type are explicity
       set in the GFF file using the 'target_organism' and 'target_type' attributes."),
  );
  $form['targets']['target_organism_id'] = array(
    '#title'       => t('Target Organism'),
    '#type'        => t('select'),
    '#description' => t("Optional. Choose the organism to which target sequences belong.
      Select this only if target sequences belong to a different organism than the
      one specified above. And only choose an organism here if all of the target sequences
      belong to the same species.  If the targets in the GFF file belong to multiple
      different species then the organism must be specified using the 'target_organism=genus:species'
      attribute in the GFF file."),
    '#options'     => $organisms,
  );
  $form['targets']['target_type'] = array(
    '#title'       => t('Target Type'),
    '#type'        => t('textfield'),
    '#description' => t("Optional. If the unique name for a target sequence is not unique (e.g. a protein
       and an mRNA have the same name) then you must specify the type for all targets in the GFF file. If
       the targets are of different types then the type must be specified using the 'target_type=type' attribute
       in the GFF file. This must be a valid Sequence Ontology (SO) term."),
  );
  $form['targets']['create_target']= array(
    '#type' => 'checkbox',
    '#title' => t('Create Target'),
    '#required' => FALSE,
    '#description' => t("If the target feature cannot be found, create one using the organism and type specified above, or
       using the 'target_organism' and 'target_type' fields specified in the GFF file.  Values specified in the
       GFF file take precedence over those specified above."),
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Import GFF3 file'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Validate the GFF3 loading job form
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form_validate($form, &$form_state) {

  $gff_file = trim($form_state['values']['gff_file']);
  $organism_id = $form_state['values']['organism_id'];
  $target_organism_id = $form_state['values']['target_organism_id'];
  $target_type = trim($form_state['values']['target_type']);
  $create_target = $form_state['values']['create_target'];
  $create_organism = $form_state['values']['create_organism'];
  $add_only = $form_state['values']['add_only'];
  $update   = $form_state['values']['update'];
  $refresh  = $form_state['values']['refresh'];
  $remove   = $form_state['values']['remove'];
  $use_transaction   = $form_state['values']['use_transaction'];
  $line_number   = trim($form_state['values']['line_number']);
  $landmark_type   = trim($form_state['values']['landmark_type']);
  $alt_id_attr   = trim($form_state['values']['alt_id_attr']);



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

  if ($line_number and !is_numeric($line_number) or $line_number < 0) {
    form_set_error('line_number', t("Please provide an integer line number greater than zero."));
  }
}

/**
 * Submit the GFF3 loading job
 *
 * @ingroup gff3_loader
 */
function tripal_feature_gff3_load_form_submit($form, &$form_state) {
  global $user;

  $gff_file = trim($form_state['values']['gff_file']);
  $organism_id = $form_state['values']['organism_id'];
  $add_only = $form_state['values']['add_only'];
  $update   = $form_state['values']['update'];
  $refresh  = $form_state['values']['refresh'];
  $remove   = $form_state['values']['remove'];
  $analysis_id = $form_state['values']['analysis_id'];
  $use_transaction   = $form_state['values']['use_transaction'];
  $target_organism_id = $form_state['values']['target_organism_id'];
  $target_type = trim($form_state['values']['target_type']);
  $create_target = $form_state['values']['create_target'];
  $line_number   = trim($form_state['values']['line_number']);
  $landmark_type   = trim($form_state['values']['landmark_type']);
  $alt_id_attr   = trim($form_state['values']['alt_id_attr']);
  $create_organism = $form_state['values']['create_organism'];


  $args = array($gff_file, $organism_id, $analysis_id, $add_only,
    $update, $refresh, $remove, $use_transaction, $target_organism_id,
    $target_type, $create_target, $line_number, $landmark_type, $alt_id_attr,
    $create_organism);

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
 * Actually load a GFF3 file. This is the function called by tripal jobs
 *
 * @param $gff_file
 * @param $organism_id
 * @param $analysis_id
 * @param $add_only
 * @param $update
 * @param $refresh
 * @param $remove
 * @param $use_transaction
 *   Whether or not to use a transaction. Default is yes (TRUE)
 * @param $target_organism_id
 * @param $target_type
 * @param $create_target
 * @param $start_line
 * @param $landmark_type
 * @param $alt_id_attr
 * @param $create_organism
 * @param $job
 *  The tripal job
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3($gff_file, $organism_id, $analysis_id,
  $add_only = 0, $update = 0, $refresh = 0, $remove = 0, $use_transaction = 1,
  $target_organism_id = NULL, $target_type = NULL,  $create_target = 0,
  $start_line = 1, $landmark_type = '', $alt_id_attr = '', $create_organism = FALSE,
  $job = NULL) {

  $ret = array();

  // empty the temp table
  $sql = "DELETE FROM {tripal_gff_temp}";
  chado_query($sql);

  // begin the transaction
  $transaction = null;
  if ($use_transaction) {
    $transaction = db_transaction();
    print "\nNOTE: Loading of this GFF file is performed using a database transaction. \n" .
         "If the load fails or is terminated prematurely then the entire set of \n" .
         "insertions/updates is rolled back and will not be found in the database\n\n";
  }
  try {

    // check to see if the file is located local to Drupal
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file;
    if (!file_exists($dfile)) {
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $gff_file;
    }
    if (!file_exists($dfile)) {
      tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "Cannot find the file: %dfile",
        array('%dfile' => $dfile));
      return 0;
    }

    print "Opening $gff_file\n";

    //$lines = file($dfile,FILE_SKIP_EMPTY_LINES);
    $fh = fopen($dfile, 'r');
    if (!$fh) {
      tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "cannot open file: %dfile",
        array('%dfile' => $dfile));
      return 0;
    }
    $filesize = filesize($dfile);

    // get the controlled vocaubulary that we'll be using.  The
    // default is the 'sequence' ontology
    $sql = "SELECT * FROM {cv} WHERE name = :cvname";
    $cv = chado_query($sql, array(':cvname' => 'sequence'))->fetchObject();
    if (!$cv) {
      tripal_report_error('T_gff3_loader', TRIPAL_ERROR, 
        "Cannot find the 'sequence' ontology", array());
      return '';
    }

    // get the organism for which this GFF3 file belongs
    $sql = "SELECT * FROM {organism} WHERE organism_id = :organism_id";
    $organism = chado_query($sql, array(':organism_id' => $organism_id))->fetchObject();

    $interval = intval($filesize * 0.0001);
    if ($interval == 0) {
      $interval = 1;
    }
    $in_fasta = 0;
    $line_num = 0;
    $num_read = 0;
    $intv_read = 0;

    // prepare the statement used to get the cvterm for each feature.
    $sel_cvterm_sql = "
      SELECT CVT.cvterm_id, CVT.cv_id, CVT.name, CVT.definition,
        CVT.dbxref_id, CVT.is_obsolete, CVT.is_relationshiptype
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE CV.cv_id = :cv_id and
       (lower(CVT.name) = lower(:name) or lower(CVTS.synonym) = lower(:synonym))
     ";

    // iterate through each line of the GFF file
    print "Parsing Line $line_num (0.00%). Memory: " . number_format(memory_get_usage()) . " bytes\r";
    while ($line = fgets($fh)) {
      $line_num++;
      $size = drupal_strlen($line);
      $num_read += $size;
      $intv_read += $size;

      if ($line_num < $start_line) {
        continue;
      }

      // update the job status every 1% features
      if ($job and $intv_read >= $interval) {
        $intv_read = 0;
        $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
        print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
        tripal_set_job_progress($job, intval(($num_read / $filesize) * 100));
      }

      // check to see if we have FASTA section, if so then set the variable
      // to start parsing
      if (preg_match('/^##FASTA/i', $line)) {
        if ($remove) {
          // we're done because this is a delete operation so break out of the loop.
          break;
        }
        tripal_feature_load_gff3_fasta($fh, $interval, $num_read, $intv_read, $line_num, $filesize, $job);
        continue;
      }
      // if the ##sequence-region line is present then we want to add a new feature
      if (preg_match('/^##sequence-region (.*?) (\d+) (\d+)$/i', $line, $region_matches)) {
        $rid = $region_matches[1];
        $rstart = $region_matches[2];
        $rend = $region_matches[3];
        if ($landmark_type) {
          $result = chado_query($sel_cvterm_sql, array(':cv_id' => $cv->cv_id, ':name' => $landmark_type, ':synonym' => $landmark_type));
          $cvterm = $result->fetchObject();
          if (!$cvterm) {
            tripal_report_error('T_gff3_loader', TRIPAL_ERROR, 'cannot find feature type \'%landmark_type\' on line %line_num of the GFF file',
              array('%landmark_type' => $landmark_type, '%line_num' => $line_num));
            return '';
          }
          tripal_feature_load_gff3_feature($organism, $analysis_id, $cvterm, $rid,
            $rid, '', 'f', 'f', 1, 0);
        }
        continue;
      }

      // skip comments
      if (preg_match('/^#/', $line)) {
        continue;
      }

      // skip empty lines
      if (preg_match('/^\s*$/', $line)) {
        continue;
      }

      // get the columns
      $cols = explode("\t", $line);
      if (sizeof($cols) != 9) {
        tripal_report_error('T_gff3_loader', TRIPAL_ERROR, 'improper number of columns on line %line_num',
          array('%line_num' => $line_num));
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
      $result = chado_query($sel_cvterm_sql, array(':cv_id' => $cv->cv_id, ':name' => $type, ':synonym' => $type));

      $cvterm = $result->fetchObject();
      if (!$cvterm) {
        tripal_report_error('T_gff3_loader', TRIPAL_ERROR, 'cannot find feature term \'%type\' on line %line_num of the GFF file',
          array('%type' => $type, '%line_num' => $line_num));
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

      // the organism to which a feature belongs can be set in the GFF
      // file using the 'organism' attribute.  By default we
      // set the $feature_organism variable to the default organism for the landmark
      $attr_organism = '';
      $feature_organism = $organism;

      foreach ($attrs as $attr) {
        $attr = rtrim($attr);
        $attr = ltrim($attr);
        if (strcmp($attr, '')==0) {
          continue;
        }
        if (!preg_match('/^[^\=]+\=.+$/', $attr)) {
          tripal_report_error('T_gff3_loader', TRIPAL_ERROR, 'Attribute is not correctly formatted on line %line_num: %attr',
            array('%line_num' => $line_num, '%attr' => $attr));
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
        $skip_feature = 0;  // if there is a problem with any of the attributes this variable gets set
        if (strcmp($tag_name, 'ID') == 0) {
          $attr_uniquename =  urldecode($tag[1]);
        }
        elseif (strcmp($tag_name, 'Name') == 0) {
          $attr_name =  urldecode($tag[1]);
        }
        elseif (strcmp($tag_name, 'organism') == 0) {
          $attr_organism = urldecode($tag[1]);
          $org_matches = array();
          if (preg_match('/^(.*?):(.*?)$/', $attr_organism, $org_matches)) {
            $values = array(
              'genus' => $org_matches[1],
              'species' => $org_matches[2],
            );
            $org = chado_select_record('organism', array("*"), $values);
            if (count($org) == 0) {
              if ($create_organism) {
                $feature_organism = (object) chado_insert_record('organism', $values);
                if (!$feature_organism) {
                  tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "Could not add the organism, '%org', from line %line. Skipping this line. ",
                    array('%org' => $attr_organism, '%line' => $line_num));
                  $skip_feature = 1;
                }
              }
              else {
                tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "The organism attribute '%org' on line %line does not exist. Skipping this line. ",
                  array('%org' => $attr_organism, '%line' => $line_num));
                $skip_feature = 1;
              }
            }
            else {
              // we found the organism in the database so use it
              $feature_organism = $org[0];
            }
          }
          else {
            tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "The organism attribute '%org' on line %line is not properly formated. It " .
              "should be of the form: organism=Genus:species.  Skipping this line.",
              array('%org' => $attr_organism, '%line' => $line_num));
            $skip_feature = 1;
          }
        }
        // get the list of non-reserved attributes
        elseif (strcmp($tag_name, 'Alias') != 0        and strcmp($tag_name, 'Parent') != 0 and
                strcmp($tag_name, 'Target') != 0       and strcmp($tag_name, 'Gap') != 0 and
                strcmp($tag_name, 'Derives_from') != 0 and strcmp($tag_name, 'Note') != 0 and
                strcmp($tag_name, 'Dbxref') != 0       and strcmp($tag_name, 'Ontology_term') != 0 and
                strcmp($tag_name, 'Is_circular') != 0  and strcmp($tag_name, 'target_organism') != 0 and
                strcmp($tag_name, 'target_type') != 0  and strcmp($tag_name, 'organism' != 0)) {
          foreach ($tags[$tag_name] as $value) {
            $attr_others[$tag_name][] = $value;
          }
        }
      }

      if ($skip_line) {
        continue;
      }

      // if neither name nor uniquename are provided then generate one
      if (!$attr_uniquename and !$attr_name) {
        // check if an alternate ID field is suggested, if so, then use
        // that for the name
        if (array_key_exists($alt_id_attr, $tags)) {
          $attr_uniquename = $tags[$alt_id_attr][0];
          $attr_name = $attr_uniquename;
        }
        // if the row has a parent then generate a uniquename using the parent name
        // add the date to the name in the event there are more than one child with
        // the same parent.
        elseif (array_key_exists('Parent', $tags)) {
          $date = getdate();
          $attr_uniquename = $tags['Parent'][0] . "-$type-$landmark-" . $date[0] . ":" . ($fmin + 1) . ".." . $fmax;
          $attr_name = $attr_uniquename;
        }
        // generate a unique name based on the date, type and location
        // and set the name to simply be the type
        else {
          $date = getdate();
          $attr_uniquename = $date[0] . "-$type-$landmark:" . ($fmin + 1) . ".." . $fmax;
          $attr_name = $type;
        }
      }

      // if a name is not specified then use the unique name as the name
      if (strcmp($attr_name, '')==0) {
        $attr_name = $attr_uniquename;
      }

      // if an ID attribute is not specified then use the attribute name plus the date
      if (!$attr_uniquename) {
        $date = getdate();
        $attr_uniquename = $attr_name . '-' . $date[0];
      }

      // make sure the landmark sequence exists in the database.  If the user
      // has not specified a landmark type (and it's not requiredin the GFF foramt)
      // then We don't know the type of the landmark so we'll hope that it's unique across
      // all types for the orgnaism. Only do this test if the landmark and the feature are
      // different.
      if (!$remove and !(strcmp($landmark, $attr_uniquename) == 0 or strcmp($landmark, $attr_name) == 0)) {
        $select = array(
          'organism_id' => $organism->organism_id,
          'uniquename'  => $landmark,
        );
        $columns = array('count(*) as num_landmarks');
        if ($landmark_type) {
          $select['type_id'] = array(
            'name' => $landmark_type,
          );
        }
        $count = chado_select_record('feature', $columns, $select);
        if (!$count or count($count) == 0 or $count[0]->num_landmarks == 0) {
          // now look for the landmark using the name rather than uniquename.
          $select = array(
            'organism_id' => $organism->organism_id,
            'name'  => $landmark,
          );
          $columns = array('count(*) as num_landmarks');
          if ($landmark_type) {
            $select['type_id'] = array(
              'name' => $landmark_type,
            );
          }
          $count = chado_select_record('feature', $columns, $select);
          if (!$count or count($count) == 0 or $count[0]->num_landmarks == 0) {
            tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "The landmark '%landmark' cannot be found for this organism (%species) " .
                  "Please add the landmark and then retry the import of this GFF3 " .
                  "file", array('%landmark' => $landmark, '%species' => $organism->genus . " " . $organism->species));
            return '';
          }
          elseif ($count[0]->num_landmarks > 1) {
            tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "The landmark '%landmark' has more than one entry for this organism (%species) " .
                  "Cannot continue", array('%landmark' => $landmark, '%species' => $organism->genus . " " . $organism->species));
            return '';
          }

        }
        if ($count[0]->num_landmarks > 1) {
          tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "The landmark '%landmark' is not unique for this organism. " .
                "The features cannot be associated", array('%landmark' => $landmark));
          return '';
        }
      }

      // if the option is to remove or refresh then we want to remove
      // the feature from the database.
      if ($remove or $refresh) {
        $sql = "DELETE FROM {feature}
                WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
        $match = array(
           'organism_id' => $feature_organism->organism_id,
           'uniquename'  => $attr_uniquename,
           'type_id'     => $cvterm->cvterm_id
        );
        $result = chado_delete_record('feature', $match);
        if (!$result) {
          tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "cannot delete feature %attr_uniquename",
            array('%attr_uniquename' => $attr_uniquename));
        }
        $feature = 0;
        unset($result);
      }

      // add or update the feature and all properties
      if ($update or $refresh or $add_only) {

        // add/update the feature
        $feature = tripal_feature_load_gff3_feature($feature_organism, $analysis_id, $cvterm,
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
          $results = chado_select_record('tripal_gff_temp', array('*'), $values);

          if (count($results) == 0) {
            $result = chado_insert_record('tripal_gff_temp', $values);
            if (!$result) {
              tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "Cound not save record in temporary table, Cannot continue.", array());
              exit;
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
            tripal_feature_load_gff3_parents($feature, $cvterm, $tags['Parent'], $feature_organism->organism_id, $fmin);
          }

          // add target relationships
          if (array_key_exists('Target', $tags)) {
            tripal_feature_load_gff3_target($feature, $tags, $target_organism_id, $target_type, $create_target, $attr_locgroup);
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
            tripal_feature_load_gff3_derives_from($feature, $tags['Derives_from'][0], $feature_organism);
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

    if (!$remove) {
      print "\nSetting ranks of children...\n";

      // get features in a relationship that are also children of an alignment
      $sql = "
        SELECT DISTINCT F.feature_id, F.organism_id, F.type_id,
          F.uniquename, FL.strand
        FROM {tripal_gff_temp} TGT
          INNER JOIN {feature} F                ON TGT.feature_id = F.feature_id
          INNER JOIN {feature_relationship} FR  ON FR.object_id   = TGT.feature_id
          INNER JOIN {cvterm} CVT               ON CVT.cvterm_id  = FR.type_id
          INNER JOIN {featureloc} FL            ON FL.feature_id  = F.feature_id
        WHERE CVT.name = 'part_of'
      ";
      $parents = chado_query($sql);

      // build and prepare the SQL for selecting the children relationship
      $sel_gffchildren_sql = "
        SELECT DISTINCT FR.feature_relationship_id, FL.fmin, FR.rank
        FROM {feature_relationship} FR
          INNER JOIN {featureloc} FL on FL.feature_id = FR.subject_id
        WHERE FR.object_id = :feature_id ORDER BY FL.fmin ASC
      ";

      // now set the rank of any parent/child relationships.  The order is based
      // on the fmin.  The start rank is 1.  This allows features with other
      // relationships to be '0' (the default), and doesn't interfer with the
      // ordering defined here.
      $num_recs = $parents->rowCount();
      $i = 1;
      $interval = intval($num_recs * 0.0001);
      if ($interval == 0) {
        $interval = 1;
      }
      $percent = sprintf("%.2f", ($i / $num_recs) * 100);
      print "Setting $i of $num_recs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";

      while ($parent = $parents->fetchObject()) {

        if ($i % $interval == 0) {
          $percent = sprintf("%.2f", ($i / $num_recs) * 100);
          print "Setting $i of $num_recs (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
        }

        // get the children
        $result = chado_query($sel_gffchildren_sql, array(':feature_id' => $parent->feature_id));

        // build an array of the children
        $children = array();
        while ($child = $result->fetchObject()) {
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
          $values = array('rank' => $rank);
          chado_update_record('feature_relationship', $match, $values);
          $rank--;
        }
        // now set the rank correctly. The rank should start at 0.
        $rank = 0;
        foreach ($children as $child) {
          $match = array('feature_relationship_id' => $child->feature_relationship_id);
          $values = array('rank' => $rank);
          //print "Was: " . $child->rank . " now $rank ($parent->strand)\n"     ;
          chado_update_record('feature_relationship', $match, $values);
          $rank++;
        }
        $i++;
      }
    }
  }
  catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    watchdog_exception('T_gff3_loader', $e);
    if ($use_transaction) {
      $transaction->rollback();
      print "FAILED: Rolling back database changes...\n";
    }
    else {
      print "FAILED\n";
    }
    return 0;
  }

  print "\nDone\n";
  return 1;
}

/**
 * Load the derives from attribute for a gff3 feature
 *
 * @param $feature
 * @param $subject
 * @param $organism
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_derives_from($feature, $subject, $organism) {

  // get the subject type
  $values = array(
    'organism_id' => $organism->organism_id,
    'uniquename' => $subject,
  );
  $result = chado_select_record('tripal_gff_temp', array('type_name'), $values);
  if (count($result) == 0) {
    tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot find subject type for feature in 'derives_from' relationship: %subject", array('%subject' => $subject));
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
  $sfeature = chado_select_record('feature', array('feature_id'), $match);
  if (count($sfeature)==0) {
    tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "Could not add 'Derives_from' relationship " .
      "for %uniquename and %subject.  Subject feature, '%subject', " .
      "cannot be found", array('%uniquename' => $feature->uniquename, '%subject' => $subject));
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
  $rel = chado_select_record('feature_relationship', array('*'), $values);
  if (count($rel) > 0) {
    return;
  }

  // finally insert the relationship if it doesn't exist
  $ret = chado_insert_record('feature_relationship', $values);
  if (!$ret) {
    tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Could not add 'Derives_from' relationship for $feature->uniquename and $subject",
      array());
  }
}

/**
 * Load the parents for a gff3 feature
 *
 * @param $feature
 * @param $cvterm
 * @param $parents
 * @param $organism_id
 * @param $fmin
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_parents($feature, $cvterm, $parents, $organism_id, $fmin) {

  $uname = $feature->uniquename;
  $type = $cvterm->name;
  $rel_type = 'part_of';

  // prepare these SQL statements that will be used repeatedly.
  $cvterm_sql = "
    SELECT CVT.cvterm_id
    FROM {cvterm} CVT
      INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
      LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
    WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)
  ";

  // iterate through the parents in the list
  foreach ($parents as $parent) {
    // get the parent cvterm
    $values = array(
      'organism_id' => $organism_id,
      'uniquename' => $parent,
    );
    $result = chado_select_record('tripal_gff_temp', array('type_name'), $values);
    if (count($result) == 0) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot find parent: %parent", array('%parent' => $parent));
       return '';
    }
    $parent_type = $result[0]->type_name;

    // try to find the parent
    $parentcvterm = chado_query($cvterm_sql, array(':cvname' => 'sequence', ':name' => $parent_type, ':synonym' => $parent_type))->fetchObject();
    $relcvterm = chado_query($cvterm_sql, array(':cvname' => 'relationship', ':name' => $rel_type, ':synonym' => $rel_type))->fetchObject();
    $values = array(
        'organism_id' => $organism_id,
        'uniquename' => $parent,
        'type_id' => $parentcvterm->cvterm_id,
    );
    $result = chado_select_record('feature', array('feature_id'), $values);
    $parent_feature = $result[0];

    // if the parent exists then add the relationship otherwise print error and skip
    if ($parent_feature) {

      // check to see if the relationship already exists
      $values = array(
        'object_id' => $parent_feature->feature_id,
        'subject_id' => $feature->feature_id,
        'type_id' => $relcvterm->cvterm_id,
      );
      $rel = chado_select_record('feature_relationship', array('*'), $values);

      if (count($rel) > 0) {
      }
      else {
        // the relationship doesn't already exist, so add it.
        $values = array(
          'subject_id' => $feature->feature_id,
          'object_id'  => $parent_feature->feature_id,
          'type_id' => $relcvterm->cvterm_id,
        );
        $result = chado_insert_record('feature_relationship', $values);
        if (!$result) {
          tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type)",
            array());
        }
      }
    }
    else {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot establish relationship '$uname' ($type) $rel_type '$parent' ($parent_type): Cannot find the parent",
        array());
    }
  }
}

/**
 * Load the dbxref attribute for a feature
 *
 * @param $feature
 * @param $dbxrefs
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
    $db = chado_select_record('db', array('db_id'), $values);
    if (count($db) == 0) {
      $values = array('name' => "$dbname");
      $db = chado_select_record('db', array('db_id'), $values);
    }
    if (count($db) == 0) {
      $values = array(
        'name' => $dbname,
        'description' => 'Added automatically by the GFF loader'
      );
      $success = chado_insert_record('db', $values);
      if ($success) {
        $values = array('name' => "$dbname");
        $db = chado_select_record('db', array('db_id'), $values);
      }
      else {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot find or add the database $dbname", array());
        return 0;
      }
    }
    $db = $db[0];

    // now check to see if the accession exists
    $values = array(
      'accession' => $accession,
      'db_id' => $db->db_id
    );
    $dbxref = chado_select_record('dbxref', array('dbxref_id'), $values);

    // if the accession doesn't exist then we want to add it
    if (sizeof($dbxref) == 0) {
      $values = array(
        'db_id' => $db->db_id,
        'accession' => $accession,
        'version' => ''
      );
      $ret = chado_insert_record('dbxref', $values);
      $values = array(
        'accession' => $accession,
        'db_id' => $db->db_id
      );
      $dbxref = chado_select_record('dbxref', array('dbxref_id'), $values);
    }
    $dbxref = $dbxref[0];

    // check to see if this feature dbxref already exists
    $values = array(
      'dbxref_id' => $dbxref->dbxref_id,
      'feature_id' => $feature->feature_id
    );
    $fdbx = chado_select_record('feature_dbxref', array('feature_dbxref_id'), $values);

    // now associate this feature with the database reference if it doesn't
    // already exist
    if (sizeof($fdbx) == 0) {
      $values = array(
        'dbxref_id' => $dbxref->dbxref_id,
        'feature_id' => $feature->feature_id
      );
      $success = chado_insert_record('feature_dbxref', $values);
      if (!$success) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to insert Dbxref: $dbname:$accession", array());
        return 0;
      }
    }
  }
  return 1;
}

/**
 * Load the cvterms for a feature. Assumes there is a dbxref.accession matching a cvterm.name
 *
 * @param $feature
 * @param $dbxrefs
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
    $db = chado_select_record('db', array('db_id'), array('name' => "DB:$dbname"));
    if (sizeof($db) == 0) {
      // now look for the name without the 'DB:' prefix.
      $db = chado_select_record('db', array('db_id'), array('name' => "$dbname"));
      if (sizeof($db) == 0) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Database, $dbname, is not present. Cannot associate term: $dbname:$accession", array());
        return 0;
      }
    }
    $db = $db[0];

    // now check to see if the accession exists
    $dbxref = chado_select_record('dbxref', array('dbxref_id'),
      array('accession' => $accession, 'db_id' => $db->db_id));
    if (sizeof($dbxref) == 0) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Accession, $accession is missing for reference: $dbname:$accession", array());
      return 0;
    }
    $dbxref = $dbxref[0];

    // now check to see if the cvterm exists
    $cvterm = chado_select_record('cvterm', array('cvterm_id'), array(
       'dbxref_id' => $dbxref->dbxref_id));
    // if it doesn't exist in the cvterm table, look for an alternate id
    if (sizeof($cvterm) == 0) {
      $cvterm = chado_select_record('cvterm_dbxref', array('cvterm_id'), array(
        'dbxref_id' => $dbxref->dbxref_id));
      if (sizeof($cvterm) == 0) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "CV Term is missing for reference: $dbname:$accession", array());
        return 0;
      }
    }
    $cvterm = $cvterm[0];


    // check to see if this feature cvterm already exists
    $fcvt = chado_select_record('feature_cvterm', array('feature_cvterm_id'),
      array('cvterm_id' => $cvterm->cvterm_id, 'feature_id' => $feature->feature_id));

    // now associate this feature with the cvterm if it doesn't already exist
    if (sizeof($fcvt)==0) {
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id,
        'feature_id' => $feature->feature_id,
        'pub_id' => array(
          'uniquename' => 'null',
        ),
      );
      $success = chado_insert_record('feature_cvterm', $values);

      if (!$success) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to insert ontology term: $dbname:$accession", array());
        return 0;
      }
    }
  }
  return 1;
}

/**
 * Load any aliases for a feature
 *
 * @param $feature
 * @param $aliases
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_alias($feature, $aliases) {

  // make sure we have a 'synonym_type' vocabulary
  $select = array('name' => 'synonym_type');
  $results = chado_select_record('cv', array('*'), $select);

  if (count($results) == 0) {
    // insert the 'synonym_type' vocabulary
    $values = array(
      'name' => 'synonym_type',
      'definition' => 'vocabulary for synonym types',
    );
    $success = chado_insert_record('cv', $values);
    if (!$success) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to add the synonyms type vocabulary", array());
      return 0;
    }
    // now that we've added the cv we need to get the record
    $results = chado_select_record('cv', array('*'), $select);
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
  $result = chado_select_record('cvterm', array('*'), $select);
  if (count($result) == 0) {
    $term = array(
      'name' => 'exact',
      'id' => "internal:exact",
      'definition' => '',
      'is_obsolete' => 0,
    );
    $syntype = tripal_cv_add_cvterm($term, $syncv->name, 0, 1);
    if (!$syntype) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot add synonym type: internal:$type", array());
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
    $result = chado_select_record('synonym', array('*'), $select);
    if (count($result) == 0) {
      $values = array(
         'name' => $alias,
         'type_id' => $syntype->cvterm_id,
         'synonym_sgml' => '',
      );
      $success = chado_insert_record('synonym', $values);
      if (!$success) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot add alias $alias to synonym table", array());
        return 0;
      }
      $result = chado_select_record('synonym', array('*'), $select);
      $synonym = $result[0];
    }
    else {
      $synonym = $result[0];
    }

    // check to see if we have a NULL publication in the pub table.  If not,
    // then add one.
    $select = array('uniquename' => 'null');
    $result = chado_select_record('pub', array('*'), $select);
    if (count($result) == 0) {
      $pub_sql = "
        INSERT INTO {pub} (uniquename,type_id)
        VALUES (:uname,
          (SELECT cvterm_id
           FROM {cvterm} CVT
             INNER JOIN {dbxref} DBX ON DBX.dbxref_id = CVT.dbxref_id
             INNER JOIN {db} DB      ON DB.db_id      = DBX.db_id
           WHERE CVT.name = :type_id))
      ";
      $status = chado_query($psql);
      if (!$status) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot prepare statement 'ins_pub_uniquename_typeid", array());
        return 0;
      }

      // insert the null pub
      $result = chado_query($pub_sql, array(':uname' => 'null', ':type_id' => 'null'))->fetchObject();
      if (!$result) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot add null publication needed for setup of alias", array());
        return 0;
      }
      $result = chado_select_record('pub', array('*'), $select);
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
    $result = chado_select_record('feature_synonym', $columns, $values);
    if (count($result) == 0) {
      $values = array(
         'synonym_id' => $synonym->synonym_id,
         'feature_id' => $feature->feature_id,
         'pub_id' => $pub->pub_id,
      );
      $success = chado_insert_record('feature_synonym', $values);

      if (!$success) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot add alias $alias to feature synonym table", array());
        return 0;
      }
    }
  }
  return 1;
}

/**
 * Create the feature record & link it to it's analysis
 *
 * @param $organism
 * @param $analysis_id
 * @param $cvterm
 * @param $uniquename
 * @param $name
 * @param $residues
 * @param $is_analysis
 * @param $is_obsolete
 * @param $add_only
 * @param $score
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_feature($organism, $analysis_id, $cvterm, $uniquename,
  $name, $residues, $is_analysis = 'f', $is_obsolete = 'f', $add_only, $score) {

  // check to see if the feature already exists
  $feature = NULL;
  $fselect = array(
     'organism_id' => $organism->organism_id,
     'uniquename' => $uniquename,
     'type_id' => $cvterm->cvterm_id
  );
  $columns = array('feature_id', 'name', 'uniquename', 'seqlen', 'organism_id', 'type_id');
  $result = chado_select_record('feature', $columns, $fselect);
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
    $result = chado_insert_record('feature', $values);
    if (!$result) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to insert feature '$uniquename' ($cvterm->name)", array());
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
    $result = chado_update_record('feature', $match, $values);
    if (!$result) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to update feature '$uniquename' ($cvterm->name)", array());
      return 0;
    }
  }
  else {
    // the feature exists and we don't want to update it so return
    // a value of 0.  This will stop all downstream property additions
    return 0;
  }

  // get the newly added feature
  $columns = array('feature_id', 'name', 'uniquename', 'seqlen', 'organism_id', 'type_id');
  $result = chado_select_record('feature', $columns, $fselect);
  $feature = $result[0];

  // add the analysisfeature entry to the analysisfeature table if it doesn't already exist
  $af_values = array(
    'analysis_id' => $analysis_id,
    'feature_id' => $feature->feature_id
  );
  $afeature = chado_select_record('analysisfeature', array('analysisfeature_id'), $af_values);
  if (count($afeature)==0) {
    // if a score is available then set that to be the significance field
    if (strcmp($score, '.') != 0) {
      $af_values['significance'] = $score;
    }
    if (!tripal_core_chado_insert('analysisfeature', $af_values)) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Could not add analysisfeature record: $analysis_id, $feature->feature_id", array());
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
      $ret = chado_update_record('analysisfeature', $af_values, $new_vals);
      if (!$ret) {
        tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Could not update analysisfeature record: $analysis_id, $feature->feature_id", array());
      }
    }
  }
  return $feature;
}

/**
 * Insert the location of the feature
 *
 * @param $feature
 * @param $organism
 * @param $landmark
 * @param $fmin
 * @param $fmax
 * @param $strand
 * @param $phase
 * @param $is_fmin_partial
 * @param $is_fmax_partial
 * @param $residue_info
 * @param $locgroup
 * @param $landmark_type_id
 * @param $landmark_organism_id
 * @param $create_landmark
 * @param $landmark_is_target
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_featureloc($feature, $organism, $landmark, $fmin,
  $fmax, $strand, $phase, $is_fmin_partial, $is_fmax_partial, $residue_info, $locgroup,
  $landmark_type_id = '', $landmark_organism_id = '', $create_landmark = 0,
  $landmark_is_target = 0) {

  $select = array(
    'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
    'uniquename' => $landmark,
  );
  if ($landmark_type_id) {
    $select['type_id'] = $landmark_type_id;
  }
  $results = chado_select_record('feature', array('feature_id'), $select);

  $srcfeature = '';
  if (count($results)==0) {
    // so we couldn't find the landmark using the uniquename. Let's try the 'name'.
    // if we return only a single result then we can proceed. Otherwise give an
    $select = array(
      'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
      'name' => $landmark,
    );
    if ($landmark_type_id) {
      $select['type_id'] = $landmark_type_id;
    }
    $results = chado_select_record('feature', array('feature_id'), $select);
    if (count($results) == 0) {
       // if the landmark is the target feature in a matched alignment then try one more time to
       // find it by querying any feature with the same uniquename. If we find one then use it.
       if ($landmark_is_target) {
         $select = array('uniquename' => $landmark);
         $results = chado_select_record('feature', array('feature_id'), $select);
         if (count($results) == 1) {
           $srcfeature = $results[0];
         }
       }

       if (!$srcfeature) {
         // we couldn't find the landmark feature, so if the user has requested we create it then do so
         // but only if we have a type id
         if ($create_landmark and $landmark_type_id) {
            $values = array(
              'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
              'name' => $landmark,
              'uniquename' => $landmark,
              'type_id' => $landmark_type_id
            );
            $results = chado_insert_record('feature', $values);
            if (!$results) {
              tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot find landmark feature: '%landmark', nor could it be inserted",
                array('%landmark' => $landmark));
              return 0;
            }
            $srcfeature = new stdClass();
            $srcfeature->feature_id = $results['feature_id'];
         }
         else {
           tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot find unique landmark feature: '%landmark'.",
             array('%landmark' => $landmark));
           return 0;
         }
       }
    }
    elseif (count($results) > 1) {
       tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "multiple landmarks exist with the name: '%landmark'.  Cannot
         resolve which one to use. Cannot add the feature location record",
         array('%landmark' => $landmark));
       return 0;
    }
    else {
      $srcfeature = $results[0];
    }
  }
  elseif (count($results) > 1) {
    tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "multiple landmarks exist with the name: '%landmark'.  Cannot
      resolve which one to use. Cannot add the feature location record",
      array('%landmark' => $landmark));
    return 0;
  }
  else {
    $srcfeature = $results[0];
  }

  // TODO: create an attribute that recognizes the residue_info,locgroup,
  //  is_fmin_partial and is_fmax_partial, right now these are
  //  hardcoded to be false and 0 below.

  // check to see if this featureloc already exists, but also keep track of the
  // last rank value
  $rank = 0;
  $exists = 0;
  $select = array('feature_id' => $feature->feature_id);
  $options = array(
    'order_by' => array(
       'rank' => 'ASC'
    ),
  );

  $locrecs = chado_select_record('featureloc', array('*'), $select, $options);

  foreach ($locrecs as $featureloc) {
    // it is possible for the featureloc->srcfeature_id to be NULL. This can happen if the srcfeature
    // is not known (according to chado table field descriptions).  If it's null then just skip this entry
    if (!$featureloc->srcfeature_id) {
      continue;
    }
    $select = array('feature_id' => $featureloc->srcfeature_id);
    $columns = array('feature_id', 'name');
    $locsfeature = chado_select_record('feature', $columns, $select);

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
        chado_update_record('featureloc', $match, $values);
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
    if ($phase) {
      $values['phase'] = $phase;
    }
    $success = chado_insert_record('featureloc', $values);
    if (!$success) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Failed to insert featureloc", array());
      exit;
      return 0;
    }
  }
  return 1;
}

/**
 * Load a preoprty (featurepop) for the feature
 *
 * @param $feature
 * @param $property
 * @param $value
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
  $result = chado_select_record('cvterm', array('*'), $select);

  // if we don't have a property like this already, then add it otherwise, just return
  if (count($result) == 0) {
    $term = array(
      'id' => "null:$property",
      'name' => $property,
      'namespace' => 'feature_property',
      'is_obsolete' => 0,
    );
    $cvterm = (object) tripal_cv_add_cvterm($term, 'feature_property', 0, 0);
    if (!$cvterm) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "Cannot add cvterm, $property", array());
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
    'order_by' => array(
      'rank' => 'ASC',
    ),
  );
  $results = chado_select_record('featureprop', array('*'), $select, $options);
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
    $result = chado_insert_record('featureprop', $values);
    if (!$result) {
      tripal_report_error("T_gff3_loader", TRIPAL_WARNING, "cannot add featureprop, $property", array());
    }
  }
}

/**
 * Load the FASTA sequences at the bottom of a GFF3 file
 *
 * @param $fh
 * @param $interval
 * @param $num_read
 * @param $intv_read
 * @param $line_num
 * @param $filesize
 * @param $job
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_fasta($fh, $interval, &$num_read, &$intv_read, &$line_num, $filesize, $job) {
  print "\nLoading FASTA sequences\n";
  $residues = '';
  $id = NULL;

  $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
  print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
  // iterate through the remaining lines of the file
  while ($line = fgets($fh)) {

    $line_num++;
    $size = drupal_strlen($line);
    $num_read += $size;
    $intv_read += $size;

    $line = trim($line);

    // update the job status every 1% features
    if ($job and $intv_read >= $interval) {
      $intv_read = 0;
      $percent = sprintf("%.2f", ($num_read / $filesize) * 100);
      print "Parsing Line $line_num (" . $percent . "%). Memory: " . number_format(memory_get_usage()) . " bytes.\r";
      tripal_set_job_progress($job, intval(($num_read / $filesize) * 100));
    }

    // if we encounter a definition line then get the name, uniquename,
    // accession and relationship subject from the definition line
    if (preg_match('/^>/', $line)) {

      // if we are beginning a new sequence then save to the database the last one we just finished.
      if ($id) {
        $values = array('uniquename' => $id);
        $result = chado_select_record('tripal_gff_temp', array('*'), $values);
        if (count($result) == 0) {
          tripal_report_error('T_gff3_loader', TRIPAL_WARNING, 'Cannot find feature to assign FASTA sequence: %uname',
             array('%uname' => $id));
        }
        else {
          // if we have a feature then add the residues
          $feature = $result[0];
          $values = array('residues' => $residues);
          $match = array('feature_id' => $feature->feature_id);
          chado_update_record('feature', $match, $values);
        }
      }

      // get the feature ID for this ID from the tripal_gff_temp table
      $id = preg_replace('/^>(.*)$/', '\1', $line);
      $residues = '';
    }
    else {
      $residues .= trim($line);
    }
  }

  // add in the last sequence
  $values = array('uniquename' => $id);
  $result = chado_select_record('tripal_gff_temp', array('*'), $values);
  if (count($result) == 0) {
    tripal_report_error('T_gff3_loader', TRIPAL_WARNING, 'Cannot find feature to assign FASTA sequence: %uname',
       array('%uname' => $id));
  }
  else {
    // if we have a feature then add the residues
    $feature = $result[0];
    $values = array('residues' => $residues);
    $match = array('feature_id' => $feature->feature_id);
    chado_update_record('feature', $match, $values);
  }

}

/**
 * Load the target attribute of a gff3 record
 *
 * @param $feature
 * @param $tags
 * @param $target_organism_id
 * @param $target_type
 * @param $create_target
 * @param $attr_locgroup
 *
 * @ingroup gff3_loader
 */
function tripal_feature_load_gff3_target($feature, $tags, $target_organism_id, $target_type, $create_target, $attr_locgroup) {
  // format is: "target_id start end [strand]", where strand is optional and may be "+" or "-"
  $matched = preg_match('/^(.*?)\s+(\d+)\s+(\d+)(\s+[\+|\-])*$/', trim($tags['Target'][0]), $matches);

  // the organism and type of the target may also be specified as an attribute. If so, then get that
  // information
  $gff_target_organism = array_key_exists('target_organism', $tags) ? $tags['target_organism'][0] : '';
  $gff_target_type = array_key_exists('target_type', $tags) ? $tags['target_type'][0] : '';

  // if we have matches and the Target is in the correct format then load the alignment
  if ($matched) {
    $target_feature = $matches[1];
    $start = $matches[2];
    $end = $matches[3];
    // if we have an optional strand, convert it to a numeric value.
    if ($matches[4]) {
      if (preg_match('/^\+$/', trim($matches[4]))) {
        $target_strand = 1;
      }
      elseif (preg_match('/^\-$/', trim($matches[4]))) {
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

    // default the target organism to be the value passed into the function, but if the GFF
    // file species the target organism then use that instead.
    $t_organism_id = $target_organism_id;
    if ($gff_target_organism) {
      // get the genus and species
      $success = preg_match('/^(.*?):(.*?)$/', $gff_target_organism, $matches);
      if ($success) {
        $values = array(
          'genus' => $matches[1],
          'species' => $matches[2],
        );
        $torganism = chado_select_record('organism', array('organism_id'), $values);
        if (count($torganism) == 1) {
          $t_organism_id = $torganism[0]->organism_id;
        }
        else {
          tripal_report_error('T_gff3_loader', TRIPAL_WARNING, "Cannot find organism for target %target.",
            array('%target' => $gff_target_organism));
          $t_organism_id = '';
        }
      }
      else {
        tripal_report_error('T_gff3_loader', TRIPAL_WARNING, "The target_organism attribute is improperly formatted: %target.
          It should be target_organism=genus:species.",
          array('%target' => $gff_target_organism));
        $t_organism_id = '';
      }
    }

    // default the target type to be the value passed into the function, but if the GFF file
    // species the target type then use that instead
    $t_type_id = '';
    if ($target_type) {
      $values = array(
        'name' => $target_type,
        'cv_id' => array(
           'name' => 'sequence',
        )
      );
      $type = chado_select_record('cvterm', array('cvterm_id'), $values);
      if (count($type) == 1) {
        $t_type_id = $type[0]->cvterm_id;
      }
      else {
        tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "The target type does not exist in the sequence ontology: %type. ",
          array('%type' => $target_type));
        exit;
      }
    }
    if ($gff_target_type) {
      $values = array(
        'name' => $gff_target_type,
        'cv_id' => array(
           'name' => 'sequence',
        )
      );

      // get the cvterm_id for the target type
      $type = chado_select_record('cvterm', array('cvterm_id'), $values);
      if (count($type) == 1) {
        $t_type_id = $type[0]->cvterm_id;
      }
      else {
        // check to see if this is a synonym
        $sql = "
          SELECT CVTS.cvterm_id
          FROM {cvtermsynonym} CVTS
            INNER JOIN {cvterm} CVT ON CVT.cvterm_id = CVTS.cvterm_id
            INNER JOIN {cv} CV      ON CV.cv_id = CVT.cv_id
          WHERE CV.name = 'sequence' and CVTS.synonym = :synonym
        ";
        $synonym = chado_query($sql, array(':synonym' => $gff_target_type))->fetchObject();
        if ($synonym) {
          $t_type_id = $synonym->cvterm_id;
        }
        else {
          tripal_report_error('T_gff3_loader', TRIPAL_WARNING, "The target_type attribute does not exist in the sequence ontology: %type. ",
            array('%type' => $gff_target_type));
          $t_type_id = '';
        }
      }
    }

    // we want to add a featureloc record that uses the target feature as the srcfeature (landmark)
    // and the landmark as the feature.
    tripal_feature_load_gff3_featureloc($feature, $organism, $target_feature, $target_fmin,
      $target_fmax, $target_strand, $phase, $attr_fmin_partial, $attr_fmax_partial, $attr_residue_info,
      $attr_locgroup, $t_type_id, $t_organism_id, $create_target, TRUE);
  }
  // the target attribute is not correctly formatted
  else {
    tripal_report_error('T_gff3_loader', TRIPAL_ERROR, "Could not add 'Target' alignment as it is improperly formatted:  '%target'",
      array('%target' => $tags['Target'][0]));
  }
}
