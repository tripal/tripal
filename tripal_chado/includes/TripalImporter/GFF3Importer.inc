<?php

class GFF3Importer extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado GFF3 File Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_gff3_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Import a GFF3 file into Chado';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['gff', 'gff3'];


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please provide the GFF3 file.';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'GFF3 File';

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import GFF3 file';

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    // get the list of organisms
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $org_rset = chado_query($sql);
    $organisms = [];
    $organisms[''] = '';
    while ($organism = $org_rset->fetchObject()) {
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
    }
    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#description' => t("Choose the organism to which these sequences are associated"),
      '#required' => TRUE,
      '#options' => $organisms,
    ];

    // Advanced Options
    $form['advanced'] = [
      '#type' => 'fieldset',
      '#title' => t('Additional Options'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];


    $form['advanced']['line_number'] = [
      '#type' => 'textfield',
      '#title' => t('Start Line Number'),
      '#description' => t('Enter the line number in the GFF file where you would like to begin processing.  The
      first line is line number 1.  This option is useful for examining loading problems with large GFF files.'),
      '#size' => 10,
    ];

    $form['advanced']['landmark_type'] = [
      '#title' => t('Landmark Type'),
      '#type' => t('textfield'),
      '#description' => t("Optional. Use this field to specify a Sequence Ontology type
       for the landmark sequences in the GFF fie (e.g. 'chromosome'). If the GFF file
       contains a '##sequence-region' line that describes the landmark sequences to
       which all others are aligned and a type is provided here then the features
       will be created if they do not already exist.  If they do exist then this
       field is not used."),
    ];

    $form['advanced']['alt_id_attr'] = [
      '#title' => t('ID Attribute'),
      '#type' => t('textfield'),
      '#description' => t("Optional. Sometimes lines in the GFF file are missing the
      required ID attribute that specifies the unique name of the feature, but there
      may be another attribute that can uniquely identify the feature.  If so,
      you may specify the name of the attribute to use for the name."),
    ];


    $form['advanced']['skip_protein'] = [
      '#type' => 'checkbox',
      '#title' => t('Skip automatic protein creation'),
      '#required' => FALSE,
      '#description' => t('The GFF loader will automatically create a protein feature for each transcript in the GFF file if a protein feature is missing in the GFF file. Check this box to disable this functionality. Protein features that are specifically present in the GFF will always be created.'),
      '#default_value' => 0,
    ];


    $form['advanced']['protein_names'] = [
      '#type' => 'fieldset',
      '#title' => t('Protein Names'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => 5,
    ];

    $form['advanced']['protein_names']['re_help'] = [
      '#type' => 'item',
      '#markup' => t('A regular expression is an advanced method for extracting information from a string of text.
                   If your GFF3 file does not contain polypeptide (or protein) features, but contains CDS features, proteins will be automatically created.
                   By default the loader will give each protein a name based on the name of the corresponding mRNA followed by the "-protein" suffix.
                   If you want to customize the name of the created protein, you can use the following regex.'),
    ];
    $form['advanced']['protein_names']['re_mrna'] = [
      '#type' => 'textfield',
      '#title' => t('Regular expression for the mRNA name'),
      '#required' => FALSE,
      '#description' => t('Enter the regular expression that will extract portions of
       the mRNA unique name. For example, for a
       mRNA with a unique name finishing by -RX (e.g. SPECIES0000001-RA),
       the regular expression would be, "^(.*?)-R([A-Z]+)$".'),
    ];
    $form['advanced']['protein_names']['re_protein'] = [
      '#type' => 'textfield',
      '#title' => t('Replacement string for the protein name'),
      '#required' => FALSE,
      '#description' => t('Enter the replacement string that will be used to create
       the protein name based on the mRNA regular expression. For example, for a
       mRNA regular expression "^(.*?)-R()[A-Z]+)$", the corresponding protein regular
       expression would be "$1-P$2".'),
    ];

    $form['advanced']['use_transaction'] = [
      '#type' => 'checkbox',
      '#title' => t('Use a transaction'),
      '#required' => FALSE,
      '#description' => t('Use a database transaction when loading the GFF file.  If an error occurs
      the entire datset loaded prior to the failure will be rolled back and will not be available
      in the database.  If this option is unchecked and failure occurs all records up to the point
      of failure will be present in the database.'),
      '#default_value' => 1,
    ];
    $form['advanced']['add_only'] = [
      '#type' => 'checkbox',
      '#title' => t('Import only new features'),
      '#required' => FALSE,
      '#description' => t('The job will skip features in the GFF file that already
                         exist in the database and import only new features.'),
    ];
    $form['advanced']['update'] = [
      '#type' => 'checkbox',
      '#title' => t('Import all and update'),
      '#required' => FALSE,
      '#default_value' => 'checked',
      '#description' => t('Existing features will be updated and new features will be added.  Attributes
                         for a feature that are not present in the GFF but which are present in the
                         database will not be altered.'),
      '#default_value' => 1,
    ];
    // SPF: there are bugs in refreshing and removing features.  The bugs arise
    //      if a feature in the GFF does not have a uniquename. GenSAS will auto
    //      generate this uniquename and it will not be the same as a previous
    //      load because it uses the date.  This causes orphaned CDS/exons, UTRs
    //      to be left behind during a delete or refresh.  So, the short term
    //      fix is to remove these options.
    //   $form['import_options']['refresh']= array(
    //     '#type' => 'checkbox',
    //     '#title' => t('Import all and replace'),
    //     '#required' => FALSE,
    //     '#description' => t('Existing features will be updated and feature properties not
    //                          present in the GFF file will be removed.'),
    //   );
    //   $form['import_options']['remove']= array(
    //     '#type' => 'checkbox',
    //     '#title' => t('Delete features'),
    //     '#required' => FALSE,
    //     '#description' => t('Features present in the GFF file that exist in the database
    //                          will be removed rather than imported'),
    //   );
    $form['advanced']['create_organism'] = [
      '#type' => 'checkbox',
      '#title' => t('Create organism'),
      '#required' => FALSE,
      '#description' => t('The Tripal GFF loader supports the "organism" attribute. This allows features of a
       different organism to be aligned to the landmark sequence of another species.  The format of the
       attribute is "organism=[genus]:[species]", where [genus] is the organism\'s genus and [species] is the
       species name. Check this box to automatically add the organism to the database if it does not already exists.
       Otherwise lines with an oraganism attribute where the organism is not present in the database will be skipped.'),
    ];

    $form['advanced']['targets'] = [
      '#type' => 'fieldset',
      '#title' => t('Targets'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => 1,
    ];
    $form['advanced']['targets']['adesc'] = [
      '#markup' => t("When alignments are represented in the GFF file (e.g. such as
       alignments of cDNA sequences to a whole genome, or blast matches), they are
       represented using two feature types: 'match' (or cDNA_match, EST_match, etc.)
       and 'match_part'.  These features may also have a 'Target' attribute to
       specify the sequence that is being aligned.
       However, the organism to which the aligned sequence belongs may not be present in the
       GFF file.  Here you can specify the organism and feature type of the target sequences.
       The options here will apply to all targets unless the organism and type are explicity
       set in the GFF file using the 'target_organism' and 'target_type' attributes."),
    ];
    $form['advanced']['targets']['target_organism_id'] = [
      '#title' => t('Target Organism'),
      '#type' => t('select'),
      '#description' => t("Optional. Choose the organism to which target sequences belong.
      Select this only if target sequences belong to a different organism than the
      one specified above. And only choose an organism here if all of the target sequences
      belong to the same species.  If the targets in the GFF file belong to multiple
      different species then the organism must be specified using the 'target_organism=genus:species'
      attribute in the GFF file."),
      '#options' => $organisms,
    ];
    $form['advanced']['targets']['target_type'] = [
      '#title' => t('Target Type'),
      '#type' => t('textfield'),
      '#description' => t("Optional. If the unique name for a target sequence is not unique (e.g. a protein
       and an mRNA have the same name) then you must specify the type for all targets in the GFF file. If
       the targets are of different types then the type must be specified using the 'target_type=type' attribute
       in the GFF file. This must be a valid Sequence Ontology (SO) term."),
    ];
    $form['advanced']['targets']['create_target'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Target'),
      '#required' => FALSE,
      '#description' => t("If the target feature cannot be found, create one using the organism and type specified above, or
       using the 'target_organism' and 'target_type' fields specified in the GFF file.  Values specified in the
       GFF file take precedence over those specified above."),
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    $organism_id = $form_state['values']['organism_id'];
    $target_organism_id = $form_state['values']['target_organism_id'];
    $target_type = trim($form_state['values']['target_type']);
    $create_target = $form_state['values']['create_target'];
    $create_organism = $form_state['values']['create_organism'];
    $add_only = $form_state['values']['add_only'];
    $update = $form_state['values']['update'];
    $refresh = 0; //$form_state['values']['refresh'];
    $remove = 0; //$form_state['values']['remove'];
    $use_transaction = $form_state['values']['use_transaction'];
    $line_number = trim($form_state['values']['line_number']);
    $landmark_type = trim($form_state['values']['landmark_type']);
    $alt_id_attr = trim($form_state['values']['alt_id_attr']);
    $re_mrna = trim($form_state['values']['re_mrna']);
    $re_protein = trim($form_state['values']['re_protein']);

    // @coder-ignore: there are no functions being called here
    if (($add_only AND ($update OR $refresh OR $remove)) OR
      ($update AND ($add_only OR $refresh OR $remove)) OR
      ($refresh AND ($update OR $add_only OR $remove)) OR
      ($remove AND ($update OR $refresh OR $add_only))) {
      form_set_error('add_only', t("Please select only one checkbox from the import options section"));
    }

    if ($line_number and !is_numeric($line_number) or $line_number < 0) {
      form_set_error('line_number', t("Please provide an integer line number greater than zero."));
    }

    if (!($re_mrna and $re_protein) and ($re_mrna or $re_protein)) {
      form_set_error('re_uname', t("You must provide both a regular expression for mRNA and a replacement string for protein"));
    }

    // check the regular expression to make sure it is valid
    set_error_handler(function () {
    }, E_WARNING);
    $result_re = preg_match("/" . $re_mrna . "/", NULL);
    $result = preg_replace("/" . $re_mrna . "/", $re_protein, NULL);
    restore_error_handler();
    if ($result_re === FALSE) {
      form_set_error('re_mrna', 'Invalid regular expression.');
    }
    else {
      if ($result === FALSE) {
        form_set_error('re_protein', 'Invalid replacement string.');
      }
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['files'][0]['file_path'];

    $organism_id = $arguments['organism_id'];
    $analysis_id = $arguments['analysis_id'];
    $add_only = $arguments['add_only'];
    $update = $arguments['update'];
    $refresh = FALSE;
    $remove = FALSE;
    $use_transaction = $arguments['use_transaction'];
    $target_organism_id = $arguments['target_organism_id'];
    $target_type = $arguments['target_type'];
    $create_target = $arguments['create_target'];
    $start_line = $arguments['line_number'];
    $landmark_type = $arguments['landmark_type'];
    $alt_id_attr = $arguments['alt_id_attr'];
    $create_organism = $arguments['create_organism'];
    $re_mrna = $arguments['re_mrna'];
    $re_protein = $arguments['re_protein'];
    $skip_protein = $arguments['skip_protein'];


    $this->loadGFF3($file_path, $organism_id, $analysis_id,
      $add_only, $update, $refresh, $remove, $use_transaction,
      $target_organism_id, $target_type, $create_target,
      $start_line, $landmark_type, $alt_id_attr, $create_organism,
      $re_mrna, $re_protein, $skip_protein);
  }

  /**
   * Actually load a GFF3 file. This is the function called by tripal jobs
   *
   * @param $gff_file
   *   The full path to the GFF file on the filesystem
   * @param $organism_id
   *   The organism_id of the organism to which the features in the GFF belong
   * @param $analysis_id
   *   The anlaysis_id of the analysis from which the features in the GFF were
   *   generated
   * @param $add_only
   *   Set to 1 if feature should be added only.  In the case where a feature
   *   already exists, it will not be updated.  Default is 0
   * @param $update
   *   Set to 1 to update existing features. New features will be added.
   *   Attributes for a feature that are not present in the GFF but which are
   *   present in the database will not be altered. Default is 1
   * @param $refresh
   *   Set to 1 to update existing features. New features will be added.
   *   Attributes for a feature that are not present in the GFF but which are
   *   present in the database will be removed. Default is 0
   * @param $remove
   *   Set to 1 to remove features present in the GFF file that exist in the
   *   database. Default is 0.
   * @param $use_transaction
   *   Set to 1 to use a transaction when loading the GFF. Any failure during
   *   loading will result in the rollback of any changes. Default is 1.
   * @param $target_organism_id
   *   If the GFF file contains a 'Target' attribute then the feature and the
   *   target will have an alignment created, but to find the proper target
   *   feature the target organism must also be known.  If different from the
   *   organism specified for the GFF file, then use  this argument to specify
   *   the target organism.  Only use this argument if all target sequences
   *   belong to the same species. If the targets in the GFF file belong to
   *   multiple different species then the organism must be specified using the
   *   'target_organism=genus:species' attribute in the GFF file. Default is
   *   NULL.
   * @param $target_type
   *   If the GFF file contains a 'Target' attribute then the feature and the
   *   target will have an alignment created, but to find the proper target
   *   feature the target organism must also be known.  This can be used to
   *   specify the target feature type to help with identification of the
   *   target
   *   feature.  Only use this argument if all target sequences types are the
   *   same. If the targets are of different types then the type must be
   *   specified using the 'target_type=type' attribute in the GFF file. This
   *   must be a valid Sequence Ontology (SO) term. Default is NULL
   * @param $create_target
   *   Set to 1 to create the target feature if it cannot be found in the
   *   database. Default is 0
   * @param $start_line
   *   Set this to the line in the GFF file where importing should start. This
   *   is useful for testing and debugging GFF files that may have problems and
   *   you want to start at a particular line to speed testing.  Default = 1
   * @param $landmark_type
   *   Use this argument to specify a Sequence Ontology term name for the
   *   landmark sequences in the GFF fie (e.g. 'chromosome'), if the GFF file
   *   contains a
   *   '##sequence-region' line that describes the landmark sequences. Default
   *   = ''
   * @param $alt_id_attr
   *   Sometimes lines in the GFF file are missing the required ID attribute
   *   that specifies the unique name of the feature. If so, you may specify
   *   the name of an existing attribute to use for the ID.
   * @param $create_organism
   *   The Tripal GFF loader supports the "organism" attribute. This allows
   *   features of a different organism to be aligned to the landmark sequence
   *   of another species. The format of the attribute is
   *   "organism=[genus]:[species]", where [genus] is the organism's genus and
   *   [species] is the species name. Check this box to automatically add the
   *   organism to the database if it does not already exists. Otherwise lines
   *   with an oraganism attribute where the organism is not present in the
   *   database will be skipped.
   * @param $re_mrna A
   *          regular expression to extract portions from mRNA id
   * @param $re_protein A
   *          replacement string to generate the protein id
   * @PARAM $skip_protein
   *        BOOL: Will the loader create the proteins inferred from the CDS?
   *
   * @ingroup gff3_loader
   */
  private function loadGFF3($gff_file, $organism_id, $analysis_id,
                            $add_only = 0, $update = 1, $refresh = 0, $remove = 0, $use_transaction = 1,
                            $target_organism_id = NULL, $target_type = NULL, $create_target = 0,
                            $start_line = 1, $landmark_type = '', $alt_id_attr = '', $create_organism = FALSE,
                            $re_mrna = '', $re_protein = '', $skip_protein = 0) {

    $ret = [];
    $date = getdate();

    // An array that stores CVterms that have been looked up so we don't have
    // to do the database query every time.
    $cvterm_lookup = [];

    // An array that stores Landmarks that have been looked up so we don't have
    // to do the database query every time.
    $landmark_lookup = [];

    // empty the temp tables
    $sql = "DELETE FROM {tripal_gff_temp}";
    chado_query($sql);
    $sql = "DELETE FROM {tripal_gffcds_temp}";
    chado_query($sql);
    $sql = "DELETE FROM {tripal_gffprotein_temp}";
    chado_query($sql);


    // check to see if the file is located local to Drupal
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $gff_file;
    if (!file_exists($dfile)) {
      // if not local to Drupal, the file must be someplace else, just use
      // the full path provided
      $dfile = $gff_file;
    }
    if (!file_exists($dfile)) {
      throw new Exception(t("Cannot find the file: !dfile", ['!dfile' => $dfile]));
    }

    $this->logMessage("Opening !gff_file", ['!gff_file' => $gff_file]);

    //$lines = file($dfile,FILE_SKIP_EMPTY_LINES);
    $fh = fopen($dfile, 'r');
    if (!$fh) {
      throw new Exception(t("Cannot open file: !dfile", ['!dfile' => $dfile]));
    }
    $filesize = filesize($dfile);
    $this->setTotalItems($filesize);

    // get the controlled vocaubulary that we'll be using.  The
    // default is the 'sequence' ontology
    $sql = "SELECT * FROM {cv} WHERE name = :cvname";
    $cv = chado_query($sql, [':cvname' => 'sequence'])->fetchObject();
    if (!$cv) {
      throw new Exception(t("Cannot find the 'sequence' ontology", []));
    }
    // get the organism for which this GFF3 file belongs
    $sql = "SELECT * FROM {organism} WHERE organism_id = :organism_id";
    $organism = chado_query($sql, [':organism_id' => $organism_id])->fetchObject();

    $in_fasta = 0;
    $line_num = 0;
    $num_read = 0;

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

    // If a landmark type was provided then pre-retrieve that.
    if ($landmark_type) {
      $query = [
        ':cv_id' => $cv->cv_id,
        ':name' => $landmark_type,
        ':synonym' => $landmark_type,
      ];
      $result = chado_query($sel_cvterm_sql, $query);
      $landmark_cvterm = $result->fetchObject();
      if (!$landmark_cvterm) {
        throw new Exception(t('Cannot find landmark feature type \'%landmark_type\'.', ['%landmark_type' => $landmark_type]));
      }
    }

    // iterate through each line of the GFF file
    while ($line = fgets($fh)) {
      $line_num++;
      $size = drupal_strlen($line);
      $this->addItemsHandled($size);
      $num_read += $size;

      if ($line_num < $start_line) {
        continue;
      }

      // check to see if we have FASTA section, if so then set the variable
      // to start parsing
      if (preg_match('/^##FASTA/i', $line)) {
        $this->logMessage("Parsing FASTA portion...");
        if ($remove) {
          // we're done because this is a delete operation so break out of the loop.
          break;
        }
        $this->loadFasta($fh, $interval, $num_read, $line_num, $filesize);
        continue;
      }
      // if the ##sequence-region line is present then we want to add a new feature
      if (preg_match('/^##sequence-region (.*?) (\d+) (\d+)$/i', $line, $region_matches)) {
        $rid = $region_matches[1];
        $rstart = $region_matches[2];
        $rend = $region_matches[3];
        if ($landmark_type) {
          $this->loadFeature($organism, $analysis_id, $landmark_cvterm, $rid,
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
        throw new Exception(t('Improper number of columns on line %line_num', ['%line_num' => $line_num]));
      }

      // get the column values
      $landmark = $cols[0];
      $source = $cols[1];
      $type = $cols[2];
      $start = $cols[3];
      $end = $cols[4];
      $score = $cols[5];
      $strand = $cols[6];
      $phase = $cols[7];
      $attrs = explode(";", $cols[8]);  // split by a semicolon

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
        if ($type == 'CDS') {
          $phase = '0';
        }
        else {
          $phase = '';
        }
      }
      if (array_key_exists($type, $cvterm_lookup)) {
        $cvterm = $cvterm_lookup[$type];
      }
      else {
        $result = chado_query($sel_cvterm_sql, [
          ':cv_id' => $cv->cv_id,
          ':name' => $type,
          ':synonym' => $type,
        ]);
        $cvterm = $result->fetchObject();
        $cvterm_lookup[$type] = $cvterm;
        if (!$cvterm) {
          throw new Exception(t('Cannot find feature term \'%type\' on line %line_num of the GFF file',
            ['%type' => $type, '%line_num' => $line_num]));
        }
      }

      // break apart each of the attributes
      $tags = [];
      $attr_name = '';
      $attr_uniquename = '';
      $attr_residue_info = '';
      $attr_locgroup = 0;
      $attr_fmin_partial = 'f';
      $attr_fmax_partial = 'f';
      $attr_is_obsolete = 'f';
      $attr_is_analysis = 'f';
      $attr_others = [];
      $residues = '';

      // the organism to which a feature belongs can be set in the GFF
      // file using the 'organism' attribute.  By default we
      // set the $feature_organism variable to the default organism for the landmark
      $attr_organism = '';
      $feature_organism = $organism;

      foreach ($attrs as $attr) {
        $attr = rtrim($attr);
        $attr = ltrim($attr);
        if (strcmp($attr, '') == 0) {
          continue;
        }
        if (!preg_match('/^[^\=]+\=.+$/', $attr)) {
          throw new Exception(t('Attribute is not correctly formatted on line %line_num: %attr',
            ['%line_num' => $line_num, '%attr' => $attr]));
        }

        // break apart each tag
        $tag = preg_split("/=/", $attr, 2);  // split by equals sign

        // multiple instances of an attribute are separated by commas
        $tag_name = $tag[0];
        if (!array_key_exists($tag_name, $tags)) {
          $tags[$tag_name] = [];
        }
        $tags[$tag_name] = array_merge($tags[$tag_name], explode(",", $tag[1]));  // split by comma


        // replace the URL escape codes for each tag
        for ($i = 0; $i < count($tags[$tag_name]); $i++) {
          $tags[$tag_name][$i] = urldecode($tags[$tag_name][$i]);
        }

        // get the name and ID tags
        $skip_feature = 0;  // if there is a problem with any of the attributes this variable gets set
        if (strcmp($tag_name, 'ID') == 0) {
          $attr_uniquename = urldecode($tag[1]);
        }
        elseif (strcmp($tag_name, 'Name') == 0) {
          $attr_name = urldecode($tag[1]);
        }
        elseif (strcmp($tag_name, 'organism') == 0) {
          $attr_organism = urldecode($tag[1]);
          $org_matches = [];
          if (preg_match('/^(.*?):(.*?)$/', $attr_organism, $org_matches)) {
            $values = [
              'genus' => $org_matches[1],
              'species' => $org_matches[2],
            ];
            $org = chado_select_record('organism', ["*"], $values);
            if (count($org) == 0) {
              if ($create_organism) {
                $feature_organism = (object) chado_insert_record('organism', $values);
                if (!$feature_organism) {
                  $this->logMessage("Could not add the organism, '%org', from line %line. Skipping this line.",
                    [
                      '%org' => $attr_organism,
                      '%line' => $line_num,
                    ], TRIPAL_ERROR);
                  $skip_feature = 1;
                }
              }
              else {
                $this->logMessage("The organism attribute '%org' on line %line does not exist. Skipping this line.",
                  [
                    '%org' => $attr_organism,
                    '%line' => $line_num,
                  ], TRIPAL_ERROR);
                $skip_feature = 1;
              }
            }
            else {
              // We found the organism in the database so use it.
              $feature_organism = $org[0];
            }
          }
          else {
            $this->logMessage("The organism attribute '%org' on line %line is not properly formated. It " .
              "should be of the form: organism=Genus:species.  Skipping this line.",
              ['%org' => $attr_organism, '%line' => $line_num], TRIPAL_ERROR);
            $skip_feature = 1;
          }
        }
        // Get the list of non-reserved attributes.
        elseif (strcmp($tag_name, 'Alias') != 0 and strcmp($tag_name, 'Parent') != 0 and
          strcmp($tag_name, 'Target') != 0 and strcmp($tag_name, 'Gap') != 0 and
          strcmp($tag_name, 'Derives_from') != 0 and strcmp($tag_name, 'Note') != 0 and
          strcmp($tag_name, 'Dbxref') != 0 and strcmp($tag_name, 'Ontology_term') != 0 and
          strcmp($tag_name, 'Is_circular') != 0 and strcmp($tag_name, 'target_organism') != 0 and
          strcmp($tag_name, 'target_type') != 0 and strcmp($tag_name, 'organism' != 0)) {
          foreach ($tags[$tag_name] as $value) {
            $attr_others[$tag_name][] = $value;
          }
        }
      }
      // If neither name nor uniquename are provided then generate one.
      if (!$attr_uniquename and !$attr_name) {
        // Check if an alternate ID field is suggested, if so, then use
        // that for the name.
        if (array_key_exists($alt_id_attr, $tags)) {
          $attr_uniquename = $tags[$alt_id_attr][0];
          $attr_name = $attr_uniquename;
        }
        // If the row has a parent then generate a uniquename using the parent name
        // add the date to the name in the event there are more than one child with
        // the same parent.
        elseif (array_key_exists('Parent', $tags)) {
          $attr_uniquename = $tags['Parent'][0] . "-$type-$landmark-" . $date[0] . ":" . ($fmin + 1) . ".." . $fmax;
          $attr_name = $attr_uniquename;
        }
        // Generate a unique name based on the date, type and location
        // and set the name to simply be the type.
        else {
          $attr_uniquename = $date[0] . "-$type-$landmark:" . ($fmin + 1) . ".." . $fmax;
          $attr_name = $type;
        }
      }

      // If a name is not specified then use the unique name as the name
      if (strcmp($attr_name, '') == 0) {
        $attr_name = $attr_uniquename;
      }

      // If an ID attribute is not specified then we must generate a
      // unique ID. Do this by combining the attribute name with the date
      // and line number.
      if (!$attr_uniquename) {
        $attr_uniquename = $attr_name . '-' . $date[0] . '-' . $line_num;
      }

      // Make sure the landmark sequence exists in the database.  If the user
      // has not specified a landmark type (and it's not required in the GFF
      // format) then we don't know the type of the landmark so we'll hope
      // that it's unique across all types for the organism. Only do this
      // test if the landmark and the feature are different.
      if (!$remove and !(strcmp($landmark, $attr_uniquename) == 0 or strcmp($landmark, $attr_name) == 0) and !in_array($landmark, $landmark_lookup)) {

        $select = [
          'organism_id' => $organism->organism_id,
          'uniquename' => $landmark,
        ];
        $columns = ['count(*) as num_landmarks'];
        if ($landmark_type) {
          $select['type_id'] = [
            'name' => $landmark_type,
          ];
        }
        $count = chado_select_record('feature', $columns, $select);
        if (!$count or count($count) == 0 or $count[0]->num_landmarks == 0) {
          // now look for the landmark using the name rather than uniquename.
          $select = [
            'organism_id' => $organism->organism_id,
            'name' => $landmark,
          ];
          $columns = ['count(*) as num_landmarks'];
          if ($landmark_type) {
            $select['type_id'] = [
              'name' => $landmark_type,
            ];
          }
          $count = chado_select_record('feature', $columns, $select);
          if (!$count or count($count) == 0 or $count[0]->num_landmarks == 0) {
            throw new Exception(t("The landmark '%landmark' cannot be found for this organism (%species) " .
              "Please add the landmark and then retry the import of this GFF3 " .
              "file", [
              '%landmark' => $landmark,
              '%species' => $organism->genus . " " . $organism->species,
            ]));
          }
          elseif ($count[0]->num_landmarks > 1) {
            throw new Exception(t("The landmark '%landmark' has more than one entry for this organism (%species) " .
              "Cannot continue", [
              '%landmark' => $landmark,
              '%species' => $organism->genus . " " . $organism->species,
            ]));
          }

        }
        if ($count[0]->num_landmarks > 1) {
          throw new Exception(t("The landmark '%landmark' is not unique for this organism. " .
            "The features cannot be associated", ['%landmark' => $landmark]));
        }

        // The landmark was found, remember it
        $landmark_lookup[] = $landmark;
      }
      /*
       // If the option is to remove or refresh then we want to remove
       // the feature from the database.
       if ($remove or $refresh) {
       // Next remove the feature itself.
       $sql = "DELETE FROM {feature}
       WHERE organism_id = %d and uniquename = '%s' and type_id = %d";
       $match = array(
       'organism_id' => $feature_organism->organism_id,
       'uniquename'  => $attr_uniquename,
       'type_id'     => $cvterm->cvterm_id
       );
       $result = chado_delete_record('feature', $match);
       if (!$result) {
       $this->logMessage("Cannot delete feature %attr_uniquename",
         array('%attr_uniquename' => $attr_uniquename), TRIPAL_ERROR);
       }
       $feature = 0;
       unset($result);
       }
       */
      // Add or update the feature and all properties.
      if ($update or $refresh or $add_only) {

        // Add/update the feature.
        $feature = $this->loadFeature($feature_organism, $analysis_id, $cvterm,
          $attr_uniquename, $attr_name, $residues, $attr_is_analysis,
          $attr_is_obsolete, $add_only, $score);

        if ($feature) {

          // Add a record for this feature to the tripal_gff_temp table for
          // later lookup.
          $values = [
            'feature_id' => $feature->feature_id,
            'organism_id' => $feature->organism_id,
            'type_name' => $type,
            'uniquename' => $feature->uniquename,
          ];
          // make sure this record doesn't already exist in our temp table
          $results = chado_select_record('tripal_gff_temp', ['*'], $values);

          if (count($results) == 0) {
            $result = chado_insert_record('tripal_gff_temp', $values);
            if (!$result) {
              throw new Exception(t("Cound not save record in temporary table, Cannot continue.", []));
            }
          }
          // Add/update the featureloc if the landmark and the ID are not the
          // same if they are the same then this entry in the GFF is probably
          // a landmark identifier.
          if (strcmp($landmark, $attr_uniquename) != 0) {
            $this->loadFeatureLoc($feature, $organism,
              $landmark, $fmin, $fmax, $strand, $phase, $attr_fmin_partial,
              $attr_fmax_partial, $attr_residue_info, $attr_locgroup);
          }

          // Add any aliases for this feature.
          if (array_key_exists('Alias', $tags)) {
            $this->loadAlias($feature, $tags['Alias']);
          }
          // Add any dbxrefs for this feature.
          if (array_key_exists('Dbxref', $tags)) {
            $this->loadDbxref($feature, $tags['Dbxref']);
          }
          // Add any ontology terms for this feature.
          if (array_key_exists('Ontology_term', $tags)) {
            $this->loadOntology($feature, $tags['Ontology_term']);
          }
          // Add parent relationships.
          if (array_key_exists('Parent', $tags)) {
            $this->loadParents($feature, $cvterm, $tags['Parent'],
              $feature_organism->organism_id, $strand, $phase, $fmin, $fmax);
          }

          // Add target relationships.
          if (array_key_exists('Target', $tags)) {
            $this->loadTarget($feature, $tags, $target_organism_id, $target_type, $create_target, $attr_locgroup);
          }
          // Add gap information.  This goes in simply as a property.
          if (array_key_exists('Gap', $tags)) {
            foreach ($tags['Gap'] as $value) {
              $this->loadProperty($feature, 'Gap', $value);
            }
          }
          // Add notes. This goes in simply as a property.
          if (array_key_exists('Note', $tags)) {
            foreach ($tags['Note'] as $value) {
              $this->loadProperty($feature, 'Note', $value);
            }
          }
          // Add the Derives_from relationship (e.g. polycistronic genes).
          if (array_key_exists('Derives_from', $tags)) {
            $this->loadDerivesFrom($feature, $cvterm, $tags['Derives_from'][0],
              $feature_organism, $fmin, $fmax);
          }
          // Add in the GFF3_source dbxref so that GBrowse can find the feature
          // using the source column.
          $source_ref = ['GFF_source:' . $source];
          $this->loadDbxref($feature, $source_ref);

          // Add any additional attributes.
          if ($attr_others) {
            foreach ($attr_others as $tag_name => $values) {
              foreach ($values as $value) {
                $this->loadProperty($feature, $tag_name, $value);
              }
            }
          }

        }
      }
    }

    // Do some last bit of processing.
    if (!$remove) {

      // First, add any protein sequences if needed.
      $sql = "SELECT feature_id FROM {tripal_gffcds_temp} LIMIT 1 OFFSET 1";
      $has_cds = chado_query($sql)->fetchField();
      if ($has_cds) {
        $this->logMessage("\nAdding protein sequences if CDS exist and no proteins in GFF...");
        $sql = "
          SELECT F.feature_id, F.name, F.uniquename, TGCT.strand,
            CVT.cvterm_id, CVT.name as feature_type,
            min(TGCT.fmin) as fmin, max(TGCT.fmax) as fmax,
            TGPT.feature_id as protein_id, TGPT.fmin as protein_fmin,
            TGPT.fmax as protein_fmax, FLM.uniquename as landmark
          FROM {tripal_gffcds_temp} TGCT
            INNER JOIN {feature} F on F.feature_id = TGCT.parent_id
            INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
            INNER JOIN {featureloc} L on F.feature_id = L.feature_id
            INNER JOIN {feature} FLM on L.srcfeature_id = FLM.feature_id
            LEFT JOIN {tripal_gffprotein_temp} TGPT on TGPT.parent_id = F.feature_id
          GROUP BY F.feature_id, F.name, F.uniquename, CVT.cvterm_id, CVT.name,
            TGPT.feature_id, TGPT.fmin, TGPT.fmax, TGCT.strand, FLM.uniquename
        ";
        $results = chado_query($sql);
        $protein_cvterm = chado_get_cvterm([
          'name' => 'polypeptide',
          'cv_id' => [
            'name' => 'sequence',
          ],
        ]);
        while ($result = $results->fetchObject()) {
          // If a protein exists with this same parent then don't add a new
          // protein.
          if (!$result->protein_id) {
            // Get details about this protein
            if ($re_mrna and $re_protein) {
              // We use a regex to generate protein name from mRNA name
              $uname = preg_replace("/$re_mrna/", $re_protein, $result->uniquename);
              $name = $result->name;
            }
            else {
              // No regex, use the default '-protein' suffix
              $uname = $result->uniquename . '-protein';
              $name = $result->name;
            }
            $values = [
              'parent_id' => $result->feature_id,
              'fmin' => $result->fmin,
            ];
            $min_phase = chado_select_record('tripal_gffcds_temp', ['phase'], $values);
            $values = [
              'parent_id' => $result->feature_id,
              'fmax' => $result->fmax,
            ];
            $max_phase = chado_select_record('tripal_gffcds_temp', ['phase'], $values);

            $pfmin = $result->fmin;
            $pfmax = $result->fmax;
            if ($result->strand == '-1') {
              $pfmax -= $max_phase[0]->phase;
            }
            else {
              $pfmin += $min_phase[0]->phase;
            }

            if ($skip_protein == 0) {
              // Add the new protein record.
              $feature = $this->loadFeature($organism, $analysis_id,
                $protein_cvterm, $uname, $name, '', 'f', 'f', 1, 0);
              // Add the derives_from relationship.
              $cvterm = chado_get_cvterm(['cvterm_id' => $result->cvterm_id]);
              $this->loadDerivesFrom($feature, $cvterm,
                $result->uniquename, $organism, $pfmin, $pfmax);
              // Add the featureloc record. Set the start of the protein to
              // be the start of the coding sequence minus the phase.
              $this->loadFeatureLoc($feature, $organism, $result->landmark,
                $pfmin, $pfmax, $result->strand, '', 'f', 'f', '', 0);
            }
          }
        }
      }

      $this->logMessage("Setting ranks of children...");

      // Get features in a relationship that are also children of an alignment.
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

      // Build and prepare the SQL for selecting the children relationship.
      $sel_gffchildren_sql = "
      SELECT DISTINCT FR.feature_relationship_id, FL.fmin, FR.rank
      FROM {feature_relationship} FR
        INNER JOIN {featureloc} FL on FL.feature_id = FR.subject_id
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = FR.type_id
      WHERE FR.object_id = :feature_id AND CVT.name = 'part_of'
      ORDER BY FL.fmin ASC
    ";

      // Now set the rank of any parent/child relationships.  The order is based
      // on the fmin.  The start rank is 1.  This allows features with other
      // relationships to be '0' (the default), and doesn't interfer with the
      // ordering defined here.
      $num_recs = $parents->rowCount();
      $i = 1;

      while ($parent = $parents->fetchObject()) {

        // get the children
        $result = chado_query($sel_gffchildren_sql, [':feature_id' => $parent->feature_id]);

        // build an array of the children
        $children = [];
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
          $match = ['feature_relationship_id' => $child->feature_relationship_id];
          $values = ['rank' => $rank];
          chado_update_record('feature_relationship', $match, $values);
          $rank--;
        }
        // now set the rank correctly. The rank should start at 0.
        $rank = 0;
        foreach ($children as $child) {
          $match = ['feature_relationship_id' => $child->feature_relationship_id];
          $values = ['rank' => $rank];
          chado_update_record('feature_relationship', $match, $values);
          $rank++;
        }
        $i++;
      }
    }


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
  private function loadDerivesFrom($feature, $cvterm, $object,
                                   $organism, $fmin, $fmax) {

    $type = $cvterm->name;

    // First look for the object feature in the temp table to get it's type.
    $values = [
      'organism_id' => $organism->organism_id,
      'uniquename' => $object,
    ];
    $result = chado_select_record('tripal_gff_temp', ['type_name'], $values);
    $type_id = NULL;
    if (count($result) > 0) {
      $otype = chado_get_cvterm([
        'name' => $result[0]->type_name,
        'cv_id' => [
          'name' => 'sequence',
        ],
      ]);
      if ($otype) {
        $type_id = $otype->cvterm_id;
      }
    }

    // If the object wasn't in the temp table then look for it in the
    // feature table and get it's type.
    if (!$type_id) {
      $result = chado_select_record('feature', ['type_id'], $values);
      if (count($result) > 1) {
        $this->logMessage("Cannot find feature type for, '!subject' , in 'derives_from' relationship. Multiple matching features exist with this uniquename.",
          ['!subject' => $object], TRIPAL_WARNING);
        return;
      }
      else {
        if (count($result) == 0) {
          $this->logMessage("Cannot find feature type for, '!subject' , in 'derives_from' relationship.",
            ['!subject' => $object], TRIPAL_WARNING);
          return '';
        }
        else {
          $type_id = $result->type_id;
        }
      }
    }

    // Get the object feature.
    $match = [
      'organism_id' => $organism->organism_id,
      'uniquename' => $object,
      'type_id' => $type_id,
    ];
    $ofeature = chado_select_record('feature', ['feature_id'], $match);
    if (count($ofeature) == 0) {
      $this->logMessage("Could not add 'Derives_from' relationship " .
        "for %uniquename and %subject.  Subject feature, '%subject', " .
        "cannot be found.", [
        '%uniquename' => $feature->uniquename,
        '%subject' => $subject,
      ], TRIPAL_ERROR);
      return;
    }

    // If this feature is a protein then add it to the tripal_gffprotein_temp.
    if ($type == 'protein' or $type == 'polypeptide') {
      $values = [
        'feature_id' => $feature->feature_id,
        'parent_id' => $ofeature[0]->feature_id,
        'fmin' => $fmin,
        'fmax' => $fmax,
      ];
      $result = chado_insert_record('tripal_gffprotein_temp', $values);
      if (!$result) {
        throw new Exception(t("Cound not save record in temporary protein table, Cannot continue.", []));
      }
    }

    // Now check to see if the relationship already exists. If it does
    // then just return.
    $values = [
      'object_id' => $ofeature[0]->feature_id,
      'subject_id' => $feature->feature_id,
      'type_id' => [
        'cv_id' => [
          'name' => 'sequence',
        ],
        'name' => 'derives_from',
      ],
      'rank' => 0,
    ];
    $rel = chado_select_record('feature_relationship', ['*'], $values);
    if (count($rel) > 0) {
      return;
    }

    // finally insert the relationship if it doesn't exist
    $ret = chado_insert_record('feature_relationship', $values);
    if (!$ret) {
      $this->logMessage("Could not add 'Derives_from' relationship for :uniquename and :subject.",
        [
          ':uniquename' => $feature->uniquename,
          ':subject' => $subject,
        ], TRIPAL_WARNING);
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
  private function loadParents($feature, $cvterm, $parents,
                               $organism_id, $strand, $phase, $fmin, $fmax) {

    $uname = $feature->uniquename;
    $type = $cvterm->name;
    $rel_type = 'part_of';

    // Prepare these SQL statements that will be used repeatedly.
    $cvterm_sql = "
    SELECT CVT.cvterm_id
    FROM {cvterm} CVT
      INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
      LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
    WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)
  ";

    // Iterate through the parents in the list.
    foreach ($parents as $parent) {
      // Get the parent cvterm.
      $values = [
        'organism_id' => $organism_id,
        'uniquename' => $parent,
      ];
      $result = chado_select_record('tripal_gff_temp', ['type_name'], $values);
      if (count($result) == 0) {
        $this->logMessage("Cannot find parent: %parent.", ['%parent' => $parent], TRIPAL_WARNING);
        return '';
      }
      $parent_type = $result[0]->type_name;

      // try to find the parent
      $parentcvterm = chado_query($cvterm_sql, [
        ':cvname' => 'sequence',
        ':name' => $parent_type,
        ':synonym' => $parent_type,
      ])->fetchObject();
      $relcvterm = chado_query($cvterm_sql, [
        ':cvname' => 'sequence',
        ':name' => $rel_type,
        ':synonym' => $rel_type,
      ])->fetchObject();
      if (!$relcvterm) {
        throw new Exception(t("Cannot find the term, 'part_of', from the sequence ontology. This term is used for associating parent and children features. Please check that the ontology is fully imported."));
      }
      $values = [
        'organism_id' => $organism_id,
        'uniquename' => $parent,
        'type_id' => $parentcvterm->cvterm_id,
      ];
      $result = chado_select_record('feature', ['feature_id'], $values);
      $parent_feature = $result[0];

      // if the parent exists then add the relationship otherwise print error and skip
      if ($parent_feature) {

        // check to see if the relationship already exists
        $values = [
          'object_id' => $parent_feature->feature_id,
          'subject_id' => $feature->feature_id,
          'type_id' => $relcvterm->cvterm_id,
        ];
        $rel = chado_select_record('feature_relationship', ['*'], $values);

        if (count($rel) > 0) {
        }
        else {
          // the relationship doesn't already exist, so add it.
          $values = [
            'subject_id' => $feature->feature_id,
            'object_id' => $parent_feature->feature_id,
            'type_id' => $relcvterm->cvterm_id,
          ];
          $result = chado_insert_record('feature_relationship', $values);
          if (!$result) {
            $this->logMessage("Failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type).",
              [], TRIPAL_WARNING);
          }
        }

        // If this feature is a CDS and now that we know the parent we can
        // add it to the tripal_gffcds_temp table for later lookup.
        if ($type == 'CDS') {
          $values = [
            'feature_id' => $feature->feature_id,
            'parent_id' => $parent_feature->feature_id,
            'fmin' => $fmin,
            'fmax' => $fmax,
            'strand' => $strand,
          ];
          if (isset($phase)) {
            $values['phase'] = $phase;
          }
          $result = chado_insert_record('tripal_gffcds_temp', $values);
          if (!$result) {
            throw new Exception(t("Cound not save record in temporary CDS table, Cannot continue.", []));
            exit;
          }
        }
      }
      else {
        $this->logMessage("Cannot establish relationship '$uname' ($type) $rel_type '$parent' ($parent_type): Cannot find the parent.",
          [], TRIPAL_WARNING);
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
  private function loadDbxref($feature, $dbxrefs) {

    // iterate through each of the dbxrefs
    foreach ($dbxrefs as $dbxref) {

      // get the database name from the reference.  If it doesn't exist then create one.
      $ref = explode(":", $dbxref);
      $dbname = trim($ref[0]);
      $accession = trim($ref[1]);

      // first look for the database name if it doesn't exist then create one.
      // first check for the fully qualified URI (e.g. DB:<dbname>. If that
      // can't be found then look for the name as is.  If it still can't be found
      // the create the database
      $values = ['name' => "DB:$dbname"];
      $db = chado_select_record('db', ['db_id'], $values);
      if (count($db) == 0) {
        $values = ['name' => "$dbname"];
        $db = chado_select_record('db', ['db_id'], $values);
      }
      if (count($db) == 0) {
        $values = [
          'name' => $dbname,
          'description' => 'Added automatically by the GFF loader',
        ];
        $success = chado_insert_record('db', $values);
        if ($success) {
          $values = ['name' => "$dbname"];
          $db = chado_select_record('db', ['db_id'], $values);
        }
        else {
          $this->logMessage("Cannot find or add the database $dbname.", [], TRIPAL_WARNING);
          return 0;
        }
      }
      $db = $db[0];

      // now check to see if the accession exists
      $values = [
        'accession' => $accession,
        'db_id' => $db->db_id,
      ];
      $dbxref = chado_select_record('dbxref', ['dbxref_id'], $values);

      // if the accession doesn't exist then we want to add it
      if (sizeof($dbxref) == 0) {
        $values = [
          'db_id' => $db->db_id,
          'accession' => $accession,
          'version' => '',
        ];
        $ret = chado_insert_record('dbxref', $values);
        $values = [
          'accession' => $accession,
          'db_id' => $db->db_id,
        ];
        $dbxref = chado_select_record('dbxref', ['dbxref_id'], $values);
      }
      $dbxref = $dbxref[0];

      // check to see if this feature dbxref already exists
      $values = [
        'dbxref_id' => $dbxref->dbxref_id,
        'feature_id' => $feature->feature_id,
      ];
      $fdbx = chado_select_record('feature_dbxref', ['feature_dbxref_id'], $values);

      // now associate this feature with the database reference if it doesn't
      // already exist
      if (sizeof($fdbx) == 0) {
        $values = [
          'dbxref_id' => $dbxref->dbxref_id,
          'feature_id' => $feature->feature_id,
        ];
        $success = chado_insert_record('feature_dbxref', $values);
        if (!$success) {
          $this->logMessage("Failed to insert Dbxref: $dbname:$accession.", [], TRIPAL_WARNING);
          return 0;
        }
      }
    }
    return 1;
  }

  /**
   * Load the cvterms for a feature.
   *
   * Assumes there is a dbxref.accession matching a cvterm.name
   *
   * @param $feature
   * @param $dbxrefs
   *
   * @ingroup gff3_loader
   */
  private function loadOntology($feature, $dbxrefs) {

    // iterate through each of the dbxrefs
    foreach ($dbxrefs as $dbxref) {

      // get the database name from the reference.  If it doesn't exist then create one.
      $ref = explode(":", $dbxref);
      $dbname = trim($ref[0]);
      $accession = trim($ref[1]);

      // first look for the database name
      $db = chado_select_record('db', ['db_id'], ['name' => "DB:$dbname"]);
      if (sizeof($db) == 0) {
        // now look for the name without the 'DB:' prefix.
        $db = chado_select_record('db', ['db_id'], ['name' => "$dbname"]);
        if (sizeof($db) == 0) {
          $this->logMessage("Database, $dbname, is not present. Cannot associate term: $dbname:$accession.", [], TRIPAL_WARNING);
          continue;
        }
      }
      $db = $db[0];

      // now check to see if the accession exists
      $dbxref = chado_select_record('dbxref', ['dbxref_id'],
        ['accession' => $accession, 'db_id' => $db->db_id]);
      if (sizeof($dbxref) == 0) {
        $this->logMessage("Accession, $accession is missing for reference: $dbname:$accession.", [], TRIPAL_WARNING);
        continue;
      }
      $dbxref = $dbxref[0];

      // now check to see if the cvterm exists
      $cvterm = chado_select_record('cvterm', ['cvterm_id'], [
        'dbxref_id' => $dbxref->dbxref_id,
      ]);
      // if it doesn't exist in the cvterm table, look for an alternate id
      if (sizeof($cvterm) == 0) {
        $cvterm = chado_select_record('cvterm_dbxref', ['cvterm_id'], [
          'dbxref_id' => $dbxref->dbxref_id,
        ]);
        if (sizeof($cvterm) == 0) {
          $this->logMessage("CV Term is missing for reference: $dbname:$accession.", [], TRIPAL_WARNING);
          continue;
        }
      }
      $cvterm = $cvterm[0];


      // check to see if this feature cvterm already exists
      $fcvt = chado_select_record('feature_cvterm', ['feature_cvterm_id'],
        [
          'cvterm_id' => $cvterm->cvterm_id,
          'feature_id' => $feature->feature_id,
        ]);

      // now associate this feature with the cvterm if it doesn't already exist
      if (sizeof($fcvt) == 0) {
        $values = [
          'cvterm_id' => $cvterm->cvterm_id,
          'feature_id' => $feature->feature_id,
          'pub_id' => [
            'uniquename' => 'null',
          ],
        ];
        $success = chado_insert_record('feature_cvterm', $values);

        if (!$success) {
          $this->logMessage("Failed to insert ontology term: $dbname:$accession.", [], TRIPAL_WARNING);
          continue;
        }
      }
    }
  }

  /**
   * Load any aliases for a feature
   *
   * @param $feature
   * @param $aliases
   *
   * @ingroup gff3_loader
   */
  private function loadAlias($feature, $aliases) {

    // make sure we have a 'synonym_type' vocabulary
    $select = ['name' => 'synonym_type'];
    $results = chado_select_record('cv', ['*'], $select);

    if (count($results) == 0) {
      // insert the 'synonym_type' vocabulary
      $values = [
        'name' => 'synonym_type',
        'definition' => 'vocabulary for synonym types',
      ];
      $success = chado_insert_record('cv', $values);
      if (!$success) {
        $this->logMessage("Failed to add the synonyms type vocabulary.", [], TRIPAL_WARNING);
        return 0;
      }
      // now that we've added the cv we need to get the record
      $results = chado_select_record('cv', ['*'], $select);
      if (count($results) > 0) {
        $syncv = $results[0];
      }
    }
    else {
      $syncv = $results[0];
    }

    // get the 'exact' cvterm, which is the type of synonym we're adding
    $select = [
      'name' => 'exact',
      'cv_id' => [
        'name' => 'synonym_type',
      ],
    ];
    $result = chado_select_record('cvterm', ['*'], $select);
    if (count($result) == 0) {
      $term = [
        'name' => 'exact',
        'id' => "synonym_type:exact",
        'definition' => '',
        'is_obsolete' => 0,
        'cv_name' => $syncv->name,
        'is_relationship' => FALSE,
      ];
      $syntype = chado_insert_cvterm($term, ['update_existing' => TRUE]);
      if (!$syntype) {
        $this->logMessage("Cannot add synonym type: internal:$type.", [], TRIPAL_WARNING);
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
      $select = [
        'name' => $alias,
        'type_id' => $syntype->cvterm_id,
      ];
      $result = chado_select_record('synonym', ['*'], $select);
      if (count($result) == 0) {
        $values = [
          'name' => $alias,
          'type_id' => $syntype->cvterm_id,
          'synonym_sgml' => '',
        ];
        $success = chado_insert_record('synonym', $values);
        if (!$success) {
          $this->logMessage("Cannot add alias $alias to synonym table.", [], TRIPAL_WARNING);
          return 0;
        }
        $result = chado_select_record('synonym', ['*'], $select);
        $synonym = $result[0];
      }
      else {
        $synonym = $result[0];
      }

      // check to see if we have a NULL publication in the pub table.  If not,
      // then add one.
      $select = ['uniquename' => 'null'];
      $result = chado_select_record('pub', ['*'], $select);
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
          $this->logMessage("Cannot prepare statement 'ins_pub_uniquename_typeid.", [], TRIPAL_WARNING);
          return 0;
        }

        // insert the null pub
        $result = chado_query($pub_sql, [
          ':uname' => 'null',
          ':type_id' => 'null',
        ])->fetchObject();
        if (!$result) {
          $this->logMessage("Cannot add null publication needed for setup of alias.", [], TRIPAL_WARNING);
          return 0;
        }
        $result = chado_select_record('pub', ['*'], $select);
        $pub = $result[0];
      }
      else {
        $pub = $result[0];
      }

      // check to see if the synonym exists in the feature_synonym table
      // if not, then add it.
      $values = [
        'synonym_id' => $synonym->synonym_id,
        'feature_id' => $feature->feature_id,
        'pub_id' => $pub->pub_id,
      ];
      $columns = ['feature_synonym_id'];
      $result = chado_select_record('feature_synonym', $columns, $values);
      if (count($result) == 0) {
        $values = [
          'synonym_id' => $synonym->synonym_id,
          'feature_id' => $feature->feature_id,
          'pub_id' => $pub->pub_id,
        ];
        $success = chado_insert_record('feature_synonym', $values);

        if (!$success) {
          $this->logMessage("Cannot add alias $alias to feature synonym table.", [], TRIPAL_WARNING);
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
  private function loadFeature($organism, $analysis_id, $cvterm, $uniquename,
                               $name, $residues, $is_analysis = 'f', $is_obsolete = 'f', $add_only, $score) {

    // Check to see if the feature already exists.
    $feature = NULL;
    $fselect = [
      'organism_id' => $organism->organism_id,
      'uniquename' => $uniquename,
      'type_id' => $cvterm->cvterm_id,
    ];
    $columns = [
      'feature_id',
      'name',
      'uniquename',
      'seqlen',
      'organism_id',
      'type_id',
    ];
    $result = chado_select_record('feature', $columns, $fselect);
    if (count($result) > 0) {
      $feature = $result[0];
    }

    if (strcmp($is_obsolete, 'f') == 0 or $is_obsolete == 0) {
      $is_obsolete = 'FALSE';
    }
    if (strcmp($is_obsolete, 't') == 0 or $is_obsolete == 1) {
      $is_obsolete = 'TRUE';
    }
    if (strcmp($is_analysis, 'f') == 0 or $is_analysis == 0) {
      $is_analysis = 'FALSE';
    }
    if (strcmp($is_analysis, 't') == 0 or $is_analysis == 1) {
      $is_analysis = 'TRUE';
    }

    // Insert the feature if it does not exist otherwise perform an update.
    if (!$feature) {
      $values = [
        'organism_id' => $organism->organism_id,
        'name' => $name,
        'uniquename' => $uniquename,
        'md5checksum' => md5($residues),
        'type_id' => $cvterm->cvterm_id,
        'is_analysis' => $is_analysis,
        'is_obsolete' => $is_obsolete,
      ];
      $feature = (object) chado_insert_record('feature', $values);
      if (!$feature) {
        $this->logMessage("Failed to insert feature '$uniquename' ($cvterm->name).", [], TRIPAL_WARNING);
        return 0;
      }
    }
    elseif (!$add_only) {
      $values = [
        'name' => $name,
        'md5checksum' => md5($residues),
        'is_analysis' => $is_analysis,
        'is_obsolete' => $is_obsolete,
      ];
      $match = [
        'organism_id' => $organism->organism_id,
        'uniquename' => $uniquename,
        'type_id' => $cvterm->cvterm_id,
      ];
      $result = chado_update_record('feature', $match, $values);
      if (!$result) {
        $this->logMessage("Failed to update feature '$uniquename' ($cvterm->name).", [], TRIPAL_WARNING);
        return 0;
      }
    }
    else {
      // The feature exists and we don't want to update it so return
      // a value of 0.  This will stop all downstream property additions
      return $feature;
    }

    // Add the analysisfeature entry to the analysisfeature table if
    // it doesn't already exist.
    $af_values = [
      'analysis_id' => $analysis_id,
      'feature_id' => $feature->feature_id,
    ];
    $afeature = chado_select_record('analysisfeature', ['analysisfeature_id'], $af_values);
    if (count($afeature) == 0) {
      // if a score is available then set that to be the significance field
      if (strcmp($score, '.') != 0) {
        $af_values['significance'] = $score;
      }
      if (!chado_insert_record('analysisfeature', $af_values)) {
        $this->logMessage("Could not add analysisfeature record: $analysis_id, $feature->feature_id.", [], TRIPAL_WARNING);
      }
    }
    else {
      // if a score is available then set that to be the significance field
      $new_vals = [];
      if (strcmp($score, '.') != 0) {
        $new_vals['significance'] = $score;
      }
      else {
        $new_vals['significance'] = '__NULL__';
      }
      if (!$add_only) {
        $ret = chado_update_record('analysisfeature', $af_values, $new_vals);
        if (!$ret) {
          $this->logMessage("Could not update analysisfeature record: $analysis_id, $feature->feature_id.", [], TRIPAL_WARNING);
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
  private function loadFeatureLoc($feature, $organism, $landmark, $fmin,
                                  $fmax, $strand, $phase, $is_fmin_partial, $is_fmax_partial, $residue_info, $locgroup,
                                  $landmark_type_id = '', $landmark_organism_id = '', $create_landmark = 0,
                                  $landmark_is_target = 0) {

    $select = [
      'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
      'uniquename' => $landmark,
    ];
    if ($landmark_type_id) {
      $select['type_id'] = $landmark_type_id;
    }
    $results = chado_select_record('feature', ['feature_id'], $select);

    $srcfeature = '';
    if (count($results) == 0) {
      // so we couldn't find the landmark using the uniquename. Let's try the 'name'.
      // if we return only a single result then we can proceed. Otherwise give an
      $select = [
        'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
        'name' => $landmark,
      ];
      if ($landmark_type_id) {
        $select['type_id'] = $landmark_type_id;
      }
      $results = chado_select_record('feature', ['feature_id'], $select);
      if (count($results) == 0) {
        // if the landmark is the target feature in a matched alignment then try one more time to
        // find it by querying any feature with the same uniquename. If we find one then use it.
        if ($landmark_is_target) {
          $select = ['uniquename' => $landmark];
          $results = chado_select_record('feature', ['feature_id'], $select);
          if (count($results) == 1) {
            $srcfeature = $results[0];
          }
        }

        if (!$srcfeature) {
          // we couldn't find the landmark feature, so if the user has requested we create it then do so
          // but only if we have a type id
          if ($create_landmark and $landmark_type_id) {
            $values = [
              'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
              'name' => $landmark,
              'uniquename' => $landmark,
              'type_id' => $landmark_type_id,
            ];
            $results = chado_insert_record('feature', $values);
            if (!$results) {
              $this->logMessage("Cannot find landmark feature: '%landmark', nor could it be inserted.",
                ['%landmark' => $landmark], TRIPAL_WARNING);
              return 0;
            }
            $srcfeature = new stdClass();
            $srcfeature->feature_id = $results['feature_id'];
          }
          else {
            $this->logMessage("Cannot find unique landmark feature: '%landmark'.",
              ['%landmark' => $landmark], TRIPAL_WARNING);
            return 0;
          }
        }
      }
      elseif (count($results) > 1) {
        $this->logMessage("multiple landmarks exist with the name: '%landmark'.  Cannot " .
          "resolve which one to use. Cannot add the feature location record.",
          ['%landmark' => $landmark], TRIPAL_WARNING);
        return 0;
      }
      else {
        $srcfeature = $results[0];
      }
    }
    elseif (count($results) > 1) {
      $this->logMessage("multiple landmarks exist with the name: '%landmark'.  Cannot " .
        "resolve which one to use. Cannot add the feature location record.",
        ['%landmark' => $landmark, TRIPAL_WARNING]);
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
    $select = ['feature_id' => $feature->feature_id];
    $options = [
      'order_by' => [
        'rank' => 'ASC',
      ],
    ];

    $locrecs = chado_select_record('featureloc', ['*'], $select, $options);

    foreach ($locrecs as $featureloc) {
      // it is possible for the featureloc->srcfeature_id to be NULL. This can happen if the srcfeature
      // is not known (according to chado table field descriptions).  If it's null then just skip this entry
      if (!$featureloc->srcfeature_id) {
        continue;
      }
      $select = ['feature_id' => $featureloc->srcfeature_id];
      $columns = ['feature_id', 'name'];
      $locsfeature = chado_select_record('feature', $columns, $select);

      // the source feature name and at least the fmin and fmax must be the same
      // for an update of the featureloc, otherwise we'll insert a new record.
      if (strcmp($locsfeature[0]->name, $landmark) == 0 and
        ($featureloc->fmin == $fmin or $featureloc->fmax == $fmax)) {
        $match = ['featureloc_id' => $featureloc->featureloc_id];
        $values = [];
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
      if (strcmp($is_fmin_partial, 'f') == 0 or !$is_fmin_partial) {
        $is_fmin_partial = 'FALSE';
      }
      elseif (strcmp($is_fmin_partial, 't') == 0 or $is_fmin_partial = 1) {
        $is_fmin_partial = 'TRUE';
      }
      if (strcmp($is_fmax_partial, 'f') == 0 or !$is_fmax_partial) {
        $is_fmax_partial = 'FALSE';
      }
      elseif (strcmp($is_fmax_partial, 't') == 0 or $is_fmax_partial = 1) {
        $is_fmax_partial = 'TRUE';
      }
      $values = [
        'feature_id' => $feature->feature_id,
        'srcfeature_id' => $srcfeature->feature_id,
        'fmin' => $fmin,
        'is_fmin_partial' => $is_fmin_partial,
        'fmax' => $fmax,
        'is_fmax_partial' => $is_fmax_partial,
        'strand' => $strand,
        'residue_info' => $residue_info,
        'locgroup' => $locgroup,
        'rank' => $rank,
      ];
      if ($phase) {
        $values['phase'] = $phase;
      }
      $success = chado_insert_record('featureloc', $values);
      if (!$success) {
        throw new Exception("Failed to insert featureloc.");
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
  private function loadProperty($feature, $property, $value) {

    // First make sure the cvterm exists.  if not, then add it.
    $select = [
      'name' => $property,
      'cv_id' => [
        'name' => 'feature_property',
      ],
    ];
    $result = chado_select_record('cvterm', ['*'], $select);

    // If we don't have a property like this already, then add it otherwise,
    // just return.
    if (count($result) == 0) {
      $term = [
        'id' => "local:$property",
        'name' => $property,
        'is_obsolete' => 0,
        'cv_name' => 'feature_property',
        'db_name' => 'local',
        'is_relationship' => FALSE,
      ];
      $cvterm = (object) chado_insert_cvterm($term, ['update_existing' => FALSE]);
      if (!$cvterm) {
        $this->logMessage("Cannot add cvterm, $property.", [], TRIPAL_WARNING);
        return 0;
      }
    }
    else {
      $cvterm = $result[0];
    }


    // Check to see if the property already exists for this feature
    // if it does but the value is unique then increment the rank and add it.
    // if the value is not unique then don't add it.
    $add = 1;
    $rank = 0;
    $select = [
      'feature_id' => $feature->feature_id,
      'type_id' => $cvterm->cvterm_id,
    ];
    $options = [
      'order_by' => [
        'rank' => 'ASC',
      ],
    ];
    $results = chado_select_record('featureprop', ['*'], $select, $options);
    foreach ($results as $prop) {
      if (strcmp($prop->value, $value) == 0) {
        $add = NULL; // don't add it, it already exists
      }
      $rank = $prop->rank + 1;
    }

    // add the property if we pass the check above
    if ($add) {
      $values = [
        'feature_id' => $feature->feature_id,
        'type_id' => $cvterm->cvterm_id,
        'value' => $value,
        'rank' => $rank,
      ];
      $result = chado_insert_record('featureprop', $values);
      if (!$result) {
        $this->logMessage("cannot add featureprop, $property.", [], TRIPAL_WARNING);
      }
    }
  }

  /**
   * Load the FASTA sequences at the bottom of a GFF3 file
   *
   * @param $fh
   * @param $interval
   * @param $num_read
   * @param $line_num
   * @param $filesize
   *
   * @ingroup gff3_loader
   */
  private function loadFasta($fh, $interval, &$num_read, &$line_num, $filesize) {
    $this->logMessage("Loading FASTA sequences...");
    $residues = '';
    $id = NULL;

    // iterate through the remaining lines of the file
    while ($line = fgets($fh)) {

      $line_num++;
      $size = drupal_strlen($line);
      $this->addItemsHandled($size);
      $num_read += $size;
      $line = trim($line);

      // if we encounter a definition line then get the name, uniquename,
      // accession and relationship subject from the definition line
      if (preg_match('/^>/', $line)) {

        // if we are beginning a new sequence then save to the database the last one we just finished.
        if ($id) {
          $values = ['uniquename' => $id];
          $result = chado_select_record('tripal_gff_temp', ['*'], $values);
          if (count($result) == 0) {
            $this->logMessage('Cannot find feature to assign FASTA sequence: %uname.',
              ['%uname' => $id], TRIPAL_WARNING);
          }
          else {
            // if we have a feature then add the residues
            $feature = $result[0];
            $values = [
              'residues' => $residues,
              'seqlen' => strlen($residues),
            ];
            $match = ['feature_id' => $feature->feature_id];
            chado_update_record('feature', $match, $values);
          }
        }

        // get the feature ID for this ID from the tripal_gff_temp table. It
        // should be the name up to the first space
        $id = preg_replace('/^>([^\s]+).*$/', '\1', $line);
        $residues = '';
      }
      else {
        $residues .= trim($line);
      }
    }

    // add in the last sequence
    $values = ['uniquename' => $id];
    $result = chado_select_record('tripal_gff_temp', ['*'], $values);
    if (count($result) == 0) {
      $this->logMessage('Cannot find feature to assign FASTA sequence: %uname.',
        ['%uname' => $id], TRIPAL_WARNING);
    }
    else {
      // if we have a feature then add the residues
      $feature = $result[0];
      $values = [
        'residues' => $residues,
        'seqlen' => strlen($residues),
      ];
      $match = ['feature_id' => $feature->feature_id];
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
  private function loadTarget($feature, $tags, $target_organism_id, $target_type, $create_target, $attr_locgroup) {
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
          $values = [
            'genus' => $matches[1],
            'species' => $matches[2],
          ];
          $torganism = chado_select_record('organism', ['organism_id'], $values);
          if (count($torganism) == 1) {
            $t_organism_id = $torganism[0]->organism_id;
          }
          else {
            $this->logMessage("Cannot find organism for target %target.",
              ['%target' => $gff_target_organism], TRIPAL_WARNING);
            $t_organism_id = '';
          }
        }
        else {
          $this->logMessage("The target_organism attribute is improperly formatted: %target. " .
            "It should be target_organism=genus:species.",
            ['%target' => $gff_target_organism], TRIPAL_WARNING);
          $t_organism_id = '';
        }
      }

      // default the target type to be the value passed into the function, but if the GFF file
      // species the target type then use that instead
      $t_type_id = '';
      if ($target_type) {
        $values = [
          'name' => $target_type,
          'cv_id' => [
            'name' => 'sequence',
          ],
        ];
        $type = chado_select_record('cvterm', ['cvterm_id'], $values);
        if (count($type) == 1) {
          $t_type_id = $type[0]->cvterm_id;
        }
        else {
          throw new Exception(t("The target type does not exist in the sequence ontology: %type. ",
            ['%type' => $target_type]));
        }
      }
      if ($gff_target_type) {
        $values = [
          'name' => $gff_target_type,
          'cv_id' => [
            'name' => 'sequence',
          ],
        ];

        // get the cvterm_id for the target type
        $type = chado_select_record('cvterm', ['cvterm_id'], $values);
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
          $synonym = chado_query($sql, [':synonym' => $gff_target_type])->fetchObject();
          if ($synonym) {
            $t_type_id = $synonym->cvterm_id;
          }
          else {
            $this->logMessage("The target_type attribute does not exist in the sequence ontology: %type.",
              ['%type' => $gff_target_type], TRIPAL_WARNING);
            $t_type_id = '';
          }
        }
      }

      // we want to add a featureloc record that uses the target feature as the srcfeature (landmark)
      // and the landmark as the feature.
      $this->loadFeatureLoc($feature, $organism, $target_feature, $target_fmin,
        $target_fmax, $target_strand, $phase, $attr_fmin_partial, $attr_fmax_partial, $attr_residue_info,
        $attr_locgroup, $t_type_id, $t_organism_id, $create_target, TRUE);
    }
    // the target attribute is not correctly formatted
    else {
      $this->logMessage("Could not add 'Target' alignment as it is improperly formatted:  '%target'",
        ['%target' => $tags['Target'][0]], TRIPAL_ERROR);
    }
  }
}
