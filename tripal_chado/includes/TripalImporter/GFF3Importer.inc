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
  private $organism_lookup = [];

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
   * Holds a mapping of Dbxref names to cvterm ids.
   */
  private $cvterm_lookup = [];

  /**
   * Holds a mapping of synonymns to ids.
   */
  private $synonym_lookup = [];

  /**
   * Maps parents to their children and contains the ranks of the children.
   */
  private $parent_lookup = [];
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
   * The list of features from the GFF3 file.  Each element is an
   * associative array of the columns from the GFF3 file, with the attribute
   * field being an associative array of key/value pairs.
   */
  private $features = [];

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
      '#title' => t('Existing Organism'),
      '#type' => 'select',
      '#description' => t("Choose an existing organism to which the entries in the GFF file will be associated."),
      '#required' => TRUE,
      '#options' => $organisms,
    ];
    $form['create_organism'] = [
      '#type' => 'checkbox',
      '#title' => t('Create organism'),
      '#required' => FALSE,
      '#description' => t('The Tripal GFF loader supports the "organism" attribute. This allows features of a
       different organism to be aligned to the landmark sequence.  The format of the
       attribute is "organism=[genus]:[species]", where [genus] is the organism\'s genus and [species] is the
       species name. Check this box to automatically add the organism to the database if it does not already exists.
       Otherwise lines with an organism attribute where the organism is not present in the database will be skipped.'),
    ];

    $form['landmark_type'] = [
      '#title' => t('Landmark Type'),
      '#type' => 'textfield',
      '#description' => t("Optional. Use this field to specify a Sequence Ontology type
       for the landmark sequences in the GFF fie (e.g. 'chromosome'). This is only needed if
       the landmark features (first column of the GFF3 file) are not already in the database.."),
    ];

    $form['proteins'] = [
      '#type' => 'fieldset',
      '#title' => t('Proteins'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];
    $form['proteins']['skip_protein'] = [
      '#type' => 'checkbox',
      '#title' => t('Skip automatic protein creation'),
      '#required' => FALSE,
      '#description' => t('The GFF loader will automatically create a protein feature for each transcript in the GFF file if a protein feature is missing in the GFF file. Check this box to disable this functionality. Protein features that are specifically present in the GFF will always be created.'),
      '#default_value' => 0,
    ];
    $form['proteins']['re_mrna'] = [
      '#type' => 'textfield',
      '#title' => t('Optional. Regular expression for the mRNA name'),
      '#required' => FALSE,
      '#description' => t('If automatic protein creation is enabled, then by default the loader will give each protein a name based on the name of the corresponding mRNA followed by the "-protein" suffix.
       If you want to customize the name of the created protein, you can enter a regular expression that will extract portions of
       the mRNA unique name. For example, for a
       mRNA with a unique name finishing by -RX (e.g. SPECIES0000001-RA),
       the regular expression would be, "^(.*?)-R([A-Z]+)$". Elements surrounded by parentheses are captured as backreferences and can be used for replacement.' ),
    ];
    $form['proteins']['re_protein'] = [
      '#type' => 'textfield',
      '#title' => t('Optional. Replacement string for the protein name'),
      '#required' => FALSE,
      '#description' => t('If a regular expression is used to specify a protein name you can use the backreference tokens to extract the portion of the mRNA name that you want to use for a protein.
       You use a dollar sign followed by a number to indicate the backreferences. For example: "$1-P$2".'),
    ];

    $form['targets'] = [
      '#type' => 'fieldset',
      '#title' => t('Targets'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];
    $form['targets']['adesc'] = [
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
    $form['targets']['target_organism_id'] = [
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
    $form['targets']['target_type'] = [
      '#title' => t('Target Type'),
      '#type' => t('textfield'),
      '#description' => t("Optional. If the unique name for a target sequence is not unique (e.g. a protein
       and an mRNA have the same name) then you must specify the type for all targets in the GFF file. If
       the targets are of different types then the type must be specified using the 'target_type=type' attribute
       in the GFF file. This must be a valid Sequence Ontology (SO) term."),
    ];
    $form['targets']['create_target'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Target'),
      '#required' => FALSE,
      '#description' => t("If the target feature cannot be found, create one using the organism and type specified above, or
       using the 'target_organism' and 'target_type' fields specified in the GFF file.  Values specified in the
       GFF file take precedence over those specified above."),
    ];

    // Advanced Options
    $form['advanced'] = [
      '#type' => 'fieldset',
      '#title' => t('Additional Options'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];


    $form['advanced']['line_number'] = [
      '#type' => 'textfield',
      '#title' => t('Start Line Number'),
      '#description' => t('Enter the line number in the GFF file where you would like to begin processing.  The
      first line is line number 1.  This option is useful for examining loading problems with large GFF files.'),
      '#size' => 10,
    ];

    $form['advanced']['alt_id_attr'] = [
      '#title' => t('ID Attribute'),
      '#type' => t('textfield'),
      '#description' => t("Optional. Sometimes lines in the GFF file are missing the
      required ID attribute that specifies the unique name of the feature, but there
      may be another attribute that can uniquely identify the feature.  If so,
      you may specify the name of the attribute to use for the name."),
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

    // Load the GFF3.
    $this->logMessage("Step 1: Preloading GFF3 file...                       ");
    $this->parseGFF3();

    // Prep the database for necessary records.
    $this->prepSynonms();
    $this->prepNullPub();
    $this->prepDBs();

    $this->logMessage("Step 2: Load landmarks sequences...                       ");
    $this->insertLandmarks();

    $this->logMessage("Step 3: Find existing features...                         ");
    $this->findFeatures();

    $this->logMessage("Step 4: Prepare for any updates ...                       ");
    $this->deleteFeatureData();

    $this->logMessage("Step 5: Loading !num_features features...                 ",
        ['!num_features' => count(array_keys($this->features))]);
    $this->insertFeatures();

    $this->logMessage("Step 6: Get new feature IDs...                            ");
    $this->findFeatures();

    $this->logMessage("Step 7: Loading locations...                              ");
    $this->insertFeatureLocs();

    $this->logMessage("Step 8: Loading properties...                             ");
    $this->insertFeatureProps();

    $this->logMessage("Step 9: Finding synonyms (aliases)...                     ");
    $this->findSynonyms();

    $this->logMessage("Step 10: Loading synonsyms (aliases)...                   ");
    $this->insertSynonyms();

    $this->logMessage("Step 11: Loading feature synonyms (aliases)...            ");
    $this->insertFeatureSynonyms();

    $this->logMessage("Step 12: Finding existing cross references...             ");
    $this->findDbxrefs();

    $this->logMessage("Step 13: Loading cross references...                      ");
    $this->insertDbxrefs();

    $this->logMessage("Step 14: Retrieving loaded cross references...            ");
    $this->findDbxrefs();

    $this->logMessage("Step 15: Loading feature cross references...              ");
    $this->insertFeatureDbxrefs();

    $this->logMessage("Step 16: Loading feature ontology terms...                ");
    $this->insertFeatureCVterms();

    $this->logMessage("Step 17: Associate child-parent relationships...          ");
    $this->findChildRanks();
    $this->insertFeatureParents();

    $this->logMessage("Step 18: Loading 'derives_from' relationships...          ");
    $this->insertFeatureDerivesFrom();

    $this->logMessage("Step 19: Loading Targets...                               ");
    // TODO: Target (target_organism & target_type)

    $this->logMessage("Step 20: Adding any missing proteins...                   ");
    // TODO: protein records.

    if (!empty($this->residue_index)) {
      $this->logMessage("Step 21: Adding sequences if available...                 ");
      $this->insertFeatureSeqs();
    }
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
    if ($is_prop_type and array_key_exists(strtolower($type), $this->featureprop_cvterm_lookup)) {
      return $this->featureprop_cvterm_lookup[strtolower($type)];
    }
    elseif (array_key_exists(strtolower($type), $this->feature_cvterm_lookup)) {
      return $this->feature_cvterm_lookup[strtolower($type)];
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
      $this->featureprop_cvterm_lookup[strtolower($cvterm->name)] = $cvterm->cvterm_id;
      $this->featureprop_cvterm_lookup[strtolower($type)] = $cvterm->cvterm_id;
    }
    else {
      $this->feature_cvterm_lookup[strtolower($cvterm->name)] = $cvterm->cvterm_id;
      $this->feature_cvterm_lookup[strtolower($type)] = $cvterm->cvterm_id;

      // If the cvterm name does not match the name provided then set a mapping.
      if ($cvterm->name != $type) {
        $this->feature_cvterm_aliases[$type] = $cvterm->name;
      }
    }
    return $cvterm->cvterm_id;
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

    // Check to see if we have a NULL publication in the pub table.  If not,
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

      // Insert the null pub.
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
   * Makes sure Chado is ready with the necessary DB records.
   */
  private function prepDBs() {

    // Get the list of database records that are needed by this GFF file. If
    // they do not exist then add them.
    $sql = "
      SELECT db_id
      FROM {db}
      WHERE name = :dbname";

    foreach (array_keys($this->db_lookup) as $dbname) {
      // First look for the database name if it doesn't exist then create one.
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
          'description' => 'Added automatically by the Triapl GFF loader.',
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

      $this->db_lookup[$dbname] = $db[0]->db_id;
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
    $attr_organism = [];
    $attr_parent = '';
    $attr_others = [];
    $attr_aliases = [];
    $attr_dbxref = [];
    $attr_derives = [];
    $attr_terms = [];
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

      if (strcmp($tag_name, 'Alias') == 0) {
        $attr_aliases = array_merge($attr_aliases, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Parent') == 0) {
        $attr_parent = urldecode($tag[1]);
      }
      elseif (strcmp($tag_name, 'Dbxref') == 0) {
        $attr_dbxref = array_merge($attr_dbxref, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Derives_from') == 0) {
        $attr_derives = array_merge($attr_derives, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'Ontology_term') == 0) {
        $attr_terms = array_merge($attr_terms, $tags[$tag_name]);
      }
      elseif (strcmp($tag_name, 'organism') == 0) {
        $attr_organism = array_merge($attr_organism, $tags[$tag_name]);
      }
      // Get the list of non-reserved attributes these will get added
      // as properties to the featureprop table.  The 'Note' and 'Gap'
      // attributes will go in as a property so those are not in the list
      // checked below.
      elseif (strcmp($tag_name, 'Name') !=0 and strcmp($tag_name, 'ID') !=0 and
              strcmp($tag_name, 'Alias') != 0 and strcmp($tag_name, 'Parent') != 0 and
              strcmp($tag_name, 'Target') != 0 and strcmp($tag_name, 'Derives_from') != 0 and
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

    // Add in the dbxref record.
    $ret['dbxrefs'] = [];
    foreach ($attr_dbxref as $key => $dbx) {
      $parts = explode(':', $dbx);
      if (count($parts) != 2) {
        throw new Exception(t('Dbxrefs must be of the format: "<db name>:<accession>". The Dbxref %dbx on line %line_num does not satisfy this format.',
            ['%line_num' => $this->current_line, '%dbx' => $dbx]));
      }
      $ret['dbxrefs']["{$parts[0]}:{$parts[1]}"] = array(
        'db' => $parts[0],
        'accession' => $parts[1],
      );
    }

    // Add in the GFF source dbxref. This is needed for GBrowse.
    $ret['dbxrefs']["GFF_source:{$ret['source']}"] = array(
      'db' => 'GFF_source',
      'accession' => $ret['source'],
    );

    // Add in the ontology terms
    $ret['terms'] = [];
    foreach ($attr_terms as $key => $dbx) {
      $parts = explode(':', $dbx);
      if (count($parts) != 2) {
        throw new Exception(t('Ontology_terms must be of the format: "<db name>:<accession>". The term %dbx on line %line_num does not satisfy this format.',
            ['%line_num' => $this->current_line, '%dbx' => $dbx]));
      }
      $ret['terms']["{$parts[0]}:{$parts[1]}"] = array(
        'db' => $parts[0],
        'accession' => $parts[1],
      );
    }

    // Add the derives from entry.
    $ret['derives_from'] = '';
    if (count($attr_derives) == 1) {
      $ret['derives_from'] = $attr_derives[0];
    }
    if (count($attr_derives) > 1) {
      throw new Exception(t('Each feature can only have one "Derives_from" attribute. The feature %uniquename has more than one: %derives',
        [
          '%uniquename' => $ret['uniquename'],
          '%derives' => $ret['derives_from'],
        ]));
    }

    // Now add all of the attributes into the return array.
    foreach ($tags as $key => $value) {
      $ret['attrs'][$key] = $value;
    }


    // Add the organism  entry.
    $ret['organism'] = '';
    if (count($attr_organism) == 1) {
      $ret['organism'] = $attr_organism[0];
    }
    if (count($attr_organism) > 1) {
      throw new Exception(t('Each feature can only have one "organism" attribute. The feature %uniquename has more than one: %organism',
        [
          '%uniquename' => $ret['uniquename'],
          '%organism' => $ret['organism'],
        ]));
    }


    $ret['properties'] = $attr_others;
    $ret['parent'] = $attr_parent;

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
        $this->residue_index[trim($id)] = ftell($this->gff_file_h);
      }
    }
  }

  /**
   * Loads the actual residue information from the FASTA section of the file.
   */
  private function insertFeatureSeqs() {

    $num_residues = count(array_keys($this->residue_index));

    $this->setItemsHandled(0);
    $this->setTotalItems($num_residues);

    $count = 0;

    foreach ($this->residue_index as $uniquename => $offset) {
      $is_landmark = FALSE;
      if (!(array_key_exists($uniquename, $this->features) and $this->features[$uniquename]) and !(array_key_exists($uniquename, $this->landmarks) and $this->landmarks[$uniquename])) {
        $this->logMessage('Cannot find feature to assign FASTA sequence: %uname.',
          ['%uname' => $uniquename], TRIPAL_WARNING);
        $count++;
        continue;
      }

      if (array_key_exists($uniquename, $this->features)) {
        $feature = $this->features[$uniquename];
      }
      else {
        $feature = $this->landmarks[$uniquename];
        $is_landmark = TRUE;
      }

      $this->ensureFeatureIsLoaded($feature);
      $id = $feature['feature_id'];

      $residues = [];
      fseek($this->gff_file_h, $offset);
      while ($line = fgets($this->gff_file_h)) {
        if (preg_match('/^>/', $line)) {
          break;
        }
        $residues[] = trim($line);
      }

      $residues = implode('', $residues);
      $feature['residues'] = $residues;

      if (!$is_landmark) {
        $this->features[$uniquename] = $feature;
      }
      else {
        $this->landmarks[$uniquename] = $feature;
      }

      chado_update_record('feature', ['feature_id' => $id], [
        'residues' => $residues,
        'seqlen' => strlen($residues),
        'md5checksum' => md5($residues),
      ]);

      $count++;
      $this->setItemsHandled($count);
    }
  }

  /**
   * Retrieves a ChadoRecord object for the landmark feature.
   *
   * @param $landmark_name
   *   The name of the landmark to get
   *
   * @return
   *   A feature ChadoRecord object or NULL if the landmark is missing and
   *   $skip_on_missing is TRUE.
   */
  private function findLandmark($landmark_name) {

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
      return NULL;
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
   * Loads into the database any landmark sequences.
   *
   * @param $line
   *   The line from the GFF file that is the ##sequence-region comment.
   */
  private function insertHeaderLandmark($line) {

    $region_matches = [];
    if (preg_match('/^##sequence-region\s+(\w*?)\s+(\d+)\s+(\d+)$/i', $line, $region_matches)) {
      $rid = $region_matches[1];
      $landmark = $this->findLandmark($rid);
      if (!$landmark) {
        $rstart = $region_matches[2];
        $rend = $region_matches[3];
        if (!$this->landmark_type) {
          throw new Exception(t('The landmark, !landmark, cannot be added becuase no landmark type was provided. Please redo the importer job and specify a landmark type.',
              ['!landmark' => $rid]));
        }
        $this->insertLandmark($rid);
      }
    }
  }

  /**
   * Loads a single landmark by name.
   */
  private function insertLandmark($name) {
    //$this->logMessage('Adding a new landmark feature: !landmark', ['!landmark' => $name]);
    $landmark = $this->insertFeature($this->organism, $this->analysis, $this->landmark_cvterm, $name,
        $name, '', 'f', 'f', 1, 0);
    $this->landmarks[$name] = [
      'uniquename' => $landmark->getValue('uniquename'),
      'name' => $landmark->getValue('name'),
      'type' => $this->landmark_cvterm->getValue('name'),
      'feature_id' => $landmark->getValue('feature_id'),
      'organism_id' => $landmark->getValue('organism_id'),
    ];
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
      $gff_feature['feature_id'] = NULL;

      // A feature may get ignored. But let's default this to FALSE.
      $gff_feature['skipped'] = FALSE;

      // Lookup the organism ID if one is requested.
      if ($gff_feature['organism']) {
        $organism_id = $this->findOrganism($gff_feature['organism'], $line_num);
        if ($organism_id) {
          $gff_feature['organism'] = $organism_id;
        }
        elsE {
          $gff_feature['skipped'] = TRUE;
        }
      }

      // Add the landmark if it doesn't exist in the landmark list.
      if (!array_key_exists($gff_feature['landmark'], $this->landmarks)) {
        $this->landmarks[$gff_feature['landmark']] = FALSE;
      }

      // Organize DBs and DBXrefs for faster access later on.
      foreach ($gff_feature['dbxrefs'] as $index => $info) {
        if (!array_key_exists($info['db'], $this->db_lookup)) {
          $this->db_lookup[$info['db']] = FALSE;
        }
        if (!array_key_exists($index, $this->dbxref_lookup)) {
          $this->dbxref_lookup[$index] = $info;
        }
      }
      // We want to make sure the Ontology_term attribute dbxrefs are
      // also easily looked up... but we do not want to create them
      // if they do not exist the precense of the 'cvterm' key will
      // tell the loadDbxrefs() function to not create the term.
      foreach ($gff_feature['terms'] as $index => $info) {
        if (!array_key_exists($info['db'], $this->db_lookup)) {
          $this->db_lookup[$info['db']] = FALSE;
        }

        if (!array_key_exists($index, $this->dbxref_lookup)) {
          $this->dbxref_lookup[$index] = $info;
          $this->dbxref_lookup[$index]['cvterm_id'] = NULL;
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

      // Store the GFF feature details.
      if ($gff_feature['uniquename'] != $gff_feature['landmark']) {
        $this->features[$gff_feature['uniquename']] = $gff_feature;
      }

    }

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
        $this->featureprop_cvterm_lookup[strtolower($cvterm->name)] = $cvterm->cvterm_id;
      }
    }
  }

  /**
   * Imports the landmark features into Chado.
   */
  private function insertLandmarks() {
    foreach ($this->landmarks as $uniquename => $feature) {
      // If the landmark does not have an entry in the GFF lines, try to
      // find or add it.
      if ($feature === FALSE) {
        // First see if there is a definition in the headers region.
        if (array_key_exists($uniquename, $this->seq_region_headers)) {
          $this->insertHeaderLandmark($this->seq_region_headers[$uniquename]);
          continue;
        }
        // Second, if a landmark_type is provided then just add the landmark feature.
        else if ($this->landmark_type) {
          $landmark = $this->findLandmark($uniquename);
          if (!$landmark) {
            $this->insertLandmark($uniquename);
          }
        }
        else {
          throw new Exception(t('The landmark (reference) sequence, !landmark, is not in the database and not specified in the GFF3 file. Please either pre-load the landmark sequences or set a "Landmark Type" in the GFF importer.',
              ['!landmark' => $uniquename]));
        }
      }
    }
  }

  /**
   * Imports the feature records into Chado.
   */
  private function insertFeatures() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "
      INSERT INTO {feature}
        (uniquename, name, type_id, organism_id, residues, md5checksum,
         seqlen, is_analysis, is_obsolete)
      VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {
      $total++;

      // Only do an insert if this feature doesn't already exist in the databse.
      if (!$feature['feature_id'] and !$feature['skipped']) {
        $i++;
        $residues = $this->getResidues($feature, FALSE);
        $type_id = $this->feature_cvterm_lookup[strtolower($feature['type'])];
        $sql .= "(:uniquename_$i, :name_$i, :type_id_$i, :organism_id_$i, :residues_$i, " .
               " :md5checksum_$i, :seqlen_$i, FALSE, FALSE),\n";
        $args[":uniquename_$i"] = $feature['uniquename'];
        $args[":name_$i"] = $feature['name'];
        $args[":type_id_$i"] = $type_id;
        $args[":organism_id_$i"] = $feature['organism'] ? $feature['organism'] : $this->organism->getID();
        $args[":residues_$i"] = $residues;
        $args[":md5checksum_$i"] = $residues ? md5($residues) : '';
        $args[":seqlen_$i"] = strlen($residues);
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   * Check if the features exist in the database.
   */
  private function findFeatures() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $sql = "SELECT uniquename, type_id, organism_id, feature_id FROM {feature} WHERE uniquename in (:uniquenames)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $names = [];
    foreach ($this->features as $uniquename => $feature) {
      $total++;

      if (!$feature['feature_id']) {
        $i++;
        $names[] = $uniquename;
      }

      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_features) {
        if (count($names) > 0) {
          $args = [':uniquenames' => $names];
          $results = chado_query($sql, $args);
          while ($f = $results->fetchObject()) {

            $matched_feature = $this->features[$f->uniquename];
            $matched_type_id = $this->feature_cvterm_lookup[strtolower($matched_feature['type'])];
            $matched_organism_id = $matched_feature['organism'] ? $matched_feature['organism'] : $this->organism->getID();

            if ($matched_type_id == $f->type_id and $matched_organism_id == $f->organism_id) {
              $this->features[$f->uniquename]['feature_id'] = $f->feature_id;
            }
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $i = 0;
        $names = [];
      }
    }
  }
  /**
   * Deletes all anciallary data about a feature so we can re-insert it.
   */
  private function deleteFeatureData() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $sql1 = "DELETE from {featureprop} WHERE feature_id IN (:feature_ids)";
    $sql2 = "DELETE from {featureloc} WHERE feature_id IN (:feature_ids)";
    $sql3 = "DELETE from {feature_cvterm} WHERE feature_id IN (:feature_ids)";
    $sql4 = "DELETE from {feature_dbxref} WHERE feature_id IN (:feature_ids)";
    $sql5 = "DELETE from {feature_synonym} WHERE feature_id IN (:feature_ids)";
    $sql6 = "DELETE from {feature_relationship} WHERE subject_id IN (:feature_ids)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $feature_ids = [];
    foreach ($this->features as $feature) {
      $total++;

      if ($feature['feature_id'] and !$feature['skipped']) {
        $i++;
        $feature_ids[] = $feature['feature_id'];
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($feature_ids) > 0) {
          $args[':feature_ids'] = $feature_ids;
          chado_query($sql1, $args);
          chado_query($sql2, $args);
          chado_query($sql3, $args);
          chado_query($sql4, $args);
          chado_query($sql5, $args);
          chado_query($sql6, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $i = 0;
        $feature_ids = [];
      }
    }
  }

  /**
   *
   */
  private function insertFeatureProps(){
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {featureprop} (feature_id, type_id, value, rank) VALUES\n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {
      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Iterate through all of the properties of this feature.
        foreach ($feature['properties'] as $prop_name => $values) {
          foreach ($values as $rank => $value) {
            $j++;
            $type_id = $this->featureprop_cvterm_lookup[strtolower($prop_name)];
            $sql .= "(:feature_id_$j, :type_id_$j, :value_$j, :rank_$j),\n";
            $args[":feature_id_$j"] = $feature['feature_id'];
            $args[":type_id_$j"] = $type_id;
            $args[":value_$j"] = $value;
            $args[":rank_$j"] = $rank;
          }
        }
      }
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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
  /**
   *
   */
  private function insertFeatureParents(){
    $batch_size = 100;
    $num_parents = count(array_keys($this->parent_lookup));
    $num_batches = (int) ($num_parents / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Get the 'part_of' cvterm
    $type_id = $this->getCvtermID('part_of');

    $init_sql = "INSERT INTO {feature_relationship} (subject_id, object_id, type_id, rank) VALUES\n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->parent_lookup as $parent => $children) {
      $total++;

      $parent_feature = $this->features[$parent];
      if (!$parent_feature['skipped']) {
        $i++;

        $rank = 0;
        foreach ($children as $feature_id) {
          $j++;
          $sql .= "(:subject_id_$j, :object_id_$j, :type_id_$j, :rank_$j),\n";
          $args[":subject_id_$j"] = $feature_id;
          $args[":object_id_$j"] = $this->features[$parent]['feature_id'];
          $args[":type_id_$j"] = $type_id;
          $args[":rank_$j"] = $rank;
          $rank++;
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_parents) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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

  /**
   *
   */
  private function findDbxrefs() {
    $batch_size = 1000;
    $num_dbxrefs = count(array_keys($this->dbxref_lookup));
    $num_batches = (int) ($num_dbxrefs / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // DBXrefs may be already present so we'll do an initial round of
    // looking for them and then insert those that don't exist.
    $init_sql = "
      SELECT DB.name, DBX.db_id, DBX.accession, DBX.dbxref_id, CVT.cvterm_id
      FROM {dbxref} DBX
        INNER JOIN {db} DB on DB.db_id = DBX.db_id
        LEFT JOIN {cvterm} CVT on DBX.dbxref_id = CVT.dbxref_id
      WHERE
    ";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->dbxref_lookup as $xref => $info) {
      $i++;
      $total++;
      $sql .= "(DBX.accession = :accession_$i and DBX.db_id = :db_id_$i) OR\n";
      $args[":accession_$i"] = $info['accession'];
      $args[":db_id_$i"] = $this->db_lookup[$info['db']];

      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_dbxrefs) {
        $sql = rtrim($sql, " OR\n");
        $sql = $init_sql . $sql;
        $results = chado_query($sql, $args);
        while ($dbxref = $results->fetchObject()) {
          $index = $dbxref->name . ':' . $dbxref->accession;
          $this->dbxref_lookup[$index]['dbxref_id'] = $dbxref->dbxref_id;
          if ($dbxref->cvterm_id) {
            $this->cvterm_lookup[$index] = $dbxref->cvterm_id;
            $this->dbxref_lookup[$index]['cvterm_id'] = $dbxref->cvterm_id;
          }
        }
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

  /**
   * Calculates ranks for all of the children of each feature.
   *
   * This function should not be executed until after features are loaded
   * into the database and we have feature_ids for all of them.
   */
  private function findChildRanks() {
    // Iterate through parent-child relationships and set the ranks.
    foreach ($this->features as $uniquename => $feature) {
      if ($feature['parent']) {
        // place features in order that they appear by their start coordinates.
        $parent = $feature['parent'];
        $start = $feature['start'];
        $this->parent_lookup[$parent][$start] = $feature['feature_id'];
      }
    }
  }

  /**
   *
   */
  private function insertDbxrefs() {
    $batch_size = 1000;
    $num_dbxrefs = count(array_keys($this->dbxref_lookup));
    $num_batches = (int) ($num_dbxrefs / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {dbxref} (db_id, accession) VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->dbxref_lookup as $info) {
      $i++;
      $total++;

      // Only do an insert if this dbxref doesn't already exist in the databse.
      // and this dbxref is from a Dbxref attribute not an Ontology_term attr.
      if (!array_key_exists('dbxref_id', $info) and
          !array_key_exists('cvterm_id', $info)) {
        $sql .= "(:db_id_$i, :accession_$i),\n";
        $args[":db_id_$i"] = $this->db_lookup[$info['db']];
        $args[":accession_$i"] = $info['accession'];
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_dbxrefs) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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

  /**
   *
   */
  private function insertFeatureDbxrefs() {
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Don't need to use placeholders for this insert since we are only using integers.
    $init_sql = "INSERT INTO {feature_dbxref} (feature_id, dbxref_id) VALUES \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {
      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Iterate through all of the dbxrefs of this feature.
        foreach ($feature['dbxrefs'] as $index => $info) {
          $j++;
          $sql .= "(:feature_id_$j, :dbxref_id_$j),\n";
          $args[":feature_id_$j"] = $feature['feature_id'];
          $args[":dbxref_id_$j"] = $this->dbxref_lookup[$index]['dbxref_id'];
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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

  /**
   *
   */
  private function insertFeatureCVterms() {
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Don't need to use placeholders for this insert since we are only using integers.

    $init_sql = "INSERT INTO {feature_cvterm} (feature_id, cvterm_id, pub_id) VALUES \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {
      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Iterate through all of the dbxrefs of this feature.
        foreach ($feature['terms'] as $index => $info) {
          $j++;
          $sql .= "(:feature_id_$j, :cvterm_id_$j, :pub_id_$j),\n";
          $args[":feature_id_$j"] = $feature['feature_id'];
          $args[":cvterm_id_$j"] = $this->cvterm_lookup[$index];
          $args[":pub_id_$j"] = $this->null_pub->pub_id;
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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

  /**
   *
   */
  private function insertFeatureDerivesFrom() {
    $batch_size = 100;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    // Get the 'derives_from' cvterm
    $type_id = $this->getCvtermID('derives_from');

    $init_sql = "INSERT INTO {feature_relationship} (subject_id, object_id, type_id, rank) VALUES\n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {
      $total++;

      // If the feature is not skipped
      if (!$feature['skipped'] and $feature['derives_from']) {
        $i++;
        $object_id = $this->features[$feature['derives_from']]['feature_id'];
        $sql .= "(:subject_id_$i, :object_id_$i, :type_id_$i, 0),\n";
        $args[":subject_id_$i"] = $feature['feature_id'];
        $args[":object_id_$i"] = $object_id;
        $args[":type_id_$i"] = $type_id;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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

  /**
   *
   */
  private function insertFeatureLocs() {
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
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {

      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Get the rank of this feature by iterating through all siblings of the
        // parent and finding where this feature is in terms of start position.
        $rank = 0;
        if (array_key_exists('Parent', $feature)) {
          $children = $this->parent_lookup[$feature['parent']];
          foreach ($children as $sib_start => $feature_id) {
            if ($sib_start == $feature['start']) {
              break;
            }
            $rank++;
          }
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
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }

  /**
   * Finds an organism from an organism attribute value.
   */
  private function findOrganism($organism_attr, $line_num) {

    if (array_key_exists($organism_attr, $this->organism_lookup)) {
      return $this->organism_lookup[$organism_attr];
    }

    // Get the organism object.
    [$genus, $species] = explode(':', $organism_attr, 2);
    $organism = new ChadoRecord('organism');
    $organism->setValues([
      'genus' => $genus,
      'species' => $species
    ]);
    $num_found = $organism->find();

    if ($num_found == 1){
      $this->organism_lookup[$organism_attr] = $organism->getID();
      return $organism->getID();
    }

    if ($num_found > 1) {
      throw new Exception(t('Multiple organisms were found for the "organism" attribute, %organism, on line %line_num',
          ['%organism' => $organism_attr, '%line_num' => $line_num]));
    }

    if ($this->create_organism) {
      $organism->insert();
      $this->organism_lookup[$organism_attr] = $organism->getID();
      $gff_feature['organism'] = $organism->getID();
      return $organism->getID();
    }
    return NULL;
  }

  /**
   *
   */
  private function findSynonyms() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "SELECT synonym_id, name FROM {synonym} WHERE \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    $batch_synonyms = [];
    foreach ($this->features as $uniquename => $feature) {
      $i++;
      $total++;

      // Get all of the synonyms for this batch.
      foreach ($feature['synonyms'] as $index => $synonym) {
        $batch_synonyms[] = $synonym;
      }

      // If we've reached the size of the batch then let's do the select
      if ($i == $batch_size or $total == $num_features) {

        $batch_synonyms = array_unique($batch_synonyms);
        foreach ($batch_synonyms as $synonym) {
          $j++;
          if (!array_key_exists($synonym, $this->synonym_lookup)) {
            $this->synonym_lookup[$synonym] = NULL;
          }
          if (!$this->synonym_lookup[$synonym]) {
            $sql .= "(type_id = :type_id_$j AND name = :name_$j) OR\n";
            $args[":type_id_$j"] = $this->exact_syn->cvterm_id;
            $args[":name_$j"] = $synonym;
          }
        }
        if (count($args) > 0) {
          $sql = rtrim($sql, " OR\n");
          $sql = $init_sql . $sql;
          $results = chado_query($sql, $args);
          while ($synonym = $results->fetchObject()) {
            $this->synonym_lookup[$synonym->name] = $synonym->synonym_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $j = 0;
        $args = [];
        $batch_synonyms = [];
      }
    }
  }

  /**
   *
   */
  private function insertSynonyms() {
    $batch_size = 1000;
    $num_synonyms = count(array_keys($this->synonym_lookup));
    $num_batches = (int) ($num_synonyms / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {synonym} (type_id, name, synonym_sgml) VALUES\n";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->synonym_lookup as $synonym => $synonym_id) {
      $i++;
      $total++;

      // Only do an insert if this dbxref doesn't already exist in the databse.
      if (!$synonym_id) {
        $sql .= "(:type_id_$i,:name_$i, ''),\n";
        $args[":type_id_$i"] = $this->exact_syn->cvterm_id;
        $args[":name_$i"] = $synonym;
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_synonyms) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;

        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }

    // Now we need to retrieve the synonyms IDs.
    $this->findSynonyms();
  }

  /**
   *
   */
  private function insertFeatureSynonyms(){
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;

    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);

    $init_sql = "INSERT INTO {feature_synonym} (synonym_id, feature_id, pub_id) VALUES \n";
    $i = 0;
    $j = 0;
    $total = 0;
    $batch_num = 1;
    $args = [];
    foreach ($this->features as $feature) {
      $total++;

      // If the feature is not skipped
      if (!$feature['skipped']) {
        $i++;

        // Handle all of the synonyms for this feature.
        foreach (array_unique($feature['synonyms']) as $synonym) {
          $j++;
          $sql .= "(:synonym_id_$j, :feature_id_$j, :pub_id_$j),\n";
          $synonym_id = $this->synonym_lookup[$synonym];
          $args[":synonym_id_$j"] = $this->synonym_lookup[$synonym];
          $args[":feature_id_$j"] = $feature['feature_id'];
          $args[":pub_id_$j"] = $this->null_pub->pub_id;
        }
      }

      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
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
        $uniquename = $attrs['Parent'][0] . "-" . $type . "-" .
          $landmark_name . ":" . ($fmin + 1) . ".." . $fmax;
        $name = $attrs['Parent'][0] . "-" . $type;
      }

      // Generate a unique name based on the type and location
      // and set the name to simply be the type.
      else {
        $uniquename = $type . "-" . $landmark_name . ":" . ($fmin + 1) . ".." . $fmax;
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
  private function loadDerivesFromOld($feature, $cvterm, $object,
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
  private function insertFeature($organism, $analysis, $cvterm, $uniquename,
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
