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
   * The lines from the ##sequence-region at the top of the GFF
   */
  private $seq_region_headers = [];

  /**
   * The path to the GFF3 file.
   */
  private $gff_file = NULL;

  /**
   * The file handle for the GFF3 file.
   */
  private $gff_file_h = NULL;

  /**
   * The organism ID for this GFF file.
   */
  private $organism_id = NULL;

  /**
   * The organism ChadoRecord object that corresponds to the $organism_id value.
   */
  private $organism = NULL;


  /**
   * An array of organism records for quick lookup.
   */
  private $organism_lookup = NULL;

  /**
   * The analysis ID for this GFF file
   */
  private $analysis_id = NULL;

  /**
   * The analysis ChadoRecord object that corresponds to the $analysis_id value.
   */
  private $analysis = NULL;

  /**
   * A flag indicating if only new items should be added (no updates)
   */
  private $add_only = NULL;

  /**
   * A flag indicting if only existing items should be updated.
   */
  private $update = TRUE;

  /**
   * If the GFF file contains a 'Target' attribute then the feature and the
   * target will have an alignment created, but to find the proper target
   * feature the target organism must also be known.  If different from the
   * organism specified for the GFF file, then use  this argument to specify
   * the target organism.  Only use this argument if all target sequences
   * belong to the same species. If the targets in the GFF file belong to
   * multiple different species then the organism must be specified using the
   * 'target_organism=genus:species' attribute in the GFF file. Default is
   * NULL.
   */
  private $target_organism_id = NULL;

  /**
   * If the GFF file contains a 'Target' attribute then the feature and the
   * target will have an alignment created, but to find the proper target
   * feature the target organism must also be known.  This can be used to
   * specify the target feature type to help with identification of the
   * target feature.  Only use this argument if all target sequences types are
   * the same. If the targets are of different types then the type must be
   * specified using the 'target_type=type' attribute in the GFF file. This
   * must be a valid Sequence Ontology (SO) term. Default is NULL
   */
  private $target_type = NULL;

  /**
   * A flag indicating if the target feature should be created. If FALSE
   * then it should already exist.
   */
  private $create_target = FALSE;

  /**
   * Set this to the line in the GFF file where importing should start. This
   * is useful for testing and debugging GFF files that may have problems and
   * you want to start at a particular line to speed testing.  Default = 1
   */
  private $start_line = 1;

  /**
   * During parsing of the GFF file this keeps track of the current line
   * number.
   */
  private $current_line = 0;

  /**
   * A Sequence Ontology term name for the landmark sequences in the GFF
   * file (e.g. 'chromosome'), if the GFF file contains a '##sequence-region'
   * line that describes the landmark sequences. Default = ''
   */
  private $landmark_type = '';

  /**
   * The ChadoRecord object for the landmark type cvterm.
   */
  private $landmark_cvterm = NULL;


  /**
   * Regular expression to pull out the mRNA name.
   */
  private $re_mrna = '';

  /**
   * Regular expression to pull out the protein name.
   */
  private $re_protein = '';

  /**
   * A flag that indicates if a protein record should be created.
   * @var integer
   */
  private $skip_protein = 0;

  /**
   * Sometimes lines in the GFF file are missing the required ID attribute
   * that specifies the unique name of the feature. If so, you may specify
   * the name of an existing attribute to use for the ID.
   */
  private $alt_id_attr = '';

  /**
   * The Tripal GFF loader supports the "organism" attribute. This allows
   * features of a different organism to be aligned to the landmark sequence
   * of another species. The format of the attribute is
   * "organism=[genus]:[species]", where [genus] is the organism's genus and
   * [species] is the species name. Check this box to automatically add the
   * organism to the database if it does not already exists. Otherwise lines
   * with an oraganism attribute where the organism is not present in the
   * database will be skipped.
   */
  private $create_organism = FALSE;

  /**
   * Holds mapping of DB names to DB ids.
   */
  private $db_lookup = [];

  /**
   * Holds a mapping of Dbxref names to ids.
   */
  private $dbxref_lookup = [];

  /**
   * An array that stores CVterms that have been looked up so we don't have
   * to do the database query every time.
   */
  private $feature_cvterm_lookup = [];


  /**
   * Holds a mapping of cvterms to their aliases that are used in the
   * GFF3 file.
   */
  private $feature_cvterm_aliases = [];

  /**
   * An array that stores CVterms that have been looked up so we don't have
   * to do the database query every time.
   */
  private $featureprop_cvterm_lookup = [];

  /**
   * Holds the CV term for the "exact" synonym.
   */
  private $exact_syn = NULL;

  /**
   * Holds the object for the null publication record.
   */
  private $null_pub = NULL;

  /**
   * An array the stores existing features in the database for the organism
   * and feature types in the database.  This is used for quick lookups
   * to prevent violating the unique constraints on a bulk insert.
   *
   * The feature_lookup is indexed first by organism_id, then by type name and
   * then by uniquename.
   */
  private $feature_lookup = [];

  /**
   * The list of features from the GFF3 file.  Each element is an
   * associative array of the columns from the GFF3 file, with the attribute
   * field being an associative array of key/value pairs.
   */
  private $features = [];

  /**
   * A mapping of features to their parents.
   */
  private $relationships = [
    'Parent' => [],
    'Child' => [],
  ];


  /**
   * An associatiave array containing the pointers to the FASTA sequences
   * in the GFF file. We don't want to load these into memory as they
   * may be too big!
   */
  private $residue_index = [];

  /**
   * An array that stores landmarks objects.  Landmarks should be inserted
   * first if they don't already exist.
   */
  private $landmarks = [];


  /**
   * A controlled vocabulary ChadoRecord object. This is the CV that will be
   * used to for feature properties.
   */
  private $feature_prop_cv = NULL;


  /**
   * A controlled vocabulary ChadoRecord object. This is the CV that will be
   * used to for feature properties.
   */
  private $feature_cv = NULL;


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
    $this->gff_file = $this->arguments['files'][0]['file_path'];

    // Set the private member variables of this class using the loader inputs.
    $this->organism_id = $arguments['organism_id'];
    $this->analysis_id = $arguments['analysis_id'];
    $this->add_only = $arguments['add_only'];
    $this->update = $arguments['update'];
    $this->target_organism_id = $arguments['target_organism_id'];
    $this->target_type = $arguments['target_type'];
    $this->create_target = $arguments['create_target'];
    $this->start_line = $arguments['line_number'];
    $this->landmark_type = $arguments['landmark_type'];
    $this->alt_id_attr = $arguments['alt_id_attr'];
    $this->create_organism = $arguments['create_organism'];
    $this->re_mrna = $arguments['re_mrna'];
    $this->re_protein = $arguments['re_protein'];
    $this->skip_protein = $arguments['skip_protein'];

    // Check to see if the file is located local to Drupal
    $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $this->gff_file;
    if (!file_exists($dfile)) {
      $this->gff_file = $dfile;
    }
    // If the file is not local to Drupal check if it exists on the system.
    else if (!file_exists($this->gff_file)) {
      throw new Exception(t("Cannot find the file: !file", ['!file' => $this->gff_file]));
    }

    // Open the GFF3 file.
    $this->logMessage("Opening !gff_file", ['!gff_file' => $this->gff_file]);
    $this->gff_file_h = fopen($this->gff_file, 'r');
    if (!$this->gff_file_h) {
      throw new Exception(t("Cannot open file: !file", ['!file' => $this->gff_file]));
    }

    // Get the feature property CV object
    $this->feature_prop_cv = new ChadoRecord('cv');
    $this->feature_prop_cv->setValues(['name' => 'feature_property']);
    $num_found = $this->feature_prop_cv->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the 'feature_property' ontology'", []));
    }

    // Get the sequence CV object.
    $this->feature_cv = new ChadoRecord('cv');
    $this->feature_cv->setValues(['name' => 'sequence']);
    $num_found = $this->feature_cv->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the 'sequence' ontology'", []));
    }

    // Get the organism object.
    $this->organism = new ChadoRecord('organism');
    $this->organism->setValues(['organism_id' => $this->organism_id]);
    $num_found = $this->organism->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the specified organism for this GFF3 file."));
    }

    // Get the analysis object.
    $this->analysis = new ChadoRecord('analysis');
    $this->analysis->setValues(['analysis_id' => $this->analysis_id]);
    $num_found = $this->analysis->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the specified organism for this GFF3 file."));
    }

    // If a landmark type was provided then get that object.
    if ($this->landmark_type) {
      $this->landmark_cvterm = new ChadoRecord('cvterm');
      $this->landmark_cvterm->setValues([
          'cv_id' => $this->feature_cv->getValue('cv_id'),
          'name' => $this->landmark_type,
      ]);
      $num_found = $this->landmark_cvterm->find();
      if ($num_found == 0) {
        throw new Exception(t('Cannot find landmark feature type \'%landmark_type\'.', ['%landmark_type' => $this->landmark_type]));
      }
    }

    // Make sure we have the synonym records and null publication ready to go.
    $this->prepSynonms();
    $this->prepNullPub();

    // Load the GFF3.
    $this->logMessage("Step 1: Preloading GFF3 file...                       ");
    $this->parseGFF3();

    $this->logMessage("Step 2: Load landmarks sequences...                   ");
    $this->loadLandmarks();

    $this->logMessage("Step 3: Loading features...                           ");
    $this->loadFeatures($this->features);

    $this->logMessage("Step 4: Loading feature locations...                  ");
    $this->loadFeatureLocs();

    $this->logMessage("Step 5: Loading features properties...                ");
    $this->loadProperties();

    $this->logMessage("Step 6: Loading features synonyms (aliases)...        ");
    $this->loadAliases();

    $this->logMessage("Step 7: Loading features cross references...          ");
    $this->loadDbxrefs();

  }

  /**
   * Cleans up the temporary tables.
   *
   * These tables are used to temporarily store features from the GFF file.
   * This way the GFF entries can be out of order when loading.
   */
  private function prepTempTables() {

    // empty the temp tables
    $sql = "DELETE FROM {tripal_gff_temp}";
    chado_query($sql);
    $sql = "DELETE FROM {tripal_gffcds_temp}";
    chado_query($sql);
    $sql = "DELETE FROM {tripal_gffprotein_temp}";
    chado_query($sql);
  }

  /**
   * Makes sure Chado is ready with the necessary synonym type records.
   */
  private function prepSynonms() {
    // make sure we have a 'synonym_type' vocabulary
    $select = ['name' => 'synonym_type'];
    $results = chado_select_record('cv', ['*'], $select);

    if (count($results) == 0) {
      // insert the 'synonym_type' vocabulary
      $values = [
          'name' => 'synonym_type',
          'definition' => 'vocabulary for synonym types',
      ];
      $success = chado_insert_record('cv', $values, array(
          'skip_validation' => TRUE,
      ));
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
    $this->exact_syn = $syntype;
  }

  /**
   * Makes sure there is a null publication in the database.
   */
  private function prepNullPub(){

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
    $this->null_pub = $pub;
  }

  /**
   * Makes sure Chado is ready with the necessary Dbxref records.
   */
  private function prepDbxrefs() {
    $sql = "
      SELECT db_id
      FROM {db}
      WHERE name = :dbname";

    foreach ($this->db_lookup as $dbname => $value) {
      $result = chado_query($sql, array(
        ':dbname' => $dbname,
      ));

      $db = $result->fetchObject() ?? NULL;
      if ($db) {
        $db = chado_get_db(array('db_id' => $db->db_id));
      }
      else {
        $db = chado_insert_db(array(
          'name' => $dbname,
        ));
      }

      $this->db_lookup[$dbname] = $db->db_id;
    }

    $sql = "
      SELECT dbxref_id
      FROM {dbxref}
      WHERE db_id = :db_id and accession = :accession";

    foreach ($this->dbxref_lookup as $index => $info) {
      $result = chado_query($sql, array(
        ':db_id' => $this->db_lookup[$info['db']],
        ':accession' => $info['accession'],
      ));

      $dbx = $result->fetchObject() ?? NULL;
      if ($dbx) {
        $dbx = chado_get_dbxref(array('dbxref_id' => $dbx->dbxref_id));
      }
      else {
        $dbx = chado_insert_dbxref(array(
          'db_id' => $this->db_lookup[$info['db']],
          'accession' => $info['accession'],
        ));
      }

      $this->dbxref_lookup[$index] = $dbx->dbxref_id;
    }
  }

  /**
   * Parses the current line of the GFF3 file for a feature.
   *
   * @return array
   *  An associative array containing the 9 elements othe GFF3 file. The
   *  9th element is an associative array of the attributes.
   */
  private function parseFeature($line) {
    $date = getdate();

    // get the columns
    $cols = explode("\t", $line);
    if (sizeof($cols) != 9) {
      throw new Exception(t('Improper number of columns on line %line_num: %line', ['%line_num' => $this->current_line, '%line' => $line]));
    }

    $ret = [
      'line' => $this->current_line,
      'landmark' => $cols[0],
      'source' => $cols[1],
      'type' => $cols[2],
      'start' => $cols[3],
      'end' => $cols[4],
      'score' => $cols[5],
      'strand' => $cols[6],
      'phase' => $cols[7],
      'attrs' => [],
    ];

    // Ready the start and stop for chado.  Chado expects these positions
    // to be zero-based, so we substract 1 from the fmin. Also, in case
    // they are backwards, put them in the right order.
    $fmin = $ret['start'] - 1;
    $fmax = $ret['end'];
    if ($ret['end'] < $ret['start']) {
      $fmin = $ret['end'] - 1;
      $fmax = $ret['start'];
    }
    $ret['start'] = $fmin;
    $ret['stop'] = $fmax;


    // Format the strand for chado
    if (strcmp($ret['strand'], '.') == 0) {
      $ret['strand'] = 0;
    }
    elseif (strcmp($ret['strand'], '+') == 0) {
      $ret['strand'] = 1;
    }
    elseif (strcmp($ret['strand'], '-') == 0) {
      $ret['strand'] = -1;
    }
    if (strcmp($ret['phase'], '.') == 0) {
      if (strtolower($ret['type']) == 'cds') {
        $ret['phase'] = '0';
      }
      else {
        $ret['phase'] = '';
      }
    }

    $tags = [];
    $attr_name = '';
    $attr_uniquename = '';
    $attrs = explode(";", $cols[8]);
    $attr_organism = $this->organism;
    $attr_parent = '';
    $attr_others = [];
    $attr_aliases = [];
    $attr_dbxref = [];
    foreach ($attrs as $attr) {
      $attr = rtrim($attr);
      $attr = ltrim($attr);
      if (strcmp($attr, '') == 0) {
        continue;
      }
      if (!preg_match('/^[^\=]+\=.+$/', $attr)) {
        throw new Exception(t('Attribute is not correctly formatted on line %line_num: %attr',
            ['%line_num' => $this->current_line, '%attr' => $attr]));
      }

      // Break apart each attribute into key/value pairs.
      $tag = preg_split("/=/", $attr, 2);

      // Multiple values of an attribute are separated by commas
      $tag_name = $tag[0];
      if (!array_key_exists($tag_name, $tags)) {
        $tags[$tag_name] = [];
      }
      $tags[$tag_name] = array_merge($tags[$tag_name], explode(",", $tag[1]));

      // Replace the URL escape codes for each tag
      for ($i = 0; $i < count($tags[$tag_name]); $i++) {
        $tags[$tag_name][$i] = urldecode($tags[$tag_name][$i]);
      }

      if (strcmp($tag_name, 'organism') == 0) {
        $attr_organism = $this->getOrganism(urldecode($tag[1]));
      }
      elseif (strcmp($tag_name, 'Alias') == 0) {
        $attr_aliases = array_merge($attr_aliases, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Parent') == 0) {
        $attr_parent = urldecode($tag[1]);
      }
      elseif (strcmp($tag_name, 'Dbxref') == 0) {
        $attr_dbxref = array_merge($attr_dbxref, $tags[$tag_name]);
      }
      // Get the list of non-reserved attributes.
      elseif (strcmp($tag_name, 'Name') !=0 and strcmp($tag_name, 'ID') !=0 and
              strcmp($tag_name, 'Alias') != 0 and strcmp($tag_name, 'Parent') != 0 and
              strcmp($tag_name, 'Target') != 0 and strcmp($tag_name, 'Gap') != 0 and
              strcmp($tag_name, 'Derives_from') != 0 and strcmp($tag_name, 'Note') != 0 and
              strcmp($tag_name, 'Dbxref') != 0 and strcmp($tag_name, 'Ontology_term') != 0 and
              strcmp($tag_name, 'Is_circular') != 0 and strcmp($tag_name, 'target_organism') != 0 and
              strcmp($tag_name, 'target_type') != 0 and strcmp($tag_name, 'organism' != 0)) {
        foreach ($tags[$tag_name] as $value) {
          if (!array_key_exists($tag_name, $attr_others)) {
            $attr_others[$tag_name] = [];
          }
          $attr_others[$tag_name][] = $value;
        }
      }
    }

    // If neither name nor uniquename are provided then generate one.
    $names = $this->getFeatureName($tags, $ret['type'], $ret['landmark'], $fmin, $fmax);
    $attr_uniquename = $names['uniquename'];
    $attr_name = $names['name'];
    $ret['name'] = $attr_name;
    $ret['uniquename'] = $attr_uniquename;
    $ret['synonyms'] = $attr_aliases;
    $ret['dbxrefs'] = [];
    foreach ($attr_dbxref as $key => $dbx) {
      $parts = explode(':', $dbx);
      if (count($parts) != 2) {
        throw new Exception(t('Dbxrefs must be of the format: "Dbxref=<db name>:<accession>". The Dbxref %dbx on line %line_num does not satisfy this format.',
            ['%line_num' => $this->current_line, '%dbx' => $dbx]));
      }
      $ret['dbxrefs']["{$parts[0]}:{$parts[1]}"] = array(
        'db' => $parts[0],
        'accession' => $parts[1],
      );
    }
    $ret['dbxrefs']["GFF_source:{$ret['source']}"] = array(
      'db' => 'GFF_source',
      'accession' => $ret['source'],
    );

    // Now add all of the attributes into the return array.
    foreach ($tags as $key => $value) {
      $ret['attrs'][$key] = $value;
    }

    $ret['organism_id'] = $attr_organism->getValue('organism_id');
    $ret['properties'] = $attr_others;
    if ($attr_parent) {
      $ret['Parent'] = $attr_parent;
    }

    return $ret;
  }

  /**
   * Indexes the FASTA section of the file for quick lookup.
   */
  private function indexFASTA() {

    // Iterate through the remaining lines of the file
    while ($line = fgets($this->gff_file_h)) {

      $this->current_line++;
      $this->addItemsHandled(drupal_strlen($line));

      // Get the ID and the current file pointer and store that for later.
      if (preg_match('/^>/', $line)) {
        $id = preg_replace('/^>([^\s]+).*$/', '\1', $line);
        $this->residue_index[$id] = ftell($this->gff_file_h);
      }
    }
  }

  /**
   * Loads into the database any landmark sequences.
   *
   * @param $line
   *   The line from the GFF file that is the ##sequence-region comment.
   */
  private function loadHeaderLandmark($line) {

    $region_matches = [];
    if (preg_match('/^##sequence-region\s+(\w*?)\s+(\d+)\s+(\d+)$/i', $line, $region_matches)) {
      $rid = $region_matches[1];
      $landmark = $this->getLandmark($rid, NULL, TRUE);
      if (!$landmark) {
        $rstart = $region_matches[2];
        $rend = $region_matches[3];
        if (!$this->landmark_type) {
          throw new Exception(t('The landmark, !landmark, cannot be added becuase no landmark type was provided. Please redo the importer job and specify a landmark type.',
              ['!landmark' => $rid]));
        }
        $this->logMessage('Adding a new landmark feature: !landmark', ['!landmark' => $rid]);
        $landmark = $this->loadFeature($this->organism, $this->analysis, $this->landmark_cvterm, $rid,
              $rid, '', 'f', 'f', 1, 0);
        $this->landmark[$rid] = [
          'uniquename' => $landmark->getValue('uniquename'),
          'name' => $landmark->getValue('name'),
          'type' => $this->landmark_cvterm->getValue('name'),
          'feature_id' => $landmark->getValue('feature_id'),
          'organism_id' => $landmark->getValue('organism_id'),
        ];
      }
    }
  }

  /**
   *
   */
  private function parseGFF3() {

    $filesize = filesize($this->gff_file);
    $this->setTotalItems($filesize);


    // Holds a unique list of cvterms for later lookup.
    $feature_cvterms = [];
    $featureprop_cvterms = [];

    while ($line = fgets($this->gff_file_h)) {
      $this->current_line++;
      $this->addItemsHandled(drupal_strlen($line));

      $line = trim($line);

      if ($this->current_line < $this->start_line) {
        continue;
      }

      // If we're in the FASTA file we're at the end of the features so return.
      if (preg_match('/^##FASTA/i', $line)) {
        $this->indexFASTA();
        continue;
      }

      // if at the ##sequence-region line handle it.
      $matches = [];
      if (preg_match('/^##sequence-region\s+(\w*?)\s+(\d+)\s+(\d+)$/i', $line, $matches)) {
        $this->seq_region_headers[$matches[1]] = $line;
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

      // Parse this feature from this line of the GFF3 file.
      $gff_feature = $this->parseFeature($line);

      // Add the landmark if it doesn't exist in the landmark list.
      if (!array_key_exists($gff_feature['landmark'], $this->landmarks)) {
        $this->landmarks[$gff_feature['landmark']] = FALSE;
      }

      // Store this feature in the landmark list or the feature list.
      if ($gff_feature['uniquename'] == $gff_feature['landmark']) {
        $this->landmarks[$gff_feature['uniquename']] = $gff_feature;
      }
      else {
        $this->features[$gff_feature['uniquename']] = $gff_feature;
      }

      // Store any parent/child relationships
      if (array_key_exists('Parent', $gff_feature)) {

        // Add the parent relationship
        if (!array_key_exists($gff_feature['Parent'], $this->relationships['Parent'])) {
          $this->relationships['Parent'][$gff_feature['Parent']] = [];
        }
        if (!array_key_exists($gff_feature['type'], $this->relationships['Parent'][$gff_feature['Parent']])) {
          $this->relationships['Parent'][$gff_feature['Parent']][$gff_feature['type']] = [];
        }
        $this->relationships['Parent'][$gff_feature['Parent']][$gff_feature['type']][$gff_feature['start']] = $gff_feature['uniquename'];

        // Add the child relationship
        $this->relationships['Child'][$gff_feature['uniquename']] = $gff_feature['Parent'];
      }

      // Organize DBs for faster acces later on.
      foreach ($gff_feature['dbxrefs'] as $index => $info) {
        if (!array_key_exists($info['db'], $this->db_lookup)) {
          $this->db_lookup[$info['db']] = FALSE;
        }

        if (!array_key_exists($index, $this->dbxref_lookup)) {
          $this->dbxref_lookup[$index] = $info;
        }
      }

      // Organize the CVterms for faster access later on.
      if (!array_key_exists($gff_feature['type'], $feature_cvterms)) {
        $feature_cvterms[$gff_feature['type']] = 0;
      }
      $feature_cvterms[$gff_feature['type']]++;


      // Organize the feature property types for faster access later on.
      foreach ($gff_feature['properties'] as $prop_name => $value) {
        if (!array_key_exists($prop_name, $featureprop_cvterms)) {
          $featureprop_cvterms[$prop_name] = 0;
        }
        $featureprop_cvterms[$prop_name]++;
      }
    }

    $this->prepDbxrefs();

    // Iterate through the feature type terms and get a chado object for each.
    $feature_cvterm_ids = [];
    foreach ($feature_cvterms as $name => $counts) {
      $cvterm_id = $this->getCvtermID($name, $this->feature_cv->getValue('cv_id'), FALSE);
      $feature_cvterm_ids[] = $cvterm_id;
    }

    // Iterate through the featureprop type terms and get a cvterm_id for
    // each. If it doesn't exist then add a new record.
    foreach ($featureprop_cvterms as $name => $counts) {
      $cvterm_id = $this->getCvtermID($name, $this->feature_prop_cv->getValue('cv_id'), TRUE);
      if (!$cvterm_id) {
        $term = [
          'id' => "local:$name",
          'name' => $name,
          'is_obsolete' => 0,
          'cv_name' => $this->feature_prop_cv->getValue('name'),
          'db_name' => 'local',
          'is_relationship' => FALSE,
        ];
        $cvterm = (object) chado_insert_cvterm($term, ['update_existing' => FALSE]);
        if (!$cvterm) {
          $this->logMessage("Cannot add cvterm, $name.", [], TRIPAL_WARNING);
          return 0;
        }
        $this->featureprop_cvterm_lookup[$cvterm->name] = $cvterm->cvterm_id;
      }
    }

    // Now, get a list of features for this organism and the given types
    // we will use this list to do an "in-memory" lookup to make sure we
    // are not violating the unique contraint of the feature table
    // prior to inserting the features.
    $sql = "
      SELECT F.uniquename, CVT.name as type
      FROM {feature} F
        INNER JOIN {cvterm} CVT on F.type_id = CVT.cvterm_id
      WHERE organism_id = :organism_id and type_id IN (:types)
    ";
    $result = chado_query($sql, [
        ':organism_id' => $this->organism_id,
        ':types' => $feature_cvterm_ids,
    ]);
    while ($feature = $result->fetchObject()) {
      $this->feature_lookup[$feature->organism_id][$feature->type][$feature->uniquename] = TRUE;
    }
  }

  /**
   * Imports the landmark features into Chado.
   */
  private function loadLandmarks() {

    $new_landmarks = [];
    foreach ($this->landmarks as $uniquename => $feature) {
      // If the landmark does not have an entry in the GFF lines, try to
      // find it in the heade (i.e. ##sequence-region section). If it
      // exists then create it.
      if ($feature === FALSE) {
        if (array_key_exists($uniquename, $this->seq_region_headers)) {
          $this->loadHeaderLandmark($this->seq_region_headers[$uniquename]);
          continue;
        }
        else {
          throw new Exception(t('The landmark (reference) sequence, !landmark, is not in the database and not specified in the GFF3 file. Please pre-load the landmark sequences or edit the GFF3 file to include them.',
              ['!landmark' => $uniquename]));
        }
      }
      else {
        $new_landmarks[$uniquename] = $feature;
      }
    }
    $this->loadFeatures($new_landmarks);
  }

  /**
   * Indicates if the feature is already loaded in the database.
   */
  private function doesFeatureAlreadyExist(&$feature) {

    if (array_key_exists($feature['type'], $this->feature_cvterm_aliases)) {
      $feature['type'] = $this->feature_cvterm_aliases[$feature['type']];
    }
    if (array_key_exists($feature->organism_id, $this->feature_lookup) and
        array_key_exists($feature['type'], $this->feature_lookup[$feature->organism_id]) and
        array_key_exists($feature['uniquename'], $this->feature_lookup[$feature->organism_id][$feature['type']])){
      return TRUE;
    }
    return FALSE;
  }

  /**
   *
   * @param unknown $feature
   * @throws Exception
   */
  private function ensureFeatureIsLoaded($feature) {

    // If this feature doesn't have a feature_id then someting is wrong.
    if (!array_key_exists('feature_id', $feature)) {
      throw new Exception(t('The feature, !feature, is in the GFF but somehow was not added to the database.',
          ['!feature' => $uniquename . " (" . $feature['name'] . ") at line " . $feature['line'] . '.']));
    }
  }

  /**
   * Imports the feature records into Chado.
   */
  private function loadFeatures($features) {
    $batch_size = 1000;
    $num_features = count(array_keys($features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Get the current max feature id prior to inserting the batch so
    // we can retrieve the feature_ids of what was inserted afterwards.
    $result = chado_query("SELECT max(feature_id) AS max_id FROM {feature}");
    $start_id = $result->fetchField();

    // If the feature table is empty, we need to set the start to 1.
    if (!$start_id) {
      $start_id = 1;
    }

    $init_sql = "
      INSERT INTO {feature}
        (uniquename, name, type_id, organism_id, residues, md5checksum,
         seqlen, is_analysis, is_obsolete)
      VALUES\n";
    $i = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    $batch_features = [];
    foreach ($features as $uniquename => $feature) {
      $batch_features[$uniquename] = $feature;

      // Only do an insert if this feature doesn't already exist in the databse.
      if (!$this->doesFeatureAlreadyExist($feature)) {

        $i++;
        $residues = $this->getResidues($feature, FALSE);
        $sql .= "(:uniquename_$i, :name_$i, :type_id_$i, :organism_id_$i, :residues_$i, " .
               " :md5checksum_$i, :seqlen_$i, FALSE, FALSE),\n";
        $args[":uniquename_$i"] = $feature['uniquename'];
        $args[":name_$i"] = $feature['name'];
        $args[":type_id_$i"] = $this->feature_cvterm_lookup[$feature['type']];
        $args[":organism_id_$i"] = $feature['organism_id'];
        $args[":residues_$i"] = $residues;
        $args[":md5checksum_$i"] = $residues ? md5($residues) : '';
        $args[":seqlen_$i"] = strlen($residues);

        // If we've reached the size of the batch then let's do the insert.
        if ($i == $batch_size) {

          // Insert the batch.
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $last_id = chado_query($sql, $args, ['return' => Database::RETURN_INSERT_ID]);
          $this->assignFeatureIDs($batch_features);
          $this->setItemsHandled($batch_num);
          $batch_num++;

          // Now reset all of the varables for the next batch.
          $sql = '';
          $i = 0;
          $args = [];
          $start_id = $last_id;
          $batch_features = [];
        }
      }
    }

    // Insert any remaining batch items
    if ($i > 0) {
      $sql = rtrim($sql, ",\n");
      $sql = $init_sql . $sql;
      $last_id = chado_query($sql, $args, ['return' => Database::RETURN_INSERT_ID]);
      $this->setItemsHandled($batch_num);
    }

    if (!empty($batch_features)) {
      $this->assignFeatureIDs($batch_features);
    }
  }

  /**
   * Adds the feature IDs to features within a range if feature_ids.
   *
   * The start and last IDs should corresopnd to Id's surrounding
   * a batch insert of features.
   */
  private function assignFeatureIDs($batch_features) {
    // Get the feature Ids for the batch sequences
    $sql = "
      SELECT feature_id
      FROM {feature} F
      WHERE uniquename = :uniquename and organism_id = :organism_id and type_id = :type_id
    ";

    foreach ($batch_features as $uniquename => $feature) {
      $result = chado_query($sql, array(
        ':uniquename' => $feature['uniquename'],
        ':organism_id' => $feature['organism_id'],
        ':type_id' => $this->feature_cvterm_lookup[$feature['type']],
      ))->fetchObject();
      if (array_key_exists($uniquename, $this->features)) {
        $this->features[$uniquename]['feature_id'] = $result->feature_id;
      }
      if (array_key_exists($uniquename, $this->landmarks)) {
        $this->landmarks[$uniquename]['feature_id'] = $result->feature_id;
      }
    }
  }

  /**
   *
   */
  private function loadProperties(){
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {featureprop}
        (feature_id, type_id, value, rank)
      VALUES\n";
    $i = 0;
    $j = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $feature) {

      // Only do an insert if this feature doesn't already exist in the databse.
      if (!$this->doesFeatureAlreadyExist($feature)) {
        $i++;

        $this->ensureFeatureIsLoaded($feature);

        // Iterate through all of the properties of this feature.
        foreach ($feature['properties'] as $prop_name => $values) {
          foreach ($values as $rank => $value) {
            $j++;
            $sql .= "(:feature_id_$j, :type_id_$j, :value_$j, :rank_$j),\n";
            $args[":feature_id_$j"] = $feature['feature_id'];
            $args[":type_id_$j"] = $this->featureprop_cvterm_lookup[$prop_name];
            $args[":value_$j"] = $value;
            $args[":rank_$j"] = $rank;
          }
        }
        // If we've reached the size of the batch then let's do the insert.
        if ($i == $batch_size) {

          // Insert the batch.
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          $last_id = chado_query($sql, $args, ['return' => Database::RETURN_INSERT_ID]);
          $this->setItemsHandled($batch_num);
          $batch_num++;

          // Now reset all of the varables for the next batch.
          $sql = '';
          $i = 0;
          $j = 0;
          $args = [];
        }
      }
    }

    // Add any remaining batch items
    if ($i > 0) {
      $sql = rtrim($sql, ",\n");
      $sql = $init_sql . $sql;
      $last_id = chado_query($sql, $args, ['return' => Database::RETURN_INSERT_ID]);
      $this->setItemsHandled($batch_num);
    }
  }

  /**
   *
   */
  private function loadDbxrefs() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Don't need to use placeholders for this insert since we are only using
    // integers.
    $count = 0;
    $batch_num = 0;
    $batch_pairs = [];
    $init_fdbx_sql = "INSERT INTO {feature_dbxref} (feature_id, dbxref_id) VALUES \n";
    $check_fdbx_sql = "SELECT feature_dbxref_id FROM {feature_dbxref} WHERE feature_id = :feature_id and dbxref_id = :dbxref_id";
    foreach ($this->features as $uniquename => $feature) {
      $count++;

      $this->ensureFeatureIsLoaded($feature);

      foreach ($feature['dbxrefs'] as $index => $info) {
        $feature_id = $feature['feature_id'];
        $dbx_id = $this->dbxref_lookup[$index];

        // Check that this feature_dbxref is not already in the database.
        $result = chado_query($check_fdbx_sql, array(
          ':feature_id' => $feature_id,
          ':dbxref_id' => $dbx_id,
        ))->fetchObject() ?? NULL;

        if (!$result) {
          $batch_pairs[] = "($feature_id, $dbx_id)";
        }
      }

      if ($count == $batch_size) {
        $batch_num++;
        if (count($batch_pairs) > 0) {
          // Perform the actual insertion.
          $fdbx_sql = $init_fdbx_sql . implode(', ', $batch_pairs);
          $last_id = chado_query($fdbx_sql, array(), array('return' => Database::RETURN_INSERT_ID));
        }

        $this->setItemsHandled($batch_num);

        $count = 0;
        $batch_pairs = [];
      }
    }

    if ($count > 0) {
      $batch_num++;
      if (count($batch_pairs) > 0) {
        // Perform the actual insertion.
        $fdbx_sql = $init_fdbx_sql . implode(', ', $batch_pairs);
        $last_id = chado_query($fdbx_sql, array(), array('return' => Database::RETURN_INSERT_ID));
      }

      $this->setItemsHandled($batch_num);
    }
  }

  /**
   *
   */
  private function loadFeatureLocs() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {featureloc}
        (srcfeature_id, feature_id, fmin, fmax, strand, phase, rank)
      VALUES\n";
    $i = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $uniquename => $feature) {

      // Only do an insert if this feature doesn't already exist in the databse.
      if (!$this->doesFeatureAlreadyExist($feature)) {
        $i++;

        $this->ensureFeatureIsLoaded($feature);

        // Get the rank of this feature by ordering all of the other
        // subfeatures of the same type that share the same parent.
        // Order them by the fmin and use the index of this feature as the
        // rank.
        $rank = 0;
        if (array_key_exists('Parent', $feature)) {
          $coords = array_keys($this->relationships['Parent'][$feature['Parent']][$feature['type']]);
          sort($coords);
          $rank = array_search($feature['start'], $coords);
        }

        $sql .= "(:srcfeature_id_$i, :feature_id_$i, :fmin_$i, :fmax_$i," .
                " :strand_$i, :phase_$i, :rank_$i),\n";
        $args[":srcfeature_id_$i"] = $this->landmarks[$feature['landmark']]['feature_id'];
        $args[":feature_id_$i"] = $feature['feature_id'];
        $args[":fmin_$i"] = $feature['start'];
        $args[":fmax_$i"] = $feature['stop'];
        $args[":strand_$i"] = $feature['strand'];
        $args[":phase_$i"] = $feature['phase'] ? $feature['phase'] : NULL;
        $args[":rank_$i"] = $rank;


        // If we've reached the size of the batch then let's do the insert.
        if ($i == $batch_size) {

          // Insert the batch.
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          //$last_id = chado_query($sql, $args, ['return' => Database::RETURN_INSERT_ID]);
          $this->setItemsHandled($batch_num);
          $batch_num++;

          // Now reset all of the varables for the next batch.
          $sql = '';
          $i = 0;
          $args = [];
        }
      }
    }

    // Add any remaining batch items
    if ($i > 0) {
      $sql = rtrim($sql, ",\n");
      $sql = $init_sql . $sql;
      $last_id = chado_query($sql, $args, ['return' => Database::RETURN_INSERT_ID]);
      $this->setItemsHandled($batch_num);
    }
  }

  /**
   *
   */
  private function loadAliases(){
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_syn_sql = "INSERT INTO {synonym} (name, type_id, synonym_sgml) VALUES \n";
    $init_fsyn_sql = "INSERT INTO {feature_synonym} (synonym_id, feature_id, pub_id) VALUES \n";
    $i = 0;
    $batch_num = 1;
    $syn_sql = '';
    $syn_args = [];
    $fsyn_sql = '';
    $fsyn_args = [];
    $batch_synonyms = [];
    $batch_featuresyn = [];
    foreach ($this->features as $uniquename => $feature) {

      // Only do an insert if this feature doesn't already exist in the databse.
      if (!$this->doesFeatureAlreadyExist($feature)) {

        $i++;

        $this->ensureFeatureIsLoaded($feature);

        // Get all of the synonyms for this batch.
        foreach ($feature['synonyms'] as $index => $synonym) {
          $batch_synonyms[] = $synonym;
          $batch_featuresyn[] = [$synonym, $feature['feature_id']];
        }

        // If we've reached the size of the batch then let's do the insert.
        if ($i == $batch_size) {

          if (count($batch_synonyms) > 0) {

            // First get the synonym_ids for those already in the database.
            $syns_avail_sql = "SELECT synonym_id, name FROM {synonym} WHERE type_id = :type_id and name IN (:names)";
            $syns_avail_args = [
              ':type_id' => $this->exact_syn->cvterm_id,
              ':names' => $batch_synonyms
            ];
            $syns_avail = chado_query($syns_avail_sql, $syns_avail_args)->fetchAllAssoc('name', PDO::FETCH_OBJ);

            // First, add any missing synonyms
            $j = 0;
            foreach ($batch_synonyms as $index => $synonym) {
              if (!array_key_exists($synonym, $syns_avail)) {
                $j++;
                $syn_sql .= "(:name_$j, :type_id_$j, ''),\n";
                $syn_args[":name_$j"] = $synonym;
                $syn_args[":type_id_$j"] = $this->exact_syn->cvterm_id;
              }
            }
            if ($syn_sql) {
              $syn_sql = rtrim($syn_sql, ",\n");
              $syn_sql = $init_syn_sql . $syn_sql;
              $last_id = chado_query($syn_sql, $syn_args, ['return' => Database::RETURN_INSERT_ID]);
              $syns_avail = chado_query($syns_avail_sql, $syns_avail_args)->fetchAllAssoc('name', PDO::FETCH_OBJ);
            }

            // Add in the feature synonym records for this batch.
            $j = 0;
            foreach ($batch_featuresyn as $index => $featuresyn) {
              $j++;
              $fsyn_sql .= "(:synonym_id_$j, :feature_id_$j, :pub_id_$j),\n";
              $fsyn_args[":synonym_id_$j"] = $syns_avail[$featuresyn[0]]->synonym_id;
              $fsyn_args[":feature_id_$j"] = $featuresyn[1];
              $fsyn_args[":pub_id_$j"] = $this->null_pub->pub_id;
            }
            $fsyn_sql = rtrim($fsyn_sql, ",\n");
            $fsyn_sql = $init_fsyn_sql . $fsyn_sql;
            $last_id = chado_query($fsyn_sql, $fsyn_args, ['return' => Database::RETURN_INSERT_ID]);
          }

          $this->setItemsHandled($batch_num);

          // Now reset all of the varables for the next batch.
          $syn_sql = '';
          $fsyn_sql = '';
          $i = 0;
          $syn_args = [];
          $fsyn_args = [];
          $batch_synonyms = [];
          $batch_featuresyn = [];
        }
      }
    }

    // Add any remaining batch items
    if ($i > 0) {
      if (count($batch_synonyms) > 0) {
        // First get the synonym_ids for those already in the database.
        $syns_avail_sql = "SELECT synonym_id, name FROM {synonym} WHERE type_id = :type_id and name IN (:names)";
        $syns_avail_args = [
            ':type_id' => $this->exact_syn->cvterm_id,
            ':names' => $batch_synonyms
        ];
        $syns_avail = chado_query($syns_avail_sql, $syns_avail_args)->fetchAllAssoc('name', PDO::FETCH_OBJ);

        // First, add any missing synonyms
        $j = 0;
        foreach ($batch_synonyms as $index => $synonym) {
          if (!array_key_exists($synonym, $syns_avail)) {
            $j++;
            $syn_sql .= "(:name_$j, :type_id_$j, ''),\n";
            $syn_args[":name_$j"] = $synonym;
            $syn_args[":type_id_$j"] = $this->exact_syn->cvterm_id;
          }
        }
        if ($syn_sql) {
          $syn_sql = rtrim($syn_sql, ",\n");
          $syn_sql = $init_syn_sql . $syn_sql;
          $last_id = chado_query($syn_sql, $syn_args, ['return' => Database::RETURN_INSERT_ID]);
          $syns_avail = chado_query($syns_avail_sql, $syns_avail_args)->fetchAllAssoc('name', PDO::FETCH_OBJ);
        }

        // Add in the feature synonym records for this batch.
        $j = 0;
        foreach ($batch_featuresyn as $index => $featuresyn) {
          $j++;
          $fsyn_sql .= "(:synonym_id_$j, :feature_id_$j, :pub_id_$j),\n";
          $fsyn_args[":synonym_id_$j"] = $syns_avail[$featuresyn[0]]->synonym_id;
          $fsyn_args[":feature_id_$j"] = $featuresyn[1];
          $fsyn_args[":pub_id_$j"] = $this->null_pub->pub_id;
        }
        $fsyn_sql = rtrim($fsyn_sql, ",\n");
        $fsyn_sql = $init_fsyn_sql . $fsyn_sql;
        $last_id = chado_query($fsyn_sql, $fsyn_args, ['return' => Database::RETURN_INSERT_ID]);
      }

      $this->setItemsHandled($batch_num);
    }
  }


  /**
   * Load a GFF3 file. This is the function called by tripal jobs
   *
   * @ingroup gff3_loader
   */
  private function loadGFF3() {

    $ret = [];
    $date = getdate();

    $filesize = filesize($this->gff_file);
    $this->setTotalItems($filesize);


    $in_fasta = 0;
    $line_num = 0;
    $num_read = 0;

    // iterate through each line of the GFF file
    while ($line = fgets($this->gff_file_h)) {
      $line_num++;
      $this->line_num = $line_num;
      $size = drupal_strlen($line);
      $this->addItemsHandled($size);
      $num_read += $size;

      if ($line_num < $this->start_line) {
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
      if (preg_match('/^##sequence-region\s+(\w*?)\s+(\d+)\s+(\d+)$/i', $line, $region_matches)) {
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

      $cvterm = $this->getCvterm($type);
      if (!$cvterm) {
        throw new Exception(t('Cannot find feature term \'%type\' on line %line_num of the GFF file',
          ['%type' => $type, '%line_num' => $line_num]));
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
                $feature_organism = (object) chado_insert_record('organism', $values, array(
                  'skip_validation' => TRUE,
                ));
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
      }

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
            'feature_id' => $feature->getID(),
            'organism_id' => $feature->getValue('organism_id'),
            'type_name' => $type,
            'uniquename' => $feature->getValue('uniquename'),
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
        $protein_cvterm = $this->getCvterm('polypeptide');
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
              $cvterm = $this->getCvterm($result->feature_type);
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
   * Load a controlled vocabulary term.
   *
   * This method first checks if the term has already been loaded in the
   * feature_cvterm_lookup array, which helps a lot with performance.
   *
   * @param $type
   * @param $cv_id
   *
   * @ingroup gff3_loader
   */
  private function getCvtermID($type, $cv_id = NULL, $is_prop_type = FALSE) {
    if (!isset($cv_id)) {
      $cv_id = $this->feature_cv->getValue('cv_id');
    }
    if ($is_prop_type and array_key_exists($type, $this->featureprop_cvterm_lookup)) {
      return $this->featureprop_cvterm_lookup[$type];
    }
    elseif (array_key_exists($type, $this->feature_cvterm_lookup)) {
      return $this->feature_cvterm_lookup[$type];
    }

    $sel_cvterm_sql = "
      SELECT CVT.cvterm_id
      FROM {cvterm} CVT
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE CVT.cv_id = {$cv_id} and
       (lower(CVT.name) = lower(:name) or lower(CVTS.synonym) = lower(:synonym))
    ";
    $result = chado_query($sel_cvterm_sql, [
      ':name' => $type,
      ':synonym' => $type,
    ]);
    $cvterm = $result->fetchObject() ?? NULL;
    if ($cvterm) {
      $cvterm = chado_get_cvterm(array('cvterm_id' => $cvterm->cvterm_id)) ?? NULL;
    }

    if ($is_prop_type) {
      $this->featureprop_cvterm_lookup[$cvterm->name] = $cvterm->cvterm_id;
      $this->featureprop_cvterm_lookup[$type] = $cvterm->cvterm_id;
    }
    else {
      $this->feature_cvterm_lookup[$cvterm->name] = $cvterm->cvterm_id;
      $this->feature_cvterm_lookup[$type] = $cvterm->cvterm_id;

      // If the cvterm name does not match the name provided then set a mapping.
      if ($cvterm->name != $type) {
        $this->feature_cvterm_aliases[$type] = $cvterm->name;
      }
    }
    return $cvterm->cvterm_id;
  }

  /**
   * Retrieves a ChadoRecord object for the landmark feature.
   *
   * @param $landmark_name
   *   The name of the landmark to get
   * @param $landmark_type
   *   A ChadoRecord object for the type of landmark.
   * @param $skip_on_missing
   *   If the landmark cannot be found and this arugment is FALSE then
   *   an error will be thrown. If TRUE then no error is thrown.
   *
   * @return
   *   A feature ChadoRecord object or NULL if the landmark is missing and
   *   $skip_on_missing is TRUE.
   */
  private function getLandmark($landmark_name, $landmark_type = NULL, $skip_on_missing = FALSE) {

    // Before performing a database query check to see if
    // this landmark is already in our lookup list.
    if (array_key_exists($landmark_name, $this->landmarks)) {
      return $this->landmarks[$landmark_name];
    }

    $landmark = new ChadoRecord('feature');
    $landmark->setValues([
      'organism_id' => $this->organism_id,
      'uniquename' => $landmark_name,
    ]);
    if ($landmark_type) {
      $landmark->setValue('type_id', $landmark_type->getValue('cvterm_id'));
    }
    $num_found = $landmark->find();
    if ($num_found == 0) {
      if ($skip_on_missing == TRUE) {
        return NULL;
      }
      throw new Exception(t("The landmark '%landmark' cannot be found for this organism (%species) " .
        "Please add the landmark and then retry the import of this GFF3 " .
        "file", [
            '%landmark' => $landmark_name,
            '%species' => $this->organism->getValues('genus') . " " . $this->organism->getValues('species'),
        ]));
    }
    if ($num_found > 1) {
      throw new Exception(t("The landmark '%landmark' has more than one entry for this organism (%species). Did you provide a landmark type? If not, try resubmitting and providing a type." .
        "Cannot continue", [
            '%landmark' => $landmark_name,
            '%species' => $this->organism->getValues('genus') . " " . $this->organism->getValues('species'),
        ]));
    }


    // The landmark was found, remember it
    $this->landmarks[$landmark_name] = $landmark;
    return $landmark;

  }

  /**
   * Retrives the organism ID that matches the provided string.
   *
   * The organism string is expected to be in the format genus:species
   * or just the full name separated by spaces.
   */
  private function getOrganism($org_string) {

    // Before performing a database query check to see if
    // this organism is already in our lookup list.
    if (array_key_exists($org_string, $this->organism_lookup)) {
      return $this->organism_lookup[$org_string];
    }

    // See if the genus and species are spearated by a colon.
    $org_matches = [];
    if (preg_match('/^(.*?):(.*?)$/', $org_string, $org_matches)) {
      $values = [
        'genus' => $org_matches[1],
        'species' => $org_matches[2],
      ];
    }
    // See if the genus, species and infraspecific name are present.
    elseif (preg_match('/^(.*?)\s+(.*?)\s+(.*)$/', $org_string, $org_matches)) {
      $values = [
        'genus' => $org_matches[1],
        'species' => $org_matches[2],
        'infraspecific_name' => $org_matches[3],
      ];
    }
    // See if just the genus ans species are present.
    elseif (preg_match('/^(.*?)\s+(.*?)$/', $org_string, $org_matches)) {
      $values = [
        'genus' => $org_matches[1],
        'species' => $org_matches[2],
      ];
    }
    else {
      throw new Exception(t("The specified organism, '%organism', is not provided in a compatible format. It must be 'genus:species', 'genus species' or 'genus species infraspecific name'.", ['%organism' => $org_string]));
    }

    // Get the organism record and add it to our lookup list for next time.
    $organism = new ChadoRecord('organism');
    $organism->setValues($values);
    $num_found = $organism->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the specified organism, '%organism', for this GFF3 file.", ['%organism' => $org_string]));
    }
    $this->organism_lookup[$org_string] = $organism;

    return $organism;
  }

  /**
   * Retrieves the residues for a given feature.
   *
   */
  private function getResidues($feature, $is_landmark = FALSE) {
    return '';
  }

  /**
   * Determines the name for a feature using the ID and name attributes.
   *
   * @param $feature_attrs
   *   The associative array of attributes for the feature.
   *
   * @param $type
   *   The type of feature.
   *
   * @return array
   *   An associative array with 'uniquename' and 'name' keys.
   */
  private function getFeatureName($attrs, $type, $landmark_name, $fmin, $fmax) {
    // To ensure a name is unique we may need to use the date.
    $uniquename = '';
    $name = '';

    // If there is no ID or name then try to create a name and ID.
    if (!array_key_exists('ID', $attrs) and !array_key_exists('name', $attrs)) {

      // Check if an alternate ID field is suggested, if so, then use
      // that for the name.
      if (array_key_exists($this->alt_id_attr, $attrs)) {
        $uniquename = $attrs[$this->alt_id_attr][0];
        $name = $uniquename;
      }

      // If the row has a parent then generate a unqiue ID
      elseif (array_key_exists('Parent', $attrs)) {
        $uniquename = md5($attrs['Parent'][0] . "-" . $type . "-" .
          $landmark_name . ":" . ($fmin + 1) . ".." . $fmax);
        $name = $attrs['Parent'][0] . "-" . $type;
      }

      // Generate a unique name based on the date, type and location
      // and set the name to simply be the type.
      else {
        $uniquename = md5($type . "-" . $landmark_name . ":" . ($fmin + 1) . ".." . $fmax);
        $name = $type . "-" . $landmark_name;
      }
    }
    elseif (!array_key_exists('Name', $attrs)) {
      $uniquename = $attrs['ID'][0];
      $name = $attrs['ID'][0];

    }
    elseif (!array_key_exists('ID', $attrs)) {
      $uniquename = $attrs['Name'][0];
      $name = $attrs['Name'][0];
    }
    else {
      $uniquename = $attrs['ID'][0];
      $name = $attrs['Name'][0];
    }

    // Does this uniquename already exist? This can happen for subfeatures
    // (e.g. CDS features) that have multiple components but are really
    // all the same thing.
    if (array_key_exists($uniquename, $this->features)) {
      if (array_key_exists('Parent', $attrs)) {
        // Iterate through the list of similar IDs and see how many we have
        // then add a numeric suffix.
        $i = 2;
        while (array_key_exists($uniquename . "_" . $i, $this->features)) {
          $i++;
        }
        $uniquename = $uniquename . "_" . $i;
      }
      else {
        throw new Exception(t("A feature with the same ID exists multiple times: !uname", ['!uname' => $uniquename]));
      }
    }

    return [
      'name' => $name,
      'uniquename' => $uniquename,
    ];
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
    $derivesfrom_term = $this->getCvterm('derives_from');

    // First look for the object feature in the temp table to get it's type.
    $values = [
      'organism_id' => $organism->organism_id,
      'uniquename' => $object,
    ];
    $result = chado_select_record('tripal_gff_temp', ['type_name'], $values);
    $type_id = NULL;
    if (count($result) > 0) {
      $type_id = $this->getCvterm($result[0]->type_name)->cvterm_id ?? NULL;
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
        '%uniquename' => $feature->getValue('uniquename'),
        '%subject' => $subject,
      ], TRIPAL_ERROR);
      return;
    }

    // If this feature is a protein then add it to the tripal_gffprotein_temp.
    if ($type == 'protein' or $type == 'polypeptide') {
      $values = [
        'feature_id' => $feature->getID(),
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
      'subject_id' => $feature->getID(),
      'type_id' => $derivesfrom_term->cvterm_id,
      'rank' => 0,
    ];
    $rel = chado_select_record('feature_relationship', ['*'], $values);
    if (count($rel) > 0) {
      return;
    }

    // finally insert the relationship if it doesn't exist
    $ret = chado_insert_record('feature_relationship', $values, array(
      'skip_validation' => TRUE,
    ));
    if (!$ret) {
      $this->logMessage("Could not add 'Derives_from' relationship for :uniquename and :subject.",
        [
          ':uniquename' => $feature->getValue('uniquename'),
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

    $uname = $feature->getValue('uniquename');
    $type = $cvterm->name;
    $rel_type = 'part_of';
    $relcvterm = $this->getCvterm($rel_type);
    if (!$relcvterm) {
      throw new Exception(t("Cannot find the term, 'part_of', from the sequence ontology. This term is used for associating parent and children features. Please check that the ontology is fully imported."));
    }

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
      $parentcvterm = $this->getCvterm($parent_type);
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
          'subject_id' => $feature->getID(),
          'type_id' => $relcvterm->cvterm_id,
        ];
        $rel = chado_select_record('feature_relationship', ['*'], $values);

        if (count($rel) > 0) {
        }
        else {
          // the relationship doesn't already exist, so add it.
          $values = [
            'subject_id' => $feature->getID(),
            'object_id' => $parent_feature->feature_id,
            'type_id' => $relcvterm->cvterm_id,
          ];
          $result = chado_insert_record('feature_relationship', $values, array(
            'skip_validation' => TRUE,
          ));
          if (!$result) {
            $this->logMessage("Failed to insert feature relationship '$uname' ($type) $rel_type '$parent' ($parent_type).",
              [], TRIPAL_WARNING);
          }
        }

        // If this feature is a CDS and now that we know the parent we can
        // add it to the tripal_gffcds_temp table for later lookup.
        if ($type == 'CDS') {
          $values = [
            'feature_id' => $feature->getID(),
            'parent_id' => $parent_feature->feature_id,
            'fmin' => $fmin,
            'fmax' => $fmax,
            'strand' => $strand,
          ];
          if (isset($phase)) {
            $values['phase'] = $phase;
          }
          $result = chado_insert_record('tripal_gffcds_temp', $values, array(
            'skip_validation' => TRUE,
          ));
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
        $success = chado_insert_record('db', $values, array(
          'skip_validation' => TRUE,
        ));
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
        $ret = chado_insert_record('dbxref', $values, array(
          'skip_validation' => TRUE,
        ));
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
        'feature_id' => $feature->getID(),
      ];
      $fdbx = chado_select_record('feature_dbxref', ['feature_dbxref_id'], $values);

      // now associate this feature with the database reference if it doesn't
      // already exist
      if (sizeof($fdbx) == 0) {
        $values = [
          'dbxref_id' => $dbxref->dbxref_id,
          'feature_id' => $feature->getID(),
        ];
        $success = chado_insert_record('feature_dbxref', $values, array(
          'skip_validation' => TRUE,
        ));
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
          'feature_id' => $feature->getID(),
        ]);

      // now associate this feature with the cvterm if it doesn't already exist
      if (sizeof($fcvt) == 0) {
        $values = [
          'cvterm_id' => $cvterm->cvterm_id,
          'feature_id' => $feature->getID(),
          'pub_id' => [
            'uniquename' => 'null',
          ],
        ];
        $success = chado_insert_record('feature_cvterm', $values, array(
          'skip_validation' => TRUE,
        ));

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
      $success = chado_insert_record('cv', $values, array(
        'skip_validation' => TRUE,
      ));
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
        $success = chado_insert_record('synonym', $values, array(
          'skip_validation' => TRUE,
        ));
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
        'feature_id' => $feature->getID(),
        'pub_id' => $pub->pub_id,
      ];
      $columns = ['feature_synonym_id'];
      $result = chado_select_record('feature_synonym', $columns, $values);
      if (count($result) == 0) {
        $values = [
          'synonym_id' => $synonym->synonym_id,
          'feature_id' => $feature->getID(),
          'pub_id' => $pub->pub_id,
        ];
        $success = chado_insert_record('feature_synonym', $values, array(
          'skip_validation' => TRUE,
        ));

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
  private function loadFeature($organism, $analysis, $cvterm, $uniquename,
                               $name, $residues, $is_analysis = 'f', $is_obsolete = 'f', $add_only, $score) {

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

    // Check to see if the feature already exists.
    $feature = new ChadoRecord('feature');
    $feature->setValues([
       'organism_id' => $organism->getValue('organism_id'),
       'uniquename' => $uniquename,
       'type_id' => $cvterm->getValue('cvterm_id'),
    ]);
    $num_matches = $feature->find();


    // Insert the feature if it does not exist otherwise perform an update.
    if ($num_matches == 0) {
      $feature->setValue('name', $name);
      $feature->setValue('md5checksum', md5($residues));
      $feature->setValue('is_analysis', $is_analysis);
      $feature->setValue('is_obsolete', $is_obsolete);
      try {
        $feature->insert();
      }
      catch (Exception $e) {
        $this->logMessage("Failed to insert feature '$uniquename' (" . $cvterm->getValue('name') . ").", [], TRIPAL_WARNING);
        return 0;
      }
    }
    elseif (!$add_only) {
      if ($num_matches > 1) {
        $this->logMessage("Failed to update feature '$uniquename' (" . $cvterm->getValue('name') . "). More than one feature exists with these criteria", [], TRIPAL_WARNING);
        return 0;
      }
      $feature->setValue('name', $name);
      $feature->setValue('md5checksum', md5($residues));
      $feature->setValue('is_analysis', $is_analysis);
      $feature->setValue('is_obsolete', $is_obsolete);
      try {
        $feature->update();
      }
      catch (Exception $e) {
        $this->logMessage("Failed to update feature '$uniquename' (" . $cvterm->getValue('name') . ").", [], TRIPAL_WARNING);
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
    $af = new ChadoRecord('analysisfeature');
    $af->setValues([
        'analysis_id' => $analysis->getValue('analysis_id'),
        'feature_id' => $feature->getID(),
    ]);
    $num_afs = $af->find();
    if ($num_afs == 0) {
      // if a score is available then set that to be the significance field
      if (strcmp($score, '.') != 0) {
        $af->setValue('significance', $score);
      }
      try {
        $af->insert();
      }
      catch (Exception $e) {
        $this->logMessage("Could not add analysisfeature record: " . $analysis->getValue('analysis_id') . ", " .  $feature->getID() . ". " . $e->getMessage(), [], TRIPAL_WARNING);
      }
    }
    else {
      // if a score is available then set that to be the significance field
      $new_vals = [];
      if (strcmp($score, '.') != 0) {
        $af->setValue('significance', $score);
      }
      else {
        $af->setValue('significance', '__NULL__');
      }
      if (!$add_only) {
        try {
          $af->update();
        }
        catch (Exception $e) {
          $this->logMessage("Could not update analysisfeature record: $analysis_id, " . $feature->getID() . ". " . $e->getMessage(), [], TRIPAL_WARNING);
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

    $srcfeature = new ChadoRecord('feature');
    $srcfeature->setValues([
        'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
        'uniquename' => $landmark,
    ]);
    if ($landmark_type_id) {
      $srcfeature->setValue('type_id', $landmark_type_id);
    }
    $num_srcf = $srcfeature->find();

    if ($num_srcf == 0) {
      // so we couldn't find the landmark using the uniquename. Let's try the 'name'.
      // if we return only a single result then we can proceed. Otherwise give an
      $srcfeature = new ChadoRecord('feature');
      $srcfeature->setValues([
          'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
          'name' => $landmark,
      ]);
      if ($landmark_type_id) {
        $srcfeature->setValue('type_id', $landmark_type_id);
      }
      $num_srcf = $srcfeature->find();
      if ($num_srcf == 0) {
         // if the landmark is the target feature in a matched alignment then try one more time to
         // find it by querying any feature with the same uniquename. If we find one then use it.
         if ($landmark_is_target) {
            $srcfeature = new ChadoRecord('feature');
            $srcfeature->setValues([
               'uniquename' => $landmark,
            ]);
            $num_srcf =  $srcfeature->find();
            if ($num_srcf > 1) {
              $this->logMessage("Multiple landmarks exist for a matached target with the name: '%landmark'.  Cannot " .
                "resolve which one to use. Cannot add the feature location record.",
                ['%landmark' => $landmark], TRIPAL_WARNING);
            }
         }

         if ($num_srcf == 0) {
            // we couldn't find the landmark feature, so if the user has requested we create it then do so
            // but only if we have a type id
            if ($create_landmark and $landmark_type_id) {
               $srcfeature = new ChadoRecord('feature');
               $srcfeature->setValues([
                  'organism_id' => $landmark_organism_id ? $landmark_organism_id : $organism->organism_id,
                  'name' => $landmark,
                  'uniquename' => $landmark,
                  'type_id' => $landmark_type_id,
               ]);
               try {
                  $srcfeature->insert();
               }
               catch (Exception $e) {
                  $this->logMessage("Cannot find landmark feature: '%landmark', nor could it be inserted. " . $e->getMessage() ,
                     ['%landmark' => $landmark], TRIPAL_WARNING);
                  return 0;
               }
            }
            else {
               $this->logMessage("Cannot find unique landmark feature: '%landmark'.",
                  ['%landmark' => $landmark], TRIPAL_WARNING);
               return 0;
            }
         }
      }
      elseif ($num_srcf > 1) {
        $this->logMessage("multiple landmarks exist with the name: '%landmark'.  Cannot " .
          "resolve which one to use. Cannot add the feature location record.",
          ['%landmark' => $landmark], TRIPAL_WARNING);
        return 0;
      }
    }
    elseif ($num_srcf > 1) {
      $this->logMessage("multiple landmarks exist with the name: '%landmark'.  Cannot " .
        "resolve which one to use. Cannot add the feature location record.",
        ['%landmark' => $landmark, TRIPAL_WARNING]);
      return 0;
    }


    // TODO: create an attribute that recognizes the residue_info,locgroup,
    //  is_fmin_partial and is_fmax_partial, right now these are
    //  hardcoded to be false and 0 below.

    // check to see if this featureloc already exists, but also keep track of the
    // last rank value
    $rank = 0;
    $exists = 0;
    $select = ['feature_id' => $feature->getID()];
    $options = [
      'order_by' => [
        'rank' => 'ASC',
      ],
    ];

    $locrecs = chado_select_record('featureloc', ['*'], $select, $options);
    foreach ($locrecs as $locrec) {
       // it is possible for the featureloc->srcfeature_id to be NULL.
       // This can happen if the srcfeature is not known (according to chado
       // table field descriptions).  If it's null then just skip this entry.
       if (!$locrec->srcfeature_id) {
          continue;
       }
       $select = ['feature_id' => $locrec->srcfeature_id];
       $columns = ['feature_id', 'name'];
       $locsfeature = chado_select_record('feature', $columns, $select);

       // the source feature name and at least the fmin and fmax must be the same
       // for an update of the featureloc, otherwise we'll insert a new record.
       if (strcmp($locsfeature[0]->name, $landmark) == 0 and
          ($locrec->fmin == $fmin or $locrec->fmax == $fmax)) {

          $featureloc = new ChadoRecord('featureloc');
          $featureloc->setValue('featureloc_id', $locrec->featureloc_id);
          $values = [];
          $exists = 1;
          if ($featureloc->fmin != $fmin) {
             $featureloc->setValue('fmin', $fmin);
          }
          if ($featureloc->fmax != $fmax) {
             $featureloc->setValue('fmax', $fmax);
          }
          if ($featureloc->strand != $strand) {
             $featureloc->setValue('strand', $strand);
          }
          if (count($featureloc->getValues()) > 0) {
            $featureloc->update();
          }
       }
       $rank = $locrec->rank + 1;
    }
    if (!$exists) {

      // this feature location is new so add it
      if (strcmp($is_fmin_partial, 'f') == 0 or !$is_fmin_partial) {
        $is_fmin_partial = 'FALSE';
      }
      elseif (strcmp($is_fmin_partial, 't') == 0 or $is_fmin_partial == 1) {
        $is_fmin_partial = 'TRUE';
      }
      if (strcmp($is_fmax_partial, 'f') == 0 or !$is_fmax_partial) {
        $is_fmax_partial = 'FALSE';
      }
      elseif (strcmp($is_fmax_partial, 't') == 0 or $is_fmax_partial == 1) {
        $is_fmax_partial = 'TRUE';
      }
      $featureloc = new ChadoRecord('featureloc');
      $featureloc->setValues([
        'feature_id' => $feature->getID(),
        'srcfeature_id' => $srcfeature->getID(),
        'fmin' => $fmin,
        'is_fmin_partial' => $is_fmin_partial,
        'fmax' => $fmax,
        'is_fmax_partial' => $is_fmax_partial,
        'strand' => $strand,
        'residue_info' => $residue_info,
        'locgroup' => $locgroup,
        'rank' => $rank,
      ]);
      if (!$residue_info) {
        $featureloc->setValue('residue_info', '__NULL__');
      }
      if ($phase) {
        $featureloc->setValue('phase', $phase);
      }
      $featureloc->insert();
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
    $result = $this->getCvterm($property, $this->feature_property_cv_id);

    // If we don't have a property like this already, then add it otherwise,
    // just return.
    if (empty($result)) {
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
      $cvterm = $result;
    }


    // Check to see if the property already exists for this feature
    // if it does but the value is unique then increment the rank and add it.
    // if the value is not unique then don't add it.
    $add = 1;
    $rank = 0;
    $select = [
      'feature_id' => $feature->getId(),
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
        'feature_id' => $feature->getID(),
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
      if (!empty($matches[4])) {
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
      $this->loadFeatureLoc($feature, NULL, $target_feature, $target_fmin,
        $target_fmax, $target_strand, NULL, NULL, NULL, NULL,
        $attr_locgroup, $t_type_id, $t_organism_id, $create_target, TRUE);
    }
    // the target attribute is not correctly formatted
    else {
      $this->logMessage("Could not add 'Target' alignment as it is improperly formatted:  '%target'",
        ['%target' => $tags['Target'][0]], TRIPAL_ERROR);
    }
  }
}
